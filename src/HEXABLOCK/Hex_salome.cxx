
// C++ : La clase principale de Hexa

// Copyright (C) 2009-2024  CEA, EDF
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

#include "HexDocument.hxx"
#include <TopoDS_Shape.hxx>

BEGIN_NAMESPACE_HEXA
//--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8
// ======================================================== addShape
NewShape* Document::addShape (long object, const char* name)
{
   TopoDS_Shape* topo  = (TopoDS_Shape*) object;
   NewShape*     shape = addShape (*topo, name);

   return shape;
}
END_NAMESPACE_HEXA
