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

#include <set>


#include <BRepTools.hxx>


// VTK includes
#include <vtkRenderer.h>
#include <vtkActorCollection.h>
#include <vtkUnstructuredGrid.h>

#include <Basics_OCCTVersion.hxx>

#include <SUIT_Session.h>

#include <SalomeApp_Study.h>
#include <SalomeApp_Application.h>

#include <OCCViewer_ViewWindow.h>

#include <SVTK_ViewManager.h>
#include <SVTK_ViewModel.h>
#include <SVTK_ViewWindow.h>
#include <SVTK_Prs.h>
#include <SALOME_Actor.h>
#include <VTKViewer_Algorithm.h>

#include <SelectMgr_Selection.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <StdSelect_BRepOwner.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <Geom_Surface.hxx>
#include <ShapeAnalysis.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <Geom_Curve.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <TopoDS_Compound.hxx>

#include "HEXABLOCKGUI.hxx"
#include "HEXABLOCKGUI_SalomeTools.hxx"
#include "HEXABLOCKGUI_OccGraphicView.hxx"



//#define _DEVDEBUG_
using namespace std;

namespace HEXABLOCK
{
namespace GUI
{

	SUIT_Study* GetActiveStudy()
	{
		SUIT_Application* app = SUIT_Session::session()->activeApplication();
		if (app)
			return app->activeStudy();
		else
			return NULL;
	}


	_PTR(Study) GetActiveStudyDocument()
	{
		SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>(GetActiveStudy());
		if (aStudy)
			return aStudy->studyDS();
		else
			return _PTR(Study)();
	}


	SALOME_Actor* findActorByEntry( SVTK_ViewWindow *theVtkViewWindow, const char* theEntry)
	{
		SALOME_Actor *foundActor = NULL;
		vtkActor     *aVTKActor  = NULL;
		Handle(SALOME_InteractiveObject) anIO;

		vtkRenderer *aRenderer = theVtkViewWindow->getRenderer();
		VTK::ActorCollectionCopy aCopy(aRenderer->GetActors());
		vtkActorCollection *aCollection = aCopy.GetActors();
		aCollection->InitTraversal();
		while( aVTKActor = aCollection->GetNextActor() ){
			foundActor = dynamic_cast<SALOME_Actor*>( aVTKActor );
			if ( foundActor && foundActor->hasIO() ){
				anIO = foundActor->getIO();
				if( anIO->hasEntry() && strcmp(anIO->getEntry(), theEntry) == 0 )
					return foundActor;
			}
		}

		return NULL; // no actor found
	}


	int GetNameOfSelectedElements( SVTK_ViewWindow *theWindow,
			const Handle(SALOME_InteractiveObject)& theIO,
			QString& theName )
	{
		SVTK_Selector* theSelector = theWindow->GetSelector();

		theName = "";

		TColStd_IndexedMapOfInteger aMapIndex;
		theSelector->GetIndex(theIO,aMapIndex);

		typedef std::set<int> TIdContainer;

		std::set<int> anIdContainer;

		for( int i = 1; i <= aMapIndex.Extent(); i++)
			anIdContainer.insert(aMapIndex(i));

		std::set<int>::const_iterator anIter = anIdContainer.begin();

		for( ; anIter != anIdContainer.end(); anIter++)
			theName += QString(" %1").arg(*anIter);

		return aMapIndex.Extent();
	}

	string shape2string( const TopoDS_Shape& aShape )
	{
		ostringstream streamShape;
		BRepTools::Write(aShape, streamShape);

		return streamShape.str();
	}

	void getEntityOwners( const Handle(AIS_InteractiveObject)& theObj,
			const Handle(AIS_InteractiveContext)& theIC,
			SelectMgr_IndexedMapOfOwner& theMap )
	{
		if ( theObj.IsNull() || theIC.IsNull() )
			return;

		TColStd_ListOfInteger modes;
		theIC->ActivatedModes( theObj, modes );

		TColStd_ListIteratorOfListOfInteger itr( modes );
		for (; itr.More(); itr.Next() ) {
			int m = itr.Value();
			if ( !theObj->HasSelection( m ) )
				continue;

			Handle(SelectMgr_Selection) sel = theObj->Selection( m );

			for ( sel->Init(); sel->More(); sel->Next() ) {
#if OCC_VERSION_LARGE > 0x06080100
                                const Handle(SelectMgr_SensitiveEntity) aHSenEntity = sel->Sensitive();
                                if ( aHSenEntity.IsNull() )
                                        continue;
                                Handle(SelectBasics_SensitiveEntity) entity = aHSenEntity->BaseSensitive();
#else
				Handle(SelectBasics_SensitiveEntity) entity = sel->Sensitive();
#endif
				if ( entity.IsNull() )
					continue;

				Handle(SelectMgr_EntityOwner) owner =
						Handle(SelectMgr_EntityOwner)::DownCast(entity->OwnerId());
				if ( !owner.IsNull() )
					theMap.Add( owner );
			}
		}
	}

	void indicesToOwners( const TColStd_IndexedMapOfInteger& aIndexMap,
			const TopoDS_Shape& aMainShape,
			const SelectMgr_IndexedMapOfOwner& anAllMap,
			SelectMgr_IndexedMapOfOwner& aToHiliteMap )
	{
		TopTools_IndexedMapOfShape aMapOfShapes;
		TopExp::MapShapes(aMainShape, aMapOfShapes);

		for  ( Standard_Integer i = 1, n = anAllMap.Extent(); i <= n; i++ )
		{
			Handle(StdSelect_BRepOwner) anOwner = Handle(StdSelect_BRepOwner)::DownCast(anAllMap( i ));
			if ( anOwner.IsNull() || !anOwner->HasShape() )
				continue;

			const TopoDS_Shape& aSubShape = anOwner->Shape();
			Standard_Integer aSubShapeId = aMapOfShapes.FindIndex( aSubShape );
			if ( !aSubShapeId || !aIndexMap.Contains( aSubShapeId ) )
				continue;

			if ( !aToHiliteMap.Contains( anOwner ) )
				aToHiliteMap.Add( anOwner );
		}
	}

	TopoDS_Shape getSubShape(const TopoDS_Shape& theShape, const int theIndex)
	{
		TopoDS_Shape theSubShape;

		if (theShape.IsNull() || theIndex < 1)
			return theSubShape;

		TopTools_IndexedMapOfShape anIndices;
		TopExp::MapShapes(theShape, anIndices);
		if (theIndex > anIndices.Extent())
			return theSubShape;
		theSubShape = anIndices.FindKey(theIndex);

		return theSubShape;
	}

	int getSubId(const TopoDS_Shape& theShape, const TopoDS_Shape& theSubShape)
	{
	    if (theShape.IsNull() || theSubShape.IsNull())
	        return -1;

	    TopTools_IndexedMapOfShape anIndices;
	    TopExp::MapShapes(theShape, anIndices);

	    return anIndices.FindIndex(theSubShape);
	}

	Standard_Boolean getExtremaSolution(const gp_Pnt&       theInitPnt,
										const TopoDS_Shape& theRefShape,
										gp_Pnt& thePnt)
	{
	  BRepBuilderAPI_MakeVertex mkVertex (theInitPnt);
	  TopoDS_Vertex anInitV = TopoDS::Vertex(mkVertex.Shape());

	  BRepExtrema_DistShapeShape anExt (anInitV, theRefShape);
	  if ( !anExt.IsDone() || anExt.NbSolution() < 1 )
		return Standard_False;
	  thePnt = anExt.PointOnShape2(1);
	  Standard_Real aMinDist2 = theInitPnt.SquareDistance( thePnt );
	  for ( Standard_Integer j = 2, jn = anExt.NbSolution(); j <= jn; j++ )
	  {
		gp_Pnt aPnt = anExt.PointOnShape2(j);
		Standard_Real aDist2 = theInitPnt.SquareDistance( aPnt );
		if ( aDist2 > aMinDist2)
		  continue;
		aMinDist2 = aDist2;
		thePnt = aPnt;
	  }
	  return Standard_True;
	}

	TopoDS_Vertex makePoint(const double x, const double y, const double z)
	{
		gp_Pnt thePoint(x, y, z);
		return BRepBuilderAPI_MakeVertex(thePoint);
	}

	TopoDS_Vertex makePointWithReference(const TopoDS_Shape& point, const double dx,
															 const double dy,
															 const double dz)
	{
	    TopoDS_Vertex res;
		gp_Pnt thePoint;

		if (point.ShapeType() != TopAbs_VERTEX) {
			Standard_TypeMismatch::Raise("Aborted: referenced shape is not a vertex");
			return res;
		}
		gp_Pnt refPoint = BRep_Tool::Pnt(TopoDS::Vertex(point));
		thePoint = gp_Pnt(refPoint.X() + dx, refPoint.Y() + dy, refPoint.Z() + dz);

		return BRepBuilderAPI_MakeVertex(thePoint);
	}

	TopoDS_Vertex makePointOnCurve(const TopoDS_Shape& edge, const double param)
	{
	    TopoDS_Vertex res;
		gp_Pnt thePoint;

		if (edge.ShapeType() != TopAbs_EDGE) {
			Standard_TypeMismatch::Raise("Aborted: curve shape is not an edge");
			return res;
		}
		Standard_Real aFP, aLP, aP;
		Handle(Geom_Curve) aCurve = BRep_Tool::Curve(TopoDS::Edge(edge), aFP, aLP);
		aP = aFP + (aLP - aFP) * param;
		thePoint = aCurve->Value(aP);

		return BRepBuilderAPI_MakeVertex(thePoint);
	}

	TopoDS_Vertex makePointOnCurveByLength(const TopoDS_Shape& edge, const TopoDS_Shape& point, const double length)
	{
	    TopoDS_Vertex res;
		gp_Pnt thePoint;

		// RefCurve
		if (edge.ShapeType() != TopAbs_EDGE)
		{
			Standard_TypeMismatch::Raise("Aborted: curve shape is not an edge");
			return res;
		}
		TopoDS_Edge aRefEdge = TopoDS::Edge(edge);
		TopoDS_Vertex V1, V2;
		TopExp::Vertices(aRefEdge, V1, V2, Standard_True);

		// RefPoint
		TopoDS_Vertex aRefVertex;
		if (point.IsNull())
			aRefVertex = V1;
		else
		{
			if (point.ShapeType() != TopAbs_VERTEX)
			{
				Standard_TypeMismatch::Raise("Aborted: shape is not a vertex");
				return res;
			}
			aRefVertex = TopoDS::Vertex(point);
		}
		gp_Pnt aRefPnt = BRep_Tool::Pnt(aRefVertex);

		// Check orientation
		Standard_Real UFirst, ULast;
		Handle(Geom_Curve) EdgeCurve = BRep_Tool::Curve(aRefEdge, UFirst, ULast);
		Handle(Geom_Curve) ReOrientedCurve = EdgeCurve;

		Standard_Real dU = ULast - UFirst;
		Standard_Real par1 = UFirst + 0.1 * dU;
		Standard_Real par2 = ULast  - 0.1 * dU;

		gp_Pnt P1 = EdgeCurve->Value(par1);
		gp_Pnt P2 = EdgeCurve->Value(par2);

		if (aRefPnt.SquareDistance(P2) < aRefPnt.SquareDistance(P1)) {
			ReOrientedCurve = EdgeCurve->Reversed();
			UFirst = EdgeCurve->ReversedParameter(ULast);
		}

		// Get the point by length
		GeomAdaptor_Curve AdapCurve = GeomAdaptor_Curve(ReOrientedCurve);
		GCPnts_AbscissaPoint anAbsPnt (AdapCurve, length, UFirst);
		Standard_Real aParam = anAbsPnt.Parameter();
		thePoint = AdapCurve.Value(aParam);

		return BRepBuilderAPI_MakeVertex(thePoint);
	}

	TopoDS_Vertex makePointOnCurveByCoord(const TopoDS_Shape& edge, const double x, const double y, const double z)
	{
	    TopoDS_Vertex res;
		gp_Pnt thePoint;

		if (edge.ShapeType() != TopAbs_EDGE) {
			Standard_TypeMismatch::Raise("Aborted: curve shape is not an edge");
			return res;
		}
		gp_Pnt anInitPnt (x, y, z);
		if (!getExtremaSolution(anInitPnt, edge, thePoint)) {
			Standard_ConstructionError::Raise
			("Point On Surface creation aborted : cannot project point");
			return res;
		}

		return BRepBuilderAPI_MakeVertex(thePoint);
	}

	TopoDS_Vertex makePointOnLinesIntersection(const TopoDS_Shape& line1, const TopoDS_Shape& line2)
	{
	    TopoDS_Vertex res;
		gp_Pnt thePoint;

		if ( (line1.ShapeType() != TopAbs_EDGE && line1.ShapeType() != TopAbs_WIRE)
				|| (line2.ShapeType() != TopAbs_EDGE && line2.ShapeType() != TopAbs_WIRE) ) {
			Standard_TypeMismatch::Raise("Aborted: Line shape is not an edge or wire");
			return res;
		}

		if (line1.IsSame(line2))
		{
			Standard_ConstructionError::Raise("The lines to make intersection must be different");
			return res;
		}

		TopoDS_Compound aCompound;
		bool retCompound = false;

		//Calculate Lines Intersection Point
		BRepExtrema_DistShapeShape dst (line1, line2);
		if (dst.IsDone()) {
			gp_Pnt P1, P2;
			BRep_Builder B;
			B.MakeCompound( aCompound );
			for (int i = 1, nbSols = dst.NbSolution(); i <= nbSols; i++) {
				P1 = dst.PointOnShape1(i);
				P2 = dst.PointOnShape2(i);
				Standard_Real Dist = P1.Distance(P2);
				if ( Dist <= Precision::Confusion() && dst.NbSolution() > 1) {
					BRepBuilderAPI_MakeVertex mkVertex (P1);
					B.Add(aCompound, mkVertex.Shape());
					retCompound = true;
				} else if ( Dist <= Precision::Confusion() ) {
					thePoint = P1;
				} else {
					Standard_TypeMismatch::Raise ("Shapes have not an Intersection Point");
				}
			}
		}
		//    TopoDS_Shape aShape;
		if ( retCompound ) {
			Standard_TypeMismatch::Raise
			("Aborted: Intersection is a compound of vertices (Not supported)");
			//aShape = aCompound;
			return res;
		}/* else {
			BRepBuilderAPI_MakeVertex mkVertex (thePoint);
			aShape = mkVertex.Shape();
		}
		return aShape;*/

		return BRepBuilderAPI_MakeVertex(thePoint);
	}

	TopoDS_Vertex makePointOnSurface(const TopoDS_Shape& face, const double param_u, const double param_v)
	{
	    TopoDS_Vertex res;
		gp_Pnt thePoint;

		if (face.ShapeType() != TopAbs_FACE) {
			Standard_TypeMismatch::Raise("Aborted: surface shape is not a face");
			return res;
		}

		TopoDS_Face F = TopoDS::Face(face);
		Handle(Geom_Surface) aSurf = BRep_Tool::Surface(F);
		Standard_Real U1,U2,V1,V2;
		ShapeAnalysis::GetFaceUVBounds(F,U1,U2,V1,V2);
		Standard_Real U = U1 + (U2-U1) * param_u;
		Standard_Real V = V1 + (V2-V1) * param_v;
		thePoint = aSurf->Value(U,V);

		return BRepBuilderAPI_MakeVertex(thePoint);
	}

	TopoDS_Vertex makePointOnSurfaceByCoord(const TopoDS_Shape& face, const double x, const double y, const double z)
	{
	    TopoDS_Vertex res;
		gp_Pnt thePoint;

		if (face.ShapeType() != TopAbs_FACE) {
			Standard_TypeMismatch::Raise
			("Point On Surface creation aborted : surface shape is not a face");
			return res;
		}
		gp_Pnt anInitPnt (x, y, z);
		if (!getExtremaSolution(anInitPnt, face, thePoint)) {
			Standard_ConstructionError::Raise
			("Point On Surface creation aborted : cannot project point");
		}

		return BRepBuilderAPI_MakeVertex(thePoint);
	}

}

}
