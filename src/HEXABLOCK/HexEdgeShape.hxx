
// class : Gestion des Sous-shapes categorie Edge

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

#ifndef __LIBE_SHAPE_H_
#define __LIBE_SHAPE_H_

#include "HexSubShape.hxx"

BEGIN_NAMESPACE_HEXA

class HexaExport EdgeShape : public SubShape
{
public :
    EdgeShape (NewShape* dad, int id);
    virtual ~EdgeShape () {}

    void  addAssociation   (Edge* elt);
    int   countAssociation ()                { return tab_assoc.size(); }
    Edge* getAssociation   (int nro);

    BRepAdaptor_Curve* getCurve ();
    BRepAdaptor_Curve* makeCurve ();
    void    getCoords (double pstart[], double pend[]);
    int     getPoint  (double param,    double point[]);
    double  getParam  (double point[]);
    double  getLength ();
    double  getRadius ();
    double  getAngle  ();
    int     onExtremity (double point[]);
    bool    definedBy   (double p1[], double p2[]);
    bool    isLinear    ()                   { return kind_of == KS_Line ; }

    static bool samePoints (double point1[], double point2[]);
    virtual EnumKindOfShape kindOf ();

    // void   saveXml (XmlWriter* xml);
private :
    void updateCurve ();

private :
    BRepAdaptor_Curve* lin_curve;
    Real3  lin_start;
    Real3  lin_end;
    double lin_length, par_mini, par_maxi;
    double lin_radius, lin_angle;

    bool  maj_curve;
    Edges tab_assoc;
};
END_NAMESPACE_HEXA
#endif
