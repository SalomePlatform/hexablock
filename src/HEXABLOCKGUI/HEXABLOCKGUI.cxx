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

#define _DEVDEBUG_

// #include <Python.h>

#include <cassert>


#include <QInputDialog>
#include <QIcon>


#include <QtxTreeView.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_DataObject.h>
#include <SUIT_DataBrowser.h>


// #include <QxScene_ViewManager.h>
#include <SVTK_ViewManager.h>
#include <SVTK_ViewModel.h>
#include <SVTK_ViewWindow.h>


#include <SalomeApp_DataObject.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Module.h>
#include <SalomeApp_Application.h>

#include <SALOME_ListIO.hxx>
#include <SALOME_ListIteratorOfListIO.hxx>


#include <SALOME_LifeCycleCORBA.hxx>

#include "Resource.hxx"
// #include "QtGuiContext.hxx"

#include "HEXABLOCKGUI.h"
#include "HEXABLOCKGUI_Export.hxx"
#include "HEXABLOCKGUI_Trace.hxx"
#include "HEXABLOCKGUI_Resource.hxx"
#include "HEXABLOCKGUI_Model.hxx"
#include "HEXABLOCKGUI_DocumentGraphicView.hxx"
#include "HEXABLOCKGUI_DocumentModel.hxx"
#include "HEXABLOCKGUI_DocumentSelectionModel.hxx"
#include "HEXABLOCKGUI_DocumentDelegate.hxx"
#include "HEXABLOCKGUI_DocumentPanel.hxx"


# define DW_MINIMUM_WIDTH 250

using namespace std;
using namespace HEXABLOCK::GUI;

int  HEXABLOCKGUI::_oldStudyId = -1;



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
  _currentSelectionModel(0),
  _documentCnt(0),
  _isSaved( false ),
  _suitVM(0),
  _selectFromTree( false )
{
  DEBTRACE("HEXABLOCKGUI::HEXABLOCKGUI");
//   _studyContextMap.clear();
}

HEXABLOCKGUI::~HEXABLOCKGUI()
{
//   if ( getApp() )
//     disconnect( getApp(), SIGNAL(studyClosed()), _genericGui, SLOT  (onCleanOnExit()));
}


// Gets an reference to the module's engine
HEXABLOCK_ORB::HEXABLOCK_Gen_ptr HEXABLOCKGUI::InitHEXABLOCKGen( SalomeApp_Application* app )
{
  Engines::Component_var comp = app->lcc()->FindOrLoad_Component( "FactoryServer","HEXABLOCK" );
  HEXABLOCK_ORB::HEXABLOCK_Gen_ptr clr = HEXABLOCK_ORB::HEXABLOCK_Gen::_narrow(comp);
  ASSERT(!CORBA::is_nil(clr));
  return clr;
}

void HEXABLOCKGUI::initialize( CAM_Application* app )
{
  DEBTRACE("HEXABLOCKGUI::initialize");
  SalomeApp_Module::initialize( app );

  InitHEXABLOCKGen( dynamic_cast<SalomeApp_Application*>( app ) );

  QWidget* aParent = application()->desktop();
  DEBTRACE(app << "  " << application() << " " << application()->desktop() << " " << aParent);

  SUIT_ResourceMgr* aResourceMgr = app->resourceMgr();
  setResource(aResourceMgr);


//   LightApp_SelectionMgr* mgr = HEXABLOCKGUI::selectionMgr();
//   connect( mgr, SIGNAL( currentSelectionChanged() ),
//                this, SLOT( printVTK() ) );

  if ( app && app->desktop() ){
      connect( app->desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
               this, SLOT(onWindowActivated( SUIT_ViewWindow* )) );

      connect( getApp()->objectBrowser()->treeView(),SIGNAL( doubleClicked(const QModelIndex&) ),
               this, SLOT( onDblClick(const QModelIndex&) ) );
//       connect( getApp(),   SIGNAL(studyClosed()),
//                _genericGui,SLOT  (onCleanOnExit()));
  }

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

  if (createSComponent()) updateObjBrowser();
}

void HEXABLOCKGUI::viewManagers( QStringList& list ) const
{
  DEBTRACE("HEXABLOCKGUI::viewManagers");
//   list.append( QxScene_Viewer::Type() );
  list.append( SVTK_Viewer::Type() );
}

bool HEXABLOCKGUI::activateModule( SUIT_Study* theStudy )
{
  DEBTRACE("HEXABLOCKGUI::activateModule");
  bool bOk = SalomeApp_Module::activateModule( theStudy );

  setMenuShown( true );
  setToolShown( true );

  showDockWidgets(false);
//   showDockWidgets(true);

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
  return SalomeApp_Module::deactivateModule( theStudy );
}

// --- Default windows

void HEXABLOCKGUI::windows( QMap<int, int>& theMap ) const
{
  DEBTRACE("HEXABLOCKGUI::windows");
  theMap.clear();
  theMap.insert( SalomeApp_Application::WT_ObjectBrowser, Qt::LeftDockWidgetArea );
//   theMap.insert( SalomeApp_Application::WT_PyConsole,     Qt::BottomDockWidgetArea );
}

QString  HEXABLOCKGUI::engineIOR() const
{
  DEBTRACE("HEXABLOCKGUI::engineIOR");
  return getApp()->defaultEngineIOR();
}

void HEXABLOCKGUI::onDblClick(const QModelIndex& index) //CS_TODO
{
  DEBTRACE("HEXABLOCKGUI::onDblClick");
//   DataObjectList dol =getApp()->objectBrowser()->getSelected();
//   if (dol.isEmpty()) return;
// 
//   SalomeApp_DataObject* item = dynamic_cast<SalomeApp_DataObject*>(dol[0]);
//   if (!item) return;
// 
//   DEBTRACE(item->name().toStdString());
//   SalomeWrap_DataModel *model = dynamic_cast<SalomeWrap_DataModel*>(dataModel());
//   if (!model) return;
//   DEBTRACE(item->entry().toStdString());
//   QWidget * viewWindow = model->getViewWindow(item->entry().toStdString());
//   if (!viewWindow) return;
//   DEBTRACE("--- " << viewWindow << " "  << item->entry().toStdString());
//   if (getApp()->activeModule()->moduleName().compare("HEXABLOCK") != 0)
//     getApp()->activateModule("HEXABLOCK");
// 
//   _selectFromTree = true;
//   viewWindow->setFocus();
//   _selectFromTree = false;
}

void HEXABLOCKGUI::onWindowActivated( SUIT_ViewWindow* svw)
{
  DEBTRACE("HEXABLOCKGUI::onWindowActivated");
  SVTK_ViewWindow* viewWindow = dynamic_cast<SVTK_ViewWindow*>(svw);
  if (!viewWindow) return;
  if (getApp()->activeModule() && getApp()->activeModule()->moduleName().compare("HEXABLOCK") != 0)
    getApp()->activateModule("HEXABLOCK");

  switchModel( viewWindow );
}
// {
//   std::cout << "CS_BP onWindowActivated onWindowActivated onWindowActivated onWindowActivated "<<std::endl;

//   SVTK_ViewWindow* viewWindow = dynamic_cast<SVTK_ViewWindow*>(svw);
//   if (!viewWindow) return;
//   DEBTRACE("viewWindow " << viewWindow);
//   DEBTRACE("activeModule()->moduleName() " << (getApp()->activeModule() ? getApp()->activeModule()->moduleName().toStdString() : "") );
//   if (getApp()->activeModule() && getApp()->activeModule()->moduleName().compare("HEXABLOCK") != 0)
//     getApp()->activateModule("HEXABLOCK");


//   disconnect(viewWindow, SIGNAL( tryClose( bool&, QxScene_ViewWindow* ) ),
//              this, SLOT(onTryClose( bool&, QxScene_ViewWindow* )) );
//   disconnect(viewWindow->getViewManager(), SIGNAL( deleteView( SUIT_ViewWindow* ) ),
//              this, SLOT(onWindowClosed( SUIT_ViewWindow* )) );
//   connect(viewWindow, SIGNAL( tryClose( bool&, QxScene_ViewWindow* ) ),
//           this, SLOT(onTryClose( bool&, QxScene_ViewWindow* )) );
//   connect(viewWindow->getViewManager(), SIGNAL( deleteView( SUIT_ViewWindow* ) ),
//           this, SLOT(onWindowClosed( SUIT_ViewWindow* )) );

//   switchModel( viewWindow ); CS_TODO dans la vue

//   _studyContextMap[getApp()->activeStudy()->id()] = QtGuiContext::getQtCurrent();
  
//   if (_selectFromTree) return;
//   SalomeWrap_DataModel *model = dynamic_cast<SalomeWrap_DataModel*>(dataModel());
//   if (!model) return;
//   model->setSelected(svw);
// }

void HEXABLOCKGUI::onWindowClosed( SUIT_ViewWindow* svw) 
{
  DEBTRACE("HEXABLOCKGUI::onWindowClosed");
}

// void HEXABLOCKGUI::onTryClose(bool &isClosed, QxScene_ViewWindow* window) //CS_TODO
// {
//   DEBTRACE("HEXABLOCKGUI::onTryClose");
//   isClosed = _genericGui->closeContext(window);
// }

CAM_DataModel* HEXABLOCKGUI::createDataModel()
{
  return new HEXABLOCKGUI_Model(this);
}

bool HEXABLOCKGUI::createSComponent()
{
  DEBTRACE("HEXABLOCKGUI::createSComponent");
  _PTR(Study)            aStudy = (( SalomeApp_Study* )(getApp()->activeStudy()))->studyDS();
  _PTR(StudyBuilder)     aBuilder (aStudy->NewBuilder());
  _PTR(GenericAttribute) anAttr;
  _PTR(AttributeName)    aName;

  // --- Find or create "HEXABLOCK" SComponent in the study

  _PTR(SComponent) aComponent = aStudy->FindComponent("HEXABLOCK");
  if ( !aComponent )
    {
      aComponent = aBuilder->NewComponent("HEXABLOCK");
      anAttr = aBuilder->FindOrCreateAttribute(aComponent, "AttributeName");
      aName = _PTR(AttributeName) (anAttr);
      aName->SetValue(getApp()->moduleTitle("HEXABLOCK").toStdString());
      
      anAttr = aBuilder->FindOrCreateAttribute(aComponent, "AttributePixMap");
      _PTR(AttributePixMap) aPixmap(anAttr);
      aPixmap->SetPixMap("share/salome/resources/hexablock/ModuleHexablock.png");
      
      aBuilder->DefineComponentInstance(aComponent, getApp()->defaultEngineIOR().toStdString());

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




void HEXABLOCKGUI::createAndFillDockWidget() 
{
  QMainWindow *aParent = application()->desktop();

  // Create dock widget (3)
  //1) *********** user input panel ( contain user's edit dialog box )
  _dwInputPanel = new QDockWidget(aParent);
  _dwInputPanel->setVisible(false);
  _dwInputPanel->setWindowTitle("Input Panel");
  _dwInputPanel->setMinimumHeight(300);
  _dwInputPanel->setMinimumWidth(500); // --- force a minimum until display
//   _dialogView = new DialogView(_dwInputPanel);
//   _dialogView = new SingleItemView(_dwInputPanel);
//   _dialogView->setMinimumHeight(400);
//   _dialogView->setItemDelegate(_treeViewDelegate);
//   _dwInputPanel->setWidget(_dialogView);
//   _dialogView->show();
  _dwInputPanel->raise();


  //2) ************* document data ( Pattern, Association, Mesh ) in treeview representation
  _treeViewDelegate = new DocumentDelegate(_dwInputPanel);
  
  //      Pattern
  _dwPattern = new QDockWidget(aParent);
  _dwPattern->setVisible(false);
  _dwPattern->setWindowTitle("Model");
  _dwPattern->setMinimumWidth(DW_MINIMUM_WIDTH); // --- force a minimum until display
//     OB_Browser* o = new OB_Browser;
//   _patternDataTreeView = o->treeView();//new QTreeView(_dwPattern);
//   _patternDataTreeView = new OB_Browser;

  QFrame*      patternFrame  = new QFrame(_dwPattern);
  QVBoxLayout* patternLayout = new QVBoxLayout(patternFrame);
  _patternDataTreeView    = new QTreeView(patternFrame); //_dwPattern);
  _patternBuilderTreeView = new QTreeView(patternFrame);
  patternLayout->addWidget(_patternDataTreeView );
  patternLayout->addWidget(_patternBuilderTreeView );
//   _patternDataTreeView->setMinimumHeight(DW_MINIMUM_WIDTH); 


  _patternDataTreeView->setEditTriggers(QAbstractItemView::AllEditTriggers); 
  _patternBuilderTreeView->setEditTriggers(QAbstractItemView::AllEditTriggers);
  //QAbstractItemView::DoubleClicked, QAbstractItemView::SelectedClicked)

  _patternDataTreeView->setItemDelegate(_treeViewDelegate);
  _patternBuilderTreeView->setItemDelegate(_treeViewDelegate);
  _dwPattern->setWidget(patternFrame);
  patternFrame->show();
  //_dwPattern->raise();

  //      Association
  _dwAssociation = new QDockWidget(aParent);
  _dwAssociation->setVisible(false);
  _dwAssociation->setWindowTitle("Association");
  _dwAssociation->setMinimumWidth(DW_MINIMUM_WIDTH); // --- force a minimum until display
  _associationTreeView = new QTreeView(_dwAssociation);
  //   _associationTreeView->setMinimumHeight(DW_MINIMUM_WIDTH);
  _associationTreeView->setEditTriggers(QAbstractItemView::AllEditTriggers);//QAbstractItemView::SelectedClicked); 
  _associationTreeView->setItemDelegate(_treeViewDelegate);
  _dwAssociation->setWidget(_associationTreeView);
  _associationTreeView->show();
  //   _dwAssociation->raise();

  //      Groups
  _dwGroups = new QDockWidget(aParent);
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
  _dwMesh->setVisible(false);
  _dwMesh->setWindowTitle("Mesh");
  _dwMesh->setMinimumWidth(DW_MINIMUM_WIDTH); // --- force a minimum until display
  _meshTreeView = new QTreeView(_dwMesh);
  //   _meshTreeView->setMinimumHeight(DW_MINIMUM_WIDTH);
  _meshTreeView->setEditTriggers(QAbstractItemView::AllEditTriggers);
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
  _dwObjectBrowser->setMinimumWidth(DW_MINIMUM_WIDTH); // --- force a minimum until display
  _dwObjectBrowser->setWindowTitle("Study");


  // dock widget position
  //   aParent->addDockWidget(Qt::LeftDockWidgetArea,  _dwPattern);
  //   aParent->addDockWidget(Qt::RightDockWidgetArea, _dwInputPanel);
  //   aParent->addDockWidget( Qt::LeftDockWidgetArea, _dwPattern ); //static_cast<Qt::DockWidgetArea>(1)
  aParent->addDockWidget( Qt::LeftDockWidgetArea, _dwObjectBrowser ); //static_cast<Qt::DockWidgetArea>(1)
  aParent->addDockWidget( Qt::LeftDockWidgetArea, _dwInputPanel );

  aParent->tabifyDockWidget( _dwObjectBrowser, _dwPattern );
  aParent->tabifyDockWidget( _dwPattern, _dwAssociation );
  aParent->tabifyDockWidget( _dwAssociation, _dwGroups );
  aParent->tabifyDockWidget( _dwGroups, _dwMesh );



#if QT_VERSION >= 0x040500
  aParent->setTabPosition(Qt::AllDockWidgetAreas, Resource::tabPanelsUp? QTabWidget::North: QTabWidget::South);
#endif


}


void HEXABLOCKGUI::createActions()
{
  QMainWindow *aParent = application()->desktop();

  // Document 
  _newAct = createAction( _menuId++, tr("Create a new document"), QIcon("icons:new_document.png"),
                                            tr("New document"),  tr("Create a new document"),
                                            0, aParent, false, this,  SLOT(newDocument()) );
  _newAct->setShortcut( Qt::CTRL + Qt::SHIFT + Qt::Key_N ); // --- QKeySequence::New ambiguous in SALOME

  _importAct = createAction( _menuId++, tr("Import a document"), QIcon("icons:import_document.png"),
                                            tr("Import document"), tr("Import a document"),
                                            0, aParent, false, this,  SLOT(importDocument()) );
  _importAct->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_O); // --- QKeySequence::Open ambiguous in SALOME


  // Pattern Data creation
  _addVertex = createAction( _menuId++, tr("Create a vertex"), QIcon("icons:add_vertex.png"),
                                            tr("Add vertex"),  tr("Create a new vertex"),
                                            0, aParent, false, this,  SLOT(addVertex()) );

  _addQuad = createAction( _menuId++, tr("Create a quad"), QIcon("icons:add_quad.png"),
                                            tr("Add quad"),  tr("Create a new quad"),
                                            0, aParent, false, this,  SLOT(addQuad()) );

  _addHexa = createAction( _menuId++, tr("Create an hexa"), QIcon("icons:add_hexa.png"),
                                            tr("Add hexa"),  tr("Create a new hexa"),
                                            0, aParent, false, this,  SLOT(addHexa()) );


  // Pattern Data edition
  _mergeVertices = createAction( _menuId++, tr("Merge vertices"), QIcon("icons:merge_vertices.png"),
                                            tr("Merge vertices"),  tr("Merge vertices"),
                                            0, aParent, false, this,  SLOT(mergeVertices()) );

  _mergeEdges = createAction( _menuId++, tr("Merge edges"), QIcon("icons:merge_edges.png"),
                                         tr("Merge edges"),  tr("Merge edges"),
                                            0, aParent, false, this,  SLOT(mergeEdges()) );

  _cutEdge    = createAction( _menuId++, tr("Cut edge"), QIcon("icons:cut_edge.png"),
                                         tr("Cut edge"),  tr("Cut edge"),
                                            0, aParent, false, this,  SLOT(cutEdge()) );



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
  int aMenuId;
  aMenuId = createMenu( "File", -1, -1 );
  createMenu( separator(), aMenuId, -1, 10 );
  aMenuId = createMenu( "HEXABLOCK" , aMenuId, -1, 10 );
  createMenu( _newAct, aMenuId );
  createMenu( _importAct, aMenuId );

//   aMenuId = createMenu( tr( "HEXABLOCK" ), -1, -1, 30 );
  aMenuId = createMenu( tr( "Object Browser" ), -1, -1, 30 );
  createMenu( _newAct, aMenuId );//, 10
  createMenu( _importAct, aMenuId );


//   _wrapper->createMenu( _importSchemaAct, aMenuId );
//   _wrapper->createMenu( _importSupervSchemaAct, aMenuId );
//   _wrapper->createMenu( _wrapper->separator(), aMenuId);
//   _wrapper->createMenu( _exportSchemaAct, aMenuId );
//   _wrapper->createMenu( _exportSchemaAsAct, aMenuId );
//   _wrapper->createMenu( _wrapper->separator(), aMenuId);
//   _wrapper->createMenu( _runLoadedSchemaAct, aMenuId );
//   _wrapper->createMenu( _loadRunStateSchemaAct, aMenuId );
//   _wrapper->createMenu( _loadAndRunSchemaAct, aMenuId );
//   _wrapper->createMenu( _loadBatchAct, aMenuId );
//   _wrapper->createMenu( _wrapper->separator(), aMenuId);
//   _wrapper->createMenu( _undoAct, aMenuId );
//   _wrapper->createMenu( _redoAct, aMenuId );
//   _wrapper->createMenu( _showUndoAct, aMenuId );
//   _wrapper->createMenu( _showRedoAct, aMenuId );
//   _wrapper->createMenu( _wrapper->separator(), aMenuId);
//   _wrapper->createMenu( _startResumeAct, aMenuId );
//   _wrapper->createMenu( _abortAct, aMenuId );
//   _wrapper->createMenu( _pauseAct, aMenuId );
//   _wrapper->createMenu( _resetAct, aMenuId );
//   _wrapper->createMenu( _wrapper->separator(), aMenuId);
//   _wrapper->createMenu( _saveRunStateAct, aMenuId );
//   //_wrapper->createMenu( _newEditionAct, aMenuId );
//   _wrapper->createMenu( _wrapper->separator(), aMenuId);
//   _wrapper->createMenu( _withoutStopModeAct, aMenuId );
//   _wrapper->createMenu( _breakpointsModeAct, aMenuId );
//   _wrapper->createMenu( _stepByStepModeAct, aMenuId );
//   _wrapper->createMenu( _wrapper->separator(), aMenuId);
//   _wrapper->createMenu( _toggleStopOnErrorAct, aMenuId );
//   _wrapper->createMenu( _wrapper->separator(), aMenuId);
//   _wrapper->createMenu( _importCatalogAct, aMenuId );
//   _wrapper->createMenu( _wrapper->separator(), aMenuId);
//   _wrapper->createMenu( _toggleStraightLinksAct, aMenuId );
//   _wrapper->createMenu( _toggleAutomaticComputeLinkAct, aMenuId );
//   _wrapper->createMenu( _toggleSimplifyLinkAct, aMenuId );
//   _wrapper->createMenu( _toggleForce2NodesLinkAct, aMenuId );
//   _wrapper->createMenu( _toggleAddRowColsAct, aMenuId );
//   _wrapper->createMenu( _wrapper->separator(), aMenuId);
//   _wrapper->createMenu( _showAllLinksAct, aMenuId );
//   _wrapper->createMenu( _hideAllLinksAct, aMenuId );
//   _wrapper->createMenu( _wrapper->separator(), aMenuId);
//   _wrapper->createMenu( _whatsThisAct, aMenuId );
  aMenuId = createMenu( tr("Pattern"), -1, -1, 30 );
  createMenu( _addVertex, aMenuId );
  createMenu( _addQuad,   aMenuId );
  createMenu( _addHexa,   aMenuId );
  

//   aMenuId = createMenu( tr("Association"), -1, -1, 30 );
//   createMenu( _newAct, aMenuId );
//   createMenu( _importAct, aMenuId );
// 
//   aMenuId = createMenu( tr("Groups"), -1, -1, 30 );
//   createMenu( _newAct, aMenuId );
//   createMenu( _importAct, aMenuId );
// 
//   aMenuId = createMenu( tr("Mesh"), -1, -1, 30 );
//   createMenu( _newAct, aMenuId );
//   createMenu( _importAct, aMenuId );


}


void HEXABLOCKGUI::createTools()
{
  int aToolId = createTool ( tr( "HEXABLOCK Toolbar" ) );

  createTool( _newAct, aToolId );
  createTool( _importAct, aToolId );

  createTool( separator(), aToolId );

  createTool( _addVertex, aToolId );
  createTool( _addQuad, aToolId );
  createTool( _addHexa, aToolId );

  createTool( separator(), aToolId );

  createTool( _mergeVertices, aToolId );
  createTool( _mergeEdges,    aToolId );
  createTool( _cutEdge,       aToolId );



//   _wrapper->createTool( _exportSchemaAsAct, aToolId );
//   _wrapper->createTool( _wrapper->separator(), aToolId);
//   _wrapper->createTool( _runLoadedSchemaAct, aToolId );
//   _wrapper->createTool( _loadRunStateSchemaAct, aToolId );
//   _wrapper->createTool( _loadAndRunSchemaAct, aToolId );
//   _wrapper->createTool( _loadBatchAct, aToolId );
//   _wrapper->createTool( _wrapper->separator(), aToolId );
//   _wrapper->createTool( _undoAct, aToolId );
//   _wrapper->createTool( _redoAct, aToolId );
//   _wrapper->createTool( _wrapper->separator(), aToolId );
//   _wrapper->createTool( _startResumeAct, aToolId );
//   _wrapper->createTool( _abortAct, aToolId );
//   _wrapper->createTool( _pauseAct, aToolId );
//   _wrapper->createTool( _resetAct, aToolId );
//   _wrapper->createTool( _wrapper->separator(), aToolId );
//   _wrapper->createTool( _saveRunStateAct, aToolId );
//   //_wrapper->createTool( _newEditionAct, aToolId );
//   _wrapper->createTool( _withoutStopModeAct, aToolId );
//   _wrapper->createTool( _breakpointsModeAct, aToolId );
//   _wrapper->createTool( _stepByStepModeAct, aToolId );
//   _wrapper->createTool( _wrapper->separator(), aToolId );
//   _wrapper->createTool( _toggleStopOnErrorAct, aToolId );
//   _wrapper->createTool( _wrapper->separator(), aToolId );
//   _wrapper->createTool( _importCatalogAct, aToolId );
//   _wrapper->createTool( _wrapper->separator(), aToolId );
//   _wrapper->createTool( _toggleStraightLinksAct, aToolId );
//   _wrapper->createTool( _toggleAutomaticComputeLinkAct, aToolId );
//   _wrapper->createTool( _toggleSimplifyLinkAct, aToolId );
//   //_wrapper->createTool( _toggleForce2NodesLinkAct, aToolId );
//   _wrapper->createTool( _toggleAddRowColsAct, aToolId );
//   _wrapper->createTool( _wrapper->separator(), aToolId );
//   _wrapper->createTool( _showAllLinksAct, aToolId );
//   _wrapper->createTool( _hideAllLinksAct, aToolId );
//   _wrapper->createTool( _wrapper->separator(), aToolId );
//   _wrapper->createTool( _whatsThisAct, aToolId );
}

void HEXABLOCKGUI::initialMenus()
{
  showEditionMenus(false);
  showExecMenus(false);
  showCommonMenus(false);
  showBaseMenus(true);
}

void HEXABLOCKGUI::showBaseMenus(bool show)
{
  DEBTRACE("HEXABLOCKGUI::showBaseMenus " << show);
  setMenuShown(_newAct, show);
  setToolShown(_newAct, show);
  setMenuShown(_importAct, show);
  setToolShown(_importAct, show);
}

void HEXABLOCKGUI::showEditionMenus(bool show)
{
  DEBTRACE("HEXABLOCKGUI::showEditionMenus " << show);
  setMenuShown(_addVertex, show);
  setToolShown(_addVertex, show);
  setMenuShown(_addQuad, show);
  setToolShown(_addQuad, show);
  setMenuShown(_addHexa, show);
  setToolShown(_addHexa, show);

  setMenuShown(_mergeVertices, show);
  setToolShown(_mergeVertices, show);
  setMenuShown(_mergeEdges, show);
  setToolShown(_mergeEdges, show);
  setMenuShown(_cutEdge, show);
  setToolShown(_cutEdge, show);


//   _wrapper->setMenuShown(_exportSchemaAct, show);
//   _wrapper->setToolShown(_exportSchemaAct, show);
//   _wrapper->setMenuShown(_exportSchemaAsAct, show);
//   _wrapper->setToolShown(_exportSchemaAsAct, show);
//   _wrapper->setMenuShown(_runLoadedSchemaAct, show);
//   _wrapper->setToolShown(_loadRunStateSchemaAct, show);
//   _wrapper->setMenuShown(_loadRunStateSchemaAct, show);
//   _wrapper->setToolShown(_runLoadedSchemaAct, show);
//   _wrapper->setMenuShown(_undoAct, show);
//   _wrapper->setToolShown(_undoAct, show);
//   _wrapper->setMenuShown(_redoAct, show);
//   _wrapper->setToolShown(_redoAct, show);
//   _wrapper->setMenuShown(_showUndoAct, show);
//   _wrapper->setMenuShown(_showRedoAct, show);
//   _wrapper->setMenuShown(_loadBatchAct, show);
//   _wrapper->setToolShown(_loadBatchAct, show);
//   _wrapper->setMenuShown(_importCatalogAct, show);
//   _wrapper->setToolShown(_importCatalogAct, show);
}

void HEXABLOCKGUI::showExecMenus(bool show)
{
  DEBTRACE("HEXABLOCKGUI::showExecMenus " << show);
//   _wrapper->setMenuShown(_startResumeAct, show);
//   _wrapper->setToolShown(_startResumeAct, show);
//   _wrapper->setMenuShown(_abortAct, show);
//   _wrapper->setToolShown(_abortAct, show);
//   _wrapper->setMenuShown(_pauseAct, show);
//   _wrapper->setToolShown(_pauseAct, show);
//   _wrapper->setMenuShown(_resetAct, show);
//   _wrapper->setToolShown(_resetAct, show);
//   _wrapper->setMenuShown(_saveRunStateAct, show);
//   _wrapper->setToolShown(_saveRunStateAct, show);
//   //_wrapper->setMenuShown(_newEditionAct, show);
//   //_wrapper->setToolShown(_newEditionAct, show);
//   _wrapper->setMenuShown(_withoutStopModeAct, show);
//   _wrapper->setToolShown(_withoutStopModeAct, show);
//   _wrapper->setMenuShown(_breakpointsModeAct, show);
//   _wrapper->setToolShown(_breakpointsModeAct, show);
//   _wrapper->setMenuShown(_stepByStepModeAct, show);
//   _wrapper->setToolShown(_stepByStepModeAct, show);
//   _wrapper->setMenuShown(_toggleStopOnErrorAct, show);
//   _wrapper->setToolShown(_toggleStopOnErrorAct, show);
}

void HEXABLOCKGUI::showCommonMenus(bool show)
{
  DEBTRACE("HEXABLOCKGUI::showCommonMenus " << show);
//   _wrapper->setMenuShown(_toggleStraightLinksAct, show);
//   _wrapper->setToolShown(_toggleStraightLinksAct, show);
//   _wrapper->setMenuShown(_toggleAutomaticComputeLinkAct, show);
//   _wrapper->setToolShown(_toggleAutomaticComputeLinkAct, show);
//   _wrapper->setMenuShown(_toggleSimplifyLinkAct, show);
//   _wrapper->setToolShown(_toggleSimplifyLinkAct, show);
//   _wrapper->setMenuShown(_toggleForce2NodesLinkAct, show);
//   //_wrapper->setToolShown(_toggleForce2NodesLinkAct, show);
//   _wrapper->setMenuShown(_toggleAddRowColsAct, show);
//   _wrapper->setToolShown(_toggleAddRowColsAct, show);
//   _wrapper->setMenuShown(_showAllLinksAct, show);
//   _wrapper->setToolShown(_showAllLinksAct, show);
//   _wrapper->setMenuShown(_hideAllLinksAct, show);
//   _wrapper->setToolShown(_hideAllLinksAct, show);
}




void HEXABLOCKGUI::switchModel(SUIT_ViewWindow *view)
{
  DEBTRACE("HEXABLOCKGUI::switchModel " << view);
  if ( _mapViewModel.count(view) ){

    // model
    _currentModel = _mapViewModel[view];
    _patternDataModel->setSourceModel(_currentModel);
    _patternBuilderModel->setSourceModel(_currentModel);

    // setting model ( in view )
    _patternDataTreeView->setModel(_patternDataModel);
    _patternBuilderTreeView->setModel(_patternBuilderModel);
//     _associationTreeView->setModel(_currentModel);
//     _groupsTreeView->setModel(_currentModel);
//     _meshTreeView->setModel(_currentModel);
    _currentGraphicView->setModel(_patternDataModel);

    // setting selection ( in view )
    _patternDataTreeView->setSelectionModel(_currentSelectionModel);
    //_patternDataTreeView->setSelectionMode( QAbstractItemView::MultiSelection );
    _patternDataTreeView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    _patternBuilderTreeView->setSelectionModel(_currentSelectionModel);
    _patternBuilderTreeView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    _currentGraphicView->setSelectionModel(_currentSelectionModel);

    showEditionMenus(true);
  } else {
    DEBTRACE("HEXABLOCKGUI::switchModel : no model found, cannot switch");
    initialMenus();
  }
}


// bool HEXABLOCKGUI::closeContext(QWidget *view)
// {
//   DEBTRACE("HEXABLOCKGUI::closeContext");
//   if (! _mapViewContext.count(view))
//     return true;
//   QtGuiContext* context = _mapViewContext[view];
//   switchModel(view);
// 
//   bool tryToSave = false;
// 
//   if (QtGuiContext::getQtCurrent()->isEdition())
//     {
//       if (!QtGuiContext::getQtCurrent()->_setOfModifiedSubjects.empty())
//         {
//           QMessageBox msgBox;
//           msgBox.setText("Some elements are modified and not taken into account.");
//           string info = "do you want to apply your changes ?\n";
//           info += " - Save    : do not take into account edition in progress,\n";
//           info += "             but if there are other modifications, select a file name for save\n";
//           info += " - Discard : discard all modifications and close the schema\n";
//           info += " - Cancel  : do not close the schema, return to edition";
//           msgBox.setInformativeText(info.c_str());
//           msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
//           msgBox.setDefaultButton(QMessageBox::Cancel);
//           int ret = msgBox.exec();
//           switch (ret)
//             {
//             case QMessageBox::Save:
//               tryToSave = true;
//               break;
//             case QMessageBox::Discard:
//               tryToSave = false;
//               break;
//             case QMessageBox::Cancel:
//             default:
//               return false;
//               break;
//             }
//         }
//       else
//         if (QtGuiContext::getQtCurrent()->isNotSaved())
//           {
//             QMessageBox msgBox;
//             msgBox.setText("The schema has been modified");
//             string info = "do you want to save the schema ?\n";
//             info += " - Save    : select a file name for save\n";
//             info += " - Discard : discard all modifications and close the schema\n";
//             info += " - Cancel  : do not close the schema, return to edition";
//             msgBox.setInformativeText(info.c_str());
//             msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
//             msgBox.setDefaultButton(QMessageBox::Cancel);
//             int ret = msgBox.exec();
//             switch (ret)
//               {
//               case QMessageBox::Save:
//                 tryToSave = true;
//                 break;
//               case QMessageBox::Discard:
//                 tryToSave = false;
//                 break;
//               case QMessageBox::Cancel:
//               default:
//                 return false;
//                 break;
//               }
//           }
// 
//       if (tryToSave)
//         {
//           onExportSchemaAs();
//           if (! _isSaved) // --- probably, user has cancelled the save dialog. Do not close
//             return false;
//         }
//     }
// 
//   map<QWidget*, HEXABLOCK::HMI::QtGuiContext*>::iterator it = _mapViewContext.begin();
//   QtGuiContext* newContext = 0;
//   QWidget* newView = 0;
//   for (; it != _mapViewContext.end(); ++it)
//     {
//       if ((*it).second != context)
//         {
//           newView = (*it).first;
//           newContext = (*it).second;
//           break;
//         }
//     }
//   int studyId = _wrapper->activeStudyId();
//   if (context->getStudyId() == studyId)
//     {
//       _wrapper->deleteSchema(view);
//       DEBTRACE("delete context");
//       if (GuiExecutor* exec = context->getGuiExecutor())
//         {
//           exec->closeContext();
//         }
//       delete context;
//       _mapViewContext.erase(view);
//       switchModel(newView);
//     }
//   return true;
// }

void HEXABLOCKGUI::showDockWidgets(bool isVisible)
{
  DEBTRACE("HEXABLOCKGUI::showDockWidgets " << isVisible);

  if (_dwPattern) _dwPattern->setVisible(isVisible);
  if (_dwPattern) _dwPattern->toggleViewAction()->setVisible(isVisible);
  if (_dwAssociation) _dwAssociation->setVisible(isVisible);
  if (_dwAssociation) _dwAssociation->toggleViewAction()->setVisible(isVisible);
  if (_dwMesh) _dwMesh->setVisible(isVisible);
  if (_dwMesh) _dwMesh->toggleViewAction()->setVisible(isVisible);
  if (_dwGroups) _dwGroups->setVisible(isVisible);
  if (_dwGroups) _dwGroups->toggleViewAction()->setVisible(isVisible);

  if (_dwInputPanel) _dwInputPanel->setVisible(isVisible);
  if (_dwInputPanel) _dwInputPanel->toggleViewAction()->setVisible(isVisible);

  if ( isVisible ) _dwPattern->raise();
}


// void HEXABLOCKGUI::raiseStacked()
// {
// //   if (_dwStacked) _dwStacked->raise();
// }


DocumentGraphicView* HEXABLOCKGUI::newGraphicView()
{
  DocumentGraphicView *newGView = 0;

  SUIT_ViewWindow *suitVW  = 0;
//   SVTK_ViewWindow *suitVW  = 0;
  if (_suitVM){// --- reuse already created view manager 
    _suitVM = getApp()->getViewManager(SVTK_Viewer::Type(), true);
    suitVW = _suitVM->createViewWindow();
  } else {
    _suitVM = getApp()->createViewManager(SVTK_Viewer::Type());
    suitVW = _suitVM->getActiveView();
  }
  newGView = new DocumentGraphicView(getApp(), suitVW, application()->desktop());
  return newGView;
}






void HEXABLOCKGUI::newDocument()
{
  DEBTRACE("HEXABLOCKGUI::newDocument");

  std::stringstream name;
  name << "newDoc_" << ++_documentCnt;

  QString fileName = name.str().c_str();

  QMainWindow *aParent = application()->desktop();
  QWidget *central = aParent->centralWidget();
  if (central)
    central->setFocus();
  else
    DEBTRACE("No Central Widget");

  // --- new Doc Model
  _currentModel     = new DocumentModel(this); //CS_TOCHECK this
//   _currentModel->setHeaderData(0, Qt::Horizontal, tr("HELLOH0"));
//   _currentModel->setHeaderData(1, Qt::Horizontal, tr("HELLOH1"));
//   _currentModel->setHeaderData(0, Qt::Vertical, tr("HELLOV0"));
  _patternDataModel    = new PatternDataModel(this);
  _patternBuilderModel = new PatternBuilderModel(this);
  _patternDataModel->setSourceModel(_currentModel);
  _patternBuilderModel->setSourceModel(_currentModel);

  // --- new Graphic view ( SVTK )
  _currentGraphicView  = newGraphicView();

  // --- setting model
  _patternDataTreeView->setModel(_patternDataModel);//_currentModel);
  _patternBuilderTreeView->setModel(_patternBuilderModel);
  _associationTreeView->setModel(_currentModel);
  _meshTreeView->setModel(_currentModel);
  _currentGraphicView->setModel(_patternDataModel);

  // --- setting selection model
  if (!_currentSelectionModel)
     _currentSelectionModel = new DocumentSelectionModel(_patternDataModel); //_currentModel

  _patternDataTreeView->setSelectionModel(_currentSelectionModel);
//   _patternBuilderTreeView->setSelectionModel(_currentSelectionModel);
//   _associationTreeView->setSelectionModel(_currentSelectionModel);
//   _meshTreeView->setSelectionModel(_currentSelectionModel);
  _currentGraphicView->setSelectionModel(_currentSelectionModel);


//   //CS_TEST
//   // ----------
//   QStandardItem *parentItem = _currentModel->invisibleRootItem();
//   QStandardItem *myItem =  new QStandardItem("MyItem");
//   parentItem->appendRow(myItem);
  // ----------
//   QModelIndex v0 = _patternDataModel->addVertex(0., 0., 0.);
//   QModelIndex v1 = _patternDataModel->addVertex(5., 0., 0.);
//   QModelIndex v2 = _patternDataModel->addVertex(5., 5., 0.);
//   QModelIndex v3 = _patternDataModel->addVertex(0., 5., 0.);
//   QModelIndex v4 = _patternDataModel->addVertex(0., 0., 5.);
//   QModelIndex v5 = _patternDataModel->addVertex(5., 0., 5.);
//   QModelIndex v6 = _patternDataModel->addVertex(5., 5., 5.);
//   QModelIndex v7 = _patternDataModel->addVertex(0., 5., 5.);
// 
// 
//   QModelIndex q0 = _patternDataModel->addQuad( v0, v1, v2, v3 );
//   QModelIndex q1 = _patternDataModel->addQuad( v4, v5, v6, v7 );
//   QModelIndex q2 = _patternDataModel->addQuad( v0, v3, v7, v4 );
//   QModelIndex q3 = _patternDataModel->addQuad( v1, v2, v6, v5 );
//   QModelIndex q4 = _patternDataModel->addQuad( v0, v1, v5, v4 );
//   QModelIndex q5 = _patternDataModel->addQuad( v3, v2, v6, v7 );
// 
// 
//   QModelIndex h0 = _patternDataModel->addHexaFromQuad( q0, q1, q2, q3, q4, q5 );


//   //CS_TEST

  _mapViewModel[ _currentGraphicView->get_SUIT_ViewWindow() ] = _currentModel;

  showDockWidgets(true);
}



void HEXABLOCKGUI::importDocument( const QString &inFile )
{
  DEBTRACE("HEXABLOCKGUI::importDocument");
  QMainWindow *aParent = application()->desktop();
  QString selectedFile;

  if ( inFile.isNull() ){
    QFileDialog dialog( aParent, "Choose a filename to load" ,
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
  }

}


void HEXABLOCKGUI::addVertex()
{
    if (!_dwInputPanel) return;

    VertexDialog* diag = new VertexDialog(_dwInputPanel);
    diag->setModel(_patternDataModel);//_currentModel);
    diag->setSelectionModel(_currentSelectionModel);
    _dwInputPanel->setWidget(diag);
    //   diag->show();
}

void HEXABLOCKGUI::addQuad()
{
  if (!_dwInputPanel) return;

  QuadDialog* diag = new QuadDialog(_dwInputPanel);
  diag->setModel(_patternDataModel);//_currentModel);
  diag->setSelectionModel(_currentSelectionModel);
  _dwInputPanel->setWidget(diag);
  //   diag->show();
}


void HEXABLOCKGUI::addHexa()
{
  if (!_dwInputPanel) return;

  HexaDialog* diag = new HexaDialog(_dwInputPanel);
  diag->setModel(_patternDataModel);//_currentModel);
  diag->setSelectionModel(_currentSelectionModel);
  _dwInputPanel->setWidget(diag);
  //   diag->show();
}


void HEXABLOCKGUI::mergeVertices()
{
  if (!_dwInputPanel) return;

  MergeVerticesDialog* diag = new MergeVerticesDialog(_dwInputPanel);
  diag->setModel(_patternDataModel);//_currentModel);
  diag->setSelectionModel(_currentSelectionModel);
  _dwInputPanel->setWidget(diag);
}


void HEXABLOCKGUI::mergeEdges()
{
  if (!_dwInputPanel) return;

  MergeEdgesDialog* diag = new MergeEdgesDialog(_dwInputPanel);
  diag->setModel(_patternDataModel);//_currentModel);
  diag->setSelectionModel(_currentSelectionModel);
  _dwInputPanel->setWidget(diag);
}


void HEXABLOCKGUI::cutEdge()
{
  if (!_dwInputPanel) return;

  CutEdgeDialog* diag = new CutEdgeDialog(_dwInputPanel);
  diag->setModel(_patternDataModel);//_currentModel);
  diag->setSelectionModel(_currentSelectionModel);
  _dwInputPanel->setWidget(diag);
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
}






