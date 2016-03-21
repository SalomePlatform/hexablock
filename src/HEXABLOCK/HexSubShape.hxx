
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

#ifndef __SUB_SHAPE_H_
#define __SUB_SHAPE_H_

#include "HexEltBase.hxx"

#include <TopoDS_Shape.hxx>

BEGIN_NAMESPACE_HEXA

class HexaExport SubShape : public EltBase
{
public :
    SubShape (NewShape* dad, int id, int dim);
    NewShape*   getParentShape() const  { return ss_parent; }
    cpchar      getParentName () const;
    int         getIdent ()             { return sub_ident ; }
    int         getDim   ()             { return sub_dim ; }
    virtual EnumKindOfShape kindOf ()   { return kind_of ; }

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
