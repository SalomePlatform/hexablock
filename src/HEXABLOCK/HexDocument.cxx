
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

#include "HexDocument.hxx"

#include "HexEltBase.hxx"
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

#include "HexXmlWriter.hxx"
#include "HexXmlTree.hxx"
#include "HexGlobale.hxx"
#include "HexGroup.hxx"
#include "Hex.hxx"
#include "HexNewShape.hxx"

void test_six (Hex::Document* docu, int option);

BEGIN_NAMESPACE_HEXA

int Document::doc_number = 0;

// ======================================================== Constructeur
Document::Document (cpchar name, Hex* dad)
{
   el_name    = name;
   hex_parent = dad;
   glob       = Globale::getInstance ();
   el_root    = this;

   doc_db        = 0;
   nbr_errors    = 0;
   doc_modified  = true;
   doc_saved     = false;
   doc_tolerance = 1e-6;
   nbr_laws      = 0;
   nbr_propagations = 0;
   maj_propagation  = true;
   doc_xml          = NULL;

   nbr_used_hexas = nbr_used_quads = nbr_used_edges = nbr_used_vertex = 0;

   for (EnumElt type = EL_NONE ; type < EL_MAXI ; type=(EnumElt) (type+1))
       {
       doc_first_elt [type] = doc_last_elt [type] = new EltBase ();
       doc_nbr_elt   [type] = 0;
       }

   if (doc_number == 0)
      strcpy (doc_ident, "doc");
   else
      sprintf (doc_ident, "doc%02d", doc_number);
   doc_number++;

   DumpLock;
   defaultLaw = addLaw ("DefaultLaw", 0);
   DumpRestore;

   maj_connection = false;
   purge_elements = false;
   holes_elements = false;
   addCloud ();
}
// ======================================================== Destructeur
Document::~Document ()
{
   delete doc_xml;

   for (EnumElt type = EL_NONE ; type < EL_MAXI ; type=(EnumElt) (type+1))
       {
       //  printf ("____________________________ Type=%d\n", type);
       EltBase* next = NULL;
       for (EltBase* elt=doc_first_elt [type] ; elt != NULL ; elt=next)
           {
           next = elt->next();
           delete elt;
           }
       }
}
// ======================================================== addVector
Vector* Document::addVector (double dx, double dy, double dz)
{
   DumpStart ("addVector", dx  << dy  << dz);
   Vector* pv = new Vector (this, dx, dy, dz);
   doc_vector.push_back (pv);

   DumpReturn (pv);
   return  pv;
}
// ======================================================== addVectorVertices
Vector* Document::addVectorVertices (Vertex* va, Vertex* vb)
{
   DumpStart ("addVectorVertices", va  << vb);

   Vector* pv = addVector (vb->getX () - va->getX (),
                           vb->getY () - va->getY (),
                           vb->getZ () - va->getZ ());

   DumpReturn (pv);
   return  pv;
}
// ======================================================== addVertex
Vertex* Document::addVertex (double x, double y, double z)
{
   DumpStart ("addVertex", x  << y  << z);

   Vertex* pv   = new Vertex (this, x, y, z);
   DumpReturn (pv);
   return pv;
}
// ======================================================== addEdge
Edge* Document::addEdge (Vertex* va, Vertex* vb)
{
   DumpStart ("addEdge", va  << vb );

   if (va==NULL || vb==NULL)
      return NULL;

   Edge* arete  = new Edge (va, vb);
   DumpReturn (arete);
   return arete;
}
// ======================================================== addEdgeVector
Edge* Document::addEdgeVector (Vertex* va, Vector* vec)
{
   DumpStart ("addEdgeVector", va  << vec );

   Vertex* vb   = addVertex (va->getX () + vec->getDx(),
                             va->getY () + vec->getDy(),
                             va->getZ () + vec->getDz());

   Edge*  arete = addEdge (va, vb);
   DumpReturn (arete);
   return arete;
}
// ======================================================== addQuadVertices
Quad* Document::addQuadVertices (Vertex* va, Vertex* vb, Vertex* vc, Vertex* vd)
{
   DumpStart ("addQuadVertices", va << vb << vc << vd );

   Quad*  face   = new Quad (va, vb, vc, vd);
   DumpReturn (face);
   return face;
}
// ======================================================== addQuad
Quad* Document::addQuad (Edge* ea, Edge* eb, Edge* ec, Edge* ed)
{
   DumpStart ("addQuad", ea << eb << ec << ed );

   Quad*  face   = new Quad (ea, eb, ec, ed);
   DumpReturn (face);
   return face;
}
// ======================================================== addHexaVertices
Hexa* Document::addHexaVertices (Vertex* va, Vertex* vb, Vertex* vc, Vertex* vd,
                                 Vertex* ve, Vertex* vf, Vertex* vg, Vertex* vh)
{
   DumpStart ("addHexaVertices", va << vb << vc << vd << ve << vf << vg << vh);

   Hexa*  pave   = new Hexa (va, vb, vc, vd, ve, vf, vg, vh);
   DumpReturn (pave);
   return pave;
}
// ======================================================== addHexa
Hexa* Document::addHexa (Quad* qa, Quad* qb, Quad* qc, Quad* qd, Quad* qe,
                         Quad* qf)
{
   DumpStart ("addHexa", qa << qb << qc << qd << qe << qf );

   Hexa*  pave   = new Hexa (qa, qb, qc, qd, qe, qf);
   DumpReturn (pave);
   return pave;
}
// ======================================================== findVertex
Vertex* Document::findVertex (double vx, double vy, double vz)
{
   DumpStart ("findVertex", vx << vy << vz);
   Vertex* found = NULL;

   double xmin = vx - doc_tolerance;
   double xmax = vx + doc_tolerance;
   double ymin = vy - doc_tolerance;
   double ymax = vy + doc_tolerance;
   double zmin = vz - doc_tolerance;
   double zmax = vz + doc_tolerance;
 
   for (EltBase* elt = doc_first_elt[EL_VERTEX]->next (); 
                 elt!= NULL && found == NULL ;
                 elt = elt->next())
       {
       if (elt->isHere())
          {
          Vertex* candidat = static_cast <Vertex*> (elt);
          if (candidat->isin (xmin, xmax, ymin, ymax, zmin, zmax))
             found = candidat;
          }
       }

   DumpReturn (found);
   return found;
}
// ======================================================== findEdge
Edge* Document::findEdge (Vertex* v1, Vertex* v2)
{
   DumpStart ("findEdge", v1 << v2);
   Edge* found = NULL;

   for (EltBase* elt = doc_first_elt[EL_EDGE]->next (); 
                 elt!= NULL && found == NULL ;
                 elt = elt->next())
       {
       Edge* candidat  = static_cast <Edge*> (elt);
       if (candidat->definedBy (v1, v2))
          found = candidat;
       }

   DumpReturn (found);
   return found;
}
// ======================================================== findQuad
Quad* Document::findQuad (Vertex* v1, Vertex* v2)
{
   DumpStart ("findQuad", v1 << v2);
   Quad* found = NULL;

   for (EltBase* elt = doc_first_elt[EL_QUAD]->next (); 
                 elt!= NULL && found == NULL ;
                 elt = elt->next())
       {
       Quad* candidat  = static_cast <Quad*> (elt);
       if (candidat->definedBy (v1, v2))
          found = candidat;
       }

   DumpReturn (found);
   return found;
}
// ======================================================== findQuad
Quad* Document::findQuad (Edge* e1, Edge* e2)
{
   DumpStart ("findQuad", e1 << e2);
   Quad* found = NULL;

   for (EltBase* elt = doc_first_elt[EL_QUAD]->next (); 
                 elt!= NULL && found == NULL ;
                 elt = elt->next())
       {
       Quad* candidat  = static_cast <Quad*> (elt);
       if (candidat->definedBy (e1, e2))
          found = candidat;
       }

   DumpReturn (found);
   return found;
}
// ======================================================== findHexa
Hexa* Document::findHexa (Vertex* v1, Vertex* v2)
{
   DumpStart ("findHexa", v1 << v2);
   Hexa* found = NULL;

   for (EltBase* elt = doc_first_elt[EL_HEXA]->next (); 
                 elt!= NULL && found == NULL ;
                 elt = elt->next())
       {
       Hexa* candidat  = static_cast <Hexa*> (elt);
       if (candidat->definedBy (v1, v2))
          found = candidat;
       }

   DumpReturn (found);
   return found;
}
// ======================================================== findElement
EltBase* Document::findElement (EnumElt type, int ident)
{
   for (EltBase* elt = doc_first_elt[type]->next (); elt!=NULL;
                 elt = elt->next())
       {
       if (elt->isHere() && elt->getId()==ident)
          return elt;
       }
   return NULL;
}
// ======================================================== findVertex
Vertex* Document::findVertex (int ident)
{
   Vertex* node = static_cast <Vertex*> (findElement (EL_VERTEX, ident));
   return  node;

}
// ======================================================== index_tv
int index_tv (Vertex* table[], Vertex* elt)
{
   for (int nro=0; nro<QUAD4; nro++)
        if (elt == table[nro])
           return nro;

   return NOTHING;
}
// ======================================================== mergeQuads
int Document::mergeQuads (Quad* dest, Quad* orig, Vertex* v1, Vertex* v2,
                                                Vertex* v3, Vertex* v4)
{
   DumpStart ("mergeQuads", dest << orig << v1 << v2  << v3 << v4);

   update ();

   if (dest==orig)
      {
      printf (" *** Quads identiques dans mergeQuads\n");
      DumpEnd;
      return HERR;
      }
   else if (dest==NULL || dest->isDeleted())
      {
      printf (" *** Quad nro 1 incorrect dans mergeQuads \n");
      DumpEnd;
      return HERR;
      }
   else if (orig==NULL || orig->isDeleted() )
      {
      printf (" *** Quad nro 2 incorrect dans mergeQuads \n");
      DumpEnd;
      return HERR;
      }

   int nbcomm = 0;
   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       int norig = orig->indexVertex (dest->getVertex(nro));
       if (norig != NOTHING)
          {
          Vertex* uv = dest->getVertex(nro);
          char nom[12];
          nbcomm ++;
          if (nbcomm==1)
             {
             printf ("  +++ Sommets communs dans mergeQuads");
             printf (" (%s,",  dest->getName (nom));
             printf (" %s)\n", orig->getName (nom));
             }
          printf ("  +++ quad1[%d] = quad2[%d] = %s\n", nro,  norig,
                                                        uv->getName (nom));
          }
       }

   if (nbcomm == 2)
      {
      printf ("  +++ Les vertex passes en arguments sont ignores\n");
      int ier = closeQuads (dest, orig);
      DumpEnd;
      return ier;
      }
   else if (nbcomm != 0)
      {
      printf ("  *** _____________________ mergeQuads refuse: \n");
      printf (" il y a %d sommet(s) sont commun(s)\n", nbcomm);
      DumpEnd;
      return HERR;
      }

   if (   v1 ==NULL || v1 ->isDeleted() || v2 ==NULL || v2 ->isDeleted()
       || v3 ==NULL || v3 ->isDeleted() || v4 ==NULL || v4 ->isDeleted())
      {
      printf ("  *** _____________________ mergeQuads refuse: \n");
      printf (" un sommet est incorrect\n");
      DumpEnd;
      return HERR;
      }


   if (debug())
      {
      printf ("  ----------------- mergeQuads : \n");
      HexDump (orig);
      HexDump (dest);
      HexDump (v1);
      HexDump (v2);
      HexDump (v3);
      HexDump (v4);
      }

   Vertex *tv1 [QUAD4], *tv2 [QUAD4];
   Edge   *te1 [QUAD4], *te2 [QUAD4];

   int ier1 = dest->ordoVertex (v1, v3, tv1);
   int ier2 = orig->ordoVertex (v2, v4, tv2);
   if (ier1 != HOK)
      {
      DumpEnd;
      return ier1;
      }
   else if (ier2 != HOK)
      {
      DumpEnd;
      return ier2;
      }

   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       te1 [nro] = dest->getEdge(nro);
       Vertex* va1 = te1[nro]->getVertex(V_AMONT);
       Vertex* vb1 = te1[nro]->getVertex(V_AVAL);
       int na = index_tv  (tv1, va1);
       int nb = index_tv  (tv1, vb1);
       if (na==NOTHING || nb==NOTHING)
          {
          DumpEnd;
          return HERR;
          }

       te2 [nro] = orig->findEdge (tv2[na], tv2[nb]);
       if (te2[nro]==NULL)
          {
          DumpEnd;
          return HERR;
          }
       }

   if (debug())
      {
      printf ("  ----------------- Correspondances mergeQuads : \n");
      for (int nro=0 ; nro<QUAD4 ; nro++)
          {
          printf ("  %d  : ", nro);
          tv2 [nro]->printName(" -> ");
          tv1 [nro]->printName("\n");
          }
      for (int nro=0 ; nro<QUAD4 ; nro++)
          {
          printf ("  %d  : ", nro);
          te2 [nro]->printName(" (");
          te2 [nro]->getVertex(0)->printName(", ");
          te2 [nro]->getVertex(1)->printName(") -> ");
          te1 [nro]->printName(" (");
          te1 [nro]->getVertex(0)->printName(", ");
          te1 [nro]->getVertex(1)->printName(")\n");
          }
      }

   replaceQuad (orig, dest);
   for (int nro=0 ; nro<QUAD4 ; nro++)
       replaceEdge   (te2[nro], te1[nro]);
   for (int nro=0 ; nro<QUAD4 ; nro++)
       replaceVertex (tv2[nro], tv1[nro]);

   maj_connection = false;
   purge_elements = false;

   DumpEnd;
   return HOK;
}
// ======================================================== closeQuads
int Document::closeQuads (Quad* dest, Quad* orig)
{
   DumpStart ("closeQuads", dest << orig);

   update ();
   char nom[12];

   if (dest==orig)
      {
      printf (" *** Quads identiques dans closeQuads : %s\n",
                 dest->getName(nom));
      DumpEnd;
      return HERR;
      }
   else if (dest==NULL || dest->isDeleted())
      {
      printf (" *** Quad nro 1 incorrect dans closeQuads \n");
      DumpEnd;
      return HERR;
      }
   else if (orig==NULL || orig->isDeleted() )
      {
      printf (" *** Quad nro 2 incorrect dans closeQuads \n");
      DumpEnd;
      return HERR;
      }

   Edge* edc = NULL;
   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       int norig = orig->indexEdge (dest->getEdge(nro));
       if (norig != NOTHING)
          {
          if (edc != NULL)
             {
             printf ("  *** Plus d'une arete commune dans closeQuads");
             printf (" (%s,",  dest->getName (nom));
             printf (" %s)\n", orig->getName (nom));
             DumpEnd;
             return HERR;
             }
          edc = dest->getEdge (nro);
          printf ("  +++ quad1[%d] = quad2[%d] = %s\n", nro,  norig,
                                                        edc->getName (nom));
          }
       }

   Vertex* va = edc->getVertex (V_AMONT);
   Vertex* vb = edc->getVertex (V_AVAL);
   Vertex *tv1 [QUAD4], *tv2 [QUAD4];
   Edge   *te1 [QUAD4], *te2 [QUAD4];

   int ier1 = dest->ordoVertex (va, vb, tv1);
   int ier2 = orig->ordoVertex (va, vb, tv2);

   if (ier1 != HOK)
      {
      DumpEnd;
      return ier1;
      }
   else if (ier2 != HOK)
      {
      DumpEnd;
      return ier2;
      }

   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       te1 [nro] = dest->getEdge(nro);
       Vertex* va1 = te1[nro]->getVertex(V_AMONT);
       Vertex* vb1 = te1[nro]->getVertex(V_AVAL);
       int na = index_tv  (tv1, va1);
       int nb = index_tv  (tv1, vb1);
       if (na==NOTHING || nb==NOTHING)
          {
          DumpEnd;
          return HERR;
          }

       te2 [nro] = orig->findEdge (tv2[na], tv2[nb]);
       if (te2[nro]==NULL)
          {
          DumpEnd;
          return HERR;
          }
       }

   if (debug())
      {
      printf ("  ----------------- Correspondances mergeQuads : \n");
      for (int nro=0 ; nro<QUAD4 ; nro++)
          {
          printf ("  %d  : ", nro);
          tv2 [nro]->printName(" -> ");
          tv1 [nro]->printName("\n");
          }
      for (int nro=0 ; nro<QUAD4 ; nro++)
          {
          printf ("  %d  : ", nro);
          te2 [nro]->printName(" (");
          te2 [nro]->getVertex(0)->printName(", ");
          te2 [nro]->getVertex(1)->printName(") -> ");
          te1 [nro]->printName(" (");
          te1 [nro]->getVertex(0)->printName(", ");
          te1 [nro]->getVertex(1)->printName(")\n");
          }
      }

   replaceQuad (orig, dest);
   for (int nro=0 ; nro<QUAD4 ; nro++)
       replaceEdge   (te2[nro], te1[nro]);
   for (int nro=0 ; nro<QUAD4 ; nro++)
       replaceVertex (tv2[nro], tv1[nro]);

   maj_connection = false;
   purge_elements = false;

   DumpEnd;
   return HOK;
}
// ======================================================== mergeEdges
int Document::mergeEdges (Edge* e1, Edge* e2, Vertex* v1, Vertex* v2)
{
   DumpStart ("mergeEdges", e1 << e2 << v1 << v2);

   if (e1==NULL || e1->isDeleted() || e2==NULL || e2->isDeleted())
      {
      DumpEnd;
      return HERR;
      }

   for (int nro=0 ; nro<V_TWO ; nro++)
       if (e1->index (e2->getVertex(nro))!= NOTHING)
          {
          DumpEnd;
          return HERR+nro+1;
          }

   Vertex *tv1 [V_TWO], *tv2 [V_TWO];

   int ier1 = e1->anaMerge (v1, tv1);
   int ier2 = e2->anaMerge (v2, tv2);

   if (ier1 != HOK)
      {
      DumpEnd;
      return ier1;
      }
   else if (ier2 != HOK)
      {
      DumpEnd;
      return ier2;
      }

   replaceEdge (e2, e1);
   for (int nro=0 ; nro<V_TWO ; nro++)
       replaceVertex (tv2[nro], tv1[nro]);

   maj_connection = false;
   DumpEnd;
   return HOK;
}
// ======================================================== mergeVertices
int Document::mergeVertices (Vertex* vpar, Vertex* vold)
{
   DumpStart ("mergeVertices", vpar << vold);

   int ier = HOK;
   if (BadElement (vpar))
      {
      Mess << "First vertex is not valid";
      ier = HERR;
      }
   else if (BadElement (vold))
      {
      Mess << "Second vertex is not valid";
      ier = HERR;
      }
   else if (vpar==vold)
      {
      Mess << "Vertices are identical";
      ier = HERR;
      }
   else if (EltIsValid (findEdge (vpar, vold)))
      {
      Mess << "Theese vertices are connected by an edge";
      ier = HERR;
      }

   if (ier != HOK)
      {
      DumpEnd;
      return ier;
      }

   map <Quad*,   Quad*>   rep_quad;
   map <Edge*,   Edge*>   rep_edge;
   map <Vertex*, Vertex*> rep_vertex;

   map <Quad*,   Quad*>   :: iterator itq;
   map <Edge*,   Edge*>   :: iterator ited;
   map <Vertex*, Vertex*> :: iterator itv;
   
   rep_vertex [vold] = vpar;
   int nbparv  = vold->getNbrParents ();

   for (int ned=0 ; ned<nbparv ; ++ned)
       {
       Edge* edold = vold->getParent (ned);
       if (edold->isHere())
          {
          Vertex* vopp  = edold->opposedVertex (vold);
          Edge*   edpar = findEdge (vpar, vopp);
          if (edpar != NULL)
             rep_edge [edold] = edpar;
          }
       }

   Quad *qold=NULL,  *qpar=NULL;

   for (ited=rep_edge.begin(); ited!=rep_edge.end() ; ++ited)
       {
       Edge* edold = ited->first;
       Edge* edpar = ited->second;
       int nbpared = edold->getNbrParents ();
       for (int nq=0 ; nq<nbpared ; ++nq)
           {
           qold = edold->getParent (nq);
           if (qold->isHere())
              {
              for (int ned=0 ; ned<QUAD4 ; ned++)
                  {
                  // Edge* edge = quad->getEdge(ned);
                  Edge* edge = qold->getEdge (ned);
                  if (edge != edold)
                     {
                     qpar = findQuad (edpar, edge);
                     if (qpar != NULL)
                        rep_quad [qold] = qpar;
                     }
                  }
              }
           }
       }

   cout << " +++ Intersection : veq = "   << rep_vertex.size() 
        << " " << rep_edge.size() <<  " " << rep_quad.size()
        << endl;

   for (itq=rep_quad.begin(); itq!=rep_quad.end() ; ++itq)
      {
      qold = itq->first;
      qpar = itq->second;
      Vertex *tv1[QUAD4], *tv2[QUAD4];
      Edge   *te1[QUAD4], *te2[QUAD4];
      Edge*  einter = qpar->inter (qold);
      if (einter!=NULL)
         {
         Vertex* va = einter->getVertex (V_AMONT);
         Vertex* vb = einter->getVertex (V_AVAL);
         qpar->ordonner (va, vb, tv1, te1); 
         qold->ordonner (va, vb, tv2, te2); 
         for (int nro=0 ; nro<QUAD4 ; nro++)
             {
             rep_edge   [te2[nro]] =  te1[nro];
             rep_vertex [tv2[nro]] =  tv1[nro];
             }
         }
      replaceQuad (qold, qpar);
      }

   for (ited=rep_edge.begin(); ited!=rep_edge.end() ; ++ited)
        replaceEdge (ited->first, ited->second);
       
   for (itv=rep_vertex.begin(); itv!=rep_vertex.end() ; ++itv)
        replaceVertex (itv->first, itv->second);
       
   maj_connection = false;
   DumpEnd;
   return HOK;
}
// ======================================================== replaceVertex
void Document::replaceVertex (Vertex* old, Vertex* par)
{
   if (BadElement(old) || BadElement(par) ||  old==par)
      return ;

   // par->replaceAssociation (old);   TODO

   for (int type=EL_EDGE ; type <= EL_HEXA ; type++)
       {
       for (EltBase* elt = doc_first_elt[type]->next (); elt!=NULL;
                     elt = elt->next())
           if (elt->isHere ())
               elt->replaceVertex (old, par);
       }
   old->suppress ();
}
// ======================================================== replaceEdge
void Document::replaceEdge (Edge* old, Edge* par)
{
   if (BadElement(old) || BadElement(par) ||  old==par)
      return;

   // par->replaceAssociation (old);   TODO

   for (int type=EL_QUAD ; type <= EL_HEXA ; type++)
       {
       for (EltBase* elt = doc_first_elt[type]->next (); elt!=NULL;
                     elt = elt->next())
           if (elt->isHere ())
               elt->replaceEdge (old, par);
       }

   old->suppress ();
}
// ======================================================== replaceQuad
int Document::replaceQuad (Quad* old, Quad* par)
{
   if (BadElement(old) || BadElement(par) ||  old==par)
      return HERR;

   // par->replaceAssociation (old);   TODO

   for (EltBase* elt = doc_first_elt[EL_HEXA]->next (); elt!=NULL;
                 elt = elt->next())
       if (elt->isHere ())
          {
          Hexa* cell = static_cast <Hexa*> (elt);
          cell->replaceQuad (old, par);
          }
   old->suppress ();
   return HOK;
}
// ========================================================== countElement
int Document::countElement (EnumElt type)
{
   int compteur = 0;
   for (EltBase* elt = doc_first_elt[type]->next (); elt!=NULL;
                 elt = elt->next())
       if (elt->isHere())
          compteur ++;

   return compteur;
}
// ========================================================== getElement
EltBase* Document::getElement (EnumElt type, int nro)
{
   int compteur = 0;
   for (EltBase* elt = doc_first_elt[type]->next (); elt!=NULL;
                 elt = elt->next())
       {
       if (elt->isHere())
          {
          if (compteur>=nro)
             return elt;
          compteur ++;
          }
       }
   return NULL;
}
// ========================================================= addLaw
Law* Document::addLaw (const char* name, int nbnodes)
{
   DumpStart ("addLaw", name << nbnodes);
   Law* loi = new Law (this, name, nbnodes);
   addLaw (loi);
   DumpReturn (loi);
   return loi;
}
// ========================================================= addLaw
Law* Document::addLaw (Law* loi)
{
   doc_laws.push_back (loi);
   nbr_laws ++;
   return loi;
}
// ========================================================= getLaw
Law* Document::getLaw (int nro)
{
   DumpStart ("getLaw", nro);
   Law* loi = NULL;

   if (nro >= 0 && nro < nbr_laws)
       loi =  doc_laws [nro];

   DumpReturn (loi);
   return loi;
}
// ========================================================= findLaw
Law* Document::findLaw (const char* name)
{
   DumpStart ("findLaw", name);

   string nom = name;
   Law*   loi = NULL;

   for (int nro=0 ;loi==NULL &&  nro<nbr_laws; nro++)
       if (doc_laws [nro]->getName() == nom)
          loi = doc_laws [nro];

   DumpReturn (loi);
   return loi;
}
// ========================================================= removeLaw
int Document::removeLaw (Law* loi)
{
   DumpStart ("removeLaw", loi);

   for (int nro=1 ; nro<nbr_laws; nro++)
       if (doc_laws [nro] == loi)
          {
          //All propagations having this law should now have the default law.
          for (int nl=0 ; nl<nbr_propagations ; nl++)
              {
              if ( doc_propagation [nl]->getLaw() == loi )
                   doc_propagation [nl]->setLaw(defaultLaw);
              }

          // delete doc_laws [nro];
          doc_laws [nro]->suppress ();
          doc_laws.erase (doc_laws.begin()+nro);
          nbr_laws = doc_laws.size();

          DumpReturn (HOK);
          return HOK;
          }

   DumpReturn (HERR);
   return HERR;
}
// ========================================================= majPropagation
void Document::majPropagation ()
{
   DumpLock;
   majReferences ();
   update ();
   // if (purge_elements)
       // purge ();

   for (int nro=0 ; nro<nbr_propagations ; nro++)
       {
       // delete doc_propagation [nro];
       // doc_propagation [nro]->suppress();
       ;
       }

   doc_propagation.clear ();
   nbr_propagations = 0;
   maj_propagation  = false;

   for (EltBase* elt = doc_first_elt[EL_EDGE]->next (); elt!=NULL;
                 elt = elt->next())
       {
       Edge* arete = static_cast <Edge*> (elt);
       if (arete!=NULL)
           arete->setPropag (NOTHING, true);
       }

   //  markAll (NO_COUNTED, EL_EDGE);

   for (EltBase* elt = doc_first_elt[EL_HEXA]->next (); elt!=NULL;
                 elt = elt->next())
       {
       Hexa* cell = static_cast <Hexa*> (elt);
       if (cell!=NULL && cell->isHere())
          {
          for (int ne=0 ; ne<HE_MAXI ; ne++)
              {
              Edge* arete = cell->getEdge(ne);
              if (arete->getPropag()<0)
                 {
                 Propagation* prop = new Propagation (this);
                 doc_propagation.push_back (prop);
                 arete->propager (prop, nbr_propagations);
                 nbr_propagations ++;
                 prop->majLaw();
                 }
              }
          }
       }
   maj_propagation  = false;
   DumpRestore;
}
// ======================================================== countPropagation
int Document::countPropagation ()
{
   if (maj_propagation)
       majPropagation ();

   return nbr_propagations;
}
// ======================================================== getPropagation
Propagation* Document::getPropagation (int nro)
{
   DumpStart ("getPropagation", nro);

   if (maj_propagation)
       majPropagation ();

   Propagation* prop = NULL;
   if (nro >= 0 && nro < nbr_propagations)
       prop = doc_propagation [nro];

   DumpReturn (prop);
   return prop;
}
// ======================================================== findPropagation
Propagation* Document::findPropagation (Edge* arete)
{
   DumpStart ("findPropagation", arete);

   Propagation* prop = NULL;
   if (arete!=NULL)
      {
      if (maj_propagation)
          majPropagation ();
      prop = getPropagation (arete->getPropag ());
      }

   DumpReturn (prop);
   return  prop;
}
// ======================================================== disconnectQuad
Elements* Document::disconnectQuad (Hexa* cell, Quad* element)
{
   DumpStart ("disconnectQuad", cell << element);

   Elements* grid = new Elements (this);
   grid->checkDisco (cell, element);

   if (grid->isValid())
      {
      update ();
      cell->disconnectQuad (element, grid);
      majReferences ();
      }

   DumpReturn (grid);
   return grid;
}
// ======================================================== disconnectEdge
Elements* Document::disconnectEdge (Hexa* cell, Edge* element)
{
   DumpStart ("disconnectEdge", cell << element);

   Elements* grid = new Elements (this);
   grid->checkDisco (cell, element);

   if (grid->isValid())
      {
      update ();
      cell->disconnectEdge (element, grid);
      majReferences ();
      }

   DumpReturn (grid);
   return grid;
}
// ======================================================== disconnectVertex
Elements* Document::disconnectVertex (Hexa* cell, Vertex* element)
{
   DumpStart ("disconnectVertex", cell << element);

   Elements* grid = new Elements (this);
   grid->checkDisco (cell, element);

   if (grid->isValid())
      {
      update ();
      cell->disconnectVertex (element, grid);
      majReferences ();
      }

   DumpReturn (grid);
   return grid;
}
// ======================================================== setName
int Document::setName (cpchar prefix)
{
   DumpStart ("setName", prefix);

   int ier = HOK;
   if (Cestvide (prefix) || el_name==prefix)
       ier = HERR;
   else
       {
       string name = prefix;
       if (hex_parent != NULL)
           hex_parent->makeName (prefix, name);
       el_name = name;
       }

   DumpReturn (ier);
   return ier ;
}
// ======================================================== setLevel

#ifdef  NO_CASCADE
#define _TEST_BICYL
#endif

void Document::setLevel (int niv)
{
   DumpStart ("setLevel", niv);

   if (niv == 747)
      checkAssociations ();
   else if (niv == 748)
      {
      int nbshapes = countShape ();
      for (int nsh=0 ; nsh < nbshapes ; nsh++)
          {
          doc_tab_shape[nsh]->saveBrep();
          }
      }
   else if (niv == 777)
      set_special_option (true);
   else if (niv == 778)
      set_special_option (false);

   else if (niv >=100 && niv <=200)
      test_six (this, niv);
   else
      {
      doc_db = niv;
      }

   DumpEnd;
}
// ======================================================== makeVarName
char* Document::makeVarName (char* name)
{
   strcpy (name, doc_ident);
   return doc_ident ;
}
// ======================================================== isEmpty
bool Document::isEmpty ()
{
   int nombre = countVertex () + countVector ();
   return nombre <= 0 && countLaw  () <= 1;
}
// ======================================================== getNextName
cpchar Document::getNextName (EnumElt type, string& buff)
{
   char name [8];
   EltBase::makeName (type, doc_nbr_elt [type], name);

   buff = name;
   return buff.c_str();
}

// ======================================================== getNextName
string Document::getNextName (EnumElt type)
{
   char name [8];
   EltBase::makeName (type, doc_nbr_elt [type], name);

   return string(name);
}

// ======================================================== lockDump
void Document::lockDump ()
{
   glob->dump.lock ();
}
// ======================================================== restoreDump
void Document::restoreDump ()
{
   glob->dump.restore (DumpActif);
}
// ======================================================== getGroup
Group* Document::getGroup (int nro)
{
   int size = doc_group.size();
   if (nro>=0 && nro<size)
      return doc_group [nro];
   else
      return NULL;
}
// ======================================================== getVector
Vector* Document::getVector (int nro)
{
   int size = doc_vector.size();
   if (nro>=0 && nro<size)
      return doc_vector [nro];
   else
      return NULL;
}
END_NAMESPACE_HEXA
