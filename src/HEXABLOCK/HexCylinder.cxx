
// C++ : Gestion des cylindres

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
//
#include "HexCylinder.hxx"
#include "HexVertex.hxx"
#include "HexVector.hxx"
#include "HexXmlWriter.hxx"

#include <cmath>

BEGIN_NAMESPACE_HEXA

bool is_out (double val, double orig, double lg);

// ======================================================== Constructeur
Cylinder::Cylinder(Vertex* b, Vector* v, double r, double h)
        : EltBase (b->dad())
{
    c_base   = b;
    c_dir    = v;
    c_radius = r;
    c_height = h;
}
// ======================================================== rdiffers
bool rdiffers (double v1, double v2)
{
   double dd = v1-v2;
   return dd*dd > 1e-20;
}
// ======================================================== norme
double norme (double px, double py, double pz)
{
   double res = sqrt (px*px + py*py + pz*pz);
   return res;
}
// ======================================================== interCylinder
///  Intersection de 2 droites (c1,v1) et (c2,v2)
///  Le point M(x,yz) est solutions de 2 equations :
///            (c1,m) = k * v1 
///            (c2,m) vectoriel v2 = 0
///            
///  x = xc1 + k * xv1                  (1)
///  y = yc1 + k * yv1                  (2)
///  z = zc1 + k * zv1                  (3)
///  (x-xc2) * yv2 - (y-yc2) * xv2 = 0  (4)
//  (z-zc2) * yv2 - (y-yc2) * zv2 = 0  (5)
///  (x-xc2) * zv2 - (z-zc2) * xv2 = 0  (6)
///             
///  En substituant les (x,y,z)
///             
///  (k*xv1+xc1-xc2) * yv2 - (k*yv1+yc1-yc2) * xv2 = 0  (4)
///  (k*zv1+zc1-zc2) * yv2 - (k*yv1+yc1-yc2) * zv2 = 0  (5)
///  (k*xv1+xc1-xc2) * zv2 - (k*zv1+zc1-zc2) * xv2 = 0  (6)
///             
///  k * (xv1*yv2 - yv1*xv2) + (xc1-xc2) * yv2 - (yc1-yc2) * xv2 = 0 (4)
///  k * dxy - nxy = 0      (4)
///  k * dzy - nzy = 0      (5)
///  k * dxz - nxz = 0      (6)
///
///  nxy = (xc2-xc1) * yv2 - (yc2-yc1) * xv2
///  dxy =  xv1*yv2 - yv1*xv2
///             
// ======================================================== interCylinder
Vertex* Cylinder::interCylinder (Cylinder* small, bool& left, bool& right)
{
    left = right = true;
    c_base->dump ();
    c_dir-> dump ();

    small->c_base->dump ();
    small->c_dir-> dump ();

    double xc1 = c_base->getX(); 
    double yc1 = c_base->getY(); 
    double zc1 = c_base->getZ(); 

    double xv1 = c_dir->getDx(); 
    double yv1 = c_dir->getDy(); 
    double zv1 = c_dir->getDz(); 

    double xc2 = small->c_base->getX(); 
    double yc2 = small->c_base->getY(); 
    double zc2 = small->c_base->getZ(); 

    double xv2 = small->c_dir->getDx(); 
    double yv2 = small->c_dir->getDy(); 
    double zv2 = small->c_dir->getDz(); 

    double nxyz [DIM3] = { (xc2-xc1) * yv2 - (yc2-yc1) * xv2,
                           (zc2-zc1) * yv2 - (yc2-yc1) * zv2,
                           (xc2-xc1) * zv2 - (zc2-zc1) * xv2 };

    double dxyz [DIM3] = { xv1*yv2- yv1*xv2, 
                           zv1*yv2- yv1*zv2,
                           xv1*zv2- zv1*xv2 };
    double lambda = 0;
    bool   prems  = true;
    for (int dd=0 ; dd<DIM3 ; dd++)
        {
                                      // dxyz!=0 : calcul de lambda
        if (rdiffers (dxyz [dd], ZEROR)) 
           {
           double kk = nxyz [dd] / dxyz[dd];
                                      // 1er lambda trouve 
           if (prems)
              {
              prems  = false;
              lambda = kk;
              }
                                      // Solutions incompatibles 
	   else if (rdiffers (kk, lambda))
              return NULL;
                                      // equation O*lamda = 0 : ignoree
	// else ....
           }
                                      // dxyz=0 et nxyz!=0 : Pas de solution
	else if (rdiffers (nxyz [dd], ZEROR)) 
           return NULL;
        }

                                      // Aucune solution trouvee
    if (prems)
       {
       HexDisplay (prems);
       return NULL;
       }
                                    // Intersection des droites
    double px = xc1 + lambda * xv1;
    double py = yc1 + lambda * yv1;
    double pz = zc1 + lambda * zv1;
                                    // Appartenance axe 1er cylindre
    double lg = c_dir->getNorm ();
    if (lambda<0 || lambda*lg > c_height)
       return NULL;
                                    // Appartenance axe 2eme cylindre
    lg = small->c_height / small->c_dir->getNorm ();
    if (is_out (px, xc2, xc2+xv2*lg ) || is_out (py, yc2, xc2+yv2*lg ) 
                                      || is_out (pz, zc2, zc2+zv2*lg ))
        return NULL;
                                    // Verif % rayons 
    double lg11 = norme (px-xc1, py-yc1, pz-zc1);
    double lg12 = c_height - lg11;

    double lg21 = norme (px-xc2, py-yc2, pz-zc2);
    double lg22 = small->c_height - lg21;

    if (lg11 < small->c_radius || lg12 < small->c_radius)
       return NULL;
    if (lg21 < c_radius        && lg22 < c_radius) 
       return NULL;

    left  = lg21 >= small->c_radius;
    right = lg22 >= small->c_radius;

    HexDisplay (left);
    HexDisplay (right);
    Vertex* sol = new Vertex (c_base->dad(), px, py, pz);
    return  sol;
}
// ======================================================== is_out
bool is_out (double val, double v1, double v2)
{
   if (v1 < v2)
      return (val < v1 || val > v2) ;

   else if (v1 > v2) 
      return (val < v2 || val > v1) ;

   else 
      return val != v1;
}
// ========================================================= saveXml 
void Cylinder::saveXml  (XmlWriter* xml)
{
   char buffer[12];

   xml->openMark     ("Cylinder");
   xml->addAttribute ("c_base", c_base->getName (buffer));
   xml->addAttribute ("c_dir",  c_dir->getName  (buffer));
   xml->addAttribute ("c_radius", c_radius);
   xml->addAttribute ("c_height", c_height);
   xml->closeMark ();
}
END_NAMESPACE_HEXA

