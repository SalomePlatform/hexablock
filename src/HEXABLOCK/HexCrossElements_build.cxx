
// C++ : Gestion des cylindres croises : construction

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
//  See http://www.salome-platform.org/ 
//  or email : webmaster.salome@opencascade.com

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

BEGIN_NAMESPACE_HEXA

static const int    MaxLevel    = 7;
static const double epaiss2 = 0.5;
static const double UnSur2pi = DEMI/M_PI;       

double calcul_centre (Vertex* orig, Vertex* inter); 

void geom_define_line (string& brep);
void geom_asso_point  (double angle, Vertex* node);
 
void geom_create_circle (double* milieu, double rayon, double* normale, 
                         double* base, string& brep);
int  geom_create_cylcyl (double* borig, double* bnorm, double* bbase, 
                         double  bray,  double  bhaut, 
                         double* sorig, double* snorm, double* sbase, 
                         double  sray,  double  shaut);
int  geom_asso_cylcyl   (Edge* edge);

// ====================================================== createBigCyl
void CrossElements::createBigCyl ()
{
   const int iv0   = 0;
   const int kv_mil = at_left ? 2 : 4 ;

   double z0, z1, z2, z3, z4;
   z0 = big_hauteur[0] = - calcul_centre (cross_cyl2->getBase(), cross_center);
   z4 = big_hauteur[4] = z0 + cross_cyl2->getHeight ();

   z1 = big_hauteur[1] = getVertexIJK (CylSmall, NxExt, S_SE, kv_mil)->getZ();
   z2 = big_hauteur[2] = getVertexIJK (CylSmall, NxExt, S_E,  kv_mil)->getZ();
   z3 = big_hauteur[3] = getVertexIJK (CylSmall, NxExt, S_NE, kv_mil)->getZ();

   enum { k0, k1, k2, k3, k4 };

   setVertex (CylBig, iv0, 0, k0,    0, 0, z0);
   addSlice  (CylBig, NxInt, k0, z0);
   addSlice  (CylBig, NxExt, k0, z0);

   setVertex (CylBig, iv0, 0, k4,    0, 0, z4);
   addSlice  (CylBig, NxInt, k4, z4);
   addSlice  (CylBig, NxExt, k4, z4);

   //------------------------------- Points intermediaires :

   double xe1 = getVertexIJK (CylBig, NxExt, S_N, k0)->getX();
   double ye1 = getVertexIJK (CylBig, NxExt, S_N, k0)->getY();

   double xe2 = getVertexIJK (CylBig, NxExt, S_S, k0)->getX();
   double ye2 = getVertexIJK (CylBig, NxExt, S_S, k0)->getY();

   double xi1 = getVertexIJK (CylBig, NxInt, S_N, k0)->getX();
   double yi1 = getVertexIJK (CylBig, NxInt, S_N, k0)->getY();

   double xi2 = getVertexIJK (CylBig, NxInt, S_S, k0)->getX();
   double yi2 = getVertexIJK (CylBig, NxInt, S_S, k0)->getY();

   //------------------------------- Reprise des vertex du cylindre 1

   //------------------------------- Centre
   if (grid_type != GR_BIPIPE) 
       copyVertex (NxExt, S_S,  3, iv0,   0,    k1);

   //------------------------------- Creation Vertex Nord-Sud
   for (int nk=k1 ; nk<k4 ; nk++)
       {
       setVertex  (CylBig, NxExt, S_N, nk,   xe1, ye1, big_hauteur[nk]);
       setVertex  (CylBig, NxExt, S_S, nk,   xe2, ye2, big_hauteur[nk]);
       setVertex  (CylBig, NxInt, S_N, nk,   xi1, yi1, big_hauteur[nk]);
       setVertex  (CylBig, NxInt, S_S, nk,   xi2, yi2, big_hauteur[nk]);
       }
 
   //------------------------------- Face k1 externe :

   copyVertex (NxExt, S_S,  5,   NxExt, S_E,  k1);
   copyVertex (NxExt, S_SE, 5,   NxExt, S_NE, k1);
   copyVertex (NxExt, S_SW, 5,   NxExt, S_SE, k1);

   copyVertex (NxExt, S_S,  1,   NxExt, S_W,  k1);
   copyVertex (NxExt, S_SE, 1,   NxExt, S_NW, k1);
   copyVertex (NxExt, S_SW, 1,   NxExt, S_SW, k1);

   //------------------------------- Face k1 interne :

   copyVertex (NxExt, S_S,  2,   NxInt, S_W,  k1);
   copyVertex (NxExt, S_SE, 2,   NxInt, S_NW, k1);
   copyVertex (NxExt, S_SW, 2,   NxInt, S_SW, k1);

   copyVertex (NxExt, S_SE, 3,   NxInt, S_N,  k1);  // 16/02/2012
   copyVertex (NxExt, S_SW, 3,   NxInt, S_S,  k1);  // 16/02/2012

   copyVertex (NxExt, S_S,  4,   NxInt, S_E,  k1);
   copyVertex (NxExt, S_SE, 4,   NxInt, S_NE, k1);
   copyVertex (NxExt, S_SW, 4,   NxInt, S_SE, k1);

   // ------------------------------------------------------------ K2
   //------------------------------- Centre
   // copyVertex (iv0, 0,      3,   iv0,   0,    k2);
   // ------------------------------ Face k2 externe :


   copyVertex (NxExt, S_E,  5,   NxExt, S_NE, k2);
   copyVertex (NxExt, S_W,  5,   NxExt, S_SE, k2);

   copyVertex (NxExt, S_E,  1,   NxExt, S_NW, k2);
   copyVertex (NxExt, S_W,  1,   NxExt, S_SW, k2);

   //------------------------------- Face k2 interne :

   copyVertex (NxExt, S_E,  2,   NxInt, S_NW, k2);
   copyVertex (NxExt, S_W,  2,   NxInt, S_SW, k2);

   copyVertex (NxExt, S_E,  4,   NxInt, S_NE, k2);
   copyVertex (NxExt, S_W,  4,   NxInt, S_SE, k2);

   if (NOT at_left)
      {
      addVertex (CylBig, NxExt, S_W, k2, z2, cross_rayon [CylBig][NxExt]);
      addVertex (CylBig, NxInt, S_W, k2, z2, cross_rayon [CylBig][NxInt]);
      }

   if (NOT at_right)
      {
      addVertex (CylBig, NxExt, S_E, k2, z2, cross_rayon [CylBig][NxExt]);
      addVertex (CylBig, NxInt, S_E, k2, z2, cross_rayon [CylBig][NxInt]);
      }

   // ------------------------------------------------------------

   //------------------------------- Centre
   if (grid_type != GR_BIPIPE) 
       copyVertex (NxExt, S_N,  3,   iv0,   0,    k3);

   //------------------------------- Face k3 externe :
   copyVertex (NxExt, S_N,  5,   NxExt, S_E,  k3);
   copyVertex (NxExt, S_NE, 5,   NxExt, S_NE, k3);
   copyVertex (NxExt, S_NW, 5,   NxExt, S_SE, k3);

   copyVertex (NxExt, S_N,  1,   NxExt, S_W,  k3);
   copyVertex (NxExt, S_NE, 1,   NxExt, S_NW, k3);
   copyVertex (NxExt, S_NW, 1,   NxExt, S_SW, k3);

   //------------------------------- Face k3 interne :

   copyVertex (NxExt, S_N,  2,   NxInt, S_W,  k3);
   copyVertex (NxExt, S_NE, 2,   NxInt, S_NW, k3);
   copyVertex (NxExt, S_NW, 2,   NxInt, S_SW, k3);

   copyVertex (NxExt, S_NE, 3,   NxInt, S_N,  k3);  // 16/02/2012
   copyVertex (NxExt, S_NW, 3,   NxInt, S_S,  k3);  // 16/02/2012

   copyVertex (NxExt, S_N,  4,   NxInt, S_E,  k3);
   copyVertex (NxExt, S_NE, 4,   NxInt, S_NE, k3);
   copyVertex (NxExt, S_NW, 4,   NxInt, S_SE, k3);

   //------------------------------- Remplissage

   if (grid_type == GR_BIPIPE) 
      {
      for (int nj=0; nj<S_MAXI ; nj++)
          {
          Vertex* node = getVertexIJK (CylBig, NxInt, nj, 0);
          double  px = node->getX();
          double  py = node->getY();
          for (int nk=1; nk<4 ; nk++)
              {
              node = getVertexIJK (CylBig, NxInt, nj, nk);
              if (node != NULL)
                 {
                 node -> setX (px);
                 node -> setY (py);
                 }
              }
          }
      }
   fillGrid (CylBig); 
}
// ====================================================== createLittleCyl
void CrossElements::createLittleCyl ()
{
   double c1 = calcul_centre (cross_cyl1->getBase(), cross_center);
   double cosalpha = cos (angle_inter[CylBig]);
   double prayext  = cross_rayon [CylBig][NxExt] * cosalpha;
   double prayint  = cross_rayon [CylBig][NxInt] * cosalpha;

   double t_haut [MaxLevel] = { -c1,  -prayext,  -prayint,  
                                 0,    prayint,  prayext, 
                                 cross_cyl1->getHeight () -c1 };

   double rm = (  cross_rayon [CylSmall][NxExt] 
                + cross_rayon [CylSmall][NxInt]) / 2; 
   double rc =    cross_rayon [CylBig]  [NxInt];

   double t_rayext [MaxLevel] = { -1,  -1, rm, rc, rm, -1, -1 }; 

   int nkdeb = at_left  ? 0        : size_v1z/2;
   int nkfin = at_right ? size_v1z : size_v1z/2;
   for (int nk = nkdeb ; nk<nkfin ; nk++)
       {
       double px = t_haut [nk]; 
       if (grid_type != GR_BIPIPE) 
          addVertex (CylSmall, 0, 0, nk, px, 0);

       addSlice (CylSmall, NxInt, nk, px);
       addSlice (CylSmall, NxExt, nk, px, t_rayext[nk]);
       }


   fillGrid (CylSmall);
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

   cross_center = cross_cyl2->interCylinder (cross_cyl1, at_left, at_right);
   if (cross_center==NULL)
      return HERR;

   double cross_gray1  = cross_cyl1->getRadius ();
   double cross_gray2  = cross_cyl2->getRadius ();
   double cross_igray1 = cross_gray1 * epaiss2;
   double cross_igray2 = cross_gray2 * epaiss2;

   if (db) 
      {
      HexDisplay (cross_gray1);
      HexDisplay (cross_gray2);
      HexDisplay (cross_igray2);
      HexDisplay (cross_igray1);
      }

   angle_inter [CylSmall] = M_PI/6;
   angle_inter [CylBig] = asin (cross_gray1/cross_gray2);

   cross_rayon [CylSmall] [0] = cross_rayon [CylBig] [0] = 0;

   cross_rayon [CylSmall] [NxInt] = cross_igray1;
   cross_rayon [CylSmall] [NxExt] = cross_gray1;
   cross_rayon [CylBig]   [NxInt] = cross_igray2;
   cross_rayon [CylBig]   [NxExt] = cross_gray2;

   createLittleCyl ();
   createBigCyl    ();

   if (at_left)
      {
      adjustLittleSlice (NxExt, 1, NxExt);
      if (grid_type == GR_BIPIPE) 
         adjustLittleSlice (NxInt, 2, NxInt);
      }

   if (at_right)
      {
      adjustLittleSlice (NxExt, 5, NxExt);
      if (grid_type == GR_BIPIPE) 
         adjustLittleSlice (NxInt, 4, NxInt);
      }

   Vector* iprim = new Vector (cross_cyl1->getDirection());
   Vector* kprim = new Vector (cross_cyl2->getDirection());
   Vector* jprim = new Vector (kprim);

   iprim->renormer ();
   kprim->renormer ();
   jprim->vectoriel (kprim, iprim);

   transfoVertices (cross_center, iprim, jprim, kprim);

   Real3 snorm, bnorm;
   iprim->getCoord (snorm);
   kprim->getCoord (bnorm);
   assoCylinder (CylSmall, snorm);
   assoCylinder (CylBig,   bnorm);

   if (at_left)
      { 
      assoIntersection (NxExt, 1, snorm, bnorm);
      if (grid_type == GR_BIPIPE) 
         { 
         assoIntersection (NxInt, 2, snorm, bnorm);
         }
      }

   if (at_right)
      { 
      assoIntersection (NxExt, NbrSlices1-1, snorm, bnorm);
      if (grid_type == GR_BIPIPE) 
         { 
         assoIntersection (NxInt, NbrSlices1-2, snorm, bnorm);
         }
      }

   assoResiduelle ();
   return HOK;
}
// ====================================================== copyVertex
void CrossElements::copyVertex (int i1, int j1, int k1, int i2, int j2, int k2)
{
   Vertex*  node = getVertexIJK (CylSmall, i1, j1, k1);
   if (node!=NULL)
      {
      setVertex (node, CylBig, i2, j2, k2);
      return;
      }

   double rayon   = cross_rayon [CylBig][i2];
   double hauteur = big_hauteur [k2];

   addVertex (CylBig, i2, j2, k2, hauteur, rayon);
}
// ===================================================== assoCylinder 
void CrossElements::assoCylinder (int cyl, double* normal)
{
   Real3   base, vec1, center, east, west, nordest;
   int nk = 0;
   if (cyl==CylSmall && NOT at_left) 
      nk  = size_hz[cyl];

   Vertex* v_e  = getVertexIJK (cyl, NxExt, S_E , nk);
   Vertex* v_ne = getVertexIJK (cyl, NxExt, S_NE, nk);
   Vertex* v_w  = getVertexIJK (cyl, NxExt, S_W , nk);

   v_e->getPoint (east); 
   v_w->getPoint (west); 
   calc_vecteur  (west, east, base);

   for (int nro=0 ; nro<DIM3 ; nro++) 
       center[nro] = (east[nro] + west[nro])/2;

   v_ne->getPoint (nordest); 
   calc_vecteur (center, nordest, vec1);

   double ps    = prod_scalaire (base, vec1);
   double pnorm = calc_norme(base) * calc_norme(vec1);

   angle_inter [cyl] = acos (ps/pnorm);

   if (cyl==CylBig || at_left)
      {
      assoSlice (cyl, base, normal, NxExt, 0);
      if (grid_type == GR_BIPIPE) 
         {
         assoSlice (cyl, base, normal, NxInt, 0);
         assoSlice (cyl, base, normal, NxInt, 1);
         }
      }

   if (cyl==CylBig || at_right)
      {
      assoSlice (cyl, base, normal, NxExt, size_hz[cyl]);
      if (grid_type == GR_BIPIPE) 
         {
         assoSlice (cyl, base, normal, NxInt, size_hz[cyl]);
         assoSlice (cyl, base, normal, NxInt, size_hz[cyl]-1);
         }
      }

   if (cyl==CylBig)
      for (int nz=1 ; nz<=3 ; nz++)
          assoBigMiddle (base, normal, nz);
}
// ===================================================== assoBigMiddle 
void CrossElements::assoBigMiddle (double* base, double* normal, int nzs)
{
   Real3  center, pnt1, pnt2;
   string brep;
   int nx = NxExt;

   Vertex* v_n  = getVertexIJK (CylBig, nx, S_N , nzs);
   Vertex* v_s  = getVertexIJK (CylBig, nx, S_S , nzs);

   v_s->getPoint (pnt1); 
   v_n->getPoint (pnt2); 

   double rayon = calc_distance (pnt1, pnt2)/2;
   double alpha = angle_inter [CylBig];
   if (nzs != 2)
      {
      double h1  = cross_rayon[CylSmall][NxExt] * cos (angle_inter[CylSmall]);
      alpha = asin (h1/cross_rayon[CylBig][NxExt]);
      }

   for (int nro=0 ; nro<DIM3 ; nro++) 
       center[nro] = (pnt1[nro] + pnt2[nro])/2; 

   geom_create_circle (center, rayon, normal, base, brep);
   geom_define_line   (brep);   // pour geom_asso_point

   if (NOT at_right)
      {
      assoArc (CylBig, NxExt, S_SE, nzs, brep, rayon);
      assoArc (CylBig, NxExt, S_E,  nzs, brep, rayon);
      }

   assoArc (CylBig, NxExt, S_NE, nzs, brep, rayon);
   assoArc (CylBig, NxExt, S_N , nzs, brep, rayon);

   if (NOT at_left)
      {
      assoArc (CylBig, NxExt, S_NW, nzs, brep, rayon);
      assoArc (CylBig, NxExt, S_W , nzs, brep, rayon);
      }

   assoArc (CylBig, NxExt, S_SW, nzs, brep, rayon);
   assoArc (CylBig, NxExt, S_S , nzs, brep, rayon);
}
// ====================================================== adjustLittleSlice
void CrossElements::adjustLittleSlice (int ni, int nk, int nibig)
{
   Vertex* node = getVertexIJK (CylSmall, ni, S_N, nk);
   if (node==NULL)
      return;

   double grayon2 = cross_rayon[CylBig][nibig] * cross_rayon[CylBig][nibig];
   double prayon  = cross_rayon[CylSmall][ni];
 
   for (int nj=0;  nj<S_MAXI ; nj++)
       {
       node  = getVertexIJK (CylSmall, ni, nj, nk);
       double angle = getAngle (CylSmall, nj);
       double py = prayon * cos (angle);
       double pz = prayon * sin (angle);
       double px = sqrt (grayon2-py*py);

       double qx = node->getX();
       if (qx>=0) node->setX ( px);
       else       node->setX (-px);
       node->setY (py);
       node->setZ (pz);
       }
}
// ===================================================== assoIntersection 
int CrossElements::assoIntersection (int nxs, int nzs, double* snorm, 
                                                       double* bnorm)
{
   Real3  center;
   Real3  pse, psw, sorig, sbase;
   Real3  pbe, pbw, borig, bbase;
   string brep;

   cross_center->getPoint (center);
   int nz = nzs < 3 ? 0 : NbrSlices1;

   getVertexIJK (CylSmall, nxs, S_E , nz)->getPoint (pse);
   getVertexIJK (CylSmall, nxs, S_W , nz)->getPoint (psw);
   getVertexIJK (CylBig,   nxs, S_E , 0) ->getPoint (pbe);
   getVertexIJK (CylBig,   nxs, S_W , 0) ->getPoint (pbw);

   double srayon = calc_distance (pse, psw)/2;
   double brayon = calc_distance (pbe, pbw)/2;

   calc_milieu  (psw, pse, sorig);
   calc_milieu  (pbw, pbe, borig);
   calc_vecteur (psw, pse, sbase);
   calc_vecteur (pbw, pbe, bbase);

   double shaut = calc_distance (center, sorig);
   double bhaut = calc_distance (center, borig)*2;
   double* orig = nzs < 3 ? sorig : center;       // Pb orientation

   int ier = geom_create_cylcyl (borig, bnorm, bbase, brayon, bhaut, 
                                  orig, snorm, sbase, srayon, shaut);
   if (ier != HOK)
      return ier;

   for (int ny=S_E ; ny<=S_SE ; ny++)
       {
       Vertex* node = getVertexIJK (CylSmall, nxs, ny, nzs);
       if (node!=NULL)
           node->clearAssociation (); 
       }

   for (int ny=S_E ; ny<=S_SE ; ny++)
       {
       Edge* edge = getEdgeJ (CylSmall, nxs, ny, nzs);
       geom_asso_cylcyl (edge);
       }

   return HOK;
}
END_NAMESPACE_HEXA
