
// C++ : Gestion des tuyaux

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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "HexPipe.hxx"
#include "HexVertex.hxx"
#include "HexVector.hxx"
#include "HexXmlWriter.hxx"

BEGIN_NAMESPACE_HEXA

// ======================================================== Constructeur
Pipe::Pipe (Vertex* b, Vector* v, double ri, double re, double h)
    : Cylinder (b, v, re, h)
{
    c_int_radius = ri;
    if (isBad () || c_int_radius <= el_root->getTolerance () 
                 || c_int_radius >= c_radius)
        setError ();
}
// ========================================================= saveXml 
void Pipe::saveXml  (XmlWriter* xml)
{
   char buffer[12];

   xml->openMark     ("Pipe");
   xml->addAttribute ("c_base", c_base->getName (buffer));
   xml->addAttribute ("c_dir",  c_dir->getName  (buffer));
   xml->addAttribute ("c_radius",     c_radius);
   xml->addAttribute ("c_int_radius", c_int_radius);
   xml->addAttribute ("c_height",     c_height);
   xml->closeMark ();
}
END_NAMESPACE_HEXA
