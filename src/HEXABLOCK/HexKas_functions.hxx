//
// CC++ : Representation Cascade d'un point
//
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

#ifndef _KAS_FUNCTIONS_HXX
#define _KAS_FUNCTIONS_HXX

class gp_Pnt;

#include "HexGlobale.hxx"

BEGIN_NAMESPACE_HEXA

int HexaExport geom_create_cylcyl (double* borig, double* bnorm, double* bbase,
                        double  bray,  double  bhaut,
                        double* sorig, double* snorm, double* sbase,
                        double  sray,  double  shaut);
int HexaExport geom_asso_cylcyl (Edge* edge);

bool HexaExport same_coords (gp_Pnt& pa, gp_Pnt& pb, double epsil2=1e-6);
void HexaExport save_brep   (cpchar nom, string brep, int& nro);
void HexaExport clean_brep  (string& brep);

void HexaExport geom_make_brep  (TopoDS_Shape& shape, string& brep);
int  HexaExport geom_brep2point (rcstring brep, double& px, double& py, double& pz);
int  HexaExport geom_brep2shape (rcstring brep, TopoDS_Shape& shape);

TopoDS_Shape HexaExport geom_brep2shape (rcstring brep);

END_NAMESPACE_HEXA
#endif
