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

#ifndef _HEXABLOCKGUI_DOCUMENTGRAPHICVIEW_HXX_
#define _HEXABLOCKGUI_DOCUMENTGRAPHICVIEW_HXX_


// SALOME GUI includes
#include <LightApp_Displayer.h>
// #include <SVTK_ViewWindow.h>
#include <SUIT_ViewWindow.h>
#include <SALOME_Actor.h>







#include <QAbstractItemView>
#include <QMap>

// #include <QContextMenuEvent>
// #include <QAction>
// #include <QGraphicsRectItem>

#include <HexVertex.hxx>
#include <HexQuad.hxx>
#include <HexEdge.hxx>
#include <HexHexa.hxx>
#include <HexDocument.hxx>


// class LightApp_Displayer;
// class SalomeApp_Application;
class LightApp_Application;
class SALOME_Actor;

// class vtkLookupTable ;
// class vtkPoints ;
// class vtkCellArray ;
// class vtkFloatArray ;
// class vtkActor ;
// class vtkPolyData;


class vtkUnstructuredGrid;


namespace HEXABLOCK
{

    namespace GUI
    {

        class Document_Actor : public SALOME_Actor
        {
          public:
            Document_Actor(HEXA_NS::Document* doc, const QString& entry);
            virtual ~Document_Actor();

            std::map<int,vtkIdType>   vtkElemsId;
            std::map<vtkIdType, int>  hexaElemsId;

            vtkUnstructuredGrid* getUnstructuredGrid();
          private:
            HEXA_NS::Document* _doc;
        };


        /********************************************************************************
         *                          DocumentGraphicView
         ********************************************************************************/
        class DocumentGraphicView : public QAbstractItemView
        {
            Q_OBJECT

            public:
            DocumentGraphicView( LightApp_Application* app, SUIT_ViewWindow *suitView, QWidget *parent = 0 );
            virtual ~DocumentGraphicView();





            SUIT_ViewWindow* get_SUIT_ViewWindow();
//             static SALOME_Actor* FindActorByEntry(SUIT_ViewWindow *theWindow, const char* theEntry);

//             void loadVTK( const QString&  path );
            void update();//mise à jours de la vue : reconstruction complète

//             bool eventFilter(QObject *obj, QEvent *event);
//             virtual void setModel ( QAbstractItemModel * model );

            QModelIndex indexAt(const QPoint &point) const;
            void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);
            QRect visualRect(const QModelIndex &index) const;

//             public:
//         virtual bool canBeDisplayed( const QString& /*entry*/, const QString& /*viewer_type*/ ) const;

        public slots:
          void onPatternDatachanged();

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

//         protected:
//           virtual SALOME_Prs* buildPresentation( const QString&, SALOME_View* = 0 );

        private:
            SUIT_ViewWindow *_suitView;
            Document_Actor  *_documentActor;
            bool _currentChanged;
        };
    }
}




#endif
