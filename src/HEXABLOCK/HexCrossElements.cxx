
// C++ : Gestion des cylindres croises

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

#include "HexCrossElements.hxx"

#include "HexDocument.hxx"
#include "HexVector.hxx"
#include "HexVertex.hxx"
#include "HexHexa.hxx"
#include "HexEdge.hxx"

#include "HexGlobale.hxx"
#include "HexCylinder.hxx"
#include "HexShape.hxx"

static bool   db  = false;

static const double UnSur2pi = DEMI/M_PI;

BEGIN_NAMESPACE_HEXA

void geom_create_circle (double* milieu, double rayon, double* normale, 
                         double* base, string& brep);

void geom_define_line (string& brep);
void geom_asso_point  (double angle, Vertex* node);

// ====================================================== Constructeur
CrossElements::CrossElements (Document* doc, EnumGrid type) 
             : Elements (doc)
{
   cross_cyl1   = NULL;
   cross_cyl1   = NULL;
   cross_cyl2   = NULL;
   cross_center = NULL;
   grid_type    = type;
   angle_inter [CylSmall] = angle_inter [CylBig] =  0;
   at_right  = at_left = true;
   is_filled = false;
}
// ====================================================== resize
void CrossElements::resize ()
{
   size_hx  = 2;
   size_hy  = S_MAXI;
   size_hz [CylSmall] = size_h1z;
   size_hz [CylBig]   = size_h2z;

   size_vx  = size_hx + 1;
   size_vy  = size_hy;
   size_vz[CylSmall] = size_v1z;
   size_vz[CylBig]   = size_v2z;

   nbr_vertex1 = size_vx*size_vy* size_v1z;
   nbr_quads1  = nbr_vertex1*DIM3;
   nbr_edges1  = nbr_quads1;
   nbr_hexas1  = size_hx * size_hy * size_h1z; 

   nbr_vertex  = nbr_vertex1 + size_vx * size_vy * size_v1z;
   nbr_quads   = nbr_vertex*DIM3;
   nbr_edges   = nbr_quads;
   nbr_hexas   = nbr_hexas1 + size_hx * size_hy * size_h2z;

   tab_hexa  .resize (nbr_hexas*2);
   tab_quad  .resize (nbr_quads*2);
   tab_edge  .resize (nbr_edges*2);
   tab_vertex.resize (nbr_vertex*2);

   for (int nc=0 ; nc< nbr_hexas  ; nc++) tab_hexa   [nc] = NULL;
   for (int nc=0 ; nc< nbr_quads  ; nc++) tab_quad   [nc] = NULL;
   for (int nc=0 ; nc< nbr_edges  ; nc++) tab_edge   [nc] = NULL;
   for (int nc=0 ; nc< nbr_vertex ; nc++) tab_vertex [nc] = NULL;
}
// ====================================================== indHexa
int CrossElements::indHexa (int cyl, int nx, int ny, int nz)
{
   if (cyl<0 || cyl>1) 
       return NOTHING;
   if (   nx < 0 || nx >= size_hx || ny < 0 || ny >= size_hy 
       || nz < 0 || nz >= size_hz[cyl]) return NOTHING; 

   int nro = cyl*nbr_hexas1 + nx + size_hx*ny + size_hx*size_hy*nz;
   return nro;
}
// ====================================================== indQuad
int CrossElements::indQuad (int cyl, int dd, int nx, int ny, int nz)
{
   if (cyl<0 || cyl>1 || dd <0 || dd >= DIM3) 
       return NOTHING;
   if (   nx < 0 || nx >= size_vx || ny < 0 || ny >= size_vy 
       || nz < 0 || nz >= size_vz[cyl]) return NOTHING; 

   int nro = cyl*nbr_quads1 + nx + size_vx*ny + size_vx*size_vy*nz 
                                 + size_vx*size_vy*size_vz[cyl]*dd;
   return nro;
}
// ====================================================== indEdge
int CrossElements::indEdge (int cyl, int dd, int nx, int ny, int nz)
{
   return indQuad (cyl, dd, nx, ny, nz);
}
// ====================================================== indVertex
int CrossElements::indVertex (int cyl, int nx, int ny, int nz)
{
   if (cyl<0 || cyl>1) 
      return NOTHING;
   if (   nx < 0 || nx >= size_vx || ny < 0 || ny >= size_vy 
       || nz < 0 || nz >= size_vz[cyl]) 
      return NOTHING;

   int nro = cyl*nbr_vertex1 + nx + size_vx*ny + size_vx*size_vy*nz;
   return nro;
}
// ------------------------------------------------------------------------
// ====================================================== getHexaIJK
Hexa* CrossElements::getHexaIJK (int cyl, int nx, int ny, int nz)
{
   int nro = indHexa (cyl, nx, ny, nz);

   if (nro >= 0 && tab_hexa[nro]!= NULL &&  tab_hexa[nro]->isHere ())
      return tab_hexa [nro]; 
   else
      return NULL;
}
// ====================================================== getQuadIJ
Quad* CrossElements::getQuadIJ (int cyl, int nx, int ny, int nz)
{
   int nro = indQuad (cyl, dir_z, nx, ny, nz);
   if (nro<0)
      return NULL;

   return tab_quad [nro]; 
}
// ====================================================== getQuadJK
Quad* CrossElements::getQuadJK (int cyl, int nx, int ny, int nz)
{
   int nro = indQuad (cyl, dir_x, nx, ny, nz);
   if (nro<0)
      return NULL;
   return tab_quad [nro]; 
}
// ====================================================== getQuadIK
Quad* CrossElements::getQuadIK (int cyl, int nx, int ny, int nz)
{
   int nro = indQuad (cyl, dir_y, nx, ny, nz);
   if (nro<0)
      return NULL;
   return tab_quad [nro]; 
}
// ====================================================== getEdgeI
Edge* CrossElements::getEdgeI (int cyl, int nx, int ny, int nz)
{
   int nro = indEdge (cyl, dir_x, nx, ny, nz);
   if (nro<0)
      return NULL;
   return tab_edge [nro]; 
}
// ====================================================== getEdgeJ
Edge* CrossElements::getEdgeJ (int cyl, int nx, int ny, int nz)
{
   int nro = indEdge (cyl, dir_y, nx, ny, nz);
   if (nro<0)
      return NULL;
   return tab_edge [nro]; 
}
// ====================================================== getEdgeK
Edge* CrossElements::getEdgeK (int cyl, int nx, int ny, int nz)
{
   int nro = indEdge (cyl, dir_z, nx, ny, nz);
   if (nro<0)
      return NULL;
   return tab_edge [nro]; 
}
// ====================================================== getVertexIJK
Vertex* CrossElements::getVertexIJK (int cyl, int nx, int ny, int nz)
{
   int nro = indVertex (cyl, nx, ny, nz);
   if (nro<0)
      return NULL;
   return tab_vertex [nro]; 
}
// ------------------------------------------------------------------------
// ====================================================== setHexa
void CrossElements::setHexa (Hexa* elt, int cyl, int nx, int ny, int nz)
{
   if (db)
      {
      printf ("tab_hexa [%d, %d,%d,%d] = ", cyl, nx, ny, nz);
      PrintName (elt);
      printf ("\n");
      }

   int nro = indHexa (cyl, nx, ny, nz);
   if (nro<0) 
      return;
   tab_hexa [nro] = elt;
}
// ====================================================== setQuad
void CrossElements::setQuad (Quad* elt, int cyl, int dd, int nx, int ny, int nz)
{
   if (db)
      {
      printf ("tab_quad [%d,%d, %d,%d,%d] = ", cyl, dd, nx, ny, nz);
      PrintName (elt);
      printf ("\n");
      }

   int nro = indQuad (cyl, dd, nx, ny, nz);
   if (nro<0) 
      return;
   tab_quad [nro] = elt;
}
// ====================================================== setEdge
void CrossElements::setEdge (Edge* elt, int cyl, int dd, int nx, int ny, int nz)
{
   if (db)
      {
      printf ("tab_edge [%d,%d, %d,%d,%d] = ", cyl, dd, nx, ny, nz);
      PrintName (elt);
      printf ("\n");
      }

   int nro = indEdge (cyl, dd, nx, ny, nz);
   if (nro<0) 
      return;
   tab_edge [nro] = elt;
}
// ====================================================== setVertex
void CrossElements::setVertex (Vertex* elt, int cyl, int nx, int ny, int nz)
{
   if (db)
      {
      printf ("tab_vertex [%d, %d,%d,%d] = ", cyl, nx, ny, nz);
      PrintName (elt);
      printf ("\n");
      }

   int nro = indVertex (cyl, nx, ny, nz);
   if (nro<0) 
      return;
   tab_vertex [nro] = elt;
}
// ====================================================== setVertex (2)
void CrossElements::setVertex (int cyl, int nx, int ny, int nz, double px, 
                                                     double py, double pz)
{
   int nro = indVertex (cyl, nx, ny, nz);
   if (nro<0) 
      return;
   Vertex*    node = el_root->addVertex (px, py, pz);
   setVertex (node, cyl, nx, ny, nz);
}
// ====================================================== copyEdge
void CrossElements::copyEdge (int d1, int i1, int j1, int k1, int d2, int i2, 
                                      int j2, int k2)
{
   Edge* edge = NULL;
   switch (d1) 
          {
          case dir_x : edge = getEdgeI (CylSmall, i1, j1, k1);
                       break;
          case dir_y : edge = getEdgeJ (CylSmall, i1, j1, k1);
                       break;
          case dir_z : edge = getEdgeK (CylSmall, i1, j1, k1);
                       break;
          }

   setEdge (edge, CylBig, d2, i2, j2, k2);
}
// ====================================================== copyQuad
void CrossElements::copyQuad (int d1, int i1, int j1, int k1, int d2, int i2, 
                                              int j2, int k2)
{
   Quad* quad = NULL;
   switch (d1) 
          {
          case dir_x : quad = getQuadJK (CylSmall, i1, j1, k1);
                       break;
          case dir_y : quad = getQuadIK (CylSmall, i1, j1, k1);
                       break;
          case dir_z : quad = getQuadIJ (CylSmall, i1, j1, k1);
                       break;
          }

   setQuad (quad, CylBig, d2, i2, j2, k2);
}
// ====================================================== addEdge
Edge* CrossElements::addEdge (Vertex* v1, Vertex* v2, int cyl, int dir, 
                             int nx, int ny, int nz)
{
   Edge* edge = NULL;
   if (v1==NULL || v2==NULL)
      return NULL;

   else if (cyl==CylBig) 
      {
      edge = findEdge1 (v1, v2);
      /* ************************************************
      if (edge != NULL) 
         {
         printf (" Edge (%d, %d,%d,%d) trouve = ", dir, nx, ny, nz);
         edge->printName ("\n");
         }
         ************************************************ */
      }

   if (edge == NULL) 
      edge = newEdge (v1, v2);

   setEdge (edge, cyl, dir, nx, ny, nz);
   return edge;
}
// ====================================================== addHexa
Hexa* CrossElements::addHexa (Quad* q1, Quad* q2, Quad* q3, Quad* q4, Quad* q5,
                            Quad* q6, int cyl, int nx, int ny, int nz)
{
/* **************************
   if (cyl==CylSmall)
      {
      if (nx == 0)  return NULL;
      if (nz != 1)  return NULL;
      if (ny != 4)  return NULL;
      return NULL;
      }

   if (cyl==CylBig)
      {
      if (nz >  2)  return NULL;
      if (nz <  1)  return NULL;
      // if (nx == 0)  return NULL;
      // if (ny != 4)  return NULL;
      }
   ************************** */

   Hexa* hexa = NULL;
   if (cyl==CylBig) 
      hexa = findHexa1 (q1, q2);

   if (hexa == NULL) 
       hexa = newHexa (q1, q2, q3, q4, q5, q6);
   else if (db)
      {
      printf (" Hexa (%d,%d,%d) trouve = ", nx, ny, nz);
      hexa->printName ("\n");
      }

   setHexa (hexa, cyl, nx, ny, nz);
   return   hexa;
}
// ====================================================== addQuad
Quad* CrossElements::addQuad (Edge* e1, Edge* e2, Edge* e3, Edge* e4, int cyl, 
                             int dir, int nx, int ny, int nz)
{
   Quad* quad = NULL;
   if (cyl==CylBig) 
      quad = findQuad1 (e1, e3);

   if (quad == NULL) 
       quad = newQuad (e1, e2, e3, e4);
   else if (db)
      {
      printf (" Quad (%d, %d,%d,%d) trouve = ", dir, nx, ny, nz);
      quad->printName ("\n");
      }

   setQuad (quad, cyl, dir, nx, ny, nz);
   return quad;
}
// ====================================================== findEdge1
Edge* CrossElements::findEdge1 (Vertex* v1, Vertex* v2)
{
   for (int nc=0; nc < nbr_edges1 ; nc++)
       if (tab_edge[nc] != NULL && tab_edge[nc]->definedBy (v1, v2))
          return tab_edge[nc];

   return NULL;
}
// ====================================================== findHexa1
Hexa* CrossElements::findHexa1 (Quad* e1, Quad* e2)
{
   for (int nc=0; nc < nbr_hexas1 ; nc++)
       if (tab_hexa[nc] != NULL && tab_hexa[nc]->definedBy (e1, e2))
          return tab_hexa[nc];

   return NULL;
}
// ====================================================== findQuad1
Quad* CrossElements::findQuad1 (Edge* e1, Edge* e2)
{
   for (int nc=0; nc < nbr_quads1 ; nc++)
       if (tab_quad[nc] != NULL && tab_quad[nc]->definedBy (e1, e2))
          return tab_quad[nc];

   return NULL;
}
// ====================================================== fillGrid
void CrossElements::fillGrid (int cyl, int deb, int fin)
{
   int nz0 = deb >= 0 ? deb : 0;
   int nzn = fin >  0 ? fin : size_vz [cyl];

   fillCenter (cyl, nz0, nzn);

   for (int nz=nz0 ; nz<nzn ; nz++)
       {
       for (int nx=1 ; nx<size_hx ; nx++)
           {
                                 //   Edges horizontaux radiaux
                                 //   Edges horizontaux // cloisons ext
           for (int ny=0 ; ny<size_vy ; ny++)
               {
               int ny1  = (ny + 1) MODULO size_vy;
               Vertex* vc = getVertexIJK (cyl, nx,   ny, nz);
               Vertex* v1 = getVertexIJK (cyl, nx+1, ny, nz);
               Vertex* v2 = getVertexIJK (cyl, nx+1, ny1,nz);

               addEdge (vc, v1, cyl, dir_x, nx,   ny, nz);
               addEdge (v1, v2, cyl, dir_y, nx+1, ny, nz);
               }
                                 //   Quads horizontaux
           for (int ny=0 ; ny<size_vy ; ny++)
               {
               int ny1  = (ny + 1) MODULO size_vy;
               Edge* e1 = getEdgeI (cyl, nx,   ny,  nz);
               Edge* e2 = getEdgeJ (cyl, nx+1, ny,  nz);
               Edge* e3 = getEdgeI (cyl, nx,   ny1, nz);
               Edge* e4 = getEdgeJ (cyl, nx,   ny,  nz);

               addQuad (e1, e2, e3, e4, cyl, dir_z, nx, ny, nz);  
               }

           if (nz>0)
              {
                                 //   Edges verticaux + cloisons interieures
              for (int ny=0 ; ny<size_vy ; ny++)
                  {
                  Vertex* vp = getVertexIJK (cyl, nx+1, ny, nz-1);
                  Vertex* vv = getVertexIJK (cyl, nx+1, ny, nz);

                  Edge* edge = addEdge (vp, vv, cyl, dir_z, nx+1, ny, nz-1);

                  Edge* e0 = getEdgeI (cyl, nx, ny,  nz-1);
                  Edge* e2 = getEdgeI (cyl, nx, ny,  nz);
                  Edge* e3 = getEdgeK (cyl, nx, ny,  nz-1);

                  addQuad (e0, edge, e2, e3, cyl, dir_y, nx, ny, nz-1);  
                  }
                                 //   Cloisons exterieures ***
              for (int ny=0 ; ny<size_vy ; ny++)
                  {
                  int ny1  = (ny + 1) MODULO size_vy;
                  Edge* e0 = getEdgeJ (cyl, nx+1, ny,  nz-1);
                  Edge* e2 = getEdgeJ (cyl, nx+1, ny,  nz);

                  Edge* e1 = getEdgeK (cyl, nx+1, ny,  nz-1);
                  Edge* e3 = getEdgeK (cyl, nx+1, ny1, nz-1);
                  addQuad (e0, e1, e2, e3, cyl, dir_x, nx+1, ny, nz-1);  
                  }
                                 //   Hexas (8) 
              if (is_filled || cyl==CylBig || (nz!=3 && nz != 4))
                  {
                  for (int ny=0 ; ny<size_hy ; ny++)
                      {
                      int ny1  = (ny + 1) MODULO size_vy;
                      // printf (" ---------- Hexa : nz=%d, ny=%d\n", ny, nz);
                      Quad* qa = getQuadIJ (cyl, nx, ny, nz-1);
                      Quad* qb = getQuadIJ (cyl, nx, ny, nz);

                      Quad* qc = getQuadJK (cyl, nx+1, ny,  nz-1);
                      Quad* qd = getQuadJK (cyl, nx,   ny,  nz-1);

                      Quad* qe = getQuadIK (cyl, nx,   ny1, nz-1);
                      Quad* qf = getQuadIK (cyl, nx,   ny,  nz-1);

                      // Hexa* cell = newHexa (qa, qb, qc, qd, qe, qf);
                      // setHexa (cell, cyl, nx, ny, nz-1);  
                      addHexa (qa, qb, qc, qd, qe, qf, cyl, nx, ny, nz-1);  
                      }
                  }
              }
          }
       }
}
// ====================================================== fillCenter
void CrossElements::fillCenter (int cyl, int nz0, int nzn)
{
   int nx = 0;

   for (int nz=nz0 ; nz<nzn ; nz++)
       {
       Vertex* center = getVertexIJK (cyl, 0, 0, nz);
                                 //   Edges horizontaux // cloisons ext
           for (int ny=0 ; ny<size_vy ; ny++)
               {
               Vertex* v1 = getVertexIJK (cyl, 1, ny, nz);
               Vertex* v2 = getVertexIJK (cyl, 1, (ny+1) MODULO size_vy, nz);
               addEdge (v1, v2, cyl, dir_y, nx+1, ny, nz);
               }
       if (is_filled)
          {
                                 //   Edges horizontaux radiaux
           for (int nc=0 ; nc<NbrIntCotes ; nc++)
               {
               Vertex* vv = getVertexIJK (cyl, 1, 2*nc, nz);
               addEdge (center, vv, cyl, dir_x, nx, nc, nz);
               }
                                 //   Quads horizontaux
           for (int nc=0; nc<NbrIntCotes ; nc++)
               {
               int nc1  = (nc + 1) MODULO NbrIntCotes;
               Edge* e1 = getEdgeI (cyl, nx, nc,    nz);
               Edge* e2 = getEdgeJ (cyl, nx+1, 2*nc,  nz);
               Edge* e3 = getEdgeJ (cyl, nx+1, 2*nc+1, nz);
               Edge* e4 = getEdgeI (cyl, nx, nc1,    nz);

               addQuad (e1, e2, e3, e4, cyl, dir_z, nx, nc, nz);  
               }
          }

       if (nz>0)
          {
                                 //   Edges verticaux + cloisons interieures
          Vertex* vhaut  = getVertexIJK (cyl,nx,0, nz-1);
          Edge*   pilier = addEdge (center, vhaut, cyl, dir_z, nx, 0, nz-1);

          for (int ny=0 ; ny<size_vy ; ny++)
              {
              Vertex* vp = getVertexIJK (cyl, 1, ny, nz-1);
              Vertex* vv = getVertexIJK (cyl, 1, ny, nz);
              Edge* edge = addEdge (vp, vv, cyl, dir_z, 1, ny, nz-1);
              if (is_filled && (ny MODULO 2) == 0)
                 {
                 Edge* e0 = getEdgeI (cyl, nx, ny/2,  nz-1);
                 Edge* e2 = getEdgeI (cyl, nx, ny/2,  nz);
                 addQuad (e0,edge, e2,pilier, cyl, dir_y, nx, ny/2, nz-1);
                 }
              }
                                 //   Cloisons exterieures
          for (int ny=0 ; ny<size_vy ; ny++)
              {
              int ny1  = (ny + 1) MODULO size_vy;
              Edge* e0 = getEdgeJ (cyl, 1, ny,  nz-1);
              Edge* e2 = getEdgeJ (cyl, 1, ny,  nz);
              Edge* e1 = getEdgeK (cyl, 1, ny,  nz-1);
              Edge* e3 = getEdgeK (cyl, 1, ny1, nz-1);

              addQuad (e0, e1, e2, e3, cyl, dir_x, 1, ny, nz-1);  
              }
                                 //   Hexas (4) 
          if (is_filled)
             {
             for (int nc=0 ; nc < NbrIntCotes ; nc++)
                 {
                  // printf (" --------------- Hexa : nz=%d, nc=%d\n", nc, nz);
                 int nc1  = (nc + 1) MODULO NbrIntCotes;
                 Quad* qa = getQuadIJ (cyl, 0, nc, nz-1);
                 Quad* qb = getQuadIJ (cyl, 0, nc, nz);

                 Quad* qc = getQuadJK (cyl, 1, 2*nc,   nz-1);
                 Quad* qe = getQuadJK (cyl, 1, 2*nc+1, nz-1);

                 Quad* qd = getQuadIK (cyl, 0, nc1, nz-1);
                 Quad* qf = getQuadIK (cyl, 0, nc,  nz-1);

                  // Hexa* cell = newHexa (qa, qb, qc, qd, qe, qf);
                  // setHexa (cell, cyl, 0, nc, nz-1);  
                 addHexa (qa, qb, qc, qd, qe, qf, cyl, nx, nc, nz-1);  
                 }
              }
          }
       }
}
// ====================================================== dump
void CrossElements::dump ()
{
   int sizey [2] = { 4, S_MAXI };
   for (int cyl=CylSmall ; cyl<=CylBig ; cyl++)
       {
       printf (" +++++++++ \n");
       printf (" +++++++++ Hexas du Cylindre nro %d\n", cyl);
       printf (" +++++++++ \n");
       for (int nz=0; nz<size_hz[cyl]; nz++)
           {
           for (int nx=0; nx<size_hx; nx++)
               {
               printf ("\n");
               for (int ny=0; ny<sizey[nx]; ny++)
                   {
                   Hexa* cell = getHexaIJK (cyl,nx,ny,nz);
                   int nro = indHexa (cyl, nx, ny, nz);
                   printf ("tab_hexa[%03d] (%d, %d,%d,%d) = ", 
                           nro, cyl,nx,ny,nz);
                   if (cell!=NULL) cell->printName("\n");
                      else         printf ("NULL\n");
                   }
               }
           }
       }
}
// ====================================================== dumpVertex
void CrossElements::dumpVertex ()
{
   int sizey [3] = { 1, S_MAXI, S_MAXI };

   for (int cyl=CylSmall ; cyl<=CylBig ; cyl++)
       {
       printf (" +++++++++ \n");
       printf (" +++++++++ Vertex du Cylindre nro %d\n", cyl);
       printf (" +++++++++ \n");
       for (int nz=0; nz<size_vz[cyl]; nz++)
           {
           for (int nx=0; nx<size_vx; nx++)
               {
               printf ("\n");
               for (int ny=0; ny<sizey[nx]; ny++)
                   {
                   Vertex* node = getVertexIJK (cyl,nx,ny,nz);
                   int nro = indVertex (cyl, nx, ny, nz);
                   printf ("tab_vertex[%03d] (%d, %d,%d,%d) = ", 
                           nro, cyl,nx,ny,nz);
                   if (node!=NULL) node->printName("\n");
                      else         printf ("NULL\n");
                   }
               }
           }
       }
}
// ====================================================== dumpHexas
void CrossElements::dumpHexas ()
{
   int sizey [3] = { 1, 4, S_MAXI };

   for (int cyl=CylSmall ; cyl<=CylBig ; cyl++)
       {
       printf (" +++++++++ \n");
       printf (" +++++++++ Hexaedres du Cylindre nro %d\n", cyl);
       printf (" +++++++++ \n");
       for (int nz=0; nz<size_hz[cyl]; nz++)
           {
           for (int nx=0; nx<size_hx; nx++)
               {
               printf ("\n");
               for (int ny=0; ny<sizey[nx]; ny++)
                   {
                   Hexa* elt = getHexaIJK (cyl,nx,ny,nz);
                   if (elt!=NULL)
                      {
                      int nro = indHexa (cyl, nx, ny, nz);
                      printf ("tab_hexa[%03d] (%d, %d,%d,%d) = ", 
                              nro, cyl,nx,ny,nz);
                      elt->printName("\n");
                      }
                   }
               }
           }
       }
}
// ====================================================== calcul_centre
double calcul_centre (Vertex* orig, Vertex* inter)
{
   double dx = inter->getX () - orig->getX ();
   double dy = inter->getY () - orig->getY ();
   double dz = inter->getZ () - orig->getZ ();
   double dd = sqrt (dx*dx + dy*dy + dz*dz);
   return dd;
}
// ===================================================== assoSlice 
void CrossElements::assoSlice (int cyl, double* base, double* normal, int nx, 
                                                                      int nzs)
{
   Real3  center, pnt1, pnt2;
   string brep;

   Vertex* v_n  = getVertexIJK (cyl, nx, S_N , nzs);
   Vertex* v_s  = getVertexIJK (cyl, nx, S_S , nzs);

   v_s->getPoint (pnt1); 
   v_n->getPoint (pnt2); 

   double rayon  = calc_distance (pnt1, pnt2)/2;
   for (int nro=0 ; nro<DIM3 ; nro++) 
       center[nro] = (pnt1[nro] + pnt2[nro])/2; 

   geom_create_circle (center, rayon, normal, base, brep);
   geom_define_line   (brep);   // pour geom_asso_point

   for (int ny=0 ; ny<S_MAXI ; ny++)
       {
       assoArc (cyl, nx, ny, nzs, brep, rayon);
       }
}
// ===================================================== assoArc 
void CrossElements::assoArc (int cyl, int nx, int ny, int nz, string& brep, 
                             double rayon)
{
    double angle1 = getAngle (cyl, ny);
    double angle2 = getAngle (cyl, ny+1);
    Edge*   edge  = getEdgeJ     (cyl, nx, ny, nz);
    Shape*  shape = new Shape (brep);

    shape->setBounds (angle1*UnSur2pi, angle2*UnSur2pi);
    edge ->addAssociation (shape);

    Vertex* node  = getVertexIJK (cyl, nx, ny, nz);
    geom_asso_point  (angle1*rayon,    node);

    int ny1 = ny+1;
    if (ny1>=S_MAXI)
        ny1 = 0;

    node = getVertexIJK (cyl, nx, ny1, nz);
    geom_asso_point (angle2*rayon, node);
}
// ====================================================== getAngle
double CrossElements::getAngle (int cyl, int nj)
{
   switch (nj)
      {
      case S_E    : return 0;
      case S_NE   : return angle_inter[cyl];
      case S_N    : return M_PI/2;
      case S_NW   : return M_PI - angle_inter[cyl];
      case S_W    : return M_PI;
      case S_SW   : return M_PI + angle_inter[cyl];
      case S_S    : return 3*M_PI/2;
      case S_SE   : return 2*M_PI - angle_inter[cyl];
      case S_MAXI : return 2*M_PI;
      default     : break;
      }
   return 0;
}
// ====================================================== addVertex
void CrossElements::addVertex (int cyl, int ni, int nj, int nk, double px,
                               double rayon)
{
   if (rayon<0.0)
       rayon = cross_rayon [cyl][ni];
   double theta = getAngle (cyl, nj);

   if (cyl==CylSmall) 
       setVertex (cyl, ni, nj, nk,  px, rayon*cos(theta), rayon*sin(theta));
   else
       setVertex (cyl, ni, nj, nk,  rayon*cos(theta), rayon*sin(theta), px);
}
// ====================================================== addSlice
void CrossElements::addSlice (int cyl, int ni, int nk, double px, double rayon)
{
   for (int nj=0 ; nj < S_MAXI ; nj++)
       addVertex (cyl, ni, nj, nk, px, rayon);
}
END_NAMESPACE_HEXA
