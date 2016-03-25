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

#ifndef _HEXABLOCKGUI_VTKGRAPHICVIEWSHANDLER_HXX_
#define _HEXABLOCKGUI_VTKGRAPHICVIEWSHANDLER_HXX_

//Salome Includes
#include <SVTK_ViewWindow.h>
#include <OCCViewer_ViewWindow.h>

//Qt Includes
#include <QMap>

//HEXABLOCK Includes
#include "HEXABLOCKGUI_Export.hxx"
#include "HEXABLOCKGUI_VtkDocumentGraphicView.hxx"

namespace HEXABLOCK
{
  namespace GUI
  {

      class HEXABLOCK_EXPORT GraphicViewsHandler
      {

      public:
          GraphicViewsHandler() {}
          virtual ~GraphicViewsHandler() {}
          VtkDocumentGraphicView* getDocumentGraphicView(SVTK_ViewWindow* view) const;
          VtkDocumentGraphicView* createDocumentGraphicView(DocumentModel* docModel, SVTK_ViewWindow* wnd, QWidget* parent);
          SVTK_ViewWindow* getOrCreateVtkWindow() const;
          SVTK_ViewWindow* createVtkWindow() const;
          OCCViewer_ViewWindow* getOrCreateOccWindow() const;
          OCCViewer_ViewWindow* createOccWindow() const;
          void closeDocumentGraphicView(SVTK_ViewWindow* window);
              void setViewManager(SUIT_ViewManager* vmanager);
          int getNbDocumentGViews() { return docGraphicViews.count();}

      protected:
          QMap<SVTK_ViewWindow*, VtkDocumentGraphicView*> docGraphicViews;
      };

  }
}

#endif
