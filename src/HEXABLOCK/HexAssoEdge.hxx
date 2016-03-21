
// class : Gestion des formes associees (Hexa 5)

// Copyright (C) 2009-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef __ASSO_EDGE_H_
#define __ASSO_EDGE_H_

#include "HexEdgeShape.hxx"

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS_Shape.hxx>

BEGIN_NAMESPACE_HEXA

class HexaExport AssoEdge
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
