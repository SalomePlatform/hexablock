
// C++ : Implementation des groupes

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

#include "HexGroup.hxx"
#include "HexXmlWriter.hxx"

BEGIN_NAMESPACE_HEXA

static const cpchar kind_name[] = { "HexaCell", "QuadCell", "EdgeCell", 
                      "HexaNode", "QuadNode", "EdgeNode", "VertexNode" };

// ======================================================== Constructeur
Group::Group   (Document* dad, cpchar nom, EnumGroup grp)
     : EltBase (dad, EL_GROUP) 
{
   string name = std::string(nom);
   name.erase (name.find_last_not_of (" \n\r\t" ) + 1);
   name.erase (0, name.find_first_not_of (" \n\r\t" ));

   if (NOT name.empty())
       setName (name);
   
   grp_kind = grp;
   switch (grp_kind)
      {
      case HexaCell : case HexaNode :
           grp_typelt = EL_HEXA;
           break;

      case QuadCell : case QuadNode :
           grp_typelt = EL_QUAD;
           break;

      case EdgeNode : case EdgeCell :
           grp_typelt = EL_EDGE;
           break;

      case VertexNode : default :
           grp_typelt = EL_VERTEX;
      }
}
// ======================================================== addElement
int Group::addElement (EltBase* elt)
{
   if (elt==NULL || elt->isDeleted() || grp_typelt != elt->getType())
      return HERR;
 
   grp_table.push_back (elt);
   return HOK;
}
// ======================================================== findElement
int Group::findElement (EltBase* elt)
{
   int nbelts = grp_table.size ();
   for (int nro=0 ; nro < nbelts ; nro++)
       if (grp_table[nro]==elt)
          return nro;
       
   return NOTHING;
}
// ======================================================== removeElement
int Group::removeElement (int nro)
{
   int nbelts = grp_table.size ();

   if (nro<0 || nro >= nbelts)
      return HERR;
 
   grp_table.erase (grp_table.begin() + nro);
   return HOK;
}
// ======================================================== removeElement
int Group::removeElement (EltBase* elt)
{
   int nro = findElement (elt);
   int ier = removeElement (nro);
   return ier;
}
// ======================================================== saveXml
void Group::saveXml (XmlWriter* xml)
{
   char buffer[16];
   int nbelts = grp_table.size ();

   xml->addMark  ("Group");

   xml->openMark ("Identification");
   xml->addAttribute ("name",  el_name);
   xml->addAttribute ("kind",  kind_name [grp_kind]);
   xml->closeMark ();

   for (int nro=0 ; nro<nbelts ; nro++)
       {
       if (grp_table[nro]->isHere ())
          {
          xml->openMark ("Element");
          xml->addAttribute ("id",  grp_table[nro]->getName (buffer));
          xml->closeMark ();
          }
       }
   xml->closeMark ();
}
// ======================================================== getKind
EnumGroup Group::getKind (cpchar kind)
{
   int nbk = sizeof (kind_name) / sizeof (cpchar);
   for (int nro=0; nro<nbk ; nro++)
       {
       if (Cestegal (kind, kind_name[nro]))
          return (EnumGroup) nro;
       }
   return VertexNode;
}
END_NAMESPACE_HEXA
