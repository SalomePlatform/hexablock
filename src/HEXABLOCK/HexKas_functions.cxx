//
// CC++ : Fonctions Interface Cascade
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

#ifndef NO_CASCADE

#include "HexKas_functions.hxx"

BEGIN_NAMESPACE_HEXA

// ====================================================== same_coords
bool same_coords (gp_Pnt& pa, gp_Pnt& pb, double epsilon)
{
   double d2 = carre (pb.X()-pa.X()) + carre (pb.Y()-pa.Y()) 
                                     + carre (pb.Z()-pa.Z()) ;
   return d2 < epsilon;
}
// ====================================================== save_brep
void save_brep (cpchar nom, string brep, int& compteur)
{
    char buff[8];
    compteur ++;
    sprintf (buff ,"%d", compteur);

    string name (nom);
    name += buff;
    name += ".brep";
    FILE*    fic = fopen (name.c_str(), "w");
    fprintf (fic, "%s\n", brep.c_str());
    fclose  (fic);
}
// ====================================================== geom_make_brep 
void geom_make_brep (TopoDS_Shape& shape, string& brep)
{
   ostringstream  stream_shape;
   BRepTools::Write (shape, stream_shape);
   brep = stream_shape.str();
}
END_NAMESPACE_HEXA
#endif
