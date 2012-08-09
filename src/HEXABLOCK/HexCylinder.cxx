
// C++ : Gestion des cylindres

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

    if (el_root->debug ())
       {
       Echo ("---------------------------------- AddCylinder");
       HexDisplay (c_radius);
       HexDisplay (c_height);
       HexDump (c_base);
       HexDump (c_dir);
       cout << endl;
       }
}
// ======================================================== rdiffers
bool rdiffers (double v1, double v2)
{
   double dd = v1-v2;
   return dd*dd > 1e-12;
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
///  (z-zc2) * yv2 - (y-yc2) * zv2 = 0  (5)
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
   Real3 orig;
   int ier = interCylinder (small, left, right, orig);
   if (ier!=HOK)
       return NULL;

    Vertex* sol = new Vertex (c_base->dad(), orig[0],  orig[1],  orig[2]);
    return sol;
}
///             
// ======================================================== interCylinder
int Cylinder::interCylinder (Cylinder* small, bool& left, bool& right, 
                             double* orig)
{
    left = right = true;
    if (el_root->debug ())
       {
       Echo ("---------------------------------- interCylinders");
       PutData (this->c_radius);
       PutData (small->c_radius);
       PutData (this->c_height);
       PutData (small->c_height);
       HexDump (this->c_base);
       HexDump (small->c_base);
       HexDump (this->c_dir);
       HexDump (small->c_dir);
       }

    double norm1 = c_dir->getNorm(); 
    double norm2 = small->c_dir->getNorm(); 

    double xc1 = c_base->getX(); 
    double yc1 = c_base->getY(); 
    double zc1 = c_base->getZ(); 

    double xc2 = small->c_base->getX(); 
    double yc2 = small->c_base->getY(); 
    double zc2 = small->c_base->getZ(); 

    double xv1 = c_dir->getDx()/norm1; 
    double yv1 = c_dir->getDy()/norm1; 
    double zv1 = c_dir->getDz()/norm1; 

    double xv2 = small->c_dir->getDx()/norm2; 
    double yv2 = small->c_dir->getDy()/norm2; 
    double zv2 = small->c_dir->getDz()/norm2; 

    double nxyz [DIM3] = { (xc2-xc1) * yv2 - (yc2-yc1) * xv2,
                           (zc2-zc1) * yv2 - (yc2-yc1) * zv2,
                           (xc2-xc1) * zv2 - (zc2-zc1) * xv2 };

    double dxyz [DIM3] = { xv1*yv2 - yv1*xv2, 
                           zv1*yv2 - yv1*zv2,
                           xv1*zv2 - zv1*xv2 };
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
              {
              cout << "*** InterCylinders : Solutions incompatibles " << endl;
              return HERR;
              }
                                      // equation O*lamda = 0 : ignoree
	// else ....
           }
                                      // dxyz=0 et nxyz!=0 : Pas de solution
	else if (rdiffers (nxyz [dd], ZEROR)) 
           {
           cout << "*** InterCylinders : intersection vide " << endl;
           return HERR;
           }
        }

                                      // Aucune solution trouvee
    if (prems)
       {
       HexDisplay (prems);
       cout << "*** InterCylinders : intersection non trouvee " << endl;
       return HERR;
       }
                                    // Intersection des droites
    orig [dir_x] = xc1 + lambda*xv1;
    orig [dir_y] = yc1 + lambda*yv1;
    orig [dir_z] = zc1 + lambda*zv1;

    Real3 base1, dir1, extr1;
    Real3 base2, dir2, extr2;

    this ->c_base->getPoint (base1);
    small->c_base->getPoint (base2);

    this ->c_dir->getCoord (dir1);
    small->c_dir->getCoord (dir2);

    for (int dd=dir_x ; dd<=dir_z ; dd++)
        {
        extr1 [dd] = base1[dd] + dir1[dd]*this ->c_height/norm1;
        extr2 [dd] = base2[dd] + dir2[dd]*small->c_height/norm2;
        }

    double dbase1 = calc_distance (orig, base1);
    double dbase2 = calc_distance (orig, base2);
    double dextr1 = calc_distance (orig, extr1);
    double dextr2 = calc_distance (orig, extr2);

    double dmin2  = std::min (dbase2, dextr2);
    double dmax2  = std::max (dbase2, dextr2);
    double dmax1  = std::max (dbase1, dextr1);
    double coeff  = 1.1;
                                    // Appartenance axe petit cylindre
    if (dmax2 > small->c_height + c_radius)
       {
       cout << "*** InterCylinders : Petit cylindre trop court" << endl;
       cout << "*** Intersection = (" << orig[0] << ", "  << orig[1] 
                             << ", "  << orig[2] << " )" << endl;
       cout << "*** distance = "  << dmin2 
            << " > gros rayon + longueur " 
            << small->c_height + c_radius << endl;
       cout << endl;
       PutCoord (base2);
       PutCoord (extr2);
       PutData(dbase2);
       PutData(dextr2);
       PutData(dextr2);
       return HERR;
       }

    left  = dbase2 > c_radius*coeff;
    right = dextr2 > c_radius*coeff;
                                    // Le gros cylindre doit depasser le petit
    if (dmax1 > c_height-coeff*small->c_radius)
       {
       cout << "*** InterCylinders : Gros cylindre trop court" << endl;
       cout << "*** Intersection = (" << orig[0] << ", "  << orig[1] 
                             << ", "  << orig[2] << " )" << endl;
       cout << "*** distance maximale = " << dmax1 << endl;
       cout << endl;
       return HERR;
       }

    if (el_root->debug ())
       {
       PutCoord   (orig);
       HexDisplay (left);
       HexDisplay (right);
       }
    return  HOK;
}
// ======================================================== is_out
bool is_out (double val, double v1, double v2)
{
   if (v1 < v2)
      return (val < v1-Epsil || val > v2+Epsil) ;

   else if (v1 > v2) 
      return (val < v2-Epsil || val > v1+Epsil) ;

   else 
      return fabs (val - v1)>Epsil;
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

