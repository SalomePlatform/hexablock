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


#include "HEXABLOCKGUI_SalomeTools.hxx"
#include "HEXABLOCKGUI_DocumentSelectionModel.hxx"
#include "HEXABLOCKGUI_DocumentModel.hxx"
#include "HEXABLOCKGUI_DocumentGraphicView.hxx"
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



PatternDataSelectionModel::PatternDataSelectionModel( QAbstractItemModel * model ):
QItemSelectionModel( model ),
MyGEOMBase_Helper( SUIT_Session::session()->activeApplication()->desktop() ),
_theModelSelectionChanged(false),
_theVtkSelectionChanged(false),
_theGeomSelectionChanged(false),
_selectionFilter(-1),
_salomeSelectionMgr(0)
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

//   if ( _salomeSelectionMgr )
//     disconnect( _salomeSelectionMgr, SIGNAL( currentSelectionChanged() ), this, SLOT( salomeSelectionChanged() ) );
  disconnect( HEXABLOCKGUI::selectionMgr(), SIGNAL( currentSelectionChanged() ), this, SLOT( salomeSelectionChanged() ) );
}


void PatternDataSelectionModel::setVertexSelection()
{
  MESSAGE("PatternDataSelectionModel::setVertexSelection(){");
  SetSelectionMode(NodeSelection);
// //  NodeSelection,
// //  CellSelection,
// //  EdgeOfCellSelection,
// //  EdgeSelection,
// //  FaceSelection,
// //  VolumeSelection,
// //  ActorSelection };
  _selectionFilter = VERTEX_TREE;
  MESSAGE("}");
}

void PatternDataSelectionModel::setEdgeSelection()
{
  MESSAGE("PatternDataSelectionModel::setEdgeSelection(){");
  SetSelectionMode(EdgeSelection);
  _selectionFilter = EDGE_TREE;
  MESSAGE("}");
}

void PatternDataSelectionModel::setQuadSelection()
{
  MESSAGE("PatternDataSelectionModel::setQuadSelection(){");
  SetSelectionMode(FaceSelection);
  _selectionFilter = QUAD_TREE;
  MESSAGE("}");
}

void PatternDataSelectionModel::setHexaSelection()
{
  MESSAGE("PatternDataSelectionModel::setHexaSelection(){");
  SetSelectionMode(VolumeSelection);
  _selectionFilter = HEXA_TREE;
  MESSAGE("}");
}


void PatternDataSelectionModel::setAllSelection()
{
  MESSAGE("PatternDataSelectionModel::setAllSelection(){");
  _selectionFilter = -1;
  MESSAGE("}");
}


QModelIndex PatternDataSelectionModel::indexBy( int role, const QString& value )
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


QModelIndex PatternDataSelectionModel::indexBy( int role, const QVariant& var )
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


void PatternDataSelectionModel::setSalomeSelectionMgr( LightApp_SelectionMgr* mgr )
{
  _salomeSelectionMgr = mgr;
  connect( _salomeSelectionMgr, SIGNAL( currentSelectionChanged() ), this, SLOT( salomeSelectionChanged() ) );
}


void  PatternDataSelectionModel::SetSelectionMode(Selection_Mode theMode)
{
  MESSAGE("PatternDataSelectionModel::SetSelectionMode(){");
  SVTK_ViewWindow* aVTKViewWindow = _getVTKViewWindow();
//   aViewWindow->clearFilters();
//   _salomeSelectionMgr->clearFilters();
  if ( aVTKViewWindow )
    aVTKViewWindow->SetSelectionMode( theMode );
  MESSAGE("}");
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
  MESSAGE("PatternDataSelectionModel::onCurrentChanged(){");
  MESSAGE("*  current  : " << current.data().toString().toStdString());
  MESSAGE("*  previous : " << previous.data().toString().toStdString());
//   _selectSalome( current, true );
//   _selectSalome( previous, false );
  MESSAGE("}");
}



void PatternDataSelectionModel::onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected )
{
  MESSAGE("PatternDataSelectionModel::onSelectionChanged(){");
  foreach( const QModelIndex& isel, selected.indexes() ){
    MESSAGE("*  selected : " << isel.data().toString().toStdString());
  }
  foreach( const QModelIndex& iunsel, deselected.indexes() ){
    MESSAGE("*  unselected : " << iunsel.data().toString().toStdString());
  }

  _theModelSelectionChanged = true;
  try {
    if ( _salomeSelectionMgr == NULL ) return;
//     if ( !_theVtkSelectionChanged and  !_theGeomSelectionChanged ) return;

    _salomeSelectionMgr->clearSelected();
    //   erasePreview(true);
    QModelIndexList indexes = selected.indexes();
    for( QModelIndexList::const_iterator i_index = indexes.begin(); i_index != indexes.end(); ++i_index ){
  //     std::cout << "entry selected" << i_index->data( HEXA_ENTRY_ROLE ).toString().toStdString() << std::endl;
      if ( !_theVtkSelectionChanged )  _selectVTK( *i_index );
      if ( !_theGeomSelectionChanged ) _highlightGEOM( *i_index );
    }
    // CS_BP todo SALOMEGUI_Swig.cxx:370
    //   indexes = deselected.indexes();
    //   for( QModelIndexList::const_iterator i_index = indexes.begin(); i_index != indexes.end(); ++i_index )
    //     _unselectSalome( *i_index);
  } catch ( ... ) {
    MESSAGE("Unknown exception was cought !!!");
  }
  _theModelSelectionChanged = false;

  MESSAGE("}");
}


void PatternDataSelectionModel::salomeSelectionChanged()
{
  MESSAGE("PatternDataSelectionModel::salomeSelectionChanged(){");
  try {
    QModelIndex toSelect;

    if ( _salomeSelectionMgr == NULL ) return;
    SALOME_ListIO salomeSelected;
  //   _salomeSelectionMgr->selectedObjects( salomeSelected, SVTK_Viewer::Type() );//salomeSelected.Extent()
    _salomeSelectionMgr->selectedObjects( salomeSelected, NULL, false );
    if ( salomeSelected.IsEmpty() ){
      MESSAGE("*  salomeSelected.IsEmpty()");
      clearSelection();
      return;
    }

    Handle(SALOME_InteractiveObject) anIObject;
    SALOME_ListIteratorOfListIO it(salomeSelected);
    for( ; it.More(); it.Next()){
      anIObject = it.Value(); //anIObject->getName()
      toSelect = _geomSelectionChanged( anIObject );// is it comming from GEOM?
      if ( !toSelect.isValid() ){
        toSelect = _vtkSelectionChanged( anIObject ); ;// or VTK?...
        if ( toSelect.isValid() )
          MESSAGE("*  OK : selection from VTK");
      } else {
        MESSAGE("*  OK : selection from GEOM");
      }
    }
  } catch ( ... ) {
    MESSAGE("*  Unknown exception was cought !!!");
  }
  MESSAGE("}");
}

SVTK_ViewWindow* PatternDataSelectionModel::_getVTKViewWindow()
{
  SVTK_ViewWindow* aVtkView = HEXABLOCKGUI::currentVtkView;
  return aVtkView;
}


OCCViewer_ViewWindow*  PatternDataSelectionModel::_getOCCViewWindow()
{
  OCCViewer_ViewWindow* aOccView = HEXABLOCKGUI::currentOccView;
  return aOccView;
}


QModelIndex PatternDataSelectionModel::_indexOf( const QString& anEntry, int role )
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


void PatternDataSelectionModel::_setVTKSelectionMode( const QModelIndex& eltIndex, SVTK_ViewWindow* vtkViewWindow )
{
  MESSAGE("PatternDataSelectionModel::_setVTKSelectionMode( "<< eltIndex.data().toString().toStdString() << " ," << vtkViewWindow << " )");
  QVariant treeVariant = eltIndex.data( HEXA_TREE_ROLE );
  if ( !treeVariant.isValid() ) return;
  int eltType = treeVariant.toInt();

  if ( (_selectionFilter != -1) and ( _selectionFilter != eltType ) ) return;

  switch ( eltType ){
    case VERTEX_TREE :
    case VERTEX_DIR_TREE : vtkViewWindow->SetSelectionMode(NodeSelection); MESSAGE("VERTEX"); break;
    case EDGE_TREE :
    case EDGE_DIR_TREE :   vtkViewWindow->SetSelectionMode(EdgeSelection); MESSAGE("EDGE");     break;
    case QUAD_TREE :
    case QUAD_DIR_TREE :   vtkViewWindow->SetSelectionMode(FaceSelection); MESSAGE("QUAD");     break;
    case HEXA_TREE :
    case HEXA_DIR_TREE :   vtkViewWindow->SetSelectionMode(VolumeSelection); MESSAGE("HEXA");   break;
    case PROPAGATION_TREE :
    case PROPAGATION_DIR_TREE :   vtkViewWindow->SetSelectionMode(EdgeSelection); MESSAGE("PROPAGATION");   break;
//  CellSelection,
//  EdgeOfCellSelection,
//  VolumeSelection,
//  ActorSelection
  }
}




// 1 vertex -> 1 point
// 1 edge   -> n lines + points(deb,fin)
// 1 quad   -> n faces
void PatternDataSelectionModel::_highlightGEOM( const QMultiMap<QString, int>&  entrySubIDs )
{
  MESSAGE("PatternDataSelectionModel::_highlightGEOM( const QMultiMap<QString, int>&  entrySubIDs ){");

  OCCViewer_ViewWindow*  occView = _getOCCViewWindow();
  if ( occView == NULL ) return;
  SOCC_Viewer* soccViewer = dynamic_cast<SOCC_Viewer*>( occView->getViewManager()->getViewModel() ); 
  if (!soccViewer) return;
  _PTR(Study) aStudy = GetActiveStudyDocument();
  if (!aStudy) return;

  CORBA::Object_var     aCorbaObj = CORBA::Object::_nil();
  GEOM::GEOM_Object_var aGeomObj  = GEOM::GEOM_Object::_nil();
  erasePreview(true);
  foreach ( QString entry, entrySubIDs.keys() ){
    _PTR(SObject) aSChild = aStudy->FindObjectID( entry.toStdString() );
    MESSAGE("*    entry => "<< entry.toStdString());
    aCorbaObj = corbaObj( aSChild );
    aGeomObj = GEOM::GEOM_Object::_narrow( aCorbaObj );
    if ( !CORBA::is_nil(aGeomObj) ){
      MESSAGE("*  !CORBA::is_nil(aGeomObj)");
      QString objIOR = GEOMBase::GetIORFromObject( aGeomObj._retn() );
      Handle(GEOM_AISShape) aSh = GEOMBase::ConvertIORinGEOMAISShape( objIOR );//, true );
      if ( !aSh.IsNull() ){
        MESSAGE("*  !aSh.IsNull() ");
        TColStd_IndexedMapOfInteger anIndexes;
        foreach ( int subid, entrySubIDs.values(entry) ){
          if ( subid != -1 ) 
            anIndexes.Add( subid );
        }
        if ( anIndexes.Extent() > 0 ){ // if it's a sub-shape
          MESSAGE("*  a sub-shape");
          aSh->highlightSubShapes( anIndexes, true );
          soccViewer->Repaint();
        } else {  // or a main shape
//             std::cout << "aSh->getIO() => " << aSh->getIO() << std::endl;
//           getDisplayer()->SetDisplayMode(0);
//           soccViewer->setColor( aSh->getIO(), QColor( Qt::red ), true );
//           soccViewer->switchRepresentation( aSh->getIO(), 2 );
          MESSAGE("*  a main shape");
//           globalSelection();
          soccViewer->highlight( aSh->getIO(), true, true );
        }
      }
    }
  }

  MESSAGE("}");
}


void PatternDataSelectionModel::_highlightGEOM( const QModelIndex & anEltIndex )
{
  MESSAGE("PatternDataSelectionModel::_highlightGEOM(" << anEltIndex.data().toString().toStdString() << ")");
  // getting association(s) from model
  QList<DocumentModel::GeomObj> assocs;
  QMultiMap< QString, int >     assocEntrySubIDs;
  DocumentModel               *docModel = NULL;
  const QSortFilterProxyModel *pModel   = dynamic_cast<const QSortFilterProxyModel *>( model() );

  if ( !pModel ) return;
  docModel = dynamic_cast<DocumentModel*>( pModel->sourceModel() ); 
  if ( !docModel ) return;
  assocs = docModel->getAssociations( pModel->mapToSource(anEltIndex) );
  foreach( const DocumentModel::GeomObj& anAssoc, assocs )
    assocEntrySubIDs.insert( anAssoc.entry, anAssoc.subid.toInt() );

  _highlightGEOM( assocEntrySubIDs );
  MESSAGE("}");
}



void PatternDataSelectionModel::_selectVTK( const QModelIndex& eltIndex )
{
  MESSAGE("PatternDataSelectionModel::_selectVTK( "<< eltIndex.data().toString().toStdString() << ")");
  SVTK_ViewWindow* currentVTKViewWindow = _getVTKViewWindow();
  if ( currentVTKViewWindow == NULL ) return;
  SVTK_Selector* selector = currentVTKViewWindow->GetSelector();
  if ( selector == NULL ) return;

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
  QVariant entryVariant    = eltIndex.data( HEXA_ENTRY_ROLE );
  QVariant docEntryVariant = eltIndex.data( HEXA_DOC_ENTRY_ROLE );

  if ( !entryVariant.isValid() ){
    //INFOS("entryVariant not valid");
    return;
  }
  if ( !docEntryVariant.isValid() ){
    //INFOS("docEntryVariant not valid");
    return;
  }

  eltEntry = entryVariant.toString();
  docEntry = docEntryVariant.toString();

  // Select the document in Salome
  docActor = dynamic_cast<Document_Actor*>( findActorByEntry( currentVTKViewWindow, docEntry.toLatin1() ) );
  if ( docActor == NULL ) return;

  // Set selection mode in VTK view
  _setVTKSelectionMode( eltIndex, currentVTKViewWindow );

// if ( _salomeSelectionMgr == NULL ) return;
//   _salomeSelectionMgr->selectedObjects( aList );
//   std::cout<<"aList.Extent() => " << aList.Extent() << std::endl;
//   aList.Append(docIO);
//   _salomeSelectionMgr->setSelectedObjects( aList, true );//false );//true ); //CS_BP false?
  docIO = docActor->getIO();
 
  // Highlight in vtk view the element from document 
  vtkElemsId = docActor->vtkElemsId[ eltEntry.toInt() ];// get vtk ids from actor
  if ( vtkElemsId > 0 ) // CS_BP ?: erreur si 1er elt == vertex (0,0,0)
    aMap.Add( vtkElemsId );
//     for (anIter = theIds.begin(); anIter != theIds.end(); ++anIter) {
//       aMap.Add(*anIter);
//     }
  selector->AddOrRemoveIndex( docIO, aMap, false );
  currentVTKViewWindow->highlight( docIO, true, true );

  MESSAGE("}");
}

QModelIndex PatternDataSelectionModel::_geomSelectionChanged( const Handle(SALOME_InteractiveObject)& anIObject )
{
  MESSAGE("PatternDataSelectionModel::_geomSelectionChanged(){");
  QModelIndex eltIndex;// the element of the model which is associated to the geom object and that is to be selected

  bool fromGEOM = ( strcmp("GEOM", anIObject->getComponentDataType()) == 0 );
  if ( !fromGEOM ) return eltIndex;
  if (!_salomeSelectionMgr) return eltIndex;

  QString aName;
  GEOM::GEOM_Object_var aGeomObj = GEOMBase::ConvertIOinGEOMObject( anIObject );

  int anIndex = -1;
  if ( GEOMBase::IsShape(aGeomObj) ){
    aName = GEOMBase::GetName(aGeomObj);
    TColStd_IndexedMapOfInteger anIndexes;
    _salomeSelectionMgr->GetIndexes(anIObject, anIndexes);
    if ( anIndexes.Extent() == 1 )
      anIndex = anIndexes(1);
      QString aGeomObjStudyEntry = aGeomObj->GetStudyEntry();
      QString aGeomObjModelEntry = aGeomObjStudyEntry + "," + QString::number(anIndex) + ";";
      eltIndex = _indexOf( aGeomObjModelEntry, HEXA_ASSOC_ENTRY_ROLE );
      if ( !_theModelSelectionChanged && eltIndex.isValid() ){ // select in model
        _theGeomSelectionChanged = true;
        //       select( newIndex, QItemSelectionModel::ClearAndSelect );
        //       select( newIndex, QItemSelectionModel::SelectCurrent );
//         setCurrentIndex( eltIndex, QItemSelectionModel::SelectCurrent );
        setCurrentIndex( eltIndex, QItemSelectionModel::Clear );  //CS_TEST
        setCurrentIndex( eltIndex, QItemSelectionModel::SelectCurrent );
        _theGeomSelectionChanged = false;
      } else {
        clearSelection();
      }
    }

  MESSAGE("}");
  return eltIndex;
}

QModelIndex PatternDataSelectionModel::_vtkSelectionChanged( const Handle(SALOME_InteractiveObject)& anIObject )
{
  MESSAGE("PatternDataSelectionModel::_vtkSelectionChanged(){");
  QModelIndex anIOIndex;// // the element of the model which is associated to the Interactive object and that is to be selected

  bool fromVTK  = ( strcmp("HEXABLOCK", anIObject->getComponentDataType()) == 0 );
  if ( !fromVTK ) return anIOIndex;
  SVTK_ViewWindow* currentVTKViewWindow = _getVTKViewWindow();
  if ( !currentVTKViewWindow )  return anIOIndex;
  if ( !anIObject->hasEntry() ) return anIOIndex;
  
  QString anIOEntry = anIObject->getEntry();
  Document_Actor *anDocActor = NULL;
  int anhexaElemsId;

  QString aText = "";
  if ( GetNameOfSelectedElements( currentVTKViewWindow, anIObject, aText ) <= 0 ) return anIOIndex;
  anDocActor = dynamic_cast<Document_Actor*>( findActorByEntry( currentVTKViewWindow, anIOEntry.toLatin1() ) );
  if ( !anDocActor ) return anIOIndex;
  anhexaElemsId = anDocActor->hexaElemsId[ aText.toInt() ];
  anIOEntry = QString::number( anhexaElemsId );
  anIOIndex = _indexOf( anIOEntry, HEXA_ENTRY_ROLE );

  if ( !_theModelSelectionChanged && anIOIndex.isValid() ){ // select in model
    _theVtkSelectionChanged = true;
//     select( anIOIndex, QItemSelectionModel::Clear );
//     select( anIOIndex, QItemSelectionModel::Select );
//     select( anIOIndex, QItemSelectionModel::ClearAndSelect );
//     select( anIOIndex, QItemSelectionModel::SelectCurrent );
//     setCurrentIndex( anIOIndex, QItemSelectionModel::SelectCurrent );
//     setCurrentIndex( anIOIndex, QItemSelectionModel::SelectCurrent );
//     setCurrentIndex( anIOIndex, QItemSelectionModel::ClearAndSelect );
//     setCurrentIndex( anIOIndex, QItemSelectionModel::ClearAndSelect );
    setCurrentIndex( anIOIndex, QItemSelectionModel::Clear );
    setCurrentIndex( anIOIndex, QItemSelectionModel::SelectCurrent );
    _theVtkSelectionChanged = false;
  } else {
    clearSelection();
  }

  MESSAGE("}");
  return anIOIndex;
}







GroupsSelectionModel::GroupsSelectionModel( QAbstractItemModel * model ):
QItemSelectionModel( model )
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




QModelIndex GroupsSelectionModel::indexBy( int role, const QVariant& var )
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


SVTK_ViewWindow* GroupsSelectionModel::_getVTKViewWindow()
{
  SVTK_ViewWindow* aVtkView = HEXABLOCKGUI::currentVtkView;
  return aVtkView;
}


void GroupsSelectionModel::_highlightGroups( const QModelIndex& eltIndex )
{
  const GroupsModel* m = dynamic_cast<const GroupsModel *>( model() );
  
// ---- VTK ----
  if ( m == NULL ) return;
  SVTK_ViewWindow* currentVTKViewWindow = _getVTKViewWindow();
  if ( currentVTKViewWindow == NULL ) return;
  SVTK_Selector* selector = currentVTKViewWindow->GetSelector();
  if ( selector == NULL ) return;

// ----  vtkActor
  // document selection
  Document_Actor* docActor = NULL;
  Handle(SALOME_InteractiveObject) docIO;
  SALOME_ListIO aList;

  // element highlight
  TColStd_MapOfInteger aMap;
  QList<int>::const_iterator anIter;
  int vtkElemsId;


  
  // debut ** data from model
  int     eltType;
  QString docEntry;

  QVariant treeVariant        = eltIndex.data( HEXA_TREE_ROLE );
  QVariant docEntryVariant    = eltIndex.data( HEXA_DOC_ENTRY_ROLE );

  if ( !treeVariant.isValid() || !docEntryVariant.isValid() ){ 
    //INFOS("data from model not valid");
    return;
  }

  eltType  = treeVariant.toInt();
  docEntry = docEntryVariant.toString();

  if ( eltType != GROUP_TREE ){
    //INFOS("bad element type : not a group item" << eltType );
    return;
  }
  // fin ** data from model

  // Select the document in Salome
  docActor = dynamic_cast<Document_Actor*>( findActorByEntry( currentVTKViewWindow, docEntry.toLatin1() ) );
  if ( docActor == NULL) return;
  docIO = docActor->getIO();

  // Highlight in vtk view the element from document
  DocumentModel::Group kind;
  QModelIndexList iElements = m->getGroupElements( eltIndex, kind );

  // Set selection mode in VTK view
  switch (kind){
  case HEXA_NS::HexaCell: case HEXA_NS::HexaNode: currentVTKViewWindow->SetSelectionMode(VolumeSelection); break;
  case HEXA_NS::QuadCell: case HEXA_NS::QuadNode: currentVTKViewWindow->SetSelectionMode(FaceSelection);   break;
  case HEXA_NS::EdgeCell: case HEXA_NS::EdgeNode: currentVTKViewWindow->SetSelectionMode(EdgeSelection);   break;
  case HEXA_NS::VertexNode: currentVTKViewWindow->SetSelectionMode(NodeSelection); break;
  }

  QString eltEntry;
  foreach( const QModelIndex& iElt, iElements ){
    eltEntry = iElt.data( HEXA_ENTRY_ROLE ).toString();
    vtkElemsId = docActor->vtkElemsId[ eltEntry.toInt() ];
    if ( vtkElemsId > 0 ) aMap.Add( vtkElemsId );
  }

  selector->AddOrRemoveIndex( docIO, aMap, false );
  currentVTKViewWindow->highlight( docIO, true, true );

}

/*
void GroupsSelectionModel::_highlightAssoc( const QModelIndex& eltIndex )
{
  const GroupsModel* m = dynamic_cast<const GroupsModel *>( model() );
  
// ---- VTK ----
  if ( m == NULL ) return;
  SVTK_ViewWindow* currentVTKViewWindow = _getVTKViewWindow();
  if ( currentVTKViewWindow == NULL ) return;
  SVTK_Selector* selector = currentVTKViewWindow->GetSelector();
  if ( selector == NULL ) return;

// ----  vtkActor
  // document selection
  Document_Actor* docActor = NULL;
  Handle(SALOME_InteractiveObject) docIO;
  SALOME_ListIO aList;

  // element highlight
  TColStd_MapOfInteger aMap;
  QList<int>::const_iterator anIter;
  int vtkElemsId;


  
  // debut ** data from model
  int     eltType;
  QString docEntry;

  QVariant treeVariant        = eltIndex.data( HEXA_TREE_ROLE );
  QVariant docEntryVariant    = eltIndex.data( HEXA_DOC_ENTRY_ROLE );

  if ( !treeVariant.isValid() || !docEntryVariant.isValid() ){ 
    //INFOS("data from model not valid");
    return;
  }

  eltType  = treeVariant.toInt();
  docEntry = docEntryVariant.toString();
 ){
    //INFOS("bad element typ
  if ( eltType != GROUP_TREE ){
    //INFOS("bad element type : not a group item" << eltType );
    return;
  }
  // fin ** data from model

  // Select the document in Salome
  docActor = dynamic_cast<Document_Actor*>( findActorByEntry( currentVTKViewWindow, docEntry.toLatin1() ) );
  if ( docActor == NULL) return;
  docIO = docActor->getIO();

  // Highlight in vtk view the element from document
  DocumentModel::Group kind;
  QModelIndexList iElements = m->getGroupElements( eltIndex, kind );

  // Set selection mode in VTK view
/*
  switch (kind){
  case HEXA_NS::HexaCell: case HEXA_NS::HexaNode: currentVTKViewWindow->SetSelectionMode(VolumeSelection); break;
  case HEXA_NS::QuadCell: case HEXA_NS::QuadNode: currentVTKViewWindow->SetSelectionMode(FaceSelection);   break;
  case HEXA_NS::EdgeCell: case HEXA_NS::EdgeNode: currentVTKViewWindow->SetSelectionMode(EdgeSelection);   break;
  case HEXA_NS::VertexNode: currentVTKViewWindow->SetSelectionMode(NodeSelection); break;
  }---/

  QString eltEntry;
  foreach( const QModelIndex& iElt, iElements ){
    eltEntry = iElt.data( HEXA_ENTRY_ROLE ).toString();
    vtkElemsId = docActor->vtkElemsId[ eltEntry.toInt() ];
    if() 
      if ( vtkElemsId > 0 ) aMap.Add( vtkElemsId );
  }

  selector->AddOrRemoveIndex( docIO, aMap, false );
  currentVTKViewWindow->highlight( docIO, true, true );

}*/



void GroupsSelectionModel::onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected )
{
  MESSAGE("GroupsSelectionModel::onSelectionChanged");
  try {
    //     if ( _salomeSelectionMgr == NULL ) return;
    //     _salomeSelectionMgr->clearSelected();
    //   erasePreview(true);
    QModelIndexList indexes = selected.indexes();
    for( QModelIndexList::const_iterator i_index = indexes.begin(); i_index != indexes.end(); ++i_index ){
      MESSAGE( "entry selected" << i_index->data( HEXA_ENTRY_ROLE ).toString().toStdString() );
      _highlightGroups( *i_index );
    }

    // CS_BP todo SALOMEGUI_Swig.cxx:370
    //   indexes = deselected.indexes();
    //   for( QModelIndexList::const_iterator i_index = indexes.begin(); i_index != indexes.end(); ++i_index )
    //     _unselectSalome( *i_index);
  } catch ( ... ) {
    MESSAGE("Unknown exception was cought !!!");
  }

}







MeshSelectionModel::MeshSelectionModel( QAbstractItemModel * model ):
QItemSelectionModel( model )
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


QModelIndex MeshSelectionModel::indexBy( int role, const QVariant& var )
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



SVTK_ViewWindow* MeshSelectionModel::_getVTKViewWindow()
{
  SVTK_ViewWindow* aVtkView = HEXABLOCKGUI::currentVtkView;
  return aVtkView;
}


void MeshSelectionModel::_highlightPropagation( const QModelIndex& eltIndex )
{
  const MeshModel* m = dynamic_cast<const MeshModel *>( model() );
  if ( m == NULL ) return;
  SVTK_ViewWindow* currentVTKViewWindow = _getVTKViewWindow();
  if ( currentVTKViewWindow == NULL ) return;
  SVTK_Selector* selector = currentVTKViewWindow->GetSelector();
  if ( selector == NULL ) return;

  // document selection
  Document_Actor* docActor = NULL;
  Handle(SALOME_InteractiveObject) docIO;
  SALOME_ListIO aList;

  // element highlight
  TColStd_MapOfInteger aMap;
  QList<int>::const_iterator anIter;
  int vtkElemsId;

  // data from model
  int     eltType;
  QString docEntry;

  QVariant treeVariant        = eltIndex.data( HEXA_TREE_ROLE );
  QVariant docEntryVariant    = eltIndex.data( HEXA_DOC_ENTRY_ROLE );

  if ( !treeVariant.isValid() || !docEntryVariant.isValid() ){ 
    //INFOS("data from model not valid");
    return;
  }

  eltType  = treeVariant.toInt();
  docEntry = docEntryVariant.toString();

  if ( eltType != PROPAGATION_TREE ){
    //INFOS("bad element type : not a propagation item" << eltType );
    return;
  }

  // Select the document in Salome
  docActor = dynamic_cast<Document_Actor*>( findActorByEntry( currentVTKViewWindow, docEntry.toLatin1() ) );
  if ( docActor == NULL) return;

//   // Set selection mode in VTK view
  currentVTKViewWindow->SetSelectionMode(EdgeSelection); 
  docIO = docActor->getIO();
 
  // Highlight in vtk view the element from document 
  QModelIndexList iEdges = m->getPropagation( eltIndex );

  QString edgeEntry;
  foreach( const QModelIndex& iEdge, iEdges ){
    edgeEntry = iEdge.data( HEXA_ENTRY_ROLE ).toString();
    vtkElemsId = docActor->vtkElemsId[ edgeEntry.toInt() ];
    if ( vtkElemsId > 0 ) aMap.Add( vtkElemsId );
  }

  selector->AddOrRemoveIndex( docIO, aMap, false );
  currentVTKViewWindow->highlight( docIO, true, true );
}



void MeshSelectionModel::onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected )
{
  MESSAGE("MeshSelectionModel::onSelectionChanged");
  try {
    //     if ( _salomeSelectionMgr == NULL ) return;
    //     _salomeSelectionMgr->clearSelected();
    //   erasePreview(true);
    QModelIndexList indexes = selected.indexes();
    for( QModelIndexList::const_iterator i_index = indexes.begin(); i_index != indexes.end(); ++i_index ){
      MESSAGE( "entry selected" << i_index->data( HEXA_ENTRY_ROLE ).toString().toStdString() );
      _highlightPropagation( *i_index );
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


/*

void PatternDataSelectionModel::setGeomEngine( GEOM::GEOM_Gen_var geomEngine )
{gaumont parnasse
  _geomEngine = geomEngine;
}

GEOM::GEOM_IOperations_ptr PatternDataSelectionModel::createOperation()
{
//   return myGeomGUI->GetGeomGen()->GetIBasicOperations(getStudyId());
  return _geomEngine->GetIBasicOperations(getStudyId());
}

bool PatternDataSelectionModel::execute(ObjectList& objects)
{
  bool res = false;

  _PTR(Study)   aStudy = GetActiveStudyDocument();
  _PTR(SObject) aSChild;
  CORBA::Object_var aCorbaObj = CORBA::Object::_nil();
  GEOM::GEOM_Object_var assoc;

  foreach( const DocumentModel::GeomObj& anAssoc, _assocList ){
    std::cout << "FOUND=> " << anAssoc.entry.toStdString() << std::endl;
    aSChild = aStudy->FindObjectID( anAssoc.entry.toStdString() );
    aCorbaObj = corbaObj(aSChild);
    assoc = GEOM::GEOM_Object::_narrow(aCorbaObj);

    if ( !CORBA::is_nil(assoc) ){
      std::cout << "geom to highlight =>" << anAssoc.name.toStdString() << std::endl;
      objects.push_back( assoc._retn() );
      res = true;
    } else {
      std::cout << "not a geom =>" << anAssoc.name.toStdString()<< std::endl;
    }
  }

  return res;
}
*/


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

// void PatternDataSelectionModel::_highlightGEOM( const QModelIndex & anEltIndex )
// {
//   std::cout << "PatternDataSelectionModel::_highlightGEOM go find ASSOCIATION for"<< anEltIndex.data().toString().toStdString()<<std::endl;
// // HEXABLOCKGUI::currentVtkView
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
