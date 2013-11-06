
// class : Gestion des formes associees (Hexa 5)

#ifndef __SUB_SHAPE_H_
#define __SUB_SHAPE_H_

#include "Hex_defines.hxx"
#include "HexEltBase.hxx"

#include <TopoDS_Shape.hxx>

BEGIN_NAMESPACE_HEXA

class HEXABLOCKENGINE_EXPORT SubShape : public EltBase
{
public :
    SubShape (NewShape* dad, int id, int dim);
    NewShape*       getParentShape() const  { return ss_parent; }
    cpchar          getParentName () const;
    int             getIdent ()             { return sub_ident ; }
    int             getDim   ()             { return sub_dim ; }
    EnumKindOfShape kindOf   ()             { return kind_of ; }

    const string&       getBrep  ();
    virtual const TopoDS_Shape& getShape ();

    void   saveXml (XmlWriter* xml);
    void   callXml (XmlWriter* xml);

protected :
    void updateShape  ();
    void updateBrep   ();

protected :
    NewShape* ss_parent;
    int       sub_ident;
    int       sub_dim;     // 0 = point, 1 = arete, 2 = face;

    TopoDS_Shape    geo_shape;
    string          geo_brep;
    bool            maj_brep, maj_shape;
    EnumKindOfShape kind_of;
};
END_NAMESPACE_HEXA
#endif
