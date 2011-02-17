#ifndef __HEXABLOCKGUI_DOCUMENTDELEGATE_HXX_
#define __HEXABLOCKGUI_DOCUMENTDELEGATE_HXX_

#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QSpinBox>

#include <QDockWidget>
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
      
      private:
          QDockWidget *_dw;
      };
  }
}

#endif
