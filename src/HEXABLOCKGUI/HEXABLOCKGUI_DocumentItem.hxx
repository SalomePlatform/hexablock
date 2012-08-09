// Copyright (C) 2009-2012  CEA/DEN, EDF R&D
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

#ifndef _HEXABLOCKGUI_DOCUMENTITEM_HXX_
#define _HEXABLOCKGUI_DOCUMENTITEM_HXX_


#include <QStandardItem>

#include <HexVertex.hxx>
#include <HexEdge.hxx>
#include <HexQuad.hxx>
#include <HexHexa.hxx>


#include <HexVector.hxx>
#include <HexCylinder.hxx>
#include <HexPipe.hxx>
#include <HexElements.hxx>
#include <HexCrossElements.hxx>



#include <HexGroup.hxx>
#include <HexLaw.hxx>
#include <HexPropagation.hxx>



Q_DECLARE_METATYPE( HEXA_NS::EltBase* );
Q_DECLARE_METATYPE( HEXA_NS::Vertex* );
Q_DECLARE_METATYPE( HEXA_NS::Edge* );
Q_DECLARE_METATYPE( HEXA_NS::Quad* );
Q_DECLARE_METATYPE( HEXA_NS::Hexa* );

Q_DECLARE_METATYPE( HEXA_NS::Vector* );
Q_DECLARE_METATYPE( HEXA_NS::Cylinder* );
Q_DECLARE_METATYPE( HEXA_NS::Pipe* );
Q_DECLARE_METATYPE( HEXA_NS::Elements* );
Q_DECLARE_METATYPE( HEXA_NS::CrossElements* );

Q_DECLARE_METATYPE( HEXA_NS::Group* );
Q_DECLARE_METATYPE( HEXA_NS::Law* );
Q_DECLARE_METATYPE( HEXA_NS::Propagation* );



// Q_DECLARE_METATYPE( QList<HEXA_NS::Shape*> ); // association

namespace HEXABLOCK
{
  namespace GUI
  {
    enum {
      VERTEXITEM = QStandardItem::UserType + 1,
      EDGEITEM,
      QUADITEM,
      HEXAITEM,

      VECTORITEM,
      CYLINDERITEM,
      PIPEITEM,
      ELEMENTSITEM,
      CROSSELEMENTSITEM,

      GROUPITEM,
      LAWITEM,
      PROPAGATIONITEM,

    };

    enum HexaTreeRole { 
      VERTEX_TREE = 10,
      EDGE_TREE,
      QUAD_TREE,
      HEXA_TREE,

      VECTOR_TREE,
      CYLINDER_TREE,
      PIPE_TREE,
      ELEMENTS_TREE,
      CROSSELEMENTS_TREE,

      GROUP_TREE,
      LAW_TREE,
      PROPAGATION_TREE,

      VERTEX_DIR_TREE,
      EDGE_DIR_TREE,
      QUAD_DIR_TREE,
      HEXA_DIR_TREE,

      VECTOR_DIR_TREE,
      CYLINDER_DIR_TREE,
      PIPE_DIR_TREE,
      ELEMENTS_DIR_TREE,
      CROSSELEMENTS_DIR_TREE,

      GROUP_DIR_TREE,
      LAW_DIR_TREE,
      PROPAGATION_DIR_TREE

    }; //HEXA_TREE_ROLE

    enum {
      HEXA_DATA_ROLE = Qt::UserRole + 1,
      HEXA_DATA_ASSOC,
      HEXA_TREE_ROLE,
      HEXA_ENTRY_ROLE,
      HEXA_DOC_ENTRY_ROLE,
      HEXA_ASSOC_ENTRY_ROLE,
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


    class VectorItem : public QStandardItem
    {
      public:
        VectorItem( HEXA_NS::Vector* hexaVector );
        virtual int type () const;
        virtual QVariant data ( int role ) const;
        virtual void setData ( const QVariant & value, int role ); //= Qt::UserRole + 1 )
      private:
        HEXA_NS::Vector* _hexaVector;
    };


    class CylinderItem : public QStandardItem
    {
      public:
        CylinderItem( HEXA_NS::Cylinder* hexaCyl );
        virtual int type () const;
        virtual QVariant data ( int role ) const;
        virtual void setData ( const QVariant & value, int role ); //= Qt::UserRole + 1 )
      private:
        HEXA_NS::Cylinder* _hexaCylinder;
    };


    class PipeItem : public QStandardItem
    {
      public:
        PipeItem( HEXA_NS::Pipe* hexaPipe );
        virtual int type () const;
        virtual QVariant data ( int role ) const;
        virtual void setData ( const QVariant & value, int role ); //= Qt::UserRole + 1 )
      private:
        HEXA_NS::Pipe* _hexaPipe;
    };


    class ElementsItem : public QStandardItem
    {
      public:
        ElementsItem( HEXA_NS::Elements* hexaElements );
        virtual int type () const;
        virtual QVariant data ( int role ) const;
        virtual void setData ( const QVariant & value, int role ); //= Qt::UserRole + 1 )
      private:
        HEXA_NS::Elements* _hexaElements;
    };

    class CrossElementsItem : public QStandardItem
    {
      public:
        CrossElementsItem( HEXA_NS::CrossElements* hexaCrossElts );
        virtual int type () const;
        virtual QVariant data ( int role ) const;
        virtual void setData ( const QVariant & value, int role ); //= Qt::UserRole + 1 )
      private:
        HEXA_NS::CrossElements* _hexaCrossElts;
    };


      

    class GroupItem : public QStandardItem
    {
      public:
        GroupItem( HEXA_NS::Group* hexaGroup );
        virtual int type () const;
        virtual QVariant data ( int role ) const;
        virtual void setData ( const QVariant & value, int role ); //= Qt::UserRole + 1 )
      private:
        HEXA_NS::Group* _hexaGroup;
    };


    class LawItem : public QStandardItem
    {
      public:
        LawItem( HEXA_NS::Law* hexaLaw );
        virtual int type () const;
        virtual QVariant data ( int role ) const;
        virtual void setData ( const QVariant & value, int role ); //= Qt::UserRole + 1 )
      private:
        HEXA_NS::Law* _hexaLaw;
    };


    class PropagationItem : public QStandardItem
    {
      public:
        PropagationItem( HEXA_NS::Propagation* hexaPropagation );
        virtual int type () const;
        virtual QVariant data ( int role ) const;
        virtual void setData ( const QVariant & value, int role ); //= Qt::UserRole + 1 )
      private:
        HEXA_NS::Propagation* _hexaPropagation;
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
