
// class : Les vecteurs

#ifndef __VECTOR_H
#define __VECTOR_H

#include "HexEltBase.hxx"

#include <cmath>

BEGIN_NAMESPACE_HEXA

class Vector : public EltBase
{
public:
    double getDx ();
    double getDy ();
    double getDz ();

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
inline double Vector::getDx() { return v_dx; }
inline double Vector::getDy() { return v_dy; }
inline double Vector::getDz() { return v_dz; }

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
