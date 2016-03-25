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

//#define _DEVDEBUG_

// VTK includes
#include <vtkRenderer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include "vtkCellArray.h"
#include "vtkPolyData.h"

#include <VTKViewer_CellLocationsArray.h>
#include <SVTK_ViewModel.h>

#include "HEXABLOCKGUI_Trace.hxx"
#include "HEXABLOCKGUI_VtkDocumentGraphicView.hxx"
#include "HEXABLOCKGUI.hxx"

#include <QScrollBar>

#ifndef M_PI
#define M_PI 3.1415927
#endif

using namespace std;
using namespace HEXABLOCK::GUI;
using namespace HEXA_NS;

Document_Actor::Document_Actor( Document* doc, const QString& entry ):
          SALOME_Actor(),
          _doc( doc )
{
    DEBTRACE("Document_Actor::Document_Actor " << entry.toLatin1() );
    const char* docName = ((doc != NULL) ? doc->getName() : "");
    Handle(SALOME_InteractiveObject) anIO = new SALOME_InteractiveObject( entry.toLatin1(), "HEXABLOCK", docName );//,theName); CS_TODO
    setIO(anIO);
    vtkUnstructuredGrid* aGrid = getUnstructuredGrid();
    vtkDataSetMapper* aMapper = vtkDataSetMapper::New();
    aMapper->SetInputData(aGrid); // saclay
//    aMapper->SetInput(aGrid);
    aGrid->Delete();

    SetVisibility( true );//VisibilityOff();
    SetPickable( true ); //PickableOff();//
    SetMapper( aMapper );
    aMapper->Delete();

    vtkProperty* aProp = vtkProperty::New();

//    aProp->SetRepresentationToSurface();
    aProp->SetRepresentationToWireframe();
//    aProp->SetRepresentationToPoints();
    aProp->EdgeVisibilityOn ();
    aProp->SetPointSize(5);
    SetProperty( aProp );
    aProp->Delete();
//    SetPointRepresentation(true);
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
    const char* docName = ((doc != NULL) ? doc->getName() : "");
    Handle(SALOME_InteractiveObject) anIO = new SALOME_InteractiveObject( entry.toLatin1(), "HEXABLOCK", docName );//,theName); CS_TODO
    setIO(anIO);
    vtkUnstructuredGrid* aGrid = getUnstructuredGrid();

    vtkDataSetMapper* aMapper = vtkDataSetMapper::New();
    aMapper->SetInputData(aGrid); // saclay
//    aMapper->SetInput (aGrid);
    aGrid->Delete();

    SetVisibility( true );//VisibilityOff();
    SetPickable( true ); //PickableOff();//
    SetMapper( aMapper );
    aMapper->Delete();

    vtkProperty* aProp = vtkProperty::New();
    aProp->SetColor(0,255,0);
//    aProp->SetRepresentationToSurface();
    aProp->SetRepresentationToWireframe();
//    aProp->SetRepresentationToPoints();
    aProp->EdgeVisibilityOn ();
    aProp->SetPointSize(5);
    SetProperty( aProp );
    aProp->Delete();
//    SetPointRepresentation(true);
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

    //Model
    setModel(documentModel);
    patternDataModel     = new PatternDataModel(parent);
    patternGeomModel     = new PatternGeomModel(parent);
    groupsModel          = new GroupsModel(parent);
    meshModel            = new MeshModel(parent);

    patternDataModel->setSourceModel(documentModel);
    patternGeomModel->setSourceModel(documentModel);
    groupsModel->setSourceModel(documentModel);
    meshModel->setSourceModel(documentModel);

    //Selection Model
    patternDataSelectionModel    = new PatternDataSelectionModel(patternDataModel);
    patternGeomSelectionModel    = new PatternGeomSelectionModel(patternGeomModel);
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

    //Delete Model
    delete patternDataModel;
    delete groupsModel;
    delete meshModel;

    //Delete Selection Model/ Disconnect signals on delete (Qt)
//    delete patternDataSelectionModel;
//    delete groupsSelectionModel;
//    delete meshSelectionModel;
}

void VtkDocumentGraphicView::setWindowTitle(const QString& title)
{
    viewWindow->setWindowTitle( QString("hexablock : ") + title );
}

void VtkDocumentGraphicView::onPatternDatachanged()
{
    update();
}

void VtkDocumentGraphicView::removeActor()
{
    if ( documentActor != NULL && viewWindow != NULL){
          viewWindow->RemoveActor( documentActor );
          documentActor->Delete();
    }
}

void VtkDocumentGraphicView::update()
{
    DocumentModel*   theModel = dynamic_cast<DocumentModel*>( model() );
    if (theModel == NULL || viewWindow == NULL) return;

    Document*  theDocumentImpl  = theModel->documentImpl();
    QString    theDocumentEntry = theModel->documentEntry();

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
}

void VtkDocumentGraphicView::setVertexSelection()
{
  setSelectionMode(NodeSelection);
}

void VtkDocumentGraphicView::setEdgeSelection()
{
  setSelectionMode(EdgeSelection);
}

void VtkDocumentGraphicView::setQuadSelection()
{
  setSelectionMode(FaceSelection);
}

void VtkDocumentGraphicView::setHexaSelection()
{
//  SetSelectionMode(VolumeSelection);
  setSelectionMode(FaceSelection); //temporary for hexa selection debug
  selectionMode = HEXA_TREE;
//  aProp->FaceVisibilityOff (); //would debug hexa preselection, selection???
}

void VtkDocumentGraphicView::setAllSelection()
{
  setSelectionMode(ActorSelection);
  selectionMode = -1;
}

void  VtkDocumentGraphicView::setSelectionMode(Selection_Mode theMode)
{
    if (viewWindow == NULL || selectionMode == theMode)
        return;

    HEXABLOCKGUI::selectionMgr()->clearSelected();
    viewWindow->SetSelectionMode( theMode );
    selectionMode = theMode;
}

void VtkDocumentGraphicView::setSelectionMode( const QModelIndex& eltIndex )
{
  QVariant treeVariant = eltIndex.data( HEXA_TREE_ROLE );
  if ( !treeVariant.isValid() ) return;
  int eltType = treeVariant.toInt();

  if ( selectionMode == eltType ) return;

  switch ( eltType ){
    case VERTEX_TREE :
    case GEOMPOINT_TREE:
    case GEOMPOINT_DIR_TREE:
    case VERTEX_DIR_TREE : setVertexSelection(); break;
    case EDGE_TREE :
    case GEOMEDGE_TREE:
    case GEOMEDGE_DIR_TREE:
    case EDGE_DIR_TREE :   setEdgeSelection(); break;
    case QUAD_TREE :
    case GEOMFACE_TREE:
    case GEOMFACE_DIR_TREE:
    case QUAD_DIR_TREE :   setQuadSelection();   break;
    case HEXA_TREE :
    case GEOMSHAPE_TREE:
    case GEOMSHAPE_DIR_TREE:
    case HEXA_DIR_TREE :   setHexaSelection();  break;
    case PROPAGATION_TREE :
    case PROPAGATION_DIR_TREE :   setEdgeSelection(); break;
    default: setAllSelection();
  }
}

void VtkDocumentGraphicView::getSelected(SALOME_ListIO& selectedObjects)
{
    HEXABLOCKGUI::selectionMgr()->selectedObjects( selectedObjects, SVTK_Viewer::Type());
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

    QModelIndexList indexList;
    indexList << ielt;
    highlight(indexList);

}

void VtkDocumentGraphicView::highlight( const QModelIndexList& elts )
{

    if (elts.size() == 0 || viewWindow == NULL || documentActor == NULL)
        return;
    SVTK_Selector* selector = viewWindow->GetSelector();
    if ( selector == NULL )
        return;
    //   Set selection mode in VTK view
//    viewWindow->SetSelectionMode(VolumeSelection);

    // --- elements highlight ---
    TColStd_IndexedMapOfInteger aMap;
    int vtkElemsId;
    QString eltEntry;

    foreach( const QModelIndex& iElt, elts ){
        if (iElt.isValid())
        {
            eltEntry = iElt.data( HEXA_ENTRY_ROLE ).toString();
            vtkElemsId = documentActor->vtkElemsId[ eltEntry.toInt() ];
            if ( vtkElemsId > 0 )
                aMap.Add( vtkElemsId );
        }
    }
    selector->AddOrRemoveIndex( documentActor->getIO(), aMap, false );
    viewWindow->highlight( documentActor->getIO(), true, true );
}


void VtkDocumentGraphicView::highlightGroups( const QModelIndex& eltIndex )
{
    if ( groupsModel == NULL || viewWindow == NULL || documentActor == NULL) return;
    SVTK_Selector* selector = viewWindow->GetSelector();
    if ( selector == NULL ) return;

    QVariant treeVariant        = eltIndex.data( HEXA_TREE_ROLE );
    if ( !treeVariant.isValid())
        return;

    int eltType  = treeVariant.toInt();
    if ( eltType != GROUP_TREE )
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

}


void VtkDocumentGraphicView::highlightPropagation( const QModelIndex& eltIndex )
{
    if ( meshModel == NULL || viewWindow == NULL || documentActor == NULL) return;
    SVTK_Selector* selector = viewWindow->GetSelector();
    if ( selector == NULL ) return;

    QVariant treeVariant        = eltIndex.data( HEXA_TREE_ROLE );
    if ( !treeVariant.isValid())
        return;

    int eltType  = treeVariant.toInt();
    if ( eltType != PROPAGATION_TREE )
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

}




/********************************************************************************
 *                   ABSTRACT METHOD ( MUST BE IMPLEMENTED )
 ********************************************************************************/
/*
    Returns the item that covers the coordinate given in the view.
 */
QModelIndex VtkDocumentGraphicView::indexAt(const QPoint &point) const
{
    return QModelIndex();
}

void VtkDocumentGraphicView::scrollTo(const QModelIndex &index, ScrollHint)
{
}

/*
    Returns the position of the item in viewport coordinates.
 */
QRect VtkDocumentGraphicView::visualRect(const QModelIndex &index) const
{
    return QRect();
}

// PROTECTED :
int VtkDocumentGraphicView::horizontalOffset() const
{
    return horizontalScrollBar()->value();
}

bool VtkDocumentGraphicView::isIndexHidden(const QModelIndex &index) const
{
    return false;
}

QModelIndex VtkDocumentGraphicView::moveCursor(QAbstractItemView::CursorAction cursorAction,
        Qt::KeyboardModifiers /*modifiers*/)
{
    QModelIndex current = currentIndex();
    return current;
}

/*
    Find the indices corresponding to the extent of the selection.
 */
void VtkDocumentGraphicView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
}


int VtkDocumentGraphicView::verticalOffset() const
{
    return verticalScrollBar()->value();
}

/*
    Returns a region corresponding to the selection in viewport coordinates.
 */
QRegion VtkDocumentGraphicView::visualRegionForSelection(const QItemSelection &selection) const
{
    return QRegion();
}


/********************************************************************************
 *                   PROTECTED SLOTS
 ********************************************************************************/
void VtkDocumentGraphicView::closeEditor ( QWidget * editor, QAbstractItemDelegate::EndEditHint hint )
{
}

void VtkDocumentGraphicView::commitData ( QWidget * editor )
{
}

void VtkDocumentGraphicView::currentChanged( const QModelIndex & current, const QModelIndex & previous )
{
    _currentChanged = true;
}

void VtkDocumentGraphicView::dataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight )
{
    update();
    _currentChanged = false;
}

void VtkDocumentGraphicView::editorDestroyed ( QObject * editor )
{
}

void VtkDocumentGraphicView::rowsAboutToBeRemoved ( const QModelIndex & parent, int start, int end )
{
}

void VtkDocumentGraphicView::rowsInserted ( const QModelIndex & parent, int start, int end )
{
}

void VtkDocumentGraphicView::selectionChanged( const QItemSelection & selected, const QItemSelection & deselected )
{
}

void VtkDocumentGraphicView::updateGeometries ()
{
}


void VtkDocumentGraphicView::setModel ( QAbstractItemModel * model )
{
    QAbstractItemView::setModel( model );

//    PatternDataModel* pdm = dynamic_cast<PatternDataModel*>(model);
//    if (pdm)
//       connect( pdm, SIGNAL(patternDataChanged() ), this,  SLOT ( onPatternDatachanged() ) );

    DocumentModel* dm = dynamic_cast<DocumentModel*>(model);
    if (dm){
//        setWindowTitle( dm->getName() );
        connect( dm, SIGNAL(patternDataChanged() ), this,  SLOT ( onPatternDatachanged() ) );
//        connect( dm, SIGNAL( nameChanged(const QString&) ), this,  SLOT ( setWindowTitle(const QString&) ) );
    }
}
