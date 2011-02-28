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

namespace HEXABLOCK
{
  namespace GUI
  {

    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT HexaBaseDialog 
    {
        public:
          HexaBaseDialog();
          virtual ~HexaBaseDialog();

          virtual void setModel(PatternDataModel *m); //(DocumentModel* m);
          virtual void setSelectionModel(DocumentSelectionModel* s); //QItemSelectionModel* s);

        protected:
          void _setAllSelection();
          void _setQuadSelectionOnly();
          void _setEdgeSelectionOnly();
          void _setVertexSelectionOnly();


          PatternDataModel       *_model;
          DocumentSelectionModel *_selectionModel;
    };



    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT VertexDialog : public QDialog,
					                   public Ui::VertexDialog,
                                                           public HexaBaseDialog
    {
      Q_OBJECT

      public:
        VertexDialog( QWidget* = 0, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~VertexDialog();

        void setValue(HEXA_NS::Vertex* v);
        HEXA_NS::Vertex* getValue();

      public slots:
        virtual void accept();

      private slots:
          void onSelectionChanged(  const QItemSelection& sel,
                                    const QItemSelection& unsel );
      private:
        HEXA_NS::Vertex     *_value;
    };




    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT QuadDialog : public QDialog,
					                 public Ui::QuadDialog,
                                                         public HexaBaseDialog
    {
      Q_OBJECT

      public:
        QuadDialog( QWidget* = 0, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~QuadDialog();

        void setValue(HEXA_NS::Quad* v);
        HEXA_NS::Quad* getValue();

//         virtual void setModel(PatternDataModel *m); //(DocumentModel* m);
        virtual void setSelectionModel(DocumentSelectionModel* s); //QItemSelectionModel* s);

      public slots:
        virtual void accept();
        virtual void reject();

      protected:
        bool eventFilter(QObject *obj, QEvent *event);

      private slots:
        void onSelectionChanged(  const QItemSelection& sel,
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



    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT HexaDialog : public QDialog,
                                                         public Ui::HexaDialog,
                                                         public HexaBaseDialog
    {
      Q_OBJECT

      public:
        HexaDialog( QWidget* = 0, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~HexaDialog();

        void setValue(HEXA_NS::Hexa* v);
        HEXA_NS::Hexa* getValue();

//         virtual void setModel(PatternDataModel *m); //(DocumentModel* m);
        virtual void setSelectionModel(DocumentSelectionModel* s); //QItemSelectionModel* s);

      public slots:
        virtual void accept();
        virtual void reject();

      protected:
        bool eventFilter(QObject *obj, QEvent *event);

      private slots:
          void onSelectionChanged(  const QItemSelection& sel,
                                    const QItemSelection& unsel );

      private:
        HEXA_NS::Hexa       *_value;

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




    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MergeVerticesDialog : public QDialog,
					                          public Ui::MergeVerticesDialog,
                                                                  public HexaBaseDialog
    {
      Q_OBJECT

      public:
        MergeVerticesDialog( QWidget* = 0, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MergeVerticesDialog();

//         virtual void setModel(PatternDataModel *m); //(DocumentModel* m);
        virtual void setSelectionModel(DocumentSelectionModel* s); //QItemSelectionModel* s);

      public slots:
        virtual void accept();
        virtual void reject();

      protected:
        bool eventFilter(QObject *obj, QEvent *event);

      private slots:
        void onSelectionChanged(  const QItemSelection& sel,
                                  const QItemSelection& unsel );
      private:
        // User selection 
        QModelIndex     _v0Index;
        QModelIndex     _v1Index;

        QModelIndex*    _currentVertexIndex;
        QLineEdit*      _currentVertexLineEdit;

   };



    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT MergeEdgesDialog : public QDialog,
                                                               public Ui::MergeEdgesDialog,
                                                               public HexaBaseDialog
    {
      Q_OBJECT

      public:
        MergeEdgesDialog( QWidget* = 0, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~MergeEdgesDialog();

//         virtual void setModel(PatternDataModel *m); //(DocumentModel* m);
        virtual void setSelectionModel(DocumentSelectionModel* s); //QItemSelectionModel* s);

      public slots:
        virtual void accept();
        virtual void reject();

      protected:
        bool eventFilter(QObject *obj, QEvent *event);

      private slots:
        void onSelectionChanged(  const QItemSelection& sel,
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







  class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT CutEdgeDialog : public QDialog,
					                  public Ui::CutEdgeDialog,
                                                          public HexaBaseDialog
  {
      Q_OBJECT

      public:
        CutEdgeDialog( QWidget* = 0, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~CutEdgeDialog();

//         virtual void setModel(PatternDataModel *m); //(DocumentModel* m);
        virtual void setSelectionModel(DocumentSelectionModel* s); //QItemSelectionModel* s);

      public slots:
        virtual void accept();
        virtual void reject();

      private slots:
        void onSelectionChanged(  const QItemSelection& sel,
                                  const QItemSelection& unsel );

      private:
        // User selection
        QModelIndex     _eIndex;

   };


  }
}

#endif
