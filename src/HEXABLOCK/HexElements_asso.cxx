//
// CC++ : Ancienne interface Cascade de la classe Elements ** A SUPPRIMER **
//
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

#include "HexElements.hxx"
#include "HexEdge.hxx"
#include "HexDocument.hxx"
#include "HexEdgeShape.hxx"
#include "HexAssoEdge.hxx"
#include "HexVertex.hxx"

/*****************************************
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>

#include <GCPnts_AbscissaPoint.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Vertex.hxx>

#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
                                    // Cercles
#include <GC_MakeCircle.hxx>
#include <Geom_Circle.hxx>
#include <gp_Circ.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Tool.hxx>
***************************************************/

BEGIN_NAMESPACE_HEXA

// ---------------------------------------------------------------------
bool requals (const double pnt1[], const double pnt2[]);
// ========================================================= cutAssociation
void Elements::cutAssociation (Edge* orig, Edges& tedges)
{
   int   nbedges = tedges.size();
   int   nbassos = orig->countAssociation();

                            // ----------- Ordonnancement des shapes
   PutData (nbassos);
   double lgpara   = 0;
   for (int ns = 0 ; ns<nbassos ; ns++)
       {
       AssoEdge* asso = orig->getAssociation (ns);
       lgpara            += fabs (asso->getEnd () - asso->getStart ());
       }

   double     delta  = lgpara / nbedges;
   int        nasso  = 0;
   AssoEdge*  asso   = orig->getAssociation (nasso);
   EdgeShape* shape  = asso->getEdgeShape ();
   double     param1 = asso->getStart ();
   double     parmax = asso->getEnd   ();

   for (int ned = 0 ; ned<nbedges ; ned++)
       {
       Edge*   edge  = tedges [ned];
       double param2 = param1 + delta;
       if (ned>0)
          {
          Real3 point;
          Vertex* node = edge->getVertex (V_AMONT);
          shape->getPoint (param1, point);
          node->setAssociation (point);
          }
       if (param2 <= parmax)
          edge->addAssociation (shape, param1, param2);
       else
          {
          edge->addAssociation (shape, param1, parmax);
          nasso ++;
          if (nasso >= nbassos)
             return;
          asso   = orig->getAssociation (nasso);
          shape  = asso->getEdgeShape ();
          param2 = param2 - 1;
          parmax = asso->getEnd   ();
          edge->addAssociation (shape, 0, param2);
          }
       param1 = param2;
       }
}
// ====================================================== geom_dump_asso
void geom_dump_asso (Edge* edge)
{
   cout << endl;
   cout << " _______________________________________________________ "
         << " dump_edge :" << endl;

   if (edge==NULL || NOT edge->isHere ())
      {
      cout << "*** deleted ***" << endl;
      return;
      }

   cout << endl;
   cout << " ... Edge = " << edge->getName ()
        << " = (" << edge->getVertex(V_AMONT)->getName()
        <<   ", " << edge->getVertex(V_AVAL )->getName() << ")" << endl;

   for (int nro=0 ; nro<V_TWO ; nro++)
       {
       Vertex* node  = edge->getVertex (nro);
       cout << " ... " << node->getName()
            <<  " = (" << node->getX()
            <<    ", " << node->getY()
            <<    ", " << node->getZ() << ")";

       if (node->isAssociated())
           cout << " --> (" << node->getAssoX()
                <<     ", " << node->getAssoY()
                <<     ", " << node->getAssoZ() << ")";
       else
           cout << " (not associated)";
       cout << endl;
       }

   int nbr_associations = edge->countAssociation ();
   PutData (nbr_associations);

   for (int nro=0 ; nro<nbr_associations ; nro++)
       {
       AssoEdge* asso = edge->getAssociation (nro);
       cout << " *** Association nro " << nro+1 << endl;
       const double* shape_origin = asso->getOrigin();
       const double* shape_extrem = asso->getExtrem();
       PutData  (asso->getStart());
       PutData  (asso->getEnd());
       PutCoord (shape_origin);
       PutCoord (shape_extrem);
       }
   cout << endl << endl;
}
END_NAMESPACE_HEXA
