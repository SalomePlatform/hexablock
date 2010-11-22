
// Class : Implementation provisoire et symbolique de la classe "shape"
//

// #ifdef  _NO_CASCADE 
#ifndef __SHAPE_H_
#define __SHAPE_H_

#include "hexa_base.hxx"

BEGIN_NAMESPACE_HEXA

class Shape 
{
public :
    Shape (string brep);
    string getBrep ()  { return b_rep; }

public :
    double debut;   
    double fin;   

public :
    // char* getName (char* buffer);

private :
    // int nro_shape;
    // static int nbr_shapes;
    std::string b_rep;
};

typedef Shape* TopoDS_Shape;

END_NAMESPACE_HEXA
#endif
// #endif
