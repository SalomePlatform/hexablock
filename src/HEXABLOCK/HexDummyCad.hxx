
// Class : Ecriture d'un dump python

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

#ifndef __DUMMY_CASCAD_
#define __DUMMY_CASCAD_

class BRepAdaptor_Curve;
class BRepAlgoAPI_Section;
class BRepBuilderAPI_MakeEdge;
class BRepBuilderAPI_MakeVertex;
class BRepBuilderAPI_Transform;
class BRepPrimAPI_MakeCylinder;
class BRepPrimAPI_MakeSphere;
class BRepTools;
class BRep_Builder;
class BRep_Tool;
class GC_MakeCircle;
class GeomAdaptor_Curve;
class GeomAPI_ProjectPointOnCurve;
class Geom_Circle;
class TopExp_Explorer;
class TopTools_IndexedMapOfShape;
class TopoDS;
class TopoDS_Compound;
class TopoDS_Edge;
class TopoDS_Iterator;
class TopoDS_Vertex;

class gp_Circ;
class gp_Dir;
class gp_Lin;
class gp_Pln;
class gp_Pnt;
class gp_Vec;
class gp_Ax2;

// -----------------------------------------------------------
#define Standard_True true
#define Handle(x) int
                        // Extrait de TopAbs_ShapeEnum.hxx 
enum TopAbs_ShapeEnum {
  TopAbs_COMPOUND, TopAbs_COMPSOLID, TopAbs_SOLID, TopAbs_SHELL,
  TopAbs_FACE, TopAbs_WIRE, TopAbs_EDGE, TopAbs_VERTEX,
  TopAbs_SHAPE
};

// -----------------------------------------------------------
class gp_Pnt
{
public :
gp_Pnt ()              {}
gp_Pnt (double, double, double)              {}
double X () { return 0;   }
double Y () { return 0;   }
double Z () { return 0;   }
};
// -----------------------------------------------------------
class gp_Vec
{
public :
gp_Vec (double, double, double)              {}
};
// -----------------------------------------------------------
class gp_Dir
{
public :
gp_Dir (int, int, int)    {}
};
// -----------------------------------------------------------
class gp_Ax2
{
public :
gp_Ax2 (gp_Pnt&, gp_Vec&, gp_Vec&)              {}
gp_Ax2 (gp_Pnt&, gp_Dir, gp_Dir)    {}
};
// -----------------------------------------------------------
class gp_Trsf
{
public :
void SetTranslation (gp_Vec) {}
void SetValues (double, double, double, double,
                double, double, double, double,
                double, double, double, double,
                double, double) {}
gp_Trsf& operator* (gp_Trsf&) { return *this ; }
};
// -----------------------------------------------------------
class TopLoc_Location
{
public :
TopLoc_Location ()         {}
TopLoc_Location (gp_Trsf&) {}
gp_Trsf  Transformation() { return tra ; }
private :
gp_Trsf  tra;
};
// -----------------------------------------------------------
class TopoDS_Shape
{
public :
   TopLoc_Location Location () { return top_loc ; }
   TopoDS_Shape    Located (TopLoc_Location& top_loc) { return *this; }
   int ShapeType () const    { return 0 ; }

   bool IsNull () const    { return true ; }
   TopoDS_Shape& operator=(const BRepBuilderAPI_MakeVertex&)
        {  return *this ; }

private :
   TopLoc_Location top_loc;
};
// -----------------------------------------------------------
class gp_Circ
{
public :
gp_Circ (gp_Ax2&, double )              {}
};
// -----------------------------------------------------------
class GCPnts_AbscissaPoint
{
public :
GCPnts_AbscissaPoint (BRepAdaptor_Curve&, double, double) {}
double Parameter ()                                       {return 1; }
static double Length (GeomAdaptor_Curve&, double&, double&)  {return 1; }
gp_Pnt Value (double)  { return point ; }

private :
gp_Pnt point;

};
// -----------------------------------------------------------
class BRepAdaptor_Curve
{
public :
BRepAdaptor_Curve (TopoDS_Edge&) {}
double FirstParameter ()                                  {return 1; }
gp_Pnt Value (double)  { return point ; }

private :
gp_Pnt point;
};
// -----------------------------------------------------------
class GeomAdaptor_Curve
{
public :
GeomAdaptor_Curve (int)   {}
int GetType ()   { return 0 ;}
};
// -----------------------------------------------------------
class BRepPrimAPI_MakeCylinder
{
public :
  BRepPrimAPI_MakeCylinder (gp_Ax2&, double, double) {}
  void         Build ()   {  }
  bool         IsDone ()  { return true; }
  TopoDS_Shape Shape ()   { return forme ; }

private :
  TopoDS_Shape forme;
};
// -----------------------------------------------------------
class BRepAlgoAPI_Section
{
public :
BRepAlgoAPI_Section (TopoDS_Shape&, TopoDS_Shape&,  bool) {}
  void         Build ()   {  }
  bool         IsDone ()  { return true; }
  void         Approximation (bool)  { }
  TopoDS_Shape Shape ()   { return forme ; }

private :
  TopoDS_Shape forme;
};
// -----------------------------------------------------------
class TopoDS_Vertex {};
// -----------------------------------------------------------
class TopoDS_Edge {};
// -----------------------------------------------------------
class TopoDS
{
public :
   static TopoDS_Edge    Edge   (const TopoDS_Shape&) { return edge ; }
   static TopoDS_Vertex  Vertex (const TopoDS_Shape&) { return vertex ; }
private :
  static TopoDS_Edge   edge;
  static TopoDS_Vertex vertex;
};
// -----------------------------------------------------------
class BRep_Tool
{
public :
   static int Curve (TopoDS_Edge, double, double) { return 0 ; }
   static int Curve (TopoDS_Edge, TopLoc_Location&, double, double) {return 0;}
   static gp_Pnt Pnt(TopoDS_Vertex vertex ) { return point; }
private :
   static gp_Pnt  point;

};
// -----------------------------------------------------------
class GeomAPI_ProjectPointOnCurve
{
public :
GeomAPI_ProjectPointOnCurve (gp_Pnt&, int) {};
int    NbPoints () {return 0 ;};
double LowerDistanceParameter () {return 0 ;};
};
// -----------------------------------------------------------
class BRepBuilderAPI_MakeEdge
{
public :
BRepBuilderAPI_MakeEdge (gp_Circ&)  {}
TopoDS_Edge  Edge () { return edge ; }
private :
  static TopoDS_Edge edge;

};
// -----------------------------------------------------------
class BRep_Builder
{
public :
   void MakeCompound (TopoDS_Compound& ) {}
   void Add (TopoDS_Compound&, TopoDS_Shape& ) {}
   void Add (TopoDS_Compound&, TopoDS_Edge& ) {}
};
// -----------------------------------------------------------
class BRepTools
{
public :
static void Write (TopoDS_Edge&, ostringstream&) {}
static void Write (TopoDS_Shape&, ostringstream&) {}
static void Read  (TopoDS_Shape&, istringstream&, BRep_Builder&) {}
static void Clean  (TopoDS_Shape&) {}
};
// -----------------------------------------------------------
class BRepPrimAPI_MakeSphere
{
public :
BRepPrimAPI_MakeSphere(gp_Ax2&, double&) {}
void         Build ()   {  }
TopoDS_Shape  Face () { return forme ; }
private :
  static TopoDS_Shape forme;

};
// -----------------------------------------------------------
class BRepBuilderAPI_Transform
{
public :
BRepBuilderAPI_Transform (TopoDS_Shape&, gp_Trsf&, bool)  {}
TopoDS_Shape  Shape () { return forme ; }

private :
  TopoDS_Shape forme;

};

// -----------------------------------------------------------
class TopExp_Explorer
{
public :
TopExp_Explorer (TopoDS_Shape&, int)    {}
bool More()                             { return false ; }
void Next()                             {}
TopoDS_Shape& Current()                 { return forme ; }
private :
  TopoDS_Shape forme;
};
// -----------------------------------------------------------
class TopoDS_Iterator
{
public :
TopoDS_Iterator (const TopoDS_Shape&)   {}
bool More()                             { return false ; }
void Next()                             {}
TopoDS_Shape& Value()                   { return forme ; }
private :
  TopoDS_Shape forme;
};
// -----------------------------------------------------------
class BRepBuilderAPI_MakeVertex
{
public :
BRepBuilderAPI_MakeVertex (gp_Pnt&) {}
TopoDS_Shape& Shape()    { return forme ; }
private :
  TopoDS_Shape forme;
};
// -----------------------------------------------------------
class TopTools_IndexedMapOfShape
{
public :
void Clear()                  {}
int Add (const TopoDS_Shape&)  { return 0 ; }
const TopoDS_Shape& FindKey (int id) { return forme ; }

private :
  TopoDS_Shape forme;
};
// -----------------------------------------------------------
class GC_MakeCircle {};
class Geom_Circle {};
class TopoDS_Compound : public TopoDS_Shape {};
class gp_Lin {};
class gp_Pln {};
#endif
