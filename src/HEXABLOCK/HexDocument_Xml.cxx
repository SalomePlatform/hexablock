
// C++ : Classe Document : methodes internes

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
#include "HexDocument.hxx"
// #include <Basics_DirUtils.hxx>   
#include <cstdlib>               // Pour atoi et atof

#include "Hex.hxx"
#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexQuad.hxx"
#include "HexHexa.hxx"

#include "HexElements.hxx"

#include "HexVector.hxx"
#include "HexMatrix.hxx"
#include "HexCloner.hxx"
#include "HexPropagation.hxx"
#include "HexLaw.hxx"
#include "HexGroup.hxx"

#include "HexNewShape.hxx"
#include "HexVertexShape.hxx"
#include "HexEdgeShape.hxx"
#include "HexFaceShape.hxx"

#include "HexXmlWriter.hxx"
#include "HexXmlTree.hxx"
#include "HexGlobale.hxx"

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
// ======================================================== get_coords
int get_coords (const string& chaine, double& x, double& y)
{
   cpchar buffer = chaine.c_str ();
   int nv = sscanf (buffer, "%lg %lg", &x, &y);
   if (nv!=2) return HERR;
   return HOK;
}
// ======================================================== parse_name
int parse_name (XmlTree* node, const string& nom, EltBase* elt)
{
   int lg    = nom.size();
   int nroid = 0;
   for (int nc=1 ; nc<lg ; nc++)
       nroid = 10*nroid + nom[nc] - '0';

   elt->setId (nroid);

   const  string& name = node->findValue ("name");
   if (name=="")
      return HERR;

   elt->setName (name);
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
// ======================================================== count_children
int count_children (XmlTree* dad)
{
   int    nbre = dad==NULL ? 0 : dad->getNbrChildren ();
   return nbre;
}
// ======================================================== loadXml
int Document::loadXml (cpchar ficname)
{
   XmlTree xml("");
   string filename = ficname;
   //el_name         = Kernel_Utils::GetBaseName ((pchar)ficname);
   make_basename (ficname, el_name);

   static const int NbExt = 3;
   static cpchar t_ext [NbExt] = { ".xml", ".XML", ".Xml" };
   size_t ici   = 0;
   bool   noext = true;
   for (int nx = 0; nx < NbExt && noext ; nx++)
       {
       ici   = el_name.rfind (t_ext[nx]);
       noext = ici < 0 || ici > el_name.size();
       }

   if (noext)
      filename += ".xml";
   else
      el_name.erase (ici, 4);

   int ier = xml.parseFile (filename);
   if (ier!=HOK)
      return ier;

   ier = parseXml (xml);
   return ier;
}
// ======================================================== setXml
int Document::setXml (cpchar flux)
{
   int posit = 0;
   int ier   = setXml (flux, posit);
   return ier;
}
// ======================================================== setXml
int Document::setXml (cpchar flux, int& posit)
{
   XmlTree xml("");

   int ier = xml.parseStream (flux, posit);
   if (ier!=HOK)
      return ier;

   ier = parseXml (xml);
   if (ier==HOK)
      doc_saved = true;

   return ier;
}
// ======================================================== parseXml
int Document::parseXml (XmlTree& xml)
{
   // xml.dump ();

   map <std::string, Vertex*> t_vertex;
   map <std::string, Edge*>   t_edge;
   map <std::string, Quad*>   t_quad;
   map <std::string, Hexa*>   t_hexa;
   map <std::string, Vector*> t_vector;
   vector <string> tname;

   const  string& version = xml.findValue ("version");
   if (version == "")
       {
       cout << " **** Format du fichier XML perime"
            << endl;
       return HERR;
       }
   const  string& name = xml.findValue ("name");
   if (name != el_name)
       setName (name.c_str());

   parseShapes (xml);

   XmlTree* rubrique = xml.findChild ("ListVertices");
   int nbrelts       = count_children (rubrique);

   Vertex* vertex = NULL;
   for (int nro=0 ; nro < nbrelts ; nro++)
       {
       XmlTree* node = rubrique->getChild (nro);
       const string& type = node->getName();
       double px, py, pz;
       if (type=="Vertex")
          {
          const  string& nom    = node->findValue ("id");
          const  string& coords = node->findValue ("coord");
          get_coords (coords, px, py, pz);

          vertex = addVertex (px, py, pz);
          parse_name (node, nom, vertex);
          t_vertex [nom] = vertex;
          }
       else if (type=="Asso")
          {
          parseAssociation (node, vertex);
          }
       }

   rubrique = xml.findChild ("ListEdges");
   nbrelts  = count_children (rubrique);
   Edge* edge = NULL;

   for (int nro=0 ; nro < nbrelts ; nro++)
       {
       XmlTree*      node = rubrique->getChild (nro);
       const string& type = node->getName();
       if (type=="Edge")
          {
          const  string& nom      = node->findValue ("id");
          const  string& vertices = node->findValue ("vertices");
          get_names (vertices, V_TWO, tname);
          edge = new Edge (t_vertex [tname[0]], t_vertex [tname[1]]);
          t_edge [nom] = edge;
          parse_name (node, nom, edge);
          }
       else if (type=="Asso")
          {
          parseAssociation (node, edge);
          }
       }

   rubrique = xml.findChild ("ListQuads");
   nbrelts  = count_children (rubrique);
   Quad* quad = NULL;

   for (int nro=0 ; nro < nbrelts ; nro++)
       {
       XmlTree*      node = rubrique->getChild (nro);
       const string& type = node->getName();
       if (type=="Quad")
          {
          const string& nom   = node->findValue ("id");
          const string& edges = node->findValue ("edges");
          get_names (edges, V_TWO, tname);

          quad = new Quad (t_edge [tname[0]], t_edge [tname[1]],
                           t_edge [tname[2]], t_edge [tname[3]]);
          t_quad [nom] = quad;
          parse_name (node, nom, quad);
          }
       else if (type=="Asso")
          {
          parseAssociation (node, quad);
          }
       }

   rubrique = xml.findChild ("ListHexas");
   nbrelts  = count_children (rubrique);

   for (int nro=0 ; nro < nbrelts ; nro++)
       {
       XmlTree* node = rubrique->getChild (nro);
       const  string& nom   = node->findValue ("id");
       const  string& quads = node->findValue ("quads");
       get_names (quads, V_TWO, tname);

       Hexa* hexa =  new Hexa (t_quad [tname[0]], t_quad [tname[1]],
                                t_quad [tname[2]], t_quad [tname[3]],
                                t_quad [tname[4]], t_quad [tname[5]]);
       t_hexa [nom] = hexa;
       parse_name (node, nom, hexa);
       }

   rubrique = xml.findChild ("ListVectors");
   nbrelts  = count_children (rubrique);

   for (int nro=0 ; nro < nbrelts ; nro++)
       {
       XmlTree* node = rubrique->getChild (nro);
       double px, py, pz;
       const  string& nom    = node->findValue ("id");
       const  string& coords = node->findValue ("coord");
       get_coords (coords, px, py, pz);

       Vector* vector = addVector (px, py, pz);
       t_vector [nom] = vector;
       parse_name (node, nom, vector);
       }

   rubrique = xml.findChild ("ListDicretizationLaws");
   nbrelts  = count_children (rubrique);

   for (int nro=0 ; nro < nbrelts ; nro++)
       {
       XmlTree* node = rubrique->getChild (nro);
       const  string& id    = node->findValue ("id");
       const  string& kind  = node->findValue ("kind");
       const  string& nodes = node->findValue ("nodes");
       const  string& coeff = node->findValue ("coeff");

       int    nbnodes = atoi (nodes.c_str());
       double koeff   = atof (coeff.c_str());
       if (id != "DefaultLaw")
          {
          Law*   law  = addLaw (id.c_str(), nbnodes);
          law->setCoefficient (koeff);
          law->setKind (kind.c_str());
          }
       }

   rubrique = xml.findChild ("ListPropagations");
   nbrelts  = count_children (rubrique);

   for (int nro=0 ; nro < nbrelts ; nro++)
       {
       XmlTree* node = rubrique->getChild (nro);
       const  string& nmedge  = node->findValue ("edge");
       const  string& nmlaw   = node->findValue ("law");
       //  const  string& nmway   = node->findValue ("way");

       edge     = t_edge [nmedge];
       Law* law = findLaw (nmlaw.c_str());
       //  bool way = nmway == "true";

       if (edge != NULL)
           edge->setLaw (law);
       }

   rubrique = xml.findChild ("ListGroups");
   int nbrgroups  = count_children (rubrique);

   for (int nro=0 ; nro < nbrgroups ; nro++)
       {
       XmlTree*  ndgroup = rubrique->getChild (nro);
       XmlTree*  node    = ndgroup ->getChild (0);
       const  string& nom   = node->findValue ("name");
       const  string& ckind = node->findValue ("kind");

       EnumGroup kind   = Group::getKind (ckind);
       Group*    groupe = addGroup (nom.c_str(), kind);
       EnumElt   type   = groupe->getTypeElt ();

       nbrelts = count_children (ndgroup);
       for (int nelt=1 ; nelt < nbrelts ; nelt++)
           {
           node = ndgroup ->getChild (nelt);
           const string& id = node->findValue ("id");
           switch (type)
              {
              case EL_HEXA : groupe->addElement (t_hexa [id]);
                   break;
              case EL_QUAD : groupe->addElement (t_quad [id]);
                   break;
              case EL_EDGE : groupe->addElement (t_edge [id]);
                   break;
              case EL_VERTEX :
              default      : groupe->addElement (t_vertex [id]);
                   break;
              }
           }
       }


   return HOK;
}
// ======================================================== save
int Document::save (const char* ficxml)
{
   DumpStart ("save", ficxml);

   if (doc_xml==NULL)
       doc_xml = new XmlWriter ();

   int ier  = doc_xml->setFileName (ficxml);
   if (ier == HOK)
       ier =  genXml ();

   DumpReturn (ier);
   return ier;
}
// ======================================================== appendXml
int Document::appendXml (pfile fstudy)
{
   if (doc_xml==NULL)
       doc_xml = new XmlWriter ();

   doc_xml->setFile (fstudy);

   int    ier = genXml ();
   return ier;
}
// ======================================================== getXml
cpchar Document::getXml ()
{
   if (doc_xml==NULL)
       doc_xml = new XmlWriter ();

   doc_xml->setStream ();
   int ier = genXml ();
   if (ier!=HOK)
      return NULL;

   return doc_xml->getXml ();
}
// ======================================================== genXml
int Document::genXml ()
{
   const int HexVersion = 1;
                                       // -- 1) Raz numerotation precedente
   markAll (NO_COUNTED);
   if (maj_propagation)
       majPropagation ();

   if (doc_xml==NULL)
       doc_xml = new XmlWriter ();

   doc_xml->startXml ();
   doc_xml->openMark ("Document");
   doc_xml->addAttribute ("name",    el_name);
   doc_xml->addAttribute ("version", HexVersion);
   doc_xml->endMark ();

   cpchar balise [] = {"ListXXXX",
          "ListVertices", "ListEdges", "ListQuads", "ListHexas", "ListVectors",
          "ListXXXX" };

   for (int type=EL_VERTEX ; type <= EL_VECTOR ; type++)
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
   doc_xml->closeMark (true);

   int nombre = countGroup();
   doc_xml->addMark ("ListGroups");
   for (int nro=0 ; nro<nombre ; nro++)
       doc_group[nro]->saveXml (doc_xml);
   doc_xml->closeMark (true);

   nombre = countShape ();
   doc_xml->addMark ("ListShapes");
   for (int nro=0 ; nro<nombre ; nro++)
       doc_tab_shape[nro]->saveXml (doc_xml);
   doc_xml->closeMark ();

   doc_xml->closeMark ();
   doc_xml->closeXml  ();
   doc_saved = true;
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
int Document::saveVtk0 (cpchar nomfic)
{
                                           // -- 1) Raz numerotation precedente
   markAll (NO_COUNTED, EL_VERTEX);

   int nbnodes = 0;
   int nbcells = 0;

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

   pfile vtk = fopen (nomfic, "w");
   if (vtk==NULL)
      {
      cout << " ****" << endl;
      cout << " **** Document::saveVtk : " << endl;
      cout << " **** Can't open file "     << endl;
      cout << " ****" << endl;

      }
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

// ====================================================== saveVtk
// ==== Nouvelle formule qui conserve les numeros de vertex
int Document::saveVtk (cpchar nomfic)
{
   DumpStart ("saveVtk", nomfic);
   int nbnodes = doc_nbr_elt [EL_VERTEX];
   int nbcells = countHexa ();

   pfile vtk = fopen (nomfic, "w");
   if (vtk==NULL)
      {
      cout << " ****" << endl;
      cout << " **** Document::saveVtk : " << endl;
      cout << " **** Can't open file "     << endl;
      cout << " ****" << endl;

      }
   fprintf (vtk, "# vtk DataFile Version 3.1\n");
   fprintf (vtk, "%s \n", nomfic);
   fprintf (vtk, "ASCII\n");
   fprintf (vtk, "DATASET UNSTRUCTURED_GRID\n");
   fprintf (vtk, "POINTS %d float\n", nbnodes);

                                           // -- 1) Les noeuds
   Real3 koord;
   static const double minvtk = 1e-30;
#define Koord(p) koord[p]<minvtk && koord[p]>-minvtk ? 0 : koord[p]

   for (EltBase* elt = doc_first_elt[EL_VERTEX]->next (); elt!=NULL;
                 elt = elt->next())
       {
       Vertex* node = static_cast <Vertex*> (elt);
       if (node->isHere())
          {
          node->getPoint (koord);
          fprintf (vtk, "%g %g %g\n", Koord(dir_x), Koord(dir_y), Koord(dir_z));
          }
       else
          fprintf (vtk, "0 0 0\n");
       }
                                           // -- 2) Les hexas

   fprintf (vtk, "CELLS %d %d\n", nbcells, nbcells*(HV_MAXI+1));

   for (EltBase* elt = doc_first_elt[EL_HEXA]->next (); elt!=NULL;
                 elt = elt->next())
       {
       Hexa* cell = static_cast <Hexa*> (elt);
       if (cell!=NULL && cell->isHere())
          cell->printHexaVtk (vtk);
       }

   fprintf (vtk, "CELL_TYPES %d\n", nbcells);
   for (int nro=0 ; nro<nbcells ; nro++)
       fprintf (vtk, "%d\n", HE_MAXI);

   fprintf (vtk, "POINT_DATA %d \n", nbnodes);
   fprintf (vtk, "SCALARS A float\n");
   fprintf (vtk, "LOOKUP_TABLE default\n");
   for (EltBase* elt = doc_first_elt[EL_VERTEX]->next (); elt!=NULL;
                 elt = elt->next())
       {
       Vertex* node = static_cast <Vertex*> (elt);
       if (node->isHere())
          {
          double color = 100*(node->getScalar()+1);
          fprintf (vtk, "%g\n", color);
          }
       else 
          fprintf (vtk, "100\n");
       }

   fclose (vtk);
   DumpReturn (HOK);
   return HOK;
}
// ====================================================== purge
void Document::purge ()
{
   purge_elements = false;
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

/* ****************************************************************
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
   **************************************************************** */
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
           if (elt->isHere())
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
// ======================================================== parseSubShape
SubShape* Document::parseSubShape (XmlTree* node)
{
   const string& name  = node->findValue   ("shape");
   int           subid = node->findInteger ("subid");
   NewShape*     shape = findShape (name);
   if (shape==NULL)
       return NULL;

   SubShape* sub_shape = shape->findSubShape (subid);
   return    sub_shape;
}
// ======================================================== parseAssociation (v)
void Document::parseAssociation (XmlTree* node, Vertex* vertex)
{
   SubShape* shape = parseSubShape (node);
   if (shape==NULL || vertex==NULL)
       return;

   if (shape->getDim()!=0)
      return;
   VertexShape* vshape = static_cast <VertexShape*> (shape);
   vertex->setAssociation (vshape);
}
// ======================================================== parseAssociation (e)
void Document::parseAssociation (XmlTree* node, Edge* edge)
{
   SubShape* shape = parseSubShape (node);
   if (shape==NULL || edge==NULL)
       return;

   if (shape->getDim()!=1)
      return;

   const  string& inter = node->findValue ("interval");
   double pdeb, pfin;
   get_coords (inter, pdeb, pfin);

   EdgeShape* line = static_cast <EdgeShape*> (shape);
   edge->addAssociation (line, pdeb, pfin);
}
// ======================================================== parseAssociation (q)
void Document::parseAssociation (XmlTree* node, Quad* quad)
{
   SubShape* shape = parseSubShape (node);
   if (shape==NULL || quad==NULL)
       return;

   if (shape->getDim()!=2)
      return;
   FaceShape* face = static_cast <FaceShape*> (shape);
   quad->addAssociation (face);
}
// ======================================================== parseShapes
void Document::parseShapes (XmlTree& root)
{
   XmlTree* rubrique = root.findChild ("ListShapes");
   int nbrelts       = count_children (rubrique);

   for (int nro=0 ; nro < nbrelts ; nro++)
       {
       XmlTree*      node = rubrique->getChild (nro);
       const string& type = node->getName();
       if (type=="Shape")
          {
          const string& nom   = node->findValue   ("id"  );
          int           orig  = node->findInteger ("type");
          const string& brep  = node->findValue   ("brep");
          NewShape*     shape = new NewShape (this, (EnumShape)orig);

          parse_name (node, nom, shape);
          shape->setBrep (brep);
          doc_tab_shape.push_back (shape);
          }
       else if (type=="Cloud")
          {
          int nbvert = count_children (node);
          for (int nv=0 ; nv < nbvert ; nv++)
              {
              Real3    point;
              XmlTree* sommet = node->getChild (nv);
              const  string& coords = sommet->findValue ("coord");
              get_coords (coords, point[dir_x], point[dir_y], point[dir_z]);
              doc_cloud->addPoint (point);
              }
          }
       }
}
END_NAMESPACE_HEXA
