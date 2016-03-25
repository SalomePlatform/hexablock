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

// Class : Ecriture d'un fichier XML
//
#ifndef __XML_TREE_H_
#define __XML_TREE_H_

#include "hexa_base.hxx"

#include <vector>

BEGIN_NAMESPACE_HEXA

   enum EnumItem { M_NONE,  M_BEGIN, M_END, M_BEGIN_CLOSE, M_CLOSE,
                   M_IDENT, M_EQUALS, M_STRING,
                   M_COMMENT, M_END_COMMENT, M_PROLOG, M_END_PROLOG,
                   M_ERROR};
class HexaExport XmlTree
{
public :
   XmlTree (const string& name, XmlTree* dad=NULL);
  ~XmlTree ();

   int  parseFile   (const string& name);
   int  parseStream (cpchar flux, int& posit);

   cpchar getName  ()   { return item_name.c_str() ; }
   int    getNbrAttributs  ()   { return nbr_attributs ; }
   int    getNbrChildren   ()   { return nbr_items     ; }

   int           findAttribute (const string& nom);
   const string& findValue     (const string& nom);
   int           findInteger   (const string& nom);

   cpchar   getAttribute  (int nro)    { return tab_attributs [nro].c_str(); }
   cpchar   getValue      (int nro)    { return tab_values    [nro].c_str(); }

   XmlTree* findChild    (const string& nom);
   XmlTree* getChild     (int nro)    { return tab_items [nro]; }
   XmlTree* getParent    ()           { return xml_parent; }

                  // ------------------------- Modifications

   XmlTree* addChild    (const string& nom);
   void     addAttribut (const string& nom, const string& valeur);
   void     setName     (const string&  nom)   { item_name = nom; }
   void     dump        (int niveau=0);

   int      goTo        (cpchar  ouca);

private :
   int  parseXml ();
   int  readLine ();
   EnumItem  readItem  (string& item);
   EnumItem  getItem   (string& item, EnumItem waited=M_NONE);
   int       getString (string& item);
   int       getIdent  (string& item);
   int       getChar   ();
   void      putError  (cpchar why);

private :
   string   item_name;
   string   item_vide;
   int      nbr_attributs;
   int      nbr_items;
   XmlTree* xml_parent;

   vector <string>   tab_attributs;
   vector <string>   tab_values;
   vector <XmlTree*> tab_items;
                               // Lecture
   string   fic_buffer;
   int      len_buffer;
   int      nro_ligne;
   int      fic_pos;
   int      pos_flow;
   pfile    xml_file;
   cpchar   xml_flow;
   bool     xml_ended;
};
END_NAMESPACE_HEXA
#endif
