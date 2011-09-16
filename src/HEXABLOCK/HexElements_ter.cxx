
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

#include "HexDocument.hxx"
#include "HexVector.hxx"
#include "HexVertex.hxx"
#include "HexHexa.hxx"
#include "HexEdge.hxx"

#include "HexGlobale.hxx"
#include "HexCylinder.hxx"

#include <map>

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
// ====================================================== makeHemiSpherical 
int Elements::makeHemiSpherical (int type, Vertex* center, double rayext, 
                                 double rayint, Vertex* plorig, Vector* plnorm,
                                 Cylinder* hole, Vector* vstart, double angle,
                                 int nr, int na, int nl)
{
/* ***********************************************
   Vertex* orig = cyl->getBase ();
   Vector* dir  = cyl->getDirection ();
   double  ray  = cyl->getRadius ();
   double  haut = cyl->getHeight ();
   
   resize (type, nr, na, nl);
   cyl_closed = false;
   makeCylindricalNodes (orig, vx, dir, ray, angle, haut, nr, na, nl, false);
   fillGrid ();
************************************************* */

   return HOK;
}
// ====================================================== getCylPoint 
int Elements::getCylPoint (int nr, int na, int nh, double& px, double& py,  
                           double& pz)
{
   double sinphi = sin (nh*cyl_dphi);
   double cosphi = cos (nh*cyl_dphi);

   double hmax = cyl_radext*cosphi;
   double hmin = cyl_radmin*hmax/cyl_radext;

   double rmax = cyl_radext*sinphi;
   double rmin = cyl_radmin;
 
   if (grid_type == GR_HALF_SHELL || grid_type == GR_8_SHELL)
      {
      rmin = std::max (cyl_radint*sinphi, cyl_radmin);
      hmin = cyl_radint*cosphi;
      }

   double rayon = rmin + nr*(rmax-rmin)/size_hx;
   
   px = rayon * cos (cyl_alpha + na*cyl_beta);
   py = rayon * sin (cyl_alpha + na*cyl_beta);
   py = rayon * cosphi;
   return HOK;
}
END_NAMESPACE_HEXA
