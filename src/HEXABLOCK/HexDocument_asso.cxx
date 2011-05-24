
// C++ : La clase principale de Hexa

#include "HexDocument.hxx"
#include "HexEdge.hxx"
#include "HexDiagnostics.hxx"

//  #include "HexEltBase.hxx"
//  #include "HexVertex.hxx"
//  #include "HexQuad.hxx"
//  #include "HexHexa.hxx"

//  #include "HexElements.hxx"
//  #include "HexCrossElements.hxx"

//  #include "HexVector.hxx"
//  #include "HexCylinder.hxx"
//  #include "HexPipe.hxx"
//  #include "HexMatrix.hxx"
//  #include "HexCloner.hxx"
//  #include "HexPropagation.hxx"
//  #include "HexLaw.hxx"

//  #include "HexXmlWriter.hxx"
//  #include "HexXmlTree.hxx"
//  #include "HexGlobale.hxx"
//  #include "HexGroups.hxx"


BEGIN_NAMESPACE_HEXA

static bool db = true;

int vertexInLine (Vertex* mfirst, Edges& mline, vector<int> &tsens);

// ====================================================== associateOpenedLine
int Document::associateOpenedLine (Edge*  mstart, Edges&  mline, Shape* gstart, 
                                   double pstart, Shapes& gline, double pend)
{
   int ier = associateLine (NULL, mstart, mline, gstart, pstart, gline, pend);
   return ier;
}
// ====================================================== associateClosedLine
int Document::associateClosedLine (Vertex* vfirst, Edge*  mstart, Edges& mline, 
                          Shape*  gstart, double pstart, Shapes& gline)
{
   if (vfirst == NULL)
      {
      putError (W_ASSO_LINE3);
      return HERR;
      }

   int ier = associateLine (vfirst, mstart, mline, gstart, pstart, gline, 1.0);
   return ier;
}

// ====================================================== associateClosedLine
int Document::associateLine (Vertex* vfirst, Edge*  mstart, Edges& mline, 
                    Shape*  gstart, double pstart, Shapes& gline, double pend)
{
   char buffer [16], cnum [8];
   int  nbseg = mline.size ();

   bool closed = vfirst != NULL;

   if (db)
      {
      PutName (vfirst);
      mstart->printName (" = (");
      mstart->getVertex(V_AMONT)->printName (", ");
      mstart->getVertex(V_AVAL) ->printName (")\n");

      for (int nro=0 ; nro<nbseg ; nro++)
          {
          printf (" %2d : ", nro);
          mline[nro]->printName(" = (");
          mline[nro]->getVertex(V_AMONT)->printName(", ");
          mline[nro]->getVertex(V_AVAL )->printName(")\n ");
          }
      }

   if (mstart == NULL)
      {
      putError (W_ASSO_LINE4);
      return HERR;
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
   Vertex* pnode = NULL;
   int     sens  = V_AMONT;
   int     nedge = NOTHING;

   if (NOT closed)
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
   else
      {
      sens = mstart->index (vfirst);
      if (sens == NOTHING) 
         {
         putError (W_ASSO_LINE2, vfirst->getName (buffer));
         return HERR;
         pnode = mstart->getVertex (sens);
         nedge = vertexInLine (pnode, mline, tab_sens);
         }
      sens  = 1-sens;
      pnode = mstart->getVertex (sens);
      nedge = vertexInLine (pnode, mline, tab_sens);
      }

   if (nedge == NOTHING) 
      {
      putError (W_ASSO_LINE, mstart->getName (buffer));
      return HERR;
      }

   Edges        les_edges;
   vector <int> les_orig; 
   les_edges.push_back (mstart);
   les_orig .push_back (1-sens);
   les_edges.push_back (mline[nedge]);
   les_orig. push_back (tab_sens[nedge]);

   if (db) 
      printf (" mstart  [%d] = mline[%d][%d] = %s\n", sens, nedge, 
                                  tab_sens [nedge], pnode->getName(buffer));

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

   printf (" ... gstart = 0x%x, pstart=%g\n", gstart, pstart);
   nbseg = gline.size ();
   for (int ns = 0 ; ns < nbseg ; ns++)
       {
       printf (" ... gline[%d] = 0x%x\n", ns, gline[ns]);
       }
    
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
   
   int ier = associateCascade (les_edges, &les_orig[0], gstart, gline, pstart, 
                               pend, closed);
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
