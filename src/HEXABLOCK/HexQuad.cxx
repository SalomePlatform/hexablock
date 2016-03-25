
// C++ : Gestion des Quadrangles

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
#include "HexQuad.hxx"

#include "HexDocument.hxx"
#include "HexHexa.hxx"
#include "HexElements.hxx"
#include "HexGlobale.hxx"

#include "HexXmlWriter.hxx"
#include "HexNewShape.hxx"
#include "HexFaceShape.hxx"

BEGIN_NAMESPACE_HEXA

bool db = on_debug();

// ======================================================== Constructeur
Quad::Quad (Vertex* va, Vertex* vb, Vertex* vc, Vertex* vd)
    : EltBase (va->dad(), EL_QUAD)
{
   q_vertex [E_A] = va;
   q_vertex [E_B] = vb;
   q_vertex [E_C] = vc;
   q_vertex [E_D] = vd;
   q_clone        = NULL;
   q_orientation  = Q_UNDEFINED;

   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       q_edge [nro] = new Edge (q_vertex[nro],
                                q_vertex[(nro+1) MODULO QUAD4]);

       if (BadElement (q_vertex [nro]) || BadElement (q_edge [nro]))
          setError ();
       else
          for (int nv=nro+1 ; nv<QUAD4 ; nv++)
              if (q_vertex[nv] == q_vertex[nro])
                 setError ();
       }

   if (el_root != NULL && el_status==HOK)
       el_root->addQuad (this);
   majReferences ();
}
// ======================================================== Constructeur bis
Quad::Quad (Edge* ea, Edge* eb, Edge* ec, Edge* ed)
    : EltBase (ea->dad(), EL_QUAD)
{
   q_edge [E_A] = ea;
   q_edge [E_B] = eb;
   q_edge [E_C] = ec;
   q_edge [E_D] = ed;
   q_clone       = NULL;
   q_orientation = Q_UNDEFINED;

   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       if (BadElement (q_edge[nro]))
          setError (886);
       else
          {
          for (int nv=nro+1 ; nv<QUAD4 ; nv++)
              if (q_edge[nv] == q_edge[nro])
                  setError (887);
          }
       }

   if (isValid())
      {
                // Cond necessaire : ea disjoint de ec (opposes)
      int nc = ea->inter (ec);
      if (nc>=0)
         {
         nc = ea->inter (eb);
         if (nc<0)
            {
            q_edge [E_C] = eb;
            q_edge [E_B] = ec;
            }
         else
            {
            nc = ea->inter (ed);
            if (nc<0)
               {
               q_edge [E_C] = ed;
               q_edge [E_D] = ec;
               }
            else
               setError (880);
            }
         }
      }

   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       int prec = (nro+1) MODULO QUAD4;
       Vertex* node = NULL;
       int nc  = q_edge[nro] -> inter (q_edge[prec]);
       if (nc>=0)
          node = q_edge[nro]->getVertex (nc);
       else
          setError (888);
       q_vertex [prec] = node;
       }

   if (isBad())
      {
      printf (" +++++++++++++++++++++++++++++++++++++++++++ \n");
      printf (" +++ Quadrangle impossible \n");
      printf (" +++++++++++++++++++++++++++++++++++++++++++ \n");
      dump ();
      printf (" +++++++++++++++++++++++++++++++++++++++++++ \n");
      // el_root->dump ();
      for (int ned=0; ned<QUAD4; ned++)
          {
          q_edge[ned]->dumpPlus ();
          }
      HexDump (q_vertex[0]);
      HexDump (q_vertex[1]);
      HexDump (q_vertex[2]);
      HexDump (q_vertex[3]);

      printf (" +++++++++++++++++++++++++++++++++++++++++++ \n");
      fatal_error ("Quadrangle impossible");
      }

   if (el_root != NULL && el_status==HOK)
       el_root->addQuad (this);
   majReferences ();
}
// ======================================================== Constructeur ter
Quad::Quad (Quad* other)
    : EltBase (other->dad(), EL_QUAD)
{
   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       q_edge   [nro] = NULL;
       q_vertex [nro] = NULL;
       }
   q_orientation = Q_UNDEFINED;
   q_clone       = NULL;

   if (el_root != NULL && el_status==HOK)
       el_root->addQuad (this);
}
// ============================================================  getEdge
Edge* Quad::getEdge (int nro)
{
   Edge* elt = NULL;
   if (nro >=0 && nro < QUAD4 && el_status == HOK && q_edge [nro]->isValid())
      elt = q_edge [nro];

   DumpStart  ("getEdge", nro);
   DumpReturn (elt);
   return elt;
}
// ============================================================  getVertex
Vertex* Quad::getVertex (int nro)
{
   Vertex* elt = NULL;
   if (nro >=0 && nro < QUAD4 && el_status == HOK && q_vertex [nro]->isValid())
      elt = q_vertex [nro];

   DumpStart  ("getVertex", nro);
   DumpReturn (elt);
   return elt;
}
// ========================================================= majReferences
void Quad::majReferences ()
{
   for (int nro=0 ; nro<QUAD4 ; nro++)
       q_edge [nro] -> addParent (this);
}
// ========================================================= getParent
Hexa* Quad::getParent  (int nro)
{
   return static_cast <Hexa*> (getFather (nro));
}
// ======================================================== anaMerge
int Quad::anaMerge (Vertex* v1, Vertex* v2, Vertex* tv1[], Edge* te1[])
{
   int orig = NOTHING;
   for (int nro=0 ; orig == NOTHING && nro < QUAD4 ; nro++)
       if (q_vertex [nro] == v1)
           orig = nro;

   if (orig==NOTHING)
      return HERR;

   int nsp1 = (orig+1)       MODULO QUAD4;
   int nsm1 = (orig+QUAD4-1) MODULO QUAD4;

   if (q_vertex [nsp1] == v2)
      {
      for (int nro=0 ; nro < QUAD4 ; nro++)
          {
          tv1 [nro] = q_vertex [(orig+nro) MODULO QUAD4];
          te1 [nro] = q_edge   [(orig+nro) MODULO QUAD4];
          }
      }
   else if (q_vertex [nsm1] == v2)
      {
      for (int nro=0 ; nro < QUAD4 ; nro++)
          {
          tv1 [nro] = q_vertex [(orig+QUAD4-nro) MODULO QUAD4];
          te1 [nro] = q_edge   [(orig+QUAD4-nro) MODULO QUAD4];
          }
      }
   else
      return 588;

   return HOK;
}
// ======================================================== ordoVertex
int Quad::ordoVertex (Vertex* v1, Vertex* v2, Vertex* tver[])
{
   int orig = NOTHING;
   for (int nro=0 ; orig == NOTHING && nro < QUAD4 ; nro++)
       if (q_vertex [nro] == v1)
           orig = nro;

   if (orig==NOTHING)
      return HERR;

   int nsp1 = (orig+1)       MODULO QUAD4;
   int nsm1 = (orig+QUAD4-1) MODULO QUAD4;

   if (q_vertex [nsp1] == v2)
      {
      for (int nro=0 ; nro < QUAD4 ; nro++)
          tver [nro] = q_vertex [(orig+nro) MODULO QUAD4];
      }
   else if (q_vertex [nsm1] == v2)
      {
      for (int nro=0 ; nro < QUAD4 ; nro++)
          tver [nro] = q_vertex [(orig+QUAD4-nro) MODULO QUAD4];
      }
   else
      return 588;

   return HOK;
}
// ======================================================== ordonner
int Quad::ordonner (Vertex* v1, Vertex* v2, Vertex* tver[], Edge* ted[])
{
   tver [0] = tver [1] = tver [2] = tver [3] = NULL;
   ted  [0] = ted  [1] = ted  [2] = ted  [3] = NULL;

   int ier = ordoVertex (v1, v2, tver);
   if (ier != HOK)
       return ier;

   for (int nro=0 ; nro < QUAD4 ; nro++)
       ted [nro] = findEdge (tver[nro], tver [(nro+1) MODULO QUAD4]);

   return HOK;
}
// ======================================================== getBrother
Quad* Quad::getBrother (StrOrient* orient)
{
/* *****************************
   printf (" getBrother ");
   dump ();
   printf (" .. Base  : ");
   orient->v21->printName();
   orient->v22->printName();
   printf ("dir=%d, arete=", orient->dir);
  ***************************** */

   int n21 = indexVertex (orient->v21);
   int n22 = indexVertex (orient->v22);

   int sens  = n22 - n21;
   if (sens >  1) sens -= QUAD4;
   if (sens < -1) sens += QUAD4;
   if (sens*sens !=1) return NULL;

   switch (orient->dir)
      {
      case OR_LEFT  : n22 = n21 - sens;
           break;
      case OR_RIGHT : n21 = n22 + sens;
           break;
      case OR_FRONT : n21 += 2;
                      n22 += 2;
           break;
      default : ;
      }

   n21 = (n21 + QUAD4) MODULO QUAD4;
   n22 = (n22 + QUAD4) MODULO QUAD4;

   orient->v21 = q_vertex [n21];
   orient->v22 = q_vertex [n22];

   Edge* arete  = findEdge (orient->v21, orient->v22);
   // arete->printName("\n");

   int nbfreres = arete->getNbrParents ();

   for (int nq = 0 ; nq < nbfreres ; nq++)
       {
       Quad* next = arete->getParent (nq);
       if (next!=NULL && next != this )
          {
          int   nbp   = next->getNbrParents();
          Hexa* dad   = next->getParent(0);
          int   mark  = next->getMark();
          int   mark2 = dad ? dad->getMark() : IS_NONE;

          if (nbp  == 1  && mark2 != IS_MARRIED && mark == IS_NONE)
             return next;
          // if (nbp  <= 1  && mark == IS_NONE)
             // return next;
          }
       }
   return NULL;
}
// ======================================================== coupler
int Quad::coupler (Quad* other, StrOrient* orient, Elements* table)
{
   if (other==NULL)
      return HERR;
   if (db)
      {
      cout << " Quads::coupler " << el_name << " -> " << other->getName () 
           << endl;
      }

   Hexa* hexa = other->getParent(0);

   setMark (IS_MARRIED);
   other->setMark (IS_MARRIED);
   if (hexa != NULL)
       hexa->setMark (IS_MARRIED);

   for (int ned = 0 ; ned < QUAD4 ; ned++)
       {
       Edge* arete  = q_edge[ned];
       int nbfreres = arete ->getNbrParents ();
       for (int nq = 0 ; nq < nbfreres ; nq++)
           {
           Quad* next = arete->getParent (nq);
           if (next!=NULL && next != this && next->getMark() > 0)
              {
              StrOrient new_ori (orient);
              new_ori.dir = OR_FRONT;
              Vertex* va  = arete->getVertex (V_AMONT);
              Vertex* vb  = arete->getVertex (V_AVAL);

//    On voit si un point de repere est conserve
              if (va == orient->v11)
                 {
                 new_ori.v12 = vb;
                 new_ori.dir += OR_LEFT;
                 }
              else if (vb == orient->v11)
                 {
                 new_ori.v12 = va;
                 new_ori.dir += OR_LEFT;
                 }

              if (va == orient->v12)
                 {
                 new_ori.v11 = vb;
                 new_ori.dir += OR_RIGHT;
                 }
              else if (vb == orient->v12)
                 {
                 new_ori.v11 = va;
                 new_ori.dir += OR_RIGHT;
                 }

              if (new_ori.dir == OR_FRONT)
                 {
                 if (definedBy (va, orient->v11))
                    {
                    new_ori.v11 = va;
                    new_ori.v12 = vb;
                    }
                 else
                    {
                    new_ori.v11 = vb;
                    new_ori.v12 = va;
                    }
                 }

              int nro = next->getMark ();
              Quad* beauf = other->getBrother (&new_ori);
              int ier = table->coupler (nro, beauf, &new_ori);
              if (ier != HOK)
                 return ier;
              ier = next->coupler (beauf, &new_ori, table);
              if (ier != HOK)
                 return ier;
              }
           }
       }
   return HOK;
}
// ======================================================== getOpposVertex
Vertex* Quad::getOpposVertex (Vertex* start)
{
   int  na = indexVertex (start);
   if (na==NOTHING)
      return NULL;
   return  q_vertex [(na+2) MODULO QUAD4];
}
// ======================================================== getOpposEdge
Edge* Quad::getOpposEdge (Edge* start, int& sens)
{
   sens = 1;
   int  na = indexVertex (start->getVertex (V_AMONT));
   int  nb = indexVertex (start->getVertex (V_AVAL));

   Vertex* vaprim = q_vertex [(nb+2) MODULO QUAD4];
   Vertex* vbprim = q_vertex [(na+2) MODULO QUAD4];

   for (int ned = 0 ; ned < QUAD4 ; ned++)
       {
       if (   q_edge[ned]->getVertex(V_AMONT) == vaprim
           && q_edge[ned]->getVertex(V_AVAL ) == vbprim)
           {
           sens = 1;
           return q_edge[ned];
           }
       else if (   q_edge[ned]->getVertex(V_AMONT) == vbprim
                && q_edge[ned]->getVertex(V_AVAL ) == vaprim)
           {
           sens = -1;
           return q_edge[ned];
           }
       }
   //             TODO : traiter l'erreur
   cout << " ... Probleme dans Quad::getOpposedEdge :" << endl;
   HexDisplay (el_name);
   PutName (start);
   PutName (vaprim);
   PutName (vbprim);
   HexDisplay (na);
   HexDisplay (nb);
   dumpPlus ();

   for (int ned = 0 ; ned < QUAD4 ; ned++)
       q_edge[ned]->dump();

   return NULL;
}
// ========================================================= saveXml
void Quad::saveXml (XmlWriter* xml)
{
   char buffer[12];
   string edges;

   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       if (nro>0) edges += " ";
       edges += q_edge[nro]->getName(buffer);
       }

   xml->openMark     ("Quad");
   xml->addAttribute ("id",    getName (buffer));
   xml->addAttribute ("edges", edges);
   if (el_name!=buffer)
       xml->addAttribute ("name", el_name);
   xml->closeMark ();

   int nbass = tab_assoc.size();
   for (int nro=0 ; nro<nbass ; nro++)
       if (tab_assoc[nro] != NULL)
           tab_assoc[nro]->saveXml (xml);
}
// ======================================================== replaceEdge
void Quad::replaceEdge (Edge* old, Edge* par)
{
   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       if (q_edge[nro]==old)
           {
           q_edge[nro] = par;
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
// ======================================================== replaceVertex
void Quad::replaceVertex (Vertex* old, Vertex* par)
{
   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       if (q_vertex [nro]==old)
          {
          q_vertex [nro] = par;
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
void Quad::dump ()
{
   printName(" = (");
   if (NOT isHere ())
      {
      printf ("*** deleted ***)\n");
      return;
      }

   for (int nro=0 ; nro<QUAD4 ; nro++)
        PrintName (q_edge[nro]);
   printf (")\n");

   printf ("        (");
   for (int nro=0 ; nro<QUAD4 ; nro++)
        PrintName (q_vertex[nro]);
   printf (")");

   dumpRef ();
   Real3 cg;
   getCenter (cg);
   printf ("     -> (%g, %g, %g)\n", cg[dir_x], cg[dir_y], cg[dir_z]);
}
// ======================================================== dumpPlus
void Quad::dumpPlus ()
{
   dump ();
   if (NOT isHere ())
      return;

   for (int nro=0 ; nro < QUAD4 ; nro++)
       {
       Vertex* pv = q_vertex[nro];
       printf ( "    ");
       if (pv!=NULL)
          {
          pv->printName ("");
          printf ( " (%g, %g, %g)\n", pv->getX(),  pv->getY(),  pv->getZ());
          }
       else
          {
          printf ( "NULL\n");
          }
       }
}
// ======================================================== getOpposEdge (2)
Edge* Quad::getOpposEdge (Edge* start)
{
   int  na = indexEdge (start);
   if (na<0)
      return NULL;
   return q_edge [(na+2) MODULO QUAD4];
}
// ======================================================== getPerpendicular
Edge* Quad::getPerpendicular (Edge* arete, Vertex* node)
{
   int na = indexEdge (arete);
   if (na<0)
      return NULL;

   int nv = arete->index (node);
   if (nv<0)
      return NULL;
   Edge* perp = q_edge [(na+1) MODULO QUAD4];

   nv = perp->index (node);
   if (nv>=0)
      return perp;

   perp = q_edge [(na+3) MODULO QUAD4];
   nv   = perp->index (node);
   if (nv>=0)
      return perp;
   else
      return NULL;
}
static cpchar t_ori[] = {"Q_INSIDE", "Q_DIRECT", "Q_INVERSE", "Q_UNDEF"};
// ======================================================== setOrientation
void Quad::setOrientation (int ori)
{
    q_orientation = ori;
    if (db && (ori==Q_DIRECT || ori==Q_INVERSE))
       printf (" %s = %s\n", el_name.c_str(), t_ori [ q_orientation ]);
}
// ======================================================== setOrientation
int Quad::setOrientation ()
{
    q_orientation = Q_INSIDE;
    if (getNbrParents() != 1)
       return q_orientation;

    Real3 cg, orig, pi, pj, vi, vj, vk;

    Hexa* hexa = getParent(0);
    hexa->getCenter (cg);

/********************************************************************
    printName (" = ");

    for (int np=0 ; np < QUAD4 ; np++)
        {
        q_vertex [np        ] -> getPoint (orig);
        q_vertex [(np+1) % 4] -> getPoint (pi);
        q_vertex [(np+3) % 4] -> getPoint (pj);

        calc_vecteur (orig, pi, vi);
        calc_vecteur (orig, pj, vj);
        calc_vecteur (orig, cg, vk);
        double pmixte = prod_mixte (vi, vj, vk);
        q_orientation = pmixte > ZEROR ? Q_DIRECT : Q_INVERSE;
        if (pmixte>0) printf (">");
           else       printf ("<");
        }

    printf ("\n");
    return;
  ******************************************************************* */
    q_vertex [0] -> getPoint (orig);
    q_vertex [1] -> getPoint (pi);
    q_vertex [3] -> getPoint (pj);

    calc_vecteur (orig, pi, vi);
    calc_vecteur (orig, pj, vj);
    calc_vecteur (cg, orig, vk);

    double pmixte = prod_mixte (vi, vj, vk);
    q_orientation = pmixte > ZEROR ? Q_DIRECT : Q_INVERSE;
    if (db)
       printf (" %s := %s\n", el_name.c_str(), t_ori [ q_orientation ]);
    return q_orientation;
}
// ========================================================== clearAssociation
void Quad::clearAssociation ()
{
   tab_assoc.clear ();
   is_associated = false;
}
// ========================================================== addAssociation
int Quad::addAssociation (NewShape* geom, int subid)
{
   if (geom == NULL)
      return HERR;

   FaceShape* face = geom->findFace (subid);
   int ier = addAssociation (face);

   return ier;
}
// ========================================================== addAssociation
int Quad::addAssociation (FaceShape* face)
{
   if (face == NULL)
      return HERR;

   face->addAssociation (this);
   tab_assoc.push_back (face);
   is_associated = true;
   return HOK;
}
// ========================================================== getAssociation
FaceShape* Quad::getAssociation (int nro)
{
   if (nro < 0 || nro >= (int)tab_assoc.size())
      return NULL;

   return tab_assoc [nro];
}
// ======================================================== commonEdge
Edge* Quad::commonEdge (Quad* other)
{
   for (int ne1=0 ; ne1<QUAD4 ; ne1++)
       for (int ne2=0 ; ne2<QUAD4 ; ne2++)
           if (q_edge [ne1] == other->q_edge [ne2])
              return q_edge [ne1];

   return NULL;
}
// ======================================================== Inter
int Quad::inter (Quad* other, int& nother)
{
   for (int ne1=0 ; ne1<QUAD4 ; ne1++)
       for (int ne2=0 ; ne2<QUAD4 ; ne2++)
           if (q_edge [ne1] == other->q_edge [ne2])
              {
              nother = ne2;
              return  ne1;
              }

   nother = NOTHING;
   return NOTHING;
}
// ======================================================== Inter (2)
Edge* Quad::inter (Quad* other)
{
   for (int ne1=0 ; ne1<QUAD4 ; ne1++)
       for (int ne2=0 ; ne2<QUAD4 ; ne2++)
           if (q_edge [ne1] == other->q_edge [ne2])
              return  q_edge [ne1];
   return NULL;
}
// ============================================================ definedBy (v)
bool Quad::definedBy  (Vertex* v1, Vertex* v2)
{
   for (int n1=0 ; n1< QUAD4 ; n1++)
       if (v1 == q_vertex[n1] && v2 == q_vertex[(n1+2) MODULO QUAD4])
          return true;

   return false;
}
// ============================================================ definedBy (e)
bool Quad::definedBy  (Edge* e1, Edge* e2)
{
   if (e1==e2 || BadElement (e1) || BadElement (e2))
      return false;

   bool f1=false, f2=false;
   for (int ned=0 ; ned< QUAD4 ; ned++)
       if      (e1 == q_edge[ned]) f1 = true;
       else if (e2 == q_edge[ned]) f2 = true;
    // if (e1 == q_edge[ned] && e2 == q_edge[(ned+2) MODULO QUAD4]) return true;

   return f1 && f2;
}
// =============================================================== findEdge
Edge* Quad::findEdge (Vertex* v1, Vertex* v2)
{
   for (int nro=0 ; nro< QUAD4 ; nro++)
       {
       Vertex* va = q_edge[nro]->getVertex(V_AMONT) ;
       Vertex* vb = q_edge[nro]->getVertex(V_AVAL) ;
       if ((v1==va && v2==vb) || (v1==vb && v2==va))
           return q_edge [nro];
       }

   return NULL;
}
// =============================================================== indexVertex
int Quad::indexVertex  (Vertex* elt)
{
   for (int n1=0 ; n1< QUAD4 ; n1++)
       if (elt == q_vertex[n1])
          return n1;

   return NOTHING;
}
// =============================================================== indexEdge
int Quad::indexEdge  (Edge* elt)
{
   for (int n1=0 ; n1< QUAD4 ; n1++)
       if (elt == q_edge[n1])
          return n1;

   return NOTHING;
}
// =============================================================== setColor
void Quad::setColor  (double val)
{
   for (int n1=0 ; n1< QUAD4 ; n1++)
       q_vertex[n1] -> setColor (val);
}
// =============================================================== duplicate
void Quad::duplicate  ()
{
   q_orientation  = Q_UNDEFINED;
   q_clone = new Quad (GetClone (q_edge [E_A]),
                       GetClone (q_edge [E_B]),
                       GetClone (q_edge [E_C]),
                       GetClone (q_edge [E_D]));
   q_clone->tab_assoc  = tab_assoc;
}
// ============================================================ nearestVertex
Vertex* Quad::nearestVertex (Vertex* other)
{
   if (BadElement (other))
      return NULL;
 
   Vertex* vsol = NULL;
   double  dmin = 1e+77;
   int nbre = countVertex ();
   for (int nro=0 ; nro<nbre ; nro++)
       {
       Vertex* vert = getVertex (nro);
       double  dist = other->dist2 (vert);
       if (dist < dmin)
          {
          dmin = dist;
          vsol = vert; 
          }
       }
   return vsol;
}
// =============================================================== reorienter
void Quad::reorienter  ()
{
   if (q_orientation != Q_INVERSE)
         return;

   Edge* edge   = q_edge [E_B];
   q_edge [E_B] = q_edge [E_D];
   q_edge [E_B] = edge;

   q_vertex [E_A] = q_edge [E_D]->commonVertex(q_edge [E_A]);
   q_vertex [E_B] = q_edge [E_A]->commonVertex(q_edge [E_B]);
   q_vertex [E_C] = q_edge [E_B]->commonVertex(q_edge [E_C]);
   q_vertex [E_D] = q_edge [E_C]->commonVertex(q_edge [E_D]);

   if (db)
       printf (" %s est reoriente\n", el_name.c_str());
   q_orientation = Q_DIRECT;
}
// =============================================================== getCenter
double* Quad::getCenter (double* center)
{
   center[dir_x] = center[dir_y] = center[dir_z] =  0;
   if (BadElement (this))
      return NULL;

   for (int nv=0 ; nv<QUAD4 ; nv++)
       {
       if (BadElement (q_vertex [nv]))
           return NULL;
       center [dir_x] += q_vertex[nv]->getX()/4;
       center [dir_y] += q_vertex[nv]->getY()/4;
       center [dir_z] += q_vertex[nv]->getZ()/4;
       }
    return center;
}
// =============================================================== dist2
double Quad::dist2 (double* point)
{
   Real3 center;
   getCenter (center);
   double d2 = carre (point[dir_x] - center[dir_x]) 
             + carre (point[dir_y] - center[dir_y]) 
             + carre (point[dir_z] - center[dir_z]) ;
   return d2;
}
// =============================================================== opposedHexa
Hexa* Quad::opposedHexa (Hexa* hexa)
{
   int nbre = getNbrParents ();
   for (int nro=0 ; nro <nbre ; ++nro)
       { 
       Hexa* dad = getParent (nro);
       if (dad!= NULL && dad->isValid() && dad != hexa)
           return dad;
       }
   return NULL;
}
END_NAMESPACE_HEXA
