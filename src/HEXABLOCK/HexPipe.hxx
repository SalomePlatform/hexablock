
// class : Les tuyaux (de pipe ...)

#ifndef __PIPE_H
#define __PIPE_H

#include "HexCylinder.hxx"

BEGIN_NAMESPACE_HEXA

class Pipe : public Cylinder 
{
public :
    double  getInternalRadius () { return c_int_radius; }

public :
    Pipe (Vertex* b, Vector* v, double ri, double re, double h);

private:
    double c_int_radius;
};

// ------------------------------------------- Inlining

END_NAMESPACE_HEXA
#endif
