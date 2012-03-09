
// C++ : La clase principale de Hexa

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
#include "HexDocument.hxx"
#include "HexEdge.hxx"
#include "HexDiagnostics.hxx"


BEGIN_NAMESPACE_HEXA

static bool db = false;

int vertexInLine (Vertex* mfirst, Edges& mline, vector<int> &tsens);
int associateShapes (Edges& mline, int msens[], Shape* gstart, Shapes& gline, 
                    double pstart, double pend, bool closed, bool inv=false);

// ====================================================== associateOpenedLine
int Document::associateOpenedLine (Edge*  mstart, Edges&  mline, Shape* gstart, 
                                   double pstart, Shapes& gline, double pend)
{
   int ier = associateLine (NULL, mstart, mline, gstart, pstart, gline, pend);
   return ier;
}
// ====================================================== associateClosedLine
int Document::associateClosedLine (Vertex* vfirst, Edge*  mstart, Edges& mline, 
                         Shape*  gstart, double pstart, bool inv, Shapes& gline)
{
   if (vfirst == NULL)
      {
      putError (W_ASSO_LINE3);
      return HERR;
      }

   int ier = associateLine (vfirst, mstart, mline, gstart, pstart, gline, 
                            1.0, inv);
   return ier;
}
// ====================================================== associateClosedLine
int Document::associateLine (Vertex* vfirst, Edge*  mstart, Edges& mline, 
                    Shape*  gstart, double pstart, Shapes& gline, double pend,
                    bool inv)
{
   db = on_debug ();
   char buffer [16], cnum [8];
   int  nbseg  = mline.size ();
   bool closed = vfirst != NULL;

   if (mstart == NULL)
      {
      putError (W_ASSO_LINE4);
      return HERR;
      }
                // Contour ferme : 
                // Le vertex de depart n'appartient pas a l'edge de depart
   int istart = mstart->index (vfirst);
   if (closed && istart == NOTHING) 
      {
      putError (W_ASSO_LINE2, vfirst->getName (buffer));
      return HERR;
      }

   if (db)
      {
      PutName (vfirst);
      printf ("  . : ");
      mstart->printName (" = (");
      mstart->getVertex(V_AMONT)->printName (", ");
      mstart->getVertex(V_AVAL) ->printName (")\n");

      for (int nro=0 ; nro<nbseg ; nro++)
          {
          printf (" %2d : ", nro);
          mline[nro]->printName(" = (");
          mline[nro]->getVertex(V_AMONT)->printName(", ");
          mline[nro]->getVertex(V_AVAL )->printName(")\n");
          }
      }


   for (int ns = 0 ; ns < nbseg ; ns++)
       {
       if (mline[ns] == NULL)
          {
          sprintf (cnum, "%d", ns);
          putError (W_ASSO_LINE5, cnum);
          return HERR;
          }
       }

   vector <int> tab_sens (nbseg, V_TWO);
   int     sens  = V_AMONT;
   int     nedge = NOTHING;
   Vertex* pnode = mstart->getVertex (sens);

   if (nbseg==0)
      {
      nedge = 0;
      }
   else if (NOT closed)
      {
      pnode = mstart->getVertex (sens);
      nedge = vertexInLine (pnode, mline, tab_sens);
      if (nedge == NOTHING) 
         {
         sens  = V_AVAL;
         pnode = mstart->getVertex (sens);
         nedge = vertexInLine (pnode, mline, tab_sens);
         }
      }
        // Closed : on recherche ou se trouve le 2e vertex de mstart
   else
      {
      sens  = 1-istart;
      pnode = mstart->getVertex (sens);
      nedge = vertexInLine (pnode, mline, tab_sens);
      }

   HexDisplay (nbseg);

   if (nedge == NOTHING) 
      {
      putError (W_ASSO_LINE, mstart->getName (buffer));
      return HERR;
      }

   Edges        les_edges;
   vector <int> les_orig; 
   les_edges.push_back (mstart);
   les_orig .push_back (1-sens);
   HexDisplay (nedge);
   cout << " ........................................ Marque 1" << endl;
   if (nbseg>0) 
      {
      les_edges.push_back (mline[nedge]);
      les_orig. push_back (tab_sens[nedge]);
      }

   cout << " ........................................ Marque 2" << endl;
   if (db) 
      printf (" mstart  [%d] = mline[%d][%d] = %s\n", sens, nedge, 
                                  tab_sens [nedge], pnode->getName(buffer));

   cout << " ........................................ Marque 3" << endl;
   for (int ns = 1 ; ns < nbseg ; ns++)
       {
       Vertex* pnode = mline[nedge]->getVertex (1-tab_sens [nedge]);
       int nro = vertexInLine (pnode, mline, tab_sens);
       if (nro == NOTHING) 
          {
          sprintf (cnum, "%d", nedge);
          putError (W_ASSO_LINE2, cnum, mline[nedge]->getName (buffer));
          return HERR;
          }
       if (db) 
          printf (" mline[%d][%d] = mline[%d][%d] = %s\n", nedge, 
               1-tab_sens [nedge], nro, tab_sens [nro], pnode->getName(buffer));
       nedge = nro;
       les_edges.push_back (mline    [nedge]);
       les_orig. push_back (tab_sens [nedge]);
       }

   /********************
   if (db)
      {
      printf (" ... gstart = 0x%x, pstart=%g\n", gstart, pstart);
      for (int ns = 0 ; ns < gline.size() ; ns++)
          {
          printf (" ... gline[%d] = 0x%x\n", ns, gline[ns]);
          }
       }
   **************************/
    
   if (closed)
      {
      Vertex* tete  = les_edges [0]    ->getVertex (les_orig[0]);
      Vertex* queue = les_edges [nbseg]->getVertex (1-les_orig[nbseg]);
      if (tete != queue)
         {
         HexDump (tete);
         HexDump (queue);
         HexDump (les_edges [0]);
         HexDump (les_edges [nbseg]);
         putError (W_ASSO_LINE6);
         return HERR;
         }
      }
   
   int ier = associateShapes (les_edges, &les_orig[0], gstart, gline, pstart, 
                              pend, closed, inv);
   return ier;
}
//
// ====================================================== vertexInLine
int vertexInLine (Vertex* mfirst, Edges& mline, vector<int> &tsens)
{
   int nbseg = mline.size ();

   for (int ned = 0 ; ned<nbseg ; ned++)
       {
       if (tsens [ned] == V_TWO)
          for (int ns=V_AMONT; ns<=V_AVAL ; ns++)
              {
              if (mline[ned]->getVertex (ns) == mfirst)
                 {
                 tsens [ned] = ns;
                 return ned; 
                 }
              }
       }
 
   return NOTHING;
}

END_NAMESPACE_HEXA
