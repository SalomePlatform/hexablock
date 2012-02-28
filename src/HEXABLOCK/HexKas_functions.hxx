//
// CC++ : Representation Cascade d'un point
//
//  Copyright (C) 2009-2011  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ 
//  or email : webmaster.salome@opencascade.com
//
//--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8

#ifndef _KAS_FUNCTIONS_HXX
#define _KAS_FUNCTIONS_HXX

#ifndef NO_CASCADE

#include "HexGlobale.hxx"

#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>

BEGIN_NAMESPACE_HEXA

int geom_create_cylcyl (double* borig, double* bnorm, double* bbase,
                        double  bray,  double  bhaut,
                        double* sorig, double* snorm, double* sbase,
                        double  sray,  double  shaut);
int geom_asso_cylcyl (Edge* edge);

bool same_coords (gp_Pnt& pa, gp_Pnt& pb);
void save_brep   (cpchar nom, string brep, int& nro);
void geom_make_brep (TopoDS_Shape& shape, string& brep);

END_NAMESPACE_HEXA
#endif
#else             // NO_CASCADE
#endif            // NO_CASCADE
