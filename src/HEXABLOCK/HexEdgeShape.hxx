
// class : Gestion des Sous-shapes categorie Edge

#ifndef __LIBE_SHAPE_H_
#define __LIBE_SHAPE_H_

#include "HexSubShape.hxx"

BEGIN_NAMESPACE_HEXA

class EdgeShape : public SubShape
{
public :
    EdgeShape (NewShape* dad, int id);

    void  addAssociation   (Edge* elt);
    int   countAssociation ()                { return tab_assoc.size(); }
    Edge* getAssociation   (int nro);

    BRepAdaptor_Curve* getCurve ();
    BRepAdaptor_Curve* makeCurve ();
    void    getCoords (double pstart[], double pend[]);
    int     getPoint  (double param,    double point[]);
    double  getParam  (double point[]);
    double  getLength ();
    int     onExtremity (double point[]);

    static bool samePoints (double point1[], double point2[]);

    // void   saveXml (XmlWriter* xml);
private :
    void updateCurve ();

private :
    BRepAdaptor_Curve* lin_curve;
    Real3  lin_start;
    Real3  lin_end;
    double lin_length, par_mini, par_maxi;

    bool  maj_curve;
    Edges tab_assoc;
};
END_NAMESPACE_HEXA
#endif
