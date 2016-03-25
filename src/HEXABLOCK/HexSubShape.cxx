
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

#include "HexSubShape.hxx"
#include "HexNewShape.hxx"
#include "HexXmlWriter.hxx"

#include <TopoDS.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>

BEGIN_NAMESPACE_HEXA

static bool db = on_debug ();  // == getenv ("HEXA_DB") > 0

// ====================================================== Constructeur
SubShape::SubShape (NewShape* dad, int id, int dim)
        : EltBase  (dad->dad(), EL_SUBSHAPE)
{
   ss_parent  = dad;
   sub_ident  = id;
   sub_dim    = dim;
   maj_brep   = maj_shape = true;
   kind_of    = KS_None; 
}
// ====================================================== getShape
const TopoDS_Shape& SubShape::getShape ()
{
   if (maj_shape)
      updateShape ();
   return geo_shape;
}
// ====================================================== getParentName
cpchar SubShape::getParentName () const
{
   if (ss_parent==NULL)
      return "<?>";

   return ss_parent->getName();
}
// ========================================================= saveXml
void SubShape::saveXml (XmlWriter* xml)
{
   callXml (xml);
   xml->closeMark ();
}
// ========================================================= callXml
void SubShape::callXml (XmlWriter* xml)
{
   xml->openMark     ("Asso");
   xml->addAttribute ("shape", getParentName());
   xml->addAttribute ("subid", getIdent());
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ====================================================== updateShape
void SubShape::updateShape ()
{
   maj_shape = false;
   geo_shape = ss_parent->getGeoShape (sub_ident);
}
END_NAMESPACE_HEXA
