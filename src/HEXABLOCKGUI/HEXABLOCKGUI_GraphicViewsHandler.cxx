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

//Salome
#include <SalomeApp_Application.h>
#include <SUIT_Session.h>
#include <SUIT_ViewManager.h>
#include <SVTK_ViewModel.h>
#include <SOCC_ViewModel.h>

//HexaBlock
#include "HEXABLOCKGUI_GraphicViewsHandler.hxx"
#include "utilities.h"



using namespace HEXABLOCK::GUI;



/*====================================================== createDocumentGraphicView
 * Create a new document
 * Returns the new document's window
 *=============================================================================*/
VtkDocumentGraphicView* GraphicViewsHandler::createDocumentGraphicView(DocumentModel* docModel,
                                                                     SVTK_ViewWindow* wnd, QWidget* parent)
{

//    if (wnd == NULL) return NULL;
//    VtkDocumentGraphicView* dgview = getDocumentGraphicView(wnd);
//    if (dgview != NULL) return dgview;
//
//    docGraphicViews[wnd] = new VtkDocumentGraphicView(docModel, wnd, parent);
//    return docGraphicViews[wnd];
    return new VtkDocumentGraphicView(docModel, wnd, parent);
}

/*====================================================== getDocumentGraphicView
 * Returns the document corresponding to a vtk window (view)
 *=============================================================================*/
VtkDocumentGraphicView* GraphicViewsHandler::getDocumentGraphicView(SVTK_ViewWindow* view) const
{
    if (view == NULL || !docGraphicViews.contains(view)) return NULL;
    return docGraphicViews[view];
}

/*====================================================== getOrCreateVtkWindow
/// Returns the active vtk window if there's one,
/// create one and return it otherwise.
 *=============================================================================*/
SVTK_ViewWindow* GraphicViewsHandler::getOrCreateVtkWindow() const
{
    SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>(SUIT_Session::session()->activeApplication());
    if( anApp == NULL) return NULL;
    SUIT_ViewManager* vmgr = anApp->getViewManager( SVTK_Viewer::Type(), true);

    return dynamic_cast<SVTK_ViewWindow*>(vmgr->getActiveView());
}


/*====================================================== createVtkWindow
/// Create a vtk window and return it
 *=============================================================================*/
SVTK_ViewWindow* GraphicViewsHandler::createVtkWindow() const
{
    SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>(SUIT_Session::session()->activeApplication());
    if( anApp == NULL) return NULL;

    SUIT_ViewManager* vmgr = anApp->newViewManager (SVTK_Viewer::Type());
    SVTK_ViewWindow* view = dynamic_cast<SVTK_ViewWindow*>(vmgr->getActiveView());
    if (view == NULL) return NULL;
    view->setWindowTitle(QObject::tr("HEXABLOCK") + " : " + view->windowTitle());

    return view;
}


/*====================================================== getOrCreateOccWindow
/// Returns the active occ window if there's one,
/// create one and return it otherwise.
 *=============================================================================*/
OCCViewer_ViewWindow* GraphicViewsHandler::getOrCreateOccWindow() const
{
    SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>(SUIT_Session::session()->activeApplication());
    if( anApp == NULL) return NULL;
    SUIT_ViewManager* vmgr = anApp->getViewManager(OCCViewer_Viewer::Type(), true);

    return dynamic_cast<OCCViewer_ViewWindow*>(vmgr->getActiveView());
}


/*====================================================== createOccWindow
/// Create an open cascade window and return it
 *=============================================================================*/
OCCViewer_ViewWindow* GraphicViewsHandler::createOccWindow() const
{
    SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>(SUIT_Session::session()->activeApplication());
    if( anApp == NULL) return NULL;

    SUIT_ViewManager* vmgr = anApp->newViewManager (OCCViewer_Viewer::Type());
    OCCViewer_ViewWindow* view = dynamic_cast<OCCViewer_ViewWindow*>(vmgr->getActiveView());
    if (view == NULL) return NULL;
    view->setWindowTitle(QObject::tr("HEXABLOCK") + " : " + view->windowTitle());

    return view;
}


/*====================================================== createDocumentGraphicView
 * Close a document graphic view
 *=============================================================================*/
void GraphicViewsHandler::closeDocumentGraphicView(SVTK_ViewWindow* window)
{
    VtkDocumentGraphicView* dgview = getDocumentGraphicView(window);
    if (dgview == NULL) return; //There's nothing to do if the window has no document

    //remove the document
    docGraphicViews.remove(window);
    delete dgview;
}


