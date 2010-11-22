
// class : Les cylindres

#ifndef __CYLINDER_H_
#define __CYLINDER_H_

#include "HexEltBase.hxx"

BEGIN_NAMESPACE_HEXA

class Cylinder : public EltBase
{
public :
    Vertex* getBase ()      { return c_base;   }
    Vector* getDirection () { return c_dir;    }
    double  getRadius ()    { return c_radius; }
    double  getHeight ()    { return c_height; }

public :
    Cylinder (Vertex* b, Vector* v, double r, double h);
    Vertex* interCylinder (Cylinder* other);

protected:
    Vertex* c_base;
    Vector* c_dir;

    double c_radius;
    double c_height;
};

// ------------------------------------------- Inlining


END_NAMESPACE_HEXA
#endif
