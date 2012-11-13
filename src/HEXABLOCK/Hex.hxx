
// class : La clase principale de Hexa

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
// See http://www.salome-platform.org/ 
// or email : webmaster.salome@opencascade.com
//

#ifndef __HEX_H
#define __HEX_H

#include "hexa_base.hxx"

BEGIN_NAMESPACE_HEXA

class Hex
{
public:
   static Hex* getInstance ();

   int       countDocument ();
   Document* getDocument (int nro);
   void      removeDocument (Document* doc);

   Document* addDocument  (cpchar name="default");
   Document* loadDocument (cpchar name);
   Document* findDocument (cpchar name);
   Document* findDocument (const string& name) 
             { return findDocument (name.c_str()) ; }

   int loadAllDocs (cpchar flow);
   int saveAllDocs (cpchar filename);
   void makeName   (cpchar radical, string& name);

   void lockDump();
   void restoreDump();
     
// private:
             //  Constructeur + destructeur
    Hex ();
   ~Hex ();

private:
   static Hex* first_instance;
   vector <Document*> liste_documents;
   Globale*    glob;
};

END_NAMESPACE_HEXA
#endif
