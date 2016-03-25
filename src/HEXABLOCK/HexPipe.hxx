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

// class : Les tuyaux (de pipe ...)

#ifndef __PIPE_H
#define __PIPE_H

#include "Hex_defines.hxx"
#include "HexCylinder.hxx"

BEGIN_NAMESPACE_HEXA

class HEXABLOCKENGINE_EXPORT Pipe : public Cylinder 
{
public :
    virtual double  getInternalRadius () { return c_int_radius; }

public :
    Pipe (Vertex* b, Vector* v, double ri, double re, double h);
    virtual ~Pipe () {}
    void saveXml (XmlWriter* xml);

private:
    double c_int_radius;
};

// ------------------------------------------- Inlining

END_NAMESPACE_HEXA
#endif
