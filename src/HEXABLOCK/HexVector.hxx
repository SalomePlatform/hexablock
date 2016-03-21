
// class : Les vecteurs

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
#ifndef __VECTOR_H
#define __VECTOR_H

#include "HexEltBase.hxx"

#include <cmath>

BEGIN_NAMESPACE_HEXA

class HexaExport Vector : public EltBase
{
public:
    double getDx ()    { return v_dx ; }
    double getDy ()    { return v_dy ; }
    double getDz ()    { return v_dz ; }
    double getNorm   ()    { return sqrt (v_dx*v_dx + v_dy*v_dy + v_dz*v_dz); }
    double getAngleX ();

    Vector (Document* doc, double dx=0, double dy=0, double dz=0);
#ifndef SWIG
public:
    Vector (Vector* lautre);
    virtual ~Vector () {}
    virtual void dump ();
    double  getCoord (int dir);
    double* getCoord (double coord[]);
    int     getUnitVector (double coord[]);

    virtual void saveXml (XmlWriter* xml);
    int    renormer  ();
    void   vectoriel  (Vector* a, Vector*b);
    void   multiplier (double scal);
    int    calculNormale (double& nx, double& ny, double& nz);

private:
    double v_dx;
    double v_dy;
    double v_dz;
#endif
};
END_NAMESPACE_HEXA
#endif
