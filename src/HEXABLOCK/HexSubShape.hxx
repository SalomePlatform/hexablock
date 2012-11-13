
// class : Gestion des formes associees (Hexa 5)

#ifndef __SUB_SHAPE_H_
#define __SUB_SHAPE_H_



#include "HexEltBase.hxx"

#ifndef NO_CASCADE
#include <TopoDS_Shape.hxx>
#endif

BEGIN_NAMESPACE_HEXA

class SubShape : public EltBase 
{
public :
    SubShape (NewShape* dad, int id, int dim);
    string getBrep ();

    // void   saveXml (XmlWriter* xml);

private :
    void updateCoords ();
    void updateShape  ();
    void updateBrep   ();

private :
    NewShape* ss_parent;
    int       ss_ident;
    int       ss_dim;     // 0 = point, 1 = arete, 2 = face;
    Real3     ss_coord;   // Pour les vertex

    TopoDS_Shape geo_shape;
    string       geo_brep;
    bool         maj_brep, maj_shape, maj_coords;
};
// _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ 
class Association 
{
public :
    string getBrep ();
private :
    double    as_start;
    double    as_end;
    SubShape* as_shape;
};
END_NAMESPACE_HEXA
#endif
