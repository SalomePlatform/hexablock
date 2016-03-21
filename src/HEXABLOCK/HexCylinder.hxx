
// class : Les cylindres

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
#ifndef __CYLINDER_H_
#define __CYLINDER_H_

#include "Hex_defines.hxx"
#include "HexEltBase.hxx"

BEGIN_NAMESPACE_HEXA

class HEXABLOCKENGINE_EXPORT Cylinder : public EltBase
{
public :
    Vertex* getBase ()      { return c_base;   }
    Vector* getDirection () { return c_dir;    }
    double  getRadius ()    { return c_radius; }
    double  getHeight ()    { return c_height; }

    Cylinder (Vertex* b, Vector* v, double r, double h);
    virtual ~Cylinder () {}

    int interCylinder(Cylinder* s, bool& left, bool& right, double* sol);
    Vertex* interCylinder(Cylinder* other, bool& left, bool& right);
    void saveXml (XmlWriter* xml);
    virtual double  getInternalRadius () { return 0; }

protected:
    Vertex* c_base;
    Vector* c_dir;

    double c_radius;
    double c_height;
};

// ------------------------------------------- Inlining


END_NAMESPACE_HEXA
#endif
