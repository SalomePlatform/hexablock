
// C++ : Gestion des Shapes (Hexa v5)

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
// See http://www.salome-platform.org/ 
// or email : webmaster.salome@opencascade.com
//


#include "HexNewShape.hxx" 
#include "HexSubShape.hxx" 

#ifndef NO_CASCADE
#include <TopoDS.hxx>
// #include <TopoDS_Shape.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <gp_Pnt.hxx>


BEGIN_NAMESPACE_HEXA

static bool db = on_debug ();  // == getenv ("HEXA_DB") > 0

// ====================================================== Constructeur
NewShape::NewShape (Document* dad)
        : EltBase  (dad, EL_SHAPE)
{
   sh_origin    =  SH_NONE; 
   brep_defined =  shape_defined = false;

   sh_level     = sh_indice      = 0;
   sh_face_max  = sh_edge_max    = sh_vertex_max = 0;
}
// ====================================================== setShape
void NewShape::setShape (const TopoDS_Shape& shape)
{
   sh_origin = SH_IMPORT;
   geo_shape = shape;
   geo_brep  = "";

   shape_defined = true;
   brep_defined  = false;

   map_shape .Clear ();
   tab_face  .clear ();
   tab_edge  .clear ();
   tab_vertex.clear ();

   sh_level    = sh_indice   = 0;
   sh_face_max = sh_edge_max = sh_vertex_max = 0;

   parseShape (shape);
}
// ====================================================== getGeoShape
const TopoDS_Shape& NewShape::getGeoShape (int id)
{
   const TopoDS_Shape& shape = map_shape.FindKey (id);
   return shape;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// ===================================================== parseShape
void NewShape::parseShape (const TopoDS_Shape& shape)
{
   push_level ();
   addSubShape (shape);

   TopoDS_Iterator iter (shape);
   for (; iter.More(); iter.Next()) 
       {
       TopoDS_Shape sub_shape = iter.Value();
       parseShape (sub_shape);
       }

   pop_level ();
}
// ===================================================== addSubShape
void NewShape::addSubShape (const TopoDS_Shape& shape)
{
   sh_indice ++;
   SubShape* sub_shape = NULL;
   
   int nro  = map_shape.Add (shape);
   int type = shape.ShapeType();

   switch (type)
      {
      case TopAbs_FACE :
           if (nro > sh_face_max)
              {
              sub_shape = new SubShape (this, nro, 2);
              tab_face.push_back (sub_shape);
              sh_face_max = nro;
              }
           break;
      case TopAbs_EDGE  :
           if (nro > sh_edge_max)
              {
              sub_shape = new SubShape (this, nro, 1);
              tab_edge.push_back (sub_shape);
              sh_edge_max = nro;
              }
           break;
      case TopAbs_VERTEX :
           if (nro > sh_vertex_max)
              {
              sub_shape = new SubShape (this, nro, 0);
              tab_vertex.push_back (sub_shape);
              sh_vertex_max = nro;
              }
           break;
      default : ;
      }

   if (NOT db)
       return;

   char chnum [8];
   sprintf (chnum, "%4d : ", sh_indice);

   cout <<  chnum;
   for (int nro=1; nro<sh_level; nro++) cout << " | ";

   switch (type)
      {
      case TopAbs_FACE :
           cout <<  " Face nro " << nro;
           break;

      case TopAbs_EDGE  :
           cout <<  " Edge nro " << nro;
           break;
      case TopAbs_VERTEX :
           {
           TopoDS_Vertex gver = TopoDS::Vertex (shape);
           gp_Pnt      gpoint = BRep_Tool::Pnt (gver);

           cout <<  " Vertex nro " << nro;
           cout << " = (" << gpoint.X() 
                << ","    << gpoint.Y() 
                << ","    << gpoint.Z() << ")";
           }
           break;
      default : 
           cout <<  " typ=" << type;
           cout <<  " nro=" << nro;
      }

   cout << endl;
}
END_NAMESPACE_HEXA
#else
// ====================================================== Constructeur
BEGIN_NAMESPACE_HEXA
NewShape::NewShape (Document* dad) : EltBase  (dad, EL_SHAPE) {}
void   NewShape::setShape (const TopoDS_Shape& shape)         {}

END_NAMESPACE_HEXA
#endif
