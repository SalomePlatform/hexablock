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

#include <QItemSelectionModel>

#include <LightApp_SelectionMgr.h>

#include <SVTK_ViewWindow.h>
#include <SVTK_Selector.h>

#include <GEOMBase_Helper.h>

#include "klinkitemselectionmodel.hxx"
#include "HEXABLOCKGUI_DocumentModel.hxx"


namespace HEXABLOCK
{
  namespace GUI
  {
    class MyGEOMBase_Helper;

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
    class PatternDataSelectionModel : public QItemSelectionModel,
                                      public GEOMBase_Helper
    {
      Q_OBJECT

      public:
        PatternDataSelectionModel( QAbstractItemModel * model );
//         PatternDataSelectionModel( QAbstractItemModel * model, QObject * parent );
        virtual ~PatternDataSelectionModel();

        void setVertexSelection();
        void setEdgeSelection();
        void setQuadSelection();
        void setHexaSelection();
        void setAllSelection();

        //Salome
        void setSalomeSelectionMgr( LightApp_SelectionMgr* mgr );
//         static LightApp_SelectionMgr* selectionMgr();
        void SetSelectionMode(Selection_Mode theMode);
//         void setGeomEngine( GEOM::GEOM_Gen_var geomEngine );

      protected slots:
        void onCurrentChanged( const QModelIndex & current, const QModelIndex & previous );
        void onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected );

        void salomeSelectionChanged(); // Salome to Qt

      private:
        SVTK_ViewWindow* _getVTKViewWindow();

        QModelIndex _indexOf( const QString& anIOEntry, int role );

        void _setVTKSelectionMode( const QModelIndex& eltIndex, SVTK_ViewWindow* vtkViewWindow );
        void _highlightGEOM( const QModelIndex & index );
        void _selectVTK( const QModelIndex & index );

        QModelIndex _geomSelectionChanged( const Handle(SALOME_InteractiveObject)& anIObject );
        QModelIndex _vtkSelectionChanged( const Handle(SALOME_InteractiveObject)& anIObject );

        LightApp_SelectionMgr* _salomeSelectionMgr;
        int                    _selectionFilter;

        bool _theModelSelectionChanged;
        bool _theVtkSelectionChanged;
        bool _theGeomSelectionChanged;

    };
  }
}

#endif


//       protected:
//         virtual GEOM::GEOM_IOperations_ptr createOperation();
//         virtual bool execute( ObjectList& );
//         virtual void displayPreview ( const bool   display,
// 				const bool   activate = false, 
//                                 const bool   update = true,
//                                 const bool   toRemoveFromEngine = true,
//                                 const double lineWidth = -1, 
//                                 const int    displayMode = -1,
//                                 const int    color  = -1 );