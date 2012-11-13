
// C++ : Table d'hexaedres (Evol Versions 3)

// Copyright (C) 2009-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include <cmath>
#include <map>

BEGIN_NAMESPACE_HEXA

static bool db = false;

// --------------------------------------------------------
struct PatQuad
{
   int   q_edge [QUAD4];
   Quad* refer;
}; 
// --------------------------------------------------------
struct PatVertex
{
   double  v_x, v_y;
   Vertex* refer;
}; 
// --------------------------------------------------------
struct PatEdge
{
   int   v_amont, v_aval;
   Edge* refer;
   int   nbr_refer;
}; 

// --------------------------------------------------------
class Pattern 
{
friend class Elements;
public  :
    int initialize (Vertex* v1, Vertex* v2, Vertex* v3);
    int addQuad    (Quad* quad);
    int verify     (int &nbed, int &nbver);

private :
    int addEdge   (Edge*   edge);
    int addVertex (Vertex* vertex);

private :
   enum EnumProj { ProjXY, ProjYZ, ProjZX };

   vector <PatVertex> pat_vertex;
   vector <PatEdge  > pat_edge;
   vector <PatQuad  > pat_quad;

   int      nbr_vertex, nbr_edges, nbr_quads;
   double   determinant;
   EnumProj projection;
   Real3    base_i, base_j, origine;

   int pos_vertex4;
   int pos_edge3, pos_edge4;
};

// ====================================================== initialize
int Pattern::initialize (Vertex* v1, Vertex* v2, Vertex* v3)
{
   nbr_vertex  = nbr_edges = nbr_quads = 0;
   projection  = ProjXY;
   determinant = 1;
   base_i[0]   = base_i[1] =  base_i[2] = 0; 
   base_j[0]   = base_j[1] =  base_j[2] = 0; 

   if (v1==NULL || v2==NULL || v3==NULL) 
      return HERR;

   addVertex (v1);
   addVertex (v2);
   addVertex (v3);
   if (nbr_vertex!=3)
      return HERR;

   Document* doc = v1->dad ();
   Edge* edc1  = doc->findEdge (v1, v2);
   Edge* edc2  = doc->findEdge (v2, v3);
   if (edc1==NULL || edc2==NULL)
      return HERR;

   addEdge (edc1);
   addEdge (edc2);
   if (nbr_edges!=2)
      return HERR;

   pat_edge[0].nbr_refer = pat_edge[1].nbr_refer = 0; 

   pat_vertex[0].v_x = pat_vertex[1].v_x = 0;
   pat_vertex[2].v_x = 1; 
   pat_vertex[0].v_y = 1;
   pat_vertex[1].v_y = pat_vertex[2].v_y = 0; 

   Real3 pb, pc;
   v1->getPoint (pc);
   v2->getPoint (origine);
   v3->getPoint (pb);

   calc_vecteur (origine, pb, base_i);
   calc_vecteur (origine, pc, base_j);

   if (db)
      {
      PutCoord (origine);
      PutCoord (base_i);
      PutCoord (base_j);
      PutData (determinant);
      }

/* ******************************
 * AB = pu.vI + pv.vJ
 * 
 * vx = pu.vI[0] + pv.vJ[0]   (1) *+vJ[1] ) };
 *
 * (1 & 2 ) : pu =  (vx.vJ[1] - vy.vJ[0]) / detxy
 *            pv = -(vx.vI[1] - vy.vI[0]) / detxy
 *x
 * (2 & 3 ) : pu =  (vy.vJ[2] - vz.vJ[1]) / detyz
 *            pv = -(vy.vI[2] - vz.vI[1]) / detyz
 *
 * (3 & 1 ) : pu =  (vz.vJ[0] - vx.vJ[2]) / detzx
 *            pv = -(vz.vI[0] - vx.vI[2]) / detzx
 *
 *  Les 3 systemes d'equations sont valides. 
 *  On va choisir celui dont la valeur absolue du determinant est maximale
   ****************************** */

   double detxy = base_i[dir_x]*base_j[dir_y] - base_i[dir_y]*base_j[dir_x];
   double detyz = base_i[dir_y]*base_j[dir_z] - base_i[dir_z]*base_j[dir_y];
   double detzx = base_i[dir_z]*base_j[dir_x] - base_i[dir_x]*base_j[dir_z];

   determinant = detxy;
   projection  = ProjXY;

   if (fabs (detyz) > fabs (determinant))
      {
      determinant = detyz;
      projection  = ProjYZ;
      }
   if (fabs (detzx) > fabs (determinant))
      {
      determinant = detzx;
      projection  = ProjZX;
      }

   return HOK;
}
// ====================================================== verify
int Pattern::verify (int &nbed, int &nbver)
{
   nbed  = nbr_edges;
   nbver = nbr_vertex;
   pos_edge3 = pos_edge4 = pos_vertex4 = NOTHING;
   
   if (pat_edge[0].nbr_refer!=1 || pat_edge[1].nbr_refer!=1) 
      return HERR;

   for (int nro=2 ; nro<nbr_edges; nro++)
       {
       if (pat_edge[nro].nbr_refer==1)
          {
          int pv1 = pat_edge[nro].v_amont;
          int pv2 = pat_edge[nro].v_aval;
          
          if (pv1==2)
             {
             pos_edge3   = nro;
             if (pos_vertex4 == NOTHING)
                 pos_vertex4 =  pv2;
             else if (pos_vertex4 != pv2)
                 {
                 return HERR;
                 }
             }
          else if (pv2==2)
             {
             pos_edge3   = nro;
             if (pos_vertex4 == NOTHING)
                 pos_vertex4 =  pv1;
             else if (pos_vertex4 != pv1)
                 {
                 return HERR;
                 }
             }
          else if (pv1==0)
             {
             pos_edge4   = nro;
             if (pos_vertex4 == NOTHING)
                 pos_vertex4 =  pv2;
             else if (pos_vertex4 != pv2)
                 {
                 return HERR;
                 }
             }
          else if (pv2==0)
             {
             pos_edge4   = nro;
             if (pos_vertex4 == NOTHING)
                 pos_vertex4 =  pv1;
             else if (pos_vertex4 != pv1)
                 {
                 return HERR;
                 }
             }
          }
       }

   if (pos_edge3==NOTHING || pos_edge4==NOTHING)
      return HERR;

   return HOK;
}
// ====================================================== addQuad
int Pattern::addQuad (Quad* elt)
{
   if (elt==NULL)
      return HERR;

   PatQuad quad;
   quad.refer = elt;
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
int Pattern::addEdge (Edge* elt)
{
   for (int nro=0; nro<nbr_edges ; nro++)
       {
       if (elt==pat_edge [nro].refer)
          {
          pat_edge [nro].nbr_refer++;
          return nro;
          }
       }

   PatEdge edge;
   edge.nbr_refer = 1;
   edge.refer    = elt;
   edge.v_amont  = addVertex (elt->getVertex (V_AMONT));
   edge.v_aval   = addVertex (elt->getVertex (V_AVAL));

   pat_edge.push_back (edge);
   nbr_edges++;
   return nbr_edges-1;
}
// ====================================================== addVertex
int Pattern::addVertex (Vertex* elt)
{
   for (int nro=0; nro<nbr_vertex ; nro++)
       {
       if (elt==pat_vertex [nro].refer)
          return nro;
       }

   PatVertex vertex;
   vertex.refer = elt;
   double vx = elt->getX() - origine [dir_x];
   double vy = elt->getY() - origine [dir_y];
   double vz = elt->getZ() - origine [dir_z];
   switch (projection)
      {
      case ProjXY :  default :
           vertex.v_x =  (vx*base_j[dir_y] - vy*base_j[dir_x]) / determinant;
           vertex.v_y = -(vx*base_i[dir_y] - vy*base_i[dir_x]) / determinant;
           break;

      case ProjYZ : 
           vertex.v_y =  (vy*base_j[dir_z] - vz*base_j[dir_y]) / determinant;
           vertex.v_x = -(vy*base_i[dir_z] - vz*base_i[dir_y]) / determinant;
           break;

      case ProjZX : 
           vertex.v_x =  (vz*base_j[dir_x] - vx*base_j[dir_z]) / determinant;
           vertex.v_y = -(vz*base_i[dir_x] - vx*base_i[dir_z]) / determinant;
           break;
      }

   if (db)
      printf (" Vertex nro %d : (%g, %g, %g)\t -> (%g, %g)\n", 
                        nbr_vertex, vx, vy, vz, vertex.v_x, vertex.v_y);
   pat_vertex.push_back (vertex);
   nbr_vertex++;
   return nbr_vertex-1;
}
// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------
// ====================================================== replaceHexas 
int Elements::replaceHexas (Quads& liste, Vertex* p1, Vertex* c1, 
                            Vertex* p2, Vertex* c2, Vertex* p3, Vertex* c3)
{
    Edge* edp1  = el_root->findEdge (p1, p2);
    Edge* edp2  = el_root->findEdge (p2, p3);
    Edge* edc1  = el_root->findEdge (c1, c2);
    Edge* edc2  = el_root->findEdge (c2, c3);

    Quad* quadc = el_root->findQuad (c1, c3);

    if (edp1==NULL || edp2==NULL || edc1==NULL || edc2==NULL || quadc==NULL)  
       {
       printf ("... Error in HexaBlock function \n");
       printf ("... doc.replace (lquads, p1,c1, p2,c2, p3,c3)\n");
       if (edp1==NULL)
           printf ("Vertices p1 and p2 don't define an edge\n");
       else if (edp2==NULL)
           printf ("Vertices p2 and p3 don't define an edge\n");
       else if (edc1==NULL)
           printf ("Vertices c1 and c2 don't define an edge\n");
       else if (edc2==NULL)
           printf ("Vertices c2 and c3 don't define an edge\n");
       else if (quadc==NULL)
           printf ("Vertices c1 and c3 don't define a quad\n");
       return HERR;
       }

    int np = quadc->getNbrParents ();
    Hexa* hexac = quadc->getParent (0);

    if (np!=1 || hexac==NULL)  
       {
       printf ("... Error in HexaBlock function \n");
       printf ("... doc.replace (lquads, p1,c1, p2,c2, p3,c3)\n");
       printf ("Quad (c1,c2,c3) is not an external quad\n");
       return HERR;
       }
                              // Analyse du pattern
    int nbquads = liste.size();
    Pattern   pattern;
    int ier = pattern.initialize (p1, p2, p3);
    if (ier!=HOK)
       {
       printf ("... Error in HexaBlock function \n");
       printf ("... doc.replace (lquads, p1,c1, p2,c2, p3,c3)\n");
       printf ("Vertices (p1,p2,p3) don't define a virtual quad\n");
       return HERR;
       }

    for (int nq=0 ; nq<nbquads ; nq++)
        {
        ier = pattern.addQuad (liste[nq]);
        if (ier!=HOK)
           {
           printf ("... Error in HexaBlock function \n");
           printf ("... doc.replace (lquads, p1,c1, p2,c2, p3,c3)\n");
           printf ("Quad nr %d of the list is NULL\n", nq+1);
           return HERR;
           }
        }
    int nbv  = 0;
    int nbed = 0;
    int nbh  = 0;
    ier = pattern.verify (nbed, nbv);
    if (ier!=HOK)
       {
       printf ("... Error in HexaBlock function \n");
       printf ("... doc.replace (lquads, p1,c1, p2,c2, p3,c3)\n");
       printf ("Vertices (p1,p2,p3) don't define a virtual quad\n");
       return HERR;
       }
                              // Analyse de la cible
    Quads pil_quad;
    Hexas pil_hexa;
 
    pil_hexa.push_back (hexac);
    pil_quad.push_back (quadc);
    bool more = true;
                              // Constitution de la pile des hexaedres
    while (more)
          {
          nbh ++;
          Quad* oppos = hexac->getOpposedQuad (quadc);
          pil_quad.push_back (oppos);
          more = oppos->getNbrParents() == 2;

          if (more)
             {
             if (oppos->getParent (0)==hexac)
                 hexac = oppos->getParent(1);
             else
                 hexac = oppos->getParent(0);
             pil_hexa.push_back (hexac);
             }
          quadc = oppos;
          }

    resize (GR_REPLACE, nbquads, nbh, nbv, nbed);

                // 1) Constitution des 4 coins de la cible
                // 2) Quadriller la face du haut
                // 3) Trouver les 4 coins de la face du bas
                // 4) Quadriller la face du bas
                // 6) Decouper l'hexaedre defini par ces 2 faces
                // 7) S'il y a un hexaedre dessous goto 3) 

    Vertex* tvert[QUAD4] = { c1, c2, c3, pil_quad[0]->getOpposVertex(c2) };
    replaceQuad (1, &pattern, pil_quad[0], tvert);
    for (int nh=1 ; nh<gr_hauteur ; nh++)
        {
        for (int nro=0 ; nro<QUAD4 ; nro++)
            {
            Edge*  edv  = pil_hexa[nh-1]->getPerpendicularEdge (pil_quad[nh-1],
                                                                tvert[nro]);
            tvert [nro] = edv ->opposedVertex(tvert[nro]);
            }
        replaceQuad (nh+1, &pattern, pil_quad[nh], tvert);
        replaceHexa (nh,   &pattern, pil_hexa[nh-1]);
        }

    for (int nh=0 ; nh<=nbh ; nh++)
        pil_quad[nh]->remove ();

    extrudeQuad (&pattern);
    replaceHexa (0, &pattern, NULL);
    return HOK;
}
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
int Elements::replaceQuad (int nh, Pattern* pat, Quad* quad, Vertex* tvert[])
{
   Vertex* pvert[QUAD4] = { tvert[0],  tvert[1],  tvert[2],  tvert[3] };

   int vnro [4] = { 0, 1, 2, pat->pos_vertex4};
   int ednro[4] = { 0, 1, pat->pos_edge3, pat->pos_edge4 };

                              // Enregistrement des vertex & edges existant
   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       Vertex* vh    = pvert [nro];
       Vertex* vh1   = pvert [(nro+1) MODULO QUAD4];
       Edge*   edh   = quad->findEdge (vh, vh1);

       repVertex (nh,   vnro  [nro], vh);
       repEdgeH  (nh,   ednro [nro], edh);
       }

   Real3 orig, ib, jb, pb, pc;
                              // Creation des vertex
   tvert[0]->getPoint (pc);
   tvert[1]->getPoint (orig);
   tvert[2]->getPoint (pb);
   calc_vecteur (orig, pb, ib);
   calc_vecteur (orig, pc, jb);

   for (int nro=0 ; nro<pat->nbr_vertex ; nro++)
       {
       double lambda = pat->pat_vertex [nro].v_x;
       double mu     = pat->pat_vertex [nro].v_y;
       double px = orig[dir_x] + lambda*ib[dir_x] + mu*jb[dir_x];
       double py = orig[dir_y] + lambda*ib[dir_y] + mu*jb[dir_y];
       double pz = orig[dir_z] + lambda*ib[dir_z] + mu*jb[dir_z];
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
int Elements::extrudeQuad (Pattern* pat)
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
int Elements::replaceHexa (int nh, Pattern* pat, Hexa* hexa)
{
   int vnro [4] = { 0, 1, 2, pat->pos_vertex4};
   int ednro[4] = { 0, 1, pat->pos_edge3, pat->pos_edge4 };

                              // Enregistrement des edges & quads existants
   if (hexa!=NULL)
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
