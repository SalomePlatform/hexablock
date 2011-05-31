//  Copyright (C) 2006-2010  CEA/DEN, EDF R&D
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
_salomeSelectionChanged(false)
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
//  if(SVTK_Selector* aSelector = aView->GetSelector()){
//             const SALOME_ListIO& aListIO = aSelector->StoredIObjects();
//             SALOME_ListIteratorOfListIO anIter(aListIO);
//             for(; anIter.More(); anIter.Next()){
//               Handle(SALOME_InteractiveObject) anIO = anIter.Value();
  SALOME_ListIO salomeSelected; 
  _salomeSelectionMgr->selectedObjects( salomeSelected, SVTK_Viewer::Type() );
  std::cout << " salomeSelected.Extent() => " << salomeSelected.Extent();

  Handle(SALOME_InteractiveObject) anIObject;
  QString anEntry;
  QString aText = "";
  int hexaElemsId;
  QModelIndexList indexes;
  QModelIndex     newIndex;

  SALOME_ListIteratorOfListIO it(salomeSelected);
  for( ; it.More(); it.Next()) {
//     Handle(SALOME_InteractiveObject) anIObject = it.Value();
    anIObject = it.Value();
    std::cout << anIObject->getName() << std::endl;
//     std::cout << "Salome selection => " <<anIObject->getEntry() << std::endl;
//       GetNameOfSelectedNodes(activeViewWindow, anIObject, aText)
    if ( GetNameOfSelectedElements( activeViewWindow, anIObject, aText ) > 0 ){
//       QStringList aListId = theNewText.split(" ", QString::SkipEmptyParts);
      hexaElemsId = Document_Actor::hexaElemsId[ aText.toInt() ];
      anEntry     = QString::number( hexaElemsId );
      std::cout << "entry from VTK elems => "<< anEntry.toStdString() << std::endl;
      indexes = model()->match(
            model()->index(0, 0),
            HEXA_ENTRY_ROLE,
            anEntry,
            1,
            Qt::MatchRecursive );
    } else if ( anIObject->hasEntry() ){
      anEntry = anIObject->getEntry();
      std::cout << "entry => "<< anEntry.toStdString() << std::endl;
      indexes = model()->match(
            model()->index(0, 0),
            HEXA_ENTRY_ROLE,
            anEntry,
            1,
            Qt::MatchRecursive);
    }
    if ( indexes.count()>0 ){
      std::cout << "Salome selection found!"<< std::endl;
      newIndex = indexes[0];
      std::cout << "data -> " << newIndex.data().toString().toStdString();
      _salomeSelectionChanged = true;
//       select( newIndex, QItemSelectionModel::ClearAndSelect );
//       select( newIndex, QItemSelectionModel::SelectCurrent );
      setCurrentIndex( newIndex, QItemSelectionModel::SelectCurrent );
      setCurrentIndex( newIndex, QItemSelectionModel::SelectCurrent );
//       setCurrentIndex( newIndex, QItemSelectionModel::ToggleCurrent );
      
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
//     //CS_TEST
//     _PTR(Study) aStudy  = HEXABLOCKGUI::activeStudy()->studyDS();
//     _PTR(SObject) aSObj = aStudy->FindObjectID( anEntry.toStdString().c_str() );
// 
//     LightApp_DataObject*      o = HEXABLOCKGUI::activeStudy()->findObjectByEntry(anEntry.toStdString().c_str());
//     HEXABLOCKGUI_DataObject* o2 = dynamic_cast<HEXABLOCKGUI_DataObject*>(o);
//     if (aSObj){
//       std::cout << "Sobj found!!    Sobj found!!    Sobj found!!" << std::endl;
//     }
//     if (o){
//       std::cout << "LightApp_DataObject found!!    LightApp_DataObject found!!    LightApp_DataObject found!!" << std::endl;
//     }
//     if (o2){
//       std::cout << "HEXABLOCKGUI_DataObject found!!    HEXABLOCKGUI_DataObject found!!   HEXABLOCKGUI_DataObject found!!" << std::endl;
//     }
//     //CS_TEST

  }
//   _salomeSelectionChanged = false;

}






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

  QStandardItem  *item = 0;
  const QSortFilterProxyModel *pmodel = dynamic_cast<const QSortFilterProxyModel *>( index.model() );
  if ( pmodel ){
//       std::cout << "pmodel" << std::endl;
      const QStandardItemModel *model = dynamic_cast<const QStandardItemModel *>( pmodel->sourceModel() );
      if ( model ){
//         std::cout << "model" << std::endl;
        item = model->itemFromIndex( pmodel->mapToSource(index) );
      }
  } else {
      const QStandardItemModel *model = dynamic_cast<const QStandardItemModel *>( index.model() );
      if ( model ){
//         std::cout << "model" << std::endl;
        item = model->itemFromIndex(index);
      }
  }

  if ( item ){
//       std::cout << "item" << std::endl;
      switch ( item->type() ){
        case VERTEXITEM : activeViewWindow->SetSelectionMode(NodeSelection);     break;
        case EDGEITEM :   activeViewWindow->SetSelectionMode(EdgeSelection);     break;
        case QUADITEM :   activeViewWindow->SetSelectionMode(FaceSelection);     break;
        case HEXAITEM :   activeViewWindow->SetSelectionMode(VolumeSelection);   break;
        default: break;
      }
  }

//  NodeSelection,
//  CellSelection,
//  EdgeOfCellSelection,
//  EdgeSelection,
//  FaceSelection,
//  VolumeSelection,
//  ActorSelection };



//   std::cout << "Try to select in Salome ->  " << index.model()->data(index).toString().toStdString() << std::endl;
  QVariant eltVariant = index.model()->data( index, HEXA_ENTRY_ROLE).toString();
  QVariant docVariant = index.model()->data( index, HEXA_DOC_ENTRY_ROLE).toString();

  if ( !eltVariant.isValid() ) return;
  if ( !docVariant.isValid() ) return;

  QString eltEntry = eltVariant.toString();
  QString docEntry = docVariant.toString();

  SALOME_Actor* docActor = DocumentGraphicView::FindActorByEntry( activeViewWindow, docEntry.toLatin1() ); //CS_TODO : mettre en commun
// SALOME_Actor* docActor = DocumentGraphicView::FindActorByEntry( aViewWindow, "0:1:1:1" );//CS_TODO
  if ( docActor ){
    std::cout<<"yes found an Actor for " << docEntry.toStdString() << std::endl;
    SALOME_ListIO aList;
    Handle(SALOME_InteractiveObject) anIO = docActor->getIO();
    aList.Append(anIO);
    _salomeSelectionMgr->setSelectedObjects( aList, true ); //CS_BP false?
    std::cout << "selected in Salome! document entry ->" << docEntry.toStdString() << std::endl;

    // get vtk ids
    TColStd_MapOfInteger aMap;
    QList<int>::const_iterator anIter;
    int vtkElemsId = Document_Actor::vtkElemsId[ eltEntry.toInt() ];
    std::cout << "Selected in Salome! vtk id ->" << vtkElemsId << std::endl;
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
//   std::cout << "PatternDataSelectionModel::onSelectionChanged" << std::endl;
  if ( _salomeSelectionChanged ) return;

  _salomeSelectionMgr->clearSelected();
  QModelIndexList indexes = selected.indexes();
//   std::cout << "indexes.count()" << indexes.count() << std::endl;
  for( QModelIndexList::const_iterator i_index = indexes.begin(); i_index != indexes.end(); ++i_index )
    _selectSalome( *i_index );

 // CS_BP todo SALOMEGUI_Swig.cxx:370
//   indexes = deselected.indexes();
//   for( QModelIndexList::const_iterator i_index = indexes.begin(); i_index != indexes.end(); ++i_index )
//     _unselectSalome( *i_index);
}

