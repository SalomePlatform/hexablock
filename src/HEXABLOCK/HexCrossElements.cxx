//  Copyright (C) 2009-2011  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// C++ : Gestion du croisement de cylindres

#include "HexCrossElements.hxx"

#include "HexDocument.hxx"
#include "HexVector.hxx"
#include "HexVertex.hxx"
#include "HexHexa.hxx"
#include "HexEdge.hxx"

#include "HexGlobale.hxx"
#include "HexCylinder.hxx"

static bool   db  = false;

static const  int NbrIntCotes = 4;
static const  int MaxLevel    = 7;

static const double epaiss2 = 0.5;

BEGIN_NAMESPACE_HEXA

double calcul_centre (Vertex* orig, Vertex* inter); 
// ====================================================== crossCylinders
CrossElements::CrossElements (Document* doc, EnumGrid type) 
             : Elements (doc)
{
   cross_cyl1   = NULL;
   cross_cyl1   = NULL;
   cross_cyl2   = NULL;
   cross_center = NULL;
   grid_type    = type;
}
// ====================================================== resize
void CrossElements::resize ()
{
   size_hx  = 2;
   size_hy  = S_MAXI;
   size_hiy = NbrIntCotes;
   size_hz [Cyl1] = size_h1z = 6;
   size_hz [Cyl2] = size_h2z = 4;

   size_vx  = size_hx + 1;
   size_vy  = size_hy;
   size_vz[Cyl1] = size_v1z = size_h1z + 1;
   size_vz[Cyl2] = size_v2z = size_h2z + 1;

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
   if (nro<0)
      return NULL;

   return tab_hexa [nro]; 
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
// ------------------------------------------------------------------------
// ====================================================== copyVertex
void CrossElements::copyVertex (int i1, int j1, int k1, int i2, int j2, int k2)
{
   Vertex*    node = getVertexIJK (Cyl1, i1, j1, k1);
   setVertex (node, Cyl2, i2, j2, k2);
}
// ====================================================== copyEdge
void CrossElements::copyEdge (int d1, int i1, int j1, int k1, int d2, int i2, 
                                      int j2, int k2)
{
   Edge* edge = NULL;
   switch (d1) 
          {
          case dir_x : edge = getEdgeI (Cyl1, i1, j1, k1);
                       break;
          case dir_y : edge = getEdgeJ (Cyl1, i1, j1, k1);
                       break;
          case dir_z : edge = getEdgeK (Cyl1, i1, j1, k1);
                       break;
          }

   setEdge (edge, Cyl2, d2, i2, j2, k2);
}
// ====================================================== copyQuad
void CrossElements::copyQuad (int d1, int i1, int j1, int k1, int d2, int i2, 
                                              int j2, int k2)
{
   Quad* quad = NULL;
   switch (d1) 
          {
          case dir_x : quad = getQuadJK (Cyl1, i1, j1, k1);
                       break;
          case dir_y : quad = getQuadIK (Cyl1, i1, j1, k1);
                       break;
          case dir_z : quad = getQuadIJ (Cyl1, i1, j1, k1);
                       break;
          }

   setQuad (quad, Cyl2, d2, i2, j2, k2);
}
// ====================================================== addEdge
Edge* CrossElements::addEdge (Vertex* v1, Vertex* v2, int cyl, int dir, 
                             int nx, int ny, int nz)
{
   Edge* edge = NULL;
   if (v1==NULL || v2==NULL)
      return NULL;

   else if (cyl==Cyl2) 
      {
      edge = findEdge1 (v1, v2);
      if (edge != NULL) 
         {
         printf (" Edge (%d, %d,%d,%d) trouve = ", dir, nx, ny, nz);
         edge->printName ("\n");
         }
      else if (v1->getId () < 122 && v2->getId () < 122)
         {
         printf (" Edge anormal = (");
         v1 ->printName (", ");
         v2 ->printName (")\n");
         }
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
   if (cyl==Cyl1)
      {
      if (nx == 0)  return NULL;
      if (nz != 1)  return NULL;
      if (ny != 4)  return NULL;
      return NULL;
      }

   if (cyl==Cyl2)
      {
      if (nz >  2)  return NULL;
      if (nz <  1)  return NULL;
      // if (nx == 0)  return NULL;
      // if (ny != 4)  return NULL;
      }
   ************************** */

   Hexa* hexa = NULL;
   if (cyl==Cyl2) 
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
   if (cyl==Cyl2) 
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
// ------------------------------------------------------------------------
// ====================================================== addLittleSlice
void CrossElements::addLittleSlice (int ni, int nk, double px, double gray, 
                                    double pray, double h1)
{
   setVertex (Cyl1, ni, S_E,  nk,    px,  pray,  0); 
   setVertex (Cyl1, ni, S_NE, nk,    px,  pray,  h1); 
   setVertex (Cyl1, ni, S_N,  nk,    px,  0,     gray); 
   setVertex (Cyl1, ni, S_NW, nk,    px, -pray,  h1); 
   setVertex (Cyl1, ni, S_W,  nk,    px, -pray,  0); 
   setVertex (Cyl1, ni, S_SW, nk,    px, -pray, -h1); 
   setVertex (Cyl1, ni, S_S,  nk,    px,  0,    -gray); 
   setVertex (Cyl1, ni, S_SE, nk,    px,  pray, -h1); 
}
// ====================================================== addBigSlice
void CrossElements::addBigSlice (int ni, int nk, double pz, double gray, 
                                 double pray, double h1)
{
   setVertex (Cyl2, ni, S_E,  nk,    pray,  0,    pz); 
   setVertex (Cyl2, ni, S_NE, nk,    pray,  h1,   pz); 
   setVertex (Cyl2, ni, S_N,  nk,    0,     gray, pz); 
   setVertex (Cyl2, ni, S_NW, nk,    -pray,  h1,  pz); 
   setVertex (Cyl2, ni, S_W,  nk,    -pray,  0,   pz); 
   setVertex (Cyl2, ni, S_SW, nk,    -pray, -h1,  pz); 
   setVertex (Cyl2, ni, S_S,  nk,    0,    -gray, pz); 
   setVertex (Cyl2, ni, S_SE, nk,    pray, -h1,   pz); 
}
// ====================================================== createBigCyl
void CrossElements::createBigCyl ()
{
   const int ivext = 2;
   const int ivint = 1;
   const int iv0   = 0;
   const int kv_mil = NbrVSlices1 / 2;

   double z0 = - calcul_centre (cross_cyl2->getBase(), cross_center);
   double z3 = getVertexIJK (Cyl1, ivext, S_NE, kv_mil)->getZ();
   double z2 = getVertexIJK (Cyl1, ivext, S_E,  kv_mil)->getZ();
   double z1 = getVertexIJK (Cyl1, ivext, S_SE, kv_mil)->getZ();
   double z4 = z0 + cross_cyl2->getHeight ();

   const int k0 = 0;
   const int k1 = 1;
   const int k2 = 2;
   const int k3 = 3;
   const int k4 = NbrVSlices2-1;

   setVertex   (Cyl2, iv0, 0, k0,    0, 0, z0);
   addBigSlice (ivint, k0, z0, cross_igray2, cross_ipray2, cross_hi2);
   addBigSlice (ivext, k0, z0, cross_gray2,  cross_pray2,  cross_he2);

   setVertex   (Cyl2, iv0, 0, k4,    0, 0, z4);
   addBigSlice (ivint, k4, z4, cross_igray2, cross_ipray2, cross_hi2);
   addBigSlice (ivext, k4, z4, cross_gray2,  cross_pray2,  cross_he2);

   //------------------------------- Points intermediaires :

   double x1 = getVertexIJK (Cyl2, ivext, S_N, k0)->getX();
   double y1 = getVertexIJK (Cyl2, ivext, S_N, k0)->getY();

   double x2 = getVertexIJK (Cyl2, ivext, S_S, k0)->getX();
   double y2 = getVertexIJK (Cyl2, ivext, S_S, k0)->getY();

   //------------------------------- Reprise des vertex du cylindre 1

   //------------------------------- Centre
   if (grid_type != GR_BIPIPE) 
       copyVertex (ivext, S_S,  3, iv0,   0,    k1);

   //------------------------------- Face k1 externe :
   setVertex   (Cyl2, ivext, S_N, k1,    x1, y1, z1);
   setVertex   (Cyl2, ivext, S_S, k1,    x2, y2, z1);

   copyVertex (ivext, S_S,  5,   ivext, S_E,  k1);
   copyVertex (ivext, S_SE, 5,   ivext, S_NE, k1);
   copyVertex (ivext, S_SW, 5,   ivext, S_SE, k1);

   copyVertex (ivext, S_S,  1,   ivext, S_W,  k1);
   copyVertex (ivext, S_SE, 1,   ivext, S_NW, k1);
   copyVertex (ivext, S_SW, 1,   ivext, S_SW, k1);

   //------------------------------- Face k1 interne :
   copyVertex (ivext, S_S,  2,   ivint, S_W,  k1);
   copyVertex (ivext, S_SE, 2,   ivint, S_NW, k1);
   copyVertex (ivext, S_SW, 2,   ivint, S_SW, k1);

   copyVertex (ivext, S_S,  4,   ivint, S_E,  k1);
   copyVertex (ivext, S_SE, 4,   ivint, S_NE, k1);
   copyVertex (ivext, S_SW, 4,   ivint, S_SE, k1);

   copyVertex (ivext, S_SW, 3,   ivint, S_S,  k1);
   copyVertex (ivext, S_SE, 3,   ivint, S_N,  k1);

   // ------------------------------------------------------------ K2
   //------------------------------- Centre
   // copyVertex (iv0, 0,      3,   iv0,   0,    k2);
   // ------------------------------ Face k2 externe :

   setVertex  (Cyl2, ivext, S_N, k2,  x1, y1, z2);
   setVertex  (Cyl2, ivext, S_S, k2,  x2, y2, z2);

   // copyVertex (iv0,   0,    5,   ivext, S_E,  k2);
   copyVertex (ivext, S_E,  5,   ivext, S_NE, k2);
   copyVertex (ivext, S_W,  5,   ivext, S_SE, k2);

   // copyVertex (iv0,   0,    1,   ivext, S_W,  k2);
   copyVertex (ivext, S_E,  1,   ivext, S_NW, k2);
   copyVertex (ivext, S_W,  1,   ivext, S_SW, k2);

   //------------------------------- Face k2 interne :
   // copyVertex (iv0,   0,    2,   ivint, S_W,  k2);
   copyVertex (ivext, S_E,  2,   ivint, S_NW, k2);
   copyVertex (ivext, S_W,  2,   ivint, S_SW, k2);

   //  copyVertex (iv0,   0,    4,   ivint, S_E,  k2);
   copyVertex (ivext, S_E,  4,   ivint, S_NE, k2);
   copyVertex (ivext, S_W,  4,   ivint, S_SE, k2);

   copyVertex (ivext, S_W,  3,   ivint, S_S,  k2);
   copyVertex (ivext, S_E,  3,   ivint, S_N,  k2);

   // ------------------------------------------------------------
   //------------------------------- Face k3 externe :

   setVertex   (Cyl2, ivext, S_N, k3,    x1, y1, z3);
   setVertex   (Cyl2, ivext, S_S, k3,    x2, y2, z3);

   //------------------------------- Centre
   if (grid_type != GR_BIPIPE) 
       copyVertex (ivext, S_N,  3,   iv0,   0,    k3);

   copyVertex (ivext, S_N,  5,   ivext, S_E,  k3);
   copyVertex (ivext, S_NE, 5,   ivext, S_NE, k3);
   copyVertex (ivext, S_NW, 5,   ivext, S_SE, k3);

   copyVertex (ivext, S_N,  1,   ivext, S_W,  k3);
   copyVertex (ivext, S_NE, 1,   ivext, S_NW, k3);
   copyVertex (ivext, S_NW, 1,   ivext, S_SW, k3);

   //------------------------------- Face k3 interne :
   copyVertex (ivext, S_N,  2,   ivint, S_W,  k3);
   copyVertex (ivext, S_NE, 2,   ivint, S_NW, k3);
   copyVertex (ivext, S_NW, 2,   ivint, S_SW, k3);

   copyVertex (ivext, S_N,  4,   ivint, S_E,  k3);
   copyVertex (ivext, S_NE, 4,   ivint, S_NE, k3);
   copyVertex (ivext, S_NW, 4,   ivint, S_SE, k3);

   copyVertex (ivext, S_NW, 3,   ivint, S_S, k3);
   copyVertex (ivext, S_NE, 3,   ivint, S_N, k3);

   //------------------------------- Remplissage
   fillGrid (Cyl2); 
}
// ====================================================== createLittleCyl
void CrossElements::createLittleCyl ()
{
   double c1 = calcul_centre (cross_cyl1->getBase(), cross_center);

   double t_haut [MaxLevel] = { -c1,  -cross_pray2,  -cross_ipray2,  
                                 0,    cross_ipray2,  cross_pray2, 
                                 cross_cyl1->getHeight () -c1 };

   double b2 = sqrt (cross_igray2*cross_igray2 - cross_ipray2*cross_ipray2);

   for (int nk = 0 ; nk<size_v1z ; nk++)
       {
       double px = t_haut [nk]; 
       if (grid_type != GR_BIPIPE) 
           setVertex (Cyl1, 0, 0, nk,    px, 0, 0); 

       addLittleSlice (1, nk, px, cross_igray1, cross_ipray1, cross_hi1);

       switch (nk)
          {
          case 2 : case 4 : 
               addLittleSlice (2, nk, px, cross_gray1, b2, cross_he1);
               break;
          case 3 : 
               addLittleSlice (2, nk, px, cross_gray1, cross_igray2, cross_he1);
               break;
          default :
               addLittleSlice (2, nk, px, cross_gray1, cross_pray1, cross_he1);
               break;
          }
       }

   fillGrid (Cyl1);
}
// ====================================================== crossCylinders
int CrossElements::crossCylinders (Cylinder* lun, Cylinder* lautre, bool fill)
{
   is_filled = fill;
   resize ();

   if (lun->getRadius() < lautre->getRadius())
      {
      cross_cyl1 = lun;
      cross_cyl2 = lautre;
      }
   else
      {
      cross_cyl1 = lautre;
      cross_cyl2 = lun;
      }

   cross_center = cross_cyl2->interCylinder (cross_cyl1);
   if (cross_center==NULL)
      return HERR;
                                             // Little
   cross_gray1  = cross_cyl1->getRadius ();
   cross_pray1  = cross_gray1 * cos (M_PI/6);
   cross_he1    = cross_gray1 / 2;
                                             // Big
   cross_gray2  = cross_cyl2->getRadius ();
   cross_pray2  = cross_gray2 * cos (M_PI/6);
   cross_he2    = cross_pray1; 

   cross_igray2 = cross_gray2 * epaiss2;
   cross_ipray2 = cross_pray2 * epaiss2;
   cross_hi2    = cross_he2   * epaiss2;

   cross_ipray1 = 0.75 * cross_hi2;
   cross_igray1 = cross_gray1 * cross_ipray1 / cross_pray1;
   cross_hi1    = cross_igray1 / 2;

   if (db) 
      {
      HexDisplay (cross_gray1);
      HexDisplay (cross_pray1);
      HexDisplay (cross_he1  );
                                             // Big
      HexDisplay (cross_gray2);
      HexDisplay (cross_pray2);
      HexDisplay (cross_he2  );

      HexDisplay (cross_igray2);
      HexDisplay (cross_ipray2);
      HexDisplay (cross_hi2   );

      HexDisplay (cross_ipray1);
      HexDisplay (cross_igray1);
      HexDisplay (cross_hi1   );
      }

   createLittleCyl ();
   createBigCyl    ();

   Vector* iprim = new Vector (cross_cyl1->getDirection());
   Vector* kprim = new Vector ((cross_cyl2->getDirection()));
   Vector* jprim = new Vector (kprim);

   iprim->renormer ();
   kprim->renormer ();
   jprim->vectoriel (kprim, iprim);

   transfoVertices (cross_center, iprim, jprim, kprim);
   return HOK;
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
              if (is_filled || cyl==Cyl2 || (nz!=3 && nz != 4))
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
   for (int cyl=Cyl1 ; cyl<=Cyl2 ; cyl++)
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
   int sizey [3] = { 1, 4, S_MAXI };

   for (int cyl=Cyl1 ; cyl<=Cyl2 ; cyl++)
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

   for (int cyl=Cyl1 ; cyl<=Cyl2 ; cyl++)
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
END_NAMESPACE_HEXA
