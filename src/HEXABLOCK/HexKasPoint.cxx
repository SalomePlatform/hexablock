//
// CC++ : Interface Cascade de la classe Elements
//
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

#include "HexKasPoint.hxx"
#include "HexVertex.hxx"
#include "HexOldShape.hxx"

#ifndef NO_CASCADE

#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>

#include <GCPnts_AbscissaPoint.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include <gp_Pnt.hxx>

BEGIN_NAMESPACE_HEXA

static bool db = false;

// ======================================================= Constructeur
KasPoint::KasPoint ()
{
   db = on_debug();
   razPoint ();
}
// ======================================================= razPoint
void KasPoint::razPoint ()
{
   is_ok = false;
   b_rep = "";
   for (int nc=0; nc <DIM3 ; nc++) g_coord [nc] = 0;
}
// ======================================================= definePoint (vertex)
int KasPoint::definePoint (Vertex* node)
{
   return HERR;
#if 0
   razPoint ();

   if (node==NULL)
      return HERR;

   Shape* asso = node->getAssociation();
   if (asso==NULL)
      return HERR;

   b_rep = asso->getBrep ();

   TopoDS_Shape topo;
   istringstream streamBrep(b_rep);
   BRep_Builder aBuilder;
   BRepTools::Read(topo, streamBrep, aBuilder);

   if (topo.ShapeType() != TopAbs_VERTEX)
      return HERR;

   is_ok    = true;
   g_vertex = TopoDS::Vertex (topo);
   g_point  = BRep_Tool::Pnt( g_vertex );

   g_coord [dir_x] = g_point.X();
   g_coord [dir_y] = g_point.Y();
   g_coord [dir_z] = g_point.Z();
   return HOK;
#endif
}
// ======================================================= definePoint (xyz)
void KasPoint::definePoint (Real3 coord)
{
   gp_Pnt gpoint (coord[dir_x], coord[dir_y], coord[dir_z]);
   definePoint   (gpoint);
}
// ======================================================= definePoint (g_pnt)
void KasPoint::definePoint (gp_Pnt& gpoint)
{
   is_ok   = true;
   g_point = gpoint;

   ostringstream   stream_shape;

   BRepBuilderAPI_MakeVertex mkVertex (g_point);
   TopoDS_Shape              aShape = mkVertex.Shape();
   BRepTools::Write(aShape, stream_shape);

   g_vertex = TopoDS::Vertex( aShape );
   b_rep    = stream_shape.str();

   g_coord [dir_x] = g_point.X();
   g_coord [dir_y] = g_point.Y();
   g_coord [dir_z] = g_point.Z();

   if (db)
      {
      cout << " KasPoint::definePoint :" << endl;
      PutCoord (g_coord);
      }
}
// ======================================================= associate
void KasPoint::associate (Vertex* node)
{
/****************
   if (db) cout << " ++ KasPoint::associate " << endl;
   if (db) PutName (node);
   if (node==NULL)
      return;

   Shape* vshape = new Shape (b_rep);
   node->setAssociation (vshape);

*********************/
}
END_NAMESPACE_HEXA
#endif
