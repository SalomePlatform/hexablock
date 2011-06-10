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
#include "ui_Group_QTD.h"
#include "ui_Law_QTD.h"
#include "ui_Propagation_QTD.h"





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

#include "klinkitemselectionmodel.hxx"


Q_DECLARE_METATYPE(QModelIndex); 

namespace HEXABLOCK
{
  namespace GUI
  {



    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT HexaBaseDialog : public QDialog
    {
        Q_OBJECT
        public:
          HexaBaseDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
          virtual ~HexaBaseDialog();

          //  model
          virtual void setDocumentModel( DocumentModel* m );

          //  selection
          virtual void setPatternDataSelectionModel( PatternDataSelectionModel* s );
          virtual void setPatternBuilderSelectionModel( PatternBuilderSelectionModel* s );
          virtual void setMeshSelectionModel( QItemSelectionModel* s );

//         signals:
//           void editingFinished ();


        protected:
          void _setAllSelection();
          void _setHexaSelectionOnly();
          void _setQuadSelectionOnly();
          void _setEdgeSelectionOnly();
          void _setVertexSelectionOnly();

          void _setElementsSelectionOnly();
          void _setVectorSelectionOnly();
          void _setCylinderSelectionOnly();
          void _setPipeSelectionOnly();

          void _setLawSelectionOnly();

        protected slots:
          virtual void onPatternDataSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel );
          virtual void onPatternBuilderSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel );
          virtual void onMeshSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel );


        protected :
//           void installEventFilter();
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

          QList<QLineEdit*>  _hexaLineEdits;
          QList<QLineEdit*>  _quadLineEdits;
          QList<QLineEdit*>  _edgeLineEdits;
          QList<QLineEdit*>  _vertexLineEdits;

          QList<QLineEdit*>  _vectorLineEdits;
          QList<QLineEdit*>  _cylinderLineEdits;
          QList<QLineEdit*>  _pipeLineEdits;
          QList<QLineEdit*>  _elementsLineEdits;

          QList<QLineEdit*>  _lawLineEdits;
    };



    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT VertexDialog : public HexaBaseDialog,
					                   public Ui::VertexDialog
    {
      Q_OBJECT

      public:
        VertexDialog( QWidget* = 0, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~VertexDialog();

        void setValue(HEXA_NS::Vertex* v);
        HEXA_NS::Vertex* getValue();

        void setIndex(const QModelIndex& i);

      public slots:
        virtual void accept();

      private:
        HEXA_NS::Vertex     *_value;
        QModelIndex         _index;
    };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT EdgeDialog : public HexaBaseDialog,
                                                       public Ui::EdgeDialog
  {
      Q_OBJECT

      public:
        EdgeDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~EdgeDialog();

        void setValue(HEXA_NS::Edge* e);
        HEXA_NS::Edge* getValue();

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual void showEvent ( QShowEvent * event );

      private:
        HEXA_NS::Edge *_value;
    };


    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT QuadDialog : public HexaBaseDialog,
					                 public Ui::QuadDialog
    {
      Q_OBJECT

      public:
        QuadDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~QuadDialog();

        void setValue(HEXA_NS::Quad* v);
        HEXA_NS::Quad* getValue();

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual void showEvent ( QShowEvent * event );

      private:
        HEXA_NS::Quad    *_value;

    };



    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT HexaDialog : public HexaBaseDialog,
                                                         public Ui::HexaDialog
    {
      Q_OBJECT

      public:
        HexaDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~HexaDialog();

        void setValue(HEXA_NS::Hexa* v);
        HEXA_NS::Hexa* getValue();

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual void showEvent ( QShowEvent * event );

      private:
        HEXA_NS::Hexa   *_value;

    };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT VectorDialog : public HexaBaseDialog,
                                                         public Ui::VectorDialog
  {
      Q_OBJECT

      public:
        VectorDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~VectorDialog();

        void setValue(HEXA_NS::Vector* v);
        HEXA_NS::Vector* getValue();

      public slots:
        virtual void accept();

//       protected:
//         virtual void showEvent ( QShowEvent * event );

      private:
        HEXA_NS::Vector *_value;
  };



  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT CylinderDialog : public HexaBaseDialog,
                                                           public Ui::CylinderDialog
  {
      Q_OBJECT

      public:
        CylinderDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~CylinderDialog();

        void setValue( HEXA_NS::Cylinder* v );
        HEXA_NS::Cylinder* getValue();

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual void showEvent ( QShowEvent * event );

      private:
        HEXA_NS::Cylinder   *_value;
    };



  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT PipeDialog : public HexaBaseDialog,
                                                       public Ui::PipeDialog
  {
      Q_OBJECT

      public:
        PipeDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~PipeDialog();

        void setValue( HEXA_NS::Pipe* p );
        HEXA_NS::Pipe* getValue();

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual void showEvent ( QShowEvent * event );

      private:
        HEXA_NS::Pipe   *_value;

    };



  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MakeGridDialog : public HexaBaseDialog,
                                                           public Ui::MakeGridDialog
  {
      Q_OBJECT

      public:
        MakeGridDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MakeGridDialog();

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual void showEvent ( QShowEvent * event );
  };




  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MakeCylinderDialog : public HexaBaseDialog,
                                                           public Ui::MakeCylinderDialog
  {
      Q_OBJECT

      public:
        MakeCylinderDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MakeCylinderDialog();

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual void showEvent ( QShowEvent * event );

  };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MakePipeDialog : public HexaBaseDialog,
                                                           public Ui::MakePipeDialog
  {
      Q_OBJECT

      public:
        MakePipeDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MakePipeDialog();

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual void showEvent ( QShowEvent * event );
  };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MakeCylindersDialog : public HexaBaseDialog,
                                                                public Ui::MakeCylindersDialog 
  {
      Q_OBJECT

      public:
        MakeCylindersDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MakeCylindersDialog();

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual void showEvent ( QShowEvent * event );
  };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MakePipesDialog : public HexaBaseDialog,
                                                            public Ui::MakePipesDialog
  {
      Q_OBJECT

      public:
        MakePipesDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MakePipesDialog();

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual void showEvent ( QShowEvent * event );
  };



  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT RemoveHexaDialog : public HexaBaseDialog,
                                                             public Ui::RemoveHexaDialog
  {
      Q_OBJECT

      public:
        RemoveHexaDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~RemoveHexaDialog();

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual void showEvent ( QShowEvent * event );

  };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT PrismQuadDialog : public HexaBaseDialog,
                                                            public Ui::PrismQuadDialog
  {
      Q_OBJECT

      public:
        PrismQuadDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~PrismQuadDialog();

      public slots:
        virtual void accept();
        virtual void reject();

      protected:
        virtual bool eventFilter(QObject *obj, QEvent *event);
//         virtual void showEvent ( QShowEvent * event );

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

      public slots:
        virtual void accept();
        virtual void reject();

      protected:
        virtual bool eventFilter(QObject *obj, QEvent *event);
//         virtual void showEvent ( QShowEvent * event );

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

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual void showEvent ( QShowEvent * event );

   };



  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT DisconnectDialog : public HexaBaseDialog,
                                                             public Ui::DisconnectDialog
  {
      Q_OBJECT

      public:
        DisconnectDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~DisconnectDialog();

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual void showEvent ( QShowEvent * event );

   };



  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT CutEdgeDialog : public HexaBaseDialog,
                                                          public Ui::CutEdgeDialog
  {
      Q_OBJECT

      public:
        CutEdgeDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~CutEdgeDialog();

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual void showEvent ( QShowEvent * event );
   };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MakeTransformationDialog : public HexaBaseDialog,
                                                          public Ui::TransformationDialog
  {
      Q_OBJECT

      public:
        MakeTransformationDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MakeTransformationDialog ();

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual void showEvent ( QShowEvent * event );
   };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MakeSymmetryDialog : public HexaBaseDialog,
                                                               public Ui::SymmetryDialog
  {
      Q_OBJECT

      public:
        MakeSymmetryDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MakeSymmetryDialog();

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual void showEvent ( QShowEvent * event );
   };




  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT PerformTransformationDialog : public HexaBaseDialog,
                                                                        public Ui::TransformationDialog
  {
      Q_OBJECT

      public:
        PerformTransformationDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~PerformTransformationDialog ();

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual void showEvent ( QShowEvent * event );
   };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT PerformSymmetryDialog : public HexaBaseDialog,
                                                          public Ui::SymmetryDialog
  {
      Q_OBJECT

      public:
        PerformSymmetryDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~PerformSymmetryDialog();

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual void showEvent ( QShowEvent * event );
   };






  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT GroupDialog : public HexaBaseDialog,
                                                        public Ui::GroupDialog
  {
      Q_OBJECT

      enum {
        LW_QMODELINDEX_ROLE = Qt::UserRole + 1
      };
      QMap<DocumentModel::Group, QString> strKind;

      public:
        GroupDialog( QWidget* = 0, bool = false, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~GroupDialog();

        void setValue(HEXA_NS::Group* v);
        HEXA_NS::Group* getValue();

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual bool eventFilter(QObject *obj, QEvent *event);

//       protected:
//         virtual void showEvent ( QShowEvent * event );

      private slots:
        void addEltBase();
        void removeEltBase();
        void clearEltBase();
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

        void setValue(HEXA_NS::Law* v);
        HEXA_NS::Law* getValue();

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual void showEvent ( QShowEvent * event );
  
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

        void setValue(HEXA_NS::Propagation* v);
        HEXA_NS::Propagation* getValue();

      public slots:
        virtual void accept();
        virtual void reject();

//       protected:
//         virtual void showEvent ( QShowEvent * event );
  
      private:
        HEXA_NS::Propagation *_value;
  };







  }
}

#endif
