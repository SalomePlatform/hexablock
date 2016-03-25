
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

#include "HexElements.hxx"

#include "HexVector.hxx"
#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexDiagnostics.hxx"
#include "HexDocument.hxx"
#include "HexHexa.hxx"
#include "HexGlobale.hxx"
#include "HexQpattern.hxx"

#include <cmath>
#include <map>

BEGIN_NAMESPACE_HEXA

static bool db = false;

// ====================================================== opposed_edge
Edge* opposed_edge (Edge* edge, Hexas& thexas, Quads& tquads)
{
   int nbquads = tquads.size();
   for (int nq = 0 ; nq<nbquads ; ++nq)
       {
       Quad* quad = tquads[nq];
       int nro = quad->indexEdge (edge);
       if (nro>=0)
          {
          Quad*  perp  = thexas[nro]->perpendicularQuad (quad, edge);
          Edge*  oppos = perp->getOpposEdge (edge);
          return oppos;
          }
       }
   return NULL;
}
// ====================================================== replaceHexas 
int Elements::replaceHexas (Quads& motif, Quads& cible, Vertex* p1, 
                            Vertex* c1, Vertex* p2, Vertex* c2)
{
    Vertices ed_motif, ed_cible;
    checkContour (motif, p1, p2, false, ed_motif);
    checkContour (cible, c1, c2, true , ed_cible);
    if (el_status != HOK)
        return el_status;

    int nbedp   = ed_motif.size();
    int nbedges = ed_cible.size();
    if (nbedp != nbedges)
       {
       setError (HERR);
       Mess << " Contour of pattern ("  << nbedp << " edges) and target ("
            << nbedges << " edges) must have the same size ";
       return el_status;
       }
                        // Constitution du pattern 
    Qpattern pattern;
    pattern.setPattern (ed_motif, motif);
    pattern.setTarget  (ed_cible, cible);
    int nbq, nbed, nbv, prof;
    pattern.getSize (nbq, nbed, nbv, prof);

    resize (GR_REPLACE, nbq, prof+1, nbv, nbed);

    replaceQuad (1, &pattern);
    for (int nh=1 ; nh<=prof ; ++nh)
        {
        pattern.stepDown();
        replaceQuad (nh+1, &pattern);
        replaceHexa (nh,   &pattern);
        }

    extrudeQuad (&pattern);
    replaceHexa (0, &pattern);
    return HOK;
}
// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------
// ====================================================== repVertex 
void Elements::repVertex (int nh, int nro, Vertex* elt)
{
   int addr = nh * pat_nbvertex + nro;
   if (tab_vertex[addr] == NULL)
       tab_vertex[addr]  = elt;
}
// ====================================================== repVertex 
Vertex* Elements::repVertex (int nh, int nro, double px, double py, double pz)
{
   int addr = nh * pat_nbvertex + nro;

   if (tab_vertex[addr] == NULL)
       tab_vertex[addr] = el_root->addVertex (px, py, pz);

   return tab_vertex[addr];
}
// ====================================================== repVertex 
Vertex* Elements::repVertex (int nh, int nro)
{
   int addr = nh * pat_nbvertex + nro;
   return tab_vertex[addr];
}
// ====================================================== repEdgeH 
void Elements::repEdgeH (int nh, int nro, Edge* elt)
{
   int addr = nh * (pat_nbedges + pat_nbvertex) + nro;
   if (tab_edge[addr] == NULL)
       tab_edge[addr]  = elt;
}
// ====================================================== repEdgeH 
Edge* Elements::repEdgeH (int nh, int nro, Vertex* v1, Vertex* v2)
{
   int addr = nh * (pat_nbedges + pat_nbvertex) + nro;
   if (tab_edge[addr] == NULL)
       tab_edge[addr]  = el_root->addEdge (v1, v2);
   return tab_edge[addr];
}
// ====================================================== repEdgeH 
Edge* Elements::repEdgeH (int nh, int nro)
{
   int addr = nh * (pat_nbedges + pat_nbvertex) + nro;
   return tab_edge[addr];
}
// ====================================================== repEdgeV 
void Elements::repEdgeV (int nh, int nro, Edge* elt)
{
   int addr = nh * (pat_nbedges + pat_nbvertex) + pat_nbedges + nro;
   if (tab_edge[addr] == NULL)
       tab_edge[addr]  = elt;
}
// ====================================================== repEdgeV 
Edge* Elements::repEdgeV (int nh, int nro, Vertex* v1, Vertex* v2)
{
   int addr = nh * (pat_nbedges + pat_nbvertex) + pat_nbedges + nro;
   if (tab_edge[addr] == NULL)
       tab_edge[addr]  = el_root->addEdge (v1, v2);
   return tab_edge[addr];
}
// ====================================================== repEdgeV 
Edge* Elements::repEdgeV (int nh, int nro)
{
   int addr = nh * (pat_nbedges + pat_nbvertex) + pat_nbedges + nro;
   return tab_edge[addr];
}
// ====================================================== repQuadH 
void Elements::repQuadH (int nh, int nro, Quad* elt)
{
   int addr = nh * (nbr_orig + pat_nbedges) + nro;
   if (tab_quad[addr] == NULL)
       tab_quad[addr]  = elt;
}
// ====================================================== repQuadH 
Quad* Elements::repQuadH (int nh, int nro)
{
   int addr = nh * (nbr_orig + pat_nbedges) + nro;
   return tab_quad [addr];
}
// ====================================================== repQuadH 
Quad* Elements::repQuadH (int nh, int nro, Edge* e1, Edge* e2, Edge* e3, 
                                           Edge* e4)
{
   int addr = nh * (nbr_orig + pat_nbedges) + nro;
   if (tab_quad[addr] == NULL)
       tab_quad[addr]  = el_root->addQuad (e1, e2, e3, e4);
   return tab_quad [addr];
}
// ====================================================== repQuadV 
void Elements::repQuadV (int nh, int nro, Quad* elt)
{
   int addr = nh * (nbr_orig + pat_nbedges) + nbr_orig + nro;
   if (tab_quad[addr] == NULL)
       tab_quad[addr]  = elt;
}
// ====================================================== repQuadV 
Quad* Elements::repQuadV (int nh, int nro)
{
   int addr = nh * (nbr_orig + pat_nbedges) + nbr_orig + nro;
   return tab_quad [addr];
}
// ====================================================== repQuadV 
Quad* Elements::repQuadV (int nh, int nro, Edge* e1, Edge* e2, Edge* e3, 
                                           Edge* e4)
{
   int addr = nh * (nbr_orig + pat_nbedges) + nbr_orig + nro;
   if (tab_quad[addr] == NULL)
       tab_quad[addr]  = el_root->addQuad (e1, e2, e3, e4);
   return tab_quad [addr];
}
// ====================================================== repHexa 
Hexa* Elements::repHexa (int nh, int nro, Quad* qa, Quad* qb, Quad* qc, 
                                          Quad* qd, Quad* qe, Quad* qf)
{
   int addr = nh * nbr_orig  + nro;
   if (tab_hexa[addr] == NULL)
       tab_hexa[addr]  = el_root->addHexa (qa, qb, qc, qd, qe, qf);

   return tab_hexa [addr];
}
// ====================================================== replaceQuad 
// ==== Creation des quads horizontaux
int Elements::replaceQuad (int nh, Qpattern* pat)
{
                              // Enregistrement des vertex existant
   int size_contour = pat->cont_nbnodes;
   for (int nro=0 ; nro<size_contour ; nro++)
       {
       Vertex* vh = pat->getTargetVertex (nro);
       repVertex (nh,  nro, vh);
       }

   Real3 orig, pmin, pmax, cg, delta, ib, jb, kb;
   pat->anaTarget (pmin, pmax, cg);
   for (int nc=0 ; nc<DIM3 ; nc++)
       delta[nc] = pmax[nc] - pmin[nc];

                              // Creation des vertex
   pat->getTargetVertex (0)->getPoint (orig);
   pat->getTargetVertex (1)->getPoint (kb);
   calc_vecteur (orig, kb, ib);
   calc_vecteur (orig, cg, jb);
   normer_vecteur (ib);
   normer_vecteur (jb);
   prod_vectoriel (ib, jb, kb);
   prod_vectoriel (kb, ib, jb);

   for (int nro=0 ; nro<pat->nbr_vertex ; nro++)
       {
       double lambda = pat->pat_vertex [nro].v_x;
       double mu     = pat->pat_vertex [nro].v_y;
       double px = pmin[dir_x] + (lambda*ib[dir_x] + mu*jb[dir_x])*delta[dir_x];
       double py = pmin[dir_y] + (lambda*ib[dir_y] + mu*jb[dir_y])*delta[dir_y];
       double pz = pmin[dir_z] + (lambda*ib[dir_z] + mu*jb[dir_z])*delta[dir_z];
       repVertex (nh, nro, px, py, pz);
       }
                              // Creation des edges horizontaux
   for (int nro=0 ; nro<pat->nbr_edges ; nro++)
       {
       int nv1 = pat->pat_edge [nro].v_amont;
       int nv2 = pat->pat_edge [nro].v_aval;
       Vertex* v1 = repVertex (nh, nv1);
       Vertex* v2 = repVertex (nh, nv2);
       repEdgeH (nh, nro, v1, v2);
       }
                              // Creation des quads horizontaux
   for (int nro=0 ; nro<pat->nbr_quads ; nro++)
       {
       Edge* eda = repEdgeH (nh, pat->pat_quad [nro].q_edge [0]);
       Edge* edb = repEdgeH (nh, pat->pat_quad [nro].q_edge [1]);
       Edge* edc = repEdgeH (nh, pat->pat_quad [nro].q_edge [2]);
       Edge* edd = repEdgeH (nh, pat->pat_quad [nro].q_edge [3]);

       repQuadH (nh,  nro, eda, edb, edc, edd);
       }
   return HOK;
}
// ====================================================== extrudeQuad 
// ==== Creation des quads horizontaux
int Elements::extrudeQuad (Qpattern* pat)
{
                              // Creation des vertex de niveau 0
   for (int nro=0 ; nro<pat->nbr_vertex ; nro++)
       {
       Vertex* v1 = repVertex (1, nro);
       Vertex* v2 = repVertex (2, nro);
       double px = 2*v1->getX() - v2->getX();
       double py = 2*v1->getY() - v2->getY();
       double pz = 2*v1->getZ() - v2->getZ();
       repVertex (0, nro, px, py, pz);
       }
                              // Creation des edges horizontaux
   for (int nro=0 ; nro<pat->nbr_edges ; nro++)
       {
       int nv1 = pat->pat_edge [nro].v_amont;
       int nv2 = pat->pat_edge [nro].v_aval;
       Vertex* v1 = repVertex (0, nv1);
       Vertex* v2 = repVertex (0, nv2);
       repEdgeH (0, nro, v1, v2);
       }
                              // Creation des quads horizontaux
   for (int nro=0 ; nro<pat->nbr_quads ; nro++)
       {
       Edge* eda = repEdgeH (0, pat->pat_quad [nro].q_edge [0]);
       Edge* edb = repEdgeH (0, pat->pat_quad [nro].q_edge [1]);
       Edge* edc = repEdgeH (0, pat->pat_quad [nro].q_edge [2]);
       Edge* edd = repEdgeH (0, pat->pat_quad [nro].q_edge [3]);

       repQuadH (0,  nro, eda, edb, edc, edd);
       }
   return HOK;
}
// ====================================================== replaceHexa 
int Elements::replaceHexa (int nh, Qpattern* pat)
{
 /* *************************************************
   int vnro [4] = { 0, 1, 2, pat->pos_vertex4};
   int ednro[4] = { 0, 1, pat->pos_edge3, pat->pos_edge4 };

                              // Enregistrement des edges & quads existants
   if (nh!=0)
       {
       for (int nro=0 ; nro<QUAD4 ; nro++)
           {
           Vertex* vh  = repVertex (nh,   vnro [nro]);
           Vertex* vh1 = repVertex (nh+1, vnro [nro]);
           Edge*   edv = hexa->findEdge (vh, vh1);
           repEdgeV (nh, vnro [nro], edv);

           Edge*   edh   = repEdgeH (nh,   ednro [nro]);
           Edge*   edh1  = repEdgeH (nh+1, ednro [nro]);
           Quad*   quadv = hexa->findQuad (edh, edh1);
           repQuadV (nh, ednro [nro], quadv);
           }
       }
    ************************************************* */
                              // Creation des edges verticaux
   for (int nro=0 ; nro<pat->nbr_vertex ; nro++)
       {
       Vertex* v1 = repVertex (nh,   nro);
       Vertex* v2 = repVertex (nh+1, nro);
       repEdgeV (nh, nro, v1, v2); 
       }
                              // Creation des quads verticaux
   for (int nro=0 ; nro<pat->nbr_edges ; nro++)
       {
       int nv1 = pat->pat_edge [nro].v_amont;
       int nv2 = pat->pat_edge [nro].v_aval;

       Edge* eda = repEdgeH (nh,   nro);
       Edge* edb = repEdgeV (nh,   nv1);
       Edge* edc = repEdgeH (nh+1, nro);
       Edge* edd = repEdgeV (nh,   nv2);
       repQuadV (nh,  nro, eda, edb, edc, edd);
       }
                              // Creation des hexaedres
   for (int nro=0 ; nro<pat->nbr_quads ; nro++)
       {
       Quad* qa = repQuadH (nh,   nro);
       Quad* qb = repQuadH (nh+1, nro);
       Quad* qc = repQuadV (nh, pat->pat_quad [nro].q_edge [0]);
       Quad* qd = repQuadV (nh, pat->pat_quad [nro].q_edge [2]);
       Quad* qe = repQuadV (nh, pat->pat_quad [nro].q_edge [1]);
       Quad* qf = repQuadV (nh, pat->pat_quad [nro].q_edge [3]);
       repHexa (nh, nro, qa, qb, qc, qd, qe, qf);
       }

   return HOK;
}
END_NAMESPACE_HEXA
