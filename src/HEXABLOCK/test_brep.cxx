
// C++ : Extraction des brep d'un fichier XML 

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

using namespace Hex;

void parse_brep (XmlTree& xml);

// ======================================================== test_brep
int test_brep (int nbargs, cpchar tabargs[])
{

   XmlTree xml("");
   string filename = "restore";
   if (nbargs>1)
      filename = tabargs [1];

   static const int NbExt = 3;
   static cpchar t_ext [NbExt] = { ".xml", ".XML", ".Xml" };
   size_t ici   = 0;
   bool   noext = true;
   for (int nx = 0; nx < NbExt && noext ; nx++)
       {
       ici   = filename.rfind (t_ext[nx]);
       noext = ici < 0 || ici > filename.size();
       }

   if (noext)
      filename += ".xml"; 

   int ier = xml.parseFile (filename);
   if (ier!=HOK) 
      return ier;

   parse_brep (xml);
   return HOK;
}
// ======================================================== get_names
void get_names (const string& chaine, vector<string>& table)
{
   table.clear ();
   int    lg    = chaine.size();
   string mot   = "";
   bool encours = false;

   for (int nc=0 ; nc<lg ; nc++)
       {
       char car  = chaine[nc];
       if (isalnum (car))
          {
          mot += car;
          encours = true;
          }
       else if (encours)
          {
          table.push_back (mot);
          encours = false;
          mot     = "";
          }
       }

   if (encours)
      table.push_back (mot);
}
// ======================================================== save_file_brep
void save_file_brep (string& nom, string& brep)
{
   string filename = nom + ".brep";
   FILE*  fic      = fopen (filename.c_str(), "w"); 

   if (fic==NULL)
      {
      cout << " *** Echec a l'ouverture du fichier " << filename << endl;
      return;
      }

   cout << " +++ Ouverture du fichier " << filename << endl;
   fprintf (fic, "%s\n", brep.c_str());
   fclose  (fic);
   
}
// ======================================================== parse_brep
void parse_brep (XmlTree& xml)
{
   // xml.dump ();

   XmlTree* rubrique = xml.findChild ("ListVertices");
   int nbrelts       = rubrique->getNbrChildren ();

   string nom, brep, filename;
   XmlTree* node = NULL;
   vector <string> tname;

   for (int nro=0 ; nro < nbrelts ; nro++)
       {
       node = rubrique->getChild (nro);
       nom  = node->findValue ("id");
       brep = node->findValue ("shape");
       if (NOT brep.empty ())
           save_file_brep (nom, brep);
       }

   rubrique = xml.findChild ("ListEdges");
   nbrelts  = rubrique->getNbrChildren ();

   for (int nro=0 ; nro < nbrelts ; nro++)
       {
       node = rubrique->getChild (nro);
       const string& type = node->getName();
       if (type=="Edge")
          {
          nom      = node->findValue ("id");
          const  string& vertices = node->findValue ("vertices");
          get_names (vertices, tname);
          }
       else if (type=="Shape")
          {
          brep  = node->findValue ("brep");
          save_file_brep (nom, brep);
          }
       }

   rubrique = xml.findChild ("ListQuads");
   nbrelts  = rubrique->getNbrChildren ();

   for (int nro=0 ; nro < nbrelts ; nro++)
       {
       node = rubrique->getChild (nro);
       const string& type = node->getName();
       if (type=="Quad")
          {
          nom   = node->findValue ("id");
          const string& edges = node->findValue ("edges");
          get_names (edges, tname);
          }
       else if (type=="Shape")
          {
          brep = node->findValue ("brep");
          save_file_brep (nom, brep);
          }
       }
}
