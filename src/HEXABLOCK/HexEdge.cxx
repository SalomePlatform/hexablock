
// C++ : Gestion des aretes

// Copyright (C) 2009-2012  CEA/DEN, EDF R&D
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
#include "HexEdge.hxx"
#include "HexVertex.hxx"
#include "HexQuad.hxx"

#include "HexPropagation.hxx"
#include "HexXmlWriter.hxx"
#include "HexShape.hxx"

static int niveau = 0;

BEGIN_NAMESPACE_HEXA

void geom_dump_asso (Edge* edge);

// ======================================================== Constructeur
Edge::Edge (Vertex* va, Vertex* vb)
    : EltBase (va->dad(), EL_EDGE)
{
   e_vertex [V_AMONT] = va; 
   e_vertex [V_AVAL ] = vb;

   e_propag = NOTHING;
   e_way    = true;
   e_law    = NULL;
   e_clone  = NULL;

   if (isBad())
      return;
   else if (BadElement (va) || BadElement (vb))
      {
      setError ();
      return;
      }
   else if (va == vb)
      {
      setError ();
      return;
      }

   majReferences ();
}
// ======================================================== Constructeur 2
Edge::Edge (Edge* other)
    : EltBase (other->dad(), EL_EDGE)
{
   e_vertex [V_AMONT] = e_vertex [V_AVAL ] = NULL;
   if (BadElement (other))
      setError ();

   e_propag = NOTHING;
   e_way    = true;
   e_law    = NULL;
   e_clone  = NULL;
}
// ======================================================== majReferences
void Edge::majReferences ()
{
   e_vertex [V_AMONT]->addParent (this);
   e_vertex [V_AVAL ]->addParent (this);
}
// ======================================================== anaMerge
int Edge::anaMerge (Vertex* orig, Vertex* tv1[])
{
   tv1 [0] = orig;

   if (orig == e_vertex [V_AMONT])
      tv1 [1] = e_vertex[V_AVAL];
   else if (orig == e_vertex [V_AVAL])
      tv1 [1] = e_vertex[V_AMONT];
   else 
      return HERR;

   return HOK;
}
// ========================================================= propager 
void Edge::propager (Propagation* prop, int groupe, int sens)
{
   setPropag (groupe, sens>0);
   prop->addEdge (this);

   niveau ++;
   int nbquads = getNbrParents ();
   for (int nq=0 ; nq<nbquads ; nq++)
       {
       Quad* quad = getParent (nq);
       if (quad!=NULL && quad->hasParents())
          {
          if (debug (6))
             {
             printf ("Prop : niv=%d edge=", niveau);
             printName ();
             printf ("parent=%d quad=", nq);
             quad->printName ("\n");
	     fflush(stdout);
             }

          int meme = 1;
          Edge* arete = quad->getOpposEdge (this, meme);
          if (arete != NULL && arete->getPropag () < 0)
              arete->propager (prop, groupe, meme*sens);
          }
       }
   niveau --;
}
// ========================================================= getParent 
Quad* Edge::getParent  (int nro)
{
   return static_cast <Quad*> (getFather (nro));
}
// ========================================================= saveXml 
void Edge::saveXml (XmlWriter* xml)
{
   char buffer[12];
   string vertices = e_vertex [V_AMONT]->getName(buffer);
   vertices += " ";
   vertices       += e_vertex [V_AVAL ]->getName(buffer);

   xml->openMark     ("Edge");
   xml->addAttribute ("id",       getName (buffer));
   xml->addAttribute ("vertices", vertices);
   if (el_name!=buffer) 
       xml->addAttribute ("name", el_name);
   xml->closeMark ();

   int nbass = tab_assoc.size();
   for (int nro=0 ; nro<nbass ; nro++)
       if (tab_assoc[nro] != NULL)
           tab_assoc[nro]->saveXml (xml); 
}
// ======================================================== replaceVertex
void Edge::replaceVertex (Vertex* old, Vertex* par)
{
   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       if (e_vertex[nro]==old) 
           {
           e_vertex[nro] = par;
	   if (debug())
	      {
              printf (" Dans ");
              printName ();
              printf (" [%d], ", nro);
              old->printName (" est remplace par ");
              par->printName ("\n");
              }
           }
       }
}
// ======================================================== dump
void Edge::dump ()
{
   printName(" = (");
   if (NOT isHere ())
      {
      printf ("*** deleted ***)\n");
      return;
      }

   e_vertex[V_AMONT]->printName(", ");
   e_vertex[V_AVAL] ->printName(")");
   if (e_propag>=0)
      {
      char signe = e_way ? '+' : '-';
      printf (", Prop= %c%d", signe, e_propag);
      }

   dumpRef ();
}
// ======================================================== dumpAsso
void Edge::dumpAsso ()
{
   if (NOT isHere ())
      {
      printName(" = *** deleted **\n");
      return;
      }

   geom_dump_asso (this);
}
// ======================================================== dumpPlus
void Edge::dumpPlus ()
{
   dump ();
   for (int nro=0 ; nro < V_TWO ; nro++)
       {
       Vertex* pv = e_vertex[nro];
       printf ( "    ");
       if (pv!=NULL)
          {
          pv->printName ();
          printf (" (%g, %g, %g)\n", pv->getX(),  pv->getY(),  pv->getZ());
          }
       else
          {
          printf (" NULL\n");
          }
       }
}
// ========================================================== addAssociation
int Edge::addAssociation (Shape* forme)
{
   if (forme == NULL)
      {
      if (el_root->debug ())
          cout << "  Edge " << el_name << " addAssociation of NULL ignored"
               << endl;
      return HERR;
      }

   tab_assoc.push_back (forme);
   if (el_root->debug (2))
      cout << "  Edge " << el_name << " addAssociation" << endl;

   return HOK;
}
// ========================================================== setAssociation
void Edge::setAssociation (Shape* forme)
{
   clearAssociation ();
   addAssociation (forme);
}
END_NAMESPACE_HEXA

