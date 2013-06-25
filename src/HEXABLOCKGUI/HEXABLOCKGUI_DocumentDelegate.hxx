// Copyright (C) 2009-2013  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
#include "HEXABLOCKGUI_DocumentItem.hxx"
#include "HEXABLOCKGUI_DocumentPanel.hxx"

namespace HEXABLOCK
{
  namespace GUI
  {
      class DocumentDelegate : public QItemDelegate
      {
          Q_OBJECT
      
      public:
          /// close the editor
          void closeDialog();

          DocumentDelegate( QDockWidget *dw, QObject *parent = 0);

          /// create the editor
          QWidget *createEditor( QWidget *parent,
                                 const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const;

          /// set editor's data
          void setEditorData( QWidget *editor, const QModelIndex &index) const;

          /// set the editor's model
          void setModelData( QWidget            *editor,
                             QAbstractItemModel *model,
                             const QModelIndex  &index ) const {};

          virtual void updateEditorGeometry( QWidget *editor,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index ) const {};

      protected:
          mutable QWidget* _currentEditor;
          virtual bool editorEvent ( QEvent * event,
        		  QAbstractItemModel * model,
        		  const QStyleOptionViewItem & option, const QModelIndex & index );
          virtual bool 	eventFilter ( QObject * editor, QEvent * event );

      private:
          QDockWidget* _dw; // creator's container

      private slots:
          void commitEditor();

      };
  }
}

#endif
