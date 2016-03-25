
// C++ : Table d'hexaedres (Evol Versions 3)

// Copyright (C) 2009-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "HexQpattern.hxx"

#include "HexVector.hxx"
#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexDiagnostics.hxx"
#include "HexDocument.hxx"
#include "HexHexa.hxx"
#include "HexGlobale.hxx"

#include <cmath>
#include <map>

BEGIN_NAMESPACE_HEXA

static bool db = false;

// =================================================== Construc QpatQuad
QpatQuad::QpatQuad (Quad* elt)
{
   refer = elt;
   cible = oppos = NULL;
   for (int nro=0 ; nro<QUAD4 ; ++nro)
       q_edge [nro] = NOTHING;
}
// =================================================== operator=
void QpatQuad::operator= (const QpatQuad& other)
{
   refer = other.refer;
   cible = other.cible;
   oppos = other.oppos;
   for (int nro=0;nro<QUAD4;++nro) q_edge[nro] = other.q_edge[nro];
}
// =================================================== Construc QpatEdge
QpatEdge::QpatEdge (Edge* elt)
{
   refer   = elt;
   cible   = oppos = NULL;
   v_amont = v_aval = NOTHING;
}
// =================================================== operator=
void QpatEdge::operator= (const QpatEdge& other)
{
   refer   = other.refer;
   cible   = other.cible;
   oppos   = other.oppos;
   v_amont = other.v_amont;
   v_aval  = other.v_aval;
}
// =================================================== Construc QpatVertex
QpatVertex::QpatVertex (Vertex* elt)
{
   refer = elt;
   cible = oppos = NULL;
   v_x   = v_y   = 0.0;
   on_contour    = true;
   quad_cible    = NOTHING;
}
// =================================================== operator=
void QpatVertex::operator= (const QpatVertex& other)
{
   refer  = other.refer;
   cible  = other.cible;
   oppos  = other.oppos;
   v_x    = other.v_x;
   v_y    = other.v_y;
   on_contour = other.on_contour;
   quad_cible = other.quad_cible;
}
// =================================================== find_quad
int find_quad (Quads& table, Vertex* vertex)
{
   int nbre = table.size();
   for (int nro=0 ; nro < nbre ; nro++)
       if (table[nro]->indexVertex(vertex)>=0)
          return nro;

   return NOTHING;
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// ===================================================== Constructeur
Qpattern::Qpattern ()
{
   nbr_vertex   = nbr_edges = nbr_quads = 0;
   cont_nbnodes = 0;
   pos_vertex4 = pos_edge3 = pos_edge4 = NOTHING;
   for (int nc=0 ; nc<DIM3 ; nc++)
       origine[nc] = base_i[nc]  = base_j[nc] = 0;

   base_i[0]  = 1; 
   base_j[1]  = 1;
   nbr_layers = 0;
   ux_mini  = uy_mini  = 0;
   ux_delta = uy_delta = 1;
}
// ====================================================== setPattern
void Qpattern::setPattern (Vertices& contour, Quads& motif)
{
    cont_nbnodes = contour.size();
    Real3 cg, point, pmax;
    Vertex::anaVertices (contour, point, pmax, cg);
    for (int nro=0 ; nro<cont_nbnodes ; ++nro)
        addVertex (contour[nro]);

    contour[0]->getPoint (origine);
    contour[1]->getPoint (point);
    setOrigin (origine, point, cg);

   double vxmin = pat_vertex[0].v_x;
   double vymin = pat_vertex[0].v_y;
   double vxmax = vxmin;
   double vymax = vymin;
   for (int np=0 ; np<cont_nbnodes ; ++np)
       {
       double ux, uy;
       projeter (contour[np], ux, uy);
       vxmin = std::min (ux, vxmin);
       vxmax = std::max (ux, vxmax);
       vymin = std::min (uy, vymin);
       vymax = std::max (uy, vymax);
       }

    ux_mini  = vxmin;
    uy_mini  = vymin;
    ux_delta = vxmax-vxmin;
    uy_delta = vymax-vymin;

    int nbre = motif.size();
    for (int nro=0 ; nro<nbre ; ++nro)
        addQuad (motif[nro]);

    for (int nro=0 ; nro<cont_nbnodes ; ++nro)
        {
        contour[nro]->getPoint (point);
        for (int nc=0 ; nc<DIM3 ; nc++) cg[nc] += point[nc];
        addVertex (contour[nro]);
        }
}
// ====================================================== setTarget
void Qpattern::setTarget (Vertices& contour, Quads& cible)
{
   old_contour = contour;

   for (int nro=0 ; nro<cont_nbnodes ; ++nro)
       {
       Vertex* vertex = contour[nro];
       pat_vertex[nro].cible = vertex;
       pat_vertex[nro].quad_cible = find_quad (cible, vertex);
       }

   int nbre = cible.size();
   nbr_layers = 999999;
   for (int nro=0 ; nro<nbre ; ++nro)
       {
       Quad* quad  = cible [nro];
       Hexa* hexa  = quad->getParent (0); 
       Quad* oppos = hexa->getOpposedQuad (quad);

       old_top   .push_back (quad);
       old_bottom.push_back (oppos);
       old_hexa  .push_back (hexa);

       int prof = 0;
       while (oppos!=NULL)
          {
          prof ++;
          hexa  = oppos->opposedHexa (hexa);
          oppos = hexa==NULL ? NULL : hexa->getOpposedQuad (oppos);
          }
       nbr_layers = std::min (prof, nbr_layers);
       }
}
// ====================================================== setOrigin
int Qpattern::setOrigin (double* orig, double* p1, double* cg)
{
   base_i[1]   = base_i[2] = 0; 
   base_j[0]   = base_j[2] = 0; 
   base_i[0]   = base_j[1] = 1;

   pat_vertex[0].v_x = pat_vertex[1].v_x = 0;
   pat_vertex[2].v_x = 1; 
   pat_vertex[0].v_y = 1;
   pat_vertex[1].v_y = pat_vertex[2].v_y = 0; 

   calc_vecteur (orig, p1, base_i);
   calc_vecteur (orig, cg, base_j);
   normer_vecteur (base_i);
   normer_vecteur (base_j);
   Real3 vk;
   prod_vectoriel (base_i, base_j, vk);
   prod_vectoriel (vk, base_i, base_j);

   if (db)
      {
      PutCoord (orig);
      PutCoord (base_i);
      PutCoord (base_j);
      }

   return HOK;
}
// ====================================================== addQuad
int Qpattern::addQuad (Quad* elt)
{
   if (elt==NULL)
      return HERR;

   QpatQuad quad (elt);
   for (int nro=0; nro<QUAD4 ; nro++)
       {
       Edge* edge = elt->getEdge (nro);
       quad.q_edge [nro] = addEdge (edge); 
       }

   pat_quad.push_back (quad);
   nbr_quads++;
   return HOK;
}
// ====================================================== addEdge
int Qpattern::addEdge (Edge* elt)
{
   for (int nro=0; nro<nbr_edges ; nro++)
       {
       if (elt==pat_edge [nro].refer)
          return nro;
       }

   QpatEdge edge (elt);
   edge.v_amont  = addVertex (elt->getVertex (V_AMONT));
   edge.v_aval   = addVertex (elt->getVertex (V_AVAL));

   pat_edge.push_back (edge);
   nbr_edges++;
   return nbr_edges-1;
}

// ====================================================== projeter
void Qpattern::projeter (Vertex* elt, double& ux, double& uy)
{
   Real3 point, local;
   elt->getPoint (point);
   calc_vecteur  (origine, point, local); 
   ux = (prod_scalaire (base_i, local) - ux_mini)/ux_delta ;
   uy = (prod_scalaire (base_j, local) - uy_mini)/uy_delta ;
}
// ====================================================== addVertex
int Qpattern::addVertex (Vertex* elt, Vertex* ref)
{
   for (int nro=0; nro<nbr_vertex ; nro++)
       {
       if (elt==pat_vertex [nro].refer)
          {
          if (ref != NULL) 
              pat_vertex [nro].on_contour = true;
          return nro;
          }
       }

   QpatVertex vertex (elt);
   projeter (elt, vertex.v_x, vertex.v_y);

   if (db)
      printf (" Vertex nro %d : (%g, %g, %g)\t -> (%g, %g)\n", 
                nbr_vertex, elt->getX(),  elt->getY(),  elt->getZ(), 
                vertex.v_x, vertex.v_y);
   pat_vertex.push_back (vertex);
   nbr_vertex++;
   return nbr_vertex-1;
}
// ====================================================== getSize
void Qpattern::getSize (int& nbq, int& nbed, int& nbv, int& prof)
{
   nbv  = nbr_vertex; 
   nbed = nbr_edges; 
   nbq  = nbr_quads; 
   prof = nbr_layers; 
}
// ====================================================== stepDown
void Qpattern::stepDown ()
{
   int nbnodes = old_contour.size();
   for (int nro=0 ; nro<nbnodes ; ++nro)
       {
       int nq = pat_vertex[nro].quad_cible;

       Hexa*   hexa     = old_hexa [nq];
       Quad*   quad     = old_top  [nq];
       Vertex* vertex   = hexa->opposedVertex (quad, old_contour[nro]);
       old_contour[nro] = vertex;
       }

   int nbquads = old_top.size();
   for (int nq=0 ; nq<nbquads ; ++nq)
       {
       Quad* oppos = old_bottom[nq];
       Hexa* hexa  = oppos->opposedHexa (old_hexa[nq]);

       old_top    [nq] -> remove ();
       old_top    [nq] = oppos;
       old_bottom [nq] = hexa==NULL ? NULL : hexa->getOpposedQuad (oppos);
       old_hexa   [nq] = hexa;
       }
}
// ====================================================== anaTarget
void Qpattern::anaTarget  (double tmin[], double tmax[], double centre[])
{
   Vertex::anaVertices (old_contour, tmin, tmax, centre);
}
END_NAMESPACE_HEXA
