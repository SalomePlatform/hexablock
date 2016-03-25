//
// CC++ : Fonctions Interface Cascade
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

#include "HexKas_functions.hxx"
//  #include "HexEdge.hxx"

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>

#include <gp_Pnt.hxx>

#include <sstream>

BEGIN_NAMESPACE_HEXA

// ====================================================== same_coords
bool same_coords (gp_Pnt& pa, gp_Pnt& pb, double epsilon)
{
   double d2 = carre (pb.X()-pa.X()) + carre (pb.Y()-pa.Y())
                                     + carre (pb.Z()-pa.Z()) ;
   return d2 < epsilon;
}
// ====================================================== save_brep
void save_brep (cpchar nom, string brep, int& compteur)
{
    char buff[8];
    compteur ++;
    sprintf (buff ,"%d", compteur);

    string name (nom);
    name += buff;
    name += ".brep";
    FILE*    fic = fopen (name.c_str(), "w");
    fprintf (fic, "%s\n", brep.c_str());
    fclose  (fic);
}
// ====================================================== geom_make_brep
void geom_make_brep (TopoDS_Shape& shape, string& brep)
{
   ostringstream  stream_shape;
   BRepTools::Write (shape, stream_shape);
   brep = stream_shape.str();
}
// ====================================================== geom_brep2shape (F)
TopoDS_Shape geom_brep2shape (rcstring brep)
{
   TopoDS_Shape topo;
   istringstream streamBrep(brep);
   BRep_Builder aBuilder;
   BRepTools::Read (topo, streamBrep, aBuilder);
   return topo;
}
// ====================================================== geom_brep2shape
int geom_brep2shape (rcstring brep, TopoDS_Shape& shape)
{
   istringstream streamBrep(brep);
   BRep_Builder  aBuilder;
   BRepTools::Read (shape, streamBrep, aBuilder);
   return HOK;
}
// ====================================================== geom_brep2point
int geom_brep2point (rcstring brep, double& px, double& py, double& pz)
{
   TopoDS_Shape topo;
   geom_brep2shape (brep, topo);

   if (topo.ShapeType() != TopAbs_VERTEX)
      return HERR;

   TopoDS_Vertex g_vertex = TopoDS::Vertex (topo);
   gp_Pnt        g_point  = BRep_Tool::Pnt( g_vertex );

   px = g_point.X();
   py = g_point.Y();
   pz = g_point.Z();
   return HOK;
}
// ====================================================== clear_associations
//  void clear_associations (Edge* edge)
//  {
   //  edge->clearAssociation();
   //  edge->getVertex(V_AMONT)->clearAssociation();
   //  edge->getVertex(V_AVAL )->clearAssociation();
//  }
// ====================================================== clean_brep
void clean_brep (string& brep)
{
   TopoDS_Shape  shape;
   BRep_Builder  builder;
   istringstream stream_brep (brep);

   BRepTools::Read  (shape, stream_brep, builder);
   BRepTools::Clean (shape);

   ostringstream    stream_shape;
   BRepTools::Write (shape, stream_shape);
   brep = stream_shape.str();
}
END_NAMESPACE_HEXA
