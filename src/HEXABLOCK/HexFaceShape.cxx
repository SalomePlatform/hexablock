
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

#include "HexFaceShape.hxx"


BEGIN_NAMESPACE_HEXA

static bool db = on_debug ();  // == getenv ("HEXA_DB") > 0

// ====================================================== Constructeur
FaceShape::FaceShape (NewShape* dad, int id)
         : SubShape  (dad, id, 2)
{
}
// ========================================================== addAssociation
void FaceShape::addAssociation (Quad* quad)
{
   tab_assoc.push_back (quad);
   is_associated = true;
}
// ====================================================== getAssociation
Quad* FaceShape::getAssociation (int nro)
{
   if (nro>0 && nro < (int)tab_assoc.size())
      return tab_assoc[nro];
   else
      return NULL;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
END_NAMESPACE_HEXA
