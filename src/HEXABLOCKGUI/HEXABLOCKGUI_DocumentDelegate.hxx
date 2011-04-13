#ifndef __HEXABLOCKGUI_DOCUMENTDELEGATE_HXX_
#define __HEXABLOCKGUI_DOCUMENTDELEGATE_HXX_

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
