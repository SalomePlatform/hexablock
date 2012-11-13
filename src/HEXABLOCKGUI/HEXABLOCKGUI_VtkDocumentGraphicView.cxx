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

//#define _DEVDEBUG_

#include <sstream>

#include <iostream>

#include <math.h>

#include "utilities.h"

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
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <SVTK_View.h>
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
#include "HEXABLOCKGUI_VtkDocumentGraphicView.hxx"
#include "HEXABLOCKGUI.hxx"



using namespace std;
using namespace HEXABLOCK::GUI;
using namespace HEXA_NS;

Document_Actor::Document_Actor( Document* doc, const QString& entry ):
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

    map <int,vtkIdType>   vtkNodeId;
    map <vtkIdType, int>  hexaNodeId;

    // Create points
    vtkPoints* aPoints = vtkPoints::New();
    int nbVertex = _doc->countVertex();
    aPoints->SetNumberOfPoints( nbVertex );

    Vertex* v = NULL;
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
    Edge* e = NULL;
    Vertex* vertexElem = NULL;
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
    Quad* q = NULL;
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
    Hexa* h = NULL;
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
    //std::cout << "theGrid->GetNumberOfCells()" << theGrid->GetNumberOfCells() << std::endl;

    return theGrid;
}

// =============================================================== Abu : debut
// ===================================================== Constructeur
Associate_Actor::Associate_Actor( Document* doc, const QString& entry)
: SALOME_Actor(), _doc( doc )
{
    DEBTRACE("Associate_Actor::Associate_Actor " << entry.toLatin1() );
    Handle(SALOME_InteractiveObject) anIO = new SALOME_InteractiveObject( entry.toLatin1(), "HEXABLOCK" );//,theName); CS_TODO
    setIO(anIO);
    vtkUnstructuredGrid* aGrid = getUnstructuredGrid();

    vtkDataSetMapper* aMapper = vtkDataSetMapper::New();
    aMapper->SetInput(aGrid);
    aGrid->Delete();

    SetVisibility( true );//VisibilityOff();
    SetPickable( true ); //PickableOff();//
    SetMapper( aMapper );
    aMapper->Delete();

    vtkProperty* aProp = vtkProperty::New();
    aProp->SetColor(0,255,0);
    //   aProp->SetRepresentationToSurface();
    aProp->SetRepresentationToWireframe();
    //   aProp->SetRepresentationToPoints();
    aProp->EdgeVisibilityOn ();
    aProp->SetPointSize(5);
    SetProperty( aProp );
    aProp->Delete();
    //   SetPointRepresentation(true);
}
// ===================================================== getUnstructuredGrid

vtkUnstructuredGrid* Associate_Actor::getUnstructuredGrid()
{
    vtkUnstructuredGrid* theGrid = vtkUnstructuredGrid::New();

    _doc->reorderFaces(); //CS_TEST

    std::map<int,vtkIdType>   vtkNodeId;
    std::map<vtkIdType, int>  hexaNodeId;

    // Create points
    vtkPoints* aPoints = vtkPoints::New();
    int nbVertex = _doc->countVertex();
    aPoints->SetNumberOfPoints( nbVertex );

    Vertex* v = NULL;
    int vertexId;
    for ( int i=0; i <nbVertex; ++i ){
        v = _doc->getVertex(i);
        aPoints->SetPoint( i, v->getX()+6, v->getY()+6, v->getZ() );
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
    Edge* e = NULL;
    Vertex* vertexElem = NULL;
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
    Quad* q = NULL;
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
    Hexa* h = NULL;
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

    return theGrid;
}


// -----------------------------------------------------------------------
//          VtkDocumentGraphicView implementation
// -----------------------------------------------------------------------
VtkDocumentGraphicView::VtkDocumentGraphicView( DocumentModel* documentModel,
        SVTK_ViewWindow* view, QWidget* parent ):
        QAbstractItemView(parent),
        viewWindow( view ),
        documentActor( NULL ),
        associateActor (NULL), // Abu
        _currentChanged( false ),
        firstUpdate(true),
        selectionMode(-1)
{
    MESSAGE("VtkDocumentGraphicView::VtkDocumentGraphicView());");

    //Model
    setModel(documentModel);
    patternDataModel     = new PatternDataModel(parent);
    patternBuilderModel  = new PatternBuilderModel(parent);
    groupsModel          = new GroupsModel(parent);
    meshModel            = new MeshModel(parent);
    patternDataModel->setSourceModel(documentModel);
    patternBuilderModel->setSourceModel(documentModel);
    groupsModel->setSourceModel(documentModel);
    meshModel->setSourceModel(documentModel);

    //Selection Model
    patternDataSelectionModel    = new PatternDataSelectionModel(patternDataModel);
    patternBuilderSelectionModel = new PatternBuilderSelectionModel(patternBuilderModel, patternDataSelectionModel);
    groupsSelectionModel         = new GroupsSelectionModel(groupsModel);
    meshSelectionModel           = new MeshSelectionModel(meshModel );
    setSelectionModel(patternDataSelectionModel);
}

//Loads the graphic view document
void VtkDocumentGraphicView::loadDocument()
{
    DocumentModel* docModel = getDocumentModel();
    if (docModel == NULL) return;

    docModel->load();
}

//Loads the document from the file in this graphic view
void VtkDocumentGraphicView::loadDocument(const QString& file)
{
    if ( file.isEmpty() ) return;
    getDocumentModel()->load(file);
}

//Saves the graphic view's document in the file
void VtkDocumentGraphicView::saveDocument(const QString& file)
{
    if (file.isEmpty()) return;
    getDocumentModel()->save(file);
}


VtkDocumentGraphicView::~VtkDocumentGraphicView()
{
    MESSAGE("VtkDocumentGraphicView::~VtkDocumentGraphicView()");

    //Delete Model
    delete patternDataModel;
    delete patternBuilderModel;
    delete groupsModel;
    delete meshModel;

    //Delete Selection Model/ Disconnect signals on delete (Qt)
//    delete patternDataSelectionModel;
    delete patternBuilderSelectionModel;
//    delete groupsSelectionModel;
//    delete meshSelectionModel;
}

void VtkDocumentGraphicView::setWindowTitle(const QString& title)
{
    viewWindow->setWindowTitle( QString("hexablock : ") + title );
}

void VtkDocumentGraphicView::onPatternDatachanged()
{
    MESSAGE("VtkDocumentGraphicView::onPatternDatachanged(){");
    update();
    MESSAGE("}");
}

void VtkDocumentGraphicView::removeActor()
{
    MESSAGE("VtkDocumentGraphicView::removeActor(){");
    if ( documentActor != NULL && viewWindow != NULL){
          viewWindow->RemoveActor( documentActor );
          documentActor->Delete();
    }
    MESSAGE("}")
}

void VtkDocumentGraphicView::update()
{
    MESSAGE("VtkDocumentGraphicView::update(){");

    DocumentModel*   theModel = dynamic_cast<DocumentModel*>( model() );
    MESSAGE("model()"<<model());
    MESSAGE("theModel"<<theModel);
    if (theModel == NULL || viewWindow == NULL) return;

    Document*  theDocumentImpl  = theModel->documentImpl();
    QString             theDocumentEntry = theModel->documentEntry();

    if ( documentActor ){
        viewWindow->RemoveActor( documentActor );
        documentActor->Delete();
    }
    documentActor  = new Document_Actor( theDocumentImpl, theDocumentEntry );
    viewWindow->AddActor( documentActor );

    viewWindow->getRenderer()->Render();
    viewWindow->Repaint();
    if (firstUpdate)
    {
        viewWindow->onFitAll();
        firstUpdate = false;
    }
    MESSAGE("}");
}

void VtkDocumentGraphicView::setVertexSelection()
{
  MESSAGE("VtkDocumentGraphicView::setVertexSelection(){");
  setSelectionMode(NodeSelection);
// //  NodeSelection,
// //  CellSelection,
// //  EdgeOfCellSelection,
// //  EdgeSelection,
// //  FaceSelection,
// //  VolumeSelection,
// //  ActorSelection };
  selectionMode = VERTEX_TREE;
  MESSAGE("}");
}

void VtkDocumentGraphicView::setEdgeSelection()
{
  MESSAGE("PatternDataSelectionModel::setEdgeSelection(){");
  setSelectionMode(EdgeSelection);
  selectionMode = EDGE_TREE;
  MESSAGE("}");
}

void VtkDocumentGraphicView::setQuadSelection()
{
  MESSAGE("VtkDocumentGraphicView::setQuadSelection(){");
  setSelectionMode(FaceSelection);
  selectionMode = QUAD_TREE;
  MESSAGE("}");
}

void VtkDocumentGraphicView::setHexaSelection()
{
  MESSAGE("VtkDocumentGraphicView::setHexaSelection(){");
//  SetSelectionMode(VolumeSelection);
  setSelectionMode(FaceSelection); //temporary for hexa selection debug
  selectionMode = HEXA_TREE;
//  aProp->FaceVisibilityOff (); //would debug hexa preselection, selection???
  MESSAGE("}");
}

void VtkDocumentGraphicView::setAllSelection()
{
  MESSAGE("VtkDocumentGraphicView::setAllSelection(){");
  setSelectionMode(ActorSelection);
  selectionMode = -1;
  MESSAGE("}");
}

void  VtkDocumentGraphicView::setSelectionMode(Selection_Mode theMode)
{
  MESSAGE("VtkDocumentGraphicView::SetSelectionMode(){");

  if ( viewWindow != NULL )
  {
      viewWindow->SetSelectionMode( theMode );
      selectionMode = theMode;
  }

  MESSAGE("}");
}

void VtkDocumentGraphicView::setSelectionMode( const QModelIndex& eltIndex )
{
  MESSAGE("VtkDocumentGraphicView::setSelectionMode( "<< eltIndex.data().toString().toStdString() << " )");
  QVariant treeVariant = eltIndex.data( HEXA_TREE_ROLE );
  if ( !treeVariant.isValid() ) return;
  int eltType = treeVariant.toInt();

  if ( selectionMode == eltType ) return;

  switch ( eltType ){
    case VERTEX_TREE :
    case VERTEX_DIR_TREE : setVertexSelection(); MESSAGE("VERTEX"); break;
    case EDGE_TREE :
    case EDGE_DIR_TREE :   setEdgeSelection(); MESSAGE("EDGE");     break;
    case QUAD_TREE :
    case QUAD_DIR_TREE :   setQuadSelection();   MESSAGE("QUAD");   break;
    case HEXA_TREE :
    case HEXA_DIR_TREE :   setHexaSelection();  MESSAGE("HEXA");   break;
    case PROPAGATION_TREE :
    case PROPAGATION_DIR_TREE :   setEdgeSelection(); MESSAGE("PROPAGATION");   break;
//  CellSelection,
//  EdgeOfCellSelection,
//  VolumeSelection,
//  ActorSelection
    MESSAGE("}");
  }
}

void VtkDocumentGraphicView::clearSelection()
{
    if (viewWindow != NULL)
    {
//        viewWindow->unHighlightAll();
        setSelectionMode(ActorSelection);
    }
}

void VtkDocumentGraphicView::highlight( const QModelIndex& ielt )
{
    MESSAGE("VtkDocumentGraphicView::highlight(const QModelIndex& ielt)");

    QModelIndexList indexList;
    indexList << ielt;
    highlight(indexList);

    MESSAGE("}");
}

void VtkDocumentGraphicView::highlight( const QModelIndexList& elts )
{
    MESSAGE("VtkDocumentGraphicView::highlight(const QModelIndexList& elts");

    if (elts.size() == 0 || viewWindow == NULL || documentActor == NULL) return;

    SVTK_Selector* selector = viewWindow->GetSelector();
    if ( selector == NULL ) return;

//    //   Set selection mode in VTK view
//    viewWindow->SetSelectionMode(VolumeSelection);

    // --- elements highlight ---
    TColStd_MapOfInteger aMap;
    int vtkElemsId;
    QString eltEntry;

    foreach( const QModelIndex& iElt, elts ){
        if (iElt.isValid())
        {
            eltEntry = iElt.data( HEXA_ENTRY_ROLE ).toString();
            vtkElemsId = documentActor->vtkElemsId[ eltEntry.toInt() ];
            if ( vtkElemsId > 0 ) aMap.Add( vtkElemsId );
        }
    }

    selector->AddOrRemoveIndex( documentActor->getIO(), aMap, false ); //true
    viewWindow->highlight( documentActor->getIO(), true, true );
    documentActor->highlight( false ); //unhighlight de la bounding box rouge

    MESSAGE("}");
}


void VtkDocumentGraphicView::highlightGroups( const QModelIndex& eltIndex )
{
    MESSAGE("VtkDocumentGraphicView::highlightGroups( const QModelIndex& eltIndex )");
    if ( groupsModel == NULL || viewWindow == NULL || documentActor == NULL) return;
    SVTK_Selector* selector = viewWindow->GetSelector();
    if ( selector == NULL ) return;

    QVariant treeVariant        = eltIndex.data( HEXA_TREE_ROLE );
    if ( !treeVariant.isValid())
        //INFOS("data from model not valid");
        return;

    int eltType  = treeVariant.toInt();
    if ( eltType != GROUP_TREE )
        //INFOS("bad element type : not a group item" << eltType );
        return;

    //Get elements to highlight
    DocumentModel::Group kind;
    QModelIndexList iElements = groupsModel->getGroupElements( eltIndex, kind );
    if (iElements.count() == 0) return;

    // ---------- elements highlight --------------

    // Set selection mode in VTK view
    switch (kind){
     case HexaCell: case HexaNode: viewWindow->SetSelectionMode(VolumeSelection); break;
     case QuadCell: case QuadNode: viewWindow->SetSelectionMode(FaceSelection);   break;
     case EdgeCell: case EdgeNode: viewWindow->SetSelectionMode(EdgeSelection);   break;
     case VertexNode: viewWindow->SetSelectionMode(NodeSelection); break;
    }

    TColStd_MapOfInteger aMap;
    int vtkElemsId;
    QString eltEntry;

    foreach( const QModelIndex& iElt, iElements ){
        if (iElt.isValid())
        {
            eltEntry = iElt.data( HEXA_ENTRY_ROLE ).toString();
            vtkElemsId = documentActor->vtkElemsId[ eltEntry.toInt() ];
            if ( vtkElemsId > 0 ) aMap.Add( vtkElemsId );
        }
    }

    selector->AddOrRemoveIndex( documentActor->getIO(), aMap, false );
    viewWindow->highlight( documentActor->getIO(), true, true );
    documentActor->highlight( false );

    MESSAGE("}");
}


void VtkDocumentGraphicView::highlightPropagation( const QModelIndex& eltIndex )
{
    MESSAGE("VtkDocumentGraphicView::highlightPropagation( const QModelIndex& eltIndex )");
    if ( meshModel == NULL || viewWindow == NULL || documentActor == NULL) return;
    SVTK_Selector* selector = viewWindow->GetSelector();
    if ( selector == NULL ) return;

    QVariant treeVariant        = eltIndex.data( HEXA_TREE_ROLE );
    if ( !treeVariant.isValid())
        //INFOS("data from model not valid");
        return;

    int eltType  = treeVariant.toInt();
    if ( eltType != PROPAGATION_TREE )
        //INFOS("bad element type : not a group item" << eltType );
        return;

    // Get elements to highlight
    QModelIndexList iEdges = meshModel->getPropagation( eltIndex );
    if (iEdges.count() == 0) return;

    // ---------- elements highlight --------------

    // Set selection mode in VTK view
    viewWindow->SetSelectionMode(EdgeSelection);

    TColStd_MapOfInteger aMap;
    int vtkElemsId;
    QString edgeEntry;

    foreach( const QModelIndex& iEdge, iEdges ){
        if (iEdge.isValid())
        {
            edgeEntry = iEdge.data( HEXA_ENTRY_ROLE ).toString();
            vtkElemsId = documentActor->vtkElemsId[ edgeEntry.toInt() ];
            if ( vtkElemsId > 0 ) aMap.Add( vtkElemsId );
        }
    }

    selector->AddOrRemoveIndex( documentActor->getIO(), aMap, false );
    viewWindow->highlight( documentActor->getIO(), true, true );
    documentActor->highlight( false );

    MESSAGE("}");
}




/********************************************************************************
 *                   ABSTRACT METHOD ( MUST BE IMPLEMENTED )
 ********************************************************************************/

/*
    Returns the item that covers the coordinate given in the view.
 */

QModelIndex VtkDocumentGraphicView::indexAt(const QPoint &point) const
{
    MESSAGE("VtkDocumentGraphicView::indexAt(){");
    MESSAGE("}");
    return QModelIndex();
}

void VtkDocumentGraphicView::scrollTo(const QModelIndex &index, ScrollHint)
{
    MESSAGE("VtkDocumentGraphicView::scrollTo(){");
    MESSAGE("*  item   is: " << index.data().toString().toStdString());
    MESSAGE("}");
}


/*
    Returns the position of the item in viewport coordinates.
 */
QRect VtkDocumentGraphicView::visualRect(const QModelIndex &index) const
{
    MESSAGE("VtkDocumentGraphicView::visualRect(){");
    MESSAGE("*  item   is: " << index.data().toString().toStdString());
    MESSAGE("}");
    return QRect();
}

// PROTECTED :
int VtkDocumentGraphicView::horizontalOffset() const
{
    MESSAGE("VtkDocumentGraphicView::horizontalOffset(){");
    MESSAGE("}");
    return horizontalScrollBar()->value();
}

bool VtkDocumentGraphicView::isIndexHidden(const QModelIndex &index) const
{
    MESSAGE("VtkDocumentGraphicView::isIndexHidden(){");
    MESSAGE("*  item   is: " << index.data().toString().toStdString());
    MESSAGE("}");
    return false;
}

QModelIndex VtkDocumentGraphicView::moveCursor(QAbstractItemView::CursorAction cursorAction,
        Qt::KeyboardModifiers /*modifiers*/)
{
    MESSAGE("VtkDocumentGraphicView::moveCursor(){");
    QModelIndex current = currentIndex();
    MESSAGE("*  current  is: " << current.data(Qt::DisplayRole).toString().toStdString());
    MESSAGE("}");
    return current;
}

/*
    Find the indices corresponding to the extent of the selection.
 */
void VtkDocumentGraphicView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    MESSAGE("VtkDocumentGraphicView::setSelection(){");
    MESSAGE("}");
}


int VtkDocumentGraphicView::verticalOffset() const
{
    MESSAGE("VtkDocumentGraphicView::verticalOffset(){");
    MESSAGE("}");
    return verticalScrollBar()->value();
}

/*
    Returns a region corresponding to the selection in viewport coordinates.
 */
QRegion VtkDocumentGraphicView::visualRegionForSelection(const QItemSelection &selection) const
{
    MESSAGE("VtkDocumentGraphicView::visualRegionForSelection(){");
    MESSAGE("}");
    return QRegion();
}




/********************************************************************************
 *                   PROTECTED SLOTS
 ********************************************************************************/
void VtkDocumentGraphicView::closeEditor ( QWidget * editor, QAbstractItemDelegate::EndEditHint hint )
{ 
    MESSAGE("VtkDocumentGraphicView::closeEditor(){");
    MESSAGE("*  hint is: " << hint);
    MESSAGE("}");
}

void VtkDocumentGraphicView::commitData ( QWidget * editor )
{ 
    MESSAGE("VtkDocumentGraphicView::commitData(){");
    MESSAGE("}");
}

void VtkDocumentGraphicView::currentChanged( const QModelIndex & current, const QModelIndex & previous )
{ 
    //   MESSAGE("VtkDocumentGraphicView::currentChanged(){");
    //   MESSAGE("*  current  is: " << current.data().toString().toStdString());
    //   MESSAGE("*  previous is: " << previous.data().toString().toStdString());
    //   MESSAGE("}");
    //   openPersistentEditor( current );
    _currentChanged = true;
}

void VtkDocumentGraphicView::dataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight )
{ 
    //   MESSAGE("VtkDocumentGraphicView::dataChanged(){");
    //   MESSAGE("*  topLeft     is: " << topLeft.data().toString().toStdString());
    //   MESSAGE("*  bottomRight is: " << bottomRight.data().toString().toStdString());
    //   MESSAGE("}");
    MESSAGE("VtkDocumentGraphicView::dataChanged(){");
    update();
    _currentChanged = false;
    //   updateObject(topLeft);
}

void VtkDocumentGraphicView::editorDestroyed ( QObject * editor )
{ 
    MESSAGE("VtkDocumentGraphicView::editorDestroyed(){");
    MESSAGE("}");
}

void VtkDocumentGraphicView::rowsAboutToBeRemoved ( const QModelIndex & parent, int start, int end )
{ 
    //   MESSAGE("VtkDocumentGraphicView::rowsAboutToBeRemoved (){");
    //   MESSAGE("*  parent    is: " << parent.data(Qt::DisplayRole).toString().toStdString());
    //   MESSAGE("*  start     is: " << start);
    //   MESSAGE("*  end       is: " << end);
    //   MESSAGE("}");
}

void VtkDocumentGraphicView::rowsInserted ( const QModelIndex & parent, int start, int end )
{ 
    //   MESSAGE("VtkDocumentGraphicView::rowsInserted(){");
    //   MESSAGE("*  parent    is: " << parent.data(Qt::DisplayRole).toString().toStdString());
    //   MESSAGE("*  start     is: " << start);
    //   MESSAGE("*  end       is: " << end);
    //   MESSAGE("}");
}

void VtkDocumentGraphicView::selectionChanged( const QItemSelection & selected, const QItemSelection & deselected )
{ 
    MESSAGE("VtkDocumentGraphicView::selectionChanged(){");
    foreach( const QModelIndex& isel, selected.indexes() ){
        MESSAGE("*  selected : " << isel.data().toString().toStdString());
    }
    foreach( const QModelIndex& iunsel, deselected.indexes() ){
        MESSAGE("*  unselected : " << iunsel.data().toString().toStdString());
    }
    MESSAGE("}");
}

void VtkDocumentGraphicView::updateGeometries ()
{ 
    MESSAGE("VtkDocumentGraphicView::updateGeometries (){");
    MESSAGE("}");
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

//show the actor when show=true and hide it when show=false
//void VtkDocumentGraphicView::showActor(bool show)
//{
//    SVTK_ViewWindow*    theVTKViewWindow = dynamic_cast<SVTK_ViewWindow*>(_suitView);
//    if (theVTKViewWindow == NULL || _documentActor == NULL)
//        return;
//
//    if (show)
//    {
//        _documentActor->SetVisibility(1);
//        theVTKViewWindow->onFitAll();
//    }
//    else
//    {
//        _documentActor->SetVisibility(0);
//        theVTKViewWindow->onResetView ();
//    }
//}

void VtkDocumentGraphicView::setModel ( QAbstractItemModel * model )
{
    MESSAGE("VtkDocumentGraphicView::setModel (){");
    QAbstractItemView::setModel( model );

    //   PatternDataModel* pdm = dynamic_cast<PatternDataModel*>(model);
    //   MESSAGE("pdm"<<pdm);
    //   if (pdm){
    //     connect( pdm, SIGNAL(patternDataChanged() ), this,  SLOT ( onPatternDatachanged() ) );
    //   }

    DocumentModel* dm = dynamic_cast<DocumentModel*>(model);
    MESSAGE("dm"<<dm);
    if (dm){
        //    setWindowTitle( dm->getName() );
        connect( dm, SIGNAL(patternDataChanged() ), this,  SLOT ( onPatternDatachanged() ) );
        //    connect( dm, SIGNAL( nameChanged(const QString&) ), this,  SLOT ( setWindowTitle(const QString&) ) );
    }
}

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
