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



#ifndef _HEXABLOCKGUI_OCCGRAPHICVIEW_HXX_
#define _HEXABLOCKGUI_OCCGRAPHICVIEW_HXX_

#include "HEXABLOCKGUI_Export.hxx"

#include <OCCViewer_ViewWindow.h>
#include "HEXABLOCKGUI_SalomeTools.hxx"

#include <AIS_Shape.hxx>
#include <SelectMgr_IndexedMapOfOwner.hxx>
#include <SOCC_Prs.h>

#include <QModelIndex>
#include <QModelIndexList>

namespace HEXABLOCK
{

    namespace GUI
    {

        /********************************************************************************
         *                          OccGraphicView
         ********************************************************************************/

        class HEXABLOCK_EXPORT OccGraphicView
        {

         public:
            OccGraphicView(OCCViewer_ViewWindow* view, QWidget* parent=NULL);
            virtual ~OccGraphicView(){};

            void highlight( const QModelIndex & index, bool only=true );
            void highlight( const QModelIndexList & indexList, bool only=true );

            // * 'only' parameter precise if we want to clear the current highlighted shapes/subshapes first
            void highlightSubShapes(const Handle(AIS_Shape)& anObj,
                                    const TColStd_IndexedMapOfInteger& aIndexMap, bool only=true);
            void highlight( const QMultiMap<QString, int>&  entrySubIDs, bool only=true );

            void setViewWindow(OCCViewer_ViewWindow* view) { viewWindow = view; }
            OCCViewer_ViewWindow* getViewWindow() const { return viewWindow; }

            void setSelectionMode(TopAbs_ShapeEnum mode);
            void setSelectionMode(const QModelIndex& eltIndex);
            TopAbs_ShapeEnum getSelectionMode() const { return selectionMode; }
            void getSelected(SALOME_ListIO& selectedObjects);
            void clearSelection();
            void localSelection(const int theMode);
            void globalSelection(const bool update = false );

            void addShape(QString& entry, const Handle(AIS_Shape)& ais_shape);
            Handle(AIS_Shape) getShape(const QString& entry);
            Handle(AIS_InteractiveContext) getContext();
            SOCC_Prs* getPrs() const { return prs; }
            void setPrs(SOCC_Prs* occPrs) { prs = occPrs; }
            void addVertexToCloud(TopoDS_Vertex& vertex);
            void displayPreview(TopoDS_Shape& shape);
            Handle(AIS_Shape) getPreviewShape();


         private:
            OCCViewer_ViewWindow*  viewWindow;
            SOCC_Prs* prs;
            Handle(AIS_Shape) previewShape;
            TopAbs_ShapeEnum selectionMode;
            QMap<QString, Handle(AIS_Shape)> shapes;
        };
    }
}


#endif
