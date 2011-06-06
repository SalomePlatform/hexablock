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

#ifndef _HEXABLOCKGUI_DOCUMENTSELECTIONMODEL_HXX_
#define _HEXABLOCKGUI_DOCUMENTSELECTIONMODEL_HXX_

// #include <QAbstractItemModel>
// #include <QStandardItemModel>
// #include <QSortFilterProxyModel>


#include <SVTK_ViewWindow.h>
#include <SVTK_Selector.h>

#include <QItemSelectionModel>

#include "klinkitemselectionmodel.hxx"

#include <LightApp_SelectionMgr.h>

// #include <QItemSelection>
// #include <QBrush>
// #include <list>

// #include "HEXABLOCKGUI_DocumentItem.hxx"
// 
// #include "HexDocument.hxx"

namespace HEXABLOCK
{
  namespace GUI
  {

    class PatternBuilderSelectionModel: public KLinkItemSelectionModel
    {
      public:
        PatternBuilderSelectionModel( QAbstractItemModel *model, 
                                      QItemSelectionModel *proxySelector, QObject *parent = 0 ):
        KLinkItemSelectionModel( model, proxySelector, parent )
        {
        }
    };

//     class DocumentSelectionModel : public QItemSelectionModel
    class PatternDataSelectionModel : public QItemSelectionModel
    {
      Q_OBJECT

      public:
        PatternDataSelectionModel( QAbstractItemModel * model );
        PatternDataSelectionModel( QAbstractItemModel * model, QObject * parent );
        virtual ~PatternDataSelectionModel();
        

        //Salome
        static LightApp_SelectionMgr* selectionMgr();
        static SVTK_ViewWindow* GetViewWindow ();
        void SetSelectionMode(Selection_Mode theMode);

      protected slots:
        void onCurrentChanged( const QModelIndex & current, const QModelIndex & previous );
        void onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected );

        void salomeSelectionChanged(); // Salome to Qt

      private:
        void _selectSalome( const QModelIndex & index ); // Qt to Salome

        LightApp_SelectionMgr* _salomeSelectionMgr;
        bool                   _salomeSelectionChanged;
    };


    


  }
}

#endif
