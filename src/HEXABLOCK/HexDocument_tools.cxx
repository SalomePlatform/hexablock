
// C++ : Classe Document : Methodes internes 2011

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
#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexQuad.hxx"
#include "HexHexa.hxx"
#include "HexGlobale.hxx"
#include "HexGroup.hxx"

BEGIN_NAMESPACE_HEXA

// ======================================================== countHexa
int Document::countHexa ()
{
   DumpStart0 ("countHexa");
   if (holes_elements)
       renumeroter ();

   int nombre =  doc_hexas.size();
   DumpReturn (nombre);
   return nombre;
}
// ======================================================== countQuad
int Document::countQuad ()
{
   DumpStart0 ("countQuad");
   if (holes_elements)
       renumeroter ();

   int nombre =  doc_quads.size();
   DumpReturn (nombre);
   return nombre;
}
// ======================================================== countEdge
int Document::countEdge ()
{
   DumpStart0 ("countEdge");
   if (holes_elements)
       renumeroter ();

   int nombre =  doc_edges.size();
   DumpReturn (nombre);
   return nombre;
}
// ======================================================== countVertex
int Document::countVertex ()
{
   DumpStart0 ("countVertex");
   if (holes_elements)
       renumeroter ();

   int nombre =  doc_vertex.size();
   DumpReturn (nombre);
   return nombre;
}

// ======================================================== getHexa
Hexa* Document::getHexa (int nro)
{
   if (holes_elements)
       renumeroter ();

   if (nro<0 || nro >= (int) doc_hexas.size())
      return NULL;

   return doc_hexas [nro];
}
// ======================================================== getQuad
Quad* Document::getQuad (int nro)
{
   if (holes_elements)
       renumeroter ();

   if (nro<0 || nro >= (int) doc_quads.size())
      return NULL;

   return doc_quads [nro];
}
// ======================================================== getEdge
Edge* Document::getEdge (int nro)
{
   if (holes_elements)
       renumeroter ();

   if (nro<0 || nro >= (int) doc_edges.size())
      return NULL;

   return doc_edges [nro];
}
// ======================================================== getVertex
Vertex* Document::getVertex (int nro)
{
   if (holes_elements)
       renumeroter ();

   if (nro<0 || nro >= (int) doc_vertex.size())
      return NULL;

   return doc_vertex [nro];
}
// ======================================================== renumeroter
void Document::renumeroter ()
{
   int nbelts = doc_vertex.size();
   int decal  = 0;
   for (int nro=0 ; nro<nbelts ; nro++)
       {
       Vertex* elt = doc_vertex [nro];
       if (elt->isDeleted())
          decal ++;
       else if (decal != 0)
          doc_vertex [nro-decal] = elt;
       }

   if (decal != 0) 
       doc_vertex.resize (nbelts-decal);
                                        // ----------------- Edges 
   nbelts = doc_edges.size();
   decal  = 0;
   for (int nro=0 ; nro<nbelts ; nro++)
       {
       Edge* elt = doc_edges [nro];
       if (elt->isDeleted())
          decal ++;
       else if (decal != 0)
          doc_edges [nro-decal] = elt;
       }

   if (decal != 0) 
       doc_edges.resize (nbelts-decal);
                                        // ----------------- Quads 
   nbelts = doc_quads.size();
   decal  = 0;
   for (int nro=0 ; nro<nbelts ; nro++)
       {
       Quad* elt = doc_quads [nro];
       if (elt->isDeleted())
          decal ++;
       else if (decal != 0)
          doc_quads [nro-decal] = elt;
       }

   if (decal != 0) 
       doc_quads.resize (nbelts-decal);
                                        // ----------------- Hexas 
   nbelts = doc_hexas.size();
   decal  = 0;
   for (int nro=0 ; nro<nbelts ; nro++)
       {
       Hexa* elt = doc_hexas [nro];
       if (elt->isDeleted())
          decal ++;
       else if (decal != 0)
          doc_hexas [nro-decal] = elt;
       }

   if (decal != 0) 
       doc_hexas.resize (nbelts-decal);
}
// ----------------------------------------------------------------------------
// ======================================================== countUsedHexa
int Document::countUsedHexa ()
{
   if (count_modified)
       recountUsed ();

   return nbr_used_hexas;
}
// ======================================================== countUsedQuad
int Document::countUsedQuad ()
{
   if (count_modified)
       recountUsed ();

   return nbr_used_quads;
}
// ======================================================== countUsedEdge
int Document::countUsedEdge ()
{
   if (count_modified)
       recountUsed ();

   return nbr_used_edges;
}
// ======================================================== countUsedVertex
int Document::countUsedVertex ()
{
   if (count_modified)
       recountUsed ();

   return nbr_used_vertex;
}

// ======================================================== getUsedHexa
Hexa* Document::getUsedHexa (int nro)
{
   if (count_modified)
       recountUsed ();

   if (nro<0 || nro >= nbr_used_hexas)
      return NULL;

   return doc_used_hexas [nro];
}
// ======================================================== getUsedQuad
Quad* Document::getUsedQuad (int nro)
{
   if (count_modified)
       recountUsed ();

   if (nro<0 || nro >= nbr_used_quads)
      return NULL;

   return doc_used_quads [nro];
}
// ======================================================== getUsedEdge
Edge* Document::getUsedEdge (int nro)
{
   if (count_modified)
       recountUsed ();

   if (nro<0 || nro >= nbr_used_edges)
      return NULL;

   return doc_used_edges [nro];
}
// ======================================================== getUsedVertex
Vertex* Document::getUsedVertex (int nro)
{
   if (count_modified)
       recountUsed ();

   if (nro<0 || nro >= nbr_used_vertex)
      return NULL;

   return doc_used_vertex [nro];
}
// ======================================================== recountUsed
void Document::recountUsed ()
{
   count_modified = false;
                                       // -- 1) Raz numerotation precedente
   markAll (NO_COUNTED);

   doc_used_hexas .clear ();
   doc_used_quads .clear ();
   doc_used_edges .clear ();
   doc_used_vertex.clear ();

   for (EltBase* elt = doc_first_elt[EL_HEXA]->next (); elt!=NULL;
                 elt = elt->next())
       {
       if (elt!=NULL && elt->isHere())
          {
          Hexa* cell = static_cast <Hexa*> (elt);
          doc_used_hexas.push_back (cell);
          for (int nb=0 ; nb<HQ_MAXI ; nb++)
              cell->getQuad (nb)->setMark (IS_USED);

          for (int nb=0 ; nb<HE_MAXI ; nb++)
              cell->getEdge (nb)->setMark (IS_USED);

          for (int nb=0 ; nb<HV_MAXI ; nb++)
              cell->getVertex (nb)->setMark (IS_USED);
          }
       }

   for (EltBase* elt = doc_first_elt[EL_QUAD]->next (); elt!=NULL;
                 elt = elt->next())
       {
       if (elt!=NULL && elt->isHere() && elt->getMark()==IS_USED)
          {
          Quad* cell = static_cast <Quad*> (elt);
          doc_used_quads.push_back (cell);
          }
       }

   for (EltBase* elt = doc_first_elt[EL_EDGE]->next (); elt!=NULL;
                 elt = elt->next())
       {
       if (elt!=NULL && elt->isHere() && elt->getMark()==IS_USED)
          {
          Edge* cell = static_cast <Edge*> (elt);
          doc_used_edges.push_back (cell);
          }
       }

   for (EltBase* elt = doc_first_elt[EL_VERTEX]->next (); elt!=NULL;
                 elt = elt->next())
       {
       if (elt!=NULL && elt->isHere() && elt->getMark()==IS_USED)
          {
          Vertex* cell = static_cast <Vertex*> (elt);
          doc_used_vertex.push_back (cell);
          }
       }

   nbr_used_hexas  = doc_used_hexas .size ();
   nbr_used_quads  = doc_used_quads .size ();
   nbr_used_edges  = doc_used_edges .size ();
   nbr_used_vertex = doc_used_vertex .size ();
}
// ========================================================= saveVtk (avec nro)
int Document::saveVtk  (cpchar radical, int &nro)
{
   char num[8];
   sprintf (num, "%d", nro);
   nro ++;

   string filename = radical;
   filename += num;
   filename += ".vtk";
   int ier = saveVtk (filename.c_str());
   return ier;
}
// ========================================================= setDeprecated
void Document::setDeprecated (int level)
{
   switch (level)
          {
                         // En cas de destruction  : parents invalides
          case 2 : maj_connection  = true;
                   purge_elements  = true;
                   holes_elements  = true;
                         // creation + destruction : propagations a revoir
          case 1 : maj_propagation = true;
                         // Par defaut :
          default: doc_modified    = true;
                   count_modified  = true;
          }
}

// ========================================================= addHexaGroup 
Group* Document::addHexaGroup (cpchar name)
{
   DumpStart ("addHexaGroup", name);
   Group* groupe = addGroup (name, HexaCell);

   DumpReturn (groupe);
   return groupe;
}
// ========================================================= addHexaNodeGroup 
Group* Document::addHexaNodeGroup (cpchar name)
{
   DumpStart ("addHexaNodeGroup", name);
   Group* groupe = addGroup (name, HexaNode);

   DumpReturn (groupe);
   return groupe;
}
// ========================================================= addQuadGroup 
Group* Document::addQuadGroup (cpchar name)
{
   DumpStart ("addQuadGroup", name);
   Group* groupe = addGroup (name, QuadCell);

   DumpReturn (groupe);
   return groupe;
}
// ========================================================= addQuadNodeGroup 
Group* Document::addQuadNodeGroup (cpchar name)
{
   DumpStart ("addQuadNodeGroup", name);
   Group* groupe = addGroup (name, QuadNode);

   DumpReturn (groupe);
   return groupe;
}
// ========================================================= addEdgeGroup 
Group* Document::addEdgeGroup (cpchar name)
{
   DumpStart ("addEdgeGroup", name);
   Group* groupe = addGroup (name, EdgeCell);

   DumpReturn (groupe);
   return groupe;
}
// ========================================================= addEdgeNodeGroup 
Group* Document::addEdgeNodeGroup (cpchar name)
{
   DumpStart ("addEdgeNodeGroup", name);
   Group* groupe = addGroup (name, EdgeNode);

   DumpReturn (groupe);
   return groupe;
}
// ========================================================= addVertexNodeGroup 
Group* Document::addVertexNodeGroup (cpchar name)
{
   DumpStart ("addVertexNodeGroup", name);
   Group* groupe = addGroup (name, VertexNode);

   DumpReturn (groupe);
   return groupe;
}
// ======================================================== addGroup
Group* Document::addGroup (cpchar name, EnumGroup kind)
{
   Group* grp = new Group (this, name, kind);
   doc_group.push_back (grp);
   return grp;
}
// ======================================================== findGroup
Group* Document::findGroup (cpchar name)
{
   DumpStart ("findGroup", name);

   int    nbre   = doc_group.size();
   Group* groupe = NULL;

   for (int ng=0 ; groupe==NULL && ng<nbre ; ng++)
       if (Cestegal (doc_group [ng]->getName(), name))
           groupe = doc_group [ng];

   DumpReturn (groupe);
   return groupe;
}
// ======================================================== removeGroup
int Document::removeGroup (Group* grp)
{
   DumpStart ("removeGroup", grp );

   int  nbre = doc_group.size();
   bool more = true;
   int  ier  = HERR;

   for (int ng=0 ; more && ng<nbre ; ng++)
       {
       if (grp == doc_group [ng])
          {
          doc_group.erase (doc_group.begin() + ng);
          more = false;
          ier  = HOK;
          }
       }
                      // Si pas trouve dans la liste. On detruit quand meme

   if (NOT (BadElement (grp)))
      grp->suppress ();

   DumpReturn (ier);
   return ier;
}
// ----------------------------------------------------------------------------
END_NAMESPACE_HEXA
