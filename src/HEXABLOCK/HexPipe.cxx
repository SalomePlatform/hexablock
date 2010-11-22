
// C++ : Gestion des tuyaux

#include "HexPipe.hxx"

BEGIN_NAMESPACE_HEXA

// ======================================================== Constructeur
Pipe::Pipe (Vertex* b, Vector* v, double ri, double re, double h)
    : Cylinder (b, v, re, h)
{
    c_int_radius = ri;
}

END_NAMESPACE_HEXA
