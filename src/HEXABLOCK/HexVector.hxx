
// class : Les vecteurs

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
#ifndef __VECTOR_H
#define __VECTOR_H

#include "HexEltBase.hxx"

#include <cmath>

BEGIN_NAMESPACE_HEXA

class Vector : public EltBase
{
public:
    double getDx ()    { return v_dx ; }
    double getDy ()    { return v_dy ; }
    double getDz ()    { return v_dz ; }
    double getCoord (int dir);

public:
    Vector (Document* doc, double dx=0, double dy=0, double dz=0);
    Vector (Vector* lautre);
    virtual ~Vector () {}
    virtual void dump ();

    double norme   ()    { return sqrt (v_dx*v_dx + v_dy*v_dy + v_dz*v_dz); }
    int    renormer ();
    void   vectoriel  (Vector* a, Vector*b);
    void   multiplier (double scal);
    int    calculNormale (double& nx, double& ny, double& nz);

private:
    double v_dx;
    double v_dy;
    double v_dz;
};
// ------------------------------------------- Inlining
// ===================================================== getCoord
inline double Vector::getCoord (int dir) 
{
   double val = 0;
   switch (dir)
          {
          case dir_x : val = v_dx; 
               break;
          case dir_y : val = v_dz; 
               break;
          case dir_z : val = v_dz; 
               break;
          }
   return val;
}
// ===================================================== renormer
inline int Vector::renormer() 
{
   double dn = norme ();
   if (dn < 1e-30) 
      return HERR;

   v_dx /= dn;
   v_dy /= dn;
   v_dz /= dn;

   return HOK;
}
// ===================================================== vectoriel
inline void Vector::vectoriel (Vector* a, Vector*b)
{
   v_dx =   a->v_dy * b->v_dz - b->v_dy * a->v_dz; 
   v_dy =   a->v_dz * b->v_dx - b->v_dz * a->v_dx; 
   v_dz =   a->v_dx * b->v_dy - b->v_dx * a->v_dy; 
}

// ===================================================== multiplier
inline void Vector::multiplier (double scalaire)
{
   v_dx *= scalaire;
   v_dy *= scalaire;
   v_dz *= scalaire;
}
// ===================================================== dump
inline void Vector::dump ()
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
#endif
