
// class : Gestion des Sous-shapes categorie Faces

#ifndef __FACE_SHAPE_H_
#define __FACE_SHAPE_H_

#include "HexSubShape.hxx"

BEGIN_NAMESPACE_HEXA

class FaceShape : public SubShape
{
public :
    FaceShape (NewShape* dad, int id);

    void  addAssociation   (Quad* elt);
    int   countAssociation ()                { return tab_assoc.size(); }
    Quad* getAssociation   (int nro);

    // void   saveXml (XmlWriter* xml);
private :
    Quads tab_assoc;
};
END_NAMESPACE_HEXA
#endif
