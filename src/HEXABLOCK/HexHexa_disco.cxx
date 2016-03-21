
// C++ : Gestion des Hexaedres

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
#include "HexHexa.hxx"
#include "HexQuad.hxx"

#include "HexVertex.hxx"
#include "HexDocument.hxx"
#include "HexEdge.hxx"

#include "HexGlobale.hxx"
#include "HexXmlWriter.hxx"
#include "HexDiagnostics.hxx"
#include "HexGlobale.hxx"
#include "HexMatrix.hxx"
#include "HexElements.hxx"

BEGIN_NAMESPACE_HEXA

// ========================================================= disconnectQuad
int Hexa::disconnectQuad (Quad* quad, Elements* nouveaux)
{
   if (debug())
      {
      printf (" ... Avant disconnectQuad, quad=");
      quad->printName ("\n");
      dumpFull ();
      }

   int nface = findQuad (quad);
   if (nface==NOTHING)
      {
      setError (HERR);
      Mess << "Quadangle doesn't belong to Hexaedron";
      return HERR;
      }
                                       // Face opposee : replace
   // int nfopp = (nface MODULO 2==0) ? nface+1 : nface-1;

   int  ind_edge  [QUAD4], ind_opp_quad [QUAD4];
   bool make_quad [QUAD4], make_edge [QUAD4];

   for (int nro=0 ; nro<QUAD4 ; nro++)
       make_quad [nro] = make_edge[nro] = false;

   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       int nro1  = (nro+1) MODULO QUAD4;
       int pedge = findEdge   (quad->getEdge   (nro));
       int pnode = findVertex (quad->getVertex (nro));
       int oppq  = findOpposedQuad (quad, quad->getEdge (nro));

       ind_edge [nro]     = pedge;
       ind_opp_quad [nro] = oppq;

       if (pedge==NOTHING || pnode==NOTHING || oppq==NOTHING)
          {
          setError (HERR);
          Mess << "Topologic problem(1) for quad's edge nr " << nro;
          return HERR;
          }

       make_quad [nro]  = h_quad[oppq]->getNbrParents() == 2;
       make_edge [nro ] = make_edge [nro ] || make_quad [nro];
       make_edge [nro1] = make_edge [nro1] || make_quad [nro];

       if (debug())
          {
          printf (" Sommet nro %d : ", nro);
          quad->getVertex(nro)->printName (", ");
          printf (" edge = ");
          quad->getEdge(nro)->printName (", ");
          printf (" quad oppose = ");
          h_quad[oppq]->printName("");
          if (make_quad [nro])
             printf (" a dissocier\n");
          else
             printf ("\n");

          }
       }

   Vertex* new_node     [QUAD4];
   Edge*   new_opp_edge [QUAD4];
   Edge*   old_opp_edge [QUAD4];

   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       old_opp_edge [nro] = NULL;
       new_opp_edge [nro] = NULL;
       Vertex*  o_v0  = quad->getVertex (nro);
       new_node [nro] = new Vertex (o_v0);
       if (debug())
          {
          printf (" quad.vertex [%d] = ", nro);
          quad->getVertex (nro)->printName (" --> ");
          new_node [nro]->printName ("\n");
          }

       if (make_edge[nro])
          {
          Quad*   pface  = h_quad [ind_opp_quad [nro]];
          int     bid;
          int     ncut = pface->inter (quad, bid);
          Edge*   ecut = pface->getEdge ((ncut+1) MODULO QUAD4);
          Vertex* vopp = ecut->getVertex(V_AMONT);
          if (vopp==o_v0)
              vopp = ecut->getVertex (V_AVAL);
          else if (o_v0 != ecut->getVertex (V_AVAL));
              {
              ecut = pface->getEdge ((ncut+3) MODULO QUAD4);
              vopp = ecut->getVertex(V_AMONT);
              if (vopp==o_v0)
                  vopp = ecut->getVertex (V_AVAL);
              else if (o_v0 != ecut->getVertex (V_AVAL))
                  {
                  setError (HERR);
                  Mess << "Topologic problem(2) for quad's edge nr " << nro;
                  return HERR;
                  }
              }

          old_opp_edge [nro] = ecut;
          new_opp_edge [nro] = new Edge (new_node[nro], vopp);
          if (debug())
             {
             printf (" quad.opp_edge [%d] = ", nro);
             old_opp_edge [nro]->printName (" --> ");
             new_opp_edge [nro]->printName ("\n");
             }
          }
       }

   Quad* new_quad = new Quad (new_node[0], new_node[1], new_node[2],
                                                        new_node[3]);

   Quad* new_opp_quad [QUAD4];
   Quad* old_opp_quad [QUAD4];
   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       old_opp_quad [nro] = NULL;
       new_opp_quad [nro] = NULL;
       if (make_quad[nro])
          {
          int nro1 = (nro+1) MODULO QUAD4;

          Edge* n_edge0 = new_quad->getEdge (nro);
          Edge* n_edge1 = new_opp_edge [nro];
          Edge* n_edge3 = new_opp_edge [nro1];

          int iv1 = n_edge1->inter (n_edge0);
          int iv3 = n_edge3->inter (n_edge0);
          if (iv1 <0 || iv3 <0)
             {
             setError (HERR);
             Mess << "Topologic problem(3) for quad's edge nr " << nro;
             return HERR;
             }

          Quad* o_face = h_quad [ind_opp_quad [nro]];
          Edge* edge2  = o_face->findEdge (n_edge1->getVertex (1-iv1),
                                           n_edge3->getVertex (1-iv3));
          if (edge2==NULL)
             {
             setError (HERR);
             Mess << "Topologic problem(4) for quad's edge nr " << nro;
             return HERR;
             }
          // Edge* o_edge0 = h_edge [ind_edge     [nro]];
          // int sens = 1;
          // Edge* edge2   = o_face->getOpposEdge (o_edge0, sens);

          old_opp_quad [nro] = o_face;
          if (debug())
             printf (" -------- Quad oppose nro %d\n", nro);
          new_opp_quad [nro] = new Quad (n_edge0, n_edge1, edge2, n_edge3);
          }
       }

   replaceQuad (quad, new_quad);
   for (int nro=0 ; nro<QUAD4 ; nro++)
       if (make_quad[nro])
          replaceQuad (old_opp_quad [nro], new_opp_quad [nro]);

   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       replaceEdge (h_edge[ind_edge[nro]], new_quad->getEdge (nro));
       if (make_edge[nro])
          replaceEdge (old_opp_edge [nro], new_opp_edge [nro]);
       }

   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       replaceVertex (quad->getVertex(nro), new_node[nro]);
       }


   h_quad [nface] = new_quad;
   if (debug())
      {
      printf (" ... Apres disconnectQuad, new_quad=");
      new_quad->printName ("\n");
      dumpFull ();
      }

   nouveaux->addQuad   (new_quad);
   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       nouveaux->addEdge   (new_quad->getEdge   (nro));
       nouveaux->addVertex (new_quad->getVertex (nro));
       if (make_edge[nro])
          nouveaux->addEdge (new_opp_edge [nro]);
       if (make_quad[nro])
          nouveaux->addQuad (new_opp_quad [nro]);
       }
   nouveaux->moveDisco (this);
   return HOK;
}
// ========================================================= disconnectEdge
int Hexa::disconnectEdge (Edge* arete, Elements* nouveaux)
{
   int nedge  = findEdge   (arete);
   int namont = findVertex (arete->getVertex(V_AMONT));
   int naval  = findVertex (arete->getVertex(V_AVAL ));

   if (nedge==NOTHING || namont==NOTHING || naval==NOTHING)
      {
      setError (HERR);
      Mess << "Edge doesn't belong to Hexaedron";
      return HERR;
      }

   if (debug())
      {
      printf (" ... Avant disconnectEdge, arete=");
      arete->printName ("\n");
      dumpFull ();
      }

   Edge*   n_edge   [HE_MAXI];
   Quad*   n_quad   [HQ_MAXI];
   Vertex* n_vertex [HV_MAXI];

   for (int nro=0 ; nro<HQ_MAXI ; nro++) n_quad [nro]   = NULL;
   for (int nro=0 ; nro<HE_MAXI ; nro++) n_edge [nro]   = NULL;
   for (int nro=0 ; nro<HV_MAXI ; nro++) n_vertex [nro] = NULL;

   Vertex* old_amont = arete->getVertex (V_AMONT);
   Vertex* old_aval  = arete->getVertex (V_AVAL );
   Vertex* new_amont = n_vertex [namont] = new Vertex (old_amont);
   Vertex* new_aval  = n_vertex [naval]  = new Vertex (old_aval);
   n_edge [nedge]    = new Edge   (new_amont, new_aval);

   // Un edge non remplace, qui contient un vertex remplace
   //         commun a plus de 2 faces (donc appartenant a un autre hexa)
   //         doit etre duplique

   for (int nro=0 ; nro<HE_MAXI ; nro++)
       {
       if (   n_edge[nro]==NULL && h_edge[nro] != NULL
           && h_edge[nro]->getNbrParents()>2)
          {
          Vertex* va =  h_edge[nro]->getVertex (V_AMONT);
          Vertex* vb =  h_edge[nro]->getVertex (V_AVAL);

          if (va==old_amont)
             n_edge [nro] = new Edge (new_amont, vb);
          else if (va==old_aval)
             n_edge [nro] = new Edge (new_aval,  vb);
          else if (vb==old_amont)
             n_edge [nro] = new Edge (va, new_amont);
          else if (vb==old_aval)
             n_edge [nro] = new Edge (va, new_aval);
          }
       }

   // Un quad non remplace, qui contient un edge remplace
   //         commun a plus de 2 Hexas
   //         doit etre duplique

   Globale* glob = Globale::getInstance();
   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       if (   n_quad[nro]==NULL && h_quad[nro] != NULL
           && h_quad[nro]->getNbrParents()>1)
          {
          Edge* qedge[QUAD4];
          bool  duplic = false;
          for (int ned=0 ; ned<QUAD4 ; ned++)
              {
              int ndup = glob->QuadEdge (nro, (EnumQuad)ned);
              if (n_edge [ndup] ==NULL)
                 qedge [ned] = h_edge[ndup];
              else
                 {
                 qedge [ned] = n_edge[ndup];
                 duplic = true;
                 }
              }
          if (duplic)
             n_quad[nro] = new Quad (qedge[Q_A], qedge[Q_B],
                                     qedge[Q_C], qedge[Q_D]);
          }

   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       if (n_quad[nro]!=NULL)
          {
          replaceQuad (h_quad[nro], n_quad[nro]);
          nouveaux->addQuad  (n_quad[nro]);
          }

   for (int nro=0 ; nro<HE_MAXI ; nro++)
       if (n_edge[nro]!=NULL)
          {
          replaceEdge (h_edge[nro], n_edge[nro]);
          nouveaux->addEdge  (n_edge[nro]);
          }

   for (int nro=0 ; nro<HV_MAXI ; nro++)
       if (n_vertex[nro]!=NULL)
          {
          replaceVertex (h_vertex[nro], n_vertex[nro]);
          nouveaux->addVertex  (n_vertex[nro]);
          }

   if (debug())
      {
      printf (" ... Apres disconnectEdge\n");
      dumpFull ();
      }

   nouveaux->moveDisco (this);
   return HOK;
}
// ========================================================= disconnectVertex
int Hexa::disconnectVertex (Vertex* noeud, Elements* nouveaux)
{
   if (debug())
      {
      printf (" ... Avant disconnectVertex, vertex=");
      noeud->printName ("\n");
      dumpFull ();
      }

   int node = findVertex (noeud);
   if (node==NOTHING)
      {
      setError (HERR);
      Mess << "Vertex doesn't belong to Hexaedron";
      return HERR;
      }

   Vertex* new_node = new Vertex (noeud);
   Quad*   new_quad [HQ_MAXI];
   Edge*   new_edge [HE_MAXI];

   for (int nro=0 ; nro<HE_MAXI ; nro++) new_edge [nro] = NULL;
   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       {
       new_quad [nro] = NULL;
            // Cete face contient le sommet et est commune a 2 hexas
       if (   h_quad[nro]->indexVertex(noeud) >= 0
           && h_quad[nro]->getNbrParents  ()  >= 2)
           {
           int nbmod = 0;
           Edge* tedge [QUAD4];
           for (int qed=0 ; qed<QUAD4 ; qed++)
               {
               Edge* arete = tedge[qed] = h_quad[nro]->getEdge (qed);
               int   indv  = arete->index (noeud);
               if (indv>=0)
                  {
                  nbmod++;
                  int hed = findEdge (arete);
                  if (hed<0)
                     {
                     setError (HERR);
                     Mess << "Topologic problem(1) for face nr " << nro;
                     return HERR;
                     }
                  if (new_edge [hed]==NULL)
                      new_edge [hed] = new Edge (new_node,
                                                 arete->getVertex(1-indv));
                  tedge [qed] = new_edge [hed];
                  }
               }
           if (nbmod!=2)
              {
              setError (HERR);
              Mess << "Topologic problem(2) for face nr " << nro;
              return HERR;
              }
           new_quad [nro] = new Quad (tedge[0], tedge[1], tedge[2], tedge[3]);
           }
       }

   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       if (new_quad [nro] != NULL)
          {
          replaceQuad (h_quad [nro], new_quad [nro]);
          nouveaux->addQuad (new_quad[nro]);
          }

   for (int nro=0 ; nro<HE_MAXI ; nro++)
       if (new_edge [nro] != NULL)
          {
          replaceEdge (h_edge [nro], new_edge [nro]);
          nouveaux->addEdge (new_edge[nro]);
          }

   replaceVertex (noeud, new_node);
   nouveaux->addVertex (new_node);

   if (debug())
      {
      printf (" ... Apres disconnectVertex\n");
      dumpFull ();
      }

   nouveaux->moveDisco (this);
   return HOK;
}
END_NAMESPACE_HEXA
