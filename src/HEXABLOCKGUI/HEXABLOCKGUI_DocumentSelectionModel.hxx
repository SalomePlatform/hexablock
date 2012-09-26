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

#ifndef _HEXABLOCKGUI_DOCUMENTSELECTIONMODEL_HXX_
#define _HEXABLOCKGUI_DOCUMENTSELECTIONMODEL_HXX_

#include <QItemSelectionModel>
#include <LightApp_SelectionMgr.h>

#include <SVTK_ViewWindow.h>
#include <SVTK_Selector.h>

// #include <SOCC_ViewModel.h>
// #include <SOCC_ViewWindow.h>
// #include <OCCViewer_ViewWindow.h>

#include "MyGEOMBase_Helper.hxx"

#include "klinkitemselectionmodel.hxx"
#include "HEXABLOCKGUI_DocumentModel.hxx"


class OCCViewer_ViewWindow;

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

    class PatternDataSelectionModel : public QItemSelectionModel,
                                      public MyGEOMBase_Helper
    {
      Q_OBJECT

      public:
        PatternDataSelectionModel( QAbstractItemModel * model );
        virtual ~PatternDataSelectionModel();

        void setVertexSelection();
        void setEdgeSelection();
        void setQuadSelection();
        void setHexaSelection();
        void setAllSelection();
        void highlightVTKElts( const QModelIndexList& elts );
        void highlightEltsWithAssocs(const QModelIndexList& elts);
        void _highlightGEOM( const QMultiMap<QString, int>&  entrySubIDs );
        void _highlightGEOM( const QModelIndex & index );

        QModelIndex  indexBy( int role, const QString&  value );
        QModelIndex  indexBy( int role, const QVariant& var );
        int getSelectionFilter() const { return _selectionFilter;}

        //Salome
        void setSalomeSelectionMgr( LightApp_SelectionMgr* mgr );
        void SetSelectionMode( Selection_Mode theMode );
        QModelIndex _geomSelectionChanged( const Handle(SALOME_InteractiveObject)& anIObject );
        QModelIndex _vtkSelectionChanged( const Handle(SALOME_InteractiveObject)& anIObject );


        bool salomeNothingSelected;

      protected slots:
        void onCurrentChanged( const QModelIndex & current, const QModelIndex & previous );
        void onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected );
        void salomeSelectionChanged(); // Salome to Qt

      private:
        SVTK_ViewWindow*       _getVTKViewWindow();
        OCCViewer_ViewWindow*  _getOCCViewWindow();

        QModelIndex _indexOf( const QString& anIOEntry, int role );
        QModelIndexList _indexListOf( const QString& anEntry, int role );
        void _setVTKSelectionMode( const QModelIndex& eltIndex );
        void _selectVTK( const QModelIndex & index );
        SUIT_ViewWindow* initOccViewManager();


        LightApp_SelectionMgr* _salomeSelectionMgr;
        int                    _selectionFilter;


        bool _theModelSelectionChanged;
        bool _theVtkSelectionChanged;
        bool _theGeomSelectionChanged;
    };



    class GroupsSelectionModel : public QItemSelectionModel
    {
      Q_OBJECT

      public:
        GroupsSelectionModel( QAbstractItemModel * model );
        virtual ~GroupsSelectionModel();

        QModelIndex  indexBy( int role, const QVariant& var );

      protected slots:
//         void onCurrentChanged( const QModelIndex & current, const QModelIndex & previous );
        void onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected );

      private:
        SVTK_ViewWindow* _getVTKViewWindow();
        void _highlightGroups( const QModelIndex& eltIndex );

    };




    class MeshSelectionModel : public QItemSelectionModel
    {
      Q_OBJECT

      public:
        MeshSelectionModel( QAbstractItemModel * model );
        virtual ~MeshSelectionModel();

        QModelIndex  indexBy( int role, const QVariant& var );

      protected slots:
//         void onCurrentChanged( const QModelIndex & current, const QModelIndex & previous );
        void onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected );

      private:
        SVTK_ViewWindow* _getVTKViewWindow();
        void _highlightPropagation( const QModelIndex& eltIndex );

    };


  }
}

#endif


