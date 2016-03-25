
// C++ : La clase principale de Hexa

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
#include "Hex.hxx"

#include "HexEltBase.hxx"
#include "HexDocument.hxx"
#include "HexGlobale.hxx"
#include <clocale>

BEGIN_NAMESPACE_HEXA

Hex* Hex::first_instance = NULL;

// ======================================================== Constructeur
Hex::Hex ()
{
   setlocale (LC_NUMERIC, "C");
   glob = Globale::getInstance ();
}
// ======================================================== Destructeur
Hex::~Hex ()
{
   int nbre = liste_documents.size();
   for (int nd=0 ; nd<nbre ; nd++) 
       delete liste_documents [nd];
}
// ======================================================== countDocument
int Hex::countDocument ()
{
   return liste_documents.size();
}
// ======================================================== getDocument
Document* Hex::getDocument (int nro)
{
   if (nro<0 || nro>=(int)liste_documents.size())
      return NULL;
   
   return liste_documents [nro];
}
// ======================================================== removeDocument
void Hex::removeDocument (Document* doc)
{
   bool actif = glob->dump.start ("hexablock", "removeDocument");
   if (actif)
       glob->dump << doc;

   int nbre = liste_documents.size();
   for (int nd=0 ; nd<nbre ; nd++) 
       {
       if (doc == liste_documents [nd])
          {
          liste_documents.erase (liste_documents.begin()+nd);
          delete doc;
          glob->dump.close (actif);
          return;
          }
       }
                      // Pas trouve dans la liste. On detruit quand meme
    glob->dump.close (actif);
    delete doc;
}
// ======================================================== addDocument
Document* Hex::addDocument (cpchar nomdoc)
{
   bool actif = glob->dump.start ("hexablock", "addDocument");
   if (actif)
       glob->dump << nomdoc;

   string name;
   makeName (nomdoc, name);
   Document* doc = new Document (name.c_str(), this);

   liste_documents.push_back (doc);

   glob->dump.close (actif, doc);
   return doc;
}
// ======================================================== loadDocument
Document* Hex::loadDocument (cpchar filename)
{
   bool actif = glob->dump.start ("hexablock", "loadDocument");
   if (actif)
       glob->dump << filename;

   Document* doc = addDocument ("default");
   doc->loadXml (filename);

   glob->dump.close (actif, doc);
   return doc;
}
// ======================================================== loadAllDocs
int Hex::loadAllDocs (cpchar stream)
{
   int posit  = 0;
   int car    = ' '; 
   int nbdocs = 0;

   while ( (car=stream[posit++]) != '>')
         if (car>='0' && car <='9')
            nbdocs = 10*nbdocs + car - '0';
            
   for (int nro = 0; nro<nbdocs ; nro++)
       {
       Document* doc = addDocument ("xxxx");
       doc->setXml (stream, posit);
       }

   PutData (posit);
   return HOK;
}
// ======================================================== saveAllDocs
int Hex::saveAllDocs (cpchar filename)
{
   FILE* fic = fopen (filename, "w");
   if (fic==NULL)
      return HERR;

   int nbdocs = countDocument ();
   fprintf (fic, "<%d>", nbdocs);

   for (int nro = 0; nro<nbdocs ; nro++)
       {
       Document* doc = getDocument (nro);
       if (doc!=NULL)
           doc->appendXml (fic);
       }

   fclose  (fic);
   return HOK;
}
// ======================================================== findDocument
Document* Hex::findDocument (cpchar name)
{
   int nbdocs   = liste_documents.size();
   for (int nro = 0; nro<nbdocs ; nro++)
       {
       Document*  doc = getDocument (nro);
       if (doc!=NULL && Cestegal (name, doc->getName ()))
          return doc;
       }
   return NULL;
}
// ======================================================== makeName
void Hex::makeName (cpchar radical, string& name)
{
   char cnum [8];
   int  numero = 0;
   while (true)
       {
       name = radical;
       if (numero>0)
          {
          sprintf (cnum, "_%d", numero);
          name += cnum;
          }
       numero ++;
       if (findDocument (name)==NULL)
          return;
       }
}
// ======================================================== lockDump
void Hex::lockDump ()
{
   glob->dump.lock ();
}
// ======================================================== restoreDump
void Hex::restoreDump ()
{
   glob->dump.restore (DumpActif);
}
// ====================================================== getInstance
Hex* Hex::getInstance  ()
{
   if (first_instance==NULL) 
       first_instance = new Hex ();

   return first_instance;
}
// ======================================================== what (?)
void Hex::what ()
{
   bool actif = glob->dump.start ("hexablock", "what", false);

   Globale* glob = Globale::getInstance  ();
   glob->mess.printMessage();

   glob->dump.close (actif);
}
// ======================================================== sizeofMessage
int Hex::sizeofMessage ()
{
   return glob->mess.sizeofMessage (); 
}
// ======================================================== getMessageLine
cpchar Hex::getMessageLine (int nlig)
{ 
   return glob->mess.getLine (nlig);
}
// ======================================================== hex_instance
Hex* hex_instance ()
{
   Hex*   hexa = Hex::getInstance ();
   return hexa;
}
// ======================================================== what (fonction)
void what ()
{
   Hex* hexa = Hex::getInstance ();
   hexa->what ();
}

END_NAMESPACE_HEXA
