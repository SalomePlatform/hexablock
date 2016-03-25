
// C++ : Gestion des soous-shapes

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

#include "HexVertexShape.hxx"
#include "HexNewShape.hxx"
#include "HexXmlWriter.hxx"

#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <gp_Pnt.hxx>

BEGIN_NAMESPACE_HEXA

// static bool db = on_debug ();  // == getenv ("HEXA_DB") > 0

// ====================================================== Constructeur
VertexShape::VertexShape (NewShape* dad, int id)
         : SubShape  (dad, id, 0)
{
   free_point      = false;
   maj_coords      = true;
   ss_coord[dir_x] = ss_coord[dir_y]  = ss_coord[dir_z] = 0;

                               // Les coordonnées
   updateCoords ();
}
// ====================================================== Constructeur bis*
VertexShape::VertexShape (NewShape* dad, int id, double* point)
         : SubShape  (dad, id, 0)
{
   free_point      = true;
   maj_coords      = false;
   maj_shape       = false;
   for (int nc=dir_x ; nc<=dir_z ; ++nc)
        ss_coord[nc] = point[nc];
}
// ====================================================== updateCoords
void VertexShape::updateCoords ()
{
   if (free_point)
      return;
   if (maj_shape)
       updateShape ();

   maj_coords = false;
   TopoDS_Vertex gver = TopoDS::Vertex (geo_shape);
   gp_Pnt      gpoint = BRep_Tool::Pnt (gver);

   ss_coord [dir_x]   = gpoint.X();
   ss_coord [dir_y]   = gpoint.Y();
   ss_coord [dir_z]   = gpoint.Z();
}
// ====================================================== getCoords
void VertexShape::getCoords (double& px, double& py, double& pz)
{
   if (maj_coords)
       updateCoords ();

   px = ss_coord[dir_x];
   py = ss_coord[dir_y];
   pz = ss_coord[dir_z];
}
#if 0
// ====================================================== getShape
const TopoDS_Shape& VertexShape::getShape()
{
    if (geo_shape.IsNull() && ss_parent != NULL &&
            ss_parent->getOrigin() == SH_CLOUD)
    {
        double x, y, z;
        getCoords(x, y, z);
        gp_Pnt pnt(x, y, z);
        geo_shape = BRepBuilderAPI_MakeVertex(pnt);
    }

    return SubShape::getShape ();
}
#endif
// ====================================================== addAssociation
void VertexShape::addAssociation (Vertex* elt)
{
   tab_assoc.push_back (elt);
   is_associated = true ;
}
// ====================================================== getAssociation
Vertex* VertexShape::getAssociation (int nro)
{
   if (nro>0 && nro < (int)tab_assoc.size())
      return tab_assoc[nro];
   else
      return NULL;
}
// ====================================================== definedBy 
bool VertexShape::definedBy (double point[])
{
   if (maj_coords)
       updateCoords ();

   const double Epsilon2 = 1e-4;
   bool   rep = same_coords (point, ss_coord, Epsilon2);
   return rep; 
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ====================================================== saveXml
void VertexShape::saveXml (XmlWriter* xml)
{
   char coord[80];
   sprintf (coord, "%g %g %g", ss_coord[dir_x], ss_coord[dir_y],
                                                ss_coord[dir_z]);

   xml->openMark     ("Vertex");
   xml->addAttribute ("subid", sub_ident);
   xml->addAttribute ("coord", coord);
   xml->closeMark ();
}
END_NAMESPACE_HEXA
