
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

#include <cmath>

// static bool db=false;

BEGIN_NAMESPACE_HEXA

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
   int ier = controlRind (center, vx, vz, radext, radint, radhole,
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
int Elements::controlRind (Vertex* cx, Vector* vx, Vector* vz, 
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

   if (nvx<Epsil1 || nvz <  Epsil1) 
      return HERR;

   double alpha = asin (rhole/rext);
   double beta  = -M_PI*DEMI;

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
// ====================================================== geom_create_circle 
void geom_create_circle (double* milieu, double rayon, Vector* normale, 
                         double* vx, string& brep)
{
   char buffer [80];
   sprintf (buffer, "(Cercle c=(%g,%g,%g), r=%g", 
                    milieu[0], milieu[1], milieu[2], rayon);
   brep = buffer;
}
// ====================================================== assoCylinder 
void Elements::assoCylinder (Vertex* ori, Vector* vz, double angle)
{
   return;           // Abu : Provisoire avant realisation 
   double dparam = (angle/360) / size_hy;
   Vector* vk = new Vector (vz);
   vk->renormer ();

   for (int nz=0 ; nz<size_vz ; nz++)
       {
       for (int nx=0 ; nx<size_vx ; nx++)
           {
           Vertex* pm = getVertexIJK (nx, 0, nz); 
           Real3   om = { pm->getX() - ori->getX(), 
                          pm->getY() - ori->getY(), 
                          pm->getZ() - ori->getZ() };

           double oh = om[dir_x]*vk->getDx() + om[dir_y]*vk->getDy() 
                     + om[dir_z]*vk->getDz() ;
           double rayon  = 0;
           Real3  ph, hm;
           for (int dd=dir_x; dd<=dir_z ; dd++)
               {
               ph [dd] = ori->getCoord(dd) + oh*vk->getCoord(dd), 
               hm [dd] = pm ->getCoord(dd) - ph[dd];
               rayon += hm[dd]*hm[dd];
               }

           rayon = sqrt (rayon);
           string brep;
           geom_create_circle (ph, rayon, vz, hm, brep);
           double param2 = 0;
           printf (" nz=%d, nx=%d, rayon=%g", nz, nx, rayon);
           printf (", centre=(%g,%g,%g)", ph[0],  ph[1],  ph[2]);
           printf (", hm=(%g,%g,%g) \n", hm[0],  hm[1],  hm[2]);

           for (int ny=0 ; ny<size_hy ; ny++)
               {
               double   param1 = param2;
               param2 = param1 + dparam;
               Edge*  edge  = getEdgeJ  (nx, ny, nz);
               Shape* shape = new Shape (brep);
               shape->setBounds (param1, param2);
               edge ->addAssociation (shape);
               printf (" .....       ny=%d, param2=%g\n", ny, param2);
               }
           }
       }
}
// ====================================================== replaceHexas 
int Elements::replaceHexas (Hexas& pattern, Vertex* p1, Vertex* c1, 
                            Vertex* p2, Vertex* c2,  Vertex* p3, Vertex* c3)
{
    return HOK;
}
END_NAMESPACE_HEXA
