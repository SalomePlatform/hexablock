
// Class : Ecriture d'un fichier XML
//
#ifndef __XML_WRITER_H
#define __XML_WRITER_H

#include "hexa_base.hxx"
#include <stack>
#include <vector>

BEGIN_NAMESPACE_HEXA

class XmlWriter 
{
public :
   XmlWriter ();
  ~XmlWriter ()    { closeXml () ; }

   int    openXml  (string& nom)     { return openXml (nom.c_str()); }
   int    openXml  (cpchar nom=NULL);
   void   closeXml ();
   cpchar getXml   ()                { return xml_buffer.c_str();    }

   void openMark  (cpchar balise);                 // <Balise .. 
   void addMark   (cpchar balise, bool jump=true); // <Balise> + eol
   void closeMark (bool jump=false);               // </Balise> ou  />
   void endMark ();       //         >

   void addAttribute (cpchar attrib, cpchar  valeur); // attrib="valeur"
   void addAttribute (cpchar attrib, int     valeur);
   void addAttribute (cpchar attrib, double  valeur);
   void addAttribute (cpchar attrib, string& valeur);

private :
    void jumpLine ();
    void alaLigne (bool force=false);
    void ecrire   (cpchar  mot);
    void ecrire   (string& mot) { ecrire (mot.c_str()) ; }
    void addMot   (cpchar  mot);

private :
    enum  {xml_decal=3, xml_size=80};
    pfile  xml_file;
    int    xml_level;
    int    xml_pos;
    bool   on_file;
    std::string  xml_buffer;
    std::stack <std::string, std::vector <std::string> >  pile_mark; 
    std::stack <int,         std::vector <int> >          pile_etat; 
};
// ====================================================== addAttribute (I)
inline void XmlWriter::addAttribute (cpchar attrib, int valeur)
{
   char buffer [20]; 
   sprintf (buffer, "%d", valeur);
   addAttribute (attrib, buffer);
}
// ====================================================== addAttribute (R)
inline void XmlWriter::addAttribute (cpchar attrib, double valeur)
{
   char buffer [20]; 
   sprintf (buffer, "%g", valeur);
   addAttribute (attrib, buffer);
}
// ====================================================== addAttribute (stl)
inline void XmlWriter::addAttribute (cpchar attrib, string& valeur)
{
   addAttribute (attrib, valeur.c_str());
}
// ====================================================== addmot
inline void XmlWriter::addMot (cpchar mot)
{
   if (on_file) 
      fprintf (xml_file, mot);
   else 
      xml_buffer += mot;
}

END_NAMESPACE_HEXA
#endif
