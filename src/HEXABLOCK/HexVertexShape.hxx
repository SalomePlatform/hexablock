
// class : Gestion des Sous-shapes categorie Vertex

#ifndef __NODE_SHAPE_H_
#define __NODE_SHAPE_H_

#include "HexSubShape.hxx"

BEGIN_NAMESPACE_HEXA

class VertexShape : public SubShape
{
public :
    VertexShape (NewShape* dad, int id);
    VertexShape (NewShape* dad, int id, double* point);

    void    addAssociation   (Vertex* elt);
    int     countAssociation ()                { return tab_assoc.size(); }
    Vertex* getAssociation   (int nro);

    void getCoords (double& px, double& py, double& pz);
    void saveXml (XmlWriter* xml);

private :
    void updateCoords ();

private :
    Real3  ss_coord;   // Pour les vertex
    bool   maj_coords;
    bool   free_point;
    vector <Vertex*> tab_assoc;
};
END_NAMESPACE_HEXA
#endif
