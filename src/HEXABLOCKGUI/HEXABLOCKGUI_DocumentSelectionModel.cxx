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


// #include "SMESHGUI_VTKUtils.h" //CS_TODO : remove


#include <set>

#include "HEXABLOCKGUI_DocumentSelectionModel.hxx"
#include "HEXABLOCKGUI_DocumentGraphicView.hxx"
#include "HEXABLOCKGUI_DocumentItem.hxx"
#include "HEXABLOCKGUI.hxx"

#include "HEXABLOCKGUI_DataModel.hxx"
#include "HEXABLOCKGUI_DataObject.hxx"


// #include "HexVertex.hxx"


#include <SUIT_MessageBox.h>

#include <SUIT_Session.h>
#include <SUIT_Selector.h>
#include <SUIT_Desktop.h>
#include <SVTK_Selector.h>
#include <SVTK_ViewModel.h>


// #include <SalomeApp_DataObject.h>
#include <SalomeApp_Study.h>
// #include <SalomeApp_Module.h>
#include <SalomeApp_Application.h>
#include <SALOME_Actor.h>
#include <SALOME_ListIO.hxx>
#include <SALOME_ListIteratorOfListIO.hxx>


//#define _DEVDEBUG_
using namespace std;
using namespace HEXABLOCK::GUI;




SVTK_ViewWindow* GetActiveViewWindow()
{
    SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>
        (SUIT_Session::session()->activeApplication());
    if (anApp) {
      if (SVTK_ViewWindow* aView = dynamic_cast<SVTK_ViewWindow*>(anApp->desktop()->activeWindow()))
        return aView;
//       SUIT_ViewManager* aViewManager =
//         anApp->getViewManager(SVTK_Viewer::Type(), createIfNotFound);
//       if (aViewManager) {
//         if (SUIT_ViewWindow* aViewWindow = aViewManager->getActiveView()) {
//           if (SVTK_ViewWindow* aView = dynamic_cast<SVTK_ViewWindow*>(aViewWindow)) {
//             aViewWindow->raise();
//             aViewWindow->setFocus();
//             return aView;
//           }
//         }
//       }
    }
    return NULL;
}


int GetNameOfSelectedNodes( SVTK_ViewWindow *theWindow,
                            const Handle(SALOME_InteractiveObject)& theIO,
                            QString& theName )
{
    SVTK_Selector* theSelector = theWindow->GetSelector();
    theName = "";
    TColStd_IndexedMapOfInteger aMapIndex;
    theSelector->GetIndex(theIO,aMapIndex);

    for( int i = 1; i <= aMapIndex.Extent(); i++ )
      theName += QString(" %1").arg(aMapIndex(i));

    return aMapIndex.Extent();
 }

int GetNameOfSelectedElements( SVTK_ViewWindow *theWindow,/* SVTK_Selector* theSelector,*/
                               const Handle(SALOME_InteractiveObject)& theIO,
                               QString& theName )
{
    SVTK_Selector* theSelector = theWindow->GetSelector();
    theName = "";
    TColStd_IndexedMapOfInteger aMapIndex;
    theSelector->GetIndex(theIO,aMapIndex);

//     typedef std::set<int> TIdContainer;
    std::set<int> anIdContainer;
    for( int i = 1; i <= aMapIndex.Extent(); i++)
      anIdContainer.insert(aMapIndex(i));

    std::set<int>::const_iterator anIter = anIdContainer.begin();
    for( ; anIter != anIdContainer.end(); anIter++)
      theName += QString(" %1").arg(*anIter);

    std::cout << "GetNameOfSelectedElements name =>" << theName.toStdString() << std::endl;
    return aMapIndex.Extent();
}







PatternDataSelectionModel::PatternDataSelectionModel( QAbstractItemModel * model ):
QItemSelectionModel( model ),
_salomeSelectionChanged(false),
_selectionFilter(-1)
// _vertexSelectionOnly( false ),
// _edgeSelectionOnly( false ),
// _quadSelectionOnly( false ),
// _hexaSelectionOnly( false )
{
  _salomeSelectionMgr = PatternDataSelectionModel::selectionMgr();

  connect( _salomeSelectionMgr, SIGNAL( currentSelectionChanged() ), this, SLOT( salomeSelectionChanged() ) );
  connect( this, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
           this, SLOT( onCurrentChanged( const QModelIndex & , const QModelIndex & ) ) );
  connect( this, SIGNAL( selectionChanged( const QItemSelection & , const QItemSelection & ) ),
           this, SLOT( onSelectionChanged( const QItemSelection & , const QItemSelection & ) ) );

}




PatternDataSelectionModel::PatternDataSelectionModel( QAbstractItemModel * model, QObject * parent ):
QItemSelectionModel( model, parent ),
_salomeSelectionChanged(false)
{
  _salomeSelectionMgr = PatternDataSelectionModel::selectionMgr();

connect( _salomeSelectionMgr, SIGNAL( currentSelectionChanged() ), this, SLOT( salomeSelectionChanged() ) );
connect( this, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
         this, SLOT( onCurrentChanged( const QModelIndex & , const QModelIndex & ) ) );
connect( this, SIGNAL( selectionChanged( const QItemSelection & , const QItemSelection & ) ),
         this, SLOT( onSelectionChanged( const QItemSelection & , const QItemSelection & ) ) );

}

PatternDataSelectionModel::~PatternDataSelectionModel()
{
}

LightApp_SelectionMgr* PatternDataSelectionModel::selectionMgr()
{
  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  if( anApp )
    return dynamic_cast<LightApp_SelectionMgr*>( anApp->selectionMgr() );
  else
    return 0;
}


SVTK_ViewWindow* PatternDataSelectionModel::GetViewWindow ()
{
    SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>
        (SUIT_Session::session()->activeApplication());
    if (anApp) {
      if (SVTK_ViewWindow* aView = dynamic_cast<SVTK_ViewWindow*>(anApp->desktop()->activeWindow()))
        return aView;

//       SUIT_ViewManager* aViewManager =
//         anApp->getViewManager(SVTK_Viewer::Type(), createIfNotFound);
//       if (aViewManager) {
//         if (SUIT_ViewWindow* aViewWindow = aViewManager->getActiveView()) {
//           if (SVTK_ViewWindow* aView = dynamic_cast<SVTK_ViewWindow*>(aViewWindow)) {
//             aViewWindow->raise();
//             aViewWindow->setFocus();
//             return aView;
//           }
//         }
//       }
    }
    return NULL;
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


void  PatternDataSelectionModel::SetSelectionMode(Selection_Mode theMode)
{
  SVTK_ViewWindow* aViewWindow = PatternDataSelectionModel::GetViewWindow();
//   aViewWindow->clearFilters();
  _salomeSelectionMgr->clearFilters();
  aViewWindow->SetSelectionMode( theMode );

}


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



void PatternDataSelectionModel::salomeSelectionChanged()
{
//   std::cout << "PatternDataSelectionModel::salomeSelectionChanged()" << std::endl;
  SVTK_ViewWindow* activeViewWindow = GetActiveViewWindow();

  SALOME_ListIO salomeSelected; 
  _salomeSelectionMgr->selectedObjects( salomeSelected, SVTK_Viewer::Type() );
  std::cout << " salomeSelected.Extent() => " << salomeSelected.Extent();

  QString anEntry;
  QString aText = "";
  int hexaElemsId;
  QModelIndexList indexes;
  QModelIndex     newIndex;

  SALOME_ListIteratorOfListIO it(salomeSelected);
  Handle(SALOME_InteractiveObject) anIObject;

  for( ; it.More(); it.Next()) {
    anIObject = it.Value(); //anIObject->getName()
    if ( anIObject->hasEntry() ){
      anEntry = anIObject->getEntry();
      std::cout << "1) entry => "<< anEntry.toStdString() << std::endl;
      //       GetNameOfSelectedNodes(activeViewWindow, anIObject, aText)
      if ( GetNameOfSelectedElements( activeViewWindow, anIObject, aText ) > 0 ){
  //       QStringList aListId = theNewText.split(" ", QString::SkipEmptyParts);
        Document_Actor* docActor = dynamic_cast<Document_Actor*>( DocumentGraphicView::FindActorByEntry(activeViewWindow, anEntry.toLatin1()) );
        hexaElemsId = docActor->hexaElemsId[ aText.toInt() ];
        anEntry     = QString::number( hexaElemsId );
        std::cout << "2) entry from VTK elems => "<< anEntry.toStdString() << std::endl;
      }
      indexes = model()->match( model()->index(0, 0),
                                HEXA_ENTRY_ROLE,
                                anEntry,
                                1,
                                Qt::MatchRecursive );
      if ( indexes.count()>0 ){
        std::cout << "Salome selection found!"<< std::endl;
        newIndex = indexes[0];
        std::cout << "data -> " << newIndex.data().toString().toStdString();
        _salomeSelectionChanged = true;
  //       select( newIndex, QItemSelectionModel::ClearAndSelect );
  //       select( newIndex, QItemSelectionModel::SelectCurrent );
        setCurrentIndex( newIndex, QItemSelectionModel::SelectCurrent );
        setCurrentIndex( newIndex, QItemSelectionModel::SelectCurrent );
                                  // QItemSelectionModel::Current
                                  // QItemSelectionModel::Select 
                                  // QItemSelectionModel::SelectCurrent
                                  // QItemSelectionModel::ToggleCurrent
                                  // QItemSelectionModel::ClearAndSelect
  //       indexes.clear();
        _salomeSelectionChanged = false;
      } else {
          std::cout << "Salome selection not found!"<< std::endl;
      }
    } //if ( anIObject->hasEntry() ){
  } // for


//   _salomeSelectionChanged = false;

}

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





void PatternDataSelectionModel::_selectSalome( const QModelIndex & index )
{
//   std::cout << "PatternDataSelectionModel::_selectSalome " << std::endl;
  SVTK_ViewWindow* activeViewWindow = GetActiveViewWindow();

  QVariant treeVariant = index.data( HEXA_TREE_ROLE );
  QVariant eltVariant  = index.data( HEXA_ENTRY_ROLE);
  QVariant docVariant  = index.data( HEXA_DOC_ENTRY_ROLE);

  if ( !treeVariant.isValid() ) return;

  int     treeType = treeVariant.toInt();
  std::cout<<"treeType  =>" << treeType  << std::endl;

  if ( _selectionFilter != -1 ){
    if ( _selectionFilter != treeType  ){
      return;
    }
  }

  switch ( treeType ){
    case VERTEX_TREE : 
    case VERTEX_DIR_TREE : activeViewWindow->SetSelectionMode(NodeSelection);     break;
    case EDGE_TREE :
    case EDGE_DIR_TREE :   activeViewWindow->SetSelectionMode(EdgeSelection);     break;
    case QUAD_TREE :
    case QUAD_DIR_TREE :   activeViewWindow->SetSelectionMode(FaceSelection);     break;
    case HEXA_TREE :
    case HEXA_DIR_TREE :   activeViewWindow->SetSelectionMode(VolumeSelection);   break;
  }
//  NodeSelection,
//  CellSelection,
//  EdgeOfCellSelection,
//  EdgeSelection,
//  FaceSelection,
//  VolumeSelection,
//  ActorSelection };


  if ( !eltVariant.isValid() ) return;
  if ( !docVariant.isValid() ) return;

  QString eltEntry = eltVariant.toString();
  QString docEntry = docVariant.toString();

  Document_Actor* docActor = dynamic_cast<Document_Actor*>( DocumentGraphicView::FindActorByEntry( activeViewWindow, docEntry.toLatin1() ) );

  if ( docActor ){
    std::cout<<"yes found an Actor for " << docEntry.toStdString() << std::endl;
    SALOME_ListIO aList;
    Handle(SALOME_InteractiveObject) anIO = docActor->getIO();
    aList.Append(anIO);
    _salomeSelectionMgr->setSelectedObjects( aList, false ); //true ); //CS_BP false?
//     std::cout << "selected in Salome! document entry ->" << docEntry.toStdString() << std::endl;

    // get vtk ids
    TColStd_MapOfInteger aMap;
    QList<int>::const_iterator anIter;
    int vtkElemsId = docActor->vtkElemsId[ eltEntry.toInt() ];
//     std::cout << "Selected in Salome! vtk id ->" << vtkElemsId << std::endl;
    if ( vtkElemsId > 0 ) // CS_BP ?: erreur si 1er elt == vertex (0,0,0)
      aMap.Add( vtkElemsId );
//     for (anIter = theIds.begin(); anIter != theIds.end(); ++anIter) {
//       aMap.Add(*anIter);
//     }

    // Set new selection
    SVTK_Selector* selector = activeViewWindow->GetSelector();
    selector->AddOrRemoveIndex( anIO, aMap, false );
    activeViewWindow->highlight( anIO, true, true );

  } else {
    std::cout << "no docActor for " << docEntry.toStdString() << std::endl;
  }

  
}



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





void PatternDataSelectionModel::onCurrentChanged( const QModelIndex & current, const QModelIndex & previous )
{
//   std::cout << "PatternDataSelectionModel::onCurrentChanged " << std::endl;
//   _selectSalome( current, true );
//   _selectSalome( previous, false );
}


void PatternDataSelectionModel::onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected )
{
  std::cout << "******PatternDataSelectionModel::onSelectionChanged" << std::endl;
  if ( _salomeSelectionChanged ) return;

  _salomeSelectionMgr->clearSelected();
  QModelIndexList indexes = selected.indexes();
  std::cout << "indexes.count()" << indexes.count() << std::endl;
  for( QModelIndexList::const_iterator i_index = indexes.begin(); i_index != indexes.end(); ++i_index ){
    _selectSalome( *i_index );
    
  }
 // CS_BP todo SALOMEGUI_Swig.cxx:370
//   indexes = deselected.indexes();
//   for( QModelIndexList::const_iterator i_index = indexes.begin(); i_index != indexes.end(); ++i_index )
//     _unselectSalome( *i_index);
}

