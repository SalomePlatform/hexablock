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

#include <set>


#include <SUIT_MessageBox.h>
#include <SUIT_Session.h>
#include <SUIT_Selector.h>
#include <SUIT_Desktop.h>


#include "HEXABLOCKGUI_SalomeTools.hxx"
#include "HEXABLOCKGUI_DocumentSelectionModel.hxx"
#include "HEXABLOCKGUI_DocumentGraphicView.hxx"
#include "HEXABLOCKGUI_DocumentItem.hxx"
#include "HEXABLOCKGUI.hxx"
#include "HEXABLOCKGUI_DataModel.hxx"
#include "HEXABLOCKGUI_DataObject.hxx"


#include <SVTK_Selector.h>
#include <SVTK_ViewModel.h>


// #include <SalomeApp_DataObject.h>
#include <SalomeApp_Study.h>
// #include <SalomeApp_Module.h>
#include <SalomeApp_Application.h>
#include <SALOME_Actor.h>
#include <SALOME_ListIO.hxx>
#include <SALOME_ListIteratorOfListIO.hxx>

#include "GEOMBase_Helper.h"

//#define _DEVDEBUG_
using namespace std;
using namespace HEXABLOCK::GUI;



PatternDataSelectionModel::PatternDataSelectionModel( QAbstractItemModel * model ):
QItemSelectionModel( model ),
_theModelSelectionChanged(false),
_theVtkSelectionChanged(false),
_theGeomSelectionChanged(false),
_selectionFilter(-1),
_salomeSelectionMgr(0),
_geomHelper( 0 )
{
  connect( this, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
           this, SLOT( onCurrentChanged( const QModelIndex & , const QModelIndex & ) ) );
  connect( this, SIGNAL( selectionChanged( const QItemSelection & , const QItemSelection & ) ),
           this, SLOT( onSelectionChanged( const QItemSelection & , const QItemSelection & ) ) );

}

// PatternDataSelectionModel::PatternDataSelectionModel( QAbstractItemModel * model, QObject * parent ):
// QItemSelectionModel( model, parent ),
// _salomeSelectionChanged(false)
// {
//   _salomeSelectionMgr = PatternDataSelectionModel::selectionMgr();
// 
//   connect( _salomeSelectionMgr, SIGNAL( currentSelectionChanged() ), this, SLOT( salomeSelectionChanged() ) );
//   connect( this, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
//           this, SLOT( onCurrentChanged( const QModelIndex & , const QModelIndex & ) ) );
//   connect( this, SIGNAL( selectionChanged( const QItemSelection & , const QItemSelection & ) ),
//           this, SLOT( onSelectionChanged( const QItemSelection & , const QItemSelection & ) ) );
// }



PatternDataSelectionModel::~PatternDataSelectionModel()
{
  disconnect( this, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
          this, SLOT( onCurrentChanged( const QModelIndex & , const QModelIndex & ) ) );
  disconnect( this, SIGNAL( selectionChanged( const QItemSelection & , const QItemSelection & ) ),
          this, SLOT( onSelectionChanged( const QItemSelection & , const QItemSelection & ) ) );

  if ( _salomeSelectionMgr )
    disconnect( _salomeSelectionMgr, SIGNAL( currentSelectionChanged() ), this, SLOT( salomeSelectionChanged() ) );
}


void PatternDataSelectionModel::setVertexSelection()
{
  SetSelectionMode(NodeSelection);
// //  NodeSelection,
// //  CellSelection,
// //  EdgeOfCellSelection,
// //  EdgeSelection,
// //  FaceSelection,
// //  VolumeSelection,
// //  ActorSelection };
  _selectionFilter = VERTEX_TREE;
}

void PatternDataSelectionModel::setEdgeSelection()
{
  SetSelectionMode(EdgeSelection);
  _selectionFilter = EDGE_TREE;
}

void PatternDataSelectionModel::setQuadSelection()
{
  SetSelectionMode(FaceSelection);
  _selectionFilter = QUAD_TREE;
}

void PatternDataSelectionModel::setHexaSelection()
{
  SetSelectionMode(VolumeSelection);
  _selectionFilter = HEXA_TREE;
}


void PatternDataSelectionModel::setAllSelection()
{
  _selectionFilter = -1;
}


void PatternDataSelectionModel::setSalomeSelectionMgr( LightApp_SelectionMgr* mgr )
{
  _salomeSelectionMgr = mgr;
  connect( _salomeSelectionMgr, SIGNAL( currentSelectionChanged() ), this, SLOT( salomeSelectionChanged() ) );
}


void  PatternDataSelectionModel::SetSelectionMode(Selection_Mode theMode)
{
  SVTK_ViewWindow* aViewWindow = _getVTKViewWindow();
//   aViewWindow->clearFilters();
//   _salomeSelectionMgr->clearFilters();
  aViewWindow->SetSelectionMode( theMode );
}


//     A) De Salome(OCC, VTK) vers Hexablock(TreeView Data) :
//         -Vertex:
//             in:     selection (vertex associé OCC).
//             out:    (vertex VTK) highlighté, (vertex TreeView) sélectionné.
// 
//             in :    selection (vertex associé VTK).
//             out:    (vertex OCC) highlighté, (vertex TreeView) sélectionné.
// 
//         -Edge:
//             in:     selection (edge associé OCC).
//             out:    (vertex VTK) highlighté, (vertex TreeView) sélectionné.
// 
//             in:     selection (edge associé VTK).
//             out:    (toute la ligne OCC) highlighté + vertex sur (deb,fin) , (edge TreeView) sélectionné.
// 
//         -Face: idem vertex
void PatternDataSelectionModel::onCurrentChanged( const QModelIndex & current, const QModelIndex & previous )
{
  std::cout << "PatternDataSelectionModel::onCurrentChanged " << std::endl;
//   _selectSalome( current, true );
//   _selectSalome( previous, false );
}



void PatternDataSelectionModel::onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected )
{
  std::cout << "******PatternDataSelectionModel::onSelectionChanged" << std::endl;
//   if ( _salomeSelectionChanged ) return;// to avoid infinite loop
  _theModelSelectionChanged = true;

  if ( _salomeSelectionMgr == NULL ) return;
  _salomeSelectionMgr->clearSelected();

  QModelIndexList indexes = selected.indexes();
  for( QModelIndexList::const_iterator i_index = indexes.begin(); i_index != indexes.end(); ++i_index ){
    std::cout << "entry selected" << i_index->data( HEXA_ENTRY_ROLE ).toString().toStdString() << std::endl;
    if ( !_theGeomSelectionChanged ) _selectGEOM( *i_index );
    if ( !_theVtkSelectionChanged )  _selectVTK( *i_index );
  }

  _theModelSelectionChanged = false;
 // CS_BP todo SALOMEGUI_Swig.cxx:370
//   indexes = deselected.indexes();
//   for( QModelIndexList::const_iterator i_index = indexes.begin(); i_index != indexes.end(); ++i_index )
//     _unselectSalome( *i_index);
}







void PatternDataSelectionModel::salomeSelectionChanged()
{
  QModelIndex eltIndex;

  std::cout << "PatternDataSelectionModel::salomeSelectionChanged()" << std::endl;
  SALOME_ListIO salomeSelected;
//   _salomeSelectionMgr->selectedObjects( salomeSelected, SVTK_Viewer::Type() );//salomeSelected.Extent()
  _salomeSelectionMgr->selectedObjects( salomeSelected, NULL, false ); 
  if ( salomeSelected.IsEmpty() ) return;

  Handle(SALOME_InteractiveObject) anIObject;
  SALOME_ListIteratorOfListIO it(salomeSelected);
  for( ; it.More(); it.Next()){
    anIObject = it.Value(); //
    std::cout << "obj name => " << anIObject->getName() << std::endl;
    eltIndex = _geomSelectionChanged( anIObject );// is it comming from GEOM?
    if ( !eltIndex.isValid() ){
      eltIndex = _vtkSelectionChanged( anIObject ); ;// or VTK?...
    }
  }
}



SVTK_ViewWindow* PatternDataSelectionModel::_getVTKViewWindow()
{
  SVTK_ViewWindow* aVtkView = HEXABLOCKGUI::currentVtkView;
  std::cout << "aVtkView ===========>" << aVtkView<< std::endl;
  return aVtkView;
}


QModelIndex PatternDataSelectionModel::_indexOf( const QString& anEntry, int role )
{
  std::cout << "_indexOf  " <<anEntry.toStdString() << std::endl;
  QModelIndex elt; // element (vertex, edge, quad) of model
  const QAbstractItemModel* theModel = model();
  QModelIndex         theStart = theModel->index(0, 0);
  QString exactMatch = anEntry + ";";
  
  QModelIndexList     theIndexes = theModel->match( theStart,
                                          role,
                                          exactMatch, //anEntry,
                                          1,
                                          Qt::MatchRecursive | Qt::MatchContains );//Qt::MatchFixedString );
// Qt::MatchContains  );
  if ( theIndexes.count()>0 ){
    elt = theIndexes[0] ;
    std::cout << "_indexOf YES found -> " << elt.data().toString().toStdString() << std::endl;
  } else {
    std::cout << "_indexOf NOT found" << std::endl;
  }

  return elt;
}


void PatternDataSelectionModel::_setVTKSelectionMode( const QModelIndex& eltIndex, SVTK_ViewWindow* vtkViewWindow )
{
  QVariant treeVariant = eltIndex.data( HEXA_TREE_ROLE );
  int eltType;

  if ( !treeVariant.isValid() ) return;
  eltType = treeVariant.toInt();

  if ( (_selectionFilter != -1) and ( _selectionFilter != eltType ) ) return;

  switch ( eltType ){
    case VERTEX_TREE :
    case VERTEX_DIR_TREE : vtkViewWindow->SetSelectionMode(NodeSelection);     break;
    case EDGE_TREE :
    case EDGE_DIR_TREE :   vtkViewWindow->SetSelectionMode(EdgeSelection);     break;
    case QUAD_TREE :
    case QUAD_DIR_TREE :   vtkViewWindow->SetSelectionMode(FaceSelection);     break;
    case HEXA_TREE :
    case HEXA_DIR_TREE :   vtkViewWindow->SetSelectionMode(VolumeSelection);   break;
//  CellSelection,
//  EdgeOfCellSelection,
//  VolumeSelection,
//  ActorSelection
  }
}



// 1 vertex -> 1 point
// 1 edge   -> n lines + points(deb,fin)
// 1 quad   -> n faces
void PatternDataSelectionModel::_selectGEOM( const QModelIndex & anEltIndex )
{
  std::cout << "PatternDataSelectionModel::_selectGEOM" << std::endl;
//CS_TODO : selection des associations + cas highlight edge
  QString               assocEntry;
//   ObjectList            assocList;
  GEOM::GEOM_Object_var assoc;
  QModelIndex childIndex;

  if ( _geomHelper == NULL )
    _geomHelper = new MyGEOMBase_Helper( SUIT_Session::session()->activeApplication()->desktop() );
  _geomHelper->erasePreview(true);


  _PTR(Study)   aStudy = GetActiveStudyDocument();
  _PTR(SObject) aSChild;
  CORBA::Object_var aCorbaObj = CORBA::Object::_nil();
  const QAbstractItemModel* theModel = model();
  std::cout << "PatternDataSelectionModel::_selectGEOM go find ASSOCIATION for " << anEltIndex.data().toString().toStdString()  << std::endl;

  QVariant assocEntriesVariant = anEltIndex.data( HEXA_ASSOC_ENTRY_ROLE );
  if ( !assocEntriesVariant.isValid() ) { std::cout<<"NOT FOUND"<< std::endl; return;}
  QString assocEntries = assocEntriesVariant.toString();

  std::cout << "assocEntries => " << assocEntries.toStdString()<< std::endl;
//   if ( !theModel->hasChildren( anEltIndex ) ) return;

  QStringList entries = assocEntries.split( ";", QString::SkipEmptyParts );
  
  foreach( const QString& entry, entries ){
    std::cout << "FOUND=> " << entry.toStdString() << std::endl;
    aSChild =  aStudy->FindObjectID( entry.toStdString() );
    aCorbaObj = corbaObj(aSChild);
    assoc = GEOM::GEOM_Object::_narrow(aCorbaObj);
//     SALOMEDS::Color aColor;
//     aColor.R = 15;
//     aColor.G = 1;
//     aColor.B = 1;
//     assoc->SetColor( aColor);//Quantity_NOC_RED );
    if ( !CORBA::is_nil(assoc) ){
      std::cout << "geom selected =>" << assoc->GetName() << std::endl;
//       assocList.push_back(assoc);
      _geomHelper->displayPreview( assoc,
                          true, //false, //append,
                          true, //activate,
                          true,//update,
                          4,//lineWidth,
                          1,//-1,//displayMode,
                          Quantity_NOC_RED) ;
//       _geomHelper->display( assocList );
    } else {
      std::cout << "not a geom =>" << assoc->GetName() << std::endl;
    }
  }
}


void PatternDataSelectionModel::_selectVTK( const QModelIndex& eltIndex )
{
  std::cout << "PatternDataSelectionModel::_selectVTK" << std::endl;
  SVTK_ViewWindow* currentVTKViewWindow = NULL;
  SVTK_Selector* selector = NULL;

  // document selection
  Document_Actor* docActor = NULL;
  Handle(SALOME_InteractiveObject) docIO;
  SALOME_ListIO aList;

  // element highlight
  TColStd_MapOfInteger aMap;
  QList<int>::const_iterator anIter;
  int vtkElemsId;

  // data from model
  QString eltEntry;
  QString docEntry;
  QVariant entryVariant;
  QVariant docEntryVariant;

  currentVTKViewWindow = _getVTKViewWindow();
  if ( currentVTKViewWindow == NULL ) return;

  // Set selection mode in VTK view
  _setVTKSelectionMode( eltIndex, currentVTKViewWindow );

  // Get Data from model
  entryVariant    = eltIndex.data( HEXA_ENTRY_ROLE );
  docEntryVariant = eltIndex.data( HEXA_DOC_ENTRY_ROLE );

  if ( !entryVariant.isValid() ) { std::cout<<"entryVariant not valid"<< std::endl; return;}
  if ( !docEntryVariant.isValid() ) { std::cout<<"docEntryVariant not valid"<< std::endl;  return;}

  eltEntry = entryVariant.toString();
  docEntry = docEntryVariant.toString();

// 
  std::cout<<"eltEntry " << eltEntry.toStdString() << std::endl;
  std::cout<<"docEntry " << docEntry.toStdString() << std::endl;

  // Select the document in Salome
  docActor = dynamic_cast<Document_Actor*>( findActorByEntry( currentVTKViewWindow, docEntry.toLatin1() ) );
  if ( docActor == NULL)  return;
  std::cout<<"yes found an Actor for " << docEntry.toStdString() << std::endl;

  if ( _salomeSelectionMgr == NULL ) return;

  _salomeSelectionMgr->selectedObjects( aList );
  docIO = docActor->getIO();
  aList.Append(docIO);
  _salomeSelectionMgr->setSelectedObjects( aList, true );//false );//true ); //CS_BP false?

  // Highlight in vtk view the element from document 
  vtkElemsId = docActor->vtkElemsId[ eltEntry.toInt() ];// get vtk ids from actor
  if ( vtkElemsId > 0 ) // CS_BP ?: erreur si 1er elt == vertex (0,0,0)
    aMap.Add( vtkElemsId );
//     for (anIter = theIds.begin(); anIter != theIds.end(); ++anIter) {
//       aMap.Add(*anIter);
//     }
  selector = currentVTKViewWindow->GetSelector();
  selector->AddOrRemoveIndex( docIO, aMap, false );
  currentVTKViewWindow->highlight( docIO, true, true );
}


QModelIndex PatternDataSelectionModel::_geomSelectionChanged( const Handle(SALOME_InteractiveObject)& anIObject )
{
  QModelIndex eltIndex;// the element of the model which is associated to the geom object and that is to be selected
  bool fromGEOM = ( strcmp("GEOM", anIObject->getComponentDataType()) == 0 );
  std::cout << "_geomSelectionChanged fromGEOM " << fromGEOM << std::endl;
  if ( fromGEOM ) {
    CORBA::Object_var aCorbaObj  = corbaObj( anIObject ); //CORBA::Object_ptr 
    if ( !CORBA::is_nil(aCorbaObj) ){
      GEOM::GEOM_Object_var aGeomObj = GEOM::GEOM_Object::_narrow( aCorbaObj );
      if ( !CORBA::is_nil(aGeomObj) ){
        QString aGeomObjEntry      = aGeomObj->GetEntry();
        QString aGeomObjStudyEntry = aGeomObj->GetStudyEntry();
        std::cout << "geom selected => "<< aGeomObj->GetName() << std::endl;
        std::cout << "geom selected => "<< aGeomObjEntry.toStdString()  << std::endl;
        std::cout << "geom selected => "<< aGeomObjStudyEntry.toStdString() << std::endl;
//         eltIndex = _eltIndexOf( aGeomObjStudyEntry );//aGeomObjEntry );
        eltIndex = _indexOf( aGeomObjStudyEntry, HEXA_ASSOC_ENTRY_ROLE );//aGeomObjEntry );

        if ( !_theModelSelectionChanged && eltIndex.isValid() ){ // select in model
          _theGeomSelectionChanged = true;
          //       select( newIndex, QItemSelectionModel::ClearAndSelect );
          //       select( newIndex, QItemSelectionModel::SelectCurrent );
          setCurrentIndex( eltIndex, QItemSelectionModel::SelectCurrent );
          setCurrentIndex( eltIndex, QItemSelectionModel::SelectCurrent );
          std::cout << "setCurrentIndex => "<< eltIndex.data().toString().toStdString() << std::endl;
          _theGeomSelectionChanged = false;
        } else {
          clearSelection();
        }

      }
    }
  }
  return eltIndex;
}


QModelIndex PatternDataSelectionModel::_vtkSelectionChanged( const Handle(SALOME_InteractiveObject)& anIObject )
{
  QModelIndex anIOIndex;// the corresponding element of the model 
  bool fromVTK  = ( strcmp("HEXABLOCK", anIObject->getComponentDataType()) == 0 );
  std::cout << "_vtkSelectionChanged fromVTK  " << fromVTK << std::endl;

  if ( fromVTK ){
    if ( anIObject->hasEntry() ){
      SVTK_ViewWindow* currentVTKViewWindow = _getVTKViewWindow();
      QString         anIOEntry = anIObject->getEntry();
      QModelIndex     anIOIndex;
      Document_Actor *anDocActor = NULL;
      int anhexaElemsId;

      QString aText = "";
//       if ( GetNameOfSelectedElements( currentVTKViewWindow, anIObject, aText ) <= 0 ) return;
      if ( GetNameOfSelectedElements( currentVTKViewWindow, anIObject, aText ) > 0 ){
        //       GetNameOfSelectedNodes(activeViewWindow, anIObject, aText)
        //       QStringList aListId = theNewText.split(" ", QString::SkipEmptyParts);
        anDocActor = dynamic_cast<Document_Actor*>( findActorByEntry( currentVTKViewWindow, anIOEntry.toLatin1() ) );
        if ( anDocActor != NULL ){
          anhexaElemsId = anDocActor->hexaElemsId[ aText.toInt() ];
          anIOEntry = QString::number( anhexaElemsId );
          anIOIndex = _indexOf( anIOEntry, HEXA_ENTRY_ROLE );
    
          if ( !_theModelSelectionChanged && anIOIndex.isValid() ){ // select in model
            _theVtkSelectionChanged = true;
            //       select( newIndex, QItemSelectionModel::ClearAndSelect );
            //       select( newIndex, QItemSelectionModel::SelectCurrent );
            setCurrentIndex( anIOIndex, QItemSelectionModel::SelectCurrent );
            setCurrentIndex( anIOIndex, QItemSelectionModel::SelectCurrent );
            std::cout << "setCurrentIndex => "<< anIOIndex.data().toString().toStdString() << std::endl;
            _theVtkSelectionChanged = false;
          } else {
            clearSelection();
          }
        }
      }
    }
  }

  return anIOIndex;
}






// SUIT_DataOwnerPtrList aList;
//   ObjectList::iterator anIter;
//   for ( anIter = objects.begin(); anIter != objects.end(); ++anIter )
//   {
//     QString anEntry = getEntry( *anIter );
//     LightApp_DataOwner* anOwher = new LightApp_DataOwner( anEntry );
//     aList.append( anOwher );
//   }
// 
//   SUIT_Session* session = SUIT_Session::session();
//   SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( session->activeApplication() );
//   if ( !app )
//     return false;
// 
//   LightApp_SelectionMgr* aMgr = app->selectionMgr();
//   if ( !aMgr )
//     return false;
// _getVTKViewWindow()
//   aMgr->setSelecte_getVTKViewWindow()d( aList, false );





//   SMESH_Actor* anActor = SMESH::FindActorByObject(myMesh);
//   if (!anActor || !anActor->hasIO())
//     return;
// 
//   Handle(SALOME_InteractiveObject) anIO = anActor->getIO();
//   //mySelectionMgr->clearSelected();
//   //mySelectionMgr->AddIObject(anIO, false);
//   SALOME_ListIO aList;
//   aList.Append(anIO);
//   mySelectionMgr->setSelectedObjects(aList, false);
// 
//   // Remove filter corresponding to the current type from viewer
//   int aType = myTable->GetType();
//   int aFilterId = SMESH::UnknownFilter;
//   if      (aType == SMESH::EDGE  ) aFilterId = SMESH::EdgeFilter;
//   else if (aType == SMESH::FACE  ) aFilterId = SMESH::FaceFilter;
//   else if (aType == SMESH::VOLUME) aFilterId = SMESH::VolumeFilter;
//   Handle(VTKViewer_Filter) aFilter = SMESH::GetFilter(aFilterId);
//   SMESH::RemoveFilter(aFilterId);
// 
//   // get vtk ids
//   TColStd_MapOfInteger aMap;
//   QList<int>::const_iterator anIter;
//   for (anIter = theIds.begin(); anIter != theIds.end(); ++anIter) {
//     aMap.Add(*anIter);
//   }
// 
//   // Set new selection
//   activeViewWindow->AddOrRemoveIndex(anIO, aMap, false);
//   if ( SVTK_ViewWindow* aViewWindow = SMESH::GetViewWindow( mySMESHGUI ))
//     aViewWindow->highlight( anIO, true, true );
// 
//   // insert previously stored filter in viewer if necessary
//   if (!aFilter.IsNull())
//     SMESH::SetFilter(aFilter);









//     //CS_TEST
//  if(SVTK_Selector* aSelector = aView->GetSelector()){
//             const SALOME_ListIO& aListIO = aSelector->StoredIObjects();
//             SALOME_ListIteratorOfListIO anIter(aListIO);
//             for(; anIter.More(); anIter.Next()){
//               Handle(SALOME_InteractiveObject) anIO = anIter.Value();
//     _PTR(Study) aStudy  = HEXABLOCKGUI::activeStudy()->studyDS();
//     _PTR(SObject) aSObj = aStudy->FindObjectID( anEntry.toStdString().c_str() );
//     LightApp_DataObject*      o = HEXABLOCKGUI::activeStudy()->findObjectByEntry(anEntry.toStdString().c_str());
//     //CS_TEST





// void PatternDataSelectionModel::test()
// {
//   if (myBusy) return;
//   myMeshActor = 0;
// 
//   SALOME_ListIO aList;
//   mySelectionMgr->selectedObjects(aList,SVTK_Viewer::Type());
// 
//   if (aList.Extent() == 1) {
//     Handle(SALOME_InteractiveObject) anIO = aList.First();
//     myMeshActor = SMESH::FindActorByEntry(anIO->getEntry());
//     if(myMeshActor){
//       QString aText;
//       if (SMESH::GetNameOfSelectedNodes(activeViewWindow,anIO,aText) == 1) {
//         if(SMDS_Mesh* aMesh = myMeshActor->GetObject()->GetMesh()) {
//           if(const SMDS_MeshNode* aNode = aMesh->FindNode(aText.toInt())) {
//             myBusy = true;
//             myId->setText(aText);
//             myX->SetValue(aNode->X());
//             myY->SetValue(aNode->Y());
//             myZ->SetValue(aNode->Z());
//             myBusy = false;
//             erasePreview(); // avoid overlapping of a selection and a preview
//             updateButtons();
//             return;
//           }
//         }
//       }
//     }
//   }
// 
//   reset();
// }

// SVTK_ViewWindow* PatternDataSelectionModel::GetViewWindow()
// {
//     SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>
//         (SUIT_Session::session()->activeApplication());
//     if (anApp) {
//       if (SVTK_ViewWindow* aView = dynamic_cast<SVTK_ViewWindow*>(anApp->desktop()->activeWindow()))
//         return aView;
// 
// //       SUIT_ViewManager* aViewManager =
// //         anApp->getViewManager(SVTK_Viewer::Type(), createIfNotFound);
// //       if (aViewManager) {
// //         if (SUIT_ViewWindow* aViewWindow = aViewManager->getActiveView()) {
// //           if (SVTK_ViewWindow* aView = dynamic_cast<SVTK_ViewWindow*>(aViewWindow)) {
// //             aViewWindow->raise();
// //             aViewWindow->setFocus();
// //             return aView;
// //           }
// //         }
// //       }
//     }
//     return NULL;
// }




// void  PatternDataSelectionModel::SetSelectionMode(Selection_Mode theMode)
// {
// 
//   QList<SUIT_Selector*> aSelectors;
//   _salomeSelectionMgr->selectors( SVTK_Viewer::Type(), aSelectors );
//   QListIterator<SUIT_Selector*> it( aSelectors );
//   
//   std::cout << "PatternDataSelectionModel::SetSelectionMode()" << std::endl;
//   while ( it.hasNext() )
//   {
//   //   SUIT_Selector* selector = it.next();
//     SVTK_Selector* selector = dynamic_cast<SVTK_Selector*>( it.next() );
//     if ( selector ){
//       std::cout << "PatternDataSelectionModel::SetSelectionMode()" << theMode << std::endl;
//       selector->SetSelectionMode(theMode);
//     }
//   }
// }

// LightApp_SelectionMgr* PatternDataSelectionModel::selectionMgr()
// {
//   SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
//   if( anApp )
//     return dynamic_cast<LightApp_SelectionMgr*>( anApp->selectionMgr() );
//   else
//     return 0;
// }

// //CS_TEST
// SUIT_DataOwnerPtrList aOList;
// LightApp_DataOwner* anOwher = new LightApp_DataOwner( "0:1:1:1:2" );
// aOList.append( anOwher );
// _salomeSelectionMgr->setSelected( aOList, false );
// //CS_TEST