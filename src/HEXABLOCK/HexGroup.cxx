
// C++ : Implementation des groupes

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

#include "HexGroup.hxx"
#include "HexEltBase.hxx"
#include "HexXmlWriter.hxx"

BEGIN_NAMESPACE_HEXA

static const cpchar kind_name[] = { "HexaCell", "QuadCell", "EdgeCell", 
                  "HexaNode", "QuadNode", "EdgeNode", "VertexNode" };

// ======================================================== Constructeur
Group::Group (cpchar nom, EnumGroup grp)  
{
   grp_name = nom ; 
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
// ======================================================== removeElement
int Group::removeElement (int nro)
{
   int nbelts = grp_table.size ();

   if (nro<0 || nro >= nbelts)
      return HERR;
 
   grp_table.erase (grp_table.begin() + nro);
   return HOK;
}
// ======================================================== saveXml
void Group::saveXml (XmlWriter* xml)
{
   char buffer[16];
   int nbelts = grp_table.size ();

   xml->addMark  ("Group");

   xml->openMark ("Identification");
   xml->addAttribute ("name",  grp_name);
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
