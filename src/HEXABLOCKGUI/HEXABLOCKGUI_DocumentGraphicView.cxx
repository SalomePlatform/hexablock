//  Copyright (C) 2009-2011  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//#define _DEVDEBUG_

#include <sstream>

#include <iostream>

#include <math.h>
#include <QtGui>


#include <LightApp_Application.h>

#include <SUIT_ViewManager.h>
#include <SUIT_ViewWindow.h>
#include <SVTK_ViewManager.h>
#include <SVTK_ViewModel.h>
#include <SVTK_ViewWindow.h>
#include <SVTK_Prs.h>
#include <SALOME_Actor.h>
#include <VTKViewer_Algorithm.h>
#include <SalomeApp_Study.h>

// VTK includes
#include <vtkRenderer.h>
#include <vtkActorCollection.h>
#include <vtkUnstructuredGrid.h>

// test tutorial (sphere)
#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>

// test point (cf. SMESHGUI)
#include <vtkIdList.h>
#include <vtkCellArray.h>
#include <vtkUnsignedCharArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkLineSource.h>

#include <vtkLine.h>
#include <vtkQuad.h>
#include <vtkHexahedron.h>

#include "vtkLookupTable.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkPointData.h"
#include "vtkProperty.h"


// #include "vtkStructuredGridReader.h"
#include "vtkUnstructuredGridReader.h"


#include <VTKViewer_CellLocationsArray.h>





#ifndef M_PI
#define M_PI 3.1415927
#endif

#include "HEXABLOCKGUI_Trace.hxx"
#include "HEXABLOCKGUI_DocumentModel.hxx"
#include "HEXABLOCKGUI_DocumentGraphicView.hxx"



using namespace std;
using namespace HEXABLOCK::GUI;







Document_Actor::Document_Actor( HEXA_NS::Document* doc, const QString& entry ):
  SALOME_Actor(),
  _doc( doc )
{
  DEBTRACE("Document_Actor::Document_Actor " << entry.toLatin1() );
  Handle(SALOME_InteractiveObject) anIO = new SALOME_InteractiveObject( entry.toLatin1(), "HEXABLOCK" );//,theName); CS_TODO
  setIO(anIO);
  vtkUnstructuredGrid* aGrid = getUnstructuredGrid();
//   std::cout << "Document_Actor aGrid->GetNumberOfCells() =>"<< aGrid->GetNumberOfCells();
  vtkDataSetMapper* aMapper = vtkDataSetMapper::New();
  aMapper->SetInput(aGrid);
  aGrid->Delete();

  SetVisibility( true );//VisibilityOff();
  SetPickable( true ); //PickableOff();//
  SetMapper( aMapper );
  aMapper->Delete();

  vtkProperty* aProp = vtkProperty::New();
//   aProp->SetRepresentationToSurface();
  aProp->SetRepresentationToWireframe();
//   aProp->SetRepresentationToPoints();
  aProp->EdgeVisibilityOn ();
  aProp->SetPointSize(5);
  SetProperty( aProp );
  aProp->Delete();
//   SetPointRepresentation(true);

}

Document_Actor::~Document_Actor()
{

}


vtkUnstructuredGrid* Document_Actor::getUnstructuredGrid()
{
  vtkUnstructuredGrid* theGrid = vtkUnstructuredGrid::New();

  _doc->reorderFaces(); //CS_TEST

  std::map<int,vtkIdType>   vtkNodeId;
  std::map<vtkIdType, int>  hexaNodeId;

  // Create points
  vtkPoints* aPoints = vtkPoints::New();
  int nbVertex = _doc->countVertex();
  aPoints->SetNumberOfPoints( nbVertex );

  HEXA_NS::Vertex* v = NULL;
  int vertexId;
  for ( int i=0; i <nbVertex; ++i ){
    v = _doc->getVertex(i);
    aPoints->SetPoint( i, v->getX(), v->getY(), v->getZ() );
    vertexId = reinterpret_cast<intptr_t>(v); //v->getId();
    vtkNodeId [ vertexId ] = i;
    hexaNodeId[ i ] = vertexId ;
//     vtkNodeId [ vertexId ] = i+1;
//     hexaNodeId[ i+1 ] = vertexId ;
  }

  theGrid->SetPoints( aPoints );
  aPoints->Delete();
//   theGrid->SetCells( 0, 0, 0, 0, 0 );


  // Calculate cells size
  int nb0DElement = _doc->countVertex();
  int nbEdge      = _doc->countEdge();
  int nbFace      = _doc->countQuad();
  int nbVolume    = _doc->countHexa();

  vtkIdType aCellsSize =  2*nb0DElement + 3*nbEdge + ( 4 + 1 )*nbFace + ( 8 + 1 )*nbVolume;
  vtkIdType aNbCells   =  nb0DElement + nbEdge + nbFace + nbVolume;

  // Create cells
  vtkCellArray* aConnectivity = vtkCellArray::New();
  aConnectivity->Allocate( aCellsSize, 0 );

  vtkUnsignedCharArray* aCellTypesArray = vtkUnsignedCharArray::New();
  aCellTypesArray->SetNumberOfComponents( 1 );
  aCellTypesArray->Allocate( aNbCells * aCellTypesArray->GetNumberOfComponents() );

  vtkIdList *anIdList = vtkIdList::New();
  vtkIdType iVtkElem = 0;
//   vtkIdType iVtkElem = 1; //CS_TEST
  int       iHexaElem;

  // VERTEX
  for ( int i=0; i<nb0DElement; ++i ){
    anIdList->SetNumberOfIds( 1 );
    v = _doc->getVertex(i);
    iHexaElem = reinterpret_cast<intptr_t>(v);//v->getId();
    vtkElemsId[iHexaElem] = iVtkElem;
    hexaElemsId[iVtkElem] = iHexaElem;
    anIdList->SetId(0, vtkNodeId[iHexaElem]);
    aConnectivity->InsertNextCell( anIdList );
    aCellTypesArray->InsertNextValue( VTK_VERTEX );//getCellType( aType, anElem->IsPoly(), aNbNodes ) );
    ++iVtkElem;
  }

  // EDGE
  HEXA_NS::Edge* e = NULL;
  HEXA_NS::Vertex* vertexElem = NULL;
  for ( int i=0; i<nbEdge; ++i ){
    anIdList->SetNumberOfIds( 2 );
    e = _doc->getEdge(i);
    iHexaElem = reinterpret_cast<intptr_t>(e); //e->getId();
    vtkElemsId[iHexaElem] = iVtkElem;
    hexaElemsId[iVtkElem] = iHexaElem;

    for( vtkIdType j = 0; j< 2; ++j ){ //j< e->countVertex(); ++j ){
      vertexElem = e->getVertex( j );
      anIdList->SetId( j, vtkNodeId[ reinterpret_cast<intptr_t>(vertexElem) ] );//vertexElem->getId() ]);
    }
    aConnectivity->InsertNextCell( anIdList );
    aCellTypesArray->InsertNextValue( VTK_LINE );//getCellType( aType, anElem->IsPoly(), aNbNodes ) );
    ++iVtkElem;
  }

  // QUAD
  HEXA_NS::Quad* q = NULL;
  HEXA_NS::Quad* quadElem = NULL;
  for ( int i=0; i<nbFace; ++i ){
    anIdList->SetNumberOfIds( 4 );
    q = _doc->getQuad(i);
    iHexaElem = reinterpret_cast<intptr_t>(q); //q->getId();
    vtkElemsId[iHexaElem] = iVtkElem;
    hexaElemsId[iVtkElem] = iHexaElem;

    for( vtkIdType j = 0; j< 4; ++j ){
      vertexElem = q->getVertex( j );
      anIdList->SetId( j, vtkNodeId[ reinterpret_cast<intptr_t>(vertexElem) ] );//vertexElem->getId() ]);
    }
    aConnectivity->InsertNextCell( anIdList );
    aCellTypesArray->InsertNextValue( VTK_QUAD );//getCellType( aType, anElem->IsPoly(), aNbNodes ) );
    ++iVtkElem;
  }

  // HEXA
  HEXA_NS::Hexa* h = NULL;
  HEXA_NS::Hexa* hexaElem = NULL;
  std::map<int, int> connectivity;
  connectivity[0] = 0;
  connectivity[1] = 1;
  connectivity[2] = 3;
  connectivity[3] = 2;
  connectivity[4] = 4;
  connectivity[5] = 5;
  connectivity[6] = 7;
  connectivity[7] = 6;
  for ( int i=0; i<nbVolume; ++i ){
    anIdList->SetNumberOfIds( 8 );
    h = _doc->getHexa(i);
    iHexaElem = reinterpret_cast<intptr_t>(h); //q->getId();
    vtkElemsId[iHexaElem] = iVtkElem;
    hexaElemsId[iVtkElem] = iHexaElem;

    for( vtkIdType j = 0; j< 8; ++j ){
      vertexElem = h->getVertex( j );// );
      anIdList->SetId( connectivity[j], vtkNodeId[ reinterpret_cast<intptr_t>(vertexElem) ]);//vertexElem->getId() ]);
    }
    aConnectivity->InsertNextCell( anIdList );
    aCellTypesArray->InsertNextValue( VTK_HEXAHEDRON );
    ++iVtkElem;
  }


// 0        1      2     3        4     5      6      7
// V_ACE, V_ACF, V_ADE, V_ADF, V_BCE, V_BCF, V_BDE, V_BDF, 
// 
// 0        1     3      2        4    5      7      6

  // Insert cells in grid
  VTKViewer_CellLocationsArray* aCellLocationsArray = VTKViewer_CellLocationsArray::New();
  aCellLocationsArray->SetNumberOfComponents( 1 );
  aCellLocationsArray->SetNumberOfTuples( aNbCells );
//   std::cout << "aNbCells =>" << aNbCells << std::endl;

  aConnectivity->InitTraversal();
  for( vtkIdType idType = 0, *pts, npts; aConnectivity->GetNextCell( npts, pts ); idType++ ){
    aCellLocationsArray->SetValue( idType, aConnectivity->GetTraversalLocation( npts ) );
  }
  theGrid->SetCells( aCellTypesArray, aCellLocationsArray,aConnectivity );

  aCellLocationsArray->Delete();
  aCellTypesArray->Delete();
  aConnectivity->Delete();
  anIdList->Delete();
  std::cout << "theGrid->GetNumberOfCells()" << theGrid->GetNumberOfCells() << std::endl;

  return theGrid;
}




// DocumentGraphicView::DocumentGraphicView(SalomeApp_Application* app, SUIT_ViewWindow *suitView, QWidget *parent)
DocumentGraphicView::DocumentGraphicView( LightApp_Application* app, SUIT_ViewWindow *suitView, QWidget *parent )
    : QAbstractItemView(parent),
      _suitView( suitView ),
      _documentActor( 0 ),
      _currentChanged( false )
{
// _suitView->getViewPort();
// _suitView->viewport();
// _suitView->installEventFilter(this);
}

DocumentGraphicView::~DocumentGraphicView()
{
}

void DocumentGraphicView::onPatternDatachanged()
{
  std::cout << "DocumentGraphicView::onPatternDatachanged " <<std::endl;
  update();
}


void DocumentGraphicView::update()
{
  std::cout << "DocumentGraphicView::update()" << std::endl;
  SVTK_ViewWindow*    theVTKViewWindow = dynamic_cast<SVTK_ViewWindow*>(_suitView);
  PatternDataModel*   theModel         = dynamic_cast<PatternDataModel *>( model() );
  HEXA_NS::Document*  theDocumentImpl  = theModel->documentImpl();
  QString             theDocumentEntry = theModel->documentEntry();

  if ( _documentActor ){
    theVTKViewWindow->RemoveActor( _documentActor );
    _documentActor->Delete();
//     delete _documentActor;
    std::cout << "DocumentGraphicView::update() REMOVE PREVIOUS ACTOR" << std::endl;
  }

  _documentActor = new Document_Actor( theDocumentImpl, theDocumentEntry );
  // display HEXABLOCK document model
  theVTKViewWindow->AddActor( _documentActor );
  theVTKViewWindow->getRenderer()->Render();
  theVTKViewWindow->Repaint();
  theVTKViewWindow->onFitAll();
  // myVTKViewWindow->SetSelectionMode( ActorSelection );
  // theVTKViewWindow->SetSelectionMode( NodeSelection );
  // myVTKViewWindow->SetSelectionMode( FaceSelection );
}


/********************************************************************************
*                   ABSTRACT METHOD ( MUST BE IMPLEMENTED )
********************************************************************************/

/*
    Returns the item that covers the coordinate given in the view.
*/

QModelIndex DocumentGraphicView::indexAt(const QPoint &point) const
{
  std::cout << "CS_BP DocumentGraphicView::indexAt "<<std::endl;
  return QModelIndex();
}

void DocumentGraphicView::scrollTo(const QModelIndex &index, ScrollHint)
{
    std::cout << "CS_BP DocumentGraphicView::scrollTo "<<std::endl;
}

/*
    Returns the position of the item in viewport coordinates.
*/
QRect DocumentGraphicView::visualRect(const QModelIndex &index) const
{
    std::cout << "CS_BP DocumentGraphicView::visualRect "<<std::endl;
    return QRect();
}

// PROTECTED :
int DocumentGraphicView::horizontalOffset() const
{
    std::cout << "CS_BP DocumentGraphicView::horizontalOffset "<<std::endl;
    return horizontalScrollBar()->value();
}

bool DocumentGraphicView::isIndexHidden(const QModelIndex & /*index*/) const
{
    std::cout << "CS_BP DocumentGraphicView::isIndexHidden "<<std::endl;
    return false;
}

QModelIndex DocumentGraphicView::moveCursor(QAbstractItemView::CursorAction cursorAction,
                                            Qt::KeyboardModifiers /*modifiers*/)
{
    std::cout << "CS_BP DocumentGraphicView::moveCursor "<<std::endl;
    QModelIndex current = currentIndex();
    return current;
}

/*
    Find the indices corresponding to the extent of the selection.
*/
void DocumentGraphicView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    std::cout << "CS_BP DocumentGraphicView::setSelection "<<std::endl;
}


int DocumentGraphicView::verticalOffset() const
{
    std::cout << "CS_BP DocumentGraphicView::verticalOffset "<<std::endl;
    return verticalScrollBar()->value();
}

/*
    Returns a region corresponding to the selection in viewport coordinates.
*/
QRegion DocumentGraphicView::visualRegionForSelection(const QItemSelection &selection) const
{
    std::cout << "CS_BP DocumentGraphicView::visualRegionForSelection "<<std::endl;
    return QRegion();
}




/********************************************************************************
*                   PROTECTED SLOTS 
********************************************************************************/
void DocumentGraphicView::closeEditor ( QWidget * editor, QAbstractItemDelegate::EndEditHint hint )
{ 
  std::cout << "DocumentGraphicView::closeEditor" << std::endl; 
}

void DocumentGraphicView::commitData ( QWidget * editor )
{ 
  std::cout << "DocumentGraphicView::commitData" << std::endl; 
}

void DocumentGraphicView::currentChanged ( const QModelIndex & current, const QModelIndex & previous )
{ 
  std::cout << "DocumentGraphicView::currentChanged" << std::endl; 
  _currentChanged = true;
}

void DocumentGraphicView::dataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight )
{ 
  std::cout << "DocumentGraphicView::dataChanged" << std::endl;
  std::cout << "topLeft -> " << topLeft.data().toString().toStdString()<<std::endl;
  std::cout << "bottomRight ->" << bottomRight.data().toString().toStdString()<<std::endl;
  update();
//   if ( !_currentChanged )
//     update(); //CS_TEST

  _currentChanged = false;
//   updateObject(topLeft);
}

void DocumentGraphicView::editorDestroyed ( QObject * editor )
{ 
  std::cout << "DocumentGraphicView::editorDestroyed" << std::endl; 
}

void DocumentGraphicView::rowsAboutToBeRemoved ( const QModelIndex & parent, int start, int end )
{ 
  std::cout << "DocumentGraphicView::rowsAboutToBeRemoved" << std::endl; 
}

void DocumentGraphicView::rowsInserted ( const QModelIndex & parent, int start, int end )
{ 
//   std::cout << "DocumentGraphicView::rowsInserted  :  " << parent.data().toString().toStdString() << std::endl;
}

void DocumentGraphicView::selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected )
{ 
//   std::cout << "DocumentGraphicView::selectionChanged" << std::endl; 
}

void DocumentGraphicView::updateGeometries ()
{ 
//   std::cout << "DocumentGraphicView::updateGeometries " << std::endl; 
}


SUIT_ViewWindow* DocumentGraphicView::get_SUIT_ViewWindow()
{
  return _suitView;
}











// bool DocumentGraphicView::canBeDisplayed( const QString& entry, const QString& viewer_type ) const //CS_TODO
// {
//   bool result = false;
// 
//   result = (viewer_type==SVTK_Viewer::Type());
// //   QStringList es = entry.split( "_" );22
// //   bool result = ( es.count() == 3 && es[ 0 ] == "ATOMSOLVGUI" && viewer_type == SVTK_Viewer::Type() ); 
// //   //  printf ( " canBeDisplayed : entry = %s, count = %d, res = %d \n", entry.latin1(), es.count(), result );
//   std::cout << "canBeDisplayed => "<< result << std::endl;
//   return result; // entry of an atom for sure
// }




// SALOME_Prs* HEXABLOCKGUI_Displayer::buildPresentation( const QString& entry, SALOME_View* theViewFrame)
// {
//     SALOME_Prs* prs = 0;
// 
//     SALOME_View* aViewFrame = theViewFrame ? theViewFrame : GetActiveView();
// 
//     if ( aViewFrame )
//     {
//         SVTK_Viewer* vtk_viewer = dynamic_cast<SVTK_Viewer*>( aViewFrame );
//         if (vtk_viewer)
//         {
//             SUIT_ViewWindow* wnd = vtk_viewer->getViewManager()->getActiveView();
//             SALOME_Actor* anActor = myGraphicView->FindActorByEntry( wnd, entry.toLatin1().data() );
//             if (!anActor)
//             {
// //                anActor = myGraphicView->CreateActor( study()->studyDS(), entry.toLatin1().data(), true );
//                 anActor = myGraphicView->CreateActor(entry.toLatin1().data());
//             }
//             if (anActor)
//             {
//                 // Display actor :
//                 SVTK_ViewWindow* vtkWnd = dynamic_cast<SVTK_ViewWindow*> (wnd);
//                 if (vtkWnd != NULL)
//                 {
//                     vtkWnd->AddActor(anActor);
//                     vtkWnd->Repaint();
//                     prs = LightApp_Displayer::buildPresentation(entry.toLatin1().data(), aViewFrame);
//                 }
//             }
//             if (prs)
//             {
//                 UpdatePrs(prs);
//             }
//             else if (anActor)
//             {
//                 //SMESH::RemoveActor( vtk_viewer->getViewManager()->getActiveView(), anActor );
//                 std::cout << "Remove Actor" << std::endl;
//             }
//         }
//     }
// 
//     return prs;
// }



// SALOME_Prs* SMESHGUI_Displayer::buildPresentation( const QString& entry, SALOME_View* theViewFrame )
// {
//   SALOME_Prs* prs = 0;
// 
//   SALOME_View* aViewFrame = theViewFrame ? theViewFrame : GetActiveView();
// 
//   if ( aViewFrame )
//   {
//     SVTK_Viewer* vtk_viewer = dynamic_cast<SVTK_Viewer*>( aViewFrame );
//     if( vtk_viewer )
//     {
//       SUIT_ViewWindow* wnd = vtk_viewer->getViewManager()->getActiveView();
//       SMESH_Actor* anActor = SMESH::FindActorByEntry( wnd, entry.toLatin1().data() );
//       if( !anActor )
//         anActor = SMESH::CreateActor( study()->studyDS(), entry.toLatin1().data(), true );
//       if( anActor )
//       {
//         SMESH::DisplayActor( wnd, anActor );
//         prs = LightApp_Displayer::buildPresentation( entry.toLatin1().data(), aViewFrame );
//       }
//       if( prs )
//         UpdatePrs( prs );
//       else if( anActor )
//         SMESH::RemoveActor( vtk_viewer->getViewManager()->getActiveView(), anActor );
//     }
//   }
// 
//   return prs;
// }





// 
// void DocumentGraphicView::RemoveActor(SUIT_ViewWindow *theWnd, SALOME_Actor* theActor)
// {
//     std::cout << "RemoveActor() : 1" << std::endl;
//     SVTK_ViewWindow* myViewWindow = dynamic_cast<SVTK_ViewWindow*>(theWnd);
// //    SVTK_ViewWindow* myViewWindow = dynamic_cast<SVTK_ViewWindow*>(_suitView);    
//     if (myViewWindow != NULL)
//     {
//         myViewWindow->RemoveActor(theActor);
//         if(theActor->hasIO())
//         {
//             std::cout << "RemoveActor() : 2" << std::endl;            
//             Handle(SALOME_InteractiveObject) anIO = theActor->getIO();
//             if(anIO->hasEntry())
//             {
//                 std::cout << "RemoveActor() : 3" << std::endl;                
//                 std::string anEntry = anIO->getEntry();
//                 SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( myViewWindow->getViewManager()->study() );
//                 int aStudyId = aStudy->id();
// //                 TVisualObjCont::key_type aKey(aStudyId,anEntry);
// //                 VISUAL_OBJ_CONT.erase(aKey);
//             }
//         }
//         theActor->Delete();
//         myViewWindow->Repaint();
//         std::cout << "RemoveActor() : 4" << std::endl;        
//     }
// }











// bool DocumentGraphicView::eventFilter(QObject *obj, QEvent *event)
// {
//     std::cout << event->type() << std::endl;
// //     if ( event->type() == QEvent::FocusIn ){ //QEvent::KeyPress) { 
// //         return false;
// //     } else {
// //          // standard event processing
// // //          return QObject::eventFilter(obj, event);
// 
//     if ( event->type() == QEvent::Paint ) { //QEvent::FocusIn ){ 
//       std::cout << "PAINTTTTTTTTTT"<< std::endl;
// //       loadVTK( "/tmp/load.vtk"); //CS_TEST
//     }
//     return _suitView->event(event);
// //     }
// }
// 
// 
// // void DocumentGraphicView::setModel ( QAbstractItemModel * model )
// // {
// //   QAbstractItemView::setModel( model );
// //   connect( model, SIGNAL(patternDataChanged() ), this,  SLOT ( slotTEST() ) );
// // }

// void DocumentGraphicView::loadVTK( const QString&  path ) //CS_TEST
// {
//   std::cout << "DocumentGraphicView::loadVTK=>"<<std::endl;
//   QFile file(path);
//   if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//     return;
// //   QByteArray vtkData = file.readAll ();
//   QString vtkData = file.readAll ();
//   vtkData.replace(",",".");
// 
// 
//   SVTK_ViewWindow* myVTKViewWindow = dynamic_cast<SVTK_ViewWindow*>(_suitView);
// 
//   // vtkStructuredGridReader
//   vtkUnstructuredGridReader* r = vtkUnstructuredGridReader::New();
// //   r->SetFileName( path.toLocal8Bit().constData() );
//   r->SetInputString( vtkData.toLocal8Bit().constData() );
//   r->SetReadFromInputString( true );
//   r->Update();
// 
//   vtkUnstructuredGrid* myGrid = r->GetOutput();//vtkUnstructuredGrid::New();
//   std::cout << "GetNumberOfCells =>"<< myGrid->GetNumberOfCells();
//   // Create and display actor
// 
//   vtkDataSetMapper* myMapper = vtkDataSetMapper::New();
//   myMapper->SetInput(myGrid);
// 
// //   if ( myPreviewActor ){
// //     myVTKViewWindow->RemoveActor(myPreviewActor);
// //     myPreviewActor->Delete();
// //   }
// 
//   SALOME_Actor* myPreviewActor = SALOME_Actor::New();
//   myPreviewActor = SALOME_Actor::New();
//   Handle(SALOME_InteractiveObject) anIO = new SALOME_InteractiveObject(QString::number( reinterpret_cast<intptr_t>(_hexaDocument) ),"HEXABLOCK");//,theName);
//   myPreviewActor->setIO(anIO);
// 
// //   myPreviewActor->PickableOff();
//   myPreviewActor->SetVisibility( true );//VisibilityOff();
//   myPreviewActor->SetPickable( true );
//   myPreviewActor->SetMapper(myMapper);
// 
//   vtkProperty* aProp = vtkProperty::New();
// //   aProp->SetRepresentationToWireframe();
//   aProp->SetRepresentationToSurface();
//   aProp->EdgeVisibilityOn ();
// 
// //   aProp->SetColor(10, 10, 250); 
//   aProp->SetPointSize(5);
//   myPreviewActor->SetProperty(aProp);
//   aProp->Delete();
// 
//   /*vtkProperty* myBackProp = vtkProperty::New();
//   GetColor( "SMESH", "backface_color", aBackRGB[0], aBackRGB[1], aBackRGB[2], QColor( 0, 0, 255 ) );
//   myBackProp->SetColor( aBackRGB[0], aBackRGB[1], aBackRGB[2] );
//   myPreviewActor->SetBackfaceProperty( myBackProp );
//   myBackProp->Delete()*/;
//   myVTKViewWindow->AddActor(myPreviewActor);
//   myVTKViewWindow->getRenderer()->Render();
//   myVTKViewWindow->Repaint();
//   myVTKViewWindow->onFitAll();
// 
//   myVTKViewWindow->SetSelectionMode( ActorSelection );
// // myVTKViewWindow->SetSelectionMode( NodeSelection );
// // myVTKViewWindow->SetSelectionMode( EdgeSelection );
// // myVTKViewWindow->SetSelectionMode( FaceSelection );
// }

// void DocumentGraphicView::rowsInserted ( const QModelIndex & parent, int start, int end )
// { 
// 
// //   std::cout << "DocumentGraphicView::rowsInserted  :  " << parent.data().toString().toStdString() << std::endl;
//   QModelIndex newRow;
// 
//   SVTK_ViewWindow* myViewWindow = dynamic_cast<SVTK_ViewWindow*>(_suitView);
//   SUIT_ViewManager* vman = myViewWindow->getViewManager();
//   SUIT_ViewModel* vmodel = vman->getViewModel();
// 
//   for ( int i = start; i<= end; ++i ){
//     newRow = parent.child(i,0);
// //     std::cout << "newRow.data().toString() =>" << newRow.data().toString().toStdString() << std::endl; 
//     QString entry = newRow.data(HEXA_ENTRY_ROLE).toString();//.toStdString();
//     Display(entry, true, dynamic_cast<SALOME_View*>(vmodel));
//     UpdateViewer();
//         
// //     addObject(newRow);
//   }
// }