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

#include <set>
#include <map>


#include "utilities.h"

#include <QtxWorkstack.h>
#include <STD_TabDesktop.h>
#include <STD_MDIDesktop.h>
#include <STD_SDIDesktop.h>

#include <SalomeApp_Tools.h>

#include "SOCC_ViewModel.h"
#include <OCCViewer_ViewManager.h>
#include <OCCViewer_ViewModel.h>
#include <OCCViewer_ViewWindow.h>

#include <SUIT_OverrideCursor.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Session.h>
#include <SUIT_Selector.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewManager.h>
#include <SVTK_View.h>

#include "HEXABLOCKGUI_VtkDocumentGraphicView.hxx"
#include "HEXABLOCKGUI_OccGraphicView.hxx"
#include "HEXABLOCKGUI_SalomeTools.hxx"
#include "HEXABLOCKGUI_DocumentSelectionModel.hxx"
#include "HEXABLOCKGUI_DocumentModel.hxx"
#include "HEXABLOCKGUI_DocumentItem.hxx"
#include "HEXABLOCKGUI.hxx"


#include <SVTK_Selector.h>
#include <SVTK_ViewModel.h>


#include <SalomeApp_Study.h>
#include <SalomeApp_Application.h>
#include <SALOME_Actor.h>
#include <SALOME_ListIO.hxx>
#include <SALOME_ListIteratorOfListIO.hxx>

#include <QMultiMap>

#include "GEOMBase.h"
#include "MyGEOMBase_Helper.hxx"

#include <OCCViewer_ViewModel.h>
#include <SVTK_ViewModel.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SALOME_ListIteratorOfListIO.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>


//#define _DEVDEBUG_
using namespace std;
using namespace HEXABLOCK::GUI;


// //===========================================================================
//                              SelectionModel
// //===========================================================================

SelectionModel::SelectionModel( QAbstractItemModel * model ):
QItemSelectionModel( model ),
//_theModelSelectionChanged(false),
//_theVtkSelectionChanged(false),
//_theGeomSelectionChanged(false),
salomeNothingSelected(true)
{
}


SelectionModel::~SelectionModel()
{
  disconnect( HEXABLOCKGUI::selectionMgr(), SIGNAL( currentSelectionChanged() ), this, SLOT( salomeSelectionChanged() ) );
}


QModelIndex SelectionModel::indexBy( int role, const QString& value )
{
  QModelIndex eltIndex; // element (vertex, edge, quad) of model
  const QAbstractItemModel* theModel = model();
  if ( !theModel ) return eltIndex;
  QModelIndexList theIndexes = theModel->match( theModel->index(0, 0),
                                          role,
                                          value,
                                          1,
                                          Qt::MatchRecursive | Qt::MatchContains );//Qt::MatchFixedString );
  if ( theIndexes.count()>0 )
    eltIndex = theIndexes[0] ;
  return eltIndex;
}


QModelIndex SelectionModel::indexBy( int role, const QVariant& var )
{
  QModelIndex eltIndex; // element (vertex, edge, quad) of model
  const QAbstractItemModel* theModel = model();
  if ( !theModel ) return eltIndex;
  QModelIndexList theIndexes = theModel->match( theModel->index(0, 0),
                                          role,
                                          var,
                                          1,
                                          Qt::MatchRecursive /*| Qt::MatchContains*/ );//Qt::MatchFixedString );
  if ( theIndexes.count()>0 )
    eltIndex = theIndexes[0] ;
  return eltIndex;
}

QModelIndex SelectionModel::indexOf( const QString& anEntry, int role )
{
  QModelIndex eltIndex; // element (vertex, edge, quad) of model
  const QAbstractItemModel* theModel = model();
  if ( !theModel ) return eltIndex;

  QModelIndexList theIndexes = theModel->match( theModel->index(0, 0),
                                          role,
                                          anEntry,
                                          1,
                                          Qt::MatchRecursive | Qt::MatchContains );//Qt::MatchFixedString );
  if ( theIndexes.count()>0 )
    eltIndex = theIndexes[0] ;
  return eltIndex;
}


QModelIndexList SelectionModel::indexListOf( const QString& anEntry, int role )
{
  QModelIndexList theIndexes; // element (vertex, edge, quad) of model
  const QAbstractItemModel* theModel = model();
  if ( !theModel ) return theIndexes;
  theIndexes = theModel->match( theModel->index(0, 0),
                                          role,
                                          anEntry,
                                          -1,
                                          Qt::MatchRecursive | Qt::MatchContains );//Qt::MatchFixedString );
  return theIndexes;
}



void SelectionModel::salomeSelectionChanged()
{
  try {

    if ( HEXABLOCKGUI::selectionMgr() == NULL ) return;
    SALOME_ListIO salomeSelected;
  //   _salomeSelectionMgr->selectedObjects( salomeSelected, SVTK_Viewer::Type() );//salomeSelected.Extent()
//    if (HEXABLOCKGUI::getActiveViewType() == HEXABLOCKGUI::VTK)
//        HEXABLOCKGUI::selectionMgr()->selectedObjects( salomeSelected, SVTK_Viewer::Type());
//    else if (HEXABLOCKGUI::getActiveViewType() == HEXABLOCKGUI::OCC)
//        HEXABLOCKGUI::selectionMgr()->selectedObjects( salomeSelected, OCCViewer_Viewer::Type() );

    HEXABLOCKGUI::selectionMgr()->selectedObjects( salomeSelected, NULL, false );
    if ( salomeSelected.IsEmpty() ){
      salomeNothingSelected = true;
      clearSelection();
      return;
    }

    Handle(SALOME_InteractiveObject) anIObject;
    SALOME_ListIteratorOfListIO it(salomeSelected);

    //Handle selection according to the source
    if (HEXABLOCKGUI::getActiveViewType() == HEXABLOCKGUI::VTK)
    {
        for( ; it.More(); it.Next()){
            anIObject = it.Value(); //anIObject->getName()
            vtkSelectionChanged( anIObject ); // or VTK?...
        }
    }
    else if (HEXABLOCKGUI::getActiveViewType() == HEXABLOCKGUI::OCC)
    {
        for( ; it.More(); it.Next()){
            anIObject = it.Value(); //anIObject->getName()
            geomSelectionChanged( anIObject );// or VTK?...
        }
    }

    salomeNothingSelected = false;
  } catch ( ... ) {
      MESSAGE("*  Unknown exception was cought !!!");
  }
}


QModelIndexList SelectionModel::getSelectionFromModel(const Handle(SALOME_InteractiveObject)& anIObject)
{
   QModelIndexList selectedIndexes;

   //verify if the IOBject is valid and from VTK selection
   bool fromVTK  = ( strcmp("HEXABLOCK", anIObject->getComponentDataType()) == 0 );
   if ( !fromVTK || !anIObject->hasEntry() || HEXABLOCKGUI::currentDocGView->getViewWindow() == NULL)
      return selectedIndexes;

   QString anIOEntry = anIObject->getEntry();
   int anhexaElemsId;
   QString aText = "";

   //extract vtk selection from the model -----------
   if ( GetNameOfSelectedElements( HEXABLOCKGUI::currentDocGView->getViewWindow(), anIObject, aText ) <= 0 )
      return selectedIndexes;

   Document_Actor* docActor = dynamic_cast<Document_Actor*>( findActorByEntry( HEXABLOCKGUI::currentDocGView->getViewWindow(),
                                                                                       anIOEntry.toLatin1() ) );
   if ( !docActor ) return selectedIndexes;
   QStringList idList = aText.split(" ");
   foreach( const QString& id, idList )
   {
      if (!id.isEmpty())
         {
            //find selection in the model
            anhexaElemsId = docActor->hexaElemsId[ id.toInt() ];
            anIOEntry = QString::number( anhexaElemsId );

            selectedIndexes << indexOf( anIOEntry, HEXA_ENTRY_ROLE );
         }
   }
   return selectedIndexes;
}


//Returns the element of the model which is associated to the Interactive object
QModelIndexList SelectionModel::getSelectionAssociactions(const Handle(SALOME_InteractiveObject)& anIObject)
{
   QModelIndexList assocsIndexes;

   //verify if the selection is really from OCC View
   bool fromGEOM = ( strcmp("GEOM", anIObject->getComponentDataType()) == 0 );
   if ( !fromGEOM || HEXABLOCKGUI::selectionMgr() == NULL ||
         HEXABLOCKGUI::currentOccGView->getViewWindow() == NULL)
      return assocsIndexes;

//   QString aName;
   GEOM::GEOM_Object_var aGeomObj = GEOMBase::ConvertIOinGEOMObject( anIObject );

   //extract associated elements in the model
   if ( GEOMBase::IsShape(aGeomObj) ){
//         aName = GEOMBase::GetName(aGeomObj);
         TColStd_IndexedMapOfInteger anIndexes;
         HEXABLOCKGUI::selectionMgr()->GetIndexes(anIObject, anIndexes);
         if (anIndexes.Extent() == 0) return assocsIndexes;

         QString aGeomObjStudyEntry = aGeomObj->GetStudyEntry();
         for( int i = 1; i <= anIndexes.Extent(); ++i)
            {
               QString aGeomObjModelEntry = aGeomObjStudyEntry + "," + QString::number(anIndexes(i)) + ";";
               assocsIndexes.append(indexListOf(aGeomObjModelEntry, HEXA_ASSOC_ENTRY_ROLE));
               if (assocsIndexes.count() > 0)
                  return assocsIndexes;    //Premature: On purpose for Monoselection
            }
   }

   return assocsIndexes;
}


// //===========================================================================
//                              PatternDataSelectionModel
// //===========================================================================

PatternDataSelectionModel::PatternDataSelectionModel( QAbstractItemModel * model ):
SelectionModel( model )
{
  connect( this, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
           this, SLOT( onCurrentChanged( const QModelIndex & , const QModelIndex & ) ) );
  connect( this, SIGNAL( selectionChanged( const QItemSelection & , const QItemSelection & ) ),
           this, SLOT( onSelectionChanged( const QItemSelection & , const QItemSelection & ) ) );

}

PatternDataSelectionModel::~PatternDataSelectionModel()
{
  disconnect( this, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
          this, SLOT( onCurrentChanged( const QModelIndex & , const QModelIndex & ) ) );
  disconnect( this, SIGNAL( selectionChanged( const QItemSelection & , const QItemSelection & ) ),
          this, SLOT( onSelectionChanged( const QItemSelection & , const QItemSelection & ) ) );
}


void PatternDataSelectionModel::onCurrentChanged( const QModelIndex & current, const QModelIndex & previous )
{
  //Setting the selection mode of the selected item from the treeview
  HEXABLOCKGUI::currentDocGView->setSelectionMode( current );
  HEXABLOCKGUI::currentOccGView->setSelectionMode( current );

}

void PatternDataSelectionModel::highlightEltsWithAssocs(const QModelIndexList& elts)
{
	try {
	    /*if ( !_theVtkSelectionChanged )*/
	    HEXABLOCKGUI::currentDocGView->highlight(elts);
	    /*if ( !_theGeomSelectionChanged )*/
	    HEXABLOCKGUI::currentOccGView->highlight(elts);
	} catch ( ... ) {
		MESSAGE("Unknown exception was cought !!!");
	}
}

void PatternDataSelectionModel::onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected )
{
//    _theModelSelectionChanged = true;
    QModelIndexList indexes = selected.indexes();
    highlightEltsWithAssocs(indexes);
//    _theModelSelectionChanged = false;
}


void PatternDataSelectionModel::geomSelectionChanged( const Handle(SALOME_InteractiveObject)& anIObject )
{
   if (HEXABLOCKGUI::assocInProgress) return;

   QModelIndexList assocsIndexes = getSelectionAssociactions(anIObject);

   if (assocsIndexes.count() == 0)
      {
         //No association found
         clearSelection();
         return;
      }

   //selection of the associated elements
   if (assocsIndexes.count() == 1 &&  assocsIndexes[0].isValid())
      {
         //Select associated element in the model => highlight in vtk view
         setCurrentIndex( assocsIndexes[0], QItemSelectionModel::Clear );  //CS_TEST
         setCurrentIndex( assocsIndexes[0], QItemSelectionModel::SelectCurrent );
      }
   else //more than one element associated: just highlight them in the vtk view
         HEXABLOCKGUI::currentDocGView->highlight(assocsIndexes);

}


void PatternDataSelectionModel::vtkSelectionChanged( const Handle(SALOME_InteractiveObject)& anIObject )
{

   QModelIndexList selectedIndexes =  getSelectionFromModel(anIObject);
   if (selectedIndexes.count() == 0)
      {
         clearSelection();
         return;
      }
   //    _theVtkSelectionChanged = true;
   QModelIndex anIOIndex = selectedIndexes[0]; //Monoselection

   //Temporary Debug for hexa selection -----------------------------
   if (HEXABLOCKGUI::currentDocGView->getSelectionMode() == HEXA_TREE)
      {
         DocumentModel               *docModel = NULL;
         const QSortFilterProxyModel *pModel   = dynamic_cast<const QSortFilterProxyModel *>( model() );

         if ( pModel != NULL){
               docModel = dynamic_cast<DocumentModel*>( pModel->sourceModel() );
               if ( docModel != NULL && anIOIndex.isValid())
                  {
                     //get the selected quad
                     HEXA_NS::Quad* quad = docModel->getHexaPtr<HEXA_NS::Quad*>(anIOIndex);

                     if (quad != NULL)
                        {
                           //get the hexa the quad belongs to
                           HEXA_NS::Hexa* hexa = docModel->getQuadHexa(quad);
                           if (hexa != NULL) //convert the hexa to a QModelIndex so we can select it in the model
                              anIOIndex = indexBy( HEXA_DATA_ROLE, QVariant::fromValue(hexa));
                           else
                              {
                                 SUIT_MessageBox::critical( 0,
                                       tr("HexaBlock"),
                                       tr("The Hexahedron this quad belongs to has been deleted!"));
                                 return;
                              }
                        }
                  }
         }
      }//end if HEXA_TREE----------------------------------------------

   //select the element in the model
   setCurrentIndex( anIOIndex, QItemSelectionModel::Clear );
   setCurrentIndex( anIOIndex, QItemSelectionModel::SelectCurrent );
   //    _theVtkSelectionChanged = false;

}


// //===========================================================================
//                              GroupsSelectionModel
// //===========================================================================

GroupsSelectionModel::GroupsSelectionModel( QAbstractItemModel * model ):
SelectionModel( model )
{
  connect( this, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
           this, SLOT( onCurrentChanged( const QModelIndex & , const QModelIndex & ) ) );
  connect( this, SIGNAL( selectionChanged( const QItemSelection & , const QItemSelection & ) ),
           this, SLOT( onSelectionChanged( const QItemSelection & , const QItemSelection & ) ) );
}

GroupsSelectionModel::~GroupsSelectionModel()
{
  disconnect( this, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
          this, SLOT( onCurrentChanged( const QModelIndex & , const QModelIndex & ) ) );
  disconnect( this, SIGNAL( selectionChanged( const QItemSelection & , const QItemSelection & ) ),
          this, SLOT( onSelectionChanged( const QItemSelection & , const QItemSelection & ) ) );
}


void GroupsSelectionModel::onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected )
{
  try {
    //     if ( _salomeSelectionMgr == NULL ) return;
    //     _salomeSelectionMgr->clearSelected();
    //   erasePreview(true);
    QModelIndexList indexes = selected.indexes();
    for( QModelIndexList::const_iterator i_index = indexes.begin(); i_index != indexes.end(); ++i_index ){
      HEXABLOCKGUI::currentDocGView->highlightGroups( *i_index );
    }

    // CS_BP todo SALOMEGUI_Swig.cxx:370
    //   indexes = deselected.indexes();
    //   for( QModelIndexList::const_iterator i_index = indexes.begin(); i_index != indexes.end(); ++i_index )
    //     _unselectSalome( *i_index);
  } catch ( ... ) {
    MESSAGE("Unknown exception was cought !!!");
  }

}


// //===========================================================================
//                              MeshSelectionModel
// //===========================================================================


MeshSelectionModel::MeshSelectionModel( QAbstractItemModel * model ):
SelectionModel( model )
{
  connect( this, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
           this, SLOT( onCurrentChanged( const QModelIndex & , const QModelIndex & ) ) );
  connect( this, SIGNAL( selectionChanged( const QItemSelection & , const QItemSelection & ) ),
           this, SLOT( onSelectionChanged( const QItemSelection & , const QItemSelection & ) ) );
}


MeshSelectionModel::~MeshSelectionModel()
{
  disconnect( this, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
          this, SLOT( onCurrentChanged( const QModelIndex & , const QModelIndex & ) ) );
  disconnect( this, SIGNAL( selectionChanged( const QItemSelection & , const QItemSelection & ) ),
          this, SLOT( onSelectionChanged( const QItemSelection & , const QItemSelection & ) ) );
}


void MeshSelectionModel::onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected )
{
  try {

    QModelIndexList indexes = selected.indexes();
    for( QModelIndexList::const_iterator i_index = indexes.begin(); i_index != indexes.end(); ++i_index ){
      HEXABLOCKGUI::currentDocGView->highlightPropagation( *i_index );
    }

    // CS_BP todo SALOMEGUI_Swig.cxx:370
    //   indexes = deselected.indexes();
    //   for( QModelIndexList::const_iterator i_index = indexes.begin(); i_index != indexes.end(); ++i_index )
    //     _unselectSalome( *i_index);
  } catch ( ... ) {
    MESSAGE("Unknown exception was cought !!!");
  }

}









// //=================================================================================
// // function : activateSelection
// // purpose  : Activate selection in accordance with myEditCurrentArgument
// //=================================================================================
// void PatternDataSelectionModel::activateSelection()
// {
//   erasePreview(false);
//
//   // local selection
//   if (!myObject->_is_nil() && !isAllSubShapes())
//   {
//     GEOM_Displayer* aDisplayer = getDisplayer();
//     SALOME_View* view = GEOM_Displayer::GetActiveView();
//     if (view) {
//       CORBA::String_var aMainEntry = myObject->GetStudyEntry();
//       Handle(SALOME_InteractiveObject) io = new SALOME_InteractiveObject (aMainEntry.in(), "GEOM", "TEMP_IO");
//       if (view->isVisible(io)) {
//         aDisplayer->Erase(myObject, false, false);
//         myIsHiddenMain = true;
//       }
//     }
//
//     int prevDisplayMode = aDisplayer->SetDisplayMode(0);
//
//     SUIT_ViewWindow* aViewWindow = 0;
//     SUIT_Study* activeStudy = SUIT_Session::session()->activeApplication()->activeStudy();
//     if (activeStudy)
//       aViewWindow = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
//     if (aViewWindow == 0) return;
//
//     SUIT_ViewManager* aViewManager = aViewWindow->getViewManager();
//     if (aViewManager->getType() != OCCViewer_Viewer::Type() &&
//         aViewManager->getType() != SVTK_Viewer::Type())
//       return;
//
//     SUIT_ViewModel* aViewModel = aViewManager->getViewModel();
//     SALOME_View* aView = dynamic_cast<SALOME_View*>(aViewModel);
//     if (aView == 0) return;
//
//     //TopoDS_Shape aMainShape = GEOM_Client::get_client().GetShape(GeometryGUI::GetGeomGen(), myObject);
//
//     TopTools_IndexedMapOfShape aSubShapesMap;
//     TopExp::MapShapes(myShape, aSubShapesMap);
//     CORBA::String_var aMainEntry = myObject->GetStudyEntry();
//     QString anEntryBase = aMainEntry.in();
//
//     TopExp_Explorer anExp (myShape, (TopAbs_ShapeEnum)shapeType());
//     for (; anExp.More(); anExp.Next())
//     {
//       TopoDS_Shape aSubShape = anExp.Current();
//       int index = aSubShapesMap.FindIndex(aSubShape);
//       QString anEntry = anEntryBase + QString("_%1").arg(index);
//
//       SALOME_Prs* aPrs = aDisplayer->buildSubshapePresentation(aSubShape, anEntry, aView);
//       if (aPrs) {
//         displayPreview(aPrs, true, false); // append, do not update
//       }
//     }
//     aDisplayer->UpdateViewer();
//     aDisplayer->SetDisplayMode(prevDisplayMode);
//   }
//
//   globalSelection(GEOM_ALLSHAPES);
// }




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


// //CS_TEST
// SUIT_DataOwnerPtrList aOList;
// LightApp_DataOwner* anOwher = new LightApp_DataOwner( "0:1:1:1:2" );
// aOList.append( anOwher );
// _salomeSelectionMgr->setSelected( aOList, false );
// //CS_TEST

// void PatternDataSelectionModel::highlightGEOM( const QModelIndex & anEltIndex )
// {
//   std::cout << "PatternDataSelectionModel::highlightGEOM go find ASSOCIATION for"<< anEltIndex.data().toString().toStdString()<<std::endl;
// // HEXABLOCKGUI::currentDocGView->getViewWindow()
//
//   QList<DocumentModel::GeomObj> assocList;
//
//   DocumentModel               *docModel = NULL;
//   const QSortFilterProxyModel *pModel   = NULL;
// //   const QStandardItemModel    *smodel
// //   const QAbstractItemModel    *theModel = NULL;
//
//   pModel = dynamic_cast<const QSortFilterProxyModel *>( model() );
//
//   std::cout << "pModel "<< pModel << std::endl;
//   if ( pModel ){
//     std::cout << "if ( pModel ){"<< std::endl;
//     docModel = dynamic_cast<DocumentModel*>( pModel->sourceModel() );
//     if ( docModel ){
//       std::cout << "if ( docModel ){"<< std::endl;
//       assocList = docModel->getAssociations( pModel->mapToSource(anEltIndex) );
//     }
//   }
//
//
//   _PTR(Study)   aStudy = GetActiveStudyDocument();
//   _PTR(SObject) aSChild;
//   CORBA::Object_var aCorbaObj = CORBA::Object::_nil();
//   GEOM::GEOM_Object_var assoc;
// //   foreach( const QString& entry, geomEntries ){
// // struct GeomObj
// //         {
// //           QString name;
// //           QString entry;
// //           QString brep;
// //           double  start;
// //           double  end;
// //         };
//
//   QVariant treeVariant = pModel->mapToSource(anEltIndex).data( HEXA_TREE_ROLE );
//   int eltType;
//   if ( !treeVariant.isValid() ) return;
//   eltType = treeVariant.toInt();
//
// //   GEOM::GeomObjPtr
// //   GEOM::GEOM_Object_ptr firstLine;  //firstLine.nullify();//
//   GEOM::GEOM_Object_var firstLine = GEOM::GEOM_Object::_nil();  //GEOM::GeomObjPtr
//   GEOM::GEOM_Object_var lastLine  = GEOM::GEOM_Object::_nil();
//   double firstParameter = 0.2; //CS_TODO
//   double lastParameter  = 0.4; //CS_TODO
//
//   foreach( const DocumentModel::GeomObj& anAssoc, assocList ){
//     std::cout << "FOUND=> " << anAssoc.entry.toStdString() << std::endl;
//     aSChild = aStudy->FindObjectID( anAssoc.entry.toStdString() );
//     aCorbaObj = corbaObj(aSChild);
//     assoc = GEOM::GEOM_Object::_narrow(aCorbaObj);
//
//     if ( !CORBA::is_nil(assoc) ){
//       std::cout << "geom to highlight =>" << anAssoc.name.toStdString() << std::endl;
// //       objects.push_back(assoc._retn());
// //       if ( eltType == EDGE_TREE ){
// //           if ( CORBA::is_nil(firstLine) ){
// //             firstLine = GEOM::GEOM_Object::_duplicate( assoc._retn() );
// //           }
// //           lastLine = GEOM::GEOM_Object::_duplicate( assoc._retn() );
// //       }
//       displayPreview( assoc._retn(),
//                       true, //false, //append,
//                       false,//true, //false, //activate,
//                       false,//true,//update,
//                       4,//lineWidth,
//                       1,//-1,//displayMode,
//                       Quantity_NOC_RED );
//     } else {
//       std::cout << "not a geom =>" << anAssoc.name.toStdString()<< std::endl;
//     }
//   }
// //     std::cout << "CORBA::is_nil(firstLine) =>" << CORBA::is_nil(firstLine) << std::endl;
// //     std::cout << "CORBA::is_nil(lastLine) =>"  << CORBA::is_nil(lastLine) << std::endl;
// //     std::cout << "test" << ( !( CORBA::is_nil(firstLine) and !CORBA::is_nil(lastLine) ) ) << std::endl;
//  /*
//   if ( !( CORBA::is_nil(firstLine) and !CORBA::is_nil(lastLine) ) ){
//     GEOM::GEOM_IBasicOperations_var anOper = _geomEngine->GetIBasicOperations( getStudyId() );
//     GEOM::GEOM_Object_var firstPoint = anOper->MakePointOnCurve( firstLine, firstParameter );
//     GEOM::GEOM_Object_var lastPoint  = anOper->MakePointOnCurve( lastLine, lastParameter );
//
// //     std::cout << "firstPoint->_is_nil() =>" << firstPoint->_is_nil() << std::endl;
//     std::cout << "lastPoint->_is_nil() =>"  << lastPoint->_is_nil() << std::endl;
//     if ( !( CORBA::is_nil(firstPoint) ) )// !firstPoint->_is_nil() )
//         displayPreview( firstPoint._retn(), true );
//     if ( !( CORBA::is_nil(lastPoint) ) )//if ( !lastPoint->_is_nil() )
//         displayPreview( lastPoint._retn(), true );
//   }*/
// }
//
