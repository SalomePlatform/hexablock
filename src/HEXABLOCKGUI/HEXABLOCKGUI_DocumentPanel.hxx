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
#include "ui_Hexa_QTD.h"

#include "ui_MergeVertices_QTD.h"
#include "ui_MergeEdges_QTD.h"
#include "ui_CutEdge_QTD.h"


#include "ui_Vector_QTD.h"
#include "ui_MakeCartesian_QTD.h"
#include "ui_MakeCylindrical_QTD.h"
#include "ui_MakeTranslation_QTD.h"


#include "HexVertex.hxx"
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


        protected:
          void _setAllSelection();
          void _setQuadSelectionOnly();
          void _setEdgeSelectionOnly();
          void _setVertexSelectionOnly();

          void _setElementsSelectionOnly();
          void _setVectorSelectionOnly();

        protected slots:
          virtual void onPatternDataSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel );
          virtual void onPatternBuilderSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel );


        protected :
//           HexaBaseModel          *_model;
//           DocumentSelectionModel *_selectionModel;
          DocumentModel*       _documentModel;
          PatternDataModel*    _patternDataModel;
          PatternBuilderModel* _patternBuilderModel;


          PatternDataSelectionModel*    _patternDataSelectionModel;
          PatternBuilderSelectionModel* _patternBuilderSelectionModel;

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

      public slots:
        virtual void accept();

      private:
        HEXA_NS::Vertex     *_value;
    };




    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT QuadDialog : public HexaBaseDialog,
					                 public Ui::QuadDialog
    {
      Q_OBJECT

      public:
        QuadDialog( QWidget* = 0, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~QuadDialog();

        void setValue(HEXA_NS::Quad* v);
        HEXA_NS::Quad* getValue();

      public slots:
        virtual void accept();
        virtual void reject();

      protected:
        bool eventFilter(QObject *obj, QEvent *event);

      protected slots:
        virtual void onPatternDataSelectionChanged( const QItemSelection& sel,
                                                    const QItemSelection& unsel );

      private:
        HEXA_NS::Quad       *_value;

        // User selection 
        QModelIndex     _v0Index;
        QModelIndex     _v1Index;
        QModelIndex     _v2Index;
        QModelIndex     _v3Index;

        QModelIndex*    _currentVertexIndex;
        QLineEdit*      _currentVertexLineEdit;
    };



    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT HexaDialog : public HexaBaseDialog,
                                                         public Ui::HexaDialog
    {
      Q_OBJECT

      public:
        HexaDialog( QWidget* = 0, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~HexaDialog();

        void setValue(HEXA_NS::Hexa* v);
        HEXA_NS::Hexa* getValue();

      public slots:
        virtual void accept();
        virtual void reject();

      protected:
        bool eventFilter(QObject *obj, QEvent *event);

      protected slots:
        virtual void onPatternDataSelectionChanged( const QItemSelection& sel,
                                                    const QItemSelection& unsel );

      private:
        HEXA_NS::Hexa   *_value;

        // User selection 
        QModelIndex     _q0Index;
        QModelIndex     _q1Index;
        QModelIndex     _q2Index;
        QModelIndex     _q3Index;
        QModelIndex     _q4Index;
        QModelIndex     _q5Index;

        QModelIndex*    _currentQuadIndex;
        QLineEdit*      _currentQuadLineEdit;
    };




    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MergeVerticesDialog : public HexaBaseDialog,
					                          public Ui::MergeVerticesDialog
    {
      Q_OBJECT

      public:
        MergeVerticesDialog( QWidget* = 0, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MergeVerticesDialog();

      public slots:
        virtual void accept();
        virtual void reject();

      protected:
        bool eventFilter(QObject *obj, QEvent *event);

      protected slots:
        virtual void onPatternDataSelectionChanged( const QItemSelection& sel,
                                                    const QItemSelection& unsel );

      private:
        // User selection 
        QModelIndex     _v0Index;
        QModelIndex     _v1Index;

        QModelIndex*    _currentVertexIndex;
        QLineEdit*      _currentVertexLineEdit;

   };



    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MergeEdgesDialog : public HexaBaseDialog,
                                                               public Ui::MergeEdgesDialog
    {
      Q_OBJECT

      public:
        MergeEdgesDialog( QWidget* = 0, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MergeEdgesDialog();

      public slots:
        virtual void accept();
        virtual void reject();

      protected:
        bool eventFilter(QObject *obj, QEvent *event);

      protected slots:
        virtual void onPatternDataSelectionChanged( const QItemSelection& sel,
                                                    const QItemSelection& unsel );

      private:
        // User selection
        QModelIndex     _e0Index;
        QModelIndex     _e1Index;
        QModelIndex     _v0Index;
        QModelIndex     _v1Index;

        QModelIndex*    _currentIndex;
        QLineEdit*      _currentLineEdit;
   };



  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT CutEdgeDialog : public HexaBaseDialog,
                                                          public Ui::CutEdgeDialog
  {
      Q_OBJECT

      public:
        CutEdgeDialog( QWidget* = 0, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~CutEdgeDialog();

      public slots:
        virtual void accept();
        virtual void reject();

      protected slots:
        virtual void onPatternDataSelectionChanged( const QItemSelection& sel,
                                                    const QItemSelection& unsel );
      private:
        // User selection
        QModelIndex     _eIndex;
   };



  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT VectorDialog : public HexaBaseDialog,
                                                         public Ui::VectorDialog
  {
      Q_OBJECT

      public:
        VectorDialog( QWidget* = 0, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~VectorDialog();

        void setValue(HEXA_NS::Vector* v);
        HEXA_NS::Vector* getValue();

      public slots:
        virtual void accept();

      private:
        HEXA_NS::Vector *_value;
  };



  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MakeCartesianDialog : public HexaBaseDialog,
                                                                public Ui::MakeCartesianDialog
  {
      Q_OBJECT

      public:
        PatternDataModel* patternDataModel;

        MakeCartesianDialog( QWidget* = 0, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MakeCartesianDialog();

      public slots:
        virtual void accept();
        virtual void reject();

      protected:
        bool eventFilter(QObject *obj, QEvent *event);

      protected slots:
        virtual void onPatternDataSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel );
        virtual void onPatternBuilderSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel );

      private:
        // User selection
        QModelIndex     _ptIndex;
        QModelIndex     _vecXIndex;
        QModelIndex     _vecYIndex;
        QModelIndex     _vecZIndex;

        QModelIndex*    _currentIndex;
        QLineEdit*      _currentLineEdit;
  };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MakeCylindricalDialog : public HexaBaseDialog,
                                                                  public Ui::MakeCylindricalDialog
  {
      Q_OBJECT

      public:
        MakeCylindricalDialog( QWidget* = 0, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MakeCylindricalDialog();

      public slots:
        virtual void accept();
        virtual void reject();

      protected:
        bool eventFilter(QObject *obj, QEvent *event);

      protected slots:
        virtual void onPatternDataSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel );
        virtual void onPatternBuilderSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel );

      private:
        // User selection
        QModelIndex     _ptIndex;
        QModelIndex     _vecXIndex;
        QModelIndex     _vecZIndex;

        QModelIndex*    _currentIndex;
        QLineEdit*      _currentLineEdit;
  };


  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MakeTranslationDialog : public HexaBaseDialog,
                                                                  public Ui::MakeTranslationDialog
  {
      Q_OBJECT

      public:
        MakeTranslationDialog( QWidget* = 0, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MakeTranslationDialog();

      public slots:
        virtual void accept();
        virtual void reject();

      protected:
        bool eventFilter(QObject *obj, QEvent *event);

      protected slots:
        virtual void onPatternBuilderSelectionChanged(  const QItemSelection& sel, const QItemSelection& unsel );

      private:
        // User selection
        QModelIndex     _eltIndex;
        QModelIndex     _vecIndex;

        QModelIndex*    _currentIndex;
        QLineEdit*      _currentLineEdit;

//         QItemSelectionModel* _selectionModel2;
  };



  }
}

#endif
