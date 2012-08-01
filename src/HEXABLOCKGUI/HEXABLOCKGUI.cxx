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

//#define _DEVDEBUG_

// #include <Python.h>

#include <cassert>


// #include "klinkitemselectionmodel.h"


#include <QInputDialog>
#include <QIcon>
#include <QStackedWidget>



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

// #include <QxScene_ViewManager.h>
#include <SVTK_ViewManager.h>
#include <SVTK_ViewModel.h>
#include <SVTK_ViewWindow.h>



#include <SOCC_ViewModel.h>
#include <SOCC_ViewWindow.h>
#include <OCCViewer_ViewWindow.h>



// SALOME KERNEL includes
#include <SALOMEDS_Study.hxx>
#include <SALOMEDSClient_StudyBuilder.hxx>
#include <SALOMEDSClient_SComponent.hxx>
#include <SALOMEDSClient_ClientFactory.hxx>
#include <SALOMEDSClient_IParameters.hxx>



#include <SalomeApp_DataObject.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Module.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_DataModel.h>


#include <SALOME_ListIO.hxx>
#include <SALOME_ListIteratorOfListIO.hxx>


#include <SALOME_LifeCycleCORBA.hxx>

#include <LightApp_VTKSelector.h>

#include <OCCViewer_ViewManager.h>


#include <QtxPopupMgr.h>


#include "Resource.hxx"
// #include "QtGuiContext.hxx"

#include "HEXABLOCKGUI.hxx"
#include "HEXABLOCKGUI_Export.hxx"
#include "HEXABLOCKGUI_Trace.hxx"
#include "HEXABLOCKGUI_Resource.hxx"
// #include "HEXABLOCKGUI_DataModel.hxx"
#include "HEXABLOCKGUI_DocumentGraphicView.hxx"
#include "HEXABLOCKGUI_DocumentModel.hxx"
#include "HEXABLOCKGUI_DocumentSelectionModel.hxx"
#include "HEXABLOCKGUI_DocumentDelegate.hxx"
#include "HEXABLOCKGUI_DocumentPanel.hxx"

#include <HEXABLOCK_version.h>

#include "MyBasicGUI_PointDlg.hxx"

// #include CORBA_CLIENT_HEADER(HEXABLOCKPlugin_Algorithm)

#include "HEXABLOCK.hxx"
#include "HexDocument_impl.hxx"


#define DW_MINIMUM_WIDTH       50
#define DWINPUT_MINIMUM_HEIGHT 50

using namespace std;
using namespace HEXABLOCK::GUI;

int  HEXABLOCKGUI::_oldStudyId = -1;

HEXABLOCK_ORB::HEXABLOCK_Gen_var HEXABLOCKGUI::_hexaEngine  = HEXABLOCK_ORB::HEXABLOCK_Gen::_nil();
// SMESH::SMESH_Gen_var             HEXABLOCKGUI::_smeshEngine = SMESH::SMESH_Gen::_nil();
GEOM::GEOM_Gen_var               HEXABLOCKGUI::_geomEngine  = GEOM::GEOM_Gen::_nil();

// std::map<HEXABLOCK::GUI::DocumentModel*,  SVTK_ViewWindow*> HEXABLOCKGUI::svtkViews;
// std::map<const QAbstractItemModel*,  SVTK_ViewWindow*> HEXABLOCKGUI::svtkViews;
// std::map<const QAbstractItemModel*,  SUIT_ViewWindow*> HEXABLOCKGUI::salomeViews;
// HEXABLOCK::GUI::DocumentModel       *HEXABLOCKGUI::_currentModel = NULL;

SVTK_ViewWindow*      HEXABLOCKGUI::currentVtkView = NULL;
OCCViewer_ViewWindow* HEXABLOCKGUI::currentOccView = NULL;

HEXABLOCKGUI::HEXABLOCKGUI() :
  SalomeApp_Module( "HEXABLOCK" ), // default name
  LightApp_Module( "HEXABLOCK" ),
  _menuId(190),
  _dwPattern(0),
  _dwAssociation(0),
  _dwGroups(0),
  _dwMesh(0),
  _dwObjectBrowser(0),
  _dwInputPanel(0),
  _currentModel(0),
  _patternDataModel(0),
  _patternBuilderModel(0),
  _patternDataTreeView(0),
  _patternBuilderTreeView(0),
  _associationTreeView(0),
  _groupsTreeView(0),
  _meshTreeView(0),
  _currentGraphicView(0),
  _treeViewDelegate(0),
  _patternDataSelectionModel(0),
  _patternBuilderSelectionModel(0),
  _meshSelectionModel(0),
  _groupsSelectionModel(0),
//   _documentCnt(0),
  _isSaved( false ),
  _suitVM(0),//,
//   _selectFromTree( false )
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
  _makeHemiSphereDiag(0)
{
  DEBTRACE("HEXABLOCKGUI::HEXABLOCKGUI");
//   _studyContextMap.clear();
}

HEXABLOCKGUI::~HEXABLOCKGUI()
{
//   if ( getApp() )
//     disconnect( getApp(), SIGNAL(studyClosed()), _genericGui, SLOT  (onCleanOnExit()));
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
HEXABLOCK_ORB::HEXABLOCK_Gen_ptr HEXABLOCKGUI::InitHEXABLOCKGen( SalomeApp_Application* app )
{
  Engines::EngineComponent_var comp = app->lcc()->FindOrLoad_Component( "FactoryServer", "HEXABLOCK" );
  HEXABLOCK_ORB::HEXABLOCK_Gen_ptr clr = HEXABLOCK_ORB::HEXABLOCK_Gen::_narrow(comp);
  ASSERT(!CORBA::is_nil(clr));
  return clr;
}

// // Gets an reference to SMESH's engine CS_TO_DELETE
// SMESH::SMESH_Gen_ptr HEXABLOCKGUI::InitSMESHGen( SalomeApp_Application* app,
//                                                  const std::string& container )
// {
//   Engines::EngineComponent_var comp = app->lcc()->FindOrLoad_Component( container.c_str(), "SMESH" );
//   SMESH::SMESH_Gen_ptr          clr = SMESH::SMESH_Gen::_narrow(comp);
//   ASSERT(!CORBA::is_nil(clr));
//   return clr;
// }
//
// // Gets an reference to GEOM's engine CS_TO_DELETE
GEOM::GEOM_Gen_ptr HEXABLOCKGUI::InitGEOMGen( SalomeApp_Application* app,
                                              const std::string& container )
{
  Engines::EngineComponent_var comp = app->lcc()->FindOrLoad_Component( container.c_str(), "GEOM" );
  GEOM::GEOM_Gen_ptr            clr = GEOM::GEOM_Gen::_narrow(comp);
  ASSERT(!CORBA::is_nil(clr));
  return clr;
}


void HEXABLOCKGUI::initialize( CAM_Application* app )
{
  DEBTRACE("HEXABLOCKGUI::initialize");
  SalomeApp_Module::initialize( app );

  _hexaEngine = InitHEXABLOCKGen( dynamic_cast<SalomeApp_Application*>( app ) );
  _geomEngine = InitGEOMGen( dynamic_cast<SalomeApp_Application*>( app ) );

  QWidget* aParent = application()->desktop();
  DEBTRACE(app << "  " << application() << " " << application()->desktop() << " " << aParent);

  SUIT_ResourceMgr* aResourceMgr = app->resourceMgr();
  setResource(aResourceMgr);

//   if ( app && app->desktop() ){
//       connect( app->desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
//                this, SLOT(onWindowActivated( SUIT_ViewWindow* )) );
//       connect( getApp()->objectBrowser()->treeView(),SIGNAL( clicked(const QModelIndex&) ),
//                this, SLOT( onObjectBrowserClick(const QModelIndex&) ) );
// //       connect( getApp(),   SIGNAL(studyClosed()), _genericGui,SLOT  (onCleanOnExit()));
//   }

// // TEST
//   QGraphicsScene* scene = new QGraphicsScene;
//   scene->addText("Hello, philou!");
//   QGraphicsView* view = new QGraphicsView(scene);
//   view->show();
//   app->desktop()->setCentralWidget(view);
// // TEST

  createAndFillDockWidget();
  createActions();
  createMenus();
  createTools();
  studyActivated();
  // add component to study
  if (createSComponent()) updateObjBrowser();

}

void HEXABLOCKGUI::viewManagers( QStringList& list ) const
{
  DEBTRACE("HEXABLOCKGUI::viewManagers");
  MESSAGE("HEXABLOCKGUI::viewManagers");
//   foreach (const QString &str, list)
//     MESSAGE("HEXABLOCKGUI::viewManagers"<<str.toStdString() );
//   list.append( QxScene_Viewer::Type() );
//   list.append( OCCViewer_Viewer::Type() );
//   list.append( SVTK_Viewer::Type() );
}

bool HEXABLOCKGUI::activateModule( SUIT_Study* theStudy )
{
  DEBTRACE("HEXABLOCKGUI::activateModule");
  bool bOk = SalomeApp_Module::activateModule( theStudy );
  if ( !bOk ) return false;

//   setMenuShown( true );
//   setToolShown( true );
  if ( _currentModel == 0 )
    initialMenus();
  else
    showAllMenus();


//   showDockWidgets(false);
  showDockWidgets(true);

//   // import Python module that manages HEXABLOCK plugins (need to be here because SalomePyQt API uses active module)
//   PyGILState_STATE gstate = PyGILState_Ensure();
//   PyObject* pluginsmanager=PyImport_ImportModule((char*)"salome_pluginsmanager");
//   if(pluginsmanager==NULL)
//     PyErr_Print();
//   else
//     {
//       PyObject* result=PyObject_CallMethod( pluginsmanager, (char*)"initialize", (char*)"isss",1,"hexablock","HEXABLOCK","Plugins");
//       if(result==NULL)
//         PyErr_Print();
//       Py_XDECREF(result);
//     }
//   PyGILState_Release(gstate);
//   // end of HEXABLOCK plugins loading

    connect( getApp()->desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
              this, SLOT(onWindowActivated( SUIT_ViewWindow* )) );
    connect( getApp()->objectBrowser()->treeView(),SIGNAL( clicked(const QModelIndex&) ),
              this, SLOT( onObjectBrowserClick(const QModelIndex&) ) );
//       connect( getApp(),   SIGNAL(studyClosed()), _genericGui,SLOT  (onCleanOnExit()));


  LightApp_SelectionMgr* sm = getApp()->selectionMgr();

  SUIT_ViewManager* vm;
  ViewManagerList OCCViewManagers, VTKViewManagers;

  application()->viewManagers( OCCViewer_Viewer::Type(), OCCViewManagers );
  QListIterator<SUIT_ViewManager*> itOCC( OCCViewManagers );
  while ( itOCC.hasNext() && (vm = itOCC.next()) )
    myOCCSelectors.append( new GEOMGUI_OCCSelector( ((OCCViewer_ViewManager*)vm)->getOCCViewer(), sm ) );

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
  QListIterator<GEOMGUI_OCCSelector*> itOCCSel( myOCCSelectors );
  while ( itOCCSel.hasNext() )
    if ( GEOMGUI_OCCSelector* sr = itOCCSel.next() )
      sr->setEnabled(true);

  // disable VTK selectors
  getApp()->selectionMgr()->setEnabled( false, SVTK_Viewer::Type() );
  QListIterator<LightApp_VTKSelector*> itVTKSel( myVTKSelectors );
  while ( itVTKSel.hasNext() )
    if ( LightApp_VTKSelector* sr = itVTKSel.next() )
      sr->setEnabled(true);

  sm->setSelectedObjects( selected, true );   //NPAL 19674


  _hexaEngine->SetCurrentStudy(SALOMEDS::Study::_nil());
  if ( SalomeApp_Study* s = dynamic_cast<SalomeApp_Study*>( theStudy ))
    if ( _PTR(Study) aStudy = s->studyDS()) {
      //define _CAST(Class, shared_ptr_Obj) dynamic_cast<SALOMEDS_##Class*>(shared_ptr_Obj.get())
      _hexaEngine->SetCurrentStudy( _CAST(Study,aStudy)->GetStudy() );
      updateObjBrowser(); // objects can be removed
    }


  return bOk;
}



bool HEXABLOCKGUI::deactivateModule( SUIT_Study* theStudy )
{
  DEBTRACE("HEXABLOCKGUI::deactivateModule");

  setMenuShown( false );
  setToolShown( false );
  showDockWidgets( false );
//   QtGuiContext *context = QtGuiContext::getQtCurrent();
//   _studyContextMap[theStudy->id()] = context;
//   DEBTRACE("_studyContextMap[theStudy] " << theStudy << " " << context);

  disconnect( getApp()->desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
            this, SLOT(onWindowActivated( SUIT_ViewWindow* )) );
  disconnect( getApp()->objectBrowser()->treeView(),SIGNAL( clicked(const QModelIndex&) ),
            this, SLOT( onObjectBrowserClick(const QModelIndex&) ) );
//       connect( getApp(),   SIGNAL(studyClosed()), _genericGui,SLOT  (onCleanOnExit()));


  if ( HEXABLOCKGUI::currentVtkView ){
    HEXABLOCKGUI::currentVtkView->SetSelectionMode( ActorSelection ); //default selectionMode in VTKView
  }

//  if (HEXABLOCKGUI::currentOccView)
//  {
//	 //getDisplayer()->globalSelection();//????	//defaut selectionMode in OccView
//  }


//   if ( _patternDataSelectionModel ){
//     delete _patternDataSelectionModel;
//     _patternDataSelectionModel = NULL;
//   }
//   if ( _patternBuilderSelectionModel ){
//     delete _patternBuilderSelectionModel;
//     _patternBuilderSelectionModel = NULL;
//   }

  qDeleteAll(myOCCSelectors);
  myOCCSelectors.clear();
  getApp()->selectionMgr()->setEnabled( true, OCCViewer_Viewer::Type() );

  qDeleteAll(myVTKSelectors);
  myVTKSelectors.clear();
  getApp()->selectionMgr()->setEnabled( true, SVTK_Viewer::Type() );

  return SalomeApp_Module::deactivateModule( theStudy );
}


bool HEXABLOCKGUI::renameAllowed( const QString& entry) const
{
//   MESSAGE("HEXABLOCKGUI::renameAllowed");
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  SalomeApp_Study* appStudy = app ? dynamic_cast<SalomeApp_Study*>( app->activeStudy() ) : 0;
  SalomeApp_DataObject* obj = appStudy ? dynamic_cast<SalomeApp_DataObject*>(appStudy->findObjectByEntry(entry)) : 0;

  bool res = app && appStudy && obj && !appStudy->isComponent(entry) && !obj->isReference();
  return (app && appStudy && obj && !appStudy->isComponent(entry) && !obj->isReference());
}


bool HEXABLOCKGUI::renameObject( const QString& entry, const QString& name)
{
  MESSAGE("HEXABLOCKGUI::renameObject");
//   bool appRes = SalomeApp_Module::renameObject(entry,name);
//   MESSAGE("appRes"<<appRes);
//   if( !appRes )
//     return false;
  bool result = false;

  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication());
  SalomeApp_Study* appStudy = app ? dynamic_cast<SalomeApp_Study*>( app->activeStudy() ) : 0;

  if(!appStudy)
    return result;

  _PTR(Study) aStudy = appStudy->studyDS();

  if(!aStudy)
    return result;

  _PTR(SObject) obj ( aStudy->FindObjectID(qPrintable(entry)) );
  _PTR(GenericAttribute) anAttr;
  if ( obj ){
    if ( obj->FindAttribute(anAttr, "AttributeName") ){
      _PTR(AttributeName) aName (anAttr);
//       GEOM::GEOM_Object_var anObj = GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(obj));
//
//       HEXABLOCK_Gen_i::Document_var aDoc = HEXABLOCK_Gen_i::Document::_narrow( theIOR );
//       if (!CORBA::is_nil(aDoc)) {
        aName->SetValue( name.toLatin1().data() ); // rename the SObject
//         aDoc->setName( name.toLatin1().data() );  // Rename the corresponding GEOM_Object
        _currentModel->setName( name/*.toLatin1().data()*/ );
// 	_currentGraphicView->setWindowTitle( _currentModel->getName() );
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
  theMap.insert( SalomeApp_Application::WT_PyConsole,     Qt::BottomDockWidgetArea );
}

// LightApp_Displayer* HEXABLOCKGUI::displayer()
// {
//   DEBTRACE("HEXABLOCKGUI::displayer");
//   return _currentGraphicView;
// }


// QString  HEXABLOCKGUI::engineIOR() const
// {
//   DEBTRACE("HEXABLOCKGUI::engineIOR");
// //   return getApp()->defaultEngineIOR();
// }




QString  HEXABLOCKGUI::engineIOR() const
{
  DEBTRACE("HEXABLOCKGUI::engineIOR");
  CORBA::ORB_var anORB = getApp()->orb();
  CORBA::String_var anIOR = anORB->object_to_string(_hexaEngine);
  return QString( anIOR.in() );
}



void HEXABLOCKGUI::onObjectBrowserClick(const QModelIndex& index)
{
  DEBTRACE("HEXABLOCKGUI::onObjectBrowserClick");
  // we want to switch automatically to the right view windows
  QWidget *viewWindow = NULL;

  //first, find entry of item selected
  QString itemEntry;
  DataObjectList dol = getApp()->objectBrowser()->getSelected();
  if (dol.isEmpty()) return;
  SalomeApp_DataObject* item = dynamic_cast<SalomeApp_DataObject*>(dol[0]);
  if (!item) return;
  itemEntry = item->entry();

//   DEBTRACE("HEXABLOCKGUI::onClick index.data()  => "<<index.data().toString().toStdString());
//   DEBTRACE("HEXABLOCKGUI::onClick index.model() => "<<index.model());
//   DEBTRACE("HEXABLOCKGUI::onClick item->name(). => "<<item->name().toStdString());
//   DEBTRACE("HEXABLOCKGUI::onClick itemEntry =>"<<itemEntry.toStdString());

  if ( !_salomeViewWindows.count( itemEntry ) ) return;
  viewWindow = _salomeViewWindows[ itemEntry ];
  if ( !viewWindow ) return;
//   _selectFromTree = true;
  viewWindow->setFocus();
  _currentGraphicView->update(); //CS_TEST
//   _selectFromTree = false;

//   if (getApp()->activeModule()->moduleName().compare("HEXABLOCK") != 0)
//     getApp()->activateModule("HEXABLOCK");
}





void HEXABLOCKGUI::onWindowActivated( SUIT_ViewWindow* svw)
{
  DEBTRACE("HEXABLOCKGUI::onWindowActivated");
  OCCViewer_ViewWindow* anOccVw = dynamic_cast<OCCViewer_ViewWindow*>(svw);

  if ( anOccVw != NULL )
    currentOccView = dynamic_cast<OCCViewer_ViewWindow*>(svw);
//   OCCViewer_Viewer*     aModel = dynamic_cast<OCCViewer_Viewer*>(svw);

  // we want to switch automatically to the right model
  // only VTK view
  SVTK_ViewWindow* viewWindow = dynamic_cast<SVTK_ViewWindow*>(svw);
  if (!viewWindow) return;
  if (getApp()->activeModule() && getApp()->activeModule()->moduleName().compare("HEXABLOCK") != 0) //CS_TODO?
    getApp()->activateModule("HEXABLOCK");

  switchModel( viewWindow );
}


void HEXABLOCKGUI::onWindowClosed( SUIT_ViewWindow* svw)
{
  DEBTRACE("HEXABLOCKGUI::onWindowClosed");
}




void HEXABLOCKGUI::onViewManagerAdded( SUIT_ViewManager*  vm)
{
  DEBTRACE("HEXABLOCKGUI::onViewManagerAdded");
  if ( vm && vm->getType() == OCCViewer_Viewer::Type() )
  {
//     qDebug( "connect" );
// //     connect( vm, SIGNAL( keyPress  ( SUIT_ViewWindow*, QKeyEvent* ) ),
// //              this, SLOT( OnKeyPress( SUIT_ViewWindow*, QKeyEvent* ) ) );
// //     connect( vm, SIGNAL( mousePress( SUIT_ViewWindow*, QMouseEvent* ) ),
// //              this, SLOT( OnMousePress( SUIT_ViewWindow*, QMouseEvent* ) ) );
// //     connect( vm, SIGNAL( mouseMove ( SUIT_ViewWindow*, QMouseEvent* ) ),
// //              this, SLOT( OnMouseMove( SUIT_ViewWindow*, QMouseEvent* ) ) );
//     LightApp_SelectionMgr* sm = getApp()->selectionMgr();
//     myOCCSelectors.append( new GEOMGUI_OCCSelector( ((OCCViewer_ViewManager*)vm)->getOCCViewer(), sm ) );
//
//     // disable OCC selectors
//     getApp()->selectionMgr()->setEnabled( false, OCCViewer_Viewer::Type() );
//     QListIterator<GEOMGUI_OCCSelector*> itOCCSel( myOCCSelectors );
//     while ( itOCCSel.hasNext() )
//       if ( GEOMGUI_OCCSelector* sr = itOCCSel.next() ){
//         sr->setEnabled(true);
//         std::cout<<"HEXABLOCKGUI::onViewManagerAdded sr"<<sr;
//       }
  }
  DEBTRACE("HEXABLOCKGUI::onViewManagerAdded");
}

void HEXABLOCKGUI::onViewManagerRemoved( SUIT_ViewManager* vm)
{
  DEBTRACE("HEXABLOCKGUI::::onViewManagerRemoved");
}

void HEXABLOCKGUI::onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected  )
{
  QModelIndexList l = selected.indexes();
  if ( l.count() == 0 ) return;
  if ( !l[0].isValid() ) return;

  QTreeView* theTree = NULL;
  if ( sender() == _patternDataSelectionModel ){
    theTree = _patternDataTreeView;
  } else if ( sender() == _patternBuilderSelectionModel  ){
    theTree = _patternBuilderTreeView;
  } else if ( sender() == _groupsSelectionModel ){
    theTree = _groupsTreeView;
  } else if ( sender() == _meshSelectionModel ){
    theTree = _meshTreeView;
  }
  if ( theTree ){
    theTree->scrollTo ( l[0] );
  }
}

// void HEXABLOCKGUI::onTryClose(bool &isClosed, QxScene_ViewWindow* window) //CS_TODO
// {
//   DEBTRACE("HEXABLOCKGUI::onTryClose");
//   isClosed = _genericGui->closeContext(window);
// }

// CAM_DataModel* HEXABLOCKGUI::createDataModel()
// {
//   DEBTRACE("HEXABLOCKGUI::createDataModel");
//   return new HEXABLOCKGUI_DataModel(this);
// //   return NULL;
// }

bool HEXABLOCKGUI::createSComponent() //addComponent
{
  DEBTRACE("HEXABLOCKGUI::createSComponent");
  // --- Find or create "HEXABLOCK" SComponent in the study

  _PTR(Study)            aStudy = (( SalomeApp_Study* )(getApp()->activeStudy()))->studyDS();
  _PTR(StudyBuilder)     aBuilder (aStudy->NewBuilder());
  _PTR(GenericAttribute) anAttr;
  _PTR(AttributeName)    aName;

  _PTR(SComponent) aComponent = aStudy->FindComponent("HEXABLOCK");
  if ( !aComponent ){
    aComponent = aBuilder->NewComponent("HEXABLOCK");
    anAttr = aBuilder->FindOrCreateAttribute(aComponent, "AttributeName");
    aName = _PTR(AttributeName) (anAttr);
    aName->SetValue(getApp()->moduleTitle("HEXABLOCK").toStdString());

    anAttr = aBuilder->FindOrCreateAttribute(aComponent, "AttributePixMap");
    _PTR(AttributePixMap) aPixmap(anAttr);
    aPixmap->SetPixMap("share/salome/resources/hexablock/ModuleHexablock.png");

    aBuilder->DefineComponentInstance(aComponent, engineIOR().toStdString());
    DEBTRACE("HEXABLOCKGUI::createSComponent engineIOR=>"<<engineIOR().toStdString());
//      aBuilder->DefineComponentInstance(aComponent, getApp()->defaultEngineIOR().toStdString());
//      DEBTRACE("HEXABLOCKGUI::createSComponent defaultEngineIOR=>"<<getApp()->defaultEngineIOR().toStdString());

//     SalomeApp_DataModel::synchronize( aComponent, HEXABLOCKGUI::activeStudy() );
    return true;
  }
  return false;
}



// bool HEXABLOCKGUI::isSelectionCompatible()
// {
//   DEBTRACE("HEXABLOCKGUI::isSelectionCompatible");
//
//   bool isCompatible = true;
//   SALOME_ListIO selected;
//   if ( LightApp_SelectionMgr *Sel = selectionMgr() )
//     Sel->selectedObjects( selected );
//
//   SALOME_ListIteratorOfListIO It( selected );
//   for ( ; isCompatible && It.More(); It.Next())
//     isCompatible =
//       ( strcmp("GEOM", It.Value()->getComponentDataType()) == 0 ) ||
//       ( strcmp("HEXABLOCK", It.Value()->getComponentDataType()) == 0 );
//
//   return isCompatible;
// }

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
  if(name=="userCatalog")
    {
//       _genericGui->getCatalogWidget()->addCatalogFromFile(Resource::userCatalog.toStdString());
    }
}

void HEXABLOCKGUI::studyActivated() //CS_TODO
{
  int newStudyId = getApp()->activeStudy()->id();
  DEBTRACE("HEXABLOCKGUI::studyActivated " << _oldStudyId << " " << newStudyId);

  if (_oldStudyId != -1)
  {
//       _studyContextMap[_oldStudyId] = QtGuiContext::getQtCurrent();
//       if (_studyContextMap.count(newStudyId))
//         {
//           DEBTRACE("switch to valid context " << QtGuiContext::getQtCurrent() << " " << _studyContextMap[newStudyId]);
//           QtGuiContext::setQtCurrent(_studyContextMap[newStudyId]);
//         }
//       else
//         {
//           DEBTRACE("no switch to null context");
//         }
  }
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
	  //   _currentModel->allowEdition();
	    QMenu menu( _patternDataTreeView );
	    //Remove association
	    QAction *clearAct = menu.addAction( "Remove association(s)" );
	    connect( clearAct, SIGNAL(triggered()), this, SLOT(clearAssociations()) );
	    menu.exec( _patternDataTreeView->mapToGlobal( aPosition) );
  }
  else {

	  QStandardItem *item = _patternDataModel->itemFromIndex ( currentIndex );

	  //We don't do anything for single items having no association
	  if ( item->type() == VERTEXITEM || item->type() == EDGEITEM ||
			  item->type() == QUADITEM || item->type() == HEXAITEM) return;

	  QMenu menu( _patternDataTreeView );

	  //Show association(s)
	  QAction *showAssocAct = menu.addAction( "Show associations" );
	  connect( showAssocAct, SIGNAL(triggered()), this, SLOT(showAssociations()) );

	  //Clear all associations
	  QAction *clearAllAssocAct = menu.addAction( "Remove all associations" );
	  connect( clearAllAssocAct, SIGNAL(triggered()), this, SLOT(clearAllAssociations()) );

	  menu.exec( _patternDataTreeView->mapToGlobal( aPosition) );
  }
}


void HEXABLOCKGUI::createAndFillDockWidget()
{
  QMainWindow *aParent = application()->desktop();

  // Create dock widget (3 dock)

  //1) *********** user input panel ( contain user's edit dialog box )
  _dwInputPanel = new QDockWidget(aParent);
//   _dwInputPanel->setWindowFlags(Qt::FramelessWindowHint);
//   _dwInputPanel->setWindowFlags(Qt::WindowTitleHint);
  _dwInputPanel->setVisible(false);
  _dwInputPanel->setWindowTitle("Input Panel");
//   _dwInputPanel->setMinimumHeight(DWINPUT_MINIMUM_HEIGHT);
  _dwInputPanel->setMinimumWidth(DW_MINIMUM_WIDTH); // --- force a minimum until display
  _dwInputPanel->raise();

//   _stacked = new QStackedWidget(_dwInputPanel);
//   _dwInputPanel->setWidget(_stacked);

  _treeViewDelegate = new DocumentDelegate(_dwInputPanel);

  //2) ************* document data ( Pattern, Association, Mesh ) in treeview representation
  //      Pattern
  _dwPattern = new QDockWidget(aParent);
  //   _dwPattern->installEventFilter(this);
  _dwPattern->setVisible(false);
  _dwPattern->setWindowTitle("Model");
  _dwPattern->setMinimumWidth(DW_MINIMUM_WIDTH); // --- force a minimum until display

  QFrame*      patternFrame  = new QFrame(_dwPattern);
  patternFrame->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
  QVBoxLayout* patternLayout = new QVBoxLayout(patternFrame);
  patternLayout->setSizeConstraint(QLayout::SetMaximumSize);
  QSplitter *splitter = new QSplitter(Qt::Vertical,patternFrame);
  _patternDataTreeView    = new QTreeView(patternFrame);//_dwPattern);
  _patternBuilderTreeView = new QTreeView(patternFrame);
  splitter->addWidget(_patternDataTreeView);
  splitter->addWidget(_patternBuilderTreeView);
  patternLayout->addWidget(splitter);
//   _patternDataTreeView->setMinimumHeight(DW_MINIMUM_WIDTH);

  _patternDataTreeView->setEditTriggers(QAbstractItemView::AllEditTriggers/*QAbstractItemView::DoubleClicked*/);
  _patternDataTreeView->setSelectionMode(QAbstractItemView::SingleSelection/*QAbstractItemView::MultiSelection*/);//);//QAbstractItemView::DoubleClicked, QAbstractItemView::SelectedClicked)
  _patternDataTreeView->setItemDelegate(_treeViewDelegate);


  _patternBuilderTreeView->setEditTriggers(QAbstractItemView::AllEditTriggers);
  _patternBuilderTreeView->setItemDelegate(_treeViewDelegate);
  _dwPattern->setWidget(patternFrame);
  patternFrame->show();
  //_dwPattern->raise();

  //      Groups
  _dwGroups = new QDockWidget(aParent);
//   _dwGroups->installEventFilter(this);

  _dwGroups->setVisible(false);
  _dwGroups->setWindowTitle("Groups");
  _dwGroups->setMinimumWidth(DW_MINIMUM_WIDTH); // --- force a minimum until display
  _groupsTreeView = new QTreeView(_dwGroups);
//   _associationTreeView->setMinimumHeight(DW_MINIMUM_WIDTH);
  _groupsTreeView->setEditTriggers(QAbstractItemView::AllEditTriggers);
  _groupsTreeView->setItemDelegate(_treeViewDelegate);
  _dwGroups->setWidget(_groupsTreeView);
  _groupsTreeView->show();

  //      Mesh
  _dwMesh = new QDockWidget(aParent);
//   _dwMesh->installEventFilter(this);
  _dwMesh->setVisible(false);
  _dwMesh->setWindowTitle("Mesh");
  _dwMesh->setMinimumWidth(DW_MINIMUM_WIDTH); // --- force a minimum until display
  _meshTreeView = new QTreeView(_dwMesh);
  //   _meshTreeView->setMinimumHeight(DW_MINIMUM_WIDTH);
  _meshTreeView->setEditTriggers(QAbstractItemView::AllEditTriggers);
  _meshTreeView->setSelectionMode(QAbstractItemView::SingleSelection);
  _meshTreeView->setItemDelegate(_treeViewDelegate);
  _dwMesh->setWidget(_meshTreeView);
  _meshTreeView->show();
  //   _dwMesh->raise();



  //3) ************* documents ( salome objectbrowser )
  QDockWidget *_dwObjectBrowser = 0;
  QWidget* wid = getApp()->objectBrowser()->treeView();
  //   QWidget *wid = application()->objectBrowser()->treeView();
  QWidget *w   = wid->parentWidget();
  while ( w && !_dwObjectBrowser ) {
    _dwObjectBrowser = ::qobject_cast<QDockWidget*>( w );
    w = w->parentWidget();
  }
//   _dwObjectBrowser->installEventFilter(this);
//   _dwObjectBrowser->setVisible(false);
  _dwObjectBrowser->setMinimumWidth(DW_MINIMUM_WIDTH); // --- force a minimum until display
  _dwObjectBrowser->setWindowTitle("Study");


  // dock widget position
  //   aParent->addDockWidget(Qt::LeftDockWidgetArea,  _dwPattern);
  //   aParent->addDockWidget(Qt::RightDockWidgetArea, _dwInputPanel);
  //   aParent->addDockWidget( Qt::LeftDockWidgetArea, _dwPattern );
  aParent->addDockWidget( Qt::LeftDockWidgetArea, _dwObjectBrowser );
  aParent->addDockWidget( Qt::LeftDockWidgetArea, _dwInputPanel );

  aParent->tabifyDockWidget( _dwObjectBrowser, _dwPattern );
  aParent->tabifyDockWidget( _dwPattern, /*_dwAssociation );
  aParent->tabifyDockWidget( _dwAssociation, */_dwGroups );
  aParent->tabifyDockWidget( _dwGroups, _dwMesh );


#if QT_VERSION >= 0x040500
  aParent->setTabPosition(Qt::AllDockWidgetAreas, Resource::tabPanelsUp? QTabWidget::North: QTabWidget::South);
#endif


//   connect( _dwPattern, SIGNAL( visibilityChanged(bool) ),     this, SLOT( showPatternMenus(bool) ) );
// //   connect( _dwAssociation, SIGNAL( visibilityChanged(bool) ), this, SLOT( showAssociationMenus(bool) ) );
//   connect( _dwGroups, SIGNAL( visibilityChanged(bool) ),      this, SLOT( showGroupsMenus(bool) ) );
//   connect( _dwMesh, SIGNAL( visibilityChanged(bool) ),        this, SLOT( showMeshMenus(bool) ) );



  // popup menu on data tree view
 _patternDataTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(_patternDataTreeView,
          SIGNAL(customContextMenuRequested(const QPoint &)),
          this,
          SLOT(treeContextMenu(const QPoint &)));
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

//  _testAct = createAction(_menuId++, tr("Test"), resMgr->loadPixmap("HEXABLOCK", tr("ICON_TEST")),
//                          tr("Test"),  tr("New test"),
//                          0, aParent, false, this, SLOT(test_association()));

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

  _addCylinder = createAction(_menuId++,
                              tr("Create a cylinder"),
                              resMgr->loadPixmap("HEXABLOCK", tr("ICON_ADD_CYLINDER")),
                              tr("Add Cylinder"),
                              tr("Create a new cylinder"),
                              0, aParent, false, this,  SLOT(addCylinder()));

  _addPipe = createAction(_menuId++,
                          tr("Create a pipe"),
                          resMgr->loadPixmap("HEXABLOCK", tr("ICON_ADD_PIPE")),
                          tr("Add Pipe"),
                          tr("Create a new pipe"),
                          0, aParent, false, this, SLOT(addPipe()));

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

// //   _makeRind     = createAction( _menuId++, tr("Make rind"), resMgr->loadPixmap( "HEXABLOCK", tr("ICON_MAKE_RIND")),
// //                                             tr("Make rind"),  tr("Make rind"),
// //                                             0, aParent, false, this, SLOT(makeRind()));

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
                            tr("Make quadrangle association"),
                            resMgr->loadPixmap("HEXABLOCK", tr("ICON_QUAD_ASSOCIATION")),
                            tr("Make Quadrangle Association"),
                            tr("Make quadrangle association"),
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


  //   _newAct->setShortcut( Qt::CTRL + Qt::SHIFT + Qt::Key_N ); // --- QKeySequence::New ambiguous in SALOME
  //       QAction* createAction(const int id,
  //                             const QString& toolTip,
  //                             const QIcon& icon,
  //                             const QString& menu,
  //                             const QString& status,
  //                             const int shortCut,
  //                             QObject* parent =0,
  //                             bool checkable = false,
  //                             QObject* receiver =0,
  //                             const char* member =0);
}


void HEXABLOCKGUI::createMenus()
{
  int aMenuId, subMenuId;
  aMenuId = createMenu( tr("MEN_FILE"), -1, -1 );
  createMenu( separator(), aMenuId, -1, 10 );
  aMenuId = createMenu( tr("HEXABLOCK"), aMenuId, -1, 10 );
  createMenu( _newAct, aMenuId );
  createMenu( _importAct, aMenuId );
  createMenu( _saveAct, aMenuId );
//   createMenu( _testAct, aMenuId );

//   aMenuId = createMenu( tr( "HEXABLOCK" ), -1, -1, 30 );
  aMenuId = createMenu( tr( "Document" ), -1, -1, 30 );
  createMenu( _newAct, aMenuId );//, 10
  createMenu( _importAct, aMenuId );
  createMenu( _saveAct, aMenuId );
//   createMenu( _testAct, aMenuId );

  aMenuId = createMenu( tr("Model"), -1, -1, 30 );
  subMenuId = createMenu( tr( "Construction" ), aMenuId, -1 );
  createMenu( _addVertex, subMenuId );
  createMenu( _addEdge,   subMenuId );
  createMenu( _addQuad,   subMenuId );
  createMenu( _addHexa,   subMenuId );
  //createMenu( separator(), aMenuId);

  // Pattern Builder
  createMenu( _addVector, subMenuId );
  createMenu( _addCylinder, subMenuId );
  createMenu( _addPipe, subMenuId );
  //createMenu( separator(), aMenuId);
  createMenu( _makeGrid,  subMenuId ); //Cartesian, Cylindrical, Spherical
  createMenu( _makeCylinder, subMenuId );
  createMenu( _makePipe,     subMenuId );
  createMenu( _makeCylinders,subMenuId );
  createMenu( _makePipes,    subMenuId );
//   createMenu( _makeRind,  aMenuId );
  createMenu( _makeHemiSphere,subMenuId );
  //createMenu( separator(), aMenuId);

  // Pattern Data Edition
  subMenuId = createMenu( tr( "Operation" ), aMenuId, -1 );
  createMenu( _removeHexa, subMenuId );
  createMenu( _prismQuad,  subMenuId );
  createMenu( _joinQuad,   subMenuId );
  createMenu( _merge,      subMenuId );//   createMenu( _mergeVertices,   aMenuId ); //   createMenu( _mergeEdges,   aMenuId );
  createMenu( _disconnect, subMenuId );
  createMenu( _cutEdge,    subMenuId );
  createMenu( _makeTransformation, subMenuId ); //   createMenu( _makeTranslation,   aMenuId );
  createMenu( _makeSymmetry,       subMenuId );
  createMenu( _performTransformation,   subMenuId );
  createMenu( _performSymmetry,         subMenuId );
  createMenu( _replaceHexa,    subMenuId );
  createMenu( _quadRevolution, subMenuId );

  // Association
  aMenuId = createMenu( tr("Association"), -1, -1, 30 );
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
}


void HEXABLOCKGUI::createTools()
{
  int aToolId;

  aToolId = createTool ( tr( "HexaBlock Toolbar" ) );
  createTool( _newAct, aToolId );
  createTool( _importAct, aToolId );
  createTool( _saveAct, aToolId );
//   createTool( _testAct, aToolId );
  //createTool( separator(), aToolId );

  // Pattern Data
  aToolId = createTool ( tr( "Construction" ) );
  createTool( _addVertex, aToolId );
  createTool( _addEdge, aToolId );
  createTool( _addQuad, aToolId );
  createTool( _addHexa, aToolId );
  //createTool( separator(), aToolId );

  // Pattern Builder
  createTool( _addVector, aToolId );
  createTool( _addCylinder, aToolId );
  createTool( _addPipe, aToolId );
  //createTool( separator(), aToolId );

  createTool( _makeGrid,  aToolId );
  createTool( _makeCylinder, aToolId );
  createTool( _makePipe,     aToolId );
  createTool( _makeCylinders,aToolId );
  createTool( _makePipes,    aToolId );
//   createTool( _makeRind,     aToolId);
  createTool( _makeHemiSphere,   aToolId );
  //createTool( separator(), aToolId );

  // Pattern Data Edition
  aToolId = createTool ( tr( "Operation" ) );
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
  //createTool( separator(), aToolId );

  // Association
  aToolId = createTool ( tr( "Association" ) );
  createTool( _assocVertex, aToolId );
  createTool( _assocEdge, aToolId );
  createTool( _assocQuad, aToolId );
  //createTool( separator(), aToolId );

  // Group
  aToolId = createTool ( tr( "Group" ) );
  createTool( _addGroup,    aToolId );
  createTool( _removeGroup, aToolId ); //CS_TODO
  //createTool( separator(), aToolId );

  // Law
  aToolId = createTool ( tr( "Mesh" ) );
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


void HEXABLOCKGUI::showDocumentMenus(bool show)
{
  DEBTRACE("HEXABLOCKGUI::showDocumentMenus " << show);
  show = true; //
  setMenuShown(_newAct, show);
  setToolShown(_newAct, show);
  setMenuShown(_importAct, show);
  setToolShown(_importAct, show);
  setMenuShown(_saveAct, show);
  setToolShown(_saveAct, show);
//   setMenuShown(_testAct, show);
//   setToolShown(_testAct, show);
}

void HEXABLOCKGUI::showPatternMenus(bool show)
{
  DEBTRACE("HEXABLOCKGUI::showPatternMenus " << show);
  DEBTRACE("_currentModel  " << _currentModel  );
  if ( show && !_currentModel ) return;

  setMenuShown(_addVertex, show );//true);
  setToolShown(_addVertex, show);
  setMenuShown(_addEdge,  show );//true);
  setToolShown(_addEdge, show);
  setMenuShown(_addQuad,  show );//true);
  setToolShown(_addQuad, show);
  setMenuShown(_addHexa,  show );//true);
  setToolShown(_addHexa, show);


  setMenuShown( _addVector,  show );//true);
  setToolShown( _addVector, show);
  setMenuShown( _addCylinder,  show );//true);
  setToolShown( _addCylinder, show);
  setMenuShown( _addPipe,  show );//true);
  setToolShown( _addPipe, show);
  setMenuShown( _makeGrid,  show );//true);
  setToolShown( _makeGrid, show);
  setMenuShown( _makeCylinder,  show );//true);
  setToolShown( _makeCylinder, show);
  setMenuShown( _makePipe,  show );//true);
  setToolShown( _makePipe, show);
  setMenuShown( _makeCylinders,  show );//true);
  setToolShown( _makeCylinders, show);
  setMenuShown( _makePipes,  show );//true);
  setToolShown( _makePipes, show);
//   setMenuShown( _makeRind,  show );//true);
//   setToolShown( _makeRind, show);
  setMenuShown( _makeHemiSphere,  show );//true);
  setToolShown( _makeHemiSphere, show);

  // Pattern Data Edition
  setMenuShown( _removeHexa,  show );//true);
  setToolShown( _removeHexa, show);
  setMenuShown( _prismQuad,  show );//true);
  setToolShown( _prismQuad, show);
  setMenuShown( _joinQuad,  show );//true);
  setToolShown( _joinQuad, show);
  setMenuShown( _merge,  show );//true);
  setToolShown( _merge, show);
  setMenuShown( _disconnect,  show );//true);
  setToolShown( _disconnect, show);
  setMenuShown( _cutEdge,  show );//true);
  setToolShown( _cutEdge, show);
  setMenuShown( _makeTransformation,  show );//true);
  setToolShown( _makeTransformation, show);
  setMenuShown( _makeSymmetry,  show );//true);
  setToolShown( _makeSymmetry, show);
  setMenuShown( _performTransformation,  show );//true);
  setToolShown( _performTransformation, show);
  setMenuShown( _performSymmetry,  show );//true);
  setToolShown( _performSymmetry, show);
  setMenuShown( _replaceHexa,  show );//true);
  setToolShown( _replaceHexa, show);
  setMenuShown( _quadRevolution,  show );//true);
  setToolShown( _quadRevolution, show);

}


void HEXABLOCKGUI::showAssociationMenus(bool show)
{
  DEBTRACE("HEXABLOCKGUI::showAssociationMenus" << show);
  if ( show && !_currentModel ) return;

  // Association Edition
  setMenuShown( _assocVertex,  show );
  setToolShown( _assocVertex, show);
  setMenuShown( _assocEdge,  show );
  setToolShown( _assocEdge, show);
  setMenuShown( _assocQuad,  show );
  setToolShown( _assocQuad, show);

}

void HEXABLOCKGUI::showGroupsMenus(bool show)
{
  DEBTRACE("HEXABLOCKGUI::showGroupsMenus" << show);
  if ( show && !_currentModel ) return;
  setMenuShown( _addGroup,  show );//true);
  setToolShown( _addGroup, show);
  setMenuShown( _removeGroup ,  show );//true);
  setToolShown( _removeGroup , show);
}

void HEXABLOCKGUI::showMeshMenus(bool show)
{
  DEBTRACE("HEXABLOCKGUI::showMeshMenus" << show);
  if ( show && !_currentModel ) return;
  setMenuShown( _addLaw,  show );//true);
  setToolShown( _addLaw, show);
  setMenuShown( _removeLaw,  show );//true);
  setToolShown( _removeLaw, show);;
  setMenuShown( _setPropagation,  show );//true);
  setToolShown( _setPropagation, show);
  setMenuShown( _computeMesh, show); //true);
  setToolShown( _computeMesh, show);
}



void HEXABLOCKGUI::switchModel(SUIT_ViewWindow *view)
{
  DEBTRACE("HEXABLOCKGUI::switchModel " << view);

  if ( _documentModels.count(view) == 0 ){
    DEBTRACE("HEXABLOCKGUI::switchModel : no model found, cannot switch");
    initialMenus();
    return;
  }

  if ( _currentModel != _documentModels[view] ){ // need to switch
    if (_dwInputPanel){
      QWidget* w = _dwInputPanel->widget();
      if (w) w->close();
    }

    // models
    _currentModel = _documentModels[view];
    _patternDataModel->setSourceModel(_currentModel);
    _patternBuilderModel->setSourceModel(_currentModel);
//     _associationsModel->setSourceModel(_currentModel);
    _groupsModel->setSourceModel(_currentModel);
    _meshModel->setSourceModel(_currentModel);

    // associate models and views
    _currentGraphicView = _documentView[view];
//     _currentGraphicView->setModel(_patternDataModel);
//     connect( _currentModel, SIGNAL(patternDataChanged() ), _currentGraphicView,  SLOT ( onPatternDatachanged() ) );
    _currentGraphicView->setModel(_currentModel);

    _patternDataTreeView->setModel(_patternDataModel); //_currentModel
    _patternBuilderTreeView->setModel(_patternBuilderModel);//_currentModel
//     _associationTreeView->setModel(_associationsModel);;
    _groupsTreeView->setModel(_groupsModel);
    _meshTreeView->setModel(_meshModel);

    // set selections for each view
    if ( _patternDataSelectionModel )    delete _patternDataSelectionModel;
    if ( _patternBuilderSelectionModel ) delete _patternBuilderSelectionModel;
    if ( _groupsSelectionModel )         delete _groupsSelectionModel;
    if ( _meshSelectionModel )           delete _meshSelectionModel;

    _patternDataSelectionModel    = new PatternDataSelectionModel( _patternDataModel );
    _patternBuilderSelectionModel = new PatternBuilderSelectionModel( _patternBuilderModel, _patternDataSelectionModel );
    _groupsSelectionModel         = new GroupsSelectionModel( _groupsModel );
    _meshSelectionModel           = new MeshSelectionModel( _meshModel );

    _patternDataSelectionModel->setSalomeSelectionMgr( selectionMgr() );
//     _meshSelectionModel->setSalomeSelectionMgr( selectionMgr() );
//     _patternDataSelectionModel->setGeomEngine( _geomEngine );

    _currentGraphicView->setSelectionModel(_patternDataSelectionModel);

    _patternDataTreeView->setSelectionModel(_patternDataSelectionModel);
    _patternDataTreeView->setEditTriggers(/*QAbstractItemView::EditKeyPressed*/QAbstractItemView::AllEditTriggers /*QAbstractItemView::NoEditTriggers*/);

    _patternBuilderTreeView->setSelectionModel(_patternBuilderSelectionModel);
    _patternBuilderTreeView->setEditTriggers(QAbstractItemView::AllEditTriggers);

    _groupsTreeView->setSelectionModel(_groupsSelectionModel);
    _groupsTreeView->setEditTriggers(QAbstractItemView::AllEditTriggers);

    _meshTreeView->setSelectionModel( _meshSelectionModel );
    _meshTreeView->setEditTriggers(QAbstractItemView::AllEditTriggers);


    // delegate for edition
    _treeViewDelegate->setDocumentModel( _currentModel );
    _treeViewDelegate->setPatternDataSelectionModel( _patternDataSelectionModel );
    _treeViewDelegate->setPatternBuilderSelectionModel( _patternBuilderSelectionModel );
    _treeViewDelegate->setGroupsSelectionModel( _groupsSelectionModel /*_groupsTreeView->selectionModel()*/ );
    _treeViewDelegate->setMeshSelectionModel( _meshSelectionModel/*_meshTreeView->selectionModel()*/ );


    connect( _patternDataSelectionModel, SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
                                   this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
    connect( _patternBuilderSelectionModel, SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
                                      this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
    connect( _groupsSelectionModel, SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
                              this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
    connect( _meshSelectionModel, SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
                            this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );


    currentVtkView = dynamic_cast<SVTK_ViewWindow*>( _currentGraphicView->get_SUIT_ViewWindow() );
  }

  showPatternMenus(true);
}




void HEXABLOCKGUI::showDockWidgets(bool isVisible)
{
  DEBTRACE("HEXABLOCKGUI::showDockWidgets " << isVisible);

  if (_dwObjectBrowser) _dwObjectBrowser->setVisible(isVisible);
  if (_dwObjectBrowser) _dwObjectBrowser->toggleViewAction()->setVisible(isVisible);

  if (_dwPattern) _dwPattern->setVisible(isVisible);
  if (_dwPattern) _dwPattern->toggleViewAction()->setVisible(isVisible);

//   if (_dwAssociation) _dwAssociation->setVisible(isVisible);
//   if (_dwAssociation) _dwAssociation->toggleViewAction()->setVisible(isVisible);

  if (_dwMesh) _dwMesh->setVisible(isVisible);
  if (_dwMesh) _dwMesh->toggleViewAction()->setVisible(isVisible);

  if (_dwGroups) _dwGroups->setVisible(isVisible);
  if (_dwGroups) _dwGroups->toggleViewAction()->setVisible(isVisible);

  if (_dwInputPanel) _dwInputPanel->setVisible(isVisible);
  if (_dwInputPanel) _dwInputPanel->toggleViewAction()->setVisible(isVisible);

  QWidget* w = _dwInputPanel->widget();
  if (w) w->show();
//   if ( isVisible ) _dwObjectBrowser->raise();//_dwPattern->raise();
}


// void HEXABLOCKGUI::raiseStacked()
// {
// //   if (_dwStacked) _dwStacked->raise();
// }




DocumentGraphicView* HEXABLOCKGUI::newGraphicView()
{
  MESSAGE("HEXABLOCKGUI::newGraphicView");
  DocumentGraphicView *newGView = 0;
  SUIT_ViewWindow *suitVW = NULL;

  SUIT_ViewManager *suitVM = getApp()->getViewManager(SVTK_Viewer::Type(), true);
  MESSAGE("suitVM->getViewsCount() => " << suitVM->getViewsCount() );
  suitVW = suitVM->getActiveView();
  if ( _documentView.count(suitVW) > 0)
    suitVW = suitVM->createViewWindow();

  if ( suitVW ){
    suitVW->setClosable(false);
    newGView = new DocumentGraphicView(getApp(), suitVW, application()->desktop());
    _documentView[suitVW] = newGView;
  }

  return newGView;
}








void HEXABLOCKGUI::testDocument()
{
  //CS_TEST
  // ----------
//   QStandardItem *parentItem = _currentModel->invisibleRootItem();
//   QStandardItem *myItem =  new QStandardItem("MyItem");
//   parentItem->appendRow(myItem);
// //   ----------
//   QModelIndex v0 = _currentModel->addVertex(0., 0., 0.);
//   QModelIndex v1 = _currentModel->addVertex(5., 0., 0.);
//   QModelIndex v2 = _currentModel->addVertex(5., 5., 0.);
//   QModelIndex v3 = _currentModel->addVertex(0., 5., 0.);
//   QModelIndex v4 = _currentModel->addVertex(0., 0., 5.);
//   QModelIndex v5 = _currentModel->addVertex(5., 0., 5.);
//   QModelIndex v6 = _currentModel->addVertex(5., 5., 5.);
//   QModelIndex v7 = _currentModel->addVertex(0., 5., 5.);
// //
// //
//   QModelIndex q0 = _currentModel->addQuadVertices( v0, v1, v2, v3 );
//   QModelIndex q1 = _currentModel->addQuadVertices( v4, v5, v6, v7 );
//   QModelIndex q2 = _currentModel->addQuadVertices( v0, v3, v7, v4 );
//   QModelIndex q3 = _currentModel->addQuadVertices( v1, v2, v6, v5 );
//   QModelIndex q4 = _currentModel->addQuadVertices( v0, v1, v5, v4 );
//   QModelIndex q5 = _currentModel->addQuadVertices( v3, v2, v6, v7 );
//
//   QModelIndex h0 = _currentModel->addHexaQuad( q0, q1, q2, q3, q4, q5 );
//   QModelIndex vx = _currentModel->addVector(1., 0., 0.);
//   QModelIndex vy = _currentModel->addVector(0., 1., 0.);
//   QModelIndex vz = _currentModel->addVector(0., 0., 1.);
//



// //   QModelIndex orig1 = _currentModel->addVertex (0, 0,0);
// //   QModelIndex orig2 = _currentModel->addVertex (50,0,0);
// //   QModelIndex vz    = _currentModel->addVector (0,0,1);
// //   QModelIndex vx    = _currentModel->addVector (1,0,0);
// //
// //   int nr  = 4;
// //   int nri = 3;
// //   int nre = nr;
// //   int na = 9;
// //   int nl = 5;
// //
// //   QModelIndex  cyl  = _currentModel->addCylinder   (orig1, vz, nr, nl);
// //   QModelIndex  pipe = _currentModel->addPipe       (orig2, vz, nri, nre, nl);
// //
// //   _currentModel->makeCylinder (cyl,  vx, nr, na, nl);
// //   _currentModel->makePipe(pipe, vx, nr, na, nl);



//   newMesh( "toto", 3, "FactoryServer");
// newMesh
}

void HEXABLOCKGUI::test_make_cart_grid()
{
QModelIndex orig1 = _currentModel->addVertex( 0, 0, 0);
QModelIndex orig2 = _currentModel->addVertex( 10, 0, 0);
QModelIndex orig3 = _currentModel->addVertex( 0, 10, 0);
QModelIndex orig4 = _currentModel->addVertex( 10, 10, 0);
QModelIndex orig5 = _currentModel->addVertex( 0, 20, 0);
QModelIndex orig6 = _currentModel->addVertex( 10, 20, 0);

QModelIndex vz = _currentModel->addVector(0, 0, 1);
QModelIndex vx = _currentModel->addVector(1, 0, 0);

//int dr = 1;
//int dl = 1;
//int nr = 2;
//int nl = 3;

// QModelIndex c1 = _currentModel->makeCylindrical(orig1, vx, vz, dr, 360, dl, nr, 4, nl, true);
// QModelIndex c2 = _currentModel->makeCylindrical(orig2, vx, vz, dr, 360, dl, nr, 8, nl, true);
// QModelIndex c3 = _currentModel->makeCylindrical(orig3, vx, vz, dr, 270, dl, nr, 8, nl, true);
// QModelIndex c4 = _currentModel->makeCylindrical(orig4, vx, vz, dr, 270, dl, nr, 7, nl, true);
// QModelIndex c5 = _currentModel->makeCylindrical(orig5, vx, vz, dr, 360, dl, nr, 5, nl, true);
// QModelIndex c6 = _currentModel->makeCylindrical(orig6, vx, vz, dr, 360, dl, nr, 6, nl, true);

}


void HEXABLOCKGUI::test_make_elmts_transform()
{

int size_x = 1;
int size_y = 1;
int size_z = 2;

QModelIndex orig = _currentModel->addVertex(0, 0, 0);
QModelIndex dirVr = _currentModel->addVector(1, 1, 1);

QModelIndex grid = _currentModel->makeCartesian(orig, dirVr, size_x, size_y, size_z);//, 0, 0, 0);
// orig.setScalar(2);

// file_name = os.path.join(os.environ['TMP'], 'transfo0.vtk')
// _currentModel->saveVtk(file_name)

QModelIndex devant = _currentModel->addVector(5, 0, 0);
QModelIndex grid2  = _currentModel->makeTranslation(grid, devant);

// file_name = os.path.join(os.environ['TMP'], 'transfo_translation.vtk')
// _currentModel->saveVtk(file_name)

QModelIndex grid4 = _currentModel->makeRotation(grid2, orig, dirVr, 45);


// file_name = os.path.join(os.environ['TMP'], 'transfo_rotation.vtk');
// _currentModel->saveVtk(file_name);


// print "...test make elements by transforming elements OK"
}


void HEXABLOCKGUI::test()
{
  DEBTRACE("HEXABLOCKGUI::test");
  _currentGraphicView->update();

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

  v0 = _currentModel->addVertex(0, 0, 0);
  v1 = _currentModel->addVertex(1, 0, 0);
  v2 = _currentModel->addVertex(1, 1, 0);
  v3 = _currentModel->addVertex(0, 1, 0);
  e0  = _currentModel->addEdgeVertices(v0, v1);
  e1  = _currentModel->addEdgeVertices(v1, v2);
  q0 = _currentModel->addQuadVertices( v0, v1, v2, v3 );


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
//   _currentModel->addAssociation( v0, v0Assoc );
//   _currentModel->addAssociation( v1, v1Assoc );
//   _currentModel->addAssociation( v2, v2Assoc );
//   _currentModel->addAssociation( v3, v3Assoc );
//
//
//   _currentModel->addAssociation( e0, e0AssocA );
//
//   _currentModel->addAssociation( e1, e1AssocA );
//   _currentModel->addAssociation( e1, e1AssocB );
//   _currentModel->addAssociation( e1, e1AssocC );
//
//   _currentModel->addAssociation( q0, q0AssocA );
//   _currentModel->addAssociation( q0, q0AssocB );





//   v0Assoc_test = _currentModel->getAssociations( v0 )[0];
//   v1Assoc_test = _currentModel->getAssociations( v1 )[0];
//   v2Assoc_test = _currentModel->getAssociations( v2 )[0];
//   v3Assoc_test = _currentModel->getAssociations( v3 )[0];
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
//   e1Assocs_test = _currentModel->getAssociations( e1 );
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
//   q0Assoc_test = _currentModel->getAssociations( q0 )[0];
//   ASSERT( q0Assoc_test.name  == q0Assoc.name );
//   ASSERT( q0Assoc_test.entry == q0Assoc.entry );


  DEBTRACE("HEXABLOCKGUI::test_association fin");
}


void HEXABLOCKGUI::newDocument()
{
  DEBTRACE("HEXABLOCKGUI::newDocument");

  SUIT_ViewWindow *suitVW = NULL;
//   std::stringstream name;
//   name << "newDoc_" << ++_documentCnt;
//   QString fileName = name.str().c_str();
  QMainWindow *aParent = application()->desktop();
  QWidget *central = aParent->centralWidget();
  if (central)
    central->setFocus();
  else
    DEBTRACE("No Central Widget");

//   BasicGUI_PointDlg* aDlg = new BasicGUI_PointDlg( NULL, aParent );
//   aDlg->show();

  // Create Document from HEXABLOCK ENGINE
  // WARNING : IN HEXABLOCK component,  GUI and ENGINE share the same process
  HEXABLOCK_ORB::Document_ptr docIn = _hexaEngine->addDocument("default");

  HEXA_NS::Document* doc = NULL;
  QString            docEntry;

  // looking doc impl ( c++ )
  Document_impl* dServant = DownCast<Document_impl*>( docIn );
  ASSERT( dServant );
  if ( dServant) doc = dServant->GetImpl();

//   doc->reorderFaces(); //CS_TEST

  // looking for docEntry
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

  DEBTRACE("HEXABLOCKGUI::newDocument docEntry "<<docEntry.toStdString());


  // Now create Document Model
  _currentModel     = new DocumentModel( doc, docEntry, this ); //CS_TOCHECK this
//   _currentModel->setHeaderData(0, Qt::Horizontal, tr("HELLOH0"));
//   _currentModel->setHeaderData(1, Qt::Horizontal, tr("HELLOH1"));
//   _currentModel->setHeaderData(0, Qt::Vertical, tr("HELLOV0"));
  _patternDataModel    = new PatternDataModel(this);
  _patternBuilderModel = new PatternBuilderModel(this);
//   _associationsModel   = new AssociationsModel(this);
  _groupsModel     = new GroupsModel(this);
  _meshModel       = new MeshModel(this);

  _patternDataModel->setSourceModel(_currentModel);
  _patternBuilderModel->setSourceModel(_currentModel);
//   _associationsModel->setSourceModel(_currentModel);
  _groupsModel->setSourceModel(_currentModel);
  _meshModel->setSourceModel(_currentModel);

  // --- new Graphic view ( SVTK )
  _currentGraphicView  = newGraphicView();
  suitVW = _currentGraphicView->get_SUIT_ViewWindow();

  // --- setting model
  _currentGraphicView->setModel(_currentModel);
//   _currentGraphicView->setModel(_patternDataModel);
//   connect( _currentModel, SIGNAL(patternDataChanged() ), _currentGraphicView,  SLOT ( onPatternDatachanged() ) );
  _patternDataTreeView->setModel(_patternDataModel);//_currentModel;
//   _patternDataTreeView->setModel(_currentModel);//;
  _patternBuilderTreeView->setModel(_patternBuilderModel);//_currentModel;
//   _associationTreeView->setModel(_associationsModel);
  _groupsTreeView->setModel(_groupsModel);
  _meshTreeView->setModel(_meshModel);


  // --- setting selection model
  if ( _patternDataSelectionModel )    delete _patternDataSelectionModel;
  if ( _patternBuilderSelectionModel ) delete _patternBuilderSelectionModel;
  if ( _groupsSelectionModel )         delete _groupsSelectionModel;
  if ( _meshSelectionModel )           delete _meshSelectionModel;

  _patternDataSelectionModel    = new PatternDataSelectionModel(_patternDataModel);
  _patternBuilderSelectionModel = new PatternBuilderSelectionModel( _patternBuilderModel, _patternDataSelectionModel );
  _groupsSelectionModel         = new GroupsSelectionModel(_groupsModel);
  _meshSelectionModel           = new MeshSelectionModel(_meshModel);

  _patternDataSelectionModel->setSalomeSelectionMgr( selectionMgr() );
//   _meshSelectionModel->setSalomeSelectionMgr( selectionMgr() );

  _currentGraphicView->setSelectionModel(_patternDataSelectionModel);
  _patternDataTreeView->setSelectionModel(_patternDataSelectionModel);
  _patternDataTreeView->setSelectionMode(QAbstractItemView::SingleSelection); //QAbstractItemView::MultiSelection //CS_TEST
  _patternBuilderTreeView->setSelectionModel(_patternBuilderSelectionModel);
  _groupsTreeView->setSelectionModel(_groupsSelectionModel);
  _meshTreeView->setSelectionModel(_meshSelectionModel);
  _meshTreeView->setSelectionMode(QAbstractItemView::SingleSelection);

  _treeViewDelegate->setDocumentModel( _currentModel );
  _treeViewDelegate->setPatternDataSelectionModel( _patternDataSelectionModel );
  _treeViewDelegate->setPatternBuilderSelectionModel( _patternBuilderSelectionModel );
  _treeViewDelegate->setGroupsSelectionModel( _groupsSelectionModel/*_groupsTreeView->selectionModel()*/ );
  _treeViewDelegate->setMeshSelectionModel( _meshSelectionModel/*_meshTreeView->selectionModel()*/ );


  connect( _patternDataSelectionModel, SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
                                 this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
  connect( _patternBuilderSelectionModel, SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
                                    this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
  connect( _groupsSelectionModel, SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
                            this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
  connect( _meshSelectionModel, SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
                          this, SLOT( onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );


  // salome view/object browser/model management
  _documentModels[ suitVW ] = _currentModel;

//   _salomeViews[ _currentModel ] = suitVW;
//   std::map<const QAbstractItemModel*,  SUIT_ViewWindow*> _salomeViews;

  _salomeViewWindows[docEntry] = suitVW;
  currentVtkView = dynamic_cast<SVTK_ViewWindow*>( _currentGraphicView->get_SUIT_ViewWindow() );
  currentVtkView->raise();

//   _currentGraphicView->setWindowTitle( _currentModel->getName() );
//   currentVtkView->setWindowTitle( _currentModel->getName() );
//   showDockWidgets(true);
//   showPatternMenus
//   _dwPattern->setVisible(true);
//   _dwPattern->toggleViewAction()->setVisible(true);
  _dwPattern->raise();
//   testDocument();
//   test_make_cart_grid();
//   test_make_elmts_transform();
  showAllMenus();
  getApp()->updateObjectBrowser();

}

void HEXABLOCKGUI::slot_modelChanged(const QModelIndex &topLeft, const QModelIndex  &bottomRight)
{
  //std::cout << "HHHHHHHHHHHHHHHHHH  Model changed." << std::endl;
  //std::cout << "HHHHHHHHHHHHHHHHHH  slot_modelChanged topLeft -> " << topLeft.data().toString().toStdString()<<std::endl;
  //std::cout << "HHHHHHHHHHHHHHHHHH  slot_modelChanged bottomRight ->" << bottomRight.data().toString().toStdString()<<std::endl;

  _patternDataTreeView->openPersistentEditor( topLeft );

//   // Make the combo boxes always displayed.
//   for ( int i = 0; i < _currentModel->rowCount(); ++i )
//     {
//       QModelIndex ind = _currentModel->index(i);
//     _patternDataTreeView->openPersistentEditor( ind );
//     std::cout << "ind" << ind.data() << std::endl;
//     }
}

void HEXABLOCKGUI::loadDocument( const QString &inFile )
{
  DEBTRACE("HEXABLOCKGUI::loadDocument");
  QMainWindow *aParent = application()->desktop();
  QString selectedFile;

  if ( inFile.isNull() ){
    QFileDialog dialog( aParent, tr("Open HexaBlock Document") ,
                        QString::null, tr( "XML-Files (*.xml);;All Files (*)" ) );
    dialog.setHistory( getQuickDirList() );
    if (dialog.exec()){
      QStringList selectedFiles = dialog.selectedFiles();
      if (!selectedFiles.isEmpty())
        selectedFile = selectedFiles.first();
    }
  } else {
    selectedFile = inFile;
  }

  if (! selectedFile.isEmpty()){
    newDocument();
    _currentModel->load(selectedFile);
    renameObject( _currentModel->documentEntry(), _currentModel->getName() );
  }
}

void HEXABLOCKGUI::saveDocument()
{
  QMainWindow *aParent = application()->desktop();
  QString anInitialPath = "";
  if ( SUIT_FileDlg::getLastVisitedPath().isEmpty() )
    anInitialPath = QDir::currentPath();
//   QString aMeshName = anIO->getName();

  QString aDocName = "document";
  QStringList filter;
  filter.append( QObject::tr( "XML_FILES_FILTER" ) + " (*.xml)" );
  filter.append( QObject::tr( "ALL_FILES_FILTER" ) + " (*)" );
  QString aFilename =  anInitialPath + QString("/") + aDocName;
  aFilename = SUIT_FileDlg::getFileName( aParent,
                                         aFilename,
                                         filter,
                                         tr( "Save HexaBlock Document" ),
                                         false );
  //std::cout<<"HEXABLOCKGUI::saveDocument()"<<std::endl;
  if ( !aFilename.isEmpty() ) {
    //std::cout<<"!aFilename.isEmpty()"<<std::endl;
    _currentModel->save( aFilename );
  }
  //CS_TODO save doc
}

void HEXABLOCKGUI::_showDialogBox( HexaBaseDialog* diag )
{
  MESSAGE("HEXABLOCKGUI::_showDialogBox()");
  if (!diag) return;
  MESSAGE("if (!diag) return;");
  if (!_dwInputPanel) return;
  MESSAGE("if (!_dwInputPanel) return;");

  diag->setDocumentModel(_currentModel);
  diag->setPatternDataSelectionModel(_patternDataSelectionModel);
  diag->setPatternBuilderSelectionModel(_patternBuilderSelectionModel);
  diag->setGroupsSelectionModel(_groupsSelectionModel);
  diag->setMeshSelectionModel(_meshSelectionModel/*_meshTreeView->selectionModel()*/);

  QWidget* w = _dwInputPanel->widget();
  if (w) w->hide();

  if ( !_dwInputPanel->isVisible() ) _dwInputPanel->setVisible(true);
  _dwInputPanel->setWidget(diag);
  _dwInputPanel->setWindowTitle(diag->windowTitle());
  diag->show();
}


void HEXABLOCKGUI::addVertex()
{
  if ( !_vertexDiag ){
    _vertexDiag = new VertexDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
  }
  _showDialogBox( _vertexDiag );
}

void HEXABLOCKGUI::addEdge()
{
  if ( !_edgeDiag ){
    _edgeDiag = new EdgeDialog( _dwInputPanel, HexaBaseDialog::NEW_MODE);
  }
  _showDialogBox( _edgeDiag );
}




void HEXABLOCKGUI::addQuad()
{
  if ( !_quadDiag ){
    _quadDiag = new QuadDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
  }
  _showDialogBox( _quadDiag );
}

void HEXABLOCKGUI::addHexa()
{
  if ( !_hexaDiag ){
    _hexaDiag = new HexaDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
  }
  _showDialogBox( _hexaDiag );
}

void HEXABLOCKGUI::addVector()
{
  if ( !_vectorDiag ){
    _vectorDiag = new VectorDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
  }
  _showDialogBox( _vectorDiag );
}

void HEXABLOCKGUI::addCylinder()
{
  if ( !_cylinderDiag ){
    _cylinderDiag = new CylinderDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
  }
  _showDialogBox( _cylinderDiag );
}


void HEXABLOCKGUI::addPipe()
{
  if ( !_pipeDiag){
    _pipeDiag = new PipeDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
  }
  _showDialogBox( _pipeDiag );
}


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


// void HEXABLOCKGUI::assocVertex()
// {
//   if ( !_vertexAssocDiag ){
//     _vertexAssocDiag = new VertexAssocDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
//   }
//   _showDialogBox( vertexAssocDiag );
// }


void HEXABLOCKGUI::assocVertex()
{
 MESSAGE("HEXABLOCKGUI::assocVertex()");
 QWidget* d = dynamic_cast<SUIT_Desktop*>(_dwInputPanel->parent());
  if ( !_vertexAssocDiag  ){
    _vertexAssocDiag  = new VertexAssocDialog( NULL, d );
  }
  _vertexAssocDiag->setDocumentModel(_currentModel);
  _vertexAssocDiag->setPatternDataSelectionModel(_patternDataSelectionModel);
  _dwInputPanel->setWidget(_vertexAssocDiag);
  _dwInputPanel->setWindowTitle(_vertexAssocDiag->windowTitle());
  //_vertexAssocDiag->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  _vertexAssocDiag->show();
}



void HEXABLOCKGUI::assocEdge()
{
  if ( !_edgeAssocDiag ){
    _edgeAssocDiag = new EdgeAssocDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
    _edgeAssocDiag->setGeomEngine( _geomEngine );
  }
  _showDialogBox( _edgeAssocDiag );
}

void HEXABLOCKGUI::assocQuad()
{
  if ( !_quadAssocDiag ){
    _quadAssocDiag = new QuadAssocDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
  }
  _showDialogBox( _quadAssocDiag );
}


void HEXABLOCKGUI::addGroup()
{
  if ( !_groupDiag ){
    _groupDiag = new GroupDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
  }
  _showDialogBox( _groupDiag );
}


/*
void HEXABLOCKGUI::removeGroup()
{
  QItemSelectionModel *groupsSelectionModel = _groupsTreeView->selectionModel();
  QModelIndex selected = groupsSelectionModel->currentIndex();
  selected = _groupsModel->mapToSource( selected );

/*
  if ( !selected.isValid() ){
    SUIT_MessageBox::information( 0, tr( "CANNOT REMOVE GROUP" ), tr( "No group selected!" ) );
    return;
  }

  //Demande de confirmation de la suppression des groupes
  if (SUIT_MessageBox::question(
            0,
            tr("Remove Group"),
            tr("Remove group : %1 ?").arg(selected.data().toString()),
            SUIT_MessageBox::Ok | SUIT_MessageBox::Cancel,
            SUIT_MessageBox::Cancel
        ) == SUIT_MessageBox::Cancel) return;

  groupsSelectionModel->clearSelection();
 
  std::cout << "//////////////////////////  " << selected.data().toString().toStdString() << std::endl;
  bool removed = _currentModel->removeGroup( selected );
  //std::cout << "  " << selected.data().toString().toStdString() << std::endl;
  if ( !removed ) {
     SUIT_MessageBox::critical( 0, tr( "ERR_ERROR" ), tr( "CANNOT REMOVE %1" ).arg(selected.data().toString()) );
     groupsSelectionModel->clearSelection();
     return;
  }
}*/



void HEXABLOCKGUI::removeGroup()
{
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
      selected = _groupsModel->mapToSource( selected );
      Q_ASSERT(selected.isValid());

      //Confirm the deletion of the group
      if (SUIT_MessageBox::question(
            0,
            tr("Remove Group"),
            tr("Remove group : %1 ?").arg(selected.data().toString()),
            SUIT_MessageBox::Ok | SUIT_MessageBox::Cancel,
            SUIT_MessageBox::Cancel
        ) == SUIT_MessageBox::Cancel) return;

      bool removed = _currentModel->removeGroup( selected );
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
}

void HEXABLOCKGUI::removeLaw()
{

  QModelIndexList l = _meshSelectionModel->selectedIndexes();
  int nbLawsRemoved = 0;

  if ( l.isEmpty() ){
    SUIT_MessageBox::information( 0, tr( "CANNOT REMOVE LAW" ), 
									tr( "No law selected!" ) );
    return;
  }

  foreach( QModelIndex selected, l ){
    if ( selected.data(HEXA_TREE_ROLE) == LAW_TREE ){
      selected = _meshModel->mapToSource( selected );
      Q_ASSERT(selected.isValid());

      //Confirm the deletion of the law
      if (SUIT_MessageBox::question(
            0,
            tr("Remove Law"),
            tr("Remove law : %1 ?\nAll propagations having this law will \
have the default law.").arg(selected.data().toString()),
            SUIT_MessageBox::Ok | SUIT_MessageBox::Cancel,
            SUIT_MessageBox::Cancel
        ) == SUIT_MessageBox::Cancel) return;

      bool removed = _currentModel->removeLaw(selected);
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
  if (!_dwInputPanel) return;
  PropagationDialog* diag = new PropagationDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
  diag->setDocumentModel(_currentModel);
  diag->setMeshSelectionModel(_meshSelectionModel);
  //diag->setValue(p);
  diag->setFocus();
  _dwInputPanel->setWidget(diag);
  _dwInputPanel->setWindowTitle( diag->windowTitle() );
  diag->clear();
}


// Dialog box to compute a mesh from a document
// --------------------------------------------

void HEXABLOCKGUI::computeMesh()
{
  if ( !_computeMeshDiag ){
    _computeMeshDiag = new ComputeMeshDialog(_dwInputPanel, HexaBaseDialog::NEW_MODE);
  }
  _computeMeshDiag->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  _showDialogBox( _computeMeshDiag );
}

void HEXABLOCKGUI::clearAssociations()
{
//   QMessageBox::warning( 0, "windowTitle()", "clearAssociations" );
  QModelIndex iDataModel = _patternDataTreeView->currentIndex();
  QModelIndex iModel     = _patternDataModel->mapToSource(iDataModel);

  _currentModel->clearEltAssociations(iModel);

  SUIT_MessageBox::information( 0, tr( "HEXA_INFO" ), tr( "ASSOCIATION CLEARED" ) );
}

void HEXABLOCKGUI::clearAllAssociations()
{
	//Confirm the deletion of the associations
	if (SUIT_MessageBox::question(
			0,
			tr("Clear Associations"),
			tr("Clear all associations ?"),
			SUIT_MessageBox::Ok | SUIT_MessageBox::Cancel,
			SUIT_MessageBox::Cancel
	) == SUIT_MessageBox::Cancel) return;

	int currentChildIndex = 0;
	QModelIndex currentIndex = _patternDataModel->mapToSource(_patternDataTreeView->currentIndex());
	QVariant currentAssocVariant;
	QString currentAssocEntry;

	QModelIndex currentChild = currentIndex.child(currentChildIndex++, 0);
	while( currentChild.isValid() ) {

		currentAssocVariant = currentChild.data( HEXA_ASSOC_ENTRY_ROLE );
		currentAssocEntry = currentChild.data( HEXA_ASSOC_ENTRY_ROLE ).toString();
		if ( currentAssocVariant.isValid() && !currentAssocEntry.isEmpty() )
			_currentModel->clearEltAssociations(currentChild);

		currentChild = currentChild.sibling(currentChildIndex++, 0);
	}

	//SUIT_MessageBox::information( 0, tr( "HEXA_INFO" ), tr( "ASSOCIATION CLEARED" ) );
}

void HEXABLOCKGUI::showAssociations()
{
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
	_patternDataSelectionModel-> highlightVTKElts( elts );

}


LightApp_SelectionMgr* HEXABLOCKGUI::selectionMgr()
{
  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  if( anApp )
    return dynamic_cast<LightApp_SelectionMgr*>( anApp->selectionMgr() );
  else
    return 0;
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
  HEXABLOCKGUI_EXPORT CAM_Module* createModule()
  {
    return new HEXABLOCKGUI();
  }

  HEXABLOCKGUI_EXPORT char* getModuleVersion()
  {
    return (char*)HEXABLOCK_VERSION_STR;
  }
}










// void  HEXABLOCKGUI::newMesh( const std::string& meshName,
//                                             int dim,
//                              const std::string& container )
// {
//   SalomeApp_Application* app = getApp();
//   int activeStudyId = app->activeStudy()->id();
//
//   if ( CORBA::is_nil(_hexaEngine) ) _hexaEngine  = InitHEXABLOCKGen( app );
//   if ( CORBA::is_nil(_smeshEngine)) _smeshEngine = InitSMESHGen( app, container );
//   if ( CORBA::is_nil(_geomEngine) ) _geomEngine  = InitGEOMGen( app, container );
//
//   std::cout << "_hexaEngine =>" << _hexaEngine << std::endl;
//   std::cout << "_smeshEngine =>" << _smeshEngine << std::endl;
//   std::cout << "_geomEngine =>" << _geomEngine << std::endl;
//
//   HEXA_NS::Document* docImpl = _currentModel->documentImpl();
//   std::cout << "docImpl =>" << docImpl << std::endl;
//   std::cout << "docImpl->getFile() =>" << docImpl->getFile() << std::endl;
//   HEXABLOCK_ORB::Document_var anDocObj=  _hexaEngine->loadDocument( docImpl->getFile() );
//   std::cout << "anDocObj =>" << anDocObj << std::endl;
//
//
//   GEOM::GEOM_I3DPrimOperations_var anGeomOper = _geomEngine->GetI3DPrimOperations( activeStudyId );
//   if ( CORBA::is_nil(anGeomOper) ) return;  //!anGeomOper->_is_nil() ) {
//   std::cout << "anGeomOper =>" << anGeomOper << std::endl;
//   GEOM::GEOM_Object_var            anGeomObj = anGeomOper->MakeBoxDXDYDZ( 5., 5., 5. );
//   std::cout << "anGeomObj =>" << anGeomObj << std::endl;
//
//
//   SALOMEDS::SObject _geomEngine->AddInStudy (in SALOMEDS::Study theStudy,
//                                   in GEOM_Object theObject,
//                                   in string theName,
//
//
//
// // void GEOMBase::PublishSubObject( GEOM::GEOM_Object_ptr object )
// // {
//   SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
//   if ( study && !CORBA::is_nil( object ) ) {
//     _PTR(Study) studyDS = study->studyDS();
//     QString entry = GetEntry( object );
//     GEOM::GEOM_Object_var father = object->GetMainShape();
//     QString fatherEntry = GetEntry( father );
//     if ( entry.isEmpty() && !CORBA::is_nil( father ) && !fatherEntry.isEmpty() ) {
//       QString name = GetName( object );
//       GeometryGUI::GetGeomGen()->AddInStudy( GeometryGUI::ClientStudyToStudy( studyDS ),
// 					     object, name.toLatin1().data(), father.in() );
//     }
//   }
// }
//
//
// //     geompy = smesh.geompy
// //     #fkl: shape  = doc.getShape()
// //     shape  = geompy.MakeBox(0, 0, 0,  1, 1, 1)
// //     geompy.addToStudy(shape, name)
// //
// //     component = salome.lcc.FindOrLoadComponent(container, "SMESH")
// //     component.init_smesh(salome.myStudy, geompy.geom)
// //     mesh = component.Mesh(shape, name)
// //
// //     so = "libHexaBlockEngine.so"
// //
// //     algo = smesh.SMESH._objref_SMESH_Gen.CreateHypothesis(component, "HEXABLOCK_3D", so)
//     SMESH::SMESH_Hypothesis_var algo = _smeshEngine->CreateHypothesis( "HEXABLOCK_3D", "libHexaBlockEngine.so");
// //       raises ( SALOME::SALOME_Exception );
// //     mesh.mesh.AddHypothesis(shape, algo)
// //
// //     hypo = smesh.SMESH._objref_SMESH_Gen.CreateHypothesis(component, "HEXABLOCK_Parameters", so)
//     //HEXABLOCKPlugin::HEXABLOCKPlugin_Hypothesis_var
//      SMESH::SMESH_Hypothesis_var hypo = _smeshEngine->CreateHypothesis( "HEXABLOCK_Parameters", "libHexaBlockEngine.so");
//
//      HEXABLOCKPlugin::HEXABLOCKPlugin_Hypothesis_var hexHypo = HEXABLOCKPlugin::HEXABLOCKPlugin_Hypothesis::_narrow(hypo);
//     ASSERT(!CORBA::is_nil(hexHypo));
//
// //     mesh.mesh.AddHypothesis(shape, hypo)
// //
// //     hexHypo->SetDocument(anDocObj);
// //     hexHypo->SetDimension(dim);
// //
// //     mesh.Compute()
// //
// //     return mesh
//
// }
//
//
//
// void  newMesh( const std::string& meshName, int dim )// const std::string& container )
// {
//
// //     _smeshEngine.init_smesh(salome.myStudy, geompy.geom)
// SMESH::SMESH_var mesh = _smeshEngine->Mesh(shape, name);
// SMESH::SMESH_Hypothesis_var algo = _smeshEngine->CreateHypothesis( "HEXABLOCK_3D", "libHexaBlockEngine.so");
// SMESH::SMESH_Hypothesis_var hypo = _smeshEngine->CreateHypothesis( "HEXABLOCK_Parameters", "libHexaBlockEngine.so");
// HEXABLOCKPlugin::HEXABLOCKPlugin_Hypothesis_var hexHypo = HEXABLOCKPlugin::HEXABLOCKPlugin_Hypothesis::_narrow(hypo);
// hexHypo->SetDocument(anDocObj);
// hexHypo->SetDimension(dim);
// }

/*
SMESH::SMESH_Gen_var SMESHGUI::GetSMESHGen()
{
  _PTR(Study) aStudy = SMESH::GetActiveStudyDocument(); //Document OCAF de l'etude active
  if ( CORBA::is_nil( myComponentSMESH ) )
    {
      SMESHGUI aGUI; //SRN BugID: IPAL9186: Create an instance of SMESHGUI to initialize myComponentSMESH
      if ( aStudy )
        aGUI.myComponentSMESH->SetCurrentStudy(_CAST(Study,aStudy)->GetStudy());
      return aGUI.myComponentSMESH;
    }
  if ( aStudy )
    myComponentSMESH->SetCurrentStudy(_CAST(Study,aStudy)->GetStudy());
  return myComponentSMESH;
}*/

// bool HEXABLOCKGUI::eventFilter(QObject *obj, QEvent *event)
// {
//     if ( event->type() == QEvent::Enter ){//QEvent::Show ){ //QEvent::KeyPress) {
//         showDocumentMenus( false );
//         showPatternMenus( false );
//         showAssociationMenus( false );
//         showGroupsMenus( false );
//         showMeshMenus( false );
//         if ( obj == _dwObjectBrowser ) {
//           showDocumentMenus( true );
//         } else if ( obj == _dwPattern  ) {
//           showPatternMenus( true );
//         } else if ( obj == _dwAssociation ) {
//           showAssociationMenus( true );
//         } else if ( obj == _dwGroups ) {
//           showGroupsMenus( true );
//         } else if ( obj == _dwMesh ) {
//
//           showMeshMenus( true );
//         }
//         return false;
//     } else {
//          // standard event processing
//          return QObject::eventFilter(obj, event);
//     }
// }

// try {
// //   throw SALOME_Exception(LOCALIZED("assocVertex"));
//   } catch ( SALOME::SALOME_Exception& exc ){
//       INFOS("Following exception was cought:\n\t"<<exc.details.text);
//   } catch( const std::exception& exc){
//       INFOS("Following exception was cought:\n\t"<<exc.what());
//   } catch (Standard_Failure& exc) {
//       MESSAGE("OCCT Exception in SMESH_Pattern: " << exc.GetMessageString());
//   } catch(...){
//       MESSAGE("Unknown exception was cought !!!");
//   }
