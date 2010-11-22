
// C++ : Ecriture en XML

#include "HexXmlWriter.hxx"

BEGIN_NAMESPACE_HEXA

enum { M_CLOSED, M_OPEN, M_SAUT };

// ========================================================= Constructeur
XmlWriter::XmlWriter  ()
{
   xml_file  = stdout;
   xml_level = 0;
   xml_pos   = 0;
}
// ========================================================= openXml 
int XmlWriter::openXml (cpchar nomfic)
{
   xml_level    = 0;
   xml_pos      = 0;
   string fname = nomfic;

   fname   += ".xml";
   xml_file = fopen (fname.c_str(), "w");
   if (xml_file==NULL)
      {
      xml_file = stdout;
      return HERR;
      }
   ecrire ("<?xml version='1.0'?>");
   return HOK;
}
// ========================================================= closeXml 
void XmlWriter::closeXml ()
{
   while (NOT pile_mark.empty())
       closeMark ();

   if (xml_file!=stdout)
      fclose (xml_file);

   xml_file = stdout;
}
// ========================================================= addMark 
void XmlWriter::addMark (cpchar balise, bool jump)
{
   openMark (balise); 
   ecrire (">");
   if (jump) 
      {
      alaLigne ();
      pile_etat.top () = M_SAUT;
      }
   else
      {
      pile_etat.top () = M_CLOSED;
      }
}
// ========================================================= endMark 
void XmlWriter::endMark ()
{
   ecrire ("> ");
   pile_etat.top () = M_SAUT;
}
// ========================================================= openMark 
void XmlWriter::openMark (cpchar balise)
{
   jumpLine ();
   string mot = "<";
   mot +=  balise;
   ecrire (mot);

   pile_mark.push (balise);
   pile_etat.push (M_OPEN);
   xml_level ++;
}
// ========================================================= closeMark
void XmlWriter::closeMark (bool jump)
{
   string balise = pile_mark.top ();
   string mot    = "</";
   int    etat   = pile_etat.top ();

   xml_level --;
   switch (etat)
      {
      case M_OPEN : ecrire (" />");
                    break;
      case M_SAUT : jumpLine ();
      default :
           mot += balise;
           mot += ">";
           ecrire (mot);
      }

   pile_mark.pop ();
   pile_etat.pop ();
   alaLigne ();
   if (jump) alaLigne (true);
}
// ========================================================= jumpLine 
void XmlWriter::jumpLine ()
{
   if (xml_pos>0)
      putc ('\n', xml_file);

   xml_pos = xml_level * xml_decal;
   for (int nc=0 ; nc < xml_pos ; nc++) putc (' ', xml_file);
}
// ========================================================= ecrire
void XmlWriter::ecrire (cpchar mot)
{
   int lg   = strlen (mot);
   xml_pos += lg;

   if (xml_pos >= xml_size)
       {
       jumpLine ();
       xml_pos += lg;
       }

   fprintf (xml_file, mot);
}
// ========================================================= alaLigne
void XmlWriter::alaLigne (bool force)
{
   if (xml_pos==0 && NOT force) 
      return;

   fprintf (xml_file, "\n");
   xml_pos = 0;
}
// ========================================================= addAttribute 
void XmlWriter::addAttribute (cpchar cle, cpchar valeur)
{
   string phrase = " ";
   phrase += cle;
   phrase += " = \"";
   phrase += valeur;
   phrase += "\"";

   ecrire (phrase);
}
END_NAMESPACE_HEXA
