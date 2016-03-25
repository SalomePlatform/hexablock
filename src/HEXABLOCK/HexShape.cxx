
// C++ : Gestion des formes associees

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

#include "HexOldShape.hxx"
#include "HexXmlWriter.hxx"

BEGIN_NAMESPACE_HEXA

// ====================================================== Constructeur
Shape::Shape  (string brep)
{
   b_rep  = brep;
   debut  = 0;
   fin    = 1;
   ident  = "";
}
void clean_brep (string& brep);
// ====================================================== saveXml
void Shape::saveXml (XmlWriter* xml)
{
   char interval [80];
   sprintf (interval, "%g %g", debut, fin);

   clean_brep (b_rep);          // Test Abu

   xml->openMark     ("Shape");
   xml->addAttribute ("interval", interval);
   xml->addAttribute ("ident", ident.c_str());
   xml->addAttribute ("brep",  b_rep.c_str());
   xml->closeMark ();
}
END_NAMESPACE_HEXA
