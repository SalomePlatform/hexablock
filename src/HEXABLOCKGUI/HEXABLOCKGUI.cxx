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

//#define _DEVDEBUG_

#include <cassert>

#include <QDir>
#include <QInputDialog>
#include <QIcon>
#include <QStackedWidget>
#include <QMenu>
#include <QSplitter>

#include <QtxTreeView.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_DataObject.h>
#include <SUIT_DataBrowser.h>
#include <SUIT_FileDlg.h>

#include <SVTK_ViewManager.h>
#include <SVTK_ViewModel.h>
#include <SVTK_ViewWindow.h>

#include <VTKViewer_ViewModel.h>
#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <VTKViewer_Algorithm.h>
#include <vtkRenderer.h>



#include <SOCC_ViewModel.h>
#include <SOCC_ViewWindow.h>
#include <OCCViewer_ViewWindow.h>
#include <SALOME_Prs.h>
#include <SOCC_Prs.h>
#include <SOCC.h>

// SALOME KERNEL includes
#include <SALOMEDS_Study.hxx>
#include <SALOMEDSClient_StudyBuilder.hxx>
#include <SALOMEDSClient_SComponent.hxx>
#include <SALOMEDSClient_ClientFactory.hxx>
#include <SALOMEDSClient_IParameters.hxx>



#include <SalomeApp_Study.h>
#include <SalomeApp_Module.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_DataModel.h>


#include <SALOME_ListIO.hxx>


#include <SALOME_LifeCycleCORBA.hxx>

#include <LightApp_VTKSelector.h>

#include <OCCViewer_ViewManager.h>

#include <QtxPopupMgr.h>

#include "Resource.hxx"

#include "HEXABLOCKGUI.hxx"
#include "HEXABLOCKGUI_Export.hxx"
#include "HEXABLOCKGUI_Trace.hxx"
#include "HEXABLOCKGUI_Resource.hxx"

#include "HEXABLOCKGUI_DocumentModel.hxx"
#include "HEXABLOCKGUI_DocumentSelectionModel.hxx"
//#include "HEXABLOCKGUI_DocumentDelegate.hxx"
#include "HEXABLOCKGUI_DocumentPanel.hxx"
#include "HEXABLOCKGUI_VtkDocumentGraphicView.hxx"
#include "HEXABLOCKGUI_OccGraphicView.hxx"
#include "HEXABLOCKGUI_GraphicViewsHandler.hxx"

#include "HEXABLOCK_version.h"

#include "MyBasicGUI_PointDlg.hxx"

// #include "HEXABLOCK.hxx"                del Hexa6
// #include "HexDocument_impl.hxx"         del Hexa6
#include "Hex.hxx"

//#include <V3d_PerspectiveView.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <Xw_Window.hxx>
#include <V3d_TypeOfShadingModel.hxx>
#include <TCollection_ExtendedString.hxx>

#include <BRepAlgoAPI_Fuse.hxx>

#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <utilities.h>

#define DW_MINIMUM_WIDTH       50
#define DWINPUT_MINIMUM_HEIGHT 50
#define DWINPUT_MINIMUM_WIDTH  50//255

using namespace std;
using namespace HEXABLOCK::GUI;

int  HEXABLOCKGUI::_oldStudyId = -1;

// HEXABLOCK_ORB::HEXABLOCK_Gen_var HEXABLOCKGUI::_hexaEngine  = HEXABLOCK_ORB::HEXABLOCK_Gen::_nil();
//
VtkDocumentGraphicView* HEXABLOCKGUI::currentDocGView = NULL;
OccGraphicView*         HEXABLOCKGUI::currentOccGView = NULL;
bool                    HEXABLOCKGUI::assocInProgress = false;
SalomeApp_Application*  HEXABLOCKGUI::myApplication = NULL;

HEXABLOCKGUI::HEXABLOCKGUI() :
          SalomeApp_Module( "HEXABLOCK" ),
//          LightApp_Module( "HEXABLOCK" ),
          _menuId(190),
          _dwPattern(0),
          _dwAssociation(0),
          _dwGroups(0),
          _dwMesh(0),
          _dwObjectBrowser(0),
          _dwInputPanel(0),
          _patternDataTreeView(0),
//          _patternBuilderTreeView(0),
          _patternGeomTreeView(0),
          _groupsTreeView(0),
          _meshTreeView(0),
//          _treeViewDelegate(0),
          _isSaved( false ),
          moduleActivatedOnce(false),
          menusEnabled(true),
          _vertexDiag(0),
          _edgeDiag(0),
          _quadDiag(0),
          _hexaDiag(0),
          _vectorDiag(0),
          _cylinderDiag(0),
          _pipeDiag(0),
          _makeGridDiag(0),
          _makeCylinderDiag(0),
          _makePipeDiag(0),
          _makeCylindersDiag(0),
          _makePipesDiag(0),
          _removeHexaDiag(0),
          _prismQuadDiag(0),
          _joinQuadDiag(0),
          _mergeDiag(0),
          _disconnectDiag(0),
          _cutEdgeDiag(0),
          _makeTransformationDiag(0),
          _makeSymmetryDiag(0),
          _performTransformationDiag(0),
          _performSymmetryDiag(0),
          _vertexAssocDiag(0),
          _edgeAssocDiag(0),
          _quadAssocDiag(0),
          _groupDiag(0),
          _lawDiag(0),
          _propagationDiag(0),
          _computeMeshDiag(0),
          _replaceHexaDiag(0),
          _quadRevolutionDiag(0),
          _makeHemiSphereDiag(0),
          _modelInfoDiag(NULL),
          _addShapeDiag(NULL),
          _vertexInfoDialog(NULL),
          _edgeInfoDialog(NULL),
          _quadInfoDialog(NULL),
          _hexaInfoDialog(NULL),
          _vectorInfoDialog(NULL),
          _groupInfoDialog(NULL),
          _lawInfoDialog(NULL),
          _propagationInfoDialog(NULL),
          currentDialog(NULL),
          lastOccPrs(NULL),
          lastVtkDocGView(NULL)
{
    DEBTRACE("HEXABLOCKGUI::HEXABLOCKGUI");

    graphicViewsHandler = new GraphicViewsHandler();
    loadDocLastPath = new QDir();
    saveDocLastPath = new QDir();
    hexa_root       = HEXA_NS::Hex::getInstance ();
}

HEXABLOCKGUI::~HEXABLOCKGUI()
{
}


SalomeApp_Study* HEXABLOCKGUI::activeStudy()
{
    SUIT_Application* app = SUIT_Session::session()->activeApplication();
    if( app )
        return dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
    else
        return NULL;
}


// Gets an reference to the module's engine
/****************************************************
HEXABLOCK_ORB::HEXABLOCK_Gen_ptr HEXABLOCKGUI::InitHEXABLOCKGen( SalomeApp_Application* app )
{
    Engines::EngineComponent_var comp = app->lcc()->FindOrLoad_Component( "FactoryServer", "HEXABLOCK" );
    HEXABLOCK_ORB::HEXABLOCK_Gen_ptr clr = HEXABLOCK_ORB::HEXABLOCK_Gen::_narrow(comp);
    ASSERT(!CORBA::is_nil(clr));
    return clr;
}
 **************************************************** */

void HEXABLOCKGUI::initialize( CAM_Application* app )
{
	MESSAGE("HEXABLOCKGUI::initialize");
    DEBTRACE("HEXABLOCKGUI::initialize");
    SalomeApp_Module::initialize( app );

    // _hexaEngine = InitHEXABLOCKGen( dynamic_cast<SalomeApp_Application*>( app ) );

    DEBTRACE(app << "  " << application() << " " << application()->desktop() << " " << aParent);

    SUIT_ResourceMgr* aResourceMgr = app->resourceMgr();
    setResource(aResourceMgr);

    createAndFillDockWidget();
    createActions();
    createMenus();
    createTools();
    studyActivated();
}

void HEXABLOCKGUI::viewManagers( QStringList& list ) const
{
    DEBTRACE("HEXABLOCKGUI::viewManagers");
    //   list.append( QxScene_Viewer::Type() );
    //   list.append( OCCViewer_Viewer::Type() );
    //   list.append( SVTK_Viewer::Type() );
}


bool HEXABLOCKGUI::activateModule( SUIT_Study* theStudy )
{
    DEBTRACE("HEXABLOCKGUI::activateModule");

    /*HexaBlock can be launched in only one application*/
    if (myApplication == NULL)
        //remember the first application
        myApplication = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );

    //application verification: if it's not the one known by HexaBlock,
    //leave without activating
    SalomeApp_Application* currentApplication =  dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
    if (myApplication != currentApplication) return false;

    bool bOk = SalomeApp_Module::activateModule( theStudy );
    if ( !bOk ) return false;

    showDockWidgets(true);

    connect( getApp()->desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
            this, SLOT(onWindowActivated( SUIT_ViewWindow* )), Qt::UniqueConnection );
    connect( getApp()->objectBrowser()->treeView(),SIGNAL( clicked(const QModelIndex&) ),
            this, SLOT( onObjectBrowserClick(const QModelIndex&) ), Qt::UniqueConnection );

    LightApp_SelectionMgr* sm = getApp()->selectionMgr();

    SUIT_ViewManager* vm;
    ViewManagerList OCCViewManagers, VTKViewManagers;

    application()->viewManagers( OCCViewer_Viewer::Type(), OCCViewManagers );
    QListIterator<SUIT_ViewManager*> itOCC( OCCViewManagers );
    while ( itOCC.hasNext() && (vm = itOCC.next()) )
        myOCCSelectors.append( new HEXABLOCKGUI_OCCSelector( ((OCCViewer_ViewManager*)vm)->getOCCViewer(), sm ) );

    application()->viewManagers( SVTK_Viewer::Type(), VTKViewManagers );
    QListIterator<SUIT_ViewManager*> itVTK( VTKViewManagers );
    while ( itVTK.hasNext() && (vm = itVTK.next()) )
        myVTKSelectors.append( new LightApp_VTKSelector( dynamic_cast<SVTK_Viewer*>( vm->getViewModel() ), sm ) );

    //NPAL 19674
    SALOME_ListIO selected;
    sm->selectedObjects( selected );
    sm->clearSelected();

    // disable OCC selectors
    getApp()->selectionMgr()->setEnabled( false, OCCViewer_Viewer::Type() );
    QListIterator<HEXABLOCKGUI_OCCSelector*> itOCCSel( myOCCSelectors );
    while ( itOCCSel.hasNext() )
        if ( HEXABLOCKGUI_OCCSelector* sr = itOCCSel.next() )
            sr->setEnabled(true);

    // disable VTK selectors
    getApp()->selectionMgr()->setEnabled( false, SVTK_Viewer::Type() );
    QListIterator<LightApp_VTKSelector*> itVTKSel( myVTKSelectors );
    while ( itVTKSel.hasNext() )
        if ( LightApp_VTKSelector* sr = itVTKSel.next() )
            sr->setEnabled(true);

    sm->setSelectedObjects( selected, true );   //NPAL 19674

    //connect close signal to vtk view manager
    vm = getApp()->getViewManager(SVTK_Viewer::Type(), false);
    if (vm != NULL)
    {
        connect( vm, SIGNAL( tryCloseView( SUIT_ViewWindow * ) ),
                this, SLOT( onWindowClosed(SUIT_ViewWindow *) ), Qt::UniqueConnection );
    }

    //connect close signal to occ view manager
    vm = getApp()->getViewManager(OCCViewer_Viewer::Type(), false);
    if (vm != NULL)
    {
        connect( vm, SIGNAL( tryCloseView( SUIT_ViewWindow * ) ),
                this, SLOT( onWindowClosed(SUIT_ViewWindow *) ), Qt::UniqueConnection );
    }

    /* ************************************   TODO Hexa6
    _hexaEngine->SetCurrentStudy(SALOMEDS::Study::_nil());
    if ( SalomeApp_Study* s = dynamic_cast<SalomeApp_Study*>( theStudy ))
        if ( _PTR(Study) aStudy = s->studyDS()) {
            _hexaEngine->SetCurrentStudy( _CAST(Study,aStudy)->GetStudy() );
            updateObjBrowser(); // objects can be removed
        }
     ************************************ */

    if (currentOccGView != NULL && currentOccGView->getViewWindow() != NULL)
            currentOccGView->getViewWindow()->installEventFilter(this);

    if (currentDocGView != NULL)
    {
        switchModel(currentDocGView);
        showAllMenus();

        if (currentDocGView->getViewWindow() != NULL)
            currentDocGView->getViewWindow()->installEventFilter(this);
    }
    else
    {
        newDocument();
//        initialMenus();
    }

    return bOk;
}


bool HEXABLOCKGUI::deactivateModule( SUIT_Study* theStudy )
{
    disconnect( getApp()->desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
            this, SLOT(onWindowActivated( SUIT_ViewWindow* )) );
    disconnect( getApp()->objectBrowser()->treeView(),SIGNAL( clicked(const QModelIndex&) ),
            this, SLOT( onObjectBrowserClick(const QModelIndex&) ) );

    SVTK_ViewWindow* vtkView = (currentDocGView != NULL) ? currentDocGView->getViewWindow() : NULL;
    if ( vtkView != NULL)
    {
        //default selectionMode in VTKView
        vtkView->SetSelectionMode( ActorSelection );
        vtkView->removeEventFilter(this);
//        vtkView->close();
    }

    OCCViewer_ViewWindow* occView = (currentOccGView != NULL) ? currentOccGView->getViewWindow() : NULL;
    if (occView != NULL)
    {
        //defaut selectionMode in OccView
        selectionMgr()->clearSelected();
        currentOccGView->globalSelection();
        occView->removeEventFilter(this);
//        occView->close();
    }

    qDeleteAll(myOCCSelectors);
    myOCCSelectors.clear();
    getApp()->selectionMgr()->setEnabled( true, OCCViewer_Viewer::Type() );

    qDeleteAll(myVTKSelectors);
    myVTKSelectors.clear();
    getApp()->selectionMgr()->setEnabled( true, SVTK_Viewer::Type() );

    bool bOk = SalomeApp_Module::deactivateModule( theStudy );

    //switch off current document graphic view
    switchOffGraphicView(currentDocGView);

    //...and hide menus
    setMenuShown( false );
    setToolShown( false );
    showDockWidgets( false );
    if (_dwInputPanel)
        _dwInputPanel->close();

    hideActor();

    return bOk;
}

SALOMEDS::Study_var HEXABLOCKGUI::ClientStudyToStudy (_PTR(Study) theStudy)
{
  SALOME_NamingService *aNamingService = SalomeApp_Application::namingService();
  CORBA::Object_var aSMObject = aNamingService->Resolve("/myStudyManager");
  SALOMEDS::StudyManager_var aStudyManager = SALOMEDS::StudyManager::_narrow(aSMObject);
  int aStudyID = theStudy->StudyId();
  SALOMEDS::Study_var aDSStudy = aStudyManager->GetStudyByID(aStudyID);
  return aDSStudy._retn();
}

void HEXABLOCKGUI::addInStudy(QMap<QString, TopoDS_Shape>& topo_shapes,
                              QMap<QString, QString>& docShapesEntry,
                              QMap<QString, QString>& docShapesName)
{
    if (topo_shapes.size() == 0 || currentOccGView == NULL || currentDocGView == NULL)
        return; //there's nothing to publish in this case

    SalomeApp_Study* appStudy = HEXABLOCKGUI::activeStudy();
    if(!appStudy) return;
    _PTR(Study) aStudy = appStudy->studyDS();
    SALOMEDS::Study_var aDSStudy = ClientStudyToStudy( aStudy );
    SALOMEDS::StudyBuilder_var     aBuilder (aDSStudy->NewBuilder());
    QString entry = currentDocGView->getDocumentModel()->documentEntry();
    SALOMEDS::SObject_var aFatherSO = aDSStudy->FindObjectID( qPrintable(entry) );
    if (aFatherSO->_is_nil())
        return;

    // * get a presentation
    bool isNewPrs = false;
    SOCC_Prs* prs = getOccPrs(currentDocGView);
    if (prs == NULL)
    {
        prs = new SOCC_Prs(0);
        isNewPrs = true;
    }

    // * init the presentation with the given shapes
    QMap<QString, TopoDS_Shape>::const_iterator i = topo_shapes.constBegin();
    while (i != topo_shapes.constEnd())
    {
        QString shapeName  = i.key();
        TopoDS_Shape shapeValue = i.value();
        Handle(AIS_Shape) ais_shape = new AIS_Shape(shapeValue);
        SALOMEDS::SObject_var aSO = aBuilder->NewObject(aFatherSO);
        QString objStudyEntry = aSO->GetID(); //the entry of the published object
        Handle( SALOME_InteractiveObject ) io = new SALOME_InteractiveObject(objStudyEntry.toStdString().c_str(),
                                                                    "HEXABLOCK", shapeName.toStdString().c_str());
        ais_shape->SetOwner(io);
        prs->AddObject(ais_shape);
        aSO->SetAttrString("AttributeName", shapeName.toStdString().c_str());
        docShapesEntry[shapeName] = objStudyEntry;
        docShapesName[objStudyEntry] = shapeName;
        currentOccGView->addShape(objStudyEntry, ais_shape);
        i++; //handle next shape
    }
    // * set the preview
    if (isNewPrs)
        prs->AddObject(currentOccGView->getPreviewShape());

    occPrs[currentDocGView] = prs;
    getApp()->updateObjectBrowser();
    showOnlyActor();
}


bool HEXABLOCKGUI::renameAllowed( const QString& entry) const
{
    SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
    SalomeApp_Study* appStudy = app ? dynamic_cast<SalomeApp_Study*>( app->activeStudy() ) : 0;
    SalomeApp_DataObject* obj = appStudy ? dynamic_cast<SalomeApp_DataObject*>(appStudy->findObjectByEntry(entry)) : 0;

    return (app && appStudy && obj && !appStudy->isComponent(entry) && !obj->isReference());
}


bool HEXABLOCKGUI::renameObject( const QString& entry, const QString& name)
{
    bool result = false;

    // Pas de renommage des shapes pour le moment, juste les documents
    VtkDocumentGraphicView* dgview = getDocGViewByEntry(entry);
    if (dgview == NULL || dgview->getDocumentModel() == NULL)
        return result;

    SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication());
    SalomeApp_Study* appStudy = app ? dynamic_cast<SalomeApp_Study*>( app->activeStudy() ) : 0;

    if(!appStudy)
        return result;

    _PTR(Study) aStudy = appStudy->studyDS();

    if(!aStudy)
        return result;;

    _PTR(SObject) obj ( aStudy->FindObjectID(qPrintable(entry)) );
    _PTR(GenericAttribute) anAttr;
    if ( obj ){
        if ( obj->FindAttribute(anAttr, "AttributeName") ){
            _PTR(AttributeName) aName (anAttr);
            DocumentModel* docModel = dgview->getDocumentModel();
            docModel->setName( name );
            aName->SetValue( name.toLatin1().data() );
            getApp()->updateObjectBrowser();
//            _dwPattern->setWindowTitle( name );
            result = true;
        }
    }
    return result;
}

// --- Default windows
void HEXABLOCKGUI::windows( QMap<int, int>& theMap ) const
{
    DEBTRACE("HEXABLOCKGUI::windows");
    theMap.clear();
    theMap.insert( SalomeApp_Application::WT_ObjectBrowser, Qt::LeftDockWidgetArea );
#ifndef DISABLE_PYCONSOLE
    theMap.insert( SalomeApp_Application::WT_PyConsole,     Qt::BottomDockWidgetArea );
#endif
}

QString  HEXABLOCKGUI::engineIOR() const
{
    DEBTRACE("HEXABLOCKGUI::engineIOR");

    /* ************************************   TODO Hexa6
    CORBA::ORB_var anORB = getApp()->orb();
    CORBA::String_var anIOR = anORB->object_to_string(_hexaEngine);
    return QString( anIOR.in() );
       ************************************ */
    return "";
}

VtkDocumentGraphicView* HEXABLOCKGUI::getOrCreateDocument(SalomeApp_DataObject* studyObject)
{
    if (docs.count(studyObject->entry()) == 0)
    {
        //Object's document not acquired yet
        VtkDocumentGraphicView* dgview;
        HEXA_NS::Hex* directory = HEXA_NS::Hex::getInstance();
        if (directory == NULL)
            return NULL;    //No working director found

        //------------------ Load the document -----------------
        HEXA_NS::Document* document = directory->findDocument(studyObject->name().toStdString());
        if (document == NULL)
            return NULL;    //No document found

        DocumentModel* docModel = new DocumentModel(document, studyObject->entry(), this );
        if (!docModel) {
            MESSAGE("Doc model creation failed!");
            return NULL;
        }
        dgview = graphicViewsHandler->createDocumentGraphicView( docModel, NULL, application()->desktop() );
        if (dgview == NULL) return NULL;

        //update the data tree
        docs[studyObject->entry()] = dgview;
    }

    return docs[studyObject->entry()];
}

void HEXABLOCKGUI::onObjectBrowserClick(const QModelIndex& index)
{
    // ** we want to switch automatically to the right view windows

    //first, find selected item
    QString itemEntry;
    DataObjectList dol = getApp()->objectBrowser()->getSelected();
    if (dol.isEmpty()) return;
    SalomeApp_DataObject* item = dynamic_cast<SalomeApp_DataObject*>(dol[0]);
    if (!item || item->parent()->name().compare(tr("HEXABLOCK")) != 0 ||
            item->entry().isEmpty())
        return;

    // find the document corresponding to it
    VtkDocumentGraphicView* docGView = NULL;
    if (docs.contains(item->entry()))
        docGView = docs[item->entry()];
    else
        docGView = getOrCreateDocument(item);

    //Init OCC if necessary
    if (currentOccGView == NULL)
    {
        currentOccGView = new OccGraphicView(graphicViewsHandler->createOccWindow(), application()->desktop());
        currentOccGView->getViewWindow()->installEventFilter(this);
    }
    else if (currentOccGView->getViewWindow() == NULL)
    {
        currentOccGView->setViewWindow(graphicViewsHandler->createOccWindow());
        currentOccGView->getViewWindow()->installEventFilter(this);
    }

    //Init the vtk view if necessary
    if (docGView->getViewWindow() == NULL)
    {
        if (currentDocGView == NULL || currentDocGView->getViewWindow() == NULL)
        {
            docGView->setViewWindow(graphicViewsHandler->createVtkWindow());
            docGView->getViewWindow()->installEventFilter(this);
            showAllMenus();
            if (!menusEnabled)
                setAllMenusEnabled(true);
        }
        else
            docGView->setViewWindow(currentDocGView->getViewWindow());

        if (docGView->getDocumentActor() == NULL)
            docGView->update();
    }

    if (docGView == NULL || docGView == currentDocGView)
    {
        showOnlyActor();
        return;
    }

    //update the current document
    switchModel(docGView);
}


void HEXABLOCKGUI::onWindowActivated( SUIT_ViewWindow* svw)
{
    DEBTRACE("HEXABLOCKGUI::onWindowActivated");
}

void HEXABLOCKGUI::onWindowClosed( SUIT_ViewWindow* svw)
{
    DEBTRACE("HEXABLOCKGUI::onWindowClosed");

    //Decharger le model correspondant (VTK)
    SVTK_ViewWindow* window = dynamic_cast<SVTK_ViewWindow*>(svw);
    if (window != NULL)
    {
        //VTK clean close
        if (currentDocGView != NULL && currentDocGView->getViewWindow() == window)
        { //HexaBlock Vtk Window has been closed

            if (currentDialog != NULL) currentDialog->close();
//            if (_treeViewDelegate != NULL) _treeViewDelegate->closeDialog();
            if (currentDocGView != NULL)
                currentDocGView->setViewWindow(NULL);

//            initialMenus();
            setAllMenusEnabled(false);
            enableDocumentMenus(true);

            return;
        }

        //SAVE DOCUMENT
        /*VtkDocumentGraphicView* dgview = graphicViewsHandler->getDocumentGraphicView(window);
        if (dgview == NULL) return; //The window has no document to close

        //Save the document? (Ask only if the document is not empty
        int answerCode = SUIT_MessageBox::question(
                            0,
                            tr("Close"),
                            tr("Save the document ?"),
                            SUIT_MessageBox::Yes | SUIT_MessageBox::Cancel,
                            SUIT_MessageBox::Cancel);
        if ( answerCode == SUIT_MessageBox::Cancel) return; //abort closing
        else if (answerCode == SUIT_MessageBox::Yes)
            saveDocument(); //Save the document
        */
//        switchOffGraphicView(window); //disconnect all connected signals
//        currentVtkView = NULL;
//        graphicViewsHandler->closeDocumentGraphicView(window);

        //create a new one (empty document)
//        newDocument();
//        currentVtkView->setWindowTitle(tr("HEXABLOCK") + " : " + currentVtkView->windowTitle());
    }

    //OCC clean close
    if (currentOccGView != NULL && svw == currentOccGView->getViewWindow())
    {
        if (_vertexAssocDiag != NULL) _vertexAssocDiag->clear();
        if (_edgeAssocDiag != NULL) _edgeAssocDiag->clear();
        if (_quadAssocDiag != NULL) _quadAssocDiag->clear();
        if (currentDialog != NULL) currentDialog->close();
//        if (_treeViewDelegate != NULL) _treeViewDelegate->closeDialog();

        currentOccGView->getViewWindow()->removeEventFilter(this);
        currentOccGView->setViewWindow(NULL);
    }
}

void HEXABLOCKGUI::onViewManagerAdded( SUIT_ViewManager*  vm)
{
    DEBTRACE("HEXABLOCKGUI::onViewManagerAdded");

    connect( vm, SIGNAL( tryCloseView( SUIT_ViewWindow * ) ),
            this, SLOT( onWindowClosed(SUIT_ViewWindow *) ), Qt::UniqueConnection );

    if ( vm && vm->getType() == OCCViewer_Viewer::Type() )
      {
        LightApp_SelectionMgr* sm = getApp()->selectionMgr();
        myOCCSelectors.append( new HEXABLOCKGUI_OCCSelector( ((OCCViewer_ViewManager*)vm)->getOCCViewer(), sm ) );
        // disable OCC selectors
        getApp()->selectionMgr()->setEnabled( false, OCCViewer_Viewer::Type() );
        QListIterator<HEXABLOCKGUI_OCCSelector*> itOCCSel( myOCCSelectors );
        while ( itOCCSel.hasNext() )
          if ( HEXABLOCKGUI_OCCSelector* sr = itOCCSel.next() )
            sr->setEnabled(true);
      }
      else if ( vm->getType() == SVTK_Viewer::Type() )
      {
        LightApp_SelectionMgr* sm = getApp()->selectionMgr();
        myVTKSelectors.append( new LightApp_VTKSelector( dynamic_cast<SVTK_Viewer*>( vm->getViewModel() ), sm ) );

        // disable VTK selectors
        getApp()->selectionMgr()->setEnabled( false, SVTK_Viewer::Type() );
        QListIterator<LightApp_VTKSelector*> itVTKSel( myVTKSelectors );
        while ( itVTKSel.hasNext() )
          if ( LightApp_VTKSelector* sr = itVTKSel.next() )
            sr->setEnabled(true);
      }
}

void HEXABLOCKGUI::onViewManagerRemoved( SUIT_ViewManager* vm)
{
    DEBTRACE("HEXABLOCKGUI::::onViewManagerRemoved");

    SUIT_ViewModel* viewer = vm->getViewModel();
    if (vm != NULL && vm->getType() == SVTK_Viewer::Type())
    {
        //VTK View Manager removed
        if (getApp()->activeModule()->moduleName().compare("HEXABLOCK") == 0)
        {
            //Process only if it's our view manager
            SUIT_ViewManager* myVm = NULL;
            if (currentDocGView != NULL && currentDocGView->getViewWindow() != NULL)
                myVm = currentDocGView->getViewWindow()->getViewManager();

            if (myVm == vm)
            {
                //close opened dialog
                if (currentDialog != NULL) currentDialog->close();
//                if (_treeViewDelegate != NULL) _treeViewDelegate->closeDialog();

                initialMenus();
            }
        }

        //remove its selector
        QListIterator<LightApp_VTKSelector*> itVTKSel( myVTKSelectors );
        while ( itVTKSel.hasNext() )
            if ( LightApp_VTKSelector* sr = itVTKSel.next() )
                if ( sr->viewer() == viewer )
                {
                    delete myVTKSelectors.takeAt( myVTKSelectors.indexOf( sr ) );
                    break;
                }
        return;
    }

    //OCC
    if ( vm && vm->getType() == OCCViewer_Viewer::Type() )
    {
        //OCC View removed: remove its selector
        QListIterator<HEXABLOCKGUI_OCCSelector*> itOCCSel( myOCCSelectors );
        while ( itOCCSel.hasNext() )
            if ( HEXABLOCKGUI_OCCSelector* sr = itOCCSel.next() )
                if ( sr->viewer() == viewer )
                {
                    delete myOCCSelectors.takeAt( myOCCSelectors.indexOf( sr ) );
                    break;
                }
    }
}

void HEXABLOCKGUI::onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected  )
{
    QModelIndexList l = selected.indexes();
    if ( l.count() == 0 ) return;
    if ( !l[0].isValid() ) return;
    VtkDocumentGraphicView* currentGraphicView = getCurrentVtkGraphicView();
    if ( currentGraphicView == NULL) return;

    QTreeView* theTree = NULL;
    if ( sender() == currentGraphicView->getPatternDataSelectionModel() ){
        theTree = _patternDataTreeView;
    }
    /*else if ( sender() == currentGraphicView->getPatternBuilderSelectionModel() ){
        theTree = _patternBuilderTreeView;
    }*/
    else if ( sender() == currentGraphicView->getPatternGeomSelectionModel() ) {
        theTree =  _patternGeomTreeView;
    }
    else if ( sender() == currentGraphicView->getGroupsSelectionModel() ){
        theTree = _groupsTreeView;
    } else if ( sender() == currentGraphicView->getMeshSelectionModel() ){
        theTree = _meshTreeView;
    }
    if ( theTree ){
        theTree->scrollTo ( l[0] );
    }
}

bool HEXABLOCKGUI::createSComponent() //addComponent
{
    DEBTRACE("HEXABLOCKGUI::createSComponent");
    // --- Find or create (if not done yet) "HEXABLOCK" SComponent in the study

    _PTR(Study)            aStudy = (( SalomeApp_Study* )(getApp()->activeStudy()))->studyDS();
    _PTR(StudyBuilder)     aBuilder (aStudy->NewBuilder());
    _PTR(GenericAttribute) anAttr;
    _PTR(AttributeName)    aName;
    _PTR(AttributeGraphic) aGraphic;

    _PTR(SComponent) aComponent = aStudy->FindComponent("HEXABLOCK");
    if ( !aComponent ){ //Create hexablock component if there's not one yet
        aComponent = aBuilder->NewComponent("HEXABLOCK");
        anAttr = aBuilder->FindOrCreateAttribute(aComponent, "AttributeName");
        aName = _PTR(AttributeName) (anAttr);
        aName->SetValue(getApp()->moduleTitle("HEXABLOCK").toStdString());

        anAttr = aBuilder->FindOrCreateAttribute(aComponent, "AttributePixMap");
        _PTR(AttributePixMap) aPixmap(anAttr);
        aPixmap->SetPixMap(tr("ICO_MODULE_HEXABLOCK_SMALL").toStdString());

        aBuilder->DefineComponentInstance(aComponent, engineIOR().toStdString());
        DEBTRACE("HEXABLOCKGUI::createSComponent engineIOR=>"<<engineIOR().toStdString());
        return true;
    }
    return false;
}

void HEXABLOCKGUI::setResource(SUIT_ResourceMgr* r)
{
    DEBTRACE("HEXABLOCKGUI::setResource");
    _myresource = new HEXABLOCKGUI_Resource(r);
    _myresource->preferencesChanged();
}

void HEXABLOCKGUI::createPreferences()
{
    DEBTRACE("HEXABLOCKGUI::createPreferences");
    _myresource->createPreferences(this);
}

void HEXABLOCKGUI::preferencesChanged( const QString& sect, const QString& name )
{
    DEBTRACE("HEXABLOCKGUI::preferencesChanged");
    _myresource->preferencesChanged(sect, name);
//    if(name=="userCatalog")
//        _genericGui->getCatalogWidget()->addCatalogFromFile(Resource::userCatalog.toStdString());
}

void HEXABLOCKGUI::studyActivated() //CS_TODO
{
    int newStudyId = getApp()->activeStudy()->id();
    DEBTRACE("HEXABLOCKGUI::studyActivated " << _oldStudyId << " " << newStudyId);
//    if (_oldStudyId != -1)
//    {
//        _studyContextMap[_oldStudyId] = QtGuiContext::getQtCurrent();
//        if (_studyContextMap.count(newStudyId))
//        {
//            DEBTRACE("switch to valid context " << QtGuiContext::getQtCurrent() << " " << _studyContextMap[newStudyId]);
//            QtGuiContext::setQtCurrent(_studyContextMap[newStudyId]);
//        }
//        else
//        {
//            DEBTRACE("no switch to null context");
//        }
//    }
    _oldStudyId = newStudyId;
}

void HEXABLOCKGUI::treeContextMenu(const QPoint& aPosition)
{
    QModelIndex currentIndex = _patternDataTreeView->currentIndex();
    QVariant currentAssocVariant;
    QString currentAssocEntry;

    currentAssocVariant = currentIndex.data( HEXA_ASSOC_ENTRY_ROLE );
    currentAssocEntry    = currentIndex.data( HEXA_ASSOC_ENTRY_ROLE ).toString();
    if ( currentAssocVariant.isValid() && !currentAssocEntry.isEmpty() ){
        QMenu menu( _patternDataTreeView );
        //Remove association
        QAction *clearAct = menu.addAction( "Remove association(s)" );
        connect( clearAct, SIGNAL(triggered()), this, SLOT(clearAssociations()), Qt::UniqueConnection );
        menu.exec( _patternDataTreeView->mapToGlobal( aPosition) );
    }
    else {

        VtkDocumentGraphicView* currentVtkGraphicView = getCurrentVtkGraphicView();
        if (currentVtkGraphicView == NULL) return;
        QStandardItem *item = currentVtkGraphicView->getPatternDataModel()->itemFromIndex(currentIndex);

        //We don't do anything for single items
        if ( item->type() == VERTEXITEM || item->type() == EDGEITEM ||
                item->type() == QUADITEM || item->type() == HEXAITEM)
            return;

        //No associations for HEXA
        QVariant treeVariant = currentIndex.data( HEXA_TREE_ROLE );
        if ( !treeVariant.isValid() ) return;
        int eltType = treeVariant.toInt();
        if (eltType == HEXA_DIR_TREE) return;
        QMenu menu( _patternDataTreeView );

        //Show association(s)
        QAction *showAssocAct = menu.addAction( "Show associations" );
        connect( showAssocAct, SIGNAL(triggered()), this, SLOT(showAssociations()), Qt::UniqueConnection );

        //Clear all associations
        QAction *clearAllAssocAct = menu.addAction( "Remove all associations" );
        connect( clearAllAssocAct, SIGNAL(triggered()), this, SLOT(clearAllAssociations()), Qt::UniqueConnection );

        menu.exec( _patternDataTreeView->mapToGlobal( aPosition) );
    }
}


void HEXABLOCKGUI::createAndFillDockWidget()
{

    QMainWindow *aParent = application()->desktop();

    // Create dock widget (3 dock)

    //1) *********** user input panel ( contain user's edit dialog box )
    _dwInputPanel = new QDockWidget(aParent);
    _dwInputPanel->setVisible(false);
    _dwInputPanel->setWindowTitle("Input Panel");
    _dwInputPanel->setObjectName("hexablockInputPanelDock");
//    _dwInputPanel->setMinimumWidth(DWINPUT_MINIMUM_WIDTH); // --- force a minimum until display

//    _treeViewDelegate = new DocumentDelegate(_dwInputPanel);

    //2) ************* document data ( Pattern, Association, Mesh ) in treeview representation
    //      Pattern
    _dwPattern = new QDockWidget(aParent);
    _dwPattern->setVisible(false);
    _dwPattern->setWindowTitle("Model");
    _dwPattern->setMinimumWidth(DW_MINIMUM_WIDTH); // --- force a minimum until display
    _dwPattern->setObjectName("hexablockModelDock");

    QFrame*      patternFrame  = new QFrame(_dwPattern);
    patternFrame->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    QVBoxLayout* patternLayout = new QVBoxLayout(patternFrame);
    patternLayout->setSizeConstraint(QLayout::SetMaximumSize);
    QSplitter *splitter = new QSplitter(Qt::Vertical,patternFrame);
    _patternDataTreeView    = new QTreeView(patternFrame);//_dwPattern);
//    _patternBuilderTreeView = new QTreeView(patternFrame);                  // ---> TO REMOVE
    _patternGeomTreeView    = new QTreeView(patternFrame);
    splitter->addWidget(_patternDataTreeView);
//    splitter->addWidget(_patternBuilderTreeView);                           // ---> TO REMOVE
    splitter->addWidget(_patternGeomTreeView);
    patternLayout->addWidget(splitter);

    _patternDataTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers/*QAbstractItemView::DoubleClicked*/);
    _patternDataTreeView->setSelectionMode(QAbstractItemView::SingleSelection/*QAbstractItemView::MultiSelection*/);
//    _patternDataTreeView->setItemDelegate(_treeViewDelegate);


//    _patternBuilderTreeView->setEditTriggers(QAbstractItemView::AllEditTriggers);
//    _patternBuilderTreeView->setItemDelegate(_treeViewDelegate);

    _patternGeomTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _patternGeomTreeView->setSelectionMode(QAbstractItemView::SingleSelection/*QAbstractItemView::MultiSelection*/);

    _dwPattern->setWidget(patternFrame);
    patternFrame->show();

    //      Groups
    _dwGroups = new QDockWidget(aParent);

    _dwGroups->setVisible(false);
    _dwGroups->setWindowTitle("Groups");
    _dwGroups->setMinimumWidth(DW_MINIMUM_WIDTH); // --- force a minimum until display
    _dwGroups->setObjectName("hexablockGroupsDock");
    _groupsTreeView = new QTreeView(_dwGroups);
    _groupsTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    _groupsTreeView->setItemDelegate(_treeViewDelegate);
    _dwGroups->setWidget(_groupsTreeView);
    _groupsTreeView->show();

    //      Mesh
    _dwMesh = new QDockWidget(aParent);
    _dwMesh->setVisible(false);
    _dwMesh->setWindowTitle("Mesh");
    _dwMesh->setMinimumWidth(DW_MINIMUM_WIDTH); // --- force a minimum until display
    _dwMesh->setObjectName("hexablockMeshDock");
    _meshTreeView = new QTreeView(_dwMesh);
    _meshTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _meshTreeView->setSelectionMode(QAbstractItemView::SingleSelection);
//    _meshTreeView->setItemDelegate(_treeViewDelegate);
    _dwMesh->setWidget(_meshTreeView);
    _meshTreeView->show();

//    QDockWidget *_dwObjectBrowser = 0;
    QWidget* wid = getApp()->objectBrowser()->treeView();
    QWidget *w   = wid->parentWidget();
    while ( w && !_dwObjectBrowser ) {
        _dwObjectBrowser = ::qobject_cast<QDockWidget*>( w );
        w = w->parentWidget();
    }
    _dwObjectBrowser->setMinimumWidth(DW_MINIMUM_WIDTH); // --- force a minimum until display
    _dwObjectBrowser->setWindowTitle("Study");

    // dock widget position
    aParent->addDockWidget( Qt::LeftDockWidgetArea, _dwObjectBrowser );
    aParent->addDockWidget( Qt::RightDockWidgetArea, _dwInputPanel );

//    aParent->tabifyDockWidget( _dwPattern, _dwObjectBrowser );
//    aParent->tabifyDockWidget( _dwGroups, _dwPattern );
//    aParent->tabifyDockWidget( _dwMesh, _dwGroups );

    aParent->tabifyDockWidget( _dwObjectBrowser, _dwPattern );
    aParent->tabifyDockWidget( _dwPattern, _dwGroups );
    aParent->tabifyDockWidget( _dwGroups, _dwMesh );

#if QT_VERSION >= 0x040500
    aParent->setTabPosition(Qt::AllDockWidgetAreas, Resource::tabPanelsUp? QTabWidget::North: QTabWidget::South);
#endif

    // popup menu on data tree view
    _patternDataTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_patternDataTreeView,
            SIGNAL(customContextMenuRequested(const QPoint &)),
            this,
            SLOT(treeContextMenu(const QPoint &)),
            Qt::UniqueConnection );
}

void HEXABLOCKGUI::createActions()
{
    QMainWindow *aParent = application()->desktop();
    SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

    // Document
    _newAct = createAction(_menuId++,
            tr("Create a new document"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_NEW_DOCUMENT")),
            tr("New Document"),
            tr("Create a new document"),
            0, aParent, false, this, SLOT(newDocument()));
    _newAct->setShortcut( Qt::CTRL + Qt::SHIFT + Qt::Key_N ); // --- QKeySequence::New ambiguous in SALOME

    _importAct = createAction(_menuId++,
            tr("Load a document"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_LOAD_DOCUMENT")),
            tr("Load Document"),
            tr("Load a document"),
            0, aParent, false, this, SLOT(loadDocument()));
    _importAct->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_O); // --- QKeySequence::Open ambiguous in SALOME

    _saveAct = createAction(_menuId++,
            tr("Save the document"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_SAVE_DOCUMENT")),
            tr("Save Document"),
            tr("Save the document"),
            0, aParent, false, this, SLOT(saveDocument()));

    // Pattern Data creation
    _addVertex = createAction(_menuId++,
            tr("Create a vertex"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_ADD_VERTEX")),
            tr("Add Vertex"),
            tr("Create a new vertex"),
            0, aParent, false, this, SLOT(addVertex()));

    _addEdge = createAction(_menuId++,
            tr("Create an edge"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_ADD_EDGE")),
            tr("Add Edge"),
            tr("Create a new edge"),
            0, aParent, false, this, SLOT(addEdge()));

    _addQuad = createAction(_menuId++,
            tr("Create a quadrangle"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_ADD_QUAD")),
            tr("Add Quadrangle"),
            tr("Create a new quadrangle"),
            0, aParent, false, this, SLOT(addQuad()));

    _addHexa = createAction(_menuId++,
            tr("Create an hexahedron"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_ADD_HEXA")),
            tr("Add Hexahedron"),
            tr("Create a new hexahedron"),
            0, aParent, false, this, SLOT(addHexa()));

    // Builder Data creation
    _addVector = createAction(_menuId++,
            tr("Create a vector"),
            resMgr->loadPixmap( "HEXABLOCK", tr("ICON_ADD_VECTOR")),
            tr("Add Vector"),
            tr("Create a new vector"),
            0, aParent, false, this, SLOT(addVector()));

//    _addCylinder = createAction(_menuId++,
//            tr("Create a cylinder"),
//            resMgr->loadPixmap("HEXABLOCK", tr("ICON_ADD_CYLINDER")),
//            tr("Add Cylinder"),
//            tr("Create a new cylinder"),
//            0, aParent, false, this,  SLOT(addCylinder()));

//    _addPipe = createAction(_menuId++,
//            tr("Create a pipe"),
//            resMgr->loadPixmap("HEXABLOCK", tr("ICON_ADD_PIPE")),
//            tr("Add Pipe"),
//            tr("Create a new pipe"),
//            0, aParent, false, this, SLOT(addPipe()));

    _makeGrid = createAction(_menuId++,
            tr("Make a grid"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_MAKE_GRID")),
            tr("Make Grid"),
            tr("Make a grid"),
            0, aParent, false, this,  SLOT(makeGrid()));

    _makeCylinder = createAction(_menuId++,
            tr("Make a cylinder"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_MAKE_CYLINDER")),
            tr("Make Cylinder"),
            tr("Make a cylinder"),
            0, aParent, false, this, SLOT(makeCylinder()));

    _makePipe = createAction(_menuId++,
            tr("Make a pipe"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_MAKE_PIPE")),
            tr("Make Pipe"),
            tr("Make a pipe"),
            0, aParent, false, this, SLOT(makePipe()));

    _makeCylinders = createAction(_menuId++,
            tr("Make cylinders"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_MAKE_CYLINDERS")),
            tr("Make Cylinders"),
            tr("Make cylinders"),
            0, aParent, false, this, SLOT(makeCylinders()));

    _makePipes = createAction(_menuId++,
            tr("Make pipes"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_MAKE_PIPES")),
            tr("Make Pipes"),
            tr("Make pipes"),
            0, aParent, false, this, SLOT(makePipes()));

    _makeHemiSphere = createAction(_menuId++,
            tr("Make an hemisphere"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_MAKE_HEMISPHERE")),
            tr("Make Hemisphere"),
            tr("Make an hemisphere"),
            0, aParent, false, this, SLOT(makeHemiSphere()));

    // Pattern Data edition
    _removeHexa = createAction(_menuId++,
            tr("Remove hexahedra"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_REMOVE_HEXA")),
            tr("Remove Hexahedra"),
            tr("Remove hexahedra"),
            0, aParent, false, this, SLOT(removeHexa()));

    _prismQuad = createAction(_menuId++,
            tr("Prism quadrangles"),
            resMgr->loadPixmap( "HEXABLOCK", tr("ICON_PRISM_QUAD")),
            tr("Prism Quadrangles"),
            tr("Prism quadrangles"),
            0, aParent, false, this, SLOT(prismQuad()));

    _joinQuad = createAction(_menuId++,
            tr("Join quadrangles"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_JOIN_QUAD")),
            tr("Join Quadrangles"),
            tr("Join quadrangles with hexahedra"),
            0, aParent, false, this, SLOT(joinQuad()));

    _merge = createAction(_menuId++,
            tr("Merge"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_MERGE")),
            tr("Merge"),
            tr("Merge"),
            0, aParent, false, this, SLOT(merge()));

    _disconnect = createAction(_menuId++,
            tr("Disconnect"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_DISCONNECT")),
            tr("Disconnect"),
            tr("Disconnect edges, vertexes or quadrangles"),
            0, aParent, false, this, SLOT(disconnectElts()));

    _cutEdge = createAction(_menuId++,
            tr("Cut edge"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_CUT_EDGE")),
            tr("Cut Edge"),
            tr("Cut edge"),
            0, aParent, false, this, SLOT(cutEdge()));

    _makeTransformation = createAction(_menuId++,
            tr("Make transformation"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_MAKE_TRANSFORMATION")),
            tr("Make Transformation"),
            tr("Make transformation"),
            0, aParent, false, this, SLOT(makeTransformation()));

    _makeSymmetry = createAction(_menuId++,
            tr("Make symmetry"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_MAKE_SYMMETRY")),
            tr("Make Symmetry"),
            tr("Make symmetry"),
            0, aParent, false, this, SLOT(makeSymmetry()));

    _performTransformation = createAction(_menuId++,
            tr("Perform transformation"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_PERFORM_TRANSFORMATION")),
            tr("Perform Transformation"),
            tr("Perform transformation"),
            0, aParent, false, this, SLOT(performTransformation()));

    _performSymmetry = createAction(_menuId++,
            tr("Perform symmetry"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_PERFORM_SYMMETRY")),
            tr("Perform Symmetry"),
            tr("Perform symmetry"),
            0, aParent, false, this, SLOT(performSymmetry()));

    _replaceHexa = createAction(_menuId++,
            tr("Replace hexahedron"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_REPLACE_HEXA")),
            tr("Replace Hexahedron"),
            tr("Replace hexahedron"),
            0, aParent, false, this, SLOT(replaceHexa()));

    _quadRevolution = createAction(_menuId++,
            tr("Quad revolution"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_QUAD_REVOLUTION")),
            tr("Quad Revolution"),
            tr("Quad revolution"),
            0, aParent, false, this, SLOT(quadRevolution()));

    // Association
    _assocVertex = createAction(_menuId++,
            tr("Make vertex association"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_VERTEX_ASSOCIATION")),
            tr("Make Vertex association"),
            tr("Make vertex association"),
            0, aParent, false, this, SLOT(assocVertex()));

    _assocEdge = createAction(_menuId++,
            tr("Make edge association"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_EDGE_ASSOCIATION")),
            tr("Make Edge Association"),
            tr("Make edge association"),
            0, aParent, false, this, SLOT(assocEdge()));

    _assocQuad = createAction(_menuId++,
            tr("Make quad association"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_QUAD_ASSOCIATION")),
            tr("Make Quad Association"),
            tr("Make quad association"),
            0, aParent, false, this, SLOT(assocQuad()));

    // Group
    _addGroup = createAction(_menuId++,
            tr("Add a group"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_ADD_GROUP")),
            tr("Add Group"),
            tr("Add a group"),
            0, aParent, false, this, SLOT(addGroup()));

    _removeGroup = createAction(_menuId++,
            tr("Remove a group"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_REMOVE_GROUP")),
            tr("Remove Group"),
            tr("Remove a group"),
            0, aParent, false, this, SLOT(removeGroup()));

    // Mesh
    _addLaw = createAction(_menuId++,
            tr("Add a law"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_ADD_LAW")),
            tr("Add Law"),
            tr("Add a law"),
            0, aParent, false, this, SLOT(addLaw()));

    _removeLaw = createAction(_menuId++,
            tr("Remove a law"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_REMOVE_LAW")),
            tr("Remove Law"),
            tr("Remove a law"),
            0, aParent, false, this, SLOT(removeLaw()));

    _setPropagation = createAction(_menuId++,
            tr("Set propagation"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_SET_PROPAGATION")),
            tr("Set Propagation"),
            tr("Set propagation"),
            0, aParent, false, this, SLOT(setPropagation()));

    _computeMesh = createAction(_menuId++,
            tr("Compute mesh"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_COMPUTE_MESH")),
            tr("Compute Mesh"),
            tr("Compute mesh"),
            0, aParent, false, this, SLOT(computeMesh()));

    _showModelInfoAct = createAction(_menuId++,
            tr("Show Model Info"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_MODEL_INFO")),
            tr("MODEL_INFO"),
            tr("MODEL_INFO"),
            0, aParent, false, this, SLOT(showModelInfo()));

    _addShapeAct = createAction(_menuId++,
            tr("Add Shape"),
            resMgr->loadPixmap("HEXABLOCK", tr("ICON_GEOM_ASSOCIATION")),
            tr("ADD_SHAPE"),
            tr("ADD_SHAPE"),
            0, aParent, false, this, SLOT(addShape()));

//   _newAct->setShortcut( Qt::CTRL + Qt::SHIFT + Qt::Key_N ); // --- QKeySequence::New ambiguous in SALOME
//   QAction* createAction(const int id,
//                         const QString& toolTip,
//                         const QIcon& icon,
//                         const QString& menu,
//                         const QString& status,
//                         const int shortCut,
//                         QObject* parent =0,
//                         bool checkable = false,
//                         QObject* receiver =0,
//                         const char* member =0);

    _sep1 =  separator();
    _sep2 =  separator();
    _sep3 =  separator();
    _sep4 =  separator();
}


void HEXABLOCKGUI::createMenus()
{
    int aMenuId, subMenuId,aMenuIdMain;

    aMenuIdMain = createMenu( tr("MEN_FILE"), -1, -1);

    aMenuId = createMenu( tr("HEXABLOCK"), aMenuIdMain, -1, 10 );
    createMenu( actionId(_newAct), aMenuId );
    createMenu( actionId(_importAct), aMenuId );
    createMenu( _saveAct, aMenuId );


    int aMenuDocId = createMenu( tr( "Document" ), -1, -1, 30 );
    createMenu( _newAct, aMenuDocId );//, 10
    createMenu( _importAct, aMenuDocId );
    createMenu( _saveAct, aMenuDocId );

    aMenuId = createMenu( tr("Model"), -1, -1, 30 );
    subMenuId = createMenu( tr( "Construction" ), aMenuId, -1 );
    createMenu( _addVertex, subMenuId );
    createMenu( _addEdge,   subMenuId );
    createMenu( _addQuad,   subMenuId );
    createMenu( _addHexa,   subMenuId );
    createMenu( _sep1, subMenuId);

    // Pattern Builder
    createMenu( _addVector, subMenuId );
//    createMenu( _addCylinder, subMenuId );
//    createMenu( _addPipe, subMenuId );

    createMenu( _sep2, subMenuId);

    createMenu( _makeGrid,  subMenuId ); //Cartesian, Cylindrical, Spherical
    createMenu( _makeHemiSphere,subMenuId );

    createMenu( _sep3, subMenuId);

    createMenu( _makeCylinder, subMenuId );
    createMenu( _makePipe,     subMenuId );
    createMenu( _makeCylinders,subMenuId );
    createMenu( _makePipes,    subMenuId );

    // Pattern Data Edition
    subMenuId = createMenu( tr( "Operation" ), aMenuId, -1 );
    createMenu( _removeHexa, subMenuId );
    createMenu( _prismQuad,  subMenuId );
    createMenu( _joinQuad,   subMenuId );
    createMenu( _merge,      subMenuId );
    createMenu( _disconnect, subMenuId );
    createMenu( _cutEdge,    subMenuId );
    createMenu( _makeTransformation, subMenuId );
    createMenu( _makeSymmetry,       subMenuId );
    createMenu( _performTransformation,   subMenuId );
    createMenu( _performSymmetry,         subMenuId );
    createMenu( _replaceHexa,    subMenuId );
    createMenu( _quadRevolution, subMenuId );
    createMenu( _sep4, aMenuId );
    createMenu( _showModelInfoAct, aMenuId );

    // Association
    aMenuId = createMenu( tr("Association"), -1, -1, 30 );
    createMenu( _addShapeAct, aMenuId );
    createMenu( _assocVertex, aMenuId );
    createMenu( _assocEdge,   aMenuId );
    createMenu( _assocQuad,   aMenuId );

    // Group
    aMenuId = createMenu( tr("Groups"), -1, -1, 30 );
    createMenu( _addGroup,    aMenuId );
    createMenu( _removeGroup, aMenuId ); //CS_TODO

    // Law
    aMenuId = createMenu( tr("Mesh "), -1, -1, 30 );
    createMenu( _addLaw,    aMenuId );
    createMenu( _removeLaw, aMenuId );
    createMenu( _setPropagation, aMenuId );
    createMenu( _computeMesh, aMenuId );

    //show model infomation menu
    QString aRule = "$component={'HEXABLOCK'}";
    popupMgr()->insert( _showModelInfoAct, -1, -1 );
    popupMgr()->setRule( _showModelInfoAct, aRule);

    //associate shape to a document menu
    popupMgr()->insert( _addShapeAct, -1, -1 );
    popupMgr()->setRule( _addShapeAct, aRule );

    //-------------------------------------------------
    // Display / Erase
    //-------------------------------------------------
    //  QString lc = "$";        // VSR : instead of QtxPopupSelection::defEquality();
    //  QString dc = "selcount"; // VSR : instead of QtxPopupSelection::defSelCountParam()
    //  QString isNotEmpty("numberOfNodes <> 0");
    //  QString aClient = QString( "%1client in {%2}" ).arg( lc ).arg( "'VTKViewer'" );
    //  QString aType = QString( "%1type in {%2}" ).arg( lc );
    ////  aType = aType.arg( mesh_part );
    //
    //  aClient = "($client in {'VTKViewer' 'ObjectBrowser'})";
    //  QString anActiveVTK = QString("activeView = '%1'").arg(SVTK_Viewer::Type());
    //  QString aSelCount = QString( "%1 > 0" ).arg( dc );

//    popupMgr()->insert( separator(), -1, -1 );
//    QString aRule = "$component={'HEXABLOCK'}";/* and  ( type='(" + aClient + " and " +
//		  aType + " and " + aSelCount + " and " + anActiveVTK + " and " + isNotEmpty + " %1 ) )";*/
//    //  MESSAGE("aClient = " << aClient.toStdString());
//    //  MESSAGE("aType = " << aType.toStdString());
//    //  MESSAGE("aSelCount = " << aSelCount.toStdString());
//    //  MESSAGE("anActiveVTK = " << anActiveVTK.toStdString());
//    //  MESSAGE("isNotEmpty = " << isNotEmpty.toStdString());
//    //  MESSAGE("aRule = " << aRule.toStdString());
//    popupMgr()->insert( _showAct, -1, -1 ); // DISPLAY
//    popupMgr()->setRule( _showAct, aRule.arg( "and (not isVisible)" ), QtxPopupMgr::VisibleRule );
//
//    popupMgr()->insert( _hideAct, -1, -1 ); // ERASE
//    popupMgr()->setRule( _hideAct, aRule.arg( "and isVisible" ), QtxPopupMgr::VisibleRule );
//
//    popupMgr()->insert( _showOnlyAct, -1, -1 ); // DISPLAY_ONLY
//    popupMgr()->setRule(_showOnlyAct, aRule.arg( "" ), QtxPopupMgr::VisibleRule );
//
//    popupMgr()->insert( separator(), -1, -1 );
}


void HEXABLOCKGUI::createTools()
{
    int aToolId;

    aToolId = createTool ( tr( "HexaBlock Toolbar" ), QString( "HexaBlockMain" ) );
    createTool( _newAct, aToolId );
    createTool( _importAct, aToolId );
    createTool( _saveAct, aToolId );
    createTool( _showModelInfoAct, aToolId );
    //   createTool( _testAct, aToolId );
    //createTool( separator(), aToolId );

    // Pattern Data
    aToolId = createTool ( tr( "Construction" ), QString( "HexaBlockConstruction" ) );
    createTool( _addVertex, aToolId );
    createTool( _addEdge, aToolId );
    createTool( _addQuad, aToolId );
    createTool( _addHexa, aToolId );

    createTool( _sep1, aToolId );

    // Pattern Builder
    createTool( _addVector, aToolId );
//    createTool( _addCylinder, aToolId );
//    createTool( _addPipe, aToolId );

    createTool( _sep2, aToolId );

    createTool( _makeGrid,  aToolId );
    createTool( _makeHemiSphere,   aToolId );

    createTool( _sep3, aToolId );

    createTool( _makeCylinder, aToolId );
    createTool( _makePipe,     aToolId );
    createTool( _makeCylinders,aToolId );
    createTool( _makePipes,    aToolId );

    // Pattern Data Edition
    aToolId = createTool ( tr( "Operation" ), QString( "HexaBlockOperation" ) );
    createTool( _removeHexa, aToolId );
    createTool( _prismQuad,  aToolId );
    createTool( _joinQuad,   aToolId );
    createTool( _merge,      aToolId );
    createTool( _disconnect, aToolId );
    createTool( _cutEdge,    aToolId );
    createTool( _makeTransformation, aToolId );
    createTool( _makeSymmetry,       aToolId );
    createTool( _performTransformation,   aToolId );
    createTool( _performSymmetry,         aToolId );
    createTool( _replaceHexa,    aToolId );
    createTool( _quadRevolution, aToolId );

    // Association
    aToolId = createTool ( tr( "Association" ), QString( "HexaBlockAssociation" ) );
    createTool( _assocVertex, aToolId );
    createTool( _assocEdge, aToolId );
    createTool( _assocQuad, aToolId );
    createTool( _addShapeAct, aToolId );

    // Group
    aToolId = createTool ( tr( "Group" ), QString( "HexaBlockGroup" ) );
    createTool( _addGroup,    aToolId );
    createTool( _removeGroup, aToolId ); //CS_TODO

    // Law
    aToolId = createTool ( tr( "Mesh" ), QString( "HexaBlockMesh" ) );
    createTool( _addLaw,    aToolId );
    createTool( _removeLaw, aToolId );
    createTool( _setPropagation, aToolId );
    createTool( _computeMesh, aToolId );

}

void HEXABLOCKGUI::initialMenus()
{
    showDocumentMenus( true );
    showPatternMenus( false );
    showAssociationMenus( false );
    showGroupsMenus( false );
    showMeshMenus( false );
}

void HEXABLOCKGUI::showAllMenus()
{
    showDocumentMenus( true );
    showPatternMenus( true );
    showAssociationMenus( true );
    showGroupsMenus( true );
    showMeshMenus( true );
}

void HEXABLOCKGUI::setAllMenusEnabled(bool enable)
{
    enableDocumentMenus( enable );
    enablePatternMenus( enable );
    enableAssociationMenus( enable );
    enableGroupsMenus( enable );
    enableMeshMenus( enable );

    menusEnabled = enable;
}

void HEXABLOCKGUI::showDocumentMenus(bool show)
{
    DEBTRACE("HEXABLOCKGUI::showDocumentMenus " << show);
    show = true; //
    setMenuShown(_newAct, show);
    setToolShown(_newAct, show);
    setMenuShown(_importAct, show);
    setToolShown(_importAct, show);
    setMenuShown(_showModelInfoAct, show);
    setToolShown(_showModelInfoAct, show);
    setMenuShown(_saveAct, show);
    setToolShown(_saveAct, show);
}

void HEXABLOCKGUI::enableDocumentMenus(bool enable)
{
    _newAct->setEnabled(enable);
//    setToolShown(_newAct, enable);

    _importAct->setEnabled(enable);
//    setToolShown(_importAct, enable);

    _showModelInfoAct->setEnabled(enable);
//    setToolShown(_showModelInfoAct, enable);

    _saveAct->setEnabled(enable);
//    setToolShown(_saveAct, enable);
}


void HEXABLOCKGUI::showPatternMenus(bool show)
{
    DEBTRACE("HEXABLOCKGUI::showPatternMenus " << show);
    DEBTRACE("getCurrentModel()  " << getCurrentModel()  );
    if ( show && !getCurrentModel() ) return;

    setMenuShown(_addVertex, show );
    setToolShown(_addVertex, show);
    setMenuShown(_addEdge,  show );
    setToolShown(_addEdge, show);
    setMenuShown(_addQuad,  show );
    setToolShown(_addQuad, show);
    setMenuShown(_addHexa,  show );
    setToolShown(_addHexa, show);

    setMenuShown(_sep1, show );
    setToolShown(_sep1, show);

    setMenuShown( _addVector,  show );
    setToolShown( _addVector, show);
//    setMenuShown( _addCylinder,  show );
//    setToolShown( _addCylinder, show);
//    setMenuShown( _addPipe,  show );
//    setToolShown( _addPipe, show);

    setMenuShown(_sep2, show );
    setToolShown(_sep2, show);

    setMenuShown( _makeGrid,  show );
    setToolShown( _makeGrid, show);
    setMenuShown( _makePipe,  show );
    setToolShown( _makePipe, show);

    setMenuShown(_sep3, show );
    setToolShown(_sep3, show);

    setMenuShown( _makeCylinder,  show );
    setToolShown( _makeCylinder, show);
    setMenuShown( _makeCylinders,  show );
    setToolShown( _makeCylinders, show);
    setMenuShown( _makePipes,  show );
    setToolShown( _makePipes, show);
    setMenuShown( _makeHemiSphere,  show );
    setToolShown( _makeHemiSphere, show);

    // Pattern Data Edition
    setMenuShown( _removeHexa,  show );
    setToolShown( _removeHexa, show);
    setMenuShown( _prismQuad,  show );
    setToolShown( _prismQuad, show);
    setMenuShown( _joinQuad,  show );
    setToolShown( _joinQuad, show);
    setMenuShown( _merge,  show );
    setToolShown( _merge, show);
    setMenuShown( _disconnect,  show );
    setToolShown( _disconnect, show);
    setMenuShown( _cutEdge,  show );
    setToolShown( _cutEdge, show);
    setMenuShown( _makeTransformation,  show );
    setToolShown( _makeTransformation, show);
    setMenuShown( _makeSymmetry,  show );
    setToolShown( _makeSymmetry, show);
    setMenuShown( _performTransformation,  show );
    setToolShown( _performTransformation, show);
    setMenuShown( _performSymmetry,  show );
    setToolShown( _performSymmetry, show);
    setMenuShown( _replaceHexa,  show );
    setToolShown( _replaceHexa, show);
    setMenuShown( _quadRevolution,  show );
    setToolShown( _quadRevolution, show);

    setMenuShown( _sep4, show );
    setMenuShown( _showModelInfoAct, show );
}

void HEXABLOCKGUI::enablePatternMenus(bool enable)
{
    if ( enable && !getCurrentModel() ) return;

    _addVertex->setEnabled(enable);
//    setToolShown(_addVertex, enable);

    _addEdge->setEnabled(enable);
//    setToolShown(_addEdge, enable);

    _addQuad->setEnabled(enable);
//    setToolShown(_addQuad, enable);

    _addHexa->setEnabled(enable);
//    setToolShown(_addHexa, enable);

//    setMenuShown(_sep1, enable );
//    setToolShown(_sep1, enable);

    _addVector->setEnabled(enable);
//    setToolShown( _addVector, enable);

//    setMenuShown(_sep2, enable );
//    setToolShown(_sep2, enable);

    _makeGrid->setEnabled(enable);
//    setToolShown( _makeGrid, enable);

    _makePipe->setEnabled(enable);
//    setToolShown( _makePipe, enable);

//    setMenuShown(_sep3, enable );
//    setToolShown(_sep3, enable);

    _makeCylinder->setEnabled(enable);
//    setToolShown( _makeCylinder, enable);

    _makeCylinders->setEnabled(enable);
//    setToolShown( _makeCylinders, enable);

    _makePipes->setEnabled(enable);
//    setToolShown( _makePipes, enable);

    _makeHemiSphere->setEnabled(enable);
//    setToolShown( _makeHemiSphere, enable);

    // Pattern Data Edition
    _removeHexa->setEnabled(enable);
//    setToolShown( _removeHexa, enable);

    _prismQuad->setEnabled(enable);
//    setToolShown( _prismQuad, enable);

    _joinQuad->setEnabled(enable);
//    setToolShown( _joinQuad, enable);

    _merge->setEnabled(enable);
//    setToolShown( _merge, enable);

    _disconnect->setEnabled(enable);
//    setToolShown( _disconnect, enable);

    _cutEdge->setEnabled(enable);
//    setToolShown( _cutEdge, enable);

    _makeTransformation->setEnabled(enable);
//    setToolShown( _makeTransformation, enable);

    _makeSymmetry->setEnabled(enable);
//    setToolShown( _makeSymmetry, enable);

    _performTransformation->setEnabled(enable);
//    setToolShown( _performTransformation, enable);

    _performSymmetry->setEnabled(enable);
//    setToolShown( _performSymmetry, enable);

    _replaceHexa->setEnabled(enable);
//    setToolShown( _replaceHexa, enable);

    _quadRevolution->setEnabled(enable);
//    setToolShown( _quadRevolution, enable);

//    setMenuShown( _sep4, enable );
    _showModelInfoAct->setEnabled(enable);
}


void HEXABLOCKGUI::showAssociationMenus(bool show)
{
    DEBTRACE("HEXABLOCKGUI::showAssociationMenus" << show);
    if ( show && !getCurrentModel() ) return;

    // Association Edition
    setMenuShown( _assocVertex,  show );
    setToolShown( _assocVertex, show );

    setMenuShown( _assocEdge,  show );
    setToolShown( _assocEdge, show );

    setMenuShown( _assocQuad,  show );
    setToolShown( _assocQuad, show );

    setMenuShown( _addShapeAct, show );
    setToolShown( _addShapeAct, show );
}

void HEXABLOCKGUI::enableAssociationMenus(bool enable)
{
    if ( enable && !getCurrentModel() )
        return;

    // Association Edition
     _assocVertex->setEnabled(enable);
 //    setToolShown( _assocVertex, enable );

     _assocEdge->setEnabled(enable);
 //    setToolShown( _assocEdge, enable );

     _assocQuad->setEnabled(enable);
 //    setToolShown( _assocQuad, enable );

     _addShapeAct->setEnabled(enable);
 //    setToolShown( _addShapeAct, enable );
}

void HEXABLOCKGUI::showGroupsMenus(bool show)
{
    DEBTRACE("HEXABLOCKGUI::showGroupsMenus" << show);
    if ( show && !getCurrentModel() ) return;
    setMenuShown( _addGroup,  show );
    setToolShown( _addGroup, show);
    setMenuShown( _removeGroup ,  show );
    setToolShown( _removeGroup , show);
}

void HEXABLOCKGUI::enableGroupsMenus(bool enable)
{
    if ( enable && !getCurrentModel() )
        return;

    _addGroup->setEnabled(enable);
//    setToolShown( _addGroup, enable);

    _removeGroup->setEnabled(enable);
//    setToolShown( _removeGroup , enable);
}

void HEXABLOCKGUI::showMeshMenus(bool show)
{
    DEBTRACE("HEXABLOCKGUI::showMeshMenus" << show);
    if ( show && !getCurrentModel() ) return;
    setMenuShown( _addLaw,  show );
    setToolShown( _addLaw, show);
    setMenuShown( _removeLaw,  show );
    setToolShown( _removeLaw, show);;
    setMenuShown( _setPropagation,  show );
    setToolShown( _setPropagation, show);
    setMenuShown( _computeMesh, show);
    setToolShown( _computeMesh, show);
}

void HEXABLOCKGUI::enableMeshMenus(bool enable)
{
    if ( enable && !getCurrentModel() )
        return;

    _addLaw->setEnabled(enable);
//    setToolShown( _addLaw, enable);

    _removeLaw->setEnabled(enable);
//    setToolShown( _removeLaw, enable);

    _setPropagation->setEnabled(enable);
//    setToolShown( _setPropagation, enable);

    _computeMesh->setEnabled(enable);
//    setToolShown( _computeMesh, enable);
}

void HEXABLOCKGUI::showVtkActor()
{
    VtkDocumentGraphicView* currentVtkGView = getCurrentVtkGraphicView();
    if (currentVtkGView == NULL || currentVtkGView->isEmpty() ||
            currentVtkGView->getDocumentActor() == NULL)
        return;

    SVTK_ViewWindow* vtkView = currentVtkGView->getViewWindow();
    if (vtkView == NULL)
        return;

    SUIT_ViewManager* vman = vtkView->getViewManager();
    SalomeApp_Study* aStudy = HEXABLOCKGUI::activeStudy();

    //show only the current actor -----------------
    vtkView->setFocus();
    //    vtkView->DisplayOnly(currentVtkGView->getDocumentActor()->getIO());
    Document_Actor *lastDocActor;
    if (lastVtkDocGView != NULL)
    {
        lastDocActor = lastVtkDocGView->getDocumentActor();
        Handle(SALOME_InteractiveObject) lastActorIO = lastDocActor->getIO();
        if (!lastActorIO.IsNull() && lastActorIO->hasEntry())
        {
            vtkView->Erase(lastActorIO);
            aStudy->setObjectProperty(vman->getId(), lastActorIO->getEntry(),  "Visibility", 0 );
            displayer()->setVisibilityState(lastActorIO->getEntry(), Qtx::HiddenState);
        }
    }
    currentVtkGView->update();
    Handle(SALOME_InteractiveObject) anIO = currentVtkGView->getDocumentActor()->getIO();
    if (!anIO.IsNull() && anIO->hasEntry())
    {
        vtkView->Display(anIO);
        aStudy->setObjectProperty(vman->getId(), anIO->getEntry(),  "Visibility", 1 );
        displayer()->setVisibilityState(anIO->getEntry(), Qtx::ShownState);
    }
    vtkView->onFitAll();
}

void HEXABLOCKGUI::showOccActor()
{
    if (currentOccGView == NULL)
        return;
    OCCViewer_ViewWindow* occView = currentOccGView->getViewWindow();
    if (occView == NULL)
        return;
    SALOME_View* vf = dynamic_cast<SALOME_View*>(occView->getViewManager()->getViewModel());
    if (vf == NULL)
        return;

    if (lastOccPrs != NULL)
        vf->Erase(lastOccPrs);
    currentOccGView->globalSelection();
    SOCC_Prs* prs = getOccPrs(currentDocGView);
    currentOccGView->setPrs(prs);
    vf->Display(prs);
    vf->Repaint();
    occView->onFitAll();
}

void HEXABLOCKGUI::hideVtkActor()
{
    VtkDocumentGraphicView* currentVtkGView = getCurrentVtkGraphicView();
    if (currentVtkGView == NULL || currentVtkGView->isEmpty() ||
        currentVtkGView->getViewWindow() == NULL ||
        currentVtkGView->getDocumentActor() == NULL) return;

    currentVtkGView->getViewWindow()->Erase(currentVtkGView->getDocumentActor()->getIO());
    currentVtkGView->getViewWindow()->onResetView();

    //update the visibility state now
    SalomeApp_Study* aStudy = HEXABLOCKGUI::activeStudy();
    SUIT_ViewManager* vman = currentVtkGView->getViewWindow()->getViewManager();
    if (aStudy == NULL || vman == NULL) return;

    Handle(SALOME_InteractiveObject) anIO = currentVtkGView->getDocumentActor()->getIO();
    aStudy->setObjectProperty(vman->getId(), anIO->getEntry(),  "Visibility", 0 );
    displayer()->setVisibilityState(anIO->getEntry(), Qtx::HiddenState);
}

void HEXABLOCKGUI::hideOccActor()
{
    OCCViewer_ViewWindow* occView = currentOccGView == NULL ? NULL : currentOccGView->getViewWindow();
    VtkDocumentGraphicView* currentVtkGView = getCurrentVtkGraphicView();
    DocumentModel* docModel = (currentVtkGView == NULL ? NULL : currentVtkGView->getDocumentModel());
    if (occView == NULL || docModel == NULL)
        return;
    SALOME_View* vf = dynamic_cast<SALOME_View*>(occView->getViewManager()->getViewModel());
    if (vf == NULL)
        return;

    SOCC_Prs* currentOccPrs = getOccPrs(currentDocGView);
    if (currentOccPrs != NULL)
        vf->Erase(currentOccPrs);
    vf->Repaint();
    occView->onResetView();
}

void HEXABLOCKGUI::showOnlyActor()
{
    showVtkActor();
    showOccActor();
}

void HEXABLOCKGUI::hideActor()
{
    hideVtkActor();
    hideOccActor();
}


void HEXABLOCKGUI::showDockWidgets(bool isVisible)
{
    DEBTRACE("HEXABLOCKGUI::showDockWidgets " << isVisible);

    if (_dwObjectBrowser) _dwObjectBrowser->setVisible(isVisible);

    if (_dwPattern) _dwPattern->setVisible(isVisible);
    if (_dwPattern) _dwPattern->toggleViewAction()->setVisible(isVisible);

    if (_dwMesh) _dwMesh->setVisible(isVisible);
    if (_dwMesh) _dwMesh->toggleViewAction()->setVisible(isVisible);

    if (_dwGroups) _dwGroups->setVisible(isVisible);
    if (_dwGroups) _dwGroups->toggleViewAction()->setVisible(isVisible);
}


//clear all used dialogs by the current model
void HEXABLOCKGUI::clearDialogs()
{
    std::set<HexaBaseDialog*>::const_iterator debut (currentModelDialogs.begin()),
                                              fin   (currentModelDialogs.end());
    for(;debut!=fin;++debut) (*debut)->clear();

    currentModelDialogs.clear(); //empty the used dialogs list
}

HexaBaseDialog* HEXABLOCKGUI::getDlgBox(VtkDocumentGraphicView* dgview)
{
    if (!gViewDlgBox.contains(dgview)) return NULL;
    return gViewDlgBox[dgview];
}

void HEXABLOCKGUI::switchOnGraphicView(VtkDocumentGraphicView* dgview)
{
   if (dgview == NULL) return;

   switchOffGraphicView(dgview, false); //to avoid double connect

   //connect signals
   connect( selectionMgr(), SIGNAL( currentSelectionChanged() ),
         dgview->getPatternDataSelectionModel(), SLOT( salomeSelectionChanged() ), Qt::UniqueConnection );
   connect( dgview->getPatternDataSelectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
         this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ), Qt::UniqueConnection );
//   connect( dgview->getPatternBuilderSelectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
//         this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ), Qt::UniqueConnection );
   connect( dgview->getPatternGeomSelectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
            this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ), Qt::UniqueConnection );
   connect( dgview->getGroupsSelectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
         this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ), Qt::UniqueConnection );
   connect( dgview->getMeshSelectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
         this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ), Qt::UniqueConnection );

   showAllMenus();
}

void HEXABLOCKGUI::switchOffGraphicView(VtkDocumentGraphicView* dgview, bool saveCurrentDlg)
{
   if (dgview == NULL) return;

   //disconnect signals
   disconnect( selectionMgr(), SIGNAL( currentSelectionChanged() ),
         dgview->getPatternDataSelectionModel(), SLOT( salomeSelectionChanged() ) );
   disconnect( dgview->getPatternDataSelectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
         this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
//   disconnect( dgview->getPatternBuilderSelectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
//         this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
   disconnect( dgview->getPatternGeomSelectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
            this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
   disconnect( dgview->getGroupsSelectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
         this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
   disconnect( dgview->getMeshSelectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
         this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );

   if (saveCurrentDlg)
       gViewDlgBox[dgview] = currentDialog;

   //close opened dialog
   if (currentDialog != NULL) currentDialog->close();
//   if (_treeViewDelegate != NULL) _treeViewDelegate->closeDialog();

   initialMenus();

}

void HEXABLOCKGUI::switchModel(VtkDocumentGraphicView* dgview)
{
    DEBTRACE("HEXABLOCKGUI::switchModel " << dgview);

    if (dgview == NULL)
    {
        MESSAGE("HEXABLOCKGUI::switchModel : no need to switch!");
        return;
    }

    //switch off the current document graphic view (disconnect signals...)
    if (currentDocGView != NULL)
    {
        //Hide current actor
        hideActor();
        switchOffGraphicView(currentDocGView);
    }

    //clear the dialogs used by the current model so they can be used by the new model
    if (currentDocGView != dgview) clearDialogs();

    _patternDataTreeView->setModel(dgview->getPatternDataModel());
//    _patternBuilderTreeView->setModel(dgview->getPatternBuilderModel());
    _patternGeomTreeView->setModel(dgview->getPatternGeomModel());
    _groupsTreeView->setModel(dgview->getGroupsModel());
    _meshTreeView->setModel(dgview->getMeshModel());

    _patternDataTreeView->setSelectionModel(dgview->getPatternDataSelectionModel());
    _patternDataTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

//    _patternBuilderTreeView->setSelectionModel(dgview->getPatternBuilderSelectionModel());
//    _patternBuilderTreeView->setEditTriggers(QAbstractItemView::AllEditTriggers);

    _patternGeomTreeView->setSelectionModel(dgview->getPatternGeomSelectionModel());
    _patternGeomTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    _groupsTreeView->setSelectionModel(dgview->getGroupsSelectionModel());
    _groupsTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    _meshTreeView->setSelectionModel(dgview->getMeshSelectionModel());
    _meshTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);


    // = * init occ view * =
    if (currentOccGView != NULL && currentOccGView->getViewWindow() == NULL)
    {
        currentOccGView->setViewWindow(graphicViewsHandler->createOccWindow());
        currentOccGView->getViewWindow()->installEventFilter(this);
    }
    // ==


    // = * init vtk view * =
    if (currentDocGView != NULL)
    {
        if (currentDocGView->getViewWindow() != NULL)
            dgview->setViewWindow(currentDocGView->getViewWindow());
        else
        {
            dgview->setViewWindow(graphicViewsHandler->createVtkWindow());
            dgview->getViewWindow()->installEventFilter(this);
        }
    }

    // ==

    switchOnGraphicView(dgview);

    if (currentDocGView != NULL)
    {
        lastVtkDocGView = currentDocGView;
        lastOccPrs = getOccPrs(currentDocGView);
    }
    else
    {
        lastVtkDocGView = dgview;
        lastOccPrs = getOccPrs(dgview);
    }

    currentDocGView = dgview;
    showOnlyActor();
    currentDocGView->getDocumentModel()->refresh();
//    _dwPattern->setWindowTitle(currentDocGView->getDocumentModel()->getName());
    showAllMenus();
    if (!menusEnabled)
        setAllMenusEnabled(true);
}

DocumentModel* HEXABLOCKGUI::getCurrentModel()
{
    if (currentDocGView == NULL) return NULL;
    return currentDocGView->getDocumentModel();
}


HEXABLOCKGUI::ViewType HEXABLOCKGUI::getActiveViewType()
{
    SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
    if (anApp == NULL) return UNKNOWN;
    SUIT_ViewManager* activeVm = anApp->activeViewManager();
    if (activeVm == NULL) return UNKNOWN;

   QString vmType = activeVm->getType();
   if ( (vmType == SVTK_Viewer::Type()) || (vmType == VTKViewer_Viewer::Type()) )
       return VTK;
   else if ( vmType == OCCViewer_Viewer::Type() )
       return OCC;

   return UNKNOWN;
}

HEXABLOCKGUI* HEXABLOCKGUI::getInstance()
{
    SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
    if (anApp == NULL)
        return NULL;

    return  dynamic_cast<HEXABLOCKGUI*>(anApp->activeModule());
}

pair <QString, HEXA_NS::Document*> HEXABLOCKGUI::newHexaDocument()
{
    // Create Document from HEXABLOCK ENGINE
    // WARNING : IN HEXABLOCK component,  GUI and ENGINE share the same process
    // HEXABLOCK_ORB::Document_ptr docIn = _hexaEngine->addDocument("default"); // Perime  Hexa6

    // looking doc impl ( c++ )
    // ^Hexa6 Document_impl* dServant = DownCast<Document_impl*>( docIn );
    // ^Hexa6 ASSERT( dServant );
    // ^Hexa6 if ( dServant)
        // ^Hexa6 doc = dServant->GetImpl();

    HEXA_NS::Document* doc = hexa_root->addDocument ("default");
    QString docEntry = addDocInStudy (doc);

    // looking for docEntry
    /* ****************************************************************
    if ( !CORBA::is_nil(docIn) ){
        CORBA::String_var anIOR = SalomeApp_Application::orb()->object_to_string( docIn );
        QString docIOR = anIOR.in();
        SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
        if ( !docIOR.isEmpty() ) {
            _PTR(SObject) SO( study->studyDS()->FindObjectIOR( docIOR.toLatin1().constData() ) );
            if ( SO )
                docEntry = SO->GetID().c_str();
        }
    }
       **************************************************************** */
    DEBTRACE("HEXABLOCKGUI::newHexaDocument docEntry "<<docEntry.toStdString());

    return make_pair (docEntry, doc);
}


void HEXABLOCKGUI::newDocument()
{
    DEBTRACE("HEXABLOCKGUI::newDocument");

    createSComponent();
    QMainWindow *aParent = application()->desktop();
    QWidget *central = aParent->centralWidget();
    if (central)
        central->setFocus();
    else
        DEBTRACE("No Central Widget");

    //Init OCC
    if (currentOccGView == NULL)
    {
        currentOccGView = new OccGraphicView(graphicViewsHandler->createOccWindow(),
                application()->desktop());
        currentOccGView->getViewWindow()->installEventFilter(this);
    }
    else if (currentOccGView->getViewWindow() == NULL)
    {
        currentOccGView->setViewWindow(graphicViewsHandler->createOccWindow());
        currentOccGView->getViewWindow()->installEventFilter(this);
    }

    //NEW DOCUMENT
    // Create Document from HEXABLOCK ENGINE
    pair <QString, HEXA_NS::Document*> docEntry_Doc ( newHexaDocument() );

    VtkDocumentGraphicView* newGraphicView = NULL;
    //One document at a time
    if (currentDocGView != NULL)
    {
        if (currentDocGView->getViewWindow() == NULL)
        {
            currentDocGView->setViewWindow(graphicViewsHandler->createVtkWindow());
            currentDocGView->getViewWindow()->installEventFilter(this);
        }

          // Create a new document with the current window
          newGraphicView = graphicViewsHandler->createDocumentGraphicView(
                                        new DocumentModel( docEntry_Doc.second,
                                        docEntry_Doc.first, this ),
                                        currentDocGView->getViewWindow(),
                                        application()->desktop() );
    }
    else
    {
          // Create the new Document Graphic View with a new window
          newGraphicView = graphicViewsHandler->createDocumentGraphicView(
                                        new DocumentModel( docEntry_Doc.second,
                                        docEntry_Doc.first, this ),
                                        graphicViewsHandler->createVtkWindow(),
                                        application()->desktop() );
          newGraphicView->getViewWindow()->installEventFilter(this);
    }

    if (newGraphicView == NULL)
        //We couldn't create a new document graphic view
        //we'd better show a dialog box info to inform the user
        return;

    docs[docEntry_Doc.first] = newGraphicView;
    switchModel(newGraphicView); //select the corresponding document in the browser instead

    getApp()->updateObjectBrowser();
    showAllMenus();
}


//Loads the model from the xmlFile in the current graphic view
void HEXABLOCKGUI::loadDocument( const QString &inFile )
{
    DEBTRACE("HEXABLOCKGUI::loadDocument");

    createSComponent();
    QMainWindow *aParent = application()->desktop();
    QString selectedFile;

    if ( inFile.isNull() ){
        QFileDialog dialog( aParent, tr("Open HexaBlock Document") ,
                QString::null, tr( "XML-Files (*.xml);;All Files (*)" ) );
        dialog.setHistory( getQuickDirList() );

        //use the last path as default
        if (!(loadDocLastPath->absolutePath()).isEmpty())
            dialog.setDirectory(*loadDocLastPath);

        if (dialog.exec()){
            QStringList selectedFiles = dialog.selectedFiles();
            if (!selectedFiles.isEmpty())
                selectedFile = selectedFiles.first();

            //remember the selected path
            *loadDocLastPath = dialog.directory();
        }
    } else
        selectedFile = inFile;

    if (selectedFile.isEmpty())
        return;

    //we create a new document if necessary
    if (currentDocGView == NULL)
        //Need a new document
        newDocument();
    else if (currentDocGView->getViewWindow() == NULL) //there's a document without a view
    {
        currentDocGView->setViewWindow(graphicViewsHandler->createVtkWindow());
        currentDocGView->getViewWindow()->installEventFilter(this);
    }

    if (!currentDocGView->isEmpty())
        //we can only have one document for a graphic view
        newDocument();

    //we load the selected file in the current graphic view
    currentDocGView->loadDocument(selectedFile);
    DocumentModel* currentModel = getCurrentModel();
    renameObject( currentModel->documentEntry(), currentModel->getName() );

    showOnlyActor();
}

void HEXABLOCKGUI::saveDocument()
{
    QMainWindow *aParent = application()->desktop();

    QString aDocName = "document", aFilename;
    QStringList filter;
    filter.append( QObject::tr( "XML_FILES_FILTER" ) + " (*.xml)" );
    filter.append( QObject::tr( "ALL_FILES_FILTER" ) + " (*)" );

    //use the last selected directory as default path for file selection
    aFilename =  saveDocLastPath->absolutePath();
    if (!aFilename.isEmpty())
        aFilename += QString("/") + aDocName;

    //show the dialog for file selection
    aFilename = SUIT_FileDlg::getFileName( aParent,
            aFilename,
            filter,
            tr( "Save HexaBlock Document" ),
            false );

    //save the document in the selected file
    if ( !aFilename.isEmpty() ) {
        getCurrentVtkGraphicView()->saveDocument( aFilename );

        //Remember the path for next save
        QFileInfo fileInfo(aFilename);
        *saveDocLastPath = fileInfo.dir();
    }
    //CS_TODO save doc
}


void HEXABLOCKGUI::slot_modelChanged(const QModelIndex &topLeft, const QModelIndex  &bottomRight)
{
    _patternDataTreeView->openPersistentEditor( topLeft );
}

void HEXABLOCKGUI::_showDialogBox( HexaBaseDialog* diag )
{
    if (diag == NULL || _dwInputPanel == NULL ||
            getCurrentVtkGraphicView() == NULL) return;

    //mark this dialog as used by the current model
    currentModelDialogs.insert(diag);

    //close the current dialog box info
//    if (_treeViewDelegate != NULL)
//        _treeViewDelegate->closeDialog();

//    if (_dwInputPanel->widget())
//        _dwInputPanel->widget()->close();

    //Temporary for debugging EdgeAssoc Faked InfoDialog
    if (diag == _edgeAssocDiag)
        diag->debugEdgeAssoc = true;
    else
        diag->debugEdgeAssoc = false;

    if (diag == _vertexAssocDiag || diag ==  _edgeAssocDiag ||
    		diag ==  _quadAssocDiag || diag == _addShapeDiag)
        assocInProgress = true;
    else
        assocInProgress = false;

    //show the dialog box in the dockwidget
    diag->resetSizeAndShow(_dwInputPanel);

    //clear the current selections
    selectionMgr()->clearSelected();

    currentDialog = diag;
    diag->setFocus();

    if (currentDocGView != NULL)
        currentDocGView->getPatternDataSelectionModel()->setInfoMode(false);
}

void HEXABLOCKGUI::showVertexInfoDialog(HEXA_NS::Vertex* vertex)
{
    if (vertex == NULL || _dwInputPanel == NULL)
        return;

    if (_vertexInfoDialog == NULL)
        _vertexInfoDialog = new VertexDialog(_dwInputPanel, HexaBaseDialog::INFO_MODE);

    _vertexInfoDialog->setValue(vertex);
    _vertexInfoDialog->resetSizeAndShow(_dwInputPanel);
    currentDialog = _vertexInfoDialog;
}

void HEXABLOCKGUI::showEdgeInfoDialog(HEXA_NS::Edge* edge)
{
    if (edge == NULL || _dwInputPanel == NULL)
        return;

    if (_edgeInfoDialog == NULL)
        _edgeInfoDialog = new EdgeDialog(_dwInputPanel, HexaBaseDialog::INFO_MODE);

    _edgeInfoDialog->setValue(edge);
    _edgeInfoDialog->resetSizeAndShow(_dwInputPanel);
    currentDialog = _edgeInfoDialog;
}

void HEXABLOCKGUI::showQuadInfoDialog(HEXA_NS::Quad* quad)
{
    if (quad == NULL || _dwInputPanel == NULL)
        return;

    if (_quadInfoDialog == NULL)
        _quadInfoDialog = new QuadDialog(_dwInputPanel, HexaBaseDialog::INFO_MODE);

    _quadInfoDialog->setValue(quad);
    _quadInfoDialog->resetSizeAndShow(_dwInputPanel);
    currentDialog = _quadInfoDialog;
}

void HEXABLOCKGUI::showHexaInfoDialog(HEXA_NS::Hexa* hexa)
{
    if (hexa == NULL || _dwInputPanel == NULL)
        return;

    if (_hexaInfoDialog == NULL)
        _hexaInfoDialog = new HexaDialog(_dwInputPanel, HexaBaseDialog::INFO_MODE);

    _hexaInfoDialog->setValue(hexa);
    _hexaInfoDialog->resetSizeAndShow(_dwInputPanel);
    currentDialog = _hexaInfoDialog;
}

void HEXABLOCKGUI::showVectorInfoDialog(HEXA_NS::Vector* vector)
{
    if (vector == NULL || _dwInputPanel == NULL)
        return;

    if (_vectorInfoDialog == NULL)
        _vectorInfoDialog = new VectorDialog(_dwInputPanel, HexaBaseDialog::INFO_MODE);

    _vectorInfoDialog->setValue(vector);
    _vectorInfoDialog->resetSizeAndShow(_dwInputPanel);
    currentDialog = _vectorInfoDialog;
}

void HEXABLOCKGUI::showGroupInfoDialog(HEXA_NS::Group* group)
{
    if (group == NULL || _dwInputPanel == NULL)
        return;

    if (_groupInfoDialog == NULL)
        _groupInfoDialog = new GroupDialog(_dwInputPanel, HexaBaseDialog::INFO_MODE);

    _groupInfoDialog->setValue(group);
    _groupInfoDialog->resetSizeAndShow(_dwInputPanel);
    currentDialog = _groupInfoDialog;
}

void HEXABLOCKGUI::showLawInfoDialog(HEXA_NS::Law* law)
{
    if (law == NULL || _dwInputPanel == NULL)
        return;

    if (_lawInfoDialog == NULL)
        _lawInfoDialog = new LawDialog(_dwInputPanel, HexaBaseDialog::INFO_MODE);

    _lawInfoDialog->setValue(law);
    _lawInfoDialog->resetSizeAndShow(_dwInputPanel);
    currentDialog = _lawInfoDialog;
}

void HEXABLOCKGUI::showPropagationInfoDialog(HEXA_NS::Propagation* propagation)
{
    if (propagation == NULL || _dwInputPanel == NULL)
        return;

    if (_propagationInfoDialog == NULL)
        _propagationInfoDialog = new PropagationDialog(_dwInputPanel, HexaBaseDialog::INFO_MODE);

    _propagationInfoDialog->setValue(propagation);
    _propagationInfoDialog->resetSizeAndShow(_dwInputPanel);
    currentDialog = _propagationInfoDialog;
}

void HEXABLOCKGUI::addVertex()
{
    if ( !_vertexDiag ){
        _vertexDiag = new VertexDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _vertexDiag );

    //set default name
    HEXA_NS::Document* doc = getCurrentModel() ? getCurrentModel()->getHexaDocument() : NULL;
    if (_vertexDiag != NULL && doc != NULL)
        _vertexDiag->name_le->setText(doc->getNextName(HEXA_NS::EL_VERTEX).c_str());
}

void HEXABLOCKGUI::addEdge()
{
    if ( !_edgeDiag ){
        _edgeDiag = new EdgeDialog( _dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _edgeDiag );

    //set default name
    HEXA_NS::Document* doc = getCurrentModel() ? getCurrentModel()->getHexaDocument() : NULL;
    if (_edgeDiag != NULL && doc != NULL)
        _edgeDiag->name_le->setText(doc->getNextName(HEXA_NS::EL_EDGE).c_str());
}

void HEXABLOCKGUI::addQuad()
{
    if ( !_quadDiag ){
        _quadDiag = new QuadDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);

    }
    _showDialogBox( _quadDiag );

    //set default name
    HEXA_NS::Document* doc = getCurrentModel() ? getCurrentModel()->getHexaDocument() : NULL;
    if (_quadDiag != NULL && doc != NULL)
        _quadDiag->name_le->setText(doc->getNextName(HEXA_NS::EL_QUAD).c_str());
}

void HEXABLOCKGUI::addHexa()
{
    if ( !_hexaDiag ){
        _hexaDiag = new HexaDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _hexaDiag );

    //set default name
    HEXA_NS::Document* doc = getCurrentModel() ? getCurrentModel()->getHexaDocument() : NULL;
    if (_hexaDiag != NULL && doc != NULL)
        _hexaDiag->name_le->setText(doc->getNextName(HEXA_NS::EL_HEXA).c_str());
}

void HEXABLOCKGUI::addVector()
{
    if ( !_vectorDiag ){
        _vectorDiag = new VectorDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _vectorDiag );

    //set default name
    HEXA_NS::Document* doc = getCurrentModel() ? getCurrentModel()->getHexaDocument() : NULL;
    if (_vectorDiag != NULL && doc != NULL)
        _vectorDiag->name_le->setText(doc->getNextName(HEXA_NS::EL_VECTOR).c_str());
}

//void HEXABLOCKGUI::addCylinder()
//{
//    if ( !_cylinderDiag ){
//        _cylinderDiag = new CylinderDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
//    }
//    _showDialogBox( _cylinderDiag );
//
//    //set default name
//    HEXA_NS::Document* doc = getCurrentModel() ? getCurrentModel()->getHexaDocument() : NULL;
//    if (_cylinderDiag != NULL && doc != NULL)
//        _cylinderDiag->name_le->setText(doc->getNextName(HEXA_NS::EL_CYLINDER).c_str());
//}


//void HEXABLOCKGUI::addPipe()
//{
//    if ( !_pipeDiag){
//        _pipeDiag = new PipeDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
//    }
//    _showDialogBox( _pipeDiag );
//
//    //set default name
//    HEXA_NS::Document* doc = getCurrentModel() ? getCurrentModel()->getHexaDocument() : NULL;
//    if (_pipeDiag != NULL && doc != NULL)
//        _pipeDiag->name_le->setText(doc->getNextName(HEXA_NS::EL_CYLINDER).c_str());
//}


void HEXABLOCKGUI::makeGrid()
{
    if ( !_makeGridDiag ){
        _makeGridDiag = new MakeGridDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _makeGridDiag );
}


void HEXABLOCKGUI::makeCylinder()
{
    if ( !_makeCylinderDiag ){
        _makeCylinderDiag = new MakeCylinderDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _makeCylinderDiag );
}

void HEXABLOCKGUI::makePipe()
{
    if ( !_makePipeDiag ){
        _makePipeDiag = new MakePipeDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _makePipeDiag );
}

void HEXABLOCKGUI::makeCylinders()
{
    if ( !_makeCylindersDiag ){
        _makeCylindersDiag = new MakeCylindersDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _makeCylindersDiag );
}

void HEXABLOCKGUI::makePipes()
{
    if ( !_makePipesDiag ){
        _makePipesDiag = new MakePipesDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _makePipesDiag );
}


void HEXABLOCKGUI::makeHemiSphere()  // NEW HEXA3
{
    if ( !_makeHemiSphereDiag ){
        _makeHemiSphereDiag = new MakeHemiSphereDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _makeHemiSphereDiag );
}


void HEXABLOCKGUI::removeHexa()
{
    if ( !_removeHexaDiag ){
        _removeHexaDiag = new RemoveHexaDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _removeHexaDiag );
}


void HEXABLOCKGUI::prismQuad()
{
    if ( !_prismQuadDiag ){
        _prismQuadDiag = new PrismQuadDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _prismQuadDiag );
}


void HEXABLOCKGUI::joinQuad()
{
    if ( !_joinQuadDiag ){
        _joinQuadDiag = new JoinQuadDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _joinQuadDiag );
}

void HEXABLOCKGUI::merge()
{
    if ( !_mergeDiag ){
        _mergeDiag = new MergeDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _mergeDiag );
}

void HEXABLOCKGUI::disconnectElts()
{
    if ( !_disconnectDiag ){
        _disconnectDiag = new DisconnectDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _disconnectDiag );
}

void HEXABLOCKGUI::cutEdge()
{
    if ( !_cutEdgeDiag ){
        _cutEdgeDiag = new CutEdgeDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _cutEdgeDiag );
}

void HEXABLOCKGUI::makeTransformation()
{
    if ( !_makeTransformationDiag ){
        _makeTransformationDiag = new MakeTransformationDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _makeTransformationDiag );
}


void HEXABLOCKGUI::makeSymmetry()
{
    if ( !_makeSymmetryDiag ){
        _makeSymmetryDiag = new MakeSymmetryDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _makeSymmetryDiag );
}


void HEXABLOCKGUI::performTransformation()
{
    if ( !_performTransformationDiag ){
        _performTransformationDiag = new PerformTransformationDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _performTransformationDiag );
}


void HEXABLOCKGUI::performSymmetry()
{
    if ( !_performSymmetryDiag ){
        _performSymmetryDiag = new PerformSymmetryDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _performSymmetryDiag );
}


void HEXABLOCKGUI::replaceHexa()    // NEW HEXA3
{
    if ( !_replaceHexaDiag ){
        _replaceHexaDiag = new ReplaceHexaDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _replaceHexaDiag );
}


void HEXABLOCKGUI::quadRevolution() // NEW HEXA3
{
    if ( !_quadRevolutionDiag ){
        _quadRevolutionDiag = new QuadRevolutionDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _quadRevolutionDiag );
}


void HEXABLOCKGUI::assocVertex()
{
	if (!_dwInputPanel) return;
	QWidget* d = dynamic_cast<SUIT_Desktop*>(_dwInputPanel->parent());
	if ( !_vertexAssocDiag  )
		_vertexAssocDiag  = new VertexAssocDialog( d /*_dwInputPanel*/);
	_showDialogBox( _vertexAssocDiag );
}


void HEXABLOCKGUI::assocEdge()
{
    if ( !_edgeAssocDiag )
        _edgeAssocDiag = new EdgeAssocDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    _showDialogBox( _edgeAssocDiag );
}

void HEXABLOCKGUI::assocQuad()
{
    if ( !_quadAssocDiag )
        _quadAssocDiag = new QuadAssocDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    _showDialogBox( _quadAssocDiag );
}


void HEXABLOCKGUI::addGroup()
{
    if ( !_groupDiag ){
        _groupDiag = new GroupDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _groupDiag );
    if (_groupDiag != NULL && getCurrentModel() != NULL)
    {
        HEXA_NS::Document* doc = getCurrentModel()->getHexaDocument();
        if (doc == NULL) return;
        char defaultName[16];
        sprintf (defaultName, "g%04d", doc->countGroup());
        _groupDiag->name_le->setText(defaultName);
    }
}

void HEXABLOCKGUI::removeGroup()
{
    VtkDocumentGraphicView* currentDGView = getCurrentVtkGraphicView();
    if (currentDGView == NULL) return;

    QItemSelectionModel *groupsSelectionModel = _groupsTreeView->selectionModel();
    QModelIndexList l = groupsSelectionModel->selectedIndexes();
    int nbGroupsRemoved = 0;

    if ( l.isEmpty() ){
        SUIT_MessageBox::information( 0, tr( "CANNOT REMOVE GROUP" ),
                tr( "No group selected!" ) );
        return;
    }

    foreach( QModelIndex selected, l ){
        if ( selected.data(HEXA_TREE_ROLE) == GROUP_TREE ){
            selected = currentDGView->getGroupsModel()->mapToSource( selected );
            Q_ASSERT(selected.isValid());

            //Confirm the deletion of the group
            if (SUIT_MessageBox::question(
                    0,
                    tr("Remove Group"),
                    tr("Remove group : %1 ?").arg(selected.data().toString()),
                    SUIT_MessageBox::Ok | SUIT_MessageBox::Cancel,
                    SUIT_MessageBox::Cancel
            ) == SUIT_MessageBox::Cancel) return;

            bool removed = getCurrentModel()->removeGroup( selected );
            if ( !removed ) {
                SUIT_MessageBox::critical( 0, tr( "ERR_ERROR" ),
                        tr( "CANNOT REMOVE %1" ).arg(selected.data().toString()) );
                return;
            }
            nbGroupsRemoved++;
        }
    }
    if (!nbGroupsRemoved)
        SUIT_MessageBox::information( 0, tr( "CANNOT REMOVE GROUP" ),
                tr( "No group selected!" ) );
}

void HEXABLOCKGUI::addLaw()
{
    if ( !_lawDiag ){
        _lawDiag = new LawDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    }
    _showDialogBox( _lawDiag );
    if (_lawDiag != NULL && getCurrentModel() != NULL)
    {
        HEXA_NS::Document* doc = getCurrentModel()->getHexaDocument();
        if (doc == NULL) return;
        char defaultName[16];
        sprintf (defaultName, "l%04d", doc->countLaw());
        _lawDiag->name_le->setText(defaultName);
    }
}

void HEXABLOCKGUI::removeLaw()
{
    VtkDocumentGraphicView* currentDGView = getCurrentVtkGraphicView();
    if (currentDGView == NULL) return;

    QModelIndexList l = currentDGView->getMeshSelectionModel()->selectedIndexes();
    int nbLawsRemoved = 0;

    if ( l.isEmpty() ){
        SUIT_MessageBox::information( 0, tr( "CANNOT REMOVE LAW" ),
                tr( "No law selected!" ) );
        return;
    }

    foreach( QModelIndex selected, l ){
        if ( selected.data(HEXA_TREE_ROLE) == LAW_TREE ){
            selected = currentDGView->getMeshModel()->mapToSource( selected );
            Q_ASSERT(selected.isValid());

            //Confirm the deletion of the law
            if (SUIT_MessageBox::question(
                    0,
                    tr("Remove Law"),
                    tr("Remove law : %1 ?\nAll propagations having this law will have the default law.").arg(selected.data().toString()),
                    SUIT_MessageBox::Ok | SUIT_MessageBox::Cancel,
                    SUIT_MessageBox::Cancel
            ) == SUIT_MessageBox::Cancel) return;

            bool removed = getCurrentModel()->removeLaw(selected);
            if ( !removed ) {
                SUIT_MessageBox::critical( 0, tr( "ERR_ERROR" ),
                        tr( "CANNOT REMOVE %1" ).arg(selected.data().toString()) );
                return;
            }
            nbLawsRemoved++;
        }
    }
    if (!nbLawsRemoved)
        SUIT_MessageBox::information( 0, tr( "CANNOT REMOVE LAW" ),
                                         tr( "No law selected!" ) );

}


void HEXABLOCKGUI::setPropagation()
{
    if ( !_propagationDiag )
        _propagationDiag = new PropagationDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);

    _showDialogBox( _propagationDiag );
}

// Dialog box to compute a mesh from a document
// --------------------------------------------
void HEXABLOCKGUI::computeMesh()
{
    if ( !_computeMeshDiag )
        _computeMeshDiag = new ComputeMeshDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);

    _showDialogBox( _computeMeshDiag );
}

void HEXABLOCKGUI::clearAssociations()
{
    VtkDocumentGraphicView* currentDGView = getCurrentVtkGraphicView();
    if (currentDGView == NULL) return;

    //   QMessageBox::warning( 0, "windowTitle()", "clearAssociations" );
    QModelIndex iDataModel = _patternDataTreeView->currentIndex();
    QModelIndex iModel     = currentDGView->getPatternDataModel()->mapToSource(iDataModel);

    getCurrentModel()->clearEltAssociations(iModel);

    //  SUIT_MessageBox::information( 0, tr( "HEXA_INFO" ), tr( "ASSOCIATION CLEARED" ) );
}

void HEXABLOCKGUI::showModelInfo()
{
    //create the info dialog if not yet
    if (_modelInfoDiag == NULL)
        _modelInfoDiag = new ModelInfoDialog(_dwInputPanel);

    //show the model informations dialog
    _showDialogBox( _modelInfoDiag );
}

void HEXABLOCKGUI::addShape()
{
    //create the info dialog if not yet
    if (_addShapeDiag == NULL)
        _addShapeDiag = new AddShapeDialog(_dwInputPanel);

    //show the model informations dialog
    _showDialogBox( _addShapeDiag );
}

void HEXABLOCKGUI::clearAllAssociations()
{
    VtkDocumentGraphicView* currentDGView = getCurrentVtkGraphicView();
    if (currentDGView == NULL) return;

    QModelIndex currentIndex = currentDGView->getPatternDataModel()->mapToSource(_patternDataTreeView->currentIndex());
    if (!currentIndex.isValid()) return;
    QVariant currentAssocVariant;
    QString currentAssocEntry;

    //Confirm the deletion of the associations
    QVariant treeVariant = currentIndex.data( HEXA_TREE_ROLE );
    if ( !treeVariant.isValid() ) return;
    int eltType = treeVariant.toInt();
    QString typeStr = "";
    HEXA_NS::EnumElt hexaType;
    if (eltType == VERTEX_DIR_TREE)
    {
        typeStr = tr("TREE_ITEM_VERTEX").toUpper();
        hexaType = HEXA_NS::EL_VERTEX;
    }
    else if (eltType == EDGE_DIR_TREE)
    {
        typeStr = tr("TREE_ITEM_EDGE").toUpper();
        hexaType = HEXA_NS::EL_EDGE;
    }
    else if (eltType == QUAD_DIR_TREE)
    {
        typeStr = tr("TREE_ITEM_QUAD").toUpper();
        hexaType = HEXA_NS::EL_QUAD;
    }

    if (SUIT_MessageBox::question(
            0,
            tr("CLEAR_ASSOCIATIONS"),
            tr("CLEAR_ALL_ASSOCIATIONS_ON") + " " + typeStr + "?",
            SUIT_MessageBox::Ok | SUIT_MessageBox::Cancel,
            SUIT_MessageBox::Cancel
    ) == SUIT_MessageBox::Cancel) return;


    DocumentModel* docModel = getCurrentModel();
    if (docModel == NULL) return;
    docModel->clearAssociation(hexaType);
}

void HEXABLOCKGUI::showAssociations()
{
    VtkDocumentGraphicView* currentDGView = getCurrentVtkGraphicView();
    if (currentDGView == NULL) return;

    QModelIndexList elts;
    int currentChildIndex = 0;
    QVariant currentAssocVariant;
    QString currentAssocEntry;
    QModelIndex currentIndex = _patternDataTreeView->currentIndex();
    QModelIndex currentChild = currentIndex.child(currentChildIndex++, 0);

    while( currentChild.isValid() ) {

        currentAssocVariant = currentChild.data( HEXA_ASSOC_ENTRY_ROLE );
        currentAssocEntry = currentChild.data( HEXA_ASSOC_ENTRY_ROLE ).toString();
        if ( currentAssocVariant.isValid() && !currentAssocEntry.isEmpty() )
            elts << currentChild;

        currentChild = currentChild.sibling(currentChildIndex++, 0);
    }
    currentDocGView->highlight(elts);
    currentOccGView->highlight(elts, false);
}


LightApp_SelectionMgr* HEXABLOCKGUI::selectionMgr()
{
    SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
    if( anApp )
        return dynamic_cast<LightApp_SelectionMgr*>( anApp->selectionMgr() );
    else
        return NULL;
}

bool HEXABLOCKGUI::eventFilter(QObject *obj, QEvent *event)
{
    if ( event->type() == QEvent::Enter )
    {   // *** ENTER EVENT

        if (currentDialog == NULL || !currentDialog->isVisible())
            return QObject::eventFilter(obj, event);

        //  ** The window acquire the focus when the cursor enter

            //OCC - window enter
        OCCViewer_ViewWindow* occWindow = dynamic_cast<OCCViewer_ViewWindow*>(obj);
        if ( occWindow != NULL)
            currentDialog->onWindowActivated(occWindow->getViewManager());
        else
            //VTK - window enter
        {
            SVTK_ViewWindow* vtkWindow = dynamic_cast<SVTK_ViewWindow*>(obj);
            if ( vtkWindow != NULL)
                currentDialog->onWindowActivated(vtkWindow->getViewManager());
        }
    }

    //standard event processing
    return QObject::eventFilter(obj, event);
}

QStringList HEXABLOCKGUI::getQuickDirList()
{
    QStringList dirList;
    SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
    if ( resMgr )
        dirList = resMgr->stringValue( "FileDlg", "QuickDirList" ).split( ';', QString::SkipEmptyParts );

    return dirList;
}


// --- Export the module

extern "C"
{
    HEXABLOCK_EXPORT CAM_Module* createModule()
    {
        return new HEXABLOCKGUI();
    }

    HEXABLOCK_EXPORT char* getModuleVersion()
    {
        return (char*)HEXABLOCK_VERSION_STR;
    }
}



// ******************** TESTS ***************************************************

void HEXABLOCKGUI::testDocument()
{
    //CS_TEST
    // ----------
    //   QStandardItem *parentItem = getCurrentModel()->invisibleRootItem();
    //   QStandardItem *myItem =  new QStandardItem("MyItem");
    //   parentItem->appendRow(myItem);
    // //   ----------
    //   QModelIndex v0 = getCurrentModel()->addVertex(0., 0., 0.);
    //   QModelIndex v1 = getCurrentModel()->addVertex(5., 0., 0.);
    //   QModelIndex v2 = getCurrentModel()->addVertex(5., 5., 0.);
    //   QModelIndex v3 = getCurrentModel()->addVertex(0., 5., 0.);
    //   QModelIndex v4 = getCurrentModel()->addVertex(0., 0., 5.);
    //   QModelIndex v5 = getCurrentModel()->addVertex(5., 0., 5.);
    //   QModelIndex v6 = getCurrentModel()->addVertex(5., 5., 5.);
    //   QModelIndex v7 = getCurrentModel()->addVertex(0., 5., 5.);
    // //
    // //
    //   QModelIndex q0 = getCurrentModel()->addQuadVertices( v0, v1, v2, v3 );
    //   QModelIndex q1 = getCurrentModel()->addQuadVertices( v4, v5, v6, v7 );
    //   QModelIndex q2 = getCurrentModel()->addQuadVertices( v0, v3, v7, v4 );
    //   QModelIndex q3 = getCurrentModel()->addQuadVertices( v1, v2, v6, v5 );
    //   QModelIndex q4 = getCurrentModel()->addQuadVertices( v0, v1, v5, v4 );
    //   QModelIndex q5 = getCurrentModel()->addQuadVertices( v3, v2, v6, v7 );
    //
    //   QModelIndex h0 = getCurrentModel()->addHexaQuad( q0, q1, q2, q3, q4, q5 );
    //   QModelIndex vx = getCurrentModel()->addVector(1., 0., 0.);
    //   QModelIndex vy = getCurrentModel()->addVector(0., 1., 0.);
    //   QModelIndex vz = getCurrentModel()->addVector(0., 0., 1.);
    //



    // //   QModelIndex orig1 = getCurrentModel()->addVertex (0, 0,0);
    // //   QModelIndex orig2 = getCurrentModel()->addVertex (50,0,0);
    // //   QModelIndex vz    = getCurrentModel()->addVector (0,0,1);
    // //   QModelIndex vx    = getCurrentModel()->addVector (1,0,0);
    // //
    // //   int nr  = 4;
    // //   int nri = 3;
    // //   int nre = nr;
    // //   int na = 9;
    // //   int nl = 5;
    // //
    // //   QModelIndex  cyl  = getCurrentModel()->addCylinder   (orig1, vz, nr, nl);
    // //   QModelIndex  pipe = getCurrentModel()->addPipe       (orig2, vz, nri, nre, nl);
    // //
    // //   getCurrentModel()->makeCylinder (cyl,  vx, nr, na, nl);
    // //   getCurrentModel()->makePipe(pipe, vx, nr, na, nl);



    //   newMesh( "toto", 3, "FactoryServer");
    // newMesh
}

void HEXABLOCKGUI::test_make_cart_grid()
{
    QModelIndex orig1 = getCurrentModel()->addVertex( 0, 0, 0);
    QModelIndex orig2 = getCurrentModel()->addVertex( 10, 0, 0);
    QModelIndex orig3 = getCurrentModel()->addVertex( 0, 10, 0);
    QModelIndex orig4 = getCurrentModel()->addVertex( 10, 10, 0);
    QModelIndex orig5 = getCurrentModel()->addVertex( 0, 20, 0);
    QModelIndex orig6 = getCurrentModel()->addVertex( 10, 20, 0);

    QModelIndex vz = getCurrentModel()->addVector(0, 0, 1);
    QModelIndex vx = getCurrentModel()->addVector(1, 0, 0);

    //int dr = 1;
    //int dl = 1;
    //int nr = 2;
    //int nl = 3;

    // QModelIndex c1 = getCurrentModel()->makeCylindrical(orig1, vx, vz, dr, 360, dl, nr, 4, nl, true);
    // QModelIndex c2 = getCurrentModel()->makeCylindrical(orig2, vx, vz, dr, 360, dl, nr, 8, nl, true);
    // QModelIndex c3 = getCurrentModel()->makeCylindrical(orig3, vx, vz, dr, 270, dl, nr, 8, nl, true);
    // QModelIndex c4 = getCurrentModel()->makeCylindrical(orig4, vx, vz, dr, 270, dl, nr, 7, nl, true);
    // QModelIndex c5 = getCurrentModel()->makeCylindrical(orig5, vx, vz, dr, 360, dl, nr, 5, nl, true);
    // QModelIndex c6 = getCurrentModel()->makeCylindrical(orig6, vx, vz, dr, 360, dl, nr, 6, nl, true);

}


void HEXABLOCKGUI::test_make_elmts_transform()
{

//    int size_x = 1;
//    int size_y = 1;
//    int size_z = 2;
//
//    QModelIndex orig = getCurrentModel()->addVertex(0, 0, 0);
//    QModelIndex dirVr = getCurrentModel()->addVector(1, 1, 1);

    //obsolete
//    QModelIndex grid = getCurrentModel()->makeCartesian(orig, dirVr, size_x, size_y, size_z);//, 0, 0, 0);
    // orig.setScalar(2);

    // file_name = os.path.join(os.environ['TMP'], 'transfo0.vtk')
    // getCurrentModel()->saveVtk(file_name)

//    QModelIndex devant = getCurrentModel()->addVector(5, 0, 0);
//    QModelIndex grid2  = getCurrentModel()->makeTranslation(grid, devant);

    // file_name = os.path.join(os.environ['TMP'], 'transfo_translation.vtk')
    // getCurrentModel()->saveVtk(file_name)

//    QModelIndex grid4 = getCurrentModel()->makeRotation(grid2, orig, dirVr, 45);


    // file_name = os.path.join(os.environ['TMP'], 'transfo_rotation.vtk');
    // getCurrentModel()->saveVtk(file_name);


    // print "...test make elements by transforming elements OK"
}
// =============================================================== addDocInSudy
QString HEXABLOCKGUI::addDocInStudy (HEXA_NS::Document* document)
{
    QString docEntry;

    if (document == NULL)
        return docEntry;

    QString docName  = document->getName ();

    SalomeApp_Study* app_study = HEXABLOCKGUI::activeStudy();
    if (app_study == NULL)
        return docEntry;

    _PTR(Study)         study    = app_study->studyDS();
    SALOMEDS::Study_var ds_study = ClientStudyToStudy (study);
    SALOMEDS::StudyBuilder_var aBuilder (ds_study->NewBuilder());
    QString entry = app_study->centry("HEXABLOCK");
    SALOMEDS::SObject_var aFatherSO = ds_study->FindObjectID( qPrintable(entry) );
    if (aFatherSO->_is_nil())
        return docEntry;

    SALOMEDS::SObject_var aSO = aBuilder->NewObject(aFatherSO);
    if (aSO->_is_nil())
        return docEntry;
    aSO->SetAttrString("AttributeName", docName.toStdString().c_str());
    docEntry = aSO->GetID(); //the entry of the published object

    return docEntry;
}

//--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8

void HEXABLOCKGUI::test()
{
    DEBTRACE("HEXABLOCKGUI::test");
    VtkDocumentGraphicView* currentDGView = getCurrentVtkGraphicView();
    if (currentDGView == NULL) return;
    currentDGView->update();

}

void HEXABLOCKGUI::test_association()
{
    DEBTRACE("HEXABLOCKGUI::test_association");
    newDocument();

    QModelIndex v0, v1, v2, v3, e0, e1, q0;

    DocumentModel::GeomObj v0Assoc, v1Assoc, v2Assoc, v3Assoc;
    DocumentModel::GeomObj e0AssocA, e1AssocA, e1AssocB, e1AssocC;
    DocumentModel::GeomObj q0AssocA, q0AssocB;

    DocumentModel::GeomObj v0Assoc_test, v1Assoc_test, v2Assoc_test, v3Assoc_test;
    QList<DocumentModel::GeomObj> e1Assocs_test;
    DocumentModel::GeomObj q0Assoc_test;

    v0 = getCurrentModel()->addVertex(0, 0, 0);
    v1 = getCurrentModel()->addVertex(1, 0, 0);
    v2 = getCurrentModel()->addVertex(1, 1, 0);
    v3 = getCurrentModel()->addVertex(0, 1, 0);
    e0  = getCurrentModel()->addEdgeVertices(v0, v1);
    e1  = getCurrentModel()->addEdgeVertices(v1, v2);
    q0 = getCurrentModel()->addQuadVertices( v0, v1, v2, v3 );


    // >>> face3.GetEntry()'0:1:4'
    // >>> e1.GetEntry()'0:1:8'
    // >>> e2.GetEntry()'0:1:9'
    // >>> e3.GetEntry()'0:1:10'
    // >>> e4.GetEntry()'0:1:11'
    // >>> v1.GetEntry()'0:1:12'
    // >>> v2.GetEntry()'0:1:13'
    // >>> v3.GetEntry()'0:1:14'
    // >>> v4.GetEntry()'0:1:15'


    //   v0Assoc.name  = "geomV0";
    //   v0Assoc.entry = "0:1:1:1:3:6";//"0:1:12";
    //   v0Assoc.brep  = "brepV0";
    //   v1Assoc.name  = "geomV1";
    //   v1Assoc.entry = "0:1:1:1:3:7";//"0:1:13";
    //   v1Assoc.brep  = "brepV1";
    //   v2Assoc.name  = "geomV2";
    //   v2Assoc.entry = "0:1:1:1:3:8";//"0:1:14";
    //   v2Assoc.brep  = "brepV2";
    //   v3Assoc.name  = "geomV3";
    //   v3Assoc.entry = "0:1:1:1:3:9";//"0:1:15";
    //   v3Assoc.brep  = "brepV3";
    //
    //
    //
    //   e0AssocA.name   = "geomE0a";
    //   e0AssocA.entry  = "0:1:1:1:3:5";//"0:1:8";
    //   e0AssocA.brep  = "brepE0a";
    //   e0AssocA.start  = 0.10;
    //   e0AssocA.end    = 0.95;
    //
    //
    //
    //   e1AssocA.name   = "geomE1a";
    //   e1AssocA.entry  = "0:1:1:1:3:2";//"0:1:8";
    //   e1AssocA.brep  = "brepE1a";
    //   e1AssocA.start  = 0.12;
    //   e1AssocA.end    = 0.89;
    //   e1AssocB.name   = "geomE1b";
    //   e1AssocB.entry  = "0:1:1:1:3:3";//"0:1:9";
    //   e1AssocB.brep  = "brepE1b";
    //   e1AssocB.start  = 0.20;
    //   e1AssocB.end    = 0.80;
    //   e1AssocC.name   = "geomE1c";
    //   e1AssocC.entry  = "0:1:1:1:3:4";//"0:1:10";
    //   e1AssocC.brep  = "brepE1c";
    //   e1AssocC.start  = 0.16;
    //   e1AssocC.end    = 0.96;
    //
    //   q0AssocA.name   = "geomQuad";
    //   q0AssocA.entry  = "0:1:1:1:3";
    //   q0AssocA.brep   = "brepq0";
    //
    //   q0AssocB.name   = "geomQuad";
    //   q0AssocB.entry  = "0:1:1:1:4";
    //   q0AssocB.brep   = "brepq0";
    //
    //   getCurrentModel()->addAssociation( v0, v0Assoc );
    //   getCurrentModel()->addAssociation( v1, v1Assoc );
    //   getCurrentModel()->addAssociation( v2, v2Assoc );
    //   getCurrentModel()->addAssociation( v3, v3Assoc );
    //
    //
    //   getCurrentModel()->addAssociation( e0, e0AssocA );
    //
    //   getCurrentModel()->addAssociation( e1, e1AssocA );
    //   getCurrentModel()->addAssociation( e1, e1AssocB );
    //   getCurrentModel()->addAssociation( e1, e1AssocC );
    //
    //   getCurrentModel()->addAssociation( q0, q0AssocA );
    //   getCurrentModel()->addAssociation( q0, q0AssocB );





    //   v0Assoc_test = getCurrentModel()->getAssociations( v0 )[0];
    //   v1Assoc_test = getCurrentModel()->getAssociations( v1 )[0];
    //   v2Assoc_test = getCurrentModel()->getAssociations( v2 )[0];
    //   v3Assoc_test = getCurrentModel()->getAssociations( v3 )[0];
    //   ASSERT( v0Assoc.name == v0Assoc_test.name );
    //   ASSERT( v1Assoc.name == v1Assoc_test.name );
    //   ASSERT( v2Assoc.name == v2Assoc_test.name );
    //   ASSERT( v3Assoc.name == v3Assoc_test.name );
    //   ASSERT( v0Assoc.entry == v0Assoc_test.entry );
    //   ASSERT( v1Assoc.entry == v1Assoc_test.entry );
    //   ASSERT( v2Assoc.entry == v2Assoc_test.entry );
    //   ASSERT( v3Assoc.entry == v3Assoc_test.entry );
    //
    //
    //   e1Assocs_test = getCurrentModel()->getAssociations( e1 );
    //   ASSERT( e1Assocs_test[0].name == e1AssocA.name );
    //   ASSERT( e1Assocs_test[1].name == e1AssocB.name );
    //   ASSERT( e1Assocs_test[2].name == e1AssocC.name );
    //   ASSERT( e1Assocs_test[0].entry == e1AssocA.entry );
    //   ASSERT( e1Assocs_test[1].entry == e1AssocB.entry );
    //   ASSERT( e1Assocs_test[2].entry == e1AssocC.entry );
    //   ASSERT( e1Assocs_test[0].start == e1AssocA.start );
    //   ASSERT( e1Assocs_test[1].start == e1AssocB.start );
    //   ASSERT( e1Assocs_test[2].start == e1AssocC.start );
    //   ASSERT( e1Assocs_test[0].end == e1AssocA.end );
    //   ASSERT( e1Assocs_test[1].end == e1AssocB.end );
    //   ASSERT( e1Assocs_test[2].end == e1AssocC.end );
    //
    //   q0Assoc_test = getCurrentModel()->getAssociations( q0 )[0];
    //   ASSERT( q0Assoc_test.name  == q0Assoc.name );
    //   ASSERT( q0Assoc_test.entry == q0Assoc.entry );


    DEBTRACE("HEXABLOCKGUI::test_association fin");
}
