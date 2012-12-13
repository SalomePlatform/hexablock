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



#ifndef _HEXABLOCKGUI_OCCGRAPHICVIEW_HXX_
#define _HEXABLOCKGUI_OCCGRAPHICVIEW_HXX_


#include <OCCViewer_ViewWindow.h>
#include "HEXABLOCKGUI_SalomeTools.hxx"

#include <QModelIndex>
#include <QModelIndexList>




namespace HEXABLOCK
{

    namespace GUI
    {

        /********************************************************************************
         *                          OccGraphicView
         ********************************************************************************/

        class OccGraphicView
        {

         public:
            OccGraphicView(OCCViewer_ViewWindow* view, QWidget* parent=NULL);
            virtual ~OccGraphicView(){};

            void highlight( const QModelIndex & index );
            void highlight( const QModelIndexList & indexList );
            void highlight( const QMultiMap<QString, int>&  entrySubIDs );

            void setViewWindow(OCCViewer_ViewWindow* view) { viewWindow = view; }
            OCCViewer_ViewWindow* getViewWindow() const { return viewWindow; }

            void setSelectionMode(TopAbs_ShapeEnum mode);
            void setSelectionMode(const QModelIndex& eltIndex);
            TopAbs_ShapeEnum getSelectionMode() const { return selectionMode; }
            void clearSelection();

         private:
            OCCViewer_ViewWindow*  viewWindow;
            TopAbs_ShapeEnum selectionMode;

        };
    }
}


#endif
