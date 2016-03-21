
// C++ : ParserXml

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

#include "HexXmlTree.hxx"

BEGIN_NAMESPACE_HEXA

enum { HEND_FILE = -2, HEND_LINE = -1, CRLF = 13 };

// ====================================================== Constructeur
XmlTree::XmlTree (const string& nom, XmlTree* dad)
{
   item_name     = nom;
   item_vide     = "";
   xml_parent    = dad;
   nbr_attributs = 0;
   nbr_items     = 0;

   fic_buffer  = "";
   len_buffer  = 0;
   xml_file    = NULL;
   nro_ligne   = 0;
   fic_pos     = 1988;

   xml_flow  = NULL;
   pos_flow  = 0;
   xml_ended = true;
}
// ====================================================== Destructeur
XmlTree::~XmlTree ()
{
}
// ====================================================== findChild
XmlTree* XmlTree::findChild (const string& nom)
{
   for (int nro = 0 ; nro<nbr_items ; nro++)
       {
       if (nom == tab_items [nro]->item_name)
           return tab_items [nro];
       }
   return NULL;
}
// ====================================================== findAttribute
int XmlTree::findAttribute (const string& nom)
{
   for (int nro = 0 ; nro<nbr_attributs ; nro++)
       {
       if (nom == tab_attributs [nro])
          return nro;
       }
   return NOTHING;
}
// ====================================================== findValue
const string& XmlTree::findValue (const string& nom)
{
   for (int nro = 0 ; nro<nbr_attributs ; nro++)
       {
       if (nom == tab_attributs [nro])
          return  tab_values    [nro];
       }
   return item_vide;
}
// ====================================================== findInteger
int XmlTree::findInteger (const string& nom)
{
   int   val = 0;
   const string chnum = findValue (nom);
   if (chnum==item_vide)
      return val;

   int lg = chnum.size();
   for (int nc=0 ; nc<lg ; ++nc)
       {
       char car = chnum[nc];
       if (car >= '0' && car <= '9')
           val  = 10*val + car - '0';
       }
   return val;
}
// ====================================================== parseFile
int XmlTree::parseFile (const string& ficnom)
{
   fic_buffer  = "";
   len_buffer  = 0;
   nro_ligne   = 0;
   fic_pos     = 1988;
   xml_flow    = NULL;
   pos_flow    = 0;
   xml_ended   = true;

   xml_file = fopen (ficnom.c_str(), "r");
   if (xml_file==NULL)
      {
      cout << " **** Fichier XML '" << ficnom << "' inaccessible"
           << endl;
      return HERR;
      }

   int ier = parseXml ();
   return ier;
}
// ====================================================== parseStream
int XmlTree::parseStream (cpchar flux, int& posit)
{
   fic_buffer  = "";
   len_buffer  = 0;
   nro_ligne   = 0;
   fic_pos     = 1988;

   xml_flow = flux;
   pos_flow = posit;

   int ier  = parseXml ();
   posit    = pos_flow;
   return ier;
}
// ====================================================== parseXml
int XmlTree::parseXml ()
{
   xml_ended  = false;

   enum EnumEtat { M_PREMS, M_BALISE_OUVERTE, M_NEUTRE };
   EnumEtat etat =  M_PREMS;

   XmlTree* node = this;
   EnumItem item_lu;
   string   nom, valeur, foo;
   while ((item_lu=readItem (nom)) != M_NONE)
      {
      switch (item_lu)
         {
         case M_BEGIN :
              item_lu = getItem (nom, M_IDENT);
              if (etat==M_PREMS)
                 setName (nom);
              else
                 node = node -> addChild  (nom);
              etat = M_BALISE_OUVERTE;
              break;

         case M_END   :
              if (etat != M_BALISE_OUVERTE)
                 putError (" balise ouverte");
              etat = M_NEUTRE;
              break;

         case M_BEGIN_CLOSE :
              if (etat == M_BALISE_OUVERTE)
                 putError (" balise ouverte");
              getItem (nom, M_IDENT);
              getItem (foo, M_END);
              node = node -> getParent ();
              if (node==NULL)
                 return HOK;
              break;

         case M_CLOSE :
              if (etat != M_BALISE_OUVERTE)
                 putError (" balise deja fermee");
              node = node -> getParent ();
              etat = M_NEUTRE;
              break;

         case M_IDENT :
              getItem (valeur, M_EQUALS);
              getItem (valeur, M_STRING);
              node -> addAttribut (nom, valeur);
              break;

         case M_COMMENT : goTo ("-->");
              break;

         case M_PROLOG  : goTo ("?>");
              break;

         default :
         case M_EQUALS :
         case M_STRING :
         case M_END_COMMENT :
         case M_END_PROLOG :
              putError ("Item incorrect");
         }
      }
   return HOK;
}
// ====================================================== getItem
EnumItem XmlTree::getItem (string& value, EnumItem waited)
{
   EnumItem item = readItem (value);

   if (item == waited)
      return item;
   putError ("Erreur de sequence");
   return item;
}
// ====================================================== readItem
EnumItem XmlTree::readItem (string& value)
{
   value  = "";

   while (true)
         {
         if (fic_pos>=len_buffer)
            {
            int ier=readLine ();
            if (ier==HEND_FILE)
               return M_NONE;
            }
         else
            {
            char  car = fic_buffer [fic_pos++];

            if (car=='=')
               return M_EQUALS;
            else if (car=='>')
               return M_END;
            else if (car=='"')
               {
               getString (value);
               return M_STRING;
               }
            else if (isalpha (car))
               {
               getIdent (value);
               return M_IDENT;
               }

            char ncar = fic_pos >= len_buffer ? ' ' : fic_buffer [fic_pos];
            if (car=='/' && ncar == '>')
               {
               fic_pos++;
               return M_CLOSE;
               }
            else if (car=='<')
               {
               if (ncar=='/')
                  {
                  fic_pos++;
                  return M_BEGIN_CLOSE;
                  }
               else if (ncar=='?')
                  {
                  fic_pos++;
                  return M_PROLOG;
                  }
               else if (ncar=='!')
                  {
                  fic_pos++;
                  return M_COMMENT;
                  }
               else
                  return M_BEGIN;
               }
            }
         }
}
// ====================================================== getIdent
int XmlTree::getIdent (string& ident)
{
   ident = "";

   for (int nc=fic_pos-1; nc<len_buffer ; nc++)
       {
       char car = fic_buffer[nc];
       if (isalnum (car) || car =='_')
           {
           ident.push_back (car);
           }
       else
           {
           fic_pos = nc;
           return HOK;
           }
       }

   fic_pos = len_buffer;
   return HOK;
}
// ====================================================== goTo
int XmlTree::goTo (cpchar ouca)
{
   int nbc = strlen (ouca) - 1;
   int pos = 0;
   int car = ' ';

   while ((car = getChar ()) != EOF)
       {
       if (car!=ouca[pos])
          pos = 0;
       else if (pos<nbc)
          pos++;
       else
          return HOK;
       }

   return HERR;
}
// ====================================================== getString
int XmlTree::getString (string& chaine)
{
   chaine  = "";
   int car = ' ';

   while ((car = getChar ()) != EOF)
       {
       if (car=='"')
          return HOK;
       chaine.push_back (car);
       }

   return HERR;
}
// ====================================================== getChar
int XmlTree::getChar ()
{
   while (true)
         {
         if (fic_pos<len_buffer)
            {
            fic_pos++;
            return fic_buffer [fic_pos-1];
            }
         int ier = readLine ();
         if (ier==HEND_FILE)
            return EOF;
         else
            return EOL;
         }
}
// ====================================================== readLine
int XmlTree::readLine ()
{
   nro_ligne++;
   fic_buffer = "";
   fic_pos    = 0;

   if (xml_ended)
       return HEND_FILE;

   len_buffer = 0;
   int ier = HEND_LINE;
   while (ier==HEND_LINE)
         {
         int carac = xml_flow != NULL ? xml_flow [pos_flow++]
		                      : fgetc (xml_file);

         if (carac==EOL || carac==CRLF)
            {
            ier = HOK;
            }
         else if (carac!=EOF && carac!=EOS)
            {
            fic_buffer.push_back (carac);
            len_buffer ++;
            }
         else if (len_buffer > 0)
            {
            xml_ended = true;
            ier = HOK;
            }
         else
            {
            xml_ended = true;
            ier = HEND_FILE;
            }
         }
   return HOK;
}
// ====================================================== putError
void XmlTree::putError (cpchar mess)
{
     printf (" ***** Erreur : %s\n", mess);
     printf (" +++ Derniere ligne lue : nro %d, %deme caractere\n",
               nro_ligne, fic_pos);
     printf ("%s\n", fic_buffer.c_str());
     //  exit (102);
}
// ====================================================== addChild
XmlTree* XmlTree::addChild (const string& nom)
{
   XmlTree* child = new XmlTree (nom, this);
   tab_items.push_back (child);
   nbr_items ++;
   return child;
}
// ====================================================== addAttribut
void XmlTree::addAttribut (const string& nom, const string& value)
{
   tab_attributs.push_back (nom);
   tab_values   .push_back (value);
   nbr_attributs ++;
   // printf (" %s = %s\n", nom.c_str(), value.c_str());
}
// ====================================================== dump
void XmlTree::dump (int niveau)
{
   string marge = "";
   for (int niv=0 ; niv<niveau ; niv++)
       marge += " | ";

   cout << marge << item_name << endl;

   for (int nc=0 ; nc<nbr_attributs ; nc++)
       {
       cout << marge  << " : "
            << tab_attributs [nc] << " = '" << tab_values [nc]
            << "'" << endl;
       }

   for (int nc=0 ; nc<nbr_items ; nc++)
       {
       tab_items [nc]->dump (niveau+1);
       }

}
END_NAMESPACE_HEXA
