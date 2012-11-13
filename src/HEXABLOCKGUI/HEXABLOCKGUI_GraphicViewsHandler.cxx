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
    MESSAGE("GraphicViewsHandler::createDocumentGraphicView(DocumentModel* docModel, SVTK_ViewWindow* wnd, QWidget* parent)");

//    if (wnd == NULL) return NULL;
//    VtkDocumentGraphicView* dgview = getDocumentGraphicView(wnd);
//    if (dgview != NULL) return dgview;
//
//    docGraphicViews[wnd] = new VtkDocumentGraphicView(docModel, wnd, parent);
//    return docGraphicViews[wnd];
    return new VtkDocumentGraphicView(docModel, wnd, parent);
    MESSAGE("}");
}

/*====================================================== getDocumentGraphicView
 * Returns the document corresponding to a vtk window (view)
 *=============================================================================*/
VtkDocumentGraphicView* GraphicViewsHandler::getDocumentGraphicView(SVTK_ViewWindow* view) const
{
    MESSAGE("GraphicViewsHandler::getDocumentGraphicView(SVTK_ViewWindow* view)");

    if (view == NULL || !docGraphicViews.contains(view)) return NULL;
    return docGraphicViews[view];

    MESSAGE("}");
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
    SUIT_ViewManager* vmgr = anApp->getViewManager( SVTK_Viewer::Type(), false );
    SVTK_ViewWindow* view;

    if (vmgr == NULL) //No Vtk view manager, so create one
    {
        vmgr = anApp->getViewManager( SVTK_Viewer::Type(), true );
        view = dynamic_cast<SVTK_ViewWindow*>(vmgr->getActiveView());
        view->setWindowTitle(QObject::tr("HEXABLOCK") + " : " + view->windowTitle());
        return view;
    }

    view = dynamic_cast<SVTK_ViewWindow*>(vmgr->createViewWindow());
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

    SUIT_ViewManager* vmgr = anApp->getViewManager( OCCViewer_Viewer::Type(), false );
    OCCViewer_ViewWindow* view;

    if (vmgr == NULL) //No Vtk view manager, so create one
    {
        vmgr = anApp->getViewManager( OCCViewer_Viewer::Type(), true );
        view = dynamic_cast<OCCViewer_ViewWindow*>(vmgr->getActiveView());
        view->setWindowTitle(QObject::tr("HEXABLOCK") + " : " + view->windowTitle());
        return view;
    }

    view = dynamic_cast<OCCViewer_ViewWindow*>(vmgr->createViewWindow());
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

//void GraphicViewsHandler::onPatternDatachanged()
//{
//    INIT_REAC(V_patternDatachaged);
//    OCCview->reacPatternDatachanged();
//    OUT_REAC();
//}

//VTKGraphicView* m_VTKview;
//
//void DocumentHandler::switchOnVTKGraphicView()
//{
//    verifieVTKView();
//
//    //connect signals
//    connect( m_VTKview->getPatternDataSelectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &) ),
//            this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
//    connect( m_VTKview->getPatternBuilderSelectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
//            this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
//    connect( m_VTKview->getGroupsSelectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
//            this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
//    connect( m_VTKview->getMeshSelectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
//            this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
//
//        _showDialogBox(getDlgBox(view));
// }
//
//DocumentHandler::onSelectionChanged(const QItemSelection &, const QItemSelection &)
//{
//  addMapConnect(++cptConnect;"onSelectionChanged");
//  if(onAction) {return;}
//  onAction = true;
//
//  ptHexaGUI->onSelectionChanged(const QItemSelection &, const QItemSelection &);
//  m_VTKview->onSelectionChanged(const QItemSelection &, const QItemSelection &);
//
//  onAction = false;
//}


