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

#ifndef _HEXABLOCKGUI_DOCUMENTGRAPHICVIEW_HXX_
#define _HEXABLOCKGUI_DOCUMENTGRAPHICVIEW_HXX_


// #include <SVTK_ViewWindow.h>
#include <SUIT_ViewWindow.h>
#include <QAbstractItemView>
#include <QMap>

// #include <QContextMenuEvent>
// #include <QAction>
// #include <QGraphicsRectItem>

#include <HexVertex.hxx>
#include <HexQuad.hxx>
#include <HexEdge.hxx>
#include <HexHexa.hxx>

// ajout JPL
class LightApp_Displayer;
class SalomeApp_Application;
class SALOME_Actor;

class vtkLookupTable ;
class vtkPoints ;
class vtkCellArray ;
class vtkFloatArray ;
class vtkActor ;
class vtkPolyData;
// end JPL

namespace HEXABLOCK
{

    namespace GUI
    {

        class MyVTKLinePlotter
        {
        public:

            MyVTKLinePlotter();

            void SetScalarRange(double minval=0.0, double maxval=1.0);
            void SetLookupTable(vtkLookupTable* table = 0);

            void SetAllLineWidth(int width = 1);
            void PlotLine(double m[3], double n[3], double scalar);
            void PlotLine(double x, double y, double z,
                          double x2, double y2, double z2, double scalar);
	
//            vtkActor* CreateActor(); // call after all lines are
//            plotted!
            bool CreateActor(SALOME_Actor* actor); // call after all lines are plotted!
            vtkPolyData* CreatePolyData(); // call after all lines are plotted!
        private:

            double m_scalarMin, m_scalarMax ;
            vtkLookupTable* m_lookupTable ;
            int m_curPointID ;
            int m_allLineWidth ;

            vtkPoints* m_points;
            vtkCellArray* m_lines;
            vtkFloatArray* m_lineScalars ;
	
        };


        /********************************************************************************
         *                          DocumentGraphicView
         ********************************************************************************/
        class DocumentGraphicView : public QAbstractItemView
        {
            Q_OBJECT

            public:
            DocumentGraphicView(SalomeApp_Application* app, SUIT_ViewWindow *suitView, QWidget *parent = 0 );
            virtual ~DocumentGraphicView();

            SUIT_ViewWindow* get_SUIT_ViewWindow();

            // ajout JPL :
            virtual LightApp_Displayer* displayer();
            bool addObject(const QModelIndex& index);
            bool updateObject(const QModelIndex& index);
            static SALOME_Actor* FindActorByEntry(SUIT_ViewWindow *theWindow, const char* theEntry);
            SALOME_Actor* CreateActor(const QString& entry);
            void RemoveActor( SUIT_ViewWindow *theWnd, SALOME_Actor* theActor);
            // end JPL

            QModelIndex indexAt(const QPoint &point) const;
            void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);
            QRect visualRect(const QModelIndex &index) const;


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
            SUIT_ViewWindow *_suitView;

            // ajout JPL :
            LightApp_Displayer*             myDisplayer;
            SalomeApp_Application* myApp;

            void _buildActor(SALOME_Actor* actor, HEXA_NS::Vertex *v);
            void _buildActor(SALOME_Actor* actor, HEXA_NS::Edge *v);
            void _buildActor(SALOME_Actor* actor, HEXA_NS::Quad *v); 
            void _buildActor(SALOME_Actor* actor, HEXA_NS::Hexa *v); 

        };
    }
}




#endif
