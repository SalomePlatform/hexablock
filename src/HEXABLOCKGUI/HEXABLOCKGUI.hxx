//  Copyright (C) 2009-2011  CEA/DEN, EDF R&D
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

#ifndef _HEXABLOCKGUI_HXX_
#define _HEXABLOCKGUI_HXX_

#include <iostream>
#include <map>

#include <QTreeView>
#include <QModelIndex>
#include <QDockWidget>

#include "klinkitemselectionmodel.hxx"

#include <OB_Browser.h>
#include <SalomeApp_Module.h>
// #include <LightApp_Module.h>

#include <LightApp_SelectionMgr.h>
#include <LightApp_DataModel.h>

#include <SALOMEconfig.h>

// #include "Resource.hxx"
#include "HEXABLOCKGUI_Resource.hxx"

#include "GEOMGUI_OCCSelector.h"


#include CORBA_CLIENT_HEADER(HEXABLOCK_Gen)
// #include CORBA_CLIENT_HEADER(GEOM_Gen)
// #include CORBA_CLIENT_HEADER(SMESH_Gen)


namespace HEXABLOCK
{
  namespace GUI
  {
    class DocumentGraphicView;
    class DocumentDelegate;
    class DocumentModel;
    class PatternDataModel;
    class PatternBuilderModel;
    class AssociationsModel;
    class GroupsModel;
    class MeshModel;
    class PatternDataSelectionModel;
    class PatternBuilderSelectionModel;
    class MeshSelectionModel;
  }
}



class SalomeApp_Study;
class SalomeApp_Application;
class SUIT_ViewWindow;
class SVTK_ViewWindow;
class OCCViewer_ViewWindow;

// // SALOME KERNEL includes
// #include <SALOMEDS_Study.hxx>
// #include <SALOMEDSClient_StudyBuilder.hxx>
// #include <SALOMEDSClient_SComponent.hxx>
// #include <SALOMEDSClient_ClientFactory.hxx>
// #include <SALOMEDSClient_IParameters.hxx>


class HEXABLOCKGUI : public SalomeApp_Module
{
  Q_OBJECT

  friend class HEXABLOCKGUI_Resource;

public:
  HEXABLOCKGUI();
  virtual ~HEXABLOCKGUI();

  static SalomeApp_Study*         activeStudy();

  static HEXABLOCK_ORB::HEXABLOCK_Gen_ptr InitHEXABLOCKGen( SalomeApp_Application* );
//   static SMESH::SMESH_Gen_ptr             InitSMESHGen( SalomeApp_Application* app, const std::string& container = "FactoryServer" );
  static GEOM::GEOM_Gen_ptr InitGEOMGen( SalomeApp_Application* app, const std::string& container = "FactoryServer" );

  static LightApp_SelectionMgr*   selectionMgr();


  static SVTK_ViewWindow*      currentVtkView;
  static OCCViewer_ViewWindow* currentOccView;

 //HEXABLOCK::GUI::DocumentModel*

  virtual void initialize( CAM_Application* app);
  virtual void windows( QMap<int, int>& theMap) const;
//   virtual LightApp_Displayer* displayer();
  virtual QString  engineIOR() const;

  virtual void viewManagers(QStringList& list) const;
  virtual void setResource(SUIT_ResourceMgr* r);
  virtual void createPreferences();
  virtual void preferencesChanged( const QString& sect, const QString& name );
  virtual void studyActivated();

  //------------------------------------
  void createAndFillDockWidget();
  void createActions();
  void createMenus();
  void createTools();

  void initialMenus();
  void showAllMenus();

//   void showBaseMenus(bool show);
//   void showEditionMenus(bool show);
//   void showExecMenus(bool show);
//   void showCommonMenus(bool show);

  void switchModel(SUIT_ViewWindow *view);
  void showDockWidgets(bool isVisible);


public slots:
  bool deactivateModule( SUIT_Study* theStudy);
  bool activateModule( SUIT_Study* theStudy);
  void onObjectBrowserClick(const QModelIndex& index);

  void showDocumentMenus(bool show);
  void showPatternMenus(bool show);
  void showAssociationMenus(bool show);
  void showGroupsMenus(bool show);
  void showMeshMenus(bool show);
  
protected slots:
  void onWindowActivated( SUIT_ViewWindow* svw);
  void onWindowClosed( SUIT_ViewWindow* svw);

  virtual void                        onViewManagerAdded( SUIT_ViewManager* );
  virtual void                        onViewManagerRemoved( SUIT_ViewManager* );
//   void onTryClose(bool &isClosed, QxScene_ViewWindow* window);

protected:
//   virtual  CAM_DataModel* createDataModel();
  bool createSComponent();
//   virtual bool isSelectionCompatible();


//   bool _selectFromTree;
  HEXABLOCKGUI_Resource* _myresource;
  static int _oldStudyId;
  static HEXABLOCK_ORB::HEXABLOCK_Gen_var _hexaEngine;
//   static SMESH::SMESH_Gen_var             _smeshEngine;
  static GEOM::GEOM_Gen_var               _geomEngine;
private slots:

  void treeContextMenu(const QPoint& aPosition);

  void test_association();
  void test();

  void newDocument();
  void importDocument( const QString &path = QString() );
  void saveDocument();

  // new
  void addVertex();
  void addEdge();
  void addQuad();
  void addHexa();

  void addVector();
  void addCylinder();
  void addPipe();
//   void makeCartesian();
//   void makeCylindrical();
  void makeGrid();
  void makeCylinder();
  void makePipe();
  void makeCylinders();
  void makePipes();

  void makeRind();    // NEW HEXA3
  void makeHemiSphere();  // NEW HEXA3

  void removeHexa();
  void prismQuad();
  void joinQuad();
//   void mergeVertices();
//   void mergeEdges();
  void merge();
  void disconnectElts();
  void cutEdge();
//   void makeTranslation();
  void makeTransformation();
  void makeSymmetry();
  void performTransformation();
  void performSymmetry();

  void replaceHexa();    // NEW HEXA3
  void quadRevolution(); // NEW HEXA3


  void assocVertex();
  void assocEdge();
  void assocQuad();

  void addGroup();
  void removeGroup (); //CS_TODO
  void addLaw();
  void removeLaw();
  void setPropagation();
  void computeMesh();

  void clearAssociations();

private:

  QList<GEOMGUI_OCCSelector*>  myOCCSelectors;


  QStringList getQuickDirList();
  HEXABLOCK::GUI::DocumentGraphicView* newGraphicView();


  // -------------------------------------------------------------------------------------------------
  //          MainWindow presentation
  // -------------------------------------------------------------------------------------------------
  QDockWidget *_dwPattern;       // Hexablock model edition
  QDockWidget *_dwAssociation;   // Hexablock association edition
  QDockWidget *_dwGroups;        // Hexablock groups edition
  QDockWidget *_dwMesh;          // Hexablock meshing edtion
  QDockWidget *_dwObjectBrowser; // Salome study
  QDockWidget *_dwInputPanel;    // user Input

  // Actions
  int _menuId;
  // Object Browser
  QAction *_newAct;
  QAction *_importAct;
  QAction *_saveAct;
  QAction *_testAct;

  // Pattern Data
  QAction *_addVertex;
  QAction *_addEdge;
  QAction *_addQuad;
  QAction *_addHexa;

  // Pattern Builder
  QAction *_addVector;
  QAction *_addCylinder;
  QAction *_addPipe;
  QAction *_makeGrid; //Cartesian, Cylindrical, Spherical
  QAction *_makeCylinder;
  QAction *_makePipe;
  QAction *_makeCylinders;
  QAction *_makePipes;

//   QAction *_makeRind;   // NEW HEXA3
  QAction *_makeHemiSphere; // NEW HEXA3


  // Pattern Data Edition
  QAction *_removeHexa;
  QAction *_prismQuad;
  QAction *_joinQuad;
  QAction *_merge;//   QAction *_mergeVertices; //   QAction *_mergeEdges;
  QAction *_disconnect;
  QAction *_cutEdge;
  QAction *_makeTransformation; //   QAction *_makeTranslation;
  QAction *_makeSymmetry;
  QAction *_performTransformation;
  QAction *_performSymmetry;

  QAction *_replaceHexa;    // NEW HEXA3
  QAction *_quadRevolution; // NEW HEXA3

  // Pattern Data Association
  QAction *_assocVertex;
  QAction *_assocEdge;
  QAction *_assocQuad;

  // Group
  QAction *_addGroup;
  QAction *_removeGroup ; //CS_TODO

  // Law
  QAction *_addLaw;
  QAction *_removeLaw;

  // Propagation
  QAction *_setPropagation;

  // Meshing
  QAction *_computeMesh;

  // -------------------------------------------------------------------------------------------------
  //          Model/View implementation  
  // -------------------------------------------------------------------------------------------------

  //      MODEL      MODEL      MODEL      MODEL      MODEL      MODEL      MODEL      MODEL      MODEL
  HEXABLOCK::GUI::DocumentModel       *_currentModel;//  a model for each document : 1..n  ( multiple document allowed )
  HEXABLOCK::GUI::PatternDataModel    *_patternDataModel;     // sub-part of DocumentModel
  HEXABLOCK::GUI::PatternBuilderModel *_patternBuilderModel;  // sub-part of DocumentModel
  HEXABLOCK::GUI::AssociationsModel   *_associationsModel;    // sub-part of DocumentModel
  HEXABLOCK::GUI::GroupsModel         *_groupsModel;    // sub-part of DocumentModel
  HEXABLOCK::GUI::MeshModel           *_meshModel;      // sub-part of DocumentModel

  //      VIEW      VIEW      VIEW      VIEW      VIEW      VIEW      VIEW      VIEW      VIEW      VIEW
  QTreeView                           *_patternDataTreeView;    //  document's pattern : 1 ( only one view )
  QTreeView                           *_patternBuilderTreeView; //  document's pattern : 1 ( only one view )
  QTreeView                           *_associationTreeView;    //  document's association : 1 ( only one view )
  QTreeView                           *_groupsTreeView; //  document's groups
  QTreeView                           *_meshTreeView;   //  document's mesh property: 1 ( only one view )
  HEXABLOCK::GUI::DocumentGraphicView *_currentGraphicView;// graphical view (SVTK view) of the document : 1..n ( multiple view )

  //      DELEGATE      DELEGATE      DELEGATE      DELEGATE      DELEGATE      DELEGATE      DELEGATE
  HEXABLOCK::GUI::DocumentDelegate    *_treeViewDelegate;  // specific editor for each item of the tree 

  //    SELECTION_MODEL      SELECTION_MODEL      SELECTION_MODEL      SELECTION_MODEL     SELECTION_MODEL
  HEXABLOCK::GUI::PatternDataSelectionModel    *_patternDataSelectionModel;   // 1..n   selection
  HEXABLOCK::GUI::PatternBuilderSelectionModel *_patternBuilderSelectionModel;// 1..n   selection
  HEXABLOCK::GUI::MeshSelectionModel           *_meshSelectionModel;

  //  SALOME   SALOME    SALOME     SALOME     SALOME     SALOME     SALOME     SALOME     SALOME     SALOME
  SUIT_ViewManager *_suitVM;
  std::map<QString, SUIT_ViewWindow*>  _salomeViewWindows; //  key = entry

  // SALOME/QT    SALOME/QT  SALOME/QT    SALOME/QT   SALOME/QT    SALOME/QT  
  std::map<SUIT_ViewWindow*, HEXABLOCK::GUI::DocumentModel*>       _documentModels;
  std::map<SUIT_ViewWindow*, HEXABLOCK::GUI::DocumentGraphicView*> _documentView;

//   static std::map<HEXABLOCK::GUI::DocumentModel*,  SUIT_ViewWindow*> _salomeViews;

//   int _documentCnt;
  bool _isSaved;


  void testDocument();
  void test_make_cart_grid();
  void test_make_elmts_transform();
  

};

#endif
