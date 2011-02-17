//  Copyright (C) 2006-2010  CEA/DEN, EDF R&D
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

#ifndef _HEXABLOCKGUI_DOCUMENTITEM_HXX_
#define _HEXABLOCKGUI_DOCUMENTITEM_HXX_


#include <QStandardItem>
#include <HexVertex.hxx>
#include <HexEdge.hxx>
#include <HexQuad.hxx>
#include <HexHexa.hxx>

Q_DECLARE_METATYPE( HEXA_NS::Vertex* );
Q_DECLARE_METATYPE( HEXA_NS::Edge* );
Q_DECLARE_METATYPE( HEXA_NS::Quad* );
Q_DECLARE_METATYPE( HEXA_NS::Hexa* );

namespace HEXABLOCK
{
  namespace GUI
  {
    enum {
      VERTEXITEM = QStandardItem::UserType + 1,
      EDGEITEM,
      QUADITEM,
      HEXAITEM
    };

    enum { 
      VERTEX_TREE = 10,
      EDGE_TREE,
      QUAD_TREE,
      HEXA_TREE,

      VECTOR_TREE,
      CYLINDER_TREE,
      PIPE_TREE,
      ELEMENTS_TREE,
      CROSSELEMENTS_TREE,

      VERTEX_DIR_TREE,
      EDGE_DIR_TREE,
      QUAD_DIR_TREE,
      HEXA_DIR_TREE,

      VECTOR_DIR_TREE,
      CYLINDER_DIR_TREE,
      PIPE_DIR_TREE,
      ELEMENTS_DIR_TREE,
      CROSSELEMENTS_DIR_TREE

    }; //HEXA_TREE_ROLE

    enum {
      HEXA_DATA_ROLE = Qt::UserRole + 1,
      HEXA_TREE_ROLE,
      HEXA_ENTRY_ROLE,
    };

// QVariant::UserType

    class VertexItem : public QStandardItem
    {
      public:
        VertexItem( HEXA_NS::Vertex* hexaVertex );
        virtual int type () const;
        virtual QVariant data ( int role ) const;
        virtual void setData ( const QVariant & value, int role ); //= Qt::UserRole + 1 )
      private:
        HEXA_NS::Vertex* _hexaVertex;
    };

    class EdgeItem : public QStandardItem
    {
      public:
        EdgeItem( HEXA_NS::Edge* hexaEdge );
        virtual int type () const;
        virtual QVariant data ( int role ) const;
        virtual void setData ( const QVariant & value, int role ); //= Qt::UserRole + 1 )
      private:
        HEXA_NS::Edge* _hexaEdge;
    };

    class QuadItem : public QStandardItem
    {
      public:
        QuadItem( HEXA_NS::Quad* hexaQuad );
        virtual int type () const;
        virtual QVariant data ( int role ) const;
        virtual void setData ( const QVariant & value, int role ); //= Qt::UserRole + 1 )
      private:
        HEXA_NS::Quad* _hexaQuad;
    };


    class HexaItem : public QStandardItem
    {
      public:
        HexaItem( HEXA_NS::Hexa* hexaHexa );
        virtual int type () const;
        virtual QVariant data ( int role ) const;
        virtual void setData ( const QVariant & value, int role ); //= Qt::UserRole + 1 )
      private:
        HEXA_NS::Hexa* _hexaHexa;
    };



  }
}

#endif






// The QStandardItem class provides an item for use with the QStandardItemModel class.
// 
// Items usually contain text, icons, or checkboxes.
// 
// Each item can have its own background brush which is set with the setBackground() function. The current background brush can be found with background(). The text label for each item can be rendered with its own font and brush. These are specified with the setFont() and setForeground() functions, and read with font() and foreground().
// 
// By default, items are enabled, editable, selectable, checkable, and can be used both as the source of a drag and drop operation and as a drop target. Each item's flags can be changed by calling setFlags(). Checkable items can be checked and unchecked with the setCheckState() function. The corresponding checkState() function indicates whether the item is currently checked.
// 
// You can store application-specific data in an item by calling setData().
// 
// Each item can have a two-dimensional table of child items. This makes it possible to build hierarchies of items. The typical hierarchy is the tree, in which case the child table is a table with a single column (a list).
// 
// The dimensions of the child table can be set with setRowCount() and setColumnCount(). Items can be positioned in the child table with setChild(). Get a pointer to a child item with child(). New rows and columns of children can also be inserted with insertRow() and insertColumn(), or appended with appendRow() and appendColumn(). When using the append and insert functions, the dimensions of the child table will grow as needed.
// 
// An existing row of children can be removed with removeRow() or takeRow(); correspondingly, a column can be removed with removeColumn() or takeColumn().
// 
// An item's children can be sorted by calling sortChildren().


// Subclassing
// 
// When subclassing QStandardItem to provide custom items, it is possible to define new types for them so that they can be distinguished from the base class. The type() function should be reimplemented to return a new type value equal to or greater than UserType.
// 
// Reimplement data() and setData() if you want to perform custom handling of data queries and/or control how an item's data is represented.
// 
// Reimplement clone() if you want QStandardItemModel to be able to create instances of your custom item class on demand (see QStandardItemModel::setItemPrototype()).
// 
// Reimplement read() and write() if you want to control how items are represented in their serialized form.
// 
// Reimplement operator<() if you want to control the semantics of item comparison. operator<() determines the sorted order when sorting items with sortChildren() or with QStandardItemModel::sort().

// QStandardItemModel model;
//  QStandardItem *parentItem = model.invisibleRootItem();
//  for (int i = 0; i < 4; ++i) {
//      QStandardItem *item = new QStandardItem(QString("item %0").arg(i));
//      parentItem->appendRow(item);
//      parentItem = item;
//  }

//  QStandardItem *item = myStandardItemModel->itemFromIndex(index);
