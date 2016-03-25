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

#ifndef _HEXABLOCKGUI_DOCUMENTSELECTIONMODEL_HXX_
#define _HEXABLOCKGUI_DOCUMENTSELECTIONMODEL_HXX_

#include "HEXABLOCKGUI_Export.hxx"

#include <QItemSelectionModel>
//#include <LightApp_SelectionMgr.h>

//#include <SVTK_ViewWindow.h>
#include <SVTK_Selector.h>

//#include "klinkitemselectionmodel.hxx"

#include "HEXABLOCKGUI_DocumentModel.hxx"

class OCCViewer_ViewWindow;


namespace HEXABLOCK
{
  namespace GUI
  {

    class HEXABLOCK_EXPORT SelectionModel: public QItemSelectionModel
    {
        Q_OBJECT

        public:

            SelectionModel( QAbstractItemModel * model );
            virtual ~SelectionModel();

            QModelIndex     indexBy( int role, const QString&  value );
            QModelIndex     indexBy( int role, const QVariant& var );
            QModelIndex     indexOf( const QString& anIOEntry, int role );
            QModelIndexList indexListOf( const QString& anEntry, int role );
            DocumentModel*  getDocModel() const;
            void            showEltInfo(QModelIndex& elt);

            virtual void geomSelectionChanged( const Handle(SALOME_InteractiveObject)& anIObject ) {}
            virtual void vtkSelectionChanged( const Handle(SALOME_InteractiveObject)& anIObject ) {}

            void clearHighlightedItems() { currentHighlightedItems.clear(); }

            void setInfoMode(bool isOn) { infoMode = isOn; }

       protected slots:

            virtual void onCurrentChanged( const QModelIndex & current,
                    const QModelIndex & previous ) {}

            virtual void onSelectionChanged( const QItemSelection & selected,
                    const QItemSelection & deselected ) {}

            void salomeSelectionChanged(); // Salome to Qt

       protected:
            QModelIndexList getSelectionFromModel(const Handle(SALOME_InteractiveObject)& anIObject);
            QModelIndexList currentHighlightedItems;
            static bool infoMode;
    };

//    class PatternBuilderSelectionModel: public KLinkItemSelectionModel
//    {
//      public:
//        PatternBuilderSelectionModel( QAbstractItemModel *model,
//                                      QItemSelectionModel *proxySelector, QObject *parent = 0 ):
//        KLinkItemSelectionModel( model, proxySelector, parent )
//        {
//        }
//    };

    class HEXABLOCK_EXPORT PatternDataSelectionModel : public SelectionModel
    {
      Q_OBJECT

      public:
        PatternDataSelectionModel( QAbstractItemModel * model );
        virtual ~PatternDataSelectionModel();

        void highlightTreeItems(QModelIndexList& indexes,
                                Qt::GlobalColor bgColor = Qt::darkGreen,
                                Qt::GlobalColor fgColor = Qt::white, bool only=true);
        void unhighlightTreeItems(bool clearSelected=true);
        QModelIndexList getGeomAssociations(const QModelIndex& dataIndex);

        //Salome
        virtual void geomSelectionChanged( const Handle(SALOME_InteractiveObject)& anIObject );
        virtual void vtkSelectionChanged( const Handle(SALOME_InteractiveObject)& anIObject );

      protected slots:
        virtual void onCurrentChanged( const QModelIndex & current, const QModelIndex & previous );
        virtual void onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected );

    };

    class HEXABLOCK_EXPORT PatternGeomSelectionModel : public SelectionModel
    {
        Q_OBJECT

    public:
        PatternGeomSelectionModel( QAbstractItemModel * model );
        virtual ~PatternGeomSelectionModel();

        QModelIndex getModelIndex(const Handle(SALOME_InteractiveObject)& anIObject);
        void highlightTreeItems(QModelIndexList& indexes,
                                Qt::GlobalColor bgColor = Qt::darkGreen,
                                Qt::GlobalColor fgColor = Qt::white, bool only=true);
        void unhighlightTreeItems(bool clearSelected=true);

    protected slots:
        virtual void onCurrentChanged( const QModelIndex & current, const QModelIndex & previous );
        virtual void onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected );

    };


    class HEXABLOCK_EXPORT GroupsSelectionModel : public SelectionModel
    {
      Q_OBJECT

      public:
        GroupsSelectionModel( QAbstractItemModel * model );
        virtual ~GroupsSelectionModel();

      protected slots:
        virtual void onSelectionChanged( const QItemSelection & selected,
                                            const QItemSelection & deselected );

    };


    class HEXABLOCK_EXPORT MeshSelectionModel : public SelectionModel
    {
      Q_OBJECT

      public:
        MeshSelectionModel( QAbstractItemModel * model );
        virtual ~MeshSelectionModel();

      protected slots:
        virtual void onSelectionChanged( const QItemSelection & selected,
                const QItemSelection & deselected );

    };


  }
}

#endif


