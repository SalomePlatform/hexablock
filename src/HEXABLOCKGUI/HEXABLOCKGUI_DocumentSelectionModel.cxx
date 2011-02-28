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

#include "HEXABLOCKGUI_DocumentSelectionModel.hxx"
#include "HEXABLOCKGUI_DocumentGraphicView.hxx"
#include "HEXABLOCKGUI_DocumentItem.hxx"
#include "HEXABLOCKGUI.hxx"
// #include "HexVertex.hxx"



#include <SUIT_Session.h>
#include <SUIT_Selector.h>
#include <SUIT_Desktop.h>
#include <SVTK_Selector.h>
#include <SVTK_ViewModel.h>


// #include <SalomeApp_DataObject.h>
// #include <SalomeApp_Study.h>
// #include <SalomeApp_Module.h>
#include <SalomeApp_Application.h>
#include <SALOME_Actor.h>
#include <SALOME_ListIO.hxx>
#include <SALOME_ListIteratorOfListIO.hxx>


//#define _DEVDEBUG_
using namespace std;
using namespace HEXABLOCK::GUI;



DocumentSelectionModel::DocumentSelectionModel( QAbstractItemModel * model ):
QItemSelectionModel( model ),
_salomeSelectionChanged(false)
{
  _salomeSelectionMgr = DocumentSelectionModel::selectionMgr();

  connect( _salomeSelectionMgr, SIGNAL( currentSelectionChanged() ), this, SLOT( salomeSelectionChanged() ) );
  connect( this, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
           this, SLOT( onCurrentChanged( const QModelIndex & , const QModelIndex & ) ) );
  connect( this, SIGNAL( selectionChanged( const QItemSelection & , const QItemSelection & ) ),
           this, SLOT( onSelectionChanged( const QItemSelection & , const QItemSelection & ) ) );

}




DocumentSelectionModel::DocumentSelectionModel( QAbstractItemModel * model, QObject * parent ):
QItemSelectionModel( model, parent ),
_salomeSelectionChanged(false)
{
  _salomeSelectionMgr = DocumentSelectionModel::selectionMgr();

connect( _salomeSelectionMgr, SIGNAL( currentSelectionChanged() ), this, SLOT( salomeSelectionChanged() ) );
connect( this, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
         this, SLOT( onCurrentChanged( const QModelIndex & , const QModelIndex & ) ) );
connect( this, SIGNAL( selectionChanged( const QItemSelection & , const QItemSelection & ) ),
         this, SLOT( onSelectionChanged( const QItemSelection & , const QItemSelection & ) ) );

}

DocumentSelectionModel::~DocumentSelectionModel()
{
}

LightApp_SelectionMgr* DocumentSelectionModel::selectionMgr()
{
  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  if( anApp )
    return dynamic_cast<LightApp_SelectionMgr*>( anApp->selectionMgr() );
  else
    return 0;
}


SVTK_ViewWindow* DocumentSelectionModel::GetViewWindow ()
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



void  DocumentSelectionModel::SetSelectionMode(Selection_Mode theMode)
{
  SVTK_ViewWindow* aViewWindow = DocumentSelectionModel::GetViewWindow();
  aViewWindow->SetSelectionMode( theMode );
}

// void  DocumentSelectionModel::SetSelectionMode(Selection_Mode theMode)
// {
// 
//   QList<SUIT_Selector*> aSelectors;
//   _salomeSelectionMgr->selectors( SVTK_Viewer::Type(), aSelectors );
//   QListIterator<SUIT_Selector*> it( aSelectors );
//   
//   std::cout << "DocumentSelectionModel::SetSelectionMode()" << std::endl;
//   while ( it.hasNext() )
//   {
//   //   SUIT_Selector* selector = it.next();
//     SVTK_Selector* selector = dynamic_cast<SVTK_Selector*>( it.next() );
//     std::cout << "DocumentSelectionModel::SetSelectionMode()" << theMode << std::endl;
//     selector->SetSelectionMode(theMode);
//   }
// }




void DocumentSelectionModel::salomeSelectionChanged()
{
//   std::cout << "DocumentSelectionModel::salomeSelectionChanged()" << std::endl;
  _salomeSelectionChanged = true;

  QModelIndex newIndex;


  SALOME_ListIO salomeSelected; 
  _salomeSelectionMgr->selectedObjects( salomeSelected );
  SALOME_ListIteratorOfListIO it(salomeSelected);
  for( ; it.More(); it.Next()) {
    Handle(SALOME_InteractiveObject) anIObject = it.Value();
//     std::cout << anIObject->getName() << std::endl;
    if(anIObject->hasEntry()){
//       std::cout << "Salome selection => " <<anIObject->getEntry() << std::endl;
      QModelIndexList indexes = model()->match(
            model()->index(0, 0),
            HEXA_ENTRY_ROLE,
            anIObject->getEntry(),
            1,
            Qt::MatchRecursive);
//       std::cout << "indexes.count()"<< indexes.count()<<std::endl;
      if ( indexes.count()>0 ){
//         std::cout << "Salome selection found!"<< std::endl;
        newIndex = indexes[0];
        select( newIndex, QItemSelectionModel::ClearAndSelect );
        setCurrentIndex( newIndex, QItemSelectionModel::Current );
                                  // QItemSelectionModel::Current
                                  // QItemSelectionModel::Select 
                                  // QItemSelectionModel::SelectCurrent
                                  // QItemSelectionModel::ToggleCurrent
                                  // QItemSelectionModel::ClearAndSelect
      } else {
        std::cout << "Salome selection not found!"<< std::endl;
      }
    }
  }

  _salomeSelectionChanged = false;
}

void DocumentSelectionModel::_selectSalome( const QModelIndex & index )
{
//   std::cout << "DocumentSelectionModel::_selectSalome " << std::endl;
//   std::cout << "Check ->  " << index.model()->data(index).toString().toStdString() << std::endl;

  QVariant variant = index.model()->data(index, HEXA_ENTRY_ROLE);
  if ( !variant.isValid() ) return;

  QString entry = variant.toString();
//   std::cout << "Found in Salome ->  " << entry.toStdString() << std::endl;
  SVTK_ViewWindow* aViewWindow = DocumentSelectionModel::GetViewWindow();
  SALOME_Actor* anActor = DocumentGraphicView::FindActorByEntry( aViewWindow, entry.toStdString().c_str() );

  if ( anActor ){
    SALOME_ListIO aList;
    Handle(SALOME_InteractiveObject) anIO = anActor->getIO();
    aList.Append(anIO);
    _salomeSelectionMgr->setSelectedObjects( aList, true ); //CS_BP false?
//     std::cout << "Selected in Salome! ->" << entry.toStdString()  << std::endl;
  }
}


void DocumentSelectionModel::onCurrentChanged( const QModelIndex & current, const QModelIndex & previous )
{
//   std::cout << "DocumentSelectionModel::onCurrentChanged " << std::endl;
//   _selectSalome( current, true );
//   _selectSalome( previous, false );
}


void DocumentSelectionModel::onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected )
{
  std::cout << "DocumentSelectionModel::onSelectionChanged" << std::endl;

  if ( _salomeSelectionChanged ) return;

  _salomeSelectionMgr->clearSelected();
  QModelIndexList indexes = selected.indexes();
  for( QModelIndexList::const_iterator i_index = indexes.begin(); i_index != indexes.end(); ++i_index )
    _selectSalome( *i_index );

 // CS_BP todo SALOMEGUI_Swig.cxx:370
//   indexes = deselected.indexes();
//   for( QModelIndexList::const_iterator i_index = indexes.begin(); i_index != indexes.end(); ++i_index )
//     _unselectSalome( *i_index);
}

