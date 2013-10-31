
// class : Gestion des formes associees (Hexa 5)

#ifndef __ASSO_EDGE_H_
#define __ASSO_EDGE_H_

#include "Hex_defines.hxx"
#include "HexEdgeShape.hxx"

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS_Shape.hxx>

BEGIN_NAMESPACE_HEXA

class HEXABLOCKENGINE_EXPORT AssoEdge
{
public :
    AssoEdge (EdgeShape* shape, double deb, double fin);
    virtual ~AssoEdge ()         {}

    EdgeShape*         getEdgeShape() { return arc_line;  }
    BRepAdaptor_Curve* getCurve ()    { return arc_curve; }
    double             getStart ()    { return arc_para1; }
    double             getEnd   ()    { return arc_para2; }
    double             length   ();
    double             getUstart ();                       // for Mesh Plugin


    const double*      getOrigin ()   { return arc_start; }
    const double*      getExtrem ()   { return arc_end;   }

    int onExtremity (AssoEdge* other, int& ext_other);
    int onExtremity (double* point);

    virtual void dump ();
    void saveXml (XmlWriter* xml);

    void setStart (double param);
    void setEnd   (double param);

private :
    bool       maj_extrem;
    double     arc_para1, arc_para2;
    EdgeShape* arc_line;

    bool    arc_reverse;
    Real3   arc_start,  arc_end;
    BRepAdaptor_Curve*  arc_curve;  // Duplication a cause du plugin BP
};
END_NAMESPACE_HEXA
#endif
