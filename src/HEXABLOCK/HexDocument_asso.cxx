
// C++ : Associations dans le document

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
#include "HexEdge.hxx"
#include "HexQuad.hxx"
#include "HexDiagnostics.hxx"
#include "HexNewShape.hxx"
#include "HexEdgeShape.hxx"
#include "HexPropagation.hxx"

#include <map>

BEGIN_NAMESPACE_HEXA

static bool db = on_debug ();  // == getenv ("HEXA_DB") > 0


int vertexInLine (Vertex* vfirst, Edges& mline, vector<int> &tsens);
// ====================================================== vertexInLine
int vertexInLine (Vertex* vfirst, Edges& mline, vector<int> &tsens)
{
   int nbseg = mline.size ();

   for (int ned = 0 ; ned<nbseg ; ned++)
       {
       if (tsens [ned] == V_TWO)
          for (int ns=V_AMONT; ns<=V_AVAL ; ns++)
              {
              if (mline[ned]->getVertex (ns) == vfirst)
                 {
                 tsens [ned] = ns;
                 return ned;
                 }
              }
       }

   return NOTHING;
}
// ====================================================== clearAssociation
void Document::clearAssociation ()
{
   clearAssoVertices ();
   clearAssoEdges ();
   clearAssoQuads ();
}
// ====================================================== clearAssociation (t)
int Document::clearAssociation (EnumElt type)
{
   switch (type)
      {
      case EL_VERTEX  :
           clearAssoVertices ();
           break;
      case EL_EDGE  :
           clearAssoEdges ();
           break;
      case EL_QUAD  :
           clearAssoQuads ();
           break;
      default : return HERR;
      }

   return HOK;
}
// ====================================================== clearAssoVertices
void Document::clearAssoVertices ()
{
   for (EltBase* elt = doc_first_elt[EL_VERTEX]->next (); elt!=NULL;
                 elt = elt->next())
       {
       Vertex* elem = static_cast <Vertex*> (elt);
       if (elem->isValid ())
           elem->clearAssociation ();
       }
}
// ====================================================== clearAssoEdges
void Document::clearAssoEdges ()
{
   for (EltBase* elt = doc_first_elt[EL_EDGE]->next (); elt!=NULL;
                 elt = elt->next())
       {
       Edge* elem = static_cast <Edge*> (elt);
       if (elem->isValid ())
           elem->clearAssociation ();
       }
}
// ====================================================== clearAssoQuads
void Document::clearAssoQuads ()
{
   for (EltBase* elt = doc_first_elt[EL_QUAD]->next (); elt!=NULL;
                 elt = elt->next())
       {
       Quad* elem = static_cast <Quad*> (elt);
       if (elem->isValid ())
           elem->clearAssociation ();
       }
}
// ====================================================== addShape (1)
NewShape* Document::addShape (TopoDS_Shape& topo, cpchar name)
{
   NewShape* shape = new NewShape (this);
   doc_tab_shape.push_back (shape);

   shape->setName  (name);
   shape->setShape (topo);
   return shape;
}
// ====================================================== addShape (2)
NewShape* Document::addShape (cpchar name, EnumShape type)
{
   NewShape* shape = new NewShape (this, type);
   doc_tab_shape.push_back (shape);

   shape->setName  (name);
   return shape;
}
// ====================================================== addCloud
void Document::addCloud ()
{
   doc_cloud = new NewShape (this, SH_CLOUD);
   doc_cloud->setName  ("cloud");

   doc_tab_shape.push_back (doc_cloud);
}
// ====================================================== getShape
NewShape* Document::getShape (int nro)
{
   int nbre = doc_tab_shape.size ();
   if (nro < 0 || nro >= nbre)
      return NULL;

   return doc_tab_shape [nro];
}
// ====================================================== getFirstExplicitShape
cpchar Document::getFirstExplicitShape()
{
   int nbre = doc_tab_shape.size ();
   for (int nro=0 ; nro < nbre ; ++nro)
       {
       NewShape* shape = doc_tab_shape [nro];
       if (shape!= NULL && shape->getOrigin()==SH_IMPORT)
          return shape->getBrep ();
       }

   return NULL;
}
// ====================================================== findShape
NewShape* Document::findShape (rcstring nom)
{
   int nbre = doc_tab_shape.size ();
   for (int nro=0 ; nro<nbre ; ++nro)
       if (nom == doc_tab_shape [nro]->getName())
          return  doc_tab_shape [nro];

   return NULL;
}
// ====================================================== find_line
EdgeShape* find_line (EdgeShapes& gline, double* point, int& sens)
{
   int nbseg = gline.size ();

   for (int ned = 0 ; ned<nbseg ; ned++)
       {
       EdgeShape* edge = gline [ned];
       if (edge!=NULL)
          {
          sens = edge->onExtremity (point);
          if (sens >=0)
             {
             gline [ned] = NULL;
             return edge;
             }
          }
       }
   return NULL;
}
// ====================================================== find_edge
Edge* find_edge (Edges& mline, Vertex* vfirst, int& sens)
{
   int nbseg = mline.size ();

   for (int ned = 0 ; ned<nbseg ; ned++)
       {
       Edge* edge = mline [ned];
       if (edge!=NULL)
          {
          sens = edge->index (vfirst);
          if (sens >=0)
             {
             mline [ned] = NULL;
             return edge;
             }
          }
       }
   return NULL;
}
// ====================================================== associateOpenedLine
int Document::associateOpenedLine (Edges  mline, NewShapes gline,
                                   IntVector tabid,
                                   double pstart, double pend)
{
   cout << "_________________________________ Asso Open Line"
        << " ed=" << mline.size()
        << " sh=" << tabid.size()
        << endl;
   int ier = associateLine (NULL, mline, gline, tabid, pstart, pend, false);
   return ier;
}
// ====================================================== associateClosedLine
int Document::associateClosedLine (Vertex* vfirst,   Edges  mline,
                                   NewShapes gline, IntVector tabid,
                                   double pstart, bool inv)
{
   if (vfirst == NULL)
      {
      putError (W_ASSO_LINE3);
      return HERR;
      }

   cout << "_________________________________ Asso Closed Line"
        << " first=" << vfirst->getName()
        << " ed=" << mline.size()
        << " sh=" << tabid.size()
        << " inv=" << inv
        << endl;
   int ier = associateLine (vfirst, mline, gline, tabid, pstart, 1.0, inv);
   PutData (ier);
   return ier;
}
// ====================================================== associateLine
int Document::associateLine (Vertex* vfirst, Edges& mline,
                             NewShapes& tshapes, IntVector& subid,
                             double pstart, double pend, bool inv)
{
   EdgeShapes glines;
   Edges      tedges;
   IntVector  gsens, tsens;
   double     lgtotale;

   bool closed = vfirst!=NULL;
   int ier = sortShapes (tshapes, subid, closed, inv, pstart, pend,
                         glines,  gsens, lgtotale);
   if (ier != HOK)
       return ier;

   ier = sortEdges (vfirst, mline, tedges, tsens);
   if (ier != HOK)
       return ier;

   int nbedges  = tedges.size();
   int nblines  = glines.size();

   EdgeShape* gstart  = glines [0];
   EdgeShape* gend    = glines [nblines-1];
   bool       boucle  = closed && nblines==1;

   double abstart = inv ? (1-pstart) : pstart;
   if (NOT closed)
      {
      bool   rev     = gsens  [nblines-1]!=0;
      double abend   = rev ?  pend      : (1-pend);
      lgtotale = lgtotale - abstart * gstart->getLength()
                          - abend   * gend  ->getLength();
      }
                                              // 1er point
   Real3  point;
   gstart->getPoint (pstart, point);
   int vsid     = doc_cloud->addPoint  (point);
   Vertex* node = tedges[0]->getVertex (tsens[0]);
   node->setAssociation (doc_cloud, vsid);

   double lgedge = lgtotale/nbedges;
   double smax   = gstart->getLength()*(1-abstart);
   double emax   = lgedge;
   double pdeb   = pstart;
   double ablast = 0;
   double tol    = 1e-2;

   int nsh   = 0;
   int ned   = 0;
   while (ned < nbedges && nsh < nblines)
       {
       EdgeShape* shape = glines [nsh];
       Edge*      edge  = tedges [ned];
       bool       grev  = gsens  [nsh]==1;
       bool    av_shape = smax < emax + tol;
       bool    av_edge  = emax < smax + tol;
       bool    vassoc   = av_edge && NOT (closed && ned == nbedges-1);
       double  pfin     = 1;
       double  lgshape  = shape->getLength ();

       double abscisse = std::min (emax, smax);
       double decal    = (abscisse-ablast) / lgshape;
       if (grev)
          {
          pfin = pdeb - decal;
          if (pfin < 0.0) pfin = 0;
          }
       else
          {
          pfin = pdeb + decal;
          if (pfin > 1.0) pfin = 1;
          }

       edge->addAssociation (shape, pdeb, pfin);

       if (vassoc)
          {
          shape->getPoint (pfin, point);
          vsid  = doc_cloud->addPoint  (point);
          node  = edge  ->getVertex (1-tsens[ned]);

          node->setAssociation (doc_cloud, vsid);
          }
       ablast = abscisse;
       pdeb   = pfin;
                       // On incremente les edges
       if (av_edge)
          {
          ned ++;
          emax += lgedge;
          }
                       // On incremente les shapes
       if (av_shape)
          {
          pdeb = 0;
          if (boucle)
             smax = shape->getLength ();
          else
             {
             nsh ++;
             if (nsh<nblines)
                 smax += glines[nsh]->getLength();
             }
          }
       }

   return HOK;
}
// ====================================================== sortShapes
int Document::sortShapes (NewShapes&  tshapes, IntVector& subid, bool closed,
                      bool inv, double pstart, double pend,
                      EdgeShapes& glines,  IntVector& gsens, double& lgtotale)
{
   char cnum [8];
   int nblines = subid.size ();
   int nombre  = tshapes.size ();

   if (nblines==0 || nombre==0)
      return HERR;

   if (nombre != 1 && nombre != nblines)
      return HERR;

   for (int ns = 0 ; ns < nombre ; ns++)
       {
       if (tshapes[ns] == NULL)
          {
          sprintf (cnum, "%d", ns);
          putError (W_ASSO_LINE5, cnum);
          return HERR;
          }
       }

   NewShape*  geom = tshapes [0];
   EdgeShapes buf_lines;
   glines.clear ();
   gsens .clear ();
   for (int ns = 0 ; ns < nblines ; ns++)
       {
       if (nombre > 1)
           geom = tshapes [ns];
       EdgeShape* shape = geom->findEdge (subid[ns]);
       if (shape==NULL)
          {
          sprintf  (cnum, "%d", ns);
          putError (W_ASSO_LINE5, cnum);
          return HERR;
          }
       buf_lines.push_back (shape);
       }

   Real3      pnext;
   EdgeShape* gnext    = buf_lines[0];
   buf_lines[0] = NULL;
   int  reste   = nblines - 1;
   int  rev     = inv;

   lgtotale = gnext->getLength();
   glines.push_back (gnext);
   gsens .push_back (inv);

   for (int nro = 0 ; nro < reste ; nro++)
       {
       double param = 1-rev;
       gnext->getPoint (param, pnext);
       gnext = find_line (buf_lines, pnext, rev);
       if (gnext == NULL)
          return HERR;
       lgtotale += gnext->getLength();
       glines.push_back (gnext);
       gsens .push_back (rev);
       }

   if (closed)
      {
      EdgeShape* gfirst = glines[0];
      gfirst->getPoint (1-inv, pnext);
      int ext = gnext->onExtremity (pnext);
      if (ext<0)
         return HERR;
      }
   return HOK;
}
// ====================================================== sortEdges
int Document::sortEdges (Vertex* vfirst, Edges& mline, Edges& tab_edge,
                          IntVector& tab_sens)
{
   char cnum [8];
   bool closed = vfirst!=NULL;

   int nbedges = mline.size ();
   if (nbedges==0)
      return HERR;

   Edge* mstart = mline[0];    // Coherence ancienne version
                // Contour ferme :
                // Le vertex de depart n'appartient pas a l'edge de depart
   int istart = mstart->index (vfirst);
   if (closed && istart == NOTHING)
      {
      putError (W_ASSO_LINE2, vfirst->getName());
      return HERR;
      }

   tab_edge.clear();
   tab_sens.clear();
   Edges  buf_edge;
   for (int nro = 0 ; nro < nbedges ; nro++)
       {
       Edge* edge = mline[nro];
       if (edge == NULL)
          {
          sprintf (cnum, "%d", nro);
          putError (W_ASSO_LINE5, cnum);
          return HERR;
          }
       buf_edge.push_back (edge);
       }

   Vertex* vnext = NULL;
   Edge*   enext = mstart;
   int sens      = istart;
   int reste     = nbedges - 1;
   tab_edge.push_back (mstart);
   buf_edge[0]  = NULL;

   if (closed)
      {
      tab_sens.push_back (istart);
      }
   else
      {
      istart = V_AMONT;
      vnext  = mstart->getVertex (1-istart);
      enext  = find_edge (buf_edge, vnext, sens);
      if (enext==NULL)
         {
         istart = 1-istart;
         vnext  = mstart->getVertex (1-istart);
         enext  = find_edge (buf_edge, vnext, sens);
         }
      if (enext==NULL)
         return HERR;
      tab_sens.push_back (istart);
      tab_edge.push_back (enext);
      tab_sens.push_back (sens);
      reste--;
      }

   for (int nro = 0 ; nro < reste ; nro++)
       {
       vnext = enext->getVertex (1 - sens);
       enext = find_edge (buf_edge, vnext, sens);
       if (enext == NULL)
          return HERR;
       tab_edge.push_back (enext);
       tab_sens.push_back (sens);
       }

   istart = tab_edge [nbedges-1]->index (vfirst);
   if (closed && istart == NOTHING)
      {
      putError (W_ASSO_LINE2, vfirst->getName());
      return HERR;
      }

   return HOK;
}
// ====================================================== checkAssociations
int Document::checkAssociations ()
{
   int nombre = countUsedEdge();
   int nberrors = 0;
   for (int ned=0 ; ned<nombre ; ++ned)
       {
       Edge* edge = getUsedEdge (ned);
       int ier = edge->checkAssociation ();
       if (ier != HOK) nberrors ++;
       }

   cout << " ... Check edges assotiations : " << nberrors
        << " error(s)." << endl;

   if (nberrors>0) return HERR;
      else         return HOK;
}
// ====================================================== addLaws
int Document::addLaws (double lgmoy, bool usemax)
{
   const double Epsilon   = 1e-6;
   const double Precision = 1e-2;
   const double BigNumber = 1e+36;

   std::map <int, Law*>    existing_law;
   std::map <int, Law*> :: iterator iter;

   if (lgmoy<=Epsilon)
      return HERR;

   int nbprop = countPropagation();
   for (int np=0 ; np<nbprop ; ++np)
       {
       Propagation* prop      = getPropagation (np);
       const Edges& tab_edges = prop->getEdges ();
       double lgref = usemax ? 0 : BigNumber;
       int  nbedges = tab_edges.size();
       for (int ned=0 ; ned<nbedges ; ++ned)
           {
           Edge*  edge = tab_edges [ned];
           double lg   = edge->getAssoLen ();
           if (usemax)
              lgref = std::max (lgref, lg);
           else
              lgref = std::min (lgref, lg);
           }
        lgref        = lgref / lgmoy;
        int    nbcut = (int) lgref; 
        double delta =  lgref - (double) nbcut; 
        if (NOT usemax || (delta < Precision))
            nbcut --;

        iter = existing_law.find (nbcut);
        if (iter == existing_law.end())
           {
           char name [2];
           sprintf (name, "u_%02d", nbcut);
           existing_law [nbcut] = addLaw (name, nbcut);
           }
       prop->setLaw (existing_law [nbcut]);
       }
   return HOK;
}
END_NAMESPACE_HEXA
