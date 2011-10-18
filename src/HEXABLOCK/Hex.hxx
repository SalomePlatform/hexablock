
// class : La clase principale de Hexa

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

#ifndef __HEX_H
#define __HEX_H

#include "hexa_base.hxx"

BEGIN_NAMESPACE_HEXA

class Hex
{
public:
   int       countDocument ();
   Document* getDocument (int nro);
   void      removeDocument (Document* doc);

   Document* addDocument ();
   Document* loadDocument (const char* filename);

             //  Constructeur + destructeur
    Hex ();
   ~Hex ();

private:
   vector <Document*> liste_documents;
};

END_NAMESPACE_HEXA
#endif
