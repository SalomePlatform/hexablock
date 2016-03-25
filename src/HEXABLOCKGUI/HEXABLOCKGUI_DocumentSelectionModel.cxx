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
#include "HEXABLOCKGUI_DocumentItem.hxx"
#include "HEXABLOCKGUI.hxx"


#include <SVTK_Selector.h>
#include <SVTK_ViewModel.h>


#include <SalomeApp_Study.h>
#include <SalomeApp_Application.h>
#include <SALOME_Actor.h>
#include <SALOME_ListIO.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <QMultiMap>

#include <OCCViewer_ViewModel.h>
#include <SVTK_ViewModel.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
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

bool SelectionModel::infoMode = true;

SelectionModel::SelectionModel( QAbstractItemModel * model ):
QItemSelectionModel( model )
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

DocumentModel* SelectionModel::getDocModel() const
{
    DocumentModel* docModel = NULL;
    const QSortFilterProxyModel *pModel   = dynamic_cast<const QSortFilterProxyModel *>( model() );
    if ( pModel != NULL)
        docModel = dynamic_cast<DocumentModel*>( pModel->sourceModel() );

    return docModel;
}

void SelectionModel::showEltInfo(QModelIndex& elt)
{
    DocumentModel* docModel = getDocModel();
    if (!elt.isValid() || docModel == NULL)
        return;

    HEXA_NS::Vertex* vertex  = docModel->getHexaPtr<HEXA_NS::Vertex*>(elt);
    HEXA_NS::Edge*   edge    = docModel->getHexaPtr<HEXA_NS::Edge*>(elt);
    HEXA_NS::Quad*   quad    = docModel->getHexaPtr<HEXA_NS::Quad*>(elt);
    HEXA_NS::Hexa*   hexa    = docModel->getHexaPtr<HEXA_NS::Hexa*>(elt);
    HEXA_NS::Vector* vector  = docModel->getHexaPtr<HEXA_NS::Vector*>(elt);
    HEXA_NS::Group*  group   = docModel->getHexaPtr<HEXA_NS::Group*>(elt);
    HEXA_NS::Law*    law     = docModel->getHexaPtr<HEXA_NS::Law*>(elt);
    HEXA_NS::Propagation* propagation = docModel->getHexaPtr<HEXA_NS::Propagation*>(elt);

    HEXABLOCKGUI*    hexagui = HEXABLOCKGUI::getInstance();
    if (vertex != NULL)
        hexagui->showVertexInfoDialog(vertex);
    else if (edge != NULL)
        hexagui->showEdgeInfoDialog(edge);
    else if (quad != NULL)
        hexagui->showQuadInfoDialog(quad);
    else if (hexa != NULL)
        hexagui->showHexaInfoDialog(hexa);
    else if (vector != NULL)
        hexagui->showVectorInfoDialog(vector);
    else if (group != NULL)
        hexagui->showGroupInfoDialog(group);
    else if (law != NULL)
        hexagui->showLawInfoDialog(law);
    else if (propagation != NULL)
        hexagui->showPropagationInfoDialog(propagation);
}

void SelectionModel::salomeSelectionChanged()
{
    // clear highlights and selections in the trees
    PatternDataSelectionModel* pdsm = HEXABLOCKGUI::currentDocGView->getPatternDataSelectionModel();
//    PatternBuilderSelectionModel* pbsm = HEXABLOCKGUI::currentDocGView->getPatternBuilderSelectionModel();
    PatternGeomSelectionModel* pgsm = HEXABLOCKGUI::currentDocGView->getPatternGeomSelectionModel();
    pdsm->clearSelection();
//    pbsm->clearSelection();
    pgsm->clearSelection();
    pdsm->unhighlightTreeItems();
    pgsm->unhighlightTreeItems();

    try {
        SALOME_ListIO selectedObjects;
        Handle(SALOME_InteractiveObject) anIObject;

        switch (HEXABLOCKGUI::getActiveViewType())
        {
        case HEXABLOCKGUI::VTK:
        {
            HEXABLOCKGUI::currentDocGView->getSelected(selectedObjects);
            if ( selectedObjects.IsEmpty() )
                return;
            anIObject = selectedObjects.First();
            vtkSelectionChanged( anIObject );
            break;
        }
        case HEXABLOCKGUI::OCC:
            HEXABLOCKGUI::currentOccGView->getSelected(selectedObjects);
            if ( selectedObjects.IsEmpty() )
                return;
            anIObject = selectedObjects.First();
            geomSelectionChanged( anIObject );
            break;
        }
    } catch ( ... ) {
        MESSAGE("*  Unknown selection exception!");
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

// //===========================================================================
//                              PatternDataSelectionModel
// //===========================================================================

PatternDataSelectionModel::PatternDataSelectionModel( QAbstractItemModel * model ):
SelectionModel( model )
{
  connect( this, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
           this, SLOT( onCurrentChanged( const QModelIndex & , const QModelIndex & ) ), Qt::UniqueConnection );
  connect( this, SIGNAL( selectionChanged( const QItemSelection & , const QItemSelection & ) ),
           this, SLOT( onSelectionChanged( const QItemSelection & , const QItemSelection & ) ), Qt::UniqueConnection );

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

void PatternDataSelectionModel::highlightTreeItems(QModelIndexList& indexes,
                                                   Qt::GlobalColor bgColor,
                                                   Qt::GlobalColor fgColor, bool only)
{
    if (indexes.count() == 0)
        return;

    if (only)
        unhighlightTreeItems();

    // * highlight item
    QAbstractItemModel* theModel = (QAbstractItemModel*) model();
    foreach( const QModelIndex& anItemIndex, indexes )
    {
        if (anItemIndex.isValid())
        {
            theModel->setData(anItemIndex, QColor(fgColor), Qt::ForegroundRole);
            theModel->setData(anItemIndex, QColor(bgColor), Qt::BackgroundRole);
            currentHighlightedItems << anItemIndex;
        }
    }

    // * scroll to the first highlighted item in the tree
    HEXABLOCKGUI* module = HEXABLOCKGUI::getInstance();
    if (module != NULL && indexes[0].isValid())
        module->getPatternDataTreeView()->scrollTo(indexes[0]);
}

void PatternDataSelectionModel::unhighlightTreeItems(bool clearSelected)
{
    if (clearSelected)
        clearSelection();
    if (currentHighlightedItems.count() == 0)
        return;

    QAbstractItemModel* theModel = (QAbstractItemModel*) model();
    foreach( const QModelIndex& anItemIndex, currentHighlightedItems)
    {
        if (anItemIndex.isValid())
        {
            theModel->setData(anItemIndex, QColor(Qt::darkGreen), Qt::ForegroundRole);
            theModel->setData(anItemIndex, QColor(Qt::white), Qt::BackgroundRole);
        }
    }
    currentHighlightedItems.clear();
}

QModelIndexList PatternDataSelectionModel::getGeomAssociations(const QModelIndex& dataIndex)
{
    QModelIndexList geomIndexList;
    DocumentModel*               docModel = getDocModel();
    PatternGeomSelectionModel*   pGSModel = HEXABLOCKGUI::currentDocGView->getPatternGeomSelectionModel();
    if (docModel == NULL || pGSModel == NULL)
        return geomIndexList;

    const QSortFilterProxyModel *pModel   = dynamic_cast<const QSortFilterProxyModel *>( model() );
    HEXA_NS::Vertex* vertex = docModel->getHexaPtr<HEXA_NS::Vertex*>(dataIndex);
    HEXA_NS::Edge*   edge   = docModel->getHexaPtr<HEXA_NS::Edge*>(dataIndex);
    HEXA_NS::Quad*   quad   = docModel->getHexaPtr<HEXA_NS::Quad*>(dataIndex);
    QModelIndex index = pModel->mapToSource(dataIndex);

    if (vertex != NULL)
        geomIndexList << docModel->getVertexAssociation( index );
    else if (edge != NULL)
        geomIndexList = docModel->getEdgeAssociations( index );
    else if ( quad != NULL)
        geomIndexList = docModel->getQuadAssociations( index );

    return geomIndexList;
}

void PatternDataSelectionModel::onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected )
{
    QModelIndexList indexes = selected.indexes();
    int nbSelected = indexes.count();
    if (nbSelected == 0)
        return;

    // ** unhighlight current highlighted items in the trees
    PatternGeomSelectionModel* pgsm = HEXABLOCKGUI::currentDocGView->getPatternGeomSelectionModel();
    if (pgsm == NULL)
        return;
    unhighlightTreeItems(false);

    QModelIndexList associatedGeomIndexes = getGeomAssociations(indexes[0]); //Mono Selection
    if (associatedGeomIndexes.count() == 0)
        pgsm->unhighlightTreeItems();

    // ** highlight association in geometry tree
    pgsm->highlightTreeItems(associatedGeomIndexes);

    // ** highlight association in the vtk view
    HEXABLOCKGUI::currentDocGView->highlight(indexes);

    // ** highlight association in the occ view
    HEXABLOCKGUI::currentOccGView->highlight(indexes);

    if (!infoMode || nbSelected > 1)
        return;

    // ** Show informations of the selected element
    QModelIndex elt = indexes[0];
    showEltInfo(elt);
}

void PatternDataSelectionModel::geomSelectionChanged( const Handle(SALOME_InteractiveObject)& anIObject )
{
//   if (HEXABLOCKGUI::assocInProgress) return;

   PatternGeomSelectionModel*  pgsm = HEXABLOCKGUI::currentDocGView->getPatternGeomSelectionModel();
   if (pgsm == NULL) return;

   QModelIndex geomModelIndex = pgsm->getModelIndex(anIObject);
   if (!geomModelIndex.isValid())
       return;
   pgsm->select( geomModelIndex, QItemSelectionModel::SelectCurrent );
}


void PatternDataSelectionModel::vtkSelectionChanged( const Handle(SALOME_InteractiveObject)& anIObject )
{
    QModelIndexList selectedIndexes =  getSelectionFromModel(anIObject);
    if (selectedIndexes.count() == 0)
    {
        clearSelection();
        return;
    }
    QModelIndex anIOIndex = selectedIndexes[0]; //Monoselection

    //Temporary Debug for hexa selection ------------------------------
    if (HEXABLOCKGUI::currentDocGView->getSelectionMode() == HEXA_TREE)
    {
        DocumentModel* docModel = getDocModel();
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
    }//end if HEXA_TREE-----------------------------------------------

    //select the element in the model
    select( anIOIndex, QItemSelectionModel::SelectCurrent );
}

// //===========================================================================
//                              PatternGeomSelectionModel
// //===========================================================================

PatternGeomSelectionModel::PatternGeomSelectionModel( QAbstractItemModel * model ):
SelectionModel( model )
{
  connect( this, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
           this, SLOT( onCurrentChanged( const QModelIndex & , const QModelIndex & ) ) );
  connect( this, SIGNAL( selectionChanged( const QItemSelection & , const QItemSelection & ) ),
           this, SLOT( onSelectionChanged( const QItemSelection & , const QItemSelection & ) ), Qt::UniqueConnection );
}

PatternGeomSelectionModel::~PatternGeomSelectionModel()
{
  disconnect( this, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
          this, SLOT( onCurrentChanged( const QModelIndex & , const QModelIndex & ) ) );
  disconnect( this, SIGNAL( selectionChanged( const QItemSelection & , const QItemSelection & ) ),
          this, SLOT( onSelectionChanged( const QItemSelection & , const QItemSelection & ) ) );
}

void PatternGeomSelectionModel::onCurrentChanged( const QModelIndex & current, const QModelIndex & previous )
{
  //Setting the selection mode of the selected item from the treeview}
  HEXABLOCKGUI::currentDocGView->setSelectionMode( current );
  HEXABLOCKGUI::currentOccGView->setSelectionMode( current );
}

void PatternGeomSelectionModel::onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected )
{
    DocumentModel* docModel = NULL;
    const QSortFilterProxyModel *pModel = dynamic_cast<const QSortFilterProxyModel *>( model() );
    if ( pModel != NULL)
        docModel = dynamic_cast<DocumentModel*>(pModel->sourceModel());

    QModelIndexList indexes = selected.indexes();
    if (indexes.count() == 0 || docModel == NULL)
        return;

    // ** unhighlight current highlighted items in the trees
    PatternDataSelectionModel* pdsm = HEXABLOCKGUI::currentDocGView->getPatternDataSelectionModel();
    if (pdsm == NULL)
        return;
    unhighlightTreeItems(false);

    //find the corresponding element in the model
    HEXA_NS::SubShape* eltSubShape = dynamic_cast<HEXA_NS::SubShape*>(docModel->getHexaPtr(indexes[0]));
    if (eltSubShape == NULL) return;

    HEXA_NS::NewShape* subShapeParent = eltSubShape->getParentShape();
    if (subShapeParent == NULL) return;

    QString parentShapeName = subShapeParent->getName();
    QString subId = QString::number(eltSubShape->getIdent());
    QString aGeomObjModelEntry = parentShapeName + "," + subId + ";";

    QModelIndexList assocsIndexes = pdsm->indexListOf(aGeomObjModelEntry, HEXA_ASSOC_ENTRY_ROLE);
    if (assocsIndexes.count() == 0)
        pdsm->unhighlightTreeItems();

    // ** highlight associations in the data tree
    pdsm->highlightTreeItems(assocsIndexes);

    // ** highlight associations in the vtk view
    HEXABLOCKGUI::currentDocGView->highlight(assocsIndexes);

    // ** highlight the selected element in the occ view
    QMultiMap<QString, int>  entrySubIDs;
    entrySubIDs.insert(parentShapeName, eltSubShape->getIdent());
    HEXABLOCKGUI::currentOccGView->highlight(entrySubIDs);
}

QModelIndex PatternGeomSelectionModel::getModelIndex(const Handle(SALOME_InteractiveObject)& anIObject)
{
    QModelIndex result;
    DocumentModel               *docModel = NULL;
    const QSortFilterProxyModel *pModel   = dynamic_cast<const QSortFilterProxyModel *>( model() );
    if ( pModel != NULL)
        docModel = dynamic_cast<DocumentModel*>( pModel->sourceModel());

    if ( docModel == NULL || HEXABLOCKGUI::selectionMgr() == NULL ||
            HEXABLOCKGUI::currentOccGView->getViewWindow() == NULL)
        return result;

    //extract associated elements in the model
    TColStd_IndexedMapOfInteger anIndexes;
    HEXABLOCKGUI::selectionMgr()->GetIndexes(anIObject, anIndexes);
    if (anIndexes.Extent() == 0) return result;
    QString aGeomObjStudyEntry = anIObject->getEntry();
    QString aGeomObjModelEntry = docModel->getGeomObjName(aGeomObjStudyEntry) + "," + QString::number(anIndexes(1));

    HEXA_NS::SubShape*    ssh =  docModel->getGeomPtr(aGeomObjModelEntry);
    HEXA_NS::VertexShape* vSh =  dynamic_cast<HEXA_NS::VertexShape*>(ssh);
    HEXA_NS::EdgeShape*   eSh =  dynamic_cast<HEXA_NS::EdgeShape*>(ssh);
    HEXA_NS::FaceShape*   fSh =  dynamic_cast<HEXA_NS::FaceShape*>(ssh);

    if (vSh != NULL)
        result = indexBy( HEXA_DATA_ROLE, QVariant::fromValue(vSh));
    else if (eSh != NULL)
        result = indexBy( HEXA_DATA_ROLE, QVariant::fromValue(eSh));
    else if (fSh != NULL)
        result = indexBy( HEXA_DATA_ROLE, QVariant::fromValue(fSh));

    return result;
}

void PatternGeomSelectionModel::highlightTreeItems(QModelIndexList& indexes,
                                                   Qt::GlobalColor bgColor,
                                                   Qt::GlobalColor fgColor, bool only)
{
    if (indexes.count() == 0)
        return;

    if (only)
        unhighlightTreeItems();

    // * highlight items
    QAbstractItemModel* theModel = (QAbstractItemModel*) model();
    foreach( const QModelIndex& anItemIndex, indexes )
    {
        if (anItemIndex.isValid())
        {
            theModel->setData(anItemIndex, QColor(fgColor), Qt::ForegroundRole);
            theModel->setData(anItemIndex, QColor(bgColor), Qt::BackgroundRole);
            currentHighlightedItems << anItemIndex;
        }
    }

    // * scroll to the first highlighted item in the tree
    HEXABLOCKGUI* module = HEXABLOCKGUI::getInstance();
    if (module != NULL && indexes[0].isValid())
        module->getPatternGeomTreeView()->scrollTo(indexes[0]);
}

void PatternGeomSelectionModel::unhighlightTreeItems(bool clearSelected)
{
    if (clearSelected)
        clearSelection();
    if (currentHighlightedItems.count() == 0)
        return;
    QAbstractItemModel* theModel = (QAbstractItemModel*) model();
    foreach( const QModelIndex& anItemIndex, currentHighlightedItems)
    {
        if (anItemIndex.isValid())
        {
            theModel->setData(anItemIndex, QColor(Qt::darkGreen), Qt::ForegroundRole);
            theModel->setData(anItemIndex, QColor(Qt::white), Qt::BackgroundRole);
        }
    }
    currentHighlightedItems.clear();
}

// //===========================================================================
//                              GroupsSelectionModel
// //===========================================================================
GroupsSelectionModel::GroupsSelectionModel( QAbstractItemModel * model ):
SelectionModel( model )
{
  connect( this, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
           this, SLOT( onCurrentChanged( const QModelIndex & , const QModelIndex & ) ), Qt::UniqueConnection );
  connect( this, SIGNAL( selectionChanged( const QItemSelection & , const QItemSelection & ) ),
           this, SLOT( onSelectionChanged( const QItemSelection & , const QItemSelection & ) ), Qt::UniqueConnection );
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
    QModelIndexList indexes = selected.indexes();
    int nbSelected = indexes.count();
    if (nbSelected == 0)
        return;

    // ** Highlight the group in the vtk view **/
    try {
        for( QModelIndexList::const_iterator i_index = indexes.begin(); i_index != indexes.end(); ++i_index ){
            HEXABLOCKGUI::currentDocGView->highlightGroups( *i_index );
        }
    } catch ( ... ) {
        MESSAGE("Unknown exception was cought !!!");
    }

    if (!infoMode || nbSelected > 1)
        return;

    // ** Show informations of the selected element **/
    QModelIndex elt = indexes[0];
    showEltInfo(elt);
}

// //===========================================================================
//                              MeshSelectionModel
// //===========================================================================
MeshSelectionModel::MeshSelectionModel( QAbstractItemModel * model ):
        SelectionModel( model )
{
    connect( this, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
            this, SLOT( onCurrentChanged( const QModelIndex & , const QModelIndex & ) ), Qt::UniqueConnection );
    connect( this, SIGNAL( selectionChanged( const QItemSelection & , const QItemSelection & ) ),
            this, SLOT( onSelectionChanged( const QItemSelection & , const QItemSelection & ) ), Qt::UniqueConnection );
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
    QModelIndexList indexes = selected.indexes();
    int nbSelected = indexes.count();
    if (nbSelected == 0)
        return;

    try {
        for( QModelIndexList::const_iterator i_index = indexes.begin(); i_index != indexes.end(); ++i_index ){
            HEXABLOCKGUI::currentDocGView->highlightPropagation( *i_index );
        }
    } catch ( ... ) {
        MESSAGE("Unknown exception was cought !!!");
    }

    if (!infoMode || nbSelected > 1)
        return;

    // ** Show informations of the selected element **/
    QModelIndex elt = indexes[0];
    showEltInfo(elt);
}

