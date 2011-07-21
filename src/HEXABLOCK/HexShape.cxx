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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// C++ : Gestion des noeuds

#include "HexShape.hxx"

BEGIN_NAMESPACE_HEXA

// int Shape::nbr_shapes = 0;

// ====================================================== Constructeur
Shape::Shape  (string brep)
{
   b_rep = brep;
   debut  = 0;
   fin    = 1;
   ident  = "";

   // nbr_shapes++;
   // nro_shape = nbr_shapes;
}

// ========================================================= getName 
/***************************************************
char* Shape::getName  (char* buffer)
{
   sprintf (buffer, "Shape0x%04d", nro_shape);
   return   buffer;
}
  *************************************************** */
END_NAMESPACE_HEXA
