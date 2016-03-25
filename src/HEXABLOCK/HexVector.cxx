
// C++ : Gestion des vecteurs

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
#include "HexVector.hxx"
#include "HexXmlWriter.hxx"

BEGIN_NAMESPACE_HEXA

// ======================================================== Constructeur
Vector::Vector (Document* doc, double dx, double dy, double dz)
      : EltBase (doc, EL_VECTOR)
{
    v_dx = dx;
    v_dy = dy;
    v_dz = dz;
}
// ======================================================== Constructeur bis
Vector::Vector (Vector* lautre)
      : EltBase (lautre->el_root)
{
    v_dx = lautre -> v_dx;
    v_dy = lautre -> v_dy;
    v_dz = lautre -> v_dz;
}

// ======================================================== calculNormale
// === Calcule un vecteur orthogonal et norme
int Vector::calculNormale (double& nx, double& ny, double& nz)
{
    double epsil = (v_dx * v_dx + v_dy * v_dy + v_dz * v_dz)*1e-6;
    int    forme = 2000;
    if  (v_dx > epsil || v_dx < -epsil) forme += 100;
    if  (v_dy > epsil || v_dy < -epsil) forme +=  10;
    if  (v_dz > epsil || v_dz < -epsil) forme +=   1;

    nx = ny = nz = 0;
    switch (forme)
       {
                             // dx=dy=dz=0 : retour vecteur i + erreur
       case 2000 : nx = 1; 
                   return HERR;
                             // dx = 0 : on prend le vecteur i
       case 2001 :  
       case 2010 :  
       case 2011 : nx = 1;
                   return HOK;  
                             // dy = 0 : on prend le vecteur j
       case 2100 :  
       case 2101 : ny = 1; 
                   return HOK;  
                             // dz = 0 : on prend le vecteur k
       case 2110 : nz = 1;
                   return HOK;  
                             // dx, dy, dy non nuls : cas general;
       default :;
       }

   double norme = sqrt (v_dx*v_dx + v_dy*v_dy);
   nx =  v_dy / norme;
   ny = -v_dx / norme;
   return HOK;  
}
// ========================================================= saveXml 
void Vector::saveXml (XmlWriter* xml)
{
   char buffer[12], coord[80];

   sprintf (coord, "%g %g %g", v_dx, v_dy, v_dz);

   xml->openMark     ("Vector");
   xml->addAttribute ("id",    getName (buffer));
   xml->addAttribute ("coord", coord);
   if (el_name!=buffer) 
       xml->addAttribute ("name", el_name);
   xml->closeMark ();
}
// ========================================================= getAngleX
double Vector::getAngleX ()
{
   static const double Epsilon = 1e-6;

   double norme = getNorm ();
   if (norme < Epsilon)
      return 0.0;

   double kos = v_dx / norme;
   double deg = acos(kos) * 180.0 / M_PI;
   return deg;
}
// ===================================================== getCoord
double Vector::getCoord (int dir) 
{
   double val = 0;
   switch (dir)
          {
          case dir_x : val = v_dx; 
               break;
          case dir_y : val = v_dy; 
               break;
          case dir_z : val = v_dz; 
               break;
          }
   return val;
}
// ===================================================== renormer
int Vector::renormer() 
{
   double dn = getNorm ();
   if (dn < 1e-30) 
      return HERR;

   v_dx /= dn;
   v_dy /= dn;
   v_dz /= dn;

   return HOK;
}
// ===================================================== vectoriel
void Vector::vectoriel (Vector* a, Vector*b)
{
   v_dx =   a->v_dy * b->v_dz - b->v_dy * a->v_dz; 
   v_dy =   a->v_dz * b->v_dx - b->v_dz * a->v_dx; 
   v_dz =   a->v_dx * b->v_dy - b->v_dx * a->v_dy; 
}

// ===================================================== getCoord
double* Vector::getCoord (double coord[])
{
   coord [dir_x] = v_dx;
   coord [dir_y] = v_dy;
   coord [dir_z] = v_dz;
   return coord;
}
// ===================================================== getUnitVector
int Vector::getUnitVector (double coord[])
{
   coord [dir_x] = v_dx;
   coord [dir_y] = v_dy;
   coord [dir_z] = v_dz;
   int ier  = normer_vecteur (coord);
   return ier;
}
// ===================================================== multiplier
void Vector::multiplier (double scalaire)
{
   v_dx *= scalaire;
   v_dy *= scalaire;
   v_dz *= scalaire;
}
// ===================================================== dump
void Vector::dump ()
{
   printf ("Vector ");
   printName (" = ");
   if (NOT isHere ())
      {
      printf ("(*** deleted ***)\n");
      return;
      }

   printf ("(%g, %g, %g)\n", v_dx,v_dy,v_dz);
}

END_NAMESPACE_HEXA
