
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

#include "HexLaw.hxx"

BEGIN_NAMESPACE_HEXA

int Law::last_law_id = 0;

// ================================================== Constructeur
Law::Law (Document* dad, cpchar name, int nbnodes) : EltBase (dad, EL_LAW)
{
   nbr_nodes = nbnodes;
   law_id = last_law_id++;

   //Initialisation du nom: un nom par défaut est donné s'il n'est pas fourni
   std::string _nom = std::string(name);
   _nom.erase (_nom.find_last_not_of (" \n\r\t" ) + 1);
   _nom.erase (0, _nom.find_first_not_of (" \n\r\t" ));
   if (!_nom.empty())
	law_name = _nom;
   else {
	char buffer [16];
    sprintf (buffer, "l%04d", law_id);
    law_name = std::string(buffer);
   }

   law_type  = Uniform;
   law_coefficient = 0.0;
}

// ================================================== Constructeur 2
Law::Law (Law* other)
{
   nbr_nodes       = other->nbr_nodes;
   law_name        = other->law_name ;
   law_type        = other->law_type ;
   law_coefficient = other->law_coefficient;
   el_root         = other->el_root;
}

// ========================================================= getNextName
char* Law::getNextName  (pchar buffer)
{
   sprintf (buffer, "l%04d", last_law_id);
   return   buffer;
}

END_NAMESPACE_HEXA
