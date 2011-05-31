#ifndef __HEXABLOCKGUI_DOCUMENTDELEGATE_HXX_
#define __HEXABLOCKGUI_DOCUMENTDELEGATE_HXX_


// QEvent * event, QAbstractItemModel 

#include <QEvent>
#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QSpinBox>

#include <QDockWidget>


#include "HEXABLOCKGUI_DocumentModel.hxx"
#include "HEXABLOCKGUI_DocumentSelectionModel.hxx"

namespace HEXABLOCK
{
  namespace GUI
  {
      class DocumentDelegate : public QItemDelegate
      {
          Q_OBJECT
      
      public:
          DocumentDelegate( QDockWidget *dw, QObject *parent = 0);
      
          QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const;
      
          void setEditorData(QWidget *editor, const QModelIndex &index) const;
          void setModelData(QWidget *editor, QAbstractItemModel *model,
                            const QModelIndex &index) const;
      
          void updateEditorGeometry(QWidget *editor,
              const QStyleOptionViewItem &option, const QModelIndex &index) const;


         // can be used by editor
          void setDocumentModel( DocumentModel* m );

          //  selection
          void setPatternDataSelectionModel( PatternDataSelectionModel* s );
          void setPatternBuilderSelectionModel( PatternBuilderSelectionModel* s );
          void setGroupSelectionModel( QItemSelectionModel* s );
          void setMeshSelectionModel( QItemSelectionModel* s );


//CS_TEST
// virtual bool 	editorEvent ( QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index ){
//   std::cout << "editorEvent editorEvent " << event->type() << std::endl;
// //   std::cout << "Type => "<< event->type() << std::endl;
//   return QItemDelegate::editorEvent ( event, model, option, index );
// }
virtual bool 	eventFilter ( QObject * editor, QEvent * event )
{
//   std::cout << "eventFilter  eventFilter " << event->type() << std::endl;
//   std::cout << std::endl;

 if (event->type() == QEvent::FocusOut) {
//              QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
//              qDebug() << "Ate key press" << keyEvent->key();
//              return true;
return false;
         } else {
              return QItemDelegate::eventFilter ( editor, event );
//              return false;
         }

  
}
//CS_TEST
      
      private:
          QDockWidget *_dw;


          // can be used by editor
          DocumentModel*                _documentModel;

          PatternDataSelectionModel*    _patternDataSelectionModel;
          PatternBuilderSelectionModel* _patternBuilderSelectionModel;
          QItemSelectionModel*          _groupsSelectionModel;
          QItemSelectionModel*          _meshSelectionModel;
      };
  }
}

#endif
