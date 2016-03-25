
// C++ : Ecriture en XML

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

#include "HexXmlWriter.hxx"

BEGIN_NAMESPACE_HEXA

enum { M_CLOSED, M_OPEN, M_SAUT };

// ========================================================= Constructeur
XmlWriter::XmlWriter  ()
{
   xml_file  = stdout;
   xml_level = 0;
   xml_pos   = 0;
   on_file   = true;
   xml_mode  = InaStudy;
}
// ========================================================= setStream
int XmlWriter::setStream ()
{
   xml_mode  = InaStream;
   xml_file  = NULL;
   on_file   = false;

   return HOK;
}
// ========================================================= setFile
int XmlWriter::setFile (pfile afile)
{
   xml_mode  = InaStudy;
   xml_file  = afile;
   on_file   = true;

   if (xml_file==NULL)
      {
      xml_file = stdout;
      xml_mode  = InaStream;
      return HERR;
      }

   return HOK;
}
// ========================================================= setFileName 
int XmlWriter::setFileName (cpchar nomfic)
{
   xml_mode  = InaFile;
   xml_file  = stdout;
   on_file   = true;

   bool suff = true;
   int  pext = strlen (nomfic) - 4;
   if (pext > 0)
      {
      string sext = &nomfic[pext];
      set_minus (sext);
      suff = sext != ".xml";
      }

   string fname = nomfic;
   if (suff) 
      fname   += ".xml";

   xml_file = fopen (fname.c_str(), "w");
   if (xml_file==NULL)
      {
      xml_file = stdout;
      return HERR;
      }

   return HOK;
}
// ========================================================= startXml 
int XmlWriter::startXml ()
{
   xml_level  = 0;
   xml_pos    = 0;
   xml_buffer = "";

   ecrire ("<?xml version='1.0'?>");
   return HOK;
}
// ========================================================= closeXml 
void XmlWriter::closeXml ()
{
   while (NOT pile_mark.empty())
       closeMark ();

   if (xml_mode == InaFile && xml_file!=stdout)
      {
      fclose (xml_file);
      xml_file = stdout;
      }
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
      addMot ("\n");

   xml_pos = xml_level * xml_decal;
   if (xml_pos > 0)
      {
      string  space (xml_pos, ' ');
      addMot (space.c_str());
      }
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

   addMot (mot);
}
// ========================================================= alaLigne
void XmlWriter::alaLigne (bool force)
{
   if (xml_pos==0 && NOT force) 
      return;

   xml_pos = 0;
   addMot ("\n");
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
