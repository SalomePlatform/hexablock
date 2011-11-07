
// C++ : Table d'hexaedres (Evol Versions 3)

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

#include "HexElements.hxx"

#include "HexVector.hxx"
#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexShape.hxx"
#include "HexGlobale.hxx"

#include <cmath>

BEGIN_NAMESPACE_HEXA

void geom_create_circle (double* milieu, double rayon, double* normale, 
                         double* base, string& brep);
void geom_create_sphere (double* milieu, double radius, string& brep);

static bool db=false;

// ======================================================== revolutionQuads
int Elements::revolutionQuads (Quads& start, Vertex* center, Vector* axis,
                               RealVector &angles)
{
   int nbiter = angles.size();
   if (center==NULL  || axis==NULL || nbiter==0)
      return HERR;

   el_root->markAll (NO_USED);
   int nbcells   = start.size ();
   nbr_vertex    = 0;
   nbr_edges     = 0;

   nbr_hexas   = nbcells*nbiter;

   tab_hexa.resize (nbr_hexas);
   tab_quad.clear ();          // verticaux
   tab_edge.clear ();
   tab_pilier.clear ();
   tab_vertex.clear ();

   revo_lution = true;
   revo_axis   = axis;
   revo_center = center;
   revo_angle  = angles;

   for (int nro=0 ; nro<nbcells ; nro++)
       {
       pushHexas (nro, start[nro], nbiter);
       }
   return HOK;
}
// ====================================================== makeRind
int Elements::makeRind (EnumGrid type, Vertex* center, Vector* vx, Vector* vz, 
                        double radext, double radint, double radhole, 
		        Vertex* plorig, double angle, int nr, int na, int nl)
{
   double phi1;
   int ier = controlRind (type, center, vx, vz, radext, radint, radhole,
                          plorig, angle, nr, na, nl, cyl_phi0, phi1);
   if (ier!=HOK)
      return ier;

   resize (type, nr, na, nl);

   cyl_radext  = radext;
   cyl_radint  = radint;
   cyl_radhole = radhole;
   cyl_closed  = type==GR_HEMISPHERIC || type==GR_RIND;

   double theta  = cyl_closed ? 2*M_PI : M_PI*angle/180;
   cyl_dphi      = (phi1-cyl_phi0)/ size_hz;
   cyl_dtheta    = theta / size_hy;

   int nb_secteurs = cyl_closed ? size_vy-1 : size_vy;

   for (int ny=0 ; ny<nb_secteurs ; ny++)
       for (int nx=0 ; nx<size_vx ; nx++)
           for (int nz=0 ; nz<size_vz ; nz++)
               {
               double px, py, pz;
               getCylPoint (nx, ny, nz, px, py, pz);
               Vertex* node = el_root->addVertex (px, py, pz);
               setVertex (node, nx, ny, nz);
               }
   if (cyl_closed) 
      for (int nx=0 ; nx<size_vx ; nx++)
          for (int nz=0 ; nz<size_vz ; nz++)
              {
              Vertex* node = getVertexIJK (nx, 0, nz);
              setVertex (node, nx, size_vy-1, nz);
              }

   transfoVertices (center, vx, vz);
   fillGrid ();
   assoCylinder (center, vz, angle);
   return HOK;
}
// ====================================================== getCylPoint 
int Elements::getCylPoint (int nr, int na, int nh, double& px, double& py,  
                           double& pz)
{
   if (grid_type == GR_CYLINDRIC)
      {
      px = cyl_radext * cos (na*cyl_dtheta);
      py = cyl_radext * sin (na*cyl_dtheta);
      pz = cyl_length * nh;
      return HOK;
      }

   bool   rind   = (grid_type == GR_RIND || grid_type == GR_PART_RIND);
   double sinphi = sin (cyl_phi0 + nh*cyl_dphi);
   double cosphi = cos (cyl_phi0 + nh*cyl_dphi);

   double rayon = 0;
   if (rind)
      {
      rayon = cyl_radint + nr*(cyl_radext-cyl_radint)/size_hx;
      pz    = rayon*sinphi;
      rayon = rayon*cosphi;
      }
   else
      {
      pz    = cyl_radext*sinphi;
      rayon = cyl_radhole + nr*(cyl_radext*cosphi - cyl_radhole)/size_hx;
      rayon = std::max (cyl_radhole, rayon);
      }

   px = rayon * cos (na*cyl_dtheta);
   py = rayon * sin (na*cyl_dtheta);

   return HOK;
}
// ====================================================== controlRind 
int Elements::controlRind (EnumGrid type, Vertex* cx, Vector* vx, Vector* vz, 
                           double rext, double rint, double rhole,
                           Vertex* px, double angle, 
                           int nrad, int nang, int nhaut, 
                           double &phi0, double &phi1)
{
   const double Epsil1 = 1e-6;
   phi0  = phi1 = 0;

   if (cx == NULL || vx == NULL || vz == NULL) 
      return HERR;

   if (nrad<=0 || nang<=0 || nhaut<=0)
      return HERR;

   if (rext <= 0.0) 
      return HERR;

   if (rint >= rext) 
      return HERR;

   if (rhole > rint) 
      return HERR;

   double nvx = vx->norme();
   double nvz = vz->norme();

   if (nvx < Epsil1 || nvz <  Epsil1) 
      return HERR;

   double alpha = asin (rhole/rext);
   double beta  = -M_PI*DEMI;
   if (type==GR_HEMISPHERIC || type==GR_PART_SPHERIC) 
       alpha = 2*alpha;

   if (px!=NULL)
      {
          // oh = oa.n/|n|
      double oh = ((px->getX() - cx->getX()) * vz->getDx()
                +  (px->getY() - cx->getY()) * vz->getDy()
                +  (px->getZ() - cx->getZ()) * vz->getDz()) / nvz;
      if (oh > rext) 
         return HERR;
      else if (oh > -rext) 
         beta  = asin (oh/rext);
      }

   phi0 = std::max (alpha - M_PI*DEMI, beta);
   phi1 = M_PI*DEMI - alpha;
   return HOK; 
}
// ====================================================== getHexas 
int Elements::getHexas (Hexas& liste)
{
   liste.clear ();
   for (int nro = 0 ; nro<nbr_hexas ; nro++)
       {
       Hexa* cell = tab_hexa [nro];
       if (cell!=NULL && cell->isValid())
          liste.push_back (cell);
       }
   return HOK;
}
// ====================================================== assoCylinder 
void Elements::assoCylinder (Vertex* ori, Vector* normal, double angle)
{
   //  return;           // Abu : Provisoire avant realisation 
   double paramax = angle/360;
   string brep;
   Shape  shape (brep);
   Shapes t_shape;
   Edges  contour;
   t_shape.push_back (&shape);

   Real3 vk = { normal->getDx(), normal->getDy(), normal->getDz() };
   normer_vecteur (vk);

   for (int nz=0 ; nz<size_vz ; nz++)
       {
       for (int nx=0 ; nx<size_vx ; nx++)
           {
           Vertex* pm = getVertexIJK (nx, 0, nz); 
           Real3   om = { pm->getX() - ori->getX(), 
                          pm->getY() - ori->getY(), 
                          pm->getZ() - ori->getZ() };

           double oh     = prod_scalaire (om, vk);
           double rayon  = 0;
           Real3  ph, hm;
           for (int dd=dir_x; dd<=dir_z ; dd++)
               {
               ph [dd] = ori->getCoord(dd) + oh*vk[dd]; 
               hm [dd] = pm ->getCoord(dd) - ph[dd];
               rayon  += hm[dd] * hm[dd];
               }

           rayon = sqrt (rayon);
           geom_create_circle (ph, rayon, vk, hm, brep);
           shape.setBrep   (brep);
           shape.setBounds (0, paramax);

           contour.clear();
           for (int ny=0 ; ny<size_hy ; ny++)
               contour.push_back (getEdgeJ  (nx, ny, nz));

           cutAssociation (t_shape, contour, false);
           }
       }
   
   // Association automatique des vertex non associes -> bph
   // Traitement des faces spheriques

   Real3 vi = { -vk[dir_x],  -vk[dir_y],  -vk[dir_z] };
   Real3 po = { ori->getX(), ori->getY(), ori->getZ() };

   switch (grid_type)
      {
      case GR_HEMISPHERIC  :    // Pour l'exterieur
      case GR_PART_SPHERIC :
           assoRind (po, vi, t_shape, size_vx-1);
           break;
      case GR_PART_RIND    :    // Exterieur + interieur
      case GR_RIND         :
           assoRind (po, vi, t_shape, 0);
           assoRind (po, vi, t_shape, size_vx-1);
           break;
      default :
           return;
      }
}
// ====================================================== assoRind 
// Association des meridiennes
// Creation sphere geometrique + association faces
void Elements::assoRind (double* ori, double* vi, Shapes& t_shape, int nx)
{
   Real3 vk;
   Edges  contour;
   Shape* shape = t_shape[0];
   string brep;
   double paramin = (cyl_phi0 + M_PI/2) / (2*M_PI);
   double paramax = paramin + size_hz*cyl_dphi/(2*M_PI);
   paramin = std::max (paramin, 0.0);
   paramax = std::min (paramax, 1.0);
   int nz1 = size_vz/2;
   int nb_secteurs = cyl_closed ? size_vy-1 : size_vy;

   for (int ny=0 ; ny<nb_secteurs ; ny++)
       {
       Vertex* pm = getVertexIJK (nx, ny, nz1); 
       Real3   vj = { pm->getX(), pm->getY(), pm->getZ() };
       prod_vectoriel (vi, vj, vk);
       double rayon = cyl_radint + nx*(cyl_radext-cyl_radint)/size_hx;
       geom_create_circle (ori, rayon, vk, vi, brep);
       shape->setBrep   (brep);
       shape->setBounds (paramin, paramax);
       contour.clear ();
       for (int nz=0 ; nz<size_hz ; nz++)
           contour.push_back (getEdgeK  (nx, ny, nz));

       cutAssociation (t_shape, contour, false);
       }
}
// ====================================================== assoCircle 
void Elements::assoCircle (double* center, Edge* ed1, Edge* ed2)
{
   Real3 oa,  ob, normal;
   Real3 pta, ptb, ptc, ptd;
   string brep;

   Echo ("Elements::assoCircle");
   HexDump (ed1->getVertex (V_AMONT));
   HexDump (ed1->getVertex (V_AVAL ));
   HexDump (ed2->getVertex (V_AMONT));
   HexDump (ed2->getVertex (V_AVAL));

//  Les 2 edges dont les petits cotes d'un rectangle de rapport L/l=sqrt(2)
//  Soit le cercle circonscrit a ce rectangle. la largeur est balayee par l'angle alpha
//  la longueur par l'angle beta = pi -alpha

   ed1->getVertex(V_AMONT)->getPoint (pta);
   ed1->getVertex(V_AVAL )->getPoint (ptb);
   ed2->getVertex(V_AMONT)->getPoint (ptc);
   ed2->getVertex(V_AVAL )->getPoint (ptd);

   double d1 = calc_distance (pta, ptc);
   double d2 = calc_distance (pta, ptd);

   if (d1 < d2)
      {
      Echo ("Permutation");
      ed2->getVertex(V_AMONT)->getPoint (ptd);
      ed2->getVertex(V_AVAL )->getPoint (ptc);
      }

   calc_vecteur   (center, pta, oa);
   calc_vecteur   (center, ptb, ob);
   prod_vectoriel (oa, ob, normal);
   double rayon = calc_norme (oa);

   geom_create_circle (center, rayon, normal, oa, brep);

   Shape* asso1 = new Shape (brep);
   Shape* asso2 = new Shape (brep);

   const double alpha = atan (sqrt(2)/2)/M_PI; //  angle proche de 70.528 degres
   asso1->setBounds (0,   alpha);
   asso2->setBounds (0.5, alpha + 0.5);

   ed1->addAssociation (asso1);
   ed2->addAssociation (asso2);
}
// ====================================================== assoSphere 
void Elements::assoSphere (Vertex* ori, Edge* t_edge[], Quad* t_quad[])
{
   Real3 center, sommet;
   ori->getPoint(center);

   assoCircle (center, t_edge [E_AC], t_edge [E_BD]);
   assoCircle (center, t_edge [E_AD], t_edge [E_BC]);
   assoCircle (center, t_edge [E_AE], t_edge [E_BF]);
   assoCircle (center, t_edge [E_AF], t_edge [E_BE]);
   assoCircle (center, t_edge [E_CE], t_edge [E_DF]);
   assoCircle (center, t_edge [E_CF], t_edge [E_DE]);

   t_edge[E_AC]->getVertex(V_AMONT)->getPoint (sommet);
   double radius = calc_distance (center, sommet);;

   string brep;
   geom_create_sphere (center, radius, brep);

   FILE*    fic = fopen ("sphere.brep", "w");
   fprintf (fic, brep.c_str());
   fclose  (fic);

   for (int nf=0 ; nf < HQ_MAXI ; nf++)
       {
       Shape* shape = new Shape (brep);
       t_quad [nf]->addAssociation (shape);
       }
}
// ====================================================== makeSphericalGrid
int Elements::makeSphericalGrid (Vertex* c, double rayon, int nb, double  k)
{
   resize (GR_SPHERIC, nb);

   if (nb<0) 
      return HERR;
   else if (rayon <=ZEROR)
      return HERR;

   Vertex* i_node [HV_MAXI];    // Les noeuds de l'hexa englobant
   Edge*   i_edge [HE_MAXI];    // Les noeuds de l'hexa englobant
   Quad*   i_quad [HQ_MAXI];    // Les noeuds de l'hexa englobant

   for (int nro=0 ; nro<HV_MAXI; nro++)
       {
       double dx = glob->CoordVertex (nro, dir_x) * rayon;
       double dy = glob->CoordVertex (nro, dir_y) * rayon;
       double dz = glob->CoordVertex (nro, dir_z) * rayon;

       i_node [nro] = el_root->addVertex (c->getX ()+dx, c->getY ()+dy, 
                                          c->getZ ()+dz);
       }

   for (int nro=0 ; nro<HE_MAXI; nro++)
       {
       int v1 = glob->EdgeVertex (nro, V_AMONT);
       int v2 = glob->EdgeVertex (nro, V_AVAL);
       i_edge[nro] = newEdge (i_node[v1], i_node[v2]);

       if (db)
          {
          char nm0[8], nm1 [8], nm2 [8];
          printf (" %2d : %s = %s = [%s, %s] = [%d,%d] = [%s,%s]\n", nro, 
                 glob->namofHexaEdge(nro), i_edge[nro]->getName(nm0), 
                 glob->namofHexaVertex(v1), glob->namofHexaVertex(v2), v1, v2,
                 i_node[v1]->getName(nm1), i_node[v2]->getName(nm2));
          }
       }
        
   for (int nro=0 ; nro<HQ_MAXI; nro++)
       i_quad[nro] = newQuad (i_edge[glob->QuadEdge (nro, E_A)], 
                              i_edge[glob->QuadEdge (nro, E_B)], 
                              i_edge[glob->QuadEdge (nro, E_C)], 
                              i_edge[glob->QuadEdge (nro, E_D)]);

   tab_hexa.push_back (newHexa (i_quad[Q_A], i_quad[Q_B], i_quad[Q_C], 
                                i_quad[Q_D], i_quad[Q_E], i_quad[Q_F]));
   double lambda  = 1;
   double dcell   = 1;
   for (int niv=0; niv<gr_rayon ; niv++)
       {
       double lambda0 = lambda;
       dcell  *= k;
       lambda += dcell;
       addStrate (i_quad, i_edge, i_node, c,  lambda/lambda0);
       }
       
   assoSphere (c, i_edge, i_quad);
   return HOK;
}
// ====================================================== replaceHexas 
int Elements::replaceHexas (Hexas& pattern, Vertex* p1, Vertex* c1, 
                            Vertex* p2, Vertex* c2,  Vertex* p3, Vertex* c3)
{
    return HOK;
}
END_NAMESPACE_HEXA
