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
#ifndef __HEXABLOCKGUI_DOCUMENTPANEL_HXX_
#define __HEXABLOCKGUI_DOCUMENTPANEL_HXX_

#include "HEXABLOCKGUI_Export.hxx"

#define MAX_WIDTH 16777215
#define MAX_HEIGHT 16777215
#define MIN_WIDTH 5
#define MIN_HEIGHT 0

#include <QShortcut>
#include <QDockWidget>

#include "ui_Vertex_QTD.h"
#include "ui_Quad_QTD.h"
#include "ui_Edge_QTD.h"
#include "ui_Hexa_QTD.h"

#include "ui_Vector_QTD.h"
#include "ui_Cylinder_QTD.h"
#include "ui_Pipe_QTD.h"
#include "ui_MakeGrid_QTD.h"
#include "ui_MakePipe_QTD.h"
#include "ui_MakeCylinder_QTD.h"
#include "ui_MakeCylinders_QTD.h"
#include "ui_MakePipes_QTD.h"
#include "ui_RemoveHexa_QTD.h"
#include "ui_PrismQuad_QTD.h"
#include "ui_JoinQuad_QTD.h"
#include "ui_Merge_QTD.h"
#include "ui_Disconnect_QTD.h"
#include "ui_CutEdge_QTD.h"
#include "ui_Transformation_QTD.h"
#include "ui_Symmetry_QTD.h"

#include "ui_EdgeAssoc_QTD.h"
#include "ui_QuadAssoc_QTD.h"

#include "ui_Group_QTD.h"
#include "ui_Law_QTD.h"
#include "ui_Propagation_QTD.h"

#include "ui_QuadRevolution_QTD.h"
#include "ui_MakeRind_QTD.h"
#include "ui_ReplaceHexa_QTD.h"
#include "ui_MakeHemiSphere_QTD.h"
#include "ui_ModelInfo_QTD.h"
#include "ui_AddShapeDialog_QTD.h"
#include "ui_ComputeMesh_QTD.h"

#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexQuad.hxx"
#include "HexHexa.hxx"


#include "HEXABLOCKGUI_DocumentSelectionModel.hxx"
#include "HEXABLOCKGUI_DocumentModel.hxx"
#include "HEXABLOCKGUI_VtkDocumentGraphicView.hxx"
#include "HEXABLOCKGUI.hxx"

#include "klinkitemselectionmodel.hxx"

Q_DECLARE_METATYPE(QModelIndex);

namespace HEXABLOCK
{
  namespace GUI
  {
    class HEXABLOCK_EXPORT HexaBaseDialog : public QDialog
    {
      Q_OBJECT

    public:
      enum Mode {
        NEW_MODE,
        INFO_MODE,
        UPDATE_MODE
      };

      // define input widget type => usefull for selection
      typedef HexaTreeRole  HexaWidgetType;
      typedef TopAbs_ShapeEnum GeomWidgetType; //CS_TODO

      enum {
            LW_QMODELINDEX_ROLE = Qt::UserRole + 1,
            LW_ASSOC_ROLE,
            LW_GEOM_OBJ_ROLE,
            LW_DATA_ROLE
      };

      HexaBaseDialog( QWidget * parent = 0, Mode editmode = NEW_MODE, Qt::WindowFlags f = 0 );
      virtual ~HexaBaseDialog(){};

      // clear all input widget
      virtual void clear(){};
      void clearWidgetsIndexes() { _index.clear(); }

      void connectDocumentGraphicView(VtkDocumentGraphicView* docGView = NULL);
      void disconnectDocumentGraphicView(VtkDocumentGraphicView* docGView = NULL);

      SVTK_ViewWindow* getCurrentVtkView() const {
          return HEXABLOCKGUI::currentDocGView->getViewWindow();
      }

      //get model
      DocumentModel* getDocumentModel() const {
         return HEXABLOCKGUI::currentDocGView->getDocumentModel();
      }

      PatternDataModel* getPatternDataModel() const {
          return HEXABLOCKGUI::currentDocGView->getPatternDataModel();
      }

      PatternGeomModel* getPatternGeomModel() const {
          return HEXABLOCKGUI::currentDocGView->getPatternGeomModel();
      }

      GroupsModel* getGroupsModel() const {
          return HEXABLOCKGUI::currentDocGView->getGroupsModel();
      }

      MeshModel* getMeshModel() const {
          return HEXABLOCKGUI::currentDocGView->getMeshModel();
      }

      //get selection patterns from the current graphic view
      PatternDataSelectionModel* getPatternDataSelectionModel() const {
         return HEXABLOCKGUI::currentDocGView->getPatternDataSelectionModel();
      }

      PatternGeomSelectionModel* getPatternGeomSelectionModel() const {
          return HEXABLOCKGUI::currentDocGView->getPatternGeomSelectionModel();
      }

      GroupsSelectionModel* getGroupsSelectionModel() const {
         return HEXABLOCKGUI::currentDocGView->getGroupsSelectionModel();
      }

      MeshSelectionModel* getMeshSelectionModel() const {
         return HEXABLOCKGUI::currentDocGView->getMeshSelectionModel();
      }

      QString getErrorMsg();

      HEXABLOCKGUI::ViewType getObjectViewType(QObject* obj);
      void modelUnregister(QWidget* widget); //unlink the widget from the model
      void resetSizeAndShow(QDockWidget* parent);
      virtual QModelIndexList getAssocsVTK()
      {
        QModelIndexList assocs;
        return assocs;
      }
      virtual QMultiMap<QString, int> getAssocsGEOM()
      {
        QMultiMap<QString, int> assocs;
        return assocs;
      }
      virtual void setCurrentGeomObj(DocumentModel::GeomObj* geomObj) {}
      virtual void computeAndSetDimension(const QModelIndex& elt);

      bool debugEdgeAssoc; //Temporary
      bool autoFocusSwitch;

    public slots:
    virtual bool apply();
    virtual void close();
    virtual void onHelpRequested();
    void highlightSelectedAssocs();
    void refreshHighlight();
    virtual void onWindowActivated(SUIT_ViewManager*){};
    void clearVTKSelection();
    void clearOCCSelection();
    void clearCurrentObjectFocus();

    protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);
    virtual void hideEvent ( QHideEvent * event );
    virtual void showEvent ( QShowEvent * event );
    virtual bool apply(QModelIndex& result) = 0;

    //Gives a default name to an element according to the last created
    void updateDefaultName(QLineEdit* name_field, HEXA_NS::EnumElt type);
    DocumentModel::GeomObj* getGeomObj(const QModelIndex& index);

    virtual void _initInputWidget( Mode editmode )=0; //must be implemented on inherited dialog box
    virtual QGroupBox*  _initButtonBox( Mode editmode );
    void _initWidget( Mode editmode ); // call _initInputWidget() & _initButtonBox()
    QModelIndexList getIndexList(QListWidget* itemsList, bool mapToSource = true);

    void _allowSelection();
    void _disallowSelection();
    QItemSelectionModel* _allowSelection( HexaWidgetType wtype );

    bool _allowVTKSelection( QObject* obj );
    bool _allowOCCSelection( QObject* obj );
    QItemSelectionModel* _getSelector( QObject* obj );

    void _selectAndHighlight( const QModelIndex& i );

    void _highlightWidget(QObject* obj, Qt::GlobalColor clr);

    void setFocusToNextField();

    void _updateCurrentObject(QObject*);

    bool isDimensionType(const QObject* obj);

    Mode _editMode;

    QMap<QObject*, QModelIndex>    _index;
    QObject*                       _currentObj;
    QMap<HexaWidgetType, QString>  _strHexaWidgetType;
    QPushButton*                   _applyButton;
    QString                        _helpFileName;

    private:
    bool _isLineOrListWidget(QObject*);

    protected slots:
    virtual void onSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel ); //from qt model/view selectionManager
    virtual void onCurrentSelectionChanged();//from salome selectionManager
    bool _onSelectionChanged( const QItemSelection& sel, QLineEdit*   le );
    bool _onSelectionChanged( const QItemSelection& sel, QListWidget* lw );
    virtual void updateButtonBox();
    void updateName();
    virtual void selectElementOfModel();

    };

    class HEXABLOCK_EXPORT VertexDialog : public HexaBaseDialog,
    public Ui::VertexDialog
    {
      Q_OBJECT

    public:
      VertexDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~VertexDialog();

      void clear();

      void setValue(HEXA_NS::Vertex* v);
      HEXA_NS::Vertex* getValue();

      void setIndex(const QModelIndex& i);

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );

    private:
    HEXA_NS::Vertex     *_value;
    QModelIndex         _ivalue;
    };


    class HEXABLOCK_EXPORT EdgeDialog : public HexaBaseDialog,
    public Ui::EdgeDialog
    {
      Q_OBJECT

    public:
      EdgeDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~EdgeDialog();

      void clear();

      void setValue(HEXA_NS::Edge* e);
      HEXA_NS::Edge* getValue();

    protected:
      void _initInputWidget( Mode editmode );

    public slots:
    virtual bool apply(QModelIndex& result);

    private:
    HEXA_NS::Edge *_value;
    QModelIndex   _ivalue;
    };


    class HEXABLOCK_EXPORT QuadDialog : public HexaBaseDialog,
    public Ui::QuadDialog
    {
      Q_OBJECT

    public:
      QuadDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~QuadDialog();

      void clear();

      void setValue(HEXA_NS::Quad* v);
      HEXA_NS::Quad* getValue();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );

    private:
    HEXA_NS::Quad    *_value;
    QModelIndex      _ivalue;
    };



    class HEXABLOCK_EXPORT HexaDialog : public HexaBaseDialog,
    public Ui::HexaDialog
    {
      Q_OBJECT

    public:
      HexaDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~HexaDialog();

      void clear();
      virtual QModelIndexList getAssocsVTK();

      void setValue(HEXA_NS::Hexa* v);
      HEXA_NS::Hexa* getValue();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );

    protected slots:
    void deleteQuadItem();
    void deleteVertexItem();
    void updateButtonBox();

    private:
    void _setValueQuads( HEXA_NS::Hexa* h );
    void _setValueVertices( HEXA_NS::Hexa* h );

    HEXA_NS::Hexa   *_value;
    QModelIndex     _ivalue;
    };


    class HEXABLOCK_EXPORT VectorDialog : public HexaBaseDialog,
    public Ui::VectorDialog
    {
      Q_OBJECT

    public:
      VectorDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~VectorDialog();

      void clear();

      void setValue(HEXA_NS::Vector* v);
      HEXA_NS::Vector* getValue();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );

    private:
    HEXA_NS::Vector *_value;
    QModelIndex     _ivalue;
    };

    class HEXABLOCK_EXPORT MakeGridDialog : public HexaBaseDialog,
    public Ui::MakeGridDialog
    {
      Q_OBJECT

    public:
      MakeGridDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~MakeGridDialog();

      void clear();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );

    protected slots:
    void addRadiusItem();
    void delRadiusItem();
    void addAngleItem();
    void delAngleItem();
    void addHeightItem();
    void delHeightItem();
    void updateHelpFileName();

    };


    class HEXABLOCK_EXPORT MakeCylinderDialog : public HexaBaseDialog,
    public Ui::MakeCylinderDialog
    {
      Q_OBJECT

    public:
      MakeCylinderDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~MakeCylinderDialog();

      void clear();

    protected:
      void _initInputWidget( Mode editmode );

    protected slots:
      void addRadiusItem();
      void delRadiusItem();
      void addAngleItem();
      void delAngleItem();
      void addHeightItem();
      void delHeightItem();
      void updateHelpFileName();

    public slots:
    virtual bool apply(QModelIndex& result);
    };


    class HEXABLOCK_EXPORT MakePipeDialog : public HexaBaseDialog,
    public Ui::MakePipeDialog
    {
      Q_OBJECT

    public:
      MakePipeDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~MakePipeDialog();

      void clear();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );

    protected slots:
    void addRadiusItem();
    void delRadiusItem();
    void addAngleItem();
    void delAngleItem();
    void addHeightItem();
    void delHeightItem();
    void updateHelpFileName();
    };


    class HEXABLOCK_EXPORT MakeCylindersDialog : public HexaBaseDialog,
    public Ui::MakeCylindersDialog
    {
      Q_OBJECT

    public:
      MakeCylindersDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~MakeCylindersDialog();

      void clear();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );
    };


    class HEXABLOCK_EXPORT MakePipesDialog : public HexaBaseDialog,
    public Ui::MakePipesDialog
    {
      Q_OBJECT

    public:
      MakePipesDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~MakePipesDialog();

      void clear();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );
    };



    class HEXABLOCK_EXPORT RemoveHexaDialog : public HexaBaseDialog,
    public Ui::RemoveHexaDialog
    {
      Q_OBJECT

    public:
      RemoveHexaDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~RemoveHexaDialog();

      void clear();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );
    };


    class HEXABLOCK_EXPORT PrismQuadDialog : public HexaBaseDialog,
    public Ui::PrismQuadDialog
    {
      Q_OBJECT

    public:
      PrismQuadDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~PrismQuadDialog();
      void clear();
      virtual QModelIndexList getAssocsVTK();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );

    protected slots:
    void addHeightItem();
    void delHeightItem();
    void updateHelpFileName();

    private slots:
    //void addQuad();
    void removeQuad();
    //void clearQuads();

    };

    class HEXABLOCK_EXPORT JoinQuadDialog : public HexaBaseDialog,
    public Ui::JoinQuadDialog
    {
      Q_OBJECT

    public:
      JoinQuadDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~JoinQuadDialog();

      void clear();
      virtual QModelIndexList getAssocsVTK();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );

    protected slots:
    void addHeightItem();
    void delHeightItem();
    void updateHelpFileName();

    private slots:
    //void addQuad();
    void removeQuad();
    //void clearQuads();

    };


    class HEXABLOCK_EXPORT MergeDialog : public HexaBaseDialog,
    public Ui::MergeDialog
    {
      Q_OBJECT

    public:
      MergeDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~MergeDialog();

      void clear();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );

    protected slots:
    void updateHelpFileName();


    };


    class HEXABLOCK_EXPORT DisconnectDialog : public HexaBaseDialog,
    public Ui::DisconnectDialog
    {
      Q_OBJECT

    public:
      DisconnectDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~DisconnectDialog();

      void clear();
      virtual QModelIndexList getAssocsVTK();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );

    protected slots:
    void updateHelpFileName();
    void deleteEdgeItem();
    void deleteHexaItem();
    };



    class HEXABLOCK_EXPORT CutEdgeDialog : public HexaBaseDialog,
    public Ui::CutEdgeDialog
    {
      Q_OBJECT

    public:
      CutEdgeDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~CutEdgeDialog();

      void clear();

    protected slots:
    void addHeightItem();
    void delHeightItem();
    void updateHelpFileName();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );
    };


    class HEXABLOCK_EXPORT MakeTransformationDialog : public HexaBaseDialog,
    public Ui::TransformationDialog
    {
      Q_OBJECT

    public:
      MakeTransformationDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~MakeTransformationDialog ();

      void clear();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );

    protected slots:
    void updateHelpFileName();
    };


    class HEXABLOCK_EXPORT MakeSymmetryDialog : public HexaBaseDialog,
    public Ui::SymmetryDialog
    {
      Q_OBJECT

    public:
      MakeSymmetryDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~MakeSymmetryDialog();

      void clear();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );

    protected slots:
    void updateHelpFileName();
    };




    class HEXABLOCK_EXPORT PerformTransformationDialog : public HexaBaseDialog,
    public Ui::TransformationDialog
    {
      Q_OBJECT

    public:
      PerformTransformationDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~PerformTransformationDialog ();

      void clear();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );

    protected slots:
    void updateHelpFileName();
    };


    class HEXABLOCK_EXPORT PerformSymmetryDialog : public HexaBaseDialog,
    public Ui::SymmetryDialog
    {
      Q_OBJECT

    public:
      PerformSymmetryDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~PerformSymmetryDialog();

      void clear();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );

    protected slots:
    void updateHelpFileName();
    };

class HEXABLOCK_EXPORT EdgeAssocDialog : public HexaBaseDialog,
    public Ui::EdgeAssocDialog
    {
      Q_OBJECT

    public:
      EdgeAssocDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~EdgeAssocDialog();
      void clear();
      virtual QModelIndexList getAssocsVTK();
      virtual QMultiMap<QString, int> getAssocsGEOM();
      virtual void setCurrentGeomObj(DocumentModel::GeomObj* geomObj) { myLine = geomObj; }

    public slots:
    virtual bool apply(QModelIndex& result);
    virtual void close();
    virtual void onWindowActivated(SUIT_ViewManager*);

    protected:
    virtual void _initInputWidget( Mode editmode );

    protected slots:
    virtual void selectElementOfGeom();
    void updateHelpFileName();

    void deleteEdgeItem(){
        delete edges_lw->currentItem();
    }
    void deleteLineItem(){
        delete lines_lw->currentItem();
    };

    private:
    DocumentModel::GeomObj* myLine;

    };


    class HEXABLOCK_EXPORT QuadAssocDialog : public HexaBaseDialog,
    public Ui::QuadAssocDialog
    {
      Q_OBJECT

    public:
      QuadAssocDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~QuadAssocDialog();
      void clear();
      virtual QModelIndexList getAssocsVTK();
      virtual QMultiMap<QString, int> getAssocsGEOM();

    public slots:
    virtual bool apply(QModelIndex& result);
    virtual void close();
    virtual void onWindowActivated(SUIT_ViewManager*);

    protected:
    virtual void _initInputWidget( Mode editmode );

    protected slots:
    void deleteFaceItem();
    virtual void selectElementOfGeom();

    private:
    QList<DocumentModel::GeomObj> _assocs;
    QShortcut*                    _delFaceShortcut;
    };



    class HEXABLOCK_EXPORT GroupDialog : public HexaBaseDialog,
    public Ui::GroupDialog
    {
      Q_OBJECT

      QMap<DocumentModel::Group, QString> strKind;

    public:
      GroupDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~GroupDialog();
      void clear();
      virtual QModelIndexList getAssocsVTK();

      void setValue(HEXA_NS::Group* v);
      HEXA_NS::Group* getValue();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );

    private slots:
    void removeEltBase();
    void onKindChanged( int index );

    private:
    HEXA_NS::Group   *_value;
    };



    class HEXABLOCK_EXPORT LawDialog : public HexaBaseDialog,
    public Ui::LawDialog
    {
      Q_OBJECT

      QMap<DocumentModel::KindLaw, QString> strKind;

    public:
      LawDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~LawDialog();
      void clear();

      void setValue(HEXA_NS::Law* v);
      HEXA_NS::Law* getValue();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );

    private:
    HEXA_NS::Law   *_value;
    };




    class HEXABLOCK_EXPORT PropagationDialog : public HexaBaseDialog,
    public Ui::PropagationDialog
    {
      Q_OBJECT

    public:
      PropagationDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~PropagationDialog();
      void clear();

      void setValue(HEXA_NS::Propagation* v);
      HEXA_NS::Propagation* getValue();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );

    protected slots:
    //void updateHelpFileName();
    void deletePropagationItem();
    virtual void selectElementOfModel();

    private:
    HEXA_NS::Propagation *_value;
    };



    // Define the compute mesh dialog box
    // ----------------------------------

    class HEXABLOCK_EXPORT ComputeMeshDialog : public HexaBaseDialog,
    public Ui::ComputeMeshDialog
    {
      Q_OBJECT

    public:
      ComputeMeshDialog( QWidget* parent = 0, Mode editmode = NEW_MODE, Qt::WindowFlags f = Qt::SubWindow );
      virtual ~ComputeMeshDialog();
      void clear();

      void setDocumentModel(DocumentModel* m);

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );
    };



    class HEXABLOCK_EXPORT ReplaceHexaDialog : public HexaBaseDialog,
    public Ui::ReplaceHexaDialog
    {
      Q_OBJECT

    public:
      ReplaceHexaDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~ReplaceHexaDialog();
      void clear();
      virtual QModelIndexList getAssocsVTK();


    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );

    protected slots:
    void deleteQuadItem();
    void deleteQuadItem2();
    void updateButtonBox();
    //void updateHelpFileName();

    private:
    HEXA_NS::Elements *_value;
    };





    class HEXABLOCK_EXPORT QuadRevolutionDialog : public HexaBaseDialog,
    public Ui::QuadRevolutionDialog
    {
      Q_OBJECT

    public:
      QuadRevolutionDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~QuadRevolutionDialog();
      void clear();
      virtual QModelIndexList getAssocsVTK();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );

    protected slots:
    void addAngleItem();
    void delAngleItem();
    void delQuadItem();
    void updateHelpFileName();

    private:
    HEXA_NS::Elements *_value;
    };



    class HEXABLOCK_EXPORT MakeHemiSphereDialog : public HexaBaseDialog,
    public Ui::MakeHemiSphereDialog
    {
      Q_OBJECT

    public:
      MakeHemiSphereDialog( QWidget* = 0, Mode = NEW_MODE, Qt::WindowFlags = Qt::SubWindow );//= 0 );
      virtual ~MakeHemiSphereDialog();
      void clear();

    public slots:
    virtual bool apply(QModelIndex& result);

    protected:
    void _initInputWidget( Mode editmode );

    protected slots:
    void addRadiusItem1();
    void delRadiusItem1();
    void addRadiusItem2();
    void delRadiusItem2();
    void addAngleItem();
    void delAngleItem();
    void addHeightItem();
    void delHeightItem();
    void updateHelpFileName();

    private:
    HEXA_NS::Elements *_value;
    };

    class HEXABLOCK_EXPORT ModelInfoDialog : public HexaBaseDialog,
    public Ui::ModelInfoDialog
    {
        Q_OBJECT

    public:
        ModelInfoDialog( QWidget* parent = NULL, Qt::WindowFlags wf = Qt::SubWindow );
        virtual ~ModelInfoDialog(){}

    public slots:
        void updateInfo();

    protected:
        virtual void hideEvent ( QHideEvent * event );
        virtual void showEvent ( QShowEvent * event );
        virtual bool apply(QModelIndex& result){ return true;}
        virtual void _initInputWidget( Mode editmode ){}
    };

    class HEXABLOCK_EXPORT AddShapeDialog : public HexaBaseDialog,
    public Ui::AddShapeDialog
    {
        Q_OBJECT

    public:
        AddShapeDialog( QWidget* parent = NULL, Mode editmode = NEW_MODE, Qt::WindowFlags wf = Qt::SubWindow );
        virtual ~AddShapeDialog();
        void clear();

    public slots:
        virtual bool apply(QModelIndex& result);
        virtual void close();
        virtual void onWindowActivated(SUIT_ViewManager*);

    protected:
        virtual void _initInputWidget( Mode editmode );

    protected slots:
        virtual void onCurrentSelectionChanged();
    };

  }
}

#endif
