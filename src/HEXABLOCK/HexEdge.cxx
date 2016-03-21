
// C++ : Gestion des aretes

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
#include "HexEdge.hxx"
#include "HexVertex.hxx"
#include "HexQuad.hxx"

#include "HexPropagation.hxx"
#include "HexXmlWriter.hxx"
#include "HexNewShape.hxx"
#include "HexAssoEdge.hxx"
#include "HexVertexShape.hxx"

static int niveau = 0;

BEGIN_NAMESPACE_HEXA

static bool db = on_debug();

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

   if (el_root != NULL)
       el_root->addEdge (this);
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

   if (el_root != NULL)
       el_root->addEdge (this);
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
// ========================================================= setPropag
void Edge::setPropag  (int nro, bool sens)
{
   e_propag = nro;
   e_way    = sens; 
   if (NOT db || nro<0) 
      return;

   int prems  = sens ? 0 : 1;
   Vertex* v1 = getVertex (  prems);
   Vertex* v2 = getVertex (1-prems);

   cout << " setPropag " << el_name
           << " = "   << nro 
           <<  " = (" << v1->getName() << ", "    << v2->getName()
           << ") = (" << v2->getX() - v1->getX() 
           << ", "    << v2->getY() - v1->getY() 
           << ", "    << v2->getZ() - v1->getZ() 
           << ")"     << endl;
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
// ======================================================== makeDefinition
string Edge::makeDefinition ()
{
   string definition = el_name;

   definition += " = (";
   definition += e_vertex [V_AMONT]->getName();
   definition += ",";
   definition += e_vertex [V_AVAL]->getName();
   definition += ")";

   return definition;
}
// ===================================================== addAssociation
int Edge::addAssociation (EdgeShape* gline, double deb, double fin)
{
   if (gline == NULL)
      return HERR;

   gline->addAssociation (this);
   AssoEdge*  asso  = new AssoEdge (gline, deb, fin);
   tab_assoc.push_back (asso);

   if (db)
      {
      cout << " Edge " << el_name
           << " = (" << e_vertex[V_AMONT]->getName()
           << " , "  << e_vertex[V_AVAL ]->getName()
           << ") addAssociation " << gline->getName ()
           << " (" << deb << ", " << fin << ")"
           << endl;
      PutCoord (asso->getOrigin ());
      PutCoord (asso->getExtrem ());
      }
   is_associated = true;
   return HOK;
}
// ========================================================== addAssociation
int Edge::addAssociation (NewShape* geom, int subid, double deb, double fin)
{
   if (geom == NULL)
      {
      if (el_root->debug ())
          cout << "  Edge " << el_name << " addAssociation of NULL ignored"
               << endl;
      return HERR;
      }

   EdgeShape* gline = geom->findEdge (subid);
   int ier = addAssociation (gline, deb, fin);
   return ier;
}
// ================================================== clearAssociation
void Edge::clearAssociation ()
{
   int nombre = tab_assoc.size ();
   for (int nro=0 ; nro<nombre ; nro++)
       {
       delete tab_assoc [nro];
       }

   tab_assoc .clear ();
   is_associated = false;
}
// ================================================== getAssociation
AssoEdge* Edge::getAssociation (int nro)
{
   if (nro<0 || nro >= (int)tab_assoc.size())
      return NULL;

   return tab_assoc [nro];
}
// =============================================================== getVertex
Vertex* Edge::getVertex(int nro)
{
   Vertex* elt = NULL;
   if (nro >=0 && nro < V_TWO  && el_status == HOK
               && e_vertex [nro]->isValid())
      elt = e_vertex [nro];

   return elt;
}
// =============================================================== index
int Edge::index (Vertex* node)
{
   return  node == NULL ? NOTHING
         : node == e_vertex[V_AMONT] ? V_AMONT
         : node == e_vertex[V_AVAL ] ? V_AVAL : NOTHING;
}
// ============================================================= opposedVertex
Vertex* Edge::opposedVertex (Vertex* sommet)
{
   int nro = index (sommet);
   return nro<0 ? NULL : e_vertex[1-nro];
}
// ============================================================= commonVertex
Vertex* Edge::commonVertex (Edge* other)
{
   int nro = inter (other);
   return nro<0 ? NULL : e_vertex[nro];
}
// ============================================================= commonPoint
double* Edge::commonPoint (Edge* other, double point[])
{
   Vertex* commun = commonVertex (other);
   if (commun==NULL)
      {
      point[dir_x] = point[dir_y] = point[dir_z] =  0;
      return NULL;
      }

   commun->getPoint (point);
   return point;
}
// =============================================================== inter
int Edge::inter (Edge* other)
{
   int nro;
   return inter (other, nro);
}
// =============================================================== inter
int Edge::inter (Edge* other, int& nother)
{
   for (int ni=0 ; ni<V_TWO ; ni++)
        for (int nj=0 ; nj<V_TWO ; nj++)
            if (e_vertex[ni] == other->e_vertex[nj])
               {
               nother =  nj;
               return ni;
               }

   nother = NOTHING;
   return   NOTHING;
}
// =============================================================== definedBy
bool Edge::definedBy  (Vertex* v1, Vertex* v2)
{
   bool   rep =    (v1 == e_vertex[V_AMONT] && v2 == e_vertex[V_AVAL ])
                || (v1 == e_vertex[V_AVAL ] && v2 == e_vertex[V_AMONT]);
   return rep;
}
// =============================================================== setColor
void Edge::setColor  (double val)
{
   e_vertex [V_AMONT]->setColor (val);
   e_vertex [V_AVAL ]->setColor (val);
}
// =============================================================== duplicate
void Edge::duplicate  ()
{
   e_clone = new Edge (GetClone (e_vertex [V_AMONT]),
                       GetClone (e_vertex [V_AVAL ]));

   // e_clone->tab_shapes = tab_shapes;
   e_clone->tab_assoc  = tab_assoc;
}
// =============================================================== getVector
double* Edge::getVector (double vecteur[])
{

   if (e_vertex[V_AMONT]==NULL ||  e_vertex[V_AVAL]==NULL)
      {
      vecteur [dir_x] = vecteur [dir_y] = vecteur [dir_z] = 0;
      return NULL;
      }

   vecteur[dir_x] = e_vertex[V_AVAL]->getX() - e_vertex[V_AMONT]->getX();
   vecteur[dir_y] = e_vertex[V_AVAL]->getY() - e_vertex[V_AMONT]->getY();
   vecteur[dir_z] = e_vertex[V_AVAL]->getZ() - e_vertex[V_AMONT]->getZ();

   return vecteur;
}
// ========================================================== checkAssociation
int Edge::checkAssociation ()
{
   int nombre = tab_assoc.size();
   if (nombre==0)
      return HOK;

   Real3 ver_assoc [V_TWO];
   int arc [V_TWO], sens [V_TWO];
   for (int nro=0 ; nro<V_TWO ; ++nro)
       {
       arc [nro] = sens [nro] = NOTHING;
       e_vertex[nro]->getAssoCoord (ver_assoc[nro]);
       }

   int ier    = HOK;
   for (int nass=0 ; nass<nombre ; ++nass)
       {
       AssoEdge*  asso = tab_assoc[nass];
       for (int nro = V_AMONT ; nro<=V_AVAL ; ++nro)
           {
           int rep = asso->onExtremity (ver_assoc[nro]);
           if (rep != NOTHING)
              {
              if (arc[nro] != NOTHING)
                 {
                 if (ier==HOK) cout << endl;
                 cout << " Association Edge "  << el_name
                      << " : Le vertex " << e_vertex[nro]->getName()
                      << " : Le vertex " << e_vertex[nro]->getName()
                      << " Touche les lignes " << arc [nro]
                      << " et " << nass << endl;
                 ier   = 112;
                 }
              arc  [nro] = nass;
              sens [nro] = rep;
              }
          }
       }

   for (int nro=0 ; nro<V_TWO ; ++nro)
       {
       if (arc [nro] == NOTHING)
          {
          if (ier==HOK) cout << endl;
          cout << " Association Edge " << el_name
               << " : Le vertex nro " << nro
               << " = " << e_vertex[nro]->getName()
               << " est isole" << endl;
          PutCoord (ver_assoc[nro]);
          ier = 111;
          }
       }
   if (ier==HOK)
       return ier;

   cout << " ** Controle associations (" << nombre << ") edge " << el_name
        << " = (" << e_vertex[V_AMONT]->getName()
        << " , "  << e_vertex[V_AVAL ]->getName()
        << ")" << endl;

   for (int nv=0 ; nv<2 ; ++nv)
       {
       Vertex* node = e_vertex[nv];
       cout << node->getName()         << " = (" << node->getX()
             << ", "   << node->getY() << ", "   << node->getZ()
             << ") -> "<< ver_assoc [nv][0] << ", " << ver_assoc [nv][1]
             << ", "   << ver_assoc [nv][2] << ")"  << endl;
       }

   for (int nass=0 ; nass<nombre ; ++nass)
       {
       AssoEdge*  asso = tab_assoc[nass];
       cout << " " << nass << " :";
       asso->dump ();
       }

   return ier;
}
// ========================================================== getAssoLen
double Edge::getAssoLen ()
{
   int    nombre = tab_assoc.size();
   double longueur = 0;
   if (nombre==0)
      {
      Real3 p1, p2;
      e_vertex [V_AMONT]-> getAssoCoord (p1);
      e_vertex [V_AVAL ]-> getAssoCoord (p2);
      longueur = calc_distance (p1, p2);
      }
   else
      {
      for (int nass=0 ; nass<nombre ; ++nass)
          longueur += tab_assoc[nass]->length ();
      }

   return longueur;
}
// ========================================================= getLength
double Edge::getLength ()
{
   Real3 p1, p2;
   e_vertex [V_AMONT]-> getAssoCoord (p1);
   e_vertex [V_AVAL ]-> getAssoCoord (p2);
   double longueur = calc_distance (p1, p2);
   return longueur;
}
// ========================================================= findAssociation
int Edge::findAssociation (NewShape* geom)
{
   Real3 point, p2;
   if (geom==NULL)
      return NOTHING;

   e_vertex [V_AMONT]-> getAssoCoord (point);
   e_vertex [V_AVAL ]-> getAssoCoord (p2);

   EdgeShape*  gline = geom->findEdge (point, p2);
   if (gline==NULL)
      { 
      cout << " *** FindAssociation "  << el_name << endl;
      for (int nv=0 ; nv < V_TWO ; ++nv)
          {
          e_vertex [nv]-> getAssoCoord (point);
          VertexShape* shape = geom->findVertex (point);
          cout << " *** Vertex nro "  << nv;
          if (shape==NULL)
             {
             cout << " absent : ";
             PutCoord (point);
             }
          else
             {
             cout << " : Subid = " << shape->getIdent() << endl;
             }
          }
      return NOTHING;
      }
   
   clearAssociation ();
   addAssociation   (gline, 0, 1); 
   return gline->getIdent();
}
// ========================================================= setAssociation
int Edge::setAssociation (NewShape* geom, int subid)
{
   if (geom == NULL)
      {
      if (el_root->debug ())
          cout << "  Edge " << el_name << " addAssociation of NULL ignored"
               << endl;
      return HERR;
      }

   EdgeShape* gline = geom->findEdge (subid);
   if (gline == NULL)
      {
      if (el_root->debug ())
          cout << "  Edge " << el_name << " addAssociation bad subid : "
               << subid << endl;
      return HERR;
      }

   Real3 p1, p2, pa, pb;
   gline->getCoords (p1, p2);
   e_vertex [V_AMONT]-> getAssoCoord (pa);
   e_vertex [V_AVAL ]-> getAssoCoord (pb);

   double da1 = calc_d2 (pa, p1);
   double da2 = calc_d2 (pa, p2);
   double db1 = calc_d2 (pb, p1);
   double db2 = calc_d2 (pb, p2);
   
   cout << "setAssociation " << el_name << " :" <<endl;

   if (da2 < da1 && db1 < db2) 
      {
      e_vertex [V_AMONT]->setAssociation (p2);
      e_vertex [V_AVAL ]->setAssociation (p1);
      }
   else
      {
      e_vertex [V_AMONT]->setAssociation (p1);
      e_vertex [V_AVAL ]->setAssociation (p2);
      }

   
   clearAssociation ();
   int ier = addAssociation   (gline, 0, 1); 
   return ier;
}
END_NAMESPACE_HEXA
