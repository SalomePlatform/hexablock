
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
class XmlTree 
{
public :
   XmlTree (const string& name, XmlTree* dad=NULL);
  ~XmlTree ();

   int  parseFile (const string& name);

   cpchar getName  ()   { return item_name.c_str() ; }
   int    getNbrAttributs  ()   { return nbr_attributs ; }
   int    getNbrChildren   ()   { return nbr_items     ; }

   int           findAttribute (const string& nom);
   const string& findValue     (const string& nom);

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
   pfile    fic_xml;
   int      nro_ligne;
   int      fic_pos;
};
END_NAMESPACE_HEXA
#endif
