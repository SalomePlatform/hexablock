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



#include "HEXABLOCKGUI_OccGraphicView.hxx"
#include "HEXABLOCKGUI.hxx"
#include "HEXABLOCKGUI_VtkDocumentGraphicView.hxx"

#include <SOCC_ViewModel.h>
#include <SUIT_ViewManager.h>



using namespace HEXABLOCK::GUI;

// -----------------------------------------------------------------------
//          OccGraphicView implementation
// -----------------------------------------------------------------------

OccGraphicView::OccGraphicView( OCCViewer_ViewWindow* view, QWidget* parent ):
        prs(NULL),
        viewWindow( view ),
        selectionMode(TopAbs_SHAPE)
{
    TopoDS_Shape shapeToPreview;
    previewShape = new AIS_Shape(shapeToPreview);
//    previewShape->SetColor(Quantity_NOC_RED);
    getContext()->SetColor( previewShape, Quantity_NOC_RED );
}

void OccGraphicView::setSelectionMode(TopAbs_ShapeEnum mode)
{
    if (viewWindow == NULL || selectionMode == mode)
        return;

    HEXABLOCKGUI::selectionMgr()->clearSelected();
    globalSelection();
    localSelection(mode);
    selectionMode = mode;
}

void OccGraphicView::setSelectionMode(const QModelIndex& eltIndex)
{

    QVariant treeVariant = eltIndex.data( HEXA_TREE_ROLE );
    if ( !treeVariant.isValid() ) return;
    int eltType = treeVariant.toInt();

    switch ( eltType ){
      case VERTEX_TREE :          setSelectionMode(TopAbs_VERTEX); break;
      case VERTEX_DIR_TREE :      setSelectionMode(TopAbs_VERTEX); break;
      case EDGE_TREE :            setSelectionMode(TopAbs_EDGE); break;
      case EDGE_DIR_TREE :        setSelectionMode(TopAbs_EDGE); break;
      case QUAD_TREE :            setSelectionMode(TopAbs_FACE); break;
      case QUAD_DIR_TREE :        setSelectionMode(TopAbs_FACE); break;
      case HEXA_TREE :            setSelectionMode(TopAbs_SHAPE); break;
      case HEXA_DIR_TREE :        setSelectionMode(TopAbs_SHAPE); break;
      case GEOMSHAPE_TREE:        setSelectionMode(TopAbs_SHAPE); break;
      case GEOMSHAPE_DIR_TREE:    setSelectionMode(TopAbs_SHAPE); break;
      case GEOMPOINT_TREE:        setSelectionMode(TopAbs_VERTEX); break;
      case GEOMPOINT_DIR_TREE:    setSelectionMode(TopAbs_VERTEX); break;
      case GEOMEDGE_TREE:         setSelectionMode(TopAbs_EDGE); break;
      case GEOMEDGE_DIR_TREE:     setSelectionMode(TopAbs_EDGE); break;
      case GEOMFACE_TREE:         setSelectionMode(TopAbs_FACE); break;
      case GEOMFACE_DIR_TREE:     setSelectionMode(TopAbs_FACE); break;
      case PROPAGATION_TREE :     setSelectionMode(TopAbs_EDGE); break;
      case PROPAGATION_DIR_TREE : setSelectionMode(TopAbs_EDGE); break;
    }
}

void OccGraphicView::getSelected(SALOME_ListIO& selectedObjects)
{
    HEXABLOCKGUI::selectionMgr()->selectedObjects( selectedObjects/*, OCCViewer_Viewer::Type()*/ );

//    SALOME_View* vf = dynamic_cast<SALOME_View*>(HEXABLOCKGUI::currentOccGView->getViewWindow()->getViewManager()->getViewModel());
//    if ( vf == NULL ) return;
//    SOCC_Viewer* viewer = dynamic_cast<SOCC_Viewer*>( vf );
//    if (viewer == NULL)
//        return;
//    AIS_ListOfInteractive theList;
//    viewer->getSelectedObjects (theList);
//    AIS_ListIteratorOfListOfInteractive it (theList);
//    //iterate on list:
//    while (it.More ())
//    {
//        Handle(SALOME_InteractiveObject) anIO = Handle(SALOME_InteractiveObject)::DownCast(it.Value()->GetOwner());
//        if (!anIO.IsNull())
//            salomeSelected.Append(anIO);
//        it.Next ();
//    }
}

void OccGraphicView::clearSelection()
{
    if (HEXABLOCKGUI::selectionMgr() == NULL)
        return;

    HEXABLOCKGUI::selectionMgr()->clearSelected();
    globalSelection();
    localSelection(TopAbs_SHAPE);
    selectionMode = TopAbs_SHAPE;
}

void OccGraphicView::localSelection(const int theMode )
{
    if (viewWindow == NULL || prs == NULL)
        return;

    // remove all filters from selection
    LightApp_SelectionMgr* sm = HEXABLOCKGUI::selectionMgr();
    sm->clearFilters();

    SALOME_View* vf = dynamic_cast<SALOME_View*>(viewWindow->getViewManager()->getViewModel());
    if (vf == NULL)
        return;

    vf->LocalSelection( prs, theMode );
}

void OccGraphicView::globalSelection(const bool update )
{
    if (viewWindow == NULL)
        return;

    SALOME_View* vf = dynamic_cast<SALOME_View*>(viewWindow->getViewManager()->getViewModel());
    if ( vf == NULL ) return;

    // Close local context
    vf->GlobalSelection( update );

    // Set selection filters in accordance with current mode
    LightApp_SelectionMgr* sm = HEXABLOCKGUI::selectionMgr();
    if ( sm == NULL)
        return;

    sm->clearFilters();

    // Remove filters from AIS_InteractiveContext
    Handle(AIS_InteractiveContext) ic;
    SOCC_Viewer* viewer = dynamic_cast<SOCC_Viewer*>( vf );
    if ( viewer )
    {
        ic = viewer->getAISContext();
        if ( !ic.IsNull() )
            ic->RemoveFilters();
    }
}

void OccGraphicView::highlight( const QModelIndex & index, bool only )
{

    // getting association(s) from model
    QMultiMap< QString, int >     assocEntrySubIDs;

    if (HEXABLOCKGUI::currentDocGView == NULL || viewWindow == NULL) return;
    DocumentModel*              docModel = HEXABLOCKGUI::currentDocGView->getDocumentModel();
    if (docModel == NULL) return;

    assocEntrySubIDs += docModel->getAssocShapesIds(index);
    highlight( assocEntrySubIDs, only );
}


void OccGraphicView::highlight( const QModelIndexList & indexList, bool only )
{

    QMultiMap< QString, int >     assocEntrySubIDs;

    if (HEXABLOCKGUI::currentDocGView == NULL || viewWindow == NULL) return;
    DocumentModel*              docModel = HEXABLOCKGUI::currentDocGView->getDocumentModel();
    PatternDataModel*           pModel = HEXABLOCKGUI::currentDocGView->getPatternDataModel();
    if (docModel == NULL || pModel == NULL) return;

    foreach( const QModelIndex& anEltIndex, indexList)
    {
       assocEntrySubIDs += docModel->getAssocShapesIds(anEltIndex);
    }
    highlight( assocEntrySubIDs, only );
}

void OccGraphicView::highlightSubShapes(const Handle(AIS_Shape)& anObj,
                                const TColStd_IndexedMapOfInteger& aIndexMap, bool only)
{
    if (viewWindow == NULL || anObj.IsNull() )
        return;

    //get the context
    Handle (AIS_InteractiveContext) anIC = getContext();
    if ( anIC.IsNull() || !anIC->HasOpenedContext() )
        return;

    // ** highlight
    Standard_Boolean isAutoHilight = anIC->AutomaticHilight();
    anIC->SetAutomaticHilight( false );

    if (only) // * clear current highlighted if asked
        anIC->ClearSelected( false );

    SelectMgr_IndexedMapOfOwner anAllMap, aToHiliteMap;

    // Get entity owners for all activated selection modes
    getEntityOwners( anObj, anIC, anAllMap );

    // Convert <aIndexMap> into the map of owners to highlight/unhighlight
    indicesToOwners( aIndexMap, anObj->Shape(), anAllMap, aToHiliteMap );

    for ( Standard_Integer i = 1, n = aToHiliteMap.Extent(); i <= n; i++ )
        anIC->AddOrRemoveSelected( aToHiliteMap( i ), false );

    anIC->SetAutomaticHilight( isAutoHilight );
    anIC->HilightSelected( false );
}

void OccGraphicView::highlight( const QMultiMap<QString, int>& entrySubIDs, bool only)
{

    if (HEXABLOCKGUI::currentDocGView == NULL || viewWindow == NULL)
        return;

    SOCC_Viewer* soccViewer = dynamic_cast<SOCC_Viewer*>( viewWindow->getViewManager()->getViewModel() );
    DocumentModel* docModel = HEXABLOCKGUI::currentDocGView->getDocumentModel();
    if (soccViewer == NULL || docModel == NULL)
        return;

    foreach ( QString shapeName, entrySubIDs.uniqueKeys() )
    {
        QString shapeEntry = docModel->getGeomObjEntry(shapeName);
        Handle(AIS_Shape) anObj = getShape(shapeEntry);
        if ( anObj.IsNull() ) continue;
        TColStd_IndexedMapOfInteger anIndexes;
        QSet<int> subIds = entrySubIDs.values(shapeName).toSet();
        foreach ( int subId, subIds ){
            if ( subId != -1 )
                anIndexes.Add( subId );
        }
        if ( anIndexes.Extent() == 0 ) continue;
        highlightSubShapes( anObj, anIndexes, only );
    }
    soccViewer->Repaint();
}

void OccGraphicView::addShape(QString& entry, const Handle(AIS_Shape)& ais_shape)
{
    if (shapes.contains(entry) || entry.isEmpty())
        return;

    shapes[entry] = ais_shape;
}

Handle(AIS_Shape) OccGraphicView::getShape(const QString& entry)
{
    Handle(AIS_Shape) aSh;

    if (!shapes.contains(entry))
        return aSh;

    return shapes[entry];
}

Handle(AIS_InteractiveContext) OccGraphicView::getContext()
{
    Handle (AIS_InteractiveContext) anIC;
    if (viewWindow == NULL)
        return anIC;

    SUIT_ViewManager*  vm = viewWindow->getViewManager();
    OCCViewer_Viewer* viewer = dynamic_cast<OCCViewer_Viewer*>(vm->getViewModel());
    Handle (V3d_Viewer) v3d_viewer = viewer->getViewer3d();
    anIC = viewer->getAISContext();

    return anIC;
}

void OccGraphicView::addVertexToCloud(TopoDS_Vertex& vertex)
{
    // * add the vertex to the cloud
    DocumentModel* docModel = HEXABLOCKGUI::currentDocGView->getDocumentModel();
    QString shapeName = "cloud";
    Handle(AIS_Shape) ais_shape = shapes[docModel->getGeomObjEntry(shapeName)];
    TopoDS_Shape sh = ais_shape->Shape();
    BRep_Builder compoundBuilder;
    compoundBuilder.Add(sh, vertex);

    // * redisplay the cloud
    ais_shape->Redisplay();
    if (viewWindow == NULL)
        return;
    SALOME_View* vf = dynamic_cast<SALOME_View*>(viewWindow->getViewManager()->getViewModel());
    if (vf == NULL)
        return;
    vf->Repaint();
}

void OccGraphicView::displayPreview(TopoDS_Shape& shape)
{
    if (viewWindow == NULL)
        return;
    SALOME_View* vf = dynamic_cast<SALOME_View*>(viewWindow->getViewManager()->getViewModel());
    if (vf == NULL)
        return;

    previewShape->Set(shape);
//    previewShape->SetColor(Quantity_NOC_RED);
    getContext()->SetColor( previewShape, Quantity_NOC_RED );
    previewShape->Redisplay();
    vf->Repaint();
}


Handle(AIS_Shape) OccGraphicView::getPreviewShape()
{
    TopoDS_Shape shapeToPreview;
    previewShape->Set(shapeToPreview);

    return previewShape;
}
