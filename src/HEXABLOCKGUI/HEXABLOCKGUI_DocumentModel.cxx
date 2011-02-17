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



#include "HEXABLOCKGUI_DocumentModel.hxx"
#include "HEXABLOCKGUI_DocumentItem.hxx"

#include "HEXABLOCKGUI.h"
#include "HEXABLOCKGUI_Trace.hxx"


#include "HexVertex.hxx"


//#define _DEVDEBUG_


using namespace std;
using namespace HEXABLOCK::GUI;



/*****************************************************************
                      DocumentModel
*****************************************************************/
DocumentModel::DocumentModel(QObject * parent):
  QStandardItemModel(parent),
  _hexaDocument(  new HEXA_NS::Document("/tmp/doc.hex") ), //CS_TODO
  _vertexDirItem( new QStandardItem("Vertex") ),
  _edgeDirItem(   new QStandardItem("Edge") ),
  _quadDirItem(   new QStandardItem("Quad") ),
  _hexaDirItem(   new QStandardItem("Hexa") ),
  _vectorDirItem(        new QStandardItem("Vector") ),
  _cylinderDirItem(      new QStandardItem("Cylinder") ),
  _pipeDirItem(          new QStandardItem("Pipe") ),
  _elementsDirItem(      new QStandardItem("Elements") ),
  _crossElementsDirItem( new QStandardItem("CrossElements") )
{
  QStandardItem *parentItem = invisibleRootItem();

  _vertexDirItem->setData( VERTEX_DIR_TREE,           HEXA_TREE_ROLE );
  _edgeDirItem->setData( EDGE_DIR_TREE,               HEXA_TREE_ROLE );
  _quadDirItem->setData( QUAD_DIR_TREE,               HEXA_TREE_ROLE );
  _hexaDirItem->setData( HEXA_DIR_TREE,               HEXA_TREE_ROLE );
  _vectorDirItem->setData( VECTOR_DIR_TREE,           HEXA_TREE_ROLE );
  _cylinderDirItem->setData( CYLINDER_DIR_TREE,       HEXA_TREE_ROLE );
  _pipeDirItem->setData( PIPE_DIR_TREE,                   HEXA_TREE_ROLE );
  _elementsDirItem->setData( ELEMENTS_DIR_TREE,           HEXA_TREE_ROLE );
  _crossElementsDirItem->setData( CROSSELEMENTS_DIR_TREE, HEXA_TREE_ROLE );

  parentItem->appendRow(_vertexDirItem);
  parentItem->appendRow(_edgeDirItem);
  parentItem->appendRow(_quadDirItem);
  parentItem->appendRow(_hexaDirItem);
  parentItem->appendRow(_vectorDirItem);
  parentItem->appendRow(_cylinderDirItem);
  parentItem->appendRow(_pipeDirItem);
  parentItem->appendRow(_elementsDirItem);
  parentItem->appendRow(_crossElementsDirItem);
}


void DocumentModel::clear()
{
  _vertexDirItem->removeRows( 0, _vertexDirItem->rowCount() );
  _edgeDirItem->removeRows(0, _edgeDirItem->rowCount() );
  _quadDirItem->removeRows(0, _quadDirItem->rowCount() );
  _hexaDirItem->removeRows(0, _hexaDirItem->rowCount() );
  _vectorDirItem->removeRows(0, _vectorDirItem->rowCount() );
  _cylinderDirItem->removeRows(0, _cylinderDirItem->rowCount() );
  _pipeDirItem->removeRows(0, _pipeDirItem->rowCount() );
  _elementsDirItem->removeRows(0, _elementsDirItem->rowCount() );
  _crossElementsDirItem->removeRows(0, _crossElementsDirItem->rowCount() );
}


void DocumentModel::load( const QString& xmlFileName ) // Fill Data
{
  QString tmp = xmlFileName.section('.', 0, 0); //CS_BP bof
  _hexaDocument->setFile( tmp.toLocal8Bit().constData() );
  _hexaDocument->loadXml();
  std::cout << "DocumentModel::load ->" << tmp.toStdString() << std::endl;
  clear();


  // DATA
  HEXA_NS::Vertex *v     = NULL;
  VertexItem      *vItem = NULL;
  for ( int i=0; i<_hexaDocument->countVertex(); ++i ){
    v = _hexaDocument->getVertex(i);
    vItem = new VertexItem(v);
    _vertexDirItem->appendRow(vItem);
//     _vertexDirItem->setEditable( false );//CS_TEST
  }

  HEXA_NS::Edge *e     = NULL;
  EdgeItem      *eItem = NULL;
  for ( int i=0; i<_hexaDocument->countEdge(); ++i ){
    e = _hexaDocument->getEdge(i);
    eItem = new EdgeItem(e);
    _edgeDirItem->appendRow(eItem);
  }

  HEXA_NS::Quad *q     = NULL;
  QuadItem      *qItem = NULL;
  for ( int i=0; i<_hexaDocument->countQuad(); ++i ){
    q = _hexaDocument->getQuad(i);
    qItem = new QuadItem(q);
    _quadDirItem->appendRow(qItem);
  }

  HEXA_NS::Hexa *h     = NULL;
  HexaItem      *hItem = NULL;
  for ( int i=0; i<_hexaDocument->countHexa(); ++i ){
    h = _hexaDocument->getHexa(i);
    hItem = new HexaItem(h);
    _hexaDirItem->appendRow(hItem);
  }

  // BUILDER, ASSOCIATION, GROUPS, ... CS_TODO

}


QModelIndex DocumentModel::addVertex( double x, double y, double z )
{
  QModelIndex vertexIndex;

  HEXA_NS::Vertex* hv = _hexaDocument->addVertex(x, y, z);
  VertexItem* v = new VertexItem(hv);
  _vertexDirItem->appendRow(v);
  vertexIndex = v->index();

  return vertexIndex;
}


QModelIndex DocumentModel::addQuad( const QModelIndex &i_v0, const QModelIndex &i_v1,
                                    const QModelIndex &i_v2, const QModelIndex &i_v3 )
{ //CS_TODO : gestion erreur
  QModelIndex quadIndex;

  QStandardItem *item_v0 = itemFromIndex(i_v0);
  QStandardItem *item_v1 = itemFromIndex(i_v1);
  QStandardItem *item_v2 = itemFromIndex(i_v2);
  QStandardItem *item_v3 = itemFromIndex(i_v3);

//   HASSERT( item_v0->type() == item_v1->type() 
//         == item_v2->type() == item_v3->type()
//         == VERTEXITEM );

  HEXA_NS::Vertex* hv0 = data(i_v0, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hv1 = data(i_v1, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hv2 = data(i_v2, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hv3 = data(i_v3, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();

  HEXA_NS::Quad* hq = _hexaDocument->addQuadVertices( hv0, hv1, hv2, hv3 );

  QuadItem* q = new QuadItem(hq);
  _quadDirItem->appendRow(q);
  quadIndex = q->index();

  return quadIndex;

}



/*****************************************************************
                      PatternDataModel
*****************************************************************/

PatternDataModel::PatternDataModel( QObject * parent ) :
  QSortFilterProxyModel( parent ),
  vertexItemFlags( Qt::NoItemFlags ),
  edgeItemFlags( Qt::NoItemFlags ),
  quadItemFlags( Qt::NoItemFlags ),
  hexaItemFlags( Qt::NoItemFlags )
{
  QString dataRegExp = QString("(%1|%2|%3|%4|%5|%6|%7|%8)").
  arg(VERTEX_TREE).arg(EDGE_TREE).arg(QUAD_TREE).arg(HEXA_TREE).                     arg(VERTEX_DIR_TREE).arg(EDGE_DIR_TREE).arg(QUAD_DIR_TREE).arg(HEXA_DIR_TREE);

  setFilterRole(HEXA_TREE_ROLE);
  setFilterRegExp ( QRegExp(dataRegExp) );
}

PatternDataModel::~PatternDataModel()
{
}

Qt::ItemFlags PatternDataModel::flags(const QModelIndex &index) const
{
  Qt::ItemFlags flags;

  if (!index.isValid()){
      std::cout<<"!index.isValid()"<<std::endl;
      return Qt::ItemIsEnabled;
  }

  QStandardItem *item = itemFromIndex ( index );

  if ( (item->type() == VERTEXITEM) && (vertexItemFlags != Qt::NoItemFlags) ){
    flags = vertexItemFlags;
  } else if ( (item->type() == EDGEITEM) && (edgeItemFlags != Qt::NoItemFlags) ){
    flags = edgeItemFlags;
  } else if ( (item->type() == QUADITEM) && (quadItemFlags != Qt::NoItemFlags) ){
    flags = quadItemFlags;
  } else if ( (item->type() == HEXAITEM) && (hexaItemFlags != Qt::NoItemFlags) ){
    flags = hexaItemFlags;
  } else {
    flags = item->flags();
  }
  return flags;
}


QVariant PatternDataModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
  if ( section == 0 and orientation == Qt::Horizontal and role == Qt::DisplayRole ){
    return QVariant( "Data" );
  } else {
    return QSortFilterProxyModel::headerData ( section, orientation, role );
  }
}

QModelIndex PatternDataModel::addVertex( double x, double y, double z )
{
  QModelIndex vertexIndex;
  DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
  if ( m != NULL ){
    QModelIndex sourceIndex = m->addVertex( x, y, z );
    vertexIndex = mapFromSource(sourceIndex);
  }
  return vertexIndex;
}

QModelIndex PatternDataModel::addQuad( const QModelIndex &i_v0, const QModelIndex &i_v1,
                                       const QModelIndex &i_v2, const QModelIndex &i_v3 )
{
  QModelIndex quadIndex;
  DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
  if ( m != NULL ){
    QModelIndex sourceIndex = m->addQuad( mapToSource(i_v0), mapToSource(i_v1),
                                          mapToSource(i_v2), mapToSource(i_v3) );
    quadIndex = mapFromSource(sourceIndex);
  }
  return quadIndex;
}

QStandardItem* PatternDataModel::itemFromIndex ( const QModelIndex & index ) const
{
  QStandardItem *item = NULL;
  DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
  if ( m != NULL ){
    item = m->itemFromIndex( mapToSource(index) );
  }
  return item;
}



PatternBuilderModel::PatternBuilderModel( QObject * parent ) :
  QSortFilterProxyModel( parent )
{
  setFilterRole(HEXA_TREE_ROLE);
  QString builderRegExp =QString("(%1|%2|%3|%4|%5|%6|%7|%8|%9|%10)").
    arg(VECTOR_TREE).arg(CYLINDER_TREE).arg(PIPE_TREE).arg(ELEMENTS_TREE).arg(CROSSELEMENTS_TREE).arg(VECTOR_DIR_TREE).arg(CYLINDER_DIR_TREE).arg(PIPE_DIR_TREE).arg(ELEMENTS_DIR_TREE).arg(CROSSELEMENTS_DIR_TREE);

  setFilterRole( HEXA_TREE_ROLE );
  setFilterRegExp ( QRegExp(builderRegExp ) ); 
}


PatternBuilderModel::~PatternBuilderModel() 
{
}


QVariant PatternBuilderModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
  if ( section == 0 and orientation == Qt::Horizontal and role == Qt::DisplayRole ){
      return QVariant( "Builder" );
  } else {
      return QSortFilterProxyModel::headerData ( section, orientation, role );
  }
}
