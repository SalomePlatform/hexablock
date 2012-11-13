
// C++ : Gestion des soous-shapes

// Copyright (C) 2009-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
// See http://www.salome-platform.org/ 
// or email : webmaster.salome@opencascade.com
//

#include "HexSubShape.hxx"
#include "HexNewShape.hxx"

#ifndef NO_CASCADE

#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <gp_Pnt.hxx>

BEGIN_NAMESPACE_HEXA

static bool db = on_debug ();  // == getenv ("HEXA_DB") > 0

// ====================================================== Constructeur
SubShape::SubShape (NewShape* dad, int id, int dim)
        : EltBase  (dad->dad(), EL_SUBSHAPE)
{
   ss_parent = dad;
   ss_ident  = id;
   ss_dim    = dim;

   maj_brep        = maj_shape        = maj_coords      = true;
   ss_coord[dir_x] = ss_coord[dir_y]  = ss_coord[dir_z] = 0;

                               // Les coordonnées
   if (ss_dim==0)
       updateCoords ();
}
// ====================================================== updateShape
void SubShape::updateShape ()
{
   maj_shape = false;
   geo_shape = ss_parent->getGeoShape (ss_ident);
}
// ====================================================== updateCoords
void SubShape::updateCoords ()
{
   if (ss_dim != 0)
       return;
   if (maj_shape) 
       updateShape ();

   maj_coords = false;
   TopoDS_Vertex gver = TopoDS::Vertex (geo_shape);
   gp_Pnt      gpoint = BRep_Tool::Pnt (gver);

   ss_coord [dir_x]   = gpoint.X();
   ss_coord [dir_y]   = gpoint.Y();
   ss_coord [dir_z]   = gpoint.Z();

   if (db) cout << " AddPoint = (" << gpoint.X() 
                << ","    << gpoint.Y() 
                << ","    << gpoint.Z() << ")\n";
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
END_NAMESPACE_HEXA
#endif
