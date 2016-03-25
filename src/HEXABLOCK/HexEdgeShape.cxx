
// C++ : Gestion des soous-shapes

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

//--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8

#include "HexEdgeShape.hxx"

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Pnt.hxx>

#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>

#include <GCPnts_AbscissaPoint.hxx>

#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <Geom_Circle.hxx>

/****************
#include <Standard_Stream.hxx>
#include <GEOMImpl_IMeasureOperations.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOMImpl_MeasureDriver.hxx>
#include <GEOMImpl_IMeasure.hxx>
#include <GEOMImpl_IShapesOperations.hxx>
#include <GEOMUtils.hxx>
#include <GEOMAlgo_ShapeInfo.hxx>
#include <GEOMAlgo_ShapeInfoFiller.hxx>
*********************************/

BEGIN_NAMESPACE_HEXA

static bool db = on_debug ();  // == getenv ("HEXA_DB") > 0

// ====================================================== Constructeur
EdgeShape::EdgeShape (NewShape* dad, int id)
         : SubShape  (dad, id, 1)
{
   maj_curve  = true;
   lin_curve  = NULL;
   lin_length = 0;
   lin_start [dir_x] = lin_start [dir_y] = lin_start [dir_z] = 0;
   lin_end   [dir_x] = lin_end   [dir_y] = lin_end   [dir_z] = 0;
   par_mini   = 0;
   par_maxi   = 0;
   lin_radius = 0;
   lin_angle  = 0;
}
// ====================================================== getCurve
BRepAdaptor_Curve* EdgeShape::getCurve ()
{
   if (maj_curve)
      updateCurve ();

   return lin_curve;
}
// ====================================================== makeCurve
BRepAdaptor_Curve* EdgeShape::makeCurve ()
{
   if (maj_shape)
      updateShape ();

   TopoDS_Edge     geo_line = TopoDS::Edge (geo_shape);
   BRepAdaptor_Curve* curve = new BRepAdaptor_Curve (geo_line);
   return curve;
}
// ====================================================== getCoords
void EdgeShape::getCoords (double* pstart, double* pend)
{
   if (maj_curve)
      updateCurve ();

   for (int nc=0 ; nc<DIM3 ; nc++)
       {
       pstart [nc] = lin_start [nc];
       pend   [nc] = lin_end   [nc];
       }
}
// ====================================================== getLength
double EdgeShape::getLength ()
{
   if (maj_curve)
      updateCurve ();

   return lin_length;
}
// ====================================================== getPoint
int EdgeShape::getPoint (double param, double* point)
{
   if (param < -0.01 || param > 1.01)
      {
      point [dir_x] = point [dir_y] = point [dir_z] = 0;
      return HERR;
      }
   else if (param < 0.01)
      param = 0;
   else if (param > 0.99)
      param = 1;

   if (maj_curve)
       updateCurve ();

   GCPnts_AbscissaPoint s1 (*lin_curve, param*lin_length,
                             lin_curve->FirstParameter());

   double para1  = s1.Parameter ();
   gp_Pnt gpnt   = lin_curve->Value (para1);

   point [dir_x] = gpnt.X();
   point [dir_y] = gpnt.Y();
   point [dir_z] = gpnt.Z();
   return HOK;
}
// ========================================================= samePoints
bool EdgeShape::samePoints (double* point1, double* point2)
{
   const double Epsilon2 = 1e-4;
   bool   rep = same_coords (point1, point2, Epsilon2);
   return rep;
}
// ========================================================= onExtremity
int EdgeShape::onExtremity (double* point)
{
   if (maj_curve)
      updateCurve ();

   if (samePoints  (point, lin_start))
      return V_AMONT;
   else if (samePoints  (point, lin_end))
      return V_AVAL;
   else
      return NOTHING;
}
// ========================================================= definedBy
bool EdgeShape::definedBy (double p1[], double p2[])
{
   if (maj_curve)
      updateCurve ();

   bool rep = false;
   if (samePoints  (p1, lin_start))
      rep = samePoints  (p2, lin_end);
   else if (samePoints  (p1, lin_end))
      rep = samePoints  (p2, lin_start);
   return rep;
}
// ========================================================= getParam
double EdgeShape::getParam (double* coord)
{
   if (maj_curve)
      updateCurve ();

   if (samePoints  (coord, lin_start))
      return 0.0;
   else if (samePoints  (coord, lin_end))
      return 1.0;

   double umin = 0, umax = 0;
   gp_Pnt gpoint (coord[dir_x], coord[dir_y], coord[dir_z]);

   TopoDS_Edge        geo_line = TopoDS::Edge (geo_shape);
   Handle(Geom_Curve) handle   = BRep_Tool::Curve (geo_line, umin, umax);

   GeomAPI_ProjectPointOnCurve projector (gpoint, handle);
   if ( projector.NbPoints() == 0 )
      return -1.0;

   double gparam = projector.LowerDistanceParameter();
   if (gparam <par_mini || gparam>par_maxi)
      return -1.0;

   gp_Pnt rpoint = lin_curve->Value (gparam);
   Real3  rcoord = { rpoint.X(), rpoint.Y(), rpoint.Z() };

   if (NOT samePoints (coord, rcoord))
      return -1.0;

   GeomAdaptor_Curve  adapt_curve (handle);

/******************
   enum GeomAbs_CurveType { GeomAbs_Line, GeomAbs_Circle, GeomAbs_Ellipse,
                            GeomAbs_Hyperbola, GeomAbs_Parabola, GeomAbs_BezierCurve,
                            GeomAbs_BSplineCurve, GeomAbs_OtherCurve };
 **********************/

   double abscis = GCPnts_AbscissaPoint::Length (adapt_curve, umin, gparam);
   double hparam = abscis/lin_length;

   // gparam = (gparam-par_mini) / (par_maxi-par_mini);
   return hparam;
}
// ====================================================== getAssociation
Edge* EdgeShape::getAssociation (int nro)
{
   if (nro>0 && nro<(int)tab_assoc.size())
      return tab_assoc[nro];
   else
      return NULL;
}
// ==================================================== addAssociation
void EdgeShape::addAssociation (Edge* edge)
{
   tab_assoc.push_back (edge);
   is_associated = true;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 // ------- Cf GEOMImpl_IMeasureOperations.cxx

// ====================================================== updateCurve
void EdgeShape::updateCurve ()
{
   delete lin_curve;
   maj_curve = false;

   if (maj_shape)
      updateShape ();

   TopoDS_Edge geo_line = TopoDS::Edge (geo_shape);
   lin_curve            = new BRepAdaptor_Curve (geo_line);
                               // Longueur de la ligne
   double umin = 0, umax = 0;
   TopLoc_Location    loc;
   Handle(Geom_Curve) handle = BRep_Tool::Curve (geo_line, loc, umin, umax);
   GeomAdaptor_Curve  adapt_curve (handle);
   lin_length = GCPnts_AbscissaPoint::Length(adapt_curve, umin, umax);

                               // Extremites (1)
   GCPnts_AbscissaPoint s1(*lin_curve, 0,          lin_curve->FirstParameter());
   GCPnts_AbscissaPoint s2(*lin_curve, lin_length, lin_curve->FirstParameter());

   par_mini = s1.Parameter ();
   par_maxi = s2.Parameter ();
                               // Extremites
   getPoint (0, lin_start);
   getPoint (1, lin_end);

   lin_radius = lin_angle = 0;
   kind_of    = (EnumKindOfShape) adapt_curve.GetType();
#ifndef NO_CASCADE
   if (kind_of==KS_Circle)
      {
      Handle(Geom_Circle) hgc = Handle(Geom_Circle)::DownCast (handle);
      lin_radius = hgc->Radius ();
      lin_angle  = (par_maxi-par_mini)*180/M_PI;
      }
#endif
}
// ====================================================== getAngle
double EdgeShape::getAngle ()
{
   if (maj_curve)
      updateCurve ();

   return lin_angle;
}
// ====================================================== getRadius
double EdgeShape::getRadius ()
{
   if (maj_curve)
      updateCurve ();

   return lin_radius;
}
// ====================================================== getRadius
EnumKindOfShape EdgeShape::kindOf ()
{
   if (maj_curve)
      updateCurve ();

   return kind_of;
}
END_NAMESPACE_HEXA
