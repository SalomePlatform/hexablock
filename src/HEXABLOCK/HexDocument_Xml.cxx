// Copyright (C) 2009-2011  CEA/DEN, EDF R&D
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//


// C++ : Classe Document : methodes internes

#include "HexDocument.hxx"

#include "HexEltBase.hxx"
#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexQuad.hxx"
#include "HexHexa.hxx"

#include "HexElements.hxx"

#include "HexVector.hxx"
#include "HexCylinder.hxx"
#include "HexPipe.hxx"
#include "HexMatrix.hxx"
#include "HexCloner.hxx"
#include "HexPropagation.hxx"
#include "HexLaw.hxx"

#include "HexXmlWriter.hxx"
#include "HexXmlTree.hxx"

BEGIN_NAMESPACE_HEXA

// ======================================================== get_coords
int get_coords (const string& chaine, double& x, double& y, double& z)
{
   // int nv = sscanf (chaine.c_str (), "%lg %lg %lg", &x, &y, &z);
   cpchar buffer = chaine.c_str ();
   int nv = sscanf (buffer, "%lg %lg %lg", &x, &y, &z);
   if (nv!=3) return HERR;
   return HOK;
}
// ======================================================== get_names
void get_names (const string& chaine, int size, vector<string>& table)
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
// ======================================================== loadXml
int Document::loadXml ()
{
   XmlTree xml("");
   xml.parseFile (doc_name + ".xml");

   int    ier = parseXml (xml);
   return ier;
}
// ======================================================== setXml
int Document::setXml (cpchar flux)
{
   XmlTree xml("");
   xml.parseFlow (flux);

   int    ier = parseXml (xml);
   return ier;
}
// ======================================================== parseXml
int Document::parseXml (XmlTree& xml)
{
   xml.dump ();

   map <std::string, Vertex*> t_vertex;
   map <std::string, Edge*>   t_edge;
   map <std::string, Quad*>   t_quad;
   map <std::string, Hexa*>   t_hexa;
   vector <string> tname;

   XmlTree* rubrique = xml.findChild ("ListVertices");
   int nbrelts       = rubrique->getNbrChildren ();

   for (int nro=0 ; nro < nbrelts ; nro++)
       {
       XmlTree* node = rubrique->getChild (nro);
       double px, py, pz;
       const  string& nom    = node->findValue ("id");
       const  string& coords = node->findValue ("coord");
       get_coords (coords, px, py, pz);

       t_vertex [nom] = addVertex (px, py, pz);
       }

   rubrique = xml.findChild ("ListEdges");
   nbrelts  = rubrique->getNbrChildren ();

   for (int nro=0 ; nro < nbrelts ; nro++)
       {
       XmlTree* node = rubrique->getChild (nro);
       const  string& nom      = node->findValue ("id");
       const  string& vertices = node->findValue ("vertices");
       get_names (vertices, V_TWO, tname);

       t_edge [nom] = new Edge (t_vertex [tname[0]], t_vertex [tname[1]]);
       }

   rubrique = xml.findChild ("ListQuads");
   nbrelts  = rubrique->getNbrChildren ();

   for (int nro=0 ; nro < nbrelts ; nro++)
       {
       XmlTree* node = rubrique->getChild (nro);
       const string& nom   = node->findValue ("id");
       const string& edges = node->findValue ("edges");
       get_names (edges, V_TWO, tname);

       t_quad [nom] = new Quad (t_edge [tname[0]], t_edge [tname[1]],
                                t_edge [tname[2]], t_edge [tname[3]]);
       }

   rubrique = xml.findChild ("ListHexas");
   nbrelts  = rubrique->getNbrChildren ();

   for (int nro=0 ; nro < nbrelts ; nro++)
       {
       XmlTree* node = rubrique->getChild (nro);
       const  string& nom   = node->findValue ("id");
       const  string& quads = node->findValue ("quads");
       get_names (quads, V_TWO, tname);

       t_hexa [nom] = new Hexa (t_quad [tname[0]], t_quad [tname[1]],
                                t_quad [tname[2]], t_quad [tname[3]],
                                t_quad [tname[4]], t_quad [tname[5]]);
       }

   return HOK;
}
// ======================================================== renumeroter
void Document::renumeroter ()
{
   doc_modified = true;
                                       // -- 1) Raz numerotation precedente
   markAll (NO_COUNTED);
}
// ======================================================== saveFile
int Document::saveFile ()
{
   int    ier = genXml (doc_name.c_str ());
   return ier;
}
// ======================================================== saveFile
cpchar Document::getXml ()
{
   int ier = genXml (NULL);
   if (ier!=HOK)
      return NULL;

   return doc_xml->getXml ();
}
// ======================================================== genXml
int Document::genXml (cpchar filename)
{
                                       // -- 1) Raz numerotation precedente
   renumeroter ();
   if (maj_propagation)
       majPropagation ();

   doc_modified = false;

   if (doc_xml==NULL)
       doc_xml = new XmlWriter ();

   doc_xml->openXml  (filename);
   doc_xml->openMark ("Document");
   doc_xml->addAttribute ("name", doc_name);
   doc_xml->endMark ();

   cpchar balise [] = {"ListXXX", 
          "ListVertices", "ListEdges", "ListQuads", "ListHexas", "ListXXXX" };

   for (int type=EL_VERTEX ; type <= EL_HEXA ; type++)
       {
       doc_xml->addMark (balise [type]);
       for (EltBase* elt = doc_first_elt[type]->next (); elt!=NULL;
                     elt = elt->next())
           {
           if (elt !=NULL && elt->isHere())
              elt->saveXml (doc_xml);
           }
       doc_xml->closeMark (true);
       }

   doc_xml->addMark ("ListDicretizationLaws");
   for (int nro=0 ; nro<nbr_laws ; nro++)
       doc_laws [nro]->saveXml (doc_xml);
   doc_xml->closeMark (true);

   doc_xml->addMark ("ListPropagations");
   for (int nro=0 ; nro<nbr_propagations ; nro++)
       doc_propagation[nro]->saveXml (doc_xml);
   doc_xml->closeMark ();

   doc_xml->closeMark ();
   doc_xml->closeXml  ();
   return  HOK;
}
// ======================================================== markAll
void Document::markAll (int marque, int type)
{
   int debut = EL_VERTEX;
   int fin   = EL_HEXA;
   if (type>=0 && type<EL_MAXI)
      debut = fin = type;

   for (int type=debut ; type <= fin ; type++)
       {
       for (EltBase* elt = doc_first_elt[type]->next (); elt!=NULL;
                     elt = elt->next())
           elt->setMark (marque);
       }
}
// ====================================================== saveVtk
int Document::saveVtk (cpchar nomfic)
{
                                           // -- 1) Raz numerotation precedente
   markAll (NO_COUNTED, EL_VERTEX);

   int nbnodes = 0;
   int nbcells = 0;
                                           // -- 2) Comptage
   for (EltBase* elt = doc_first_elt[EL_HEXA]->next (); elt!=NULL;
                 elt = elt->next())
       {
       Hexa* cell = static_cast <Hexa*> (elt);
       if (cell!=NULL && cell->isHere())
          {
          nbcells ++;
          nbnodes += cell->countNodes ();
          }
       }

   pfile    vtk = fopen (nomfic, "w");
   fprintf (vtk, "# vtk DataFile Version 3.1\n");
   fprintf (vtk, "%s \n", nomfic);
   fprintf (vtk, "ASCII\n");
   fprintf (vtk, "DATASET UNSTRUCTURED_GRID\n");
   fprintf (vtk, "POINTS %d float\n", nbnodes);

                                           // -- 2) Les noeuds
   int nronode = 0;
   for (EltBase* elt = doc_first_elt[EL_HEXA]->next (); elt!=NULL;
                 elt = elt->next())
       {
       Hexa* cell = static_cast <Hexa*> (elt);
       if (cell!=NULL && cell->isHere())
          cell->printNodes (vtk, nronode);
       }
                                           // -- 2) Les hexas

   fprintf (vtk, "CELLS %d %d\n", nbcells, nbcells*(HV_MAXI+1));

   for (EltBase* elt = doc_first_elt[EL_HEXA]->next (); elt!=NULL;
                 elt = elt->next())
       {
       Hexa* cell = static_cast <Hexa*> (elt);
       if (cell!=NULL && cell->isHere())
          cell->printHexa (vtk);
       }

   fprintf (vtk, "CELL_TYPES %d\n", nbcells);
   for (int nro=0 ; nro<nbcells ; nro++)
       fprintf (vtk, "%d\n", HE_MAXI);

   fprintf (vtk, "POINT_DATA %d \n", nbnodes);
   fprintf (vtk, "SCALARS A float\n");
   fprintf (vtk, "LOOKUP_TABLE default\n");

   for (EltBase* elt = doc_first_elt[EL_HEXA]->next (); elt!=NULL;
                 elt = elt->next())
       {
       Hexa* cell = static_cast <Hexa*> (elt);
       if (cell!=NULL && cell->isHere())
          cell->colorNodes (vtk);
       }

   fclose (vtk);
   return HOK;
}
// ====================================================== purge
void Document::purge ()
{
   purge_elements = true;
                       // ------------------- Raz marques
   markAll (NO_USED);

                       // ------------------- Marquage elements utilises
   for (EltBase* elt = doc_first_elt[EL_HEXA]->next (); elt!=NULL;
                 elt = elt->next())
       {
       Hexa* cell = static_cast <Hexa*> (elt);
       if (cell!=NULL && cell->isHere())
          cell->markElements (IS_USED);
       }
                       // ------------------- Elimination elements inutilises
   for (int type=EL_VERTEX ; type <= EL_QUAD ; type++)
       {
       for (EltBase* elt = doc_first_elt[type]->next (); elt!=NULL;
                     elt = elt->next())
           {
           if (elt->getMark  () == NO_USED)
               elt->suppress ();
           }
       }
                       // ------------------- Sortie elements inutilises

   EltBase* trash = doc_last_elt [EL_REMOVED];

   for (int type=EL_VERTEX ; type <= EL_HEXA ; type++)
       {
       doc_nbr_elt [type] = 0;
       EltBase* last = doc_first_elt [type];
       for (EltBase* elt = last->next (); elt!=NULL; elt = last->next())
           {
           if (elt->isHere  ())
               {
               doc_nbr_elt  [type] ++;
               last = elt;
               }
           else
               {
               last  -> setNext (elt -> next());
               trash -> setNext (elt);
               trash = elt;
               trash -> setNext (NULL);
               }
           }
       doc_last_elt [type] = last;
       }

   doc_last_elt [EL_REMOVED] = trash;
   update ();
}
// ======================================================== majReferences
void Document::majReferences ()
{
   maj_connection = false;

   for (int type=EL_VERTEX ; type <= EL_QUAD ; type++)
       {
       for (EltBase* elt = doc_first_elt[type]->next (); elt!=NULL;
                     elt = elt->next())
           {
           elt->razReferences ();
           }
       }

   for (int type=EL_EDGE ; type <= EL_HEXA ; type++)
       {
       for (EltBase* elt = doc_first_elt[type]->next (); elt!=NULL;
                     elt = elt->next())
           {
           if (elt->isHere ())
               elt->majReferences ();
           }
       }
}
// ======================================================== dump
void Document::dump ()
{
   cpchar nom_type [] = { "Elments non classes", 
          "Sommets", "Aretes", "Faces", "Hexaedres", "Elements detruits" };

   for (int type=EL_VERTEX ; type <= EL_HEXA ; type++)
       {
       printf ("\n");
       printf (" ++++ Liste des %s\n", nom_type[type]);
       printf ("\n");

       for (EltBase* elt = doc_first_elt[type]->next (); elt!=NULL;
                     elt = elt->next())
           {
           elt->dump ();
           }
       }

   printf (" ++++ End od dump\n");
}
// ======================================================== putError
void Document::putError (cpchar mess, cpchar info1, cpchar info2)
{
     nbr_errors ++;
     printf (" ********************************************************** \n");
     printf (" ****  HexaBlocks Error nro %d :\n", nbr_errors);
     printf (" **** ");
     printf (mess, info1, info2);
     printf ("\n");
     printf (" **** \n");
     printf (" ********************************************************** \n");
}
// ======================================================== hputError
void Document::hputError (cpchar mess, EltBase* e1, EltBase* e2)
{
   char name1 [32] = { 0 };
   char name2 [32] = { 0 };

   if (e1!=NULL) e1->getName (name1);
   if (e2!=NULL) e2->getName (name2);

   putError (mess, name1, name2);
}

END_NAMESPACE_HEXA
