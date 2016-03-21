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

#ifndef _HEXABLOCKGUI_VTKDOCUMENTGRAPHICVIEW_HXX_
#define _HEXABLOCKGUI_VTKDOCUMENTGRAPHICVIEW_HXX_

#include "HEXABLOCKGUI_Export.hxx"

// SALOME GUI includes
#include <LightApp_Displayer.h>
#include <SUIT_ViewWindow.h>
#include <SALOME_Actor.h>
#include <SVTK_InteractorStyle.h>
#include <QAbstractItemView>
#include <QMap>

#include <HexVertex.hxx>
#include <HexQuad.hxx>
#include <HexEdge.hxx>
#include <HexHexa.hxx>
#include <HexDocument.hxx>
#include <vtkInteractorStyleTrackballCamera.h>
#include <SVTK_RenderWindowInteractor.h>
#include <vtkActor2D.h>
#include <SVTK_ViewWindow.h>

#include "HEXABLOCKGUI_DocumentModel.hxx"
#include "HEXABLOCKGUI_DocumentSelectionModel.hxx"

class LightApp_Application;
class SALOME_Actor;

class vtkUnstructuredGrid;



namespace HEXABLOCK
{

    namespace GUI
    {
        class HEXABLOCK_EXPORT Document_Actor : public SALOME_Actor
        {
          public:
            Document_Actor(HEXA_NS::Document* doc, const QString& entry);
            virtual ~Document_Actor();

            // Highlight
            virtual void highlight(bool theHighlight) {}

            std::map<int,vtkIdType>   vtkElemsId;
            std::map<vtkIdType, int>  hexaElemsId;

            vtkUnstructuredGrid* getUnstructuredGrid();
          private:
            HEXA_NS::Document* _doc;

        };
                   // Abu
        class HEXABLOCK_EXPORT Associate_Actor : public SALOME_Actor
        {
          public:
            Associate_Actor(HEXA_NS::Document* doc, const QString& entry);
            virtual ~Associate_Actor() {}

            // Highlight
            virtual void highlight(bool theHighlight) {}

            std::map<int,vtkIdType>   vtkElemsId;
            std::map<vtkIdType, int>  hexaElemsId;

            vtkUnstructuredGrid* getUnstructuredGrid();
          private:
            HEXA_NS::Document* _doc;
        };

        /********************************************************************************
         *                          VtkDocumentGraphicView
         ********************************************************************************/
        class HEXABLOCK_EXPORT VtkDocumentGraphicView : public QAbstractItemView
        {
            Q_OBJECT

            public:
            VtkDocumentGraphicView(DocumentModel* docModel, SVTK_ViewWindow* view, QWidget* parent=NULL);
            virtual ~VtkDocumentGraphicView();
	    
	        void setModel ( QAbstractItemModel * model );
	        bool isEmpty() const { return getDocumentModel()->isEmpty(); }
	        void highlight( const QModelIndex& ielt );
	        void highlight( const QModelIndexList& elts );
	        void highlightGroups( const QModelIndex& eltIndex );
	        void highlightPropagation( const QModelIndex& eltIndex );

            void setSelectionMode( const Selection_Mode theMode );
            void setSelectionMode( const QModelIndex& eltIndex );
            int getSelectionMode() const { return selectionMode; }
            void getSelected(SALOME_ListIO& selectedObjects);
            void clearSelection();

	        void loadDocument(const QString& file); //Loads a selected document
	        void loadDocument(); //Loads the current document
	        void saveDocument(const QString& file);
	        void update();//mise à jours de la vue : reconstruction complète
	        void removeActor();

	        QModelIndex indexAt(const QPoint &point) const;
	        void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);
	        QRect visualRect(const QModelIndex &index) const;

	        SVTK_ViewWindow* getViewWindow() const { return viewWindow; }
	        void setViewWindow(SVTK_ViewWindow* view) { viewWindow = view; }
	        Document_Actor* getDocumentActor() const { return documentActor; }

	        DocumentModel*      getDocumentModel() const {
	            return dynamic_cast<DocumentModel*>(model());
	        }
	        PatternDataSelectionModel* getPatternDataSelectionModel() const
	        { return patternDataSelectionModel; }

	        PatternGeomSelectionModel* getPatternGeomSelectionModel() const
            { return patternGeomSelectionModel;}

	        GroupsSelectionModel* getGroupsSelectionModel() const
	        { return groupsSelectionModel; }

	        MeshSelectionModel* getMeshSelectionModel() const
	        { return meshSelectionModel; }

	        PatternDataModel*    getPatternDataModel() const {return patternDataModel;}
	        PatternGeomModel*    getPatternGeomModel() const { return patternGeomModel;}
	        GroupsModel*         getGroupsModel() const {return groupsModel;}
	        MeshModel*           getMeshModel() const {return meshModel;}

        public slots:
          void setWindowTitle(const QString& title);
          void onPatternDatachanged();
          void setVertexSelection();
          void setEdgeSelection();
          void setQuadSelection();
          void setHexaSelection();
          void setAllSelection();

        protected slots:
          virtual void 	closeEditor ( QWidget * editor, QAbstractItemDelegate::EndEditHint hint );
          virtual void 	commitData ( QWidget * editor );
          virtual void 	currentChanged ( const QModelIndex & current, const QModelIndex & previous );
          virtual void 	dataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight );
          virtual void 	editorDestroyed ( QObject * editor );
          virtual void 	rowsAboutToBeRemoved ( const QModelIndex & parent, int start, int end );
          virtual void 	rowsInserted ( const QModelIndex & parent, int start, int end );
          virtual void 	selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected );
          virtual void 	updateGeometries ();

        protected:
            int horizontalOffset() const;
            bool isIndexHidden(const QModelIndex &index) const;
            QModelIndex moveCursor(QAbstractItemView::CursorAction cursorAction,
                                   Qt::KeyboardModifiers modifiers);
            void setSelection(const QRect&, QItemSelectionModel::SelectionFlags command);
            int verticalOffset() const;
            QRegion visualRegionForSelection(const QItemSelection &selection) const;

        private:
            SVTK_ViewWindow*  viewWindow;
            int selectionMode;
            bool state;

            Document_Actor*   documentActor;
            Associate_Actor*  associateActor;

            PatternDataSelectionModel*     patternDataSelectionModel;
            PatternGeomSelectionModel*     patternGeomSelectionModel;
            GroupsSelectionModel*          groupsSelectionModel;
            MeshSelectionModel*            meshSelectionModel;

            PatternDataModel*              patternDataModel;
            PatternGeomModel*              patternGeomModel;
            GroupsModel*                   groupsModel;
            MeshModel*                     meshModel;

            bool _currentChanged;
            bool firstUpdate;
        };
    }
}




#endif
