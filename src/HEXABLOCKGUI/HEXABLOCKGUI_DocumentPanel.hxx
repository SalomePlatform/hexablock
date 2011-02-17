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

#include "HexVertex.hxx"
#include "HexQuad.hxx"
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

    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT VertexDialog : public QDialog,
					                   public Ui::VertexDialog
    {
      Q_OBJECT

      public:
        VertexDialog( QWidget* = 0, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~VertexDialog();

        void setValue(HEXA_NS::Vertex* v);
        HEXA_NS::Vertex* getValue();

        void setModel(PatternDataModel* m); //(DocumentModel* m);
        void setSelectionModel(DocumentSelectionModel* s); //QItemSelectionModel* s);

      public slots:
        virtual void accept();

      private:
        HEXA_NS::Vertex     *_value;
        PatternDataModel    *_model; //DocumentModel       *
        DocumentSelectionModel *_selectionModel; //QItemSelectionModel 
        
    };




    class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT QuadDialog : public QDialog,
					                 public Ui::QuadDialog
    {
      Q_OBJECT

      public:
        QuadDialog( QWidget* = 0, Qt::WindowFlags = Qt::SubWindow );//= 0 );
        virtual ~QuadDialog();

        void setValue(HEXA_NS::Quad* v);
        HEXA_NS::Quad* getValue();

        void setModel(PatternDataModel *m); //(DocumentModel* m);
        void setSelectionModel(DocumentSelectionModel* s); //QItemSelectionModel* s);

      public slots:
        virtual void accept();
        virtual void reject();
        void onSelectionChanged(  const QItemSelection& sel,
                                  const QItemSelection& unsel );


      protected:
        bool eventFilter(QObject *obj, QEvent *event);

      private:
        HEXA_NS::Quad       *_value;
        PatternDataModel    *_model; //DocumentModel* 
        DocumentSelectionModel *_selectionModel; //QItemSelectionModel 

        // User selection 
        QModelIndex     _v0Index;
        QModelIndex     _v1Index;
        QModelIndex     _v2Index;
        QModelIndex     _v3Index;

        QModelIndex*    _currentVertexIndex;
        QLineEdit*      _currentVertexLineEdit;

    };




//     class HEXABLOCKGUI_DOCUMENTPANEL_EXPORT HexaDialog : public QDialog,
// 					                 public Ui::HexaDialog
//     {
//       Q_OBJECT
// 
//       public:
//         HexaDialog( QWidget* = 0, Qt::WindowFlags = Qt::SubWindow );//= 0 );
//         virtual ~HexaDialog();
// 
//         void setValue(HEXA_NS::Hexa* v);
//         HEXA_NS::Hexa* getValue();
// 
//         void setModel(DocumentModel* m);
//         void setSelectionModel(QItemSelectionModel* s);
// 
//       public slots:
//         virtual void accept();
// 
//       private:
//         HEXA_NS::Hexa       *_value;
//         DocumentModel       *_model;
//         QItemSelectionModel *_selectionModel;
//     };


  }
}

#endif
