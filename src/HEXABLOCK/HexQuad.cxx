
// C++ : Gestion des Quadrangles

//  Copyright (C) 2009-2011  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "HexQuad.hxx"

#include "HexDocument.hxx"
#include "HexHexa.hxx"
#include "HexElements.hxx"

#include "HexXmlWriter.hxx"
#include "HexShape.hxx"

BEGIN_NAMESPACE_HEXA

// ======================================================== Constructeur
Quad::Quad (Vertex* va, Vertex* vb, Vertex* vc, Vertex* vd)
    : EltBase (va->dad(), EL_QUAD)
{
   q_vertex [E_A] = va;
   q_vertex [E_B] = vb;
   q_vertex [E_C] = vc;
   q_vertex [E_D] = vd;
   q_clone        = NULL;

   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       q_edge [nro] = new Edge (q_vertex[nro], 
                                q_vertex[(nro+1) MODULO QUAD4]);
       }

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
   q_clone      = NULL;

   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       int prec = (nro+1) MODULO QUAD4; 
       Vertex* node = NULL;
       int nc  = q_edge[nro] -> inter (q_edge[prec]);
       if (nc>=0)
          node = q_edge[nro]->getVertex (nc);
       else  
          el_status = 888;
       q_vertex [prec] = node;
       }

   if (el_status != HOK)
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
      // exit (1);
      }

   majReferences ();
}
// ======================================================== Constructeur bis
Quad::Quad (Quad* other)
    : EltBase (other->dad(), EL_QUAD)
{
   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       q_edge   [nro] = NULL;
       q_vertex [nro] = NULL;
       }
}
// ========================================================= getParent 
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
int Quad::ordoVertex (Vertex* v1, Vertex* v2, Vertex* tv1[])
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
          tv1 [nro] = q_vertex [(orig+nro) MODULO QUAD4];
      }
   else if (q_vertex [nsm1] == v2) 
      {
      for (int nro=0 ; nro < QUAD4 ; nro++)
          tv1 [nro] = q_vertex [(orig+QUAD4-nro) MODULO QUAD4];
      }
   else 
      return 588;

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
   arete->printName("\n");

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

          if (nbp  <= 1  && mark2 != IS_MARRIED && mark == IS_NONE)
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
              table->coupler (nro, beauf, &new_ori);
              next->coupler (beauf, &new_ori, table);
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
// ========================================================= getParent 
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
   printf (")\n" );

   printf ("        (" );
   for (int nro=0 ; nro<QUAD4 ; nro++)
        PrintName (q_vertex[nro]);
   printf (")" );

   dumpRef ();
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
END_NAMESPACE_HEXA
