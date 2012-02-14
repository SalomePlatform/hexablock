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
#ifndef __HEXABLOCKGUI_DOCUMENTPANEL_HXX_
#define __HEXABLOCKGUI_DOCUMENTPANEL_HXX_


#if defined WIN32
#  if defined HEXABLOCKGUI_DOCUMENTPANEL_EXPORT || defined HexablockGui_DocumentPanel_EXPORTS
#    define HEXABLOCKGUI_DOCUMENTPANEL_EXPORT __declspec( dllexport )
#  else
#    define HEXABLOCKGUI_DOCUMENTPANEL_EXPORT __declspec( dllimport )
#  endif
#else
#  define HEXABLOCKGUI_DOCUMENTPANEL_EXPORT
#endif



#include <QShortcut>

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



#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexQuad.hxx"
#include "HexHexa.hxx"
// #include <QItemDelegate>
// #include <QModelIndex>
// #include <QObject>
// #include <QSize>
// #include <QSpinBox>
// #include <QDockWidget>
// #include <QItemSelectionModel>

#include "HEXABLOCKGUI_DocumentSelectionModel.hxx"
#include "HEXABLOCKGUI_DocumentModel.hxx"
// #include "HEXABLOCKGUI_SalomeTools.hxx"

#include "klinkitemselectionmodel.hxx"

#include <GEOMBase_Helper.h>
#include "BasicGUI_PointDlg.h"

Q_DECLARE_METATYPE(QModelIndex); 
Q_DECLARE_METATYPE(HEXABLOCK::GUI::DocumentModel::GeomObj);

namespace HEXABLOCK
{
  namespace GUI
  {



    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT HexaBaseDialog : public QDialog
    {
        Q_OBJECT
        public:

          enum {
            LW_QMODELINDEX_ROLE = Qt::UserRole + 1,
            LW_ASSOC_ROLE
          };

          HexaBaseDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
          virtual ~HexaBaseDialog();

          // clear all input widget
          virtual void clear();

          //  model
          virtual void setDocumentModel( DocumentModel* m );

          //  selection
          virtual void setPatternDataSelectionModel( PatternDataSelectionModel* s );
          virtual void setPatternBuilderSelectionModel( PatternBuilderSelectionModel* s );
          virtual void setMeshSelectionModel( QItemSelectionModel* s );

//         signals:
//           void editingFinished ();

        protected:
          void initLineEdits();

          void _allowSelection();
          void _disallowSelection();

//           void _setAllSelection();
          void _setHexaSelectionOnly();
          void _setQuadSelectionOnly();
          void _setEdgeSelectionOnly();
          void _setVertexSelectionOnly();

          void _setElementsSelectionOnly();
          void _setVectorSelectionOnly();
          void _setCylinderSelectionOnly();
          void _setPipeSelectionOnly();

          void _setLawSelectionOnly();

        public slots:
          virtual bool apply() = 0;
          virtual void accept();
          virtual void reject();
          virtual void onHelpRequested();

        protected slots:
          virtual void onSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel );
          virtual void onPatternDataSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel );
          virtual void onPatternBuilderSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel );
          virtual void onMeshSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel );
          virtual void onItemSelectionChanged();

        protected :
//           void installEventFilter();
          QDialogButtonBox* _initButtonBox();
          virtual bool eventFilter(QObject *obj, QEvent *event);
//           HexaBaseModel          *_model;
//           DocumentSelectionModel *_selectionModel;


          DocumentModel*       _documentModel;
//           PatternDataModel*    _patternDataModel;
//           PatternBuilderModel* _patternBuilderModel;

          // user selection
          PatternDataSelectionModel*    _patternDataSelectionModel;
          PatternBuilderSelectionModel* _patternBuilderSelectionModel;
          QItemSelectionModel*          _groupsSelectionModel;
          QItemSelectionModel*          _meshSelectionModel;

          QMap<QObject*, QModelIndex>   _index;
          QObject*                      _currentObj;

          int                           _expectedSelection;
          bool                          _selectionMutex;

          QList<QLineEdit*>  _hexaLineEdits;
          QList<QLineEdit*>  _quadLineEdits;
          QList<QLineEdit*>  _edgeLineEdits;
          QList<QLineEdit*>  _vertexLineEdits;

          QList<QLineEdit*>  _vectorLineEdits;
          QList<QLineEdit*>  _cylinderLineEdits;
          QList<QLineEdit*>  _pipeLineEdits;
          QList<QLineEdit*>  _elementsLineEdits;

          QList<QLineEdit*>  _lawLineEdits;

          QPushButton* _applyCloseButton;
          QPushButton* _applyButton;

          QString  _helpFileName;

    };



    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT VertexDialog : public HexaBaseDialog,
					                   public Ui::VertexDialog
    {
      Q_OBJECT

      public:
        VertexDialog( QWidget* = 0, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~VertexDialog();

        void clear();

        void setValue(HEXA_NS::Vertex* v);
        HEXA_NS::Vertex* getValue();

        void setIndex(const QModelIndex& i);

      public slots:
        virtual bool apply();
//         void updateName();


      private:
        HEXA_NS::Vertex     *_value;
        QModelIndex         _ivalue;
    };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT EdgeDialog : public HexaBaseDialog,
                                                       public Ui::EdgeDialog
  {
      Q_OBJECT

      public:
        EdgeDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~EdgeDialog();

        void clear();

        void setValue(HEXA_NS::Edge* e);
        HEXA_NS::Edge* getValue();

      public slots:
        virtual bool apply();
        void updateName();

      private:
        HEXA_NS::Edge *_value;
        QModelIndex   _ivalue;
    };


    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT QuadDialog : public HexaBaseDialog,
					                 public Ui::QuadDialog
    {
      Q_OBJECT

      public:
        QuadDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~QuadDialog();

        void clear();

        void setValue(HEXA_NS::Quad* v);
        HEXA_NS::Quad* getValue();

      public slots:
        virtual bool apply();
        void updateName();

      private:
        HEXA_NS::Quad    *_value;
        QModelIndex      _ivalue;
    };



    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT HexaDialog : public HexaBaseDialog,
                                                         public Ui::HexaDialog
    {
      Q_OBJECT

      public:
        HexaDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~HexaDialog();

        void clear();

        void setValue(HEXA_NS::Hexa* v);
        HEXA_NS::Hexa* getValue();

      public slots:
        virtual bool apply();
        void updateName();
        void onSelectionChanged( const QItemSelection& sel, const QItemSelection& unsel );

      protected:
        bool eventFilter(QObject *obj, QEvent *event);

      protected slots:
        void deleteQuadItem();
        void deleteVertexItem();
        void updateButtonBox();

      private:
        void _fillQuads( HEXA_NS::Hexa* h );
        void _fillVertices( HEXA_NS::Hexa* h );

        HEXA_NS::Hexa   *_value;
        QModelIndex     _ivalue;
    };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT VectorDialog : public HexaBaseDialog,
                                                         public Ui::VectorDialog
  {
      Q_OBJECT

      public:
        VectorDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~VectorDialog();

        void clear();

        void setValue(HEXA_NS::Vector* v);
        HEXA_NS::Vector* getValue();

      public slots:
        virtual bool apply();
        void updateName();

      private:
        HEXA_NS::Vector *_value;
        QModelIndex     _ivalue;
  };



  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT CylinderDialog : public HexaBaseDialog,
                                                           public Ui::CylinderDialog
  {
      Q_OBJECT

      public:
        CylinderDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~CylinderDialog();

        void clear();

        void setValue( HEXA_NS::Cylinder* v );
        HEXA_NS::Cylinder* getValue();

      public slots:
        virtual bool apply();
        void updateName();

      private:
        HEXA_NS::Cylinder   *_value;
        QModelIndex         _ivalue;
    };



  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT PipeDialog : public HexaBaseDialog,
                                                       public Ui::PipeDialog
  {
      Q_OBJECT

      public:
        PipeDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~PipeDialog();

        void clear();

        void setValue( HEXA_NS::Pipe* p );
        HEXA_NS::Pipe* getValue();

      public slots:
        virtual bool apply();
        void updateName();

      private:
        HEXA_NS::Pipe   *_value;
        QModelIndex     _ivalue;

    };



  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MakeGridDialog : public HexaBaseDialog,
                                                           public Ui::MakeGridDialog
  {
      Q_OBJECT

      public:
        MakeGridDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MakeGridDialog();

        void clear();

      public slots:
        virtual bool apply();

      protected slots:
        void addRadiusItem();
        void delRadiusItem();
        void addAngleItem();
        void delAngleItem();
        void addHeightItem();
        void delHeightItem();
        void updateButtonBox();

      protected slots:
        void updateHelpFileName();

  };




  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MakeCylinderDialog : public HexaBaseDialog,
                                                           public Ui::MakeCylinderDialog
  {
      Q_OBJECT

      public:
        MakeCylinderDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MakeCylinderDialog();

        void clear();

      public slots:
        virtual bool apply();
  };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MakePipeDialog : public HexaBaseDialog,
                                                           public Ui::MakePipeDialog
  {
      Q_OBJECT

      public:
        MakePipeDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MakePipeDialog();

        void clear();

      public slots:
        virtual bool apply();
  };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MakeCylindersDialog : public HexaBaseDialog,
                                                                public Ui::MakeCylindersDialog 
  {
      Q_OBJECT

      public:
        MakeCylindersDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MakeCylindersDialog();

        void clear();

      public slots:
        virtual bool apply();
  };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MakePipesDialog : public HexaBaseDialog,
                                                            public Ui::MakePipesDialog
  {
      Q_OBJECT

      public:
        MakePipesDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MakePipesDialog();

        void clear();

      public slots:
        virtual bool apply();
  };



  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT RemoveHexaDialog : public HexaBaseDialog,
                                                             public Ui::RemoveHexaDialog
  {
      Q_OBJECT

      public:
        RemoveHexaDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~RemoveHexaDialog();

        void clear();

      public slots:
        virtual bool apply();
  };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT PrismQuadDialog : public HexaBaseDialog,
                                                            public Ui::PrismQuadDialog
  {
      Q_OBJECT

      public:
        PrismQuadDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~PrismQuadDialog();

        void clear();

      public slots:
        virtual bool apply();

      protected:
        virtual bool eventFilter(QObject *obj, QEvent *event);

      protected slots:
        virtual void onSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel );

      private slots:
        void addQuad();
        void removeQuad();
        void clearQuads();

  };

  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT JoinQuadDialog : public HexaBaseDialog,
                                                           public Ui::JoinQuadDialog
  {
      Q_OBJECT

      public:
        JoinQuadDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~JoinQuadDialog();

        void clear();

      public slots:
        virtual bool apply();

      protected:
        virtual bool eventFilter(QObject *obj, QEvent *event);
     
      protected slots:
        virtual void onSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel );

      private slots:
        void addQuad();
        void removeQuad();
        void clearQuads();

  };


    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MergeDialog : public HexaBaseDialog,
                                                          public Ui::MergeDialog
    {
      Q_OBJECT

      public:
        MergeDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MergeDialog();

        void clear();

      public slots:
        virtual bool apply();

      protected slots:
        void updateHelpFileName();

   };



  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT DisconnectDialog : public HexaBaseDialog,
                                                             public Ui::DisconnectDialog
  {
      Q_OBJECT

      public:
        DisconnectDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~DisconnectDialog();

        void clear();

      public slots:
        virtual bool apply();

      protected slots:
        void updateHelpFileName();
   };



  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT CutEdgeDialog : public HexaBaseDialog,
                                                          public Ui::CutEdgeDialog
  {
      Q_OBJECT

      public:
        CutEdgeDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~CutEdgeDialog();

        void clear();

      public slots:
        virtual bool apply();
   };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MakeTransformationDialog : public HexaBaseDialog,
                                                          public Ui::TransformationDialog
  {
      Q_OBJECT

      public:
        MakeTransformationDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MakeTransformationDialog ();

        void clear();

      public slots:
        virtual bool apply();

      protected slots:
        void updateHelpFileName();
   };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MakeSymmetryDialog : public HexaBaseDialog,
                                                               public Ui::SymmetryDialog
  {
      Q_OBJECT

      public:
        MakeSymmetryDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MakeSymmetryDialog();

        void clear();

      public slots:
        virtual bool apply();

      protected slots:
        void updateHelpFileName();
   };




  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT PerformTransformationDialog : public HexaBaseDialog,
                                                                        public Ui::TransformationDialog
  {
      Q_OBJECT

      public:
        PerformTransformationDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~PerformTransformationDialog ();

        void clear();

      public slots:
        virtual bool apply();

      protected slots:
        void updateHelpFileName();
   };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT PerformSymmetryDialog : public HexaBaseDialog,
                                                                  public Ui::SymmetryDialog
  {
      Q_OBJECT

      public:
        PerformSymmetryDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~PerformSymmetryDialog();

        void clear();

      public slots:
        virtual bool apply();

      protected slots:
        void updateHelpFileName();
   };




  class MyBasicGUI_PointDlg : public BasicGUI_PointDlg
  {
    public:
      MyBasicGUI_PointDlg( GeometryGUI* g, QWidget* w = 0, bool b= false, Qt::WindowFlags f= 0 );
      virtual ~MyBasicGUI_PointDlg();
  
      QPushButton* buttonCancel() const;
      QPushButton* buttonOk() const;
      QPushButton* buttonApply() const;
      QPushButton* buttonHelp() const;
  };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT VertexAssocDialog : public HexaBaseDialog,
                                                              public GEOMBase_Helper
  {
      Q_OBJECT

      public:
        VertexAssocDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );
        virtual ~VertexAssocDialog();
        void clear();

        void setDocumentModel(DocumentModel* m);
        void setPatternDataSelectionModel(PatternDataSelectionModel* s);

      public slots:
        void onSelectionChanged( const QItemSelection& sel, const QItemSelection& unsel );
        virtual bool apply();

      protected:
        bool eventFilter(QObject *obj, QEvent *event);
        void _raiseCurrentOccView();

      protected slots:
        void onWindowActivated(SUIT_ViewManager* vm);

      private:
        DocumentModel*              _documentModel;
        PatternDataSelectionModel*  _patternDataSelectionModel;

      // the dialog box contains : 1)vertex selection from HEXABLOCK 2)point construction in GEOM
        QLineEdit*            _vertex_le;
        QModelIndex*          _ivertex;
        MyBasicGUI_PointDlg*  _nested;
  };



  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT EdgeAssocDialog : public HexaBaseDialog,
                                                            public Ui::EdgeAssocDialog,
                                                            public GEOMBase_Helper
  {
      Q_OBJECT

      public:
        EdgeAssocDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~EdgeAssocDialog();
        void clear();

        void setGeomEngine( GEOM::GEOM_Gen_var geomEngine );

      public slots:
        virtual bool apply();

      protected:
        virtual bool eventFilter(QObject *obj, QEvent *event);
//         virtual void hideEvent ( QHideEvent * event );
//         virtual void showEvent ( QShowEvent * event );
        virtual GEOM::GEOM_IOperations_ptr createOperation();
        virtual bool execute( ObjectList& );

      protected slots:
        virtual void onSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel );

        void onWindowActivated(SUIT_ViewManager*);

        void deleteEdgeItem();
        void deleteLineItem();

        void addLine();
        void pstartChanged( double val );
        void pendChanged( double val );

      private:
        LightApp_SelectionMgr*        _mgr;

//         QShortcut* _delEdgeShortcut;
//         QShortcut* _delLineShortcut;


        // Preview in GEOM
        GEOM::GeomObjPtr    _firstLine;
        GEOM::GeomObjPtr    _lastLine;
        GEOM::GeomObjPtr    _currentLine;
        double              _currentParameter;
        GEOM::GEOM_Gen_var  _geomEngine ;

  };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT QuadAssocDialog : public HexaBaseDialog,
                                                            public Ui::QuadAssocDialog,
                                                            public GEOMBase_Helper
  {
      Q_OBJECT

      public:
        QuadAssocDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~QuadAssocDialog();
        void clear();

      public slots:
        virtual bool apply();

      protected:
        virtual bool eventFilter(QObject *obj, QEvent *event);

      protected slots:
        void onWindowActivated(SUIT_ViewManager*);
        void addFace();
        void deleteFaceItem();

      private:
        QList<DocumentModel::GeomObj> _assocs;
        LightApp_SelectionMgr*        _mgr;
        QShortcut*                    _delFaceShortcut;
  };



  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT GroupDialog : public HexaBaseDialog,
                                                        public Ui::GroupDialog
  {
      Q_OBJECT

      QMap<DocumentModel::Group, QString> strKind;

      public:
        GroupDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~GroupDialog();
        void clear();

        void setValue(HEXA_NS::Group* v);
        HEXA_NS::Group* getValue();

      public slots:
        virtual bool apply();

      protected slots:
        virtual void onSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel );

      private slots:
//         void addEltBase();
        void removeEltBase();
//         void clearEltBase();
        void onKindChanged( int index );

      private:
        HEXA_NS::Group   *_value;
  };



  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT LawDialog : public HexaBaseDialog,
                                                      public Ui::LawDialog
  {
      Q_OBJECT

      QMap<DocumentModel::KindLaw, QString> strKind;

      public:
        LawDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~LawDialog();
        void clear();

        void setValue(HEXA_NS::Law* v);
        HEXA_NS::Law* getValue();

      public slots:
        virtual bool apply();

      private:
        HEXA_NS::Law   *_value;
  };




  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT PropagationDialog : public HexaBaseDialog,
                                                      public Ui::PropagationDialog
  {
      Q_OBJECT

      public:
        PropagationDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~PropagationDialog();
        void clear();

        void setValue(HEXA_NS::Propagation* v);
        HEXA_NS::Propagation* getValue();

      public slots:
        virtual bool apply();

      protected slots:
        void updateHelpFileName();

      private:
        HEXA_NS::Propagation *_value;
  };



    // Define the compute mesh dialog box
    // ----------------------------------

  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT ComputeMeshDialog : public HexaBaseDialog
  {
      Q_OBJECT

      public:
        ComputeMeshDialog( QWidget* parent = 0, bool editMode = false, Qt::WindowFlags f = Qt::SubWindow );
        virtual ~ComputeMeshDialog();
        void clear();

        void setDocumentModel(DocumentModel* m);

      public slots:
        virtual bool apply();
//         virtual void accept();
//         virtual void reject();

      private:
	QLineEdit* _name;
	QSpinBox*  _dim;
	QLineEdit* _fact;
  };



  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT ReplaceHexaDialog : public HexaBaseDialog,
                                                              public Ui::ReplaceHexaDialog
  {
      Q_OBJECT

      public:
        ReplaceHexaDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~ReplaceHexaDialog();
        void clear();

      public slots:
        virtual bool apply();
        void onSelectionChanged( const QItemSelection& sel, const QItemSelection& unsel );

      protected:
        bool eventFilter(QObject *obj, QEvent *event);

      protected slots:
        void deleteQuadItem();
        void updateButtonBox();
  };





  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT QuadRevolutionDialog : public HexaBaseDialog,
                                                                 public Ui::QuadRevolutionDialog
  {
      Q_OBJECT

      public:
        QuadRevolutionDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~QuadRevolutionDialog();
        void clear();

      public slots:
        virtual bool apply();
        void onSelectionChanged( const QItemSelection& sel, const QItemSelection& unsel );

      protected:
        bool eventFilter(QObject *obj, QEvent *event);

      protected slots:
        void addAngleItem();
        void delAngleItem();
        void delQuadItem();
        void updateButtonBox();
  };



  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MakeHemiSphereDialog : public HexaBaseDialog,
                                                             public Ui::MakeHemiSphereDialog
  {
      Q_OBJECT

      public:
        MakeHemiSphereDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MakeHemiSphereDialog();
        void clear();

//         void setValue(HEXA_NS::Propagation* v);
//         HEXA_NS::Propagation* getValue();

      public slots:
        virtual bool apply();

      private:
//         HEXA_NS::Propagation *_value;:q
  };

//   class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MakeRindDialog : public HexaBaseDialog,
//                                                     public Ui::MakeRindDialog
//   {
//       Q_OBJECT
// 
//       public:
//         MakeRindDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
//         virtual ~MakeRindDialog();
// 
//       public slots:
//         virtual bool apply();
//   
//       private:
// //         HEXA_NS::Propagation *_value;:q
// 
//   };


  }
}



#endif
