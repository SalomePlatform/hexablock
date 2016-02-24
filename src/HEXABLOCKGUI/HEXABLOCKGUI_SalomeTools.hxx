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

#ifndef _HEXABLOCKGUI_SALOMETOOLS_HXX_
#define _HEXABLOCKGUI_SALOMETOOLS_HXX_

#include "HEXABLOCKGUI_Export.hxx"

#include <SalomeApp_Application.h>
#include <SALOME_Actor.h>
#include <SALOME_ListIO.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>
#include <SelectMgr_IndexedMapOfOwner.hxx>
#include <SALOME_Actor.h>
#include <SVTK_ViewWindow.h>

#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

namespace HEXABLOCK
{
  namespace GUI
  {

HEXABLOCK_EXPORT  SALOME_Actor* findActorByEntry( SVTK_ViewWindow *theVtkViewWindow, const char* theEntry );
  _PTR(Study)   GetActiveStudyDocument();

HEXABLOCK_EXPORT int GetNameOfSelectedElements( SVTK_ViewWindow *theWindow,/* SVTK_Selector* theSelector,*/
                                 const Handle(SALOME_InteractiveObject)& theIO,
                                 QString& theName );

HEXABLOCK_EXPORT std::string shape2string( const TopoDS_Shape& aShape );

HEXABLOCK_EXPORT void getEntityOwners( const Handle(AIS_InteractiveObject)& theObj,
                               const Handle(AIS_InteractiveContext)& theIC,
                               SelectMgr_IndexedMapOfOwner& theMap );

HEXABLOCK_EXPORT void indicesToOwners( const TColStd_IndexedMapOfInteger& aIndexMap,
                               const TopoDS_Shape& aMainShape,
                               const SelectMgr_IndexedMapOfOwner& anAllMap,
                               SelectMgr_IndexedMapOfOwner& aToHiliteMap );

HEXABLOCK_EXPORT TopoDS_Shape getSubShape(const TopoDS_Shape& theShape, const int theIndex);
HEXABLOCK_EXPORT int getSubId(const TopoDS_Shape& theShape, const TopoDS_Shape& theSubShape);

HEXABLOCK_EXPORT Standard_Boolean getExtremaSolution(const gp_Pnt& theInitPnt,
  									  const TopoDS_Shape& theRefShape,
  									  gp_Pnt& thePnt);

HEXABLOCK_EXPORT TopoDS_Vertex makePoint(const double x, const double y, const double z);

HEXABLOCK_EXPORT TopoDS_Vertex makePointWithReference(const TopoDS_Shape& point, const double dx,
                                                           const double dy,
                                                           const double dz);

HEXABLOCK_EXPORT TopoDS_Vertex makePointOnCurve(const TopoDS_Shape& edge, const double param);

HEXABLOCK_EXPORT TopoDS_Vertex makePointOnCurveByLength(const TopoDS_Shape& edge, const TopoDS_Shape& point, const double length);

HEXABLOCK_EXPORT TopoDS_Vertex makePointOnCurveByCoord(const TopoDS_Shape& edge, const double x, const double y, const double z);

  TopoDS_Vertex makePointOnLinesIntersection(const TopoDS_Shape& line1, const TopoDS_Shape& line2);

HEXABLOCK_EXPORT TopoDS_Vertex makePointOnSurface(const TopoDS_Shape& face, const double param_u, const double param_v);

HEXABLOCK_EXPORT TopoDS_Vertex makePointOnSurfaceByCoord(const TopoDS_Shape& face, const double x, const double y, const double z);

  }
}

#endif
