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



#include "HEXABLOCKGUI_OccGraphicView.hxx"
#include "utilities.h"
#include "HEXABLOCKGUI.hxx"
#include "HEXABLOCKGUI_DocumentModel.hxx"
#include "GEOMBase.h"
#include "MyGEOMBase_Helper.hxx"
#include "HEXABLOCKGUI_VtkDocumentGraphicView.hxx"
#include "HexNewShape.hxx"
#include "HexVertexShape.hxx"
#include "HexEdgeShape.hxx"
#include "HexFaceShape.hxx"

#include <SOCC_ViewModel.h>
#include <SUIT_ViewManager.h>



using namespace HEXABLOCK::GUI;

// -----------------------------------------------------------------------
//          OccGraphicView implementation
// -----------------------------------------------------------------------

OccGraphicView::OccGraphicView( OCCViewer_ViewWindow* view, QWidget* parent ):
        viewWindow( view ),
        selectionMode(TopAbs_SHAPE)
{
}

void OccGraphicView::setSelectionMode(TopAbs_ShapeEnum mode)
{
    if (HEXABLOCKGUI::selectionMgr() == NULL ||
            HEXABLOCKGUI::geomBaseHelper == NULL || viewWindow == NULL) return;

    HEXABLOCKGUI::selectionMgr()->clearSelected();
    HEXABLOCKGUI::geomBaseHelper->globalSelection(viewWindow);
//    HEXABLOCKGUI::geomBaseHelper->localSelection(GEOM::GEOM_Object::_nil(), mode);
    HEXABLOCKGUI::geomBaseHelper->localSelection(viewWindow, mode);
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


void OccGraphicView::clearSelection()
{
    if (HEXABLOCKGUI::selectionMgr() == NULL || viewWindow == NULL ||
            HEXABLOCKGUI::geomBaseHelper == NULL)
        return;

    HEXABLOCKGUI::selectionMgr()->clearSelected();
    HEXABLOCKGUI::geomBaseHelper->globalSelection(viewWindow);
    HEXABLOCKGUI::geomBaseHelper->localSelection(viewWindow, TopAbs_SHAPE);
    selectionMode = TopAbs_SHAPE;
}


void OccGraphicView::highlight( const QModelIndex & index )
{

    // getting association(s) from model
    QMultiMap< QString, int >     assocEntrySubIDs;

    if (HEXABLOCKGUI::currentDocGView == NULL || viewWindow == NULL) return;
    DocumentModel*              docModel = HEXABLOCKGUI::currentDocGView->getDocumentModel();
    if (docModel == NULL) return;

    assocEntrySubIDs += docModel->getAssocShapesIds(index);
    highlight( assocEntrySubIDs );
}


void OccGraphicView::highlight( const QModelIndexList & indexList )
{

//    QModelIndex index;
    QMultiMap< QString, int >     assocEntrySubIDs;

    if (HEXABLOCKGUI::currentDocGView == NULL || viewWindow == NULL) return;
    DocumentModel*              docModel = HEXABLOCKGUI::currentDocGView->getDocumentModel();
    PatternDataModel*           pModel = HEXABLOCKGUI::currentDocGView->getPatternDataModel();
    if (docModel == NULL || pModel == NULL) return;

    foreach( const QModelIndex& anEltIndex, indexList)
    {
//        index = pModel->mapToSource(anEltIndex);
       assocEntrySubIDs += docModel->getAssocShapesIds(anEltIndex);
    }
    highlight( assocEntrySubIDs );
}

void OccGraphicView::highlight( const QMultiMap<QString, int>&  entrySubIDs )
{
      if ( viewWindow == NULL ) return;

      SOCC_Viewer* soccViewer = dynamic_cast<SOCC_Viewer*>( viewWindow->getViewManager()->getViewModel() );
      if (!soccViewer) return;

      DocumentModel* docModel = HEXABLOCKGUI::currentDocGView->getDocumentModel();
      _PTR(Study) aStudy = GetActiveStudyDocument();
      if (!aStudy || docModel == NULL) return;

      CORBA::Object_var     aCorbaObj = CORBA::Object::_nil();
      GEOM::GEOM_Object_var aGeomObj  = GEOM::GEOM_Object::_nil();
      HEXABLOCKGUI::geomBaseHelper->erasePreview(true);

      foreach ( QString shapeName, entrySubIDs.keys() ){
        QString shapeEntry = docModel->getGeomObjEntry(shapeName);
        _PTR(SObject) aSChild = aStudy->FindObjectID( shapeEntry.toStdString() );
        aCorbaObj = corbaObj( aSChild );
        aGeomObj = GEOM::GEOM_Object::_narrow( aCorbaObj );
        if ( !CORBA::is_nil(aGeomObj) ){
          QString objIOR = GEOMBase::GetIORFromObject( aGeomObj._retn() );
          Handle(GEOM_AISShape) aSh = GEOMBase::ConvertIORinGEOMAISShape( objIOR );//, true );
          if ( !aSh.IsNull() ){
            TColStd_IndexedMapOfInteger anIndexes;
            foreach ( int subid, entrySubIDs.values(shapeName) ){
              if ( subid != -1 )
                anIndexes.Add( subid );
            }
            if ( anIndexes.Extent() > 0 ){ // if it's a sub-shape
              aSh->highlightSubShapes( anIndexes, true );
    //          soccViewer->Repaint();
            } else {  // or a main shape
    //             std::cout << "aSh->getIO() => " << aSh->getIO() << std::endl;
    //           soccViewer->setColor( aSh->getIO(), QColor( Qt::red ), true );
    //           soccViewer->switchRepresentation( aSh->getIO(), 2 );
    //           globalSelection();
              soccViewer->highlight( aSh->getIO(), true, true ); // TODO: not working
            }
          }
        }
      }
      soccViewer->Repaint();

}
