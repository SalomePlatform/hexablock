//
// CC++ : interface modele/cascade
//

// #include <sstream>
// #include <algorithm>

// CasCade includes
#include <AIS_Shape.hxx>

#include <Precision.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <TopoDS_Wire.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <IntCurvesFace_ShapeIntersector.hxx>

// SMESH includes
//  #include "SMDS_MeshNode.hxx"
//  #include "SMDS_MeshVolume.hxx"
//  #include "SMESH_Gen.hxx"
//  #include "SMESH_MesherHelper.hxx"
//  #include "SMESHDS_Group.hxx"

// HEXABLOCK includes
#include "HexDocument.hxx"
#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexQuad.hxx"
#include "HexHexa.hxx"
#include "HexPropagation.hxx"
#include "HexShape.hxx"
#include "HexGroups.hxx"

//  #include "Basics_Utils.hxx"
//  #include "utilities.h"

#ifdef WNT
#include <process.h>
#else
#include <unistd.h>
#endif

#include <stdexcept>

#ifndef EXCEPTION
#define EXCEPTION(TYPE, MSG) {\
  std::ostringstream aStream;\
  aStream<<__FILE__<<"["<<__LINE__<<"]::"<<MSG;\
  throw TYPE(aStream.str());\
}
#endif

#ifdef _DEBUG_
static int MYDEBUG = 0;
#else
static int MYDEBUG = 0;
#endif

static double HEXA_EPSILON  = 1E-6; //1E-3; 
static double HEXA_QUAD_WAY = PI/4.; //3.*PI/8.;


BEGIN_NAMESPACE_HEXA

// =================================================================== asso2edge
TopoDS_Edge asso2edge (Shape* asso)
{
   string brep = asso->getBrep();

   istringstream streamBrep(brep);
   BRep_Builder  aBuilder;
   TopoDS_Shape  shape;

   BRepTools::Read(shape, streamBrep, aBuilder);
   return TopoDS::Edge(shape);
}

// =============================================================== edge_length
double edge_length (const TopoDS_Edge& edge )
{
  if (BRep_Tool::Degenerated(edge))
    return 0;

  double umin = 0, umax = 0;
  TopLoc_Location    loc;
  Handle(Geom_Curve) curv = BRep_Tool::Curve (edge, loc, umin, umax);
  GeomAdaptor_Curve AdaptCurve(curv);

  double lg = GCPnts_AbscissaPoint::Length(AdaptCurve, umin, umax);
  return lg;
}

// ====================================================== associateCascade
int Document::associateCascade (Edges& mline, int msens[], Shape* gstart, 
                                Shapes& gline, double pstart, double pend, 
                                bool closed)
{
   int nbedges  = mline.size ();
   int nbshapes = gline.size ();

   HexDisplay (nbedges);
   HexDisplay (nbshapes);

   TopoDS_Edge c_start  = asso2edge (gstart);

   for (int ns=0 ; ns<nbshapes ; ns++)
       {
       // double px1, py1, pz1,  px2, py2, pz2;
       TopoDS_Edge c_edge  = asso2edge  (gline [ns]);
       double      lg      = edge_length(c_edge);

       HexDisplay (ns);
       HexDisplay (c_edge.ShapeType());
       HexDisplay (lg);
       }

   return HOK;
}
END_NAMESPACE_HEXA
