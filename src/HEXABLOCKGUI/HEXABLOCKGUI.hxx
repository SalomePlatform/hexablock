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

#ifndef _HEXABLOCKGUI_HXX_
#define _HEXABLOCKGUI_HXX_

#include "HEXABLOCKGUI_Export.hxx"

#include <iostream>
#include <map>
#include <set>

#include <QTreeView>
#include <QModelIndex>
#include <QDockWidget>

#include "klinkitemselectionmodel.hxx"

#include <OB_Browser.h>
#include <SalomeApp_Module.h>
#include <SalomeApp_DataObject.h>

#include <LightApp_SelectionMgr.h>
#include <LightApp_DataModel.h>
#include <SOCC_Prs.h>
#include <SALOMEconfig.h>

#include "HEXABLOCKGUI_Resource.hxx"
#include "HEXABLOCKGUI_OCCSelector.h"
#include "hexa_base.hxx"

class QDir;

// #include CORBA_CLIENT_HEADER(HEXABLOCK_Gen)  // Abu Juin 2013

namespace HEXABLOCK
{
  namespace GUI
  {
    class VtkDocumentGraphicView;
    class OccGraphicView;
    class GraphicViewsHandler;
//    class DocumentDelegate;
    class DocumentModel;
    class PatternDataModel;
//    class PatternBuilderModel;
    class AssociationsModel;
    class GroupsModel;
    class MeshModel;
    class PatternDataSelectionModel;
//    class PatternBuilderSelectionModel;
    class GroupsSelectionModel;
    class MeshSelectionModel;

    class HexaBaseDialog;
    class VertexDialog;
    class EdgeDialog;
    class QuadDialog;
    class HexaDialog;
    class VectorDialog;
    class CylinderDialog;
    class PipeDialog;
    class MakeGridDialog;
    class MakeCylinderDialog;
    class MakePipeDialog;
    class MakeCylindersDialog;
    class MakePipesDialog;
    class RemoveHexaDialog;
    class PrismQuadDialog;
    class JoinQuadDialog;
    class MergeDialog;
    class DisconnectDialog;
    class CutEdgeDialog;
    class MakeTransformationDialog;
    class MakeSymmetryDialog;
    class PerformTransformationDialog;
    class PerformSymmetryDialog;
    typedef class MyBasicGUI_PointDlg VertexAssocDialog;
    class EdgeAssocDialog;
    class QuadAssocDialog;
    class GroupDialog;
    class LawDialog;
    class PropagationDialog;
    class ComputeMeshDialog;
    class ReplaceHexaDialog;
    class QuadRevolutionDialog;
    class MakeHemiSphereDialog;
    class ModelInfoDialog;
    class AddShapeDialog;
  }
}

class HEXABLOCKGUI_Resource;//SUIT_ResourceMgr;
class SalomeApp_Study;
class SalomeApp_Application;
class SUIT_ViewWindow;
class SVTK_ViewWindow;
class OCCViewer_ViewWindow;
class LightApp_VTKSelector;


class HEXABLOCK_EXPORT HEXABLOCKGUI : public SalomeApp_Module
{
  Q_OBJECT

  friend class HEXABLOCKGUI_Resource;

public:
  HEXABLOCKGUI();
  virtual ~HEXABLOCKGUI();

  enum ViewType {
      VTK,
      OCC,
      VTK_OCC,
      UNKNOWN
  };

  static SalomeApp_Study*         activeStudy();

  // static HEXABLOCK_ORB::HEXABLOCK_Gen_ptr InitHEXABLOCKGen( SalomeApp_Application* );
  static LightApp_SelectionMgr*   selectionMgr();
  static SALOMEDS::Study_var  ClientStudyToStudy (_PTR(Study) theStudy);
  static ViewType getActiveViewType();
  static HEXABLOCKGUI*  getInstance();

  //add an object in the study
  void addInStudy(QMap<QString, TopoDS_Shape>& topo_shapes,
                         QMap<QString, QString>& docShapesEntry,
                         QMap<QString, QString>& docShapesName);

  static HEXABLOCK::GUI::VtkDocumentGraphicView* currentDocGView;
  static HEXABLOCK::GUI::OccGraphicView* currentOccGView;
  static bool assocInProgress;
  static SalomeApp_Application* myApplication;

  virtual void initialize( CAM_Application* app);

  virtual bool renameAllowed( const QString& ) const;
  virtual bool renameObject( const QString& entry, const QString& name);
  virtual void windows( QMap<int, int>& theMap) const;
  virtual QString  engineIOR() const;

  virtual void viewManagers(QStringList& list) const;
  virtual void setResource(SUIT_ResourceMgr* r);
  virtual void createPreferences();
  virtual void preferencesChanged( const QString& sect, const QString& name );
  virtual void studyActivated();

  ///Returns a new HEXA_NS::Document and it's entry in the study
  std::pair <QString, HEXA_NS::Document*> newHexaDocument();

  //------------------------------------
  void createAndFillDockWidget();
  void createActions();
  void createMenus();
  void createTools();

  void initialMenus();
  void showAllMenus();
  void setAllMenusEnabled(bool enable);
  void updateSelectors();

  void switchModel(HEXABLOCK::GUI::VtkDocumentGraphicView* dgview);
  void switchOnGraphicView(HEXABLOCK::GUI::VtkDocumentGraphicView* dgview);
  void switchOffGraphicView(HEXABLOCK::GUI::VtkDocumentGraphicView* dgview,
                              bool saveCurrentDlg = true);
  void showDockWidgets(bool isVisible);

  HEXABLOCK::GUI::VtkDocumentGraphicView* getCurrentVtkGraphicView() const { return currentDocGView; }
  HEXABLOCK::GUI::VtkDocumentGraphicView* getOrCreateDocument(SalomeApp_DataObject* studyObject);
  HEXABLOCK::GUI::VtkDocumentGraphicView* getDocGViewByEntry(const QString& entry) const
  {
      return ( (docs.contains(entry) ? docs[entry] : NULL ) );
  }
  HEXABLOCK::GUI::DocumentModel* getCurrentModel();
  HEXABLOCK::GUI::HexaBaseDialog* getDlgBox(HEXABLOCK::GUI::VtkDocumentGraphicView* view);
  QTreeView* getPatternDataTreeView() const { return _patternDataTreeView; }
  QTreeView* getPatternGeomTreeView() const { return _patternGeomTreeView; }

  void showVertexInfoDialog(HEXA_NS::Vertex* vertex);
  void showEdgeInfoDialog(HEXA_NS::Edge* edge);
  void showQuadInfoDialog(HEXA_NS::Quad* quad);
  void showHexaInfoDialog(HEXA_NS::Hexa* hexa);
  void showVectorInfoDialog(HEXA_NS::Vector* vector);
  void showGroupInfoDialog(HEXA_NS::Group* group);
  void showLawInfoDialog(HEXA_NS::Law* law);
  void showPropagationInfoDialog(HEXA_NS::Propagation* propagation);

  void showVtkActor();
  void showOccActor();
  void hideVtkActor();
  void hideOccActor();

public slots:
  bool deactivateModule( SUIT_Study* theStudy);
  bool activateModule( SUIT_Study* theStudy);
  void onObjectBrowserClick(const QModelIndex& index);

  void showDocumentMenus(bool show);
  void showPatternMenus(bool show);
  void showAssociationMenus(bool show);
  void showGroupsMenus(bool show);
  void showMeshMenus(bool show);

  void enableDocumentMenus(bool enable);
  void enablePatternMenus(bool enable);
  void enableAssociationMenus(bool enable);
  void enableGroupsMenus(bool enable);
  void enableMeshMenus(bool enable);

protected slots:
  void onWindowActivated( SUIT_ViewWindow* svw);
  void onWindowClosed( SUIT_ViewWindow* svw);

  virtual void                        onViewManagerAdded( SUIT_ViewManager* );
  virtual void                        onViewManagerRemoved( SUIT_ViewManager* );

  void onSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected );
  void hideActor();
  void showOnlyActor();

protected:
  bool createSComponent();
  virtual bool eventFilter(QObject *obj, QEvent *event);

  void _showDialogBox( HEXABLOCK::GUI::HexaBaseDialog* diag );
  HEXABLOCK::GUI::HexaBaseDialog* currentDialog;

  SOCC_Prs* lastOccPrs;
  HEXABLOCK::GUI::VtkDocumentGraphicView* lastVtkDocGView;

  HEXABLOCKGUI_Resource* _myresource;
  static int _oldStudyId;
  // static HEXABLOCK_ORB::HEXABLOCK_Gen_var _hexaEngine;  // Hexa6 TODo
private slots:

  void slot_modelChanged(const QModelIndex &topLeft, const QModelIndex  &bottomRight);

  void treeContextMenu(const QPoint& aPosition);

  void test_association();
  void test();

  void newDocument();
  void loadDocument( const QString &path = QString() );
  void saveDocument();

  // new
  void addVertex();
  void addEdge();
  void addQuad();
  void addHexa();

  void addVector();
//  void addCylinder();
//  void addPipe();
  void makeGrid();
  void makeCylinder();
  void makePipe();
  void makeCylinders();
  void makePipes();

  void makeHemiSphere();  // NEW HEXA3

  void removeHexa();
  void prismQuad();
  void joinQuad();
  void merge();
  void disconnectElts();
  void cutEdge();
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

  //show the current model's information (nb vertex, edge, ... and more)
  void showModelInfo();

  //associate a shape to a document
  void addShape();

  //Clears associations of a single element (VERTEX, EDGE, QUAD, ...)
  void clearAssociations();

  //Clears associations under a tree (VERTEX_TREE, EDGE_TREE, ...)
  void clearAllAssociations();

  void showAssociations();

private:

  QList<HEXABLOCKGUI_OCCSelector*>  myOCCSelectors;
  QList<LightApp_VTKSelector*> myVTKSelectors;


  QStringList getQuickDirList();
  void clearDialogs();

  SOCC_Prs* getOccPrs(HEXABLOCK::GUI::VtkDocumentGraphicView* docGView)
  {
      if (docGView == NULL)
          return NULL;
      if (!occPrs.contains(docGView))
          occPrs[docGView] = new SOCC_Prs(0);
      return occPrs[docGView];
  }

  // -------------------------------------------------------------------------------------------------
  //          MainWindow presentation
  // -------------------------------------------------------------------------------------------------
  QDockWidget *_dwPattern;       // Hexablock model edition
  QDockWidget *_dwAssociation;   // Hexablock association edition
  QDockWidget *_dwGroups;        // Hexablock groups edition
  QDockWidget *_dwMesh;          // Hexablock meshing edtion
  QDockWidget *_dwObjectBrowser; // Salome study
  QDockWidget *_dwInputPanel;    // user Input


  // ** Elements creation and modification Dialogs **

  // Dialog Box ( to keep persistent values )
  HEXABLOCK::GUI::VertexDialog*                 _vertexDiag;
  HEXABLOCK::GUI::EdgeDialog*                   _edgeDiag;
  HEXABLOCK::GUI::QuadDialog*                   _quadDiag;
  HEXABLOCK::GUI::HexaDialog*                   _hexaDiag;
  HEXABLOCK::GUI::VectorDialog*                 _vectorDiag;
  HEXABLOCK::GUI::CylinderDialog*               _cylinderDiag;
  HEXABLOCK::GUI::PipeDialog*                   _pipeDiag;
  HEXABLOCK::GUI::MakeGridDialog*               _makeGridDiag;
  HEXABLOCK::GUI::MakeCylinderDialog*           _makeCylinderDiag;
  HEXABLOCK::GUI::MakePipeDialog*               _makePipeDiag;
  HEXABLOCK::GUI::MakeCylindersDialog*          _makeCylindersDiag;
  HEXABLOCK::GUI::MakePipesDialog*              _makePipesDiag;
  HEXABLOCK::GUI::RemoveHexaDialog*             _removeHexaDiag;
  HEXABLOCK::GUI::PrismQuadDialog*              _prismQuadDiag;
  HEXABLOCK::GUI::JoinQuadDialog*               _joinQuadDiag;
  HEXABLOCK::GUI::MergeDialog*                  _mergeDiag;
  HEXABLOCK::GUI::DisconnectDialog*             _disconnectDiag;
  HEXABLOCK::GUI::CutEdgeDialog*                _cutEdgeDiag;
  HEXABLOCK::GUI::MakeTransformationDialog*     _makeTransformationDiag;
  HEXABLOCK::GUI::MakeSymmetryDialog*           _makeSymmetryDiag;
  HEXABLOCK::GUI::PerformTransformationDialog*  _performTransformationDiag;
  HEXABLOCK::GUI::PerformSymmetryDialog*        _performSymmetryDiag;
  HEXABLOCK::GUI::VertexAssocDialog*            _vertexAssocDiag;
  HEXABLOCK::GUI::EdgeAssocDialog*              _edgeAssocDiag;
  HEXABLOCK::GUI::QuadAssocDialog*              _quadAssocDiag;
  HEXABLOCK::GUI::GroupDialog*                  _groupDiag;
  HEXABLOCK::GUI::LawDialog*                    _lawDiag;
  HEXABLOCK::GUI::PropagationDialog*            _propagationDiag;
  HEXABLOCK::GUI::ComputeMeshDialog*            _computeMeshDiag;
  HEXABLOCK::GUI::ReplaceHexaDialog*            _replaceHexaDiag;
  HEXABLOCK::GUI::QuadRevolutionDialog*         _quadRevolutionDiag;
  HEXABLOCK::GUI::MakeHemiSphereDialog*         _makeHemiSphereDiag;
  HEXABLOCK::GUI::ModelInfoDialog*              _modelInfoDiag;
  HEXABLOCK::GUI::AddShapeDialog*               _addShapeDiag;

  // ** Elements information dialogs **
  HEXABLOCK::GUI::VertexDialog*                 _vertexInfoDialog;
  HEXABLOCK::GUI::EdgeDialog*                   _edgeInfoDialog;
  HEXABLOCK::GUI::QuadDialog*                   _quadInfoDialog;
  HEXABLOCK::GUI::HexaDialog*                   _hexaInfoDialog;
  HEXABLOCK::GUI::VectorDialog*                 _vectorInfoDialog;
  HEXABLOCK::GUI::GroupDialog*                  _groupInfoDialog;
  HEXABLOCK::GUI::LawDialog*                    _lawInfoDialog;
  HEXABLOCK::GUI::PropagationDialog*            _propagationInfoDialog;


  std::set<HEXABLOCK::GUI::HexaBaseDialog*> currentModelDialogs;
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
//  QAction *_addCylinder;
//  QAction *_addPipe;
  QAction *_makeGrid; //Cartesian, Cylindrical, Spherical
  QAction *_makeCylinder;
  QAction *_makePipe;
  QAction *_makeCylinders;
  QAction *_makePipes;

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
  QAction* _showModelInfoAct;
  QAction* _addShapeAct;


  QAction* _sep1; // tmonin
  QAction* _sep2; // tmonin
  QAction* _sep3; // tmonin
  QAction* _sep4; // ...... tparti

  // -------------------------------------------------------------------------------------------------
  //          Model/View implementation
  // -------------------------------------------------------------------------------------------------

  HEXABLOCK::GUI::AssociationsModel   *_associationsModel;    // sub-part of DocumentModel

  //      VIEW      VIEW      VIEW      VIEW      VIEW      VIEW      VIEW      VIEW      VIEW      VIEW
  QTreeView* _patternDataTreeView;    //  document's pattern : 1 ( only one view )
//  QTreeView* _patternBuilderTreeView; //  document's pattern : 1 ( only one view )
  QTreeView* _patternGeomTreeView; // the geometries' tree view
  QTreeView* _associationTreeView;    //  document's association : 1 ( only one view )
  QTreeView* _groupsTreeView; //  document's groups
  QTreeView* _meshTreeView;   //  document's mesh property: 1 ( only one view )

  //      DELEGATE      DELEGATE      DELEGATE      DELEGATE      DELEGATE      DELEGATE      DELEGATE
//  HEXABLOCK::GUI::DocumentDelegate    *_treeViewDelegate;  // specific editor for each item of the tree

  HEXABLOCK::GUI::GraphicViewsHandler*  graphicViewsHandler; //vtk views hanlder (create, close, ...)

  //  SALOME   SALOME    SALOME   SALOME     SALOME     SALOME     SALOME     SALOME     SALOME     SALOME
  QMap<QString, HEXABLOCK::GUI::VtkDocumentGraphicView*>  docs; //  key = entry
  QMap<HEXABLOCK::GUI::VtkDocumentGraphicView*, HEXABLOCK::GUI::HexaBaseDialog*>  gViewDlgBox; //  key = entry
  QMap<HEXABLOCK::GUI::VtkDocumentGraphicView*, SOCC_Prs*> occPrs;

  bool _isSaved;
  bool moduleActivatedOnce;
  bool menusEnabled;

  QDir* loadDocLastPath;
  QDir* saveDocLastPath;

  void testDocument();
  void test_make_cart_grid();
  void test_make_elmts_transform();

  QString       addDocInStudy (HEXA_NS::Document* doc);  // Hexa6
  HEXA_NS::Hex* hexa_root;                               // Hexa6
};

#endif
