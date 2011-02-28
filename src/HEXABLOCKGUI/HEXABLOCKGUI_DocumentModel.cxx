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

void DocumentModel::updateView(VertexItem* vItem)
{
    std::cout << "DocumentModel::updateView()" << std::endl;
    // mise à jour du vertex
    HEXA_NS::Vertex* vertex = vItem->data(HEXA_DATA_ROLE).value<HEXA_NS::Vertex*>();
    vItem->setData(QVariant::fromValue(vertex), HEXA_DATA_ROLE);

    // recherche des edges associes :
    if (vertex->hasParents())
    {
        // ie : le vertex a servi pour la creation
        // d'un ou plusieurs edges, etc...
        ostringstream oss;
        oss << vertex->getNbrParents();
        std::cout << "nombre de parents : " << oss.str() << std::endl;
      
        for (int ne = 0; ne < vertex->getNbrParents(); ne++)
        {
            HEXA_NS::Edge* edge = vertex->getParent(ne);

            char pName[12];
            QString name = edge->getName(pName);
            std::cout << "nom edge = " << name.toStdString() << std::endl;

            ostringstream oss1;
            oss1 << _edgeDirItem->rowCount();
            std::cout << "nombre d'edges trouves : " << oss1.str() << std::endl;


            // mise à jour des edges :
            QStandardItem* item = NULL;
            bool item_found = false;
            int ind = 0;
            while (!item_found && ind <  _edgeDirItem->rowCount())
            {
                item = _edgeDirItem->child(ind, 0);
                if(item->text() == name)
                {
                    item_found = true;
                    item->setData(QVariant::fromValue(edge), HEXA_DATA_ROLE);
                }
                else
                {
                    ind++;
                }
            }

            // recherche des quads associes
            if (edge->hasParents())
            {
                // les parents sont des quads
                for (int nq = 0; nq < edge->getNbrParents(); nq++)
                {
                    HEXA_NS::Quad* quad = edge->getParent(nq);

                    char pName[12];
                    QString name = quad->getName(pName);
                    // mise à jour des quads :
                    QStandardItem* item = NULL;
                    bool item_found = false;
                    int ind = 0;
                    while (!item_found && ind <  _quadDirItem->rowCount())
                    {
                        item = _quadDirItem->child(ind, 0);
                        if(item->text() == name)
                        {
                            item_found = true;
                            item->setData(QVariant::fromValue(quad), HEXA_DATA_ROLE);
                        }
                        else
                        {
                            ind++;
                        }
                    }

                    if (quad->hasParents())
                    {
                        // les parents sont des hexas
                        for (int nh = 0; nh < quad->getNbrParents(); nh++)
                        {
                            HEXA_NS::Hexa* hexa = quad->getParent(nh);

                            char pName[12];
                            QString name = hexa->getName(pName);
                            // mise à jour des hexas :
                            QStandardItem* item = NULL;
                            bool item_found = false;
                            int ind = 0;
                            while (!item_found && ind <  _hexaDirItem->rowCount())
                            {
                                item = _hexaDirItem->child(ind, 0);
                                if(item->text() == name)
                                {
                                    item_found = true;
                                    item->setData(QVariant::fromValue(hexa), HEXA_DATA_ROLE);
                                }
                                else
                                {
                                    ind++;
                                }
                            }

                        }
                        
                    }

                }
            }   
        }
    }
}

QModelIndex DocumentModel::addQuad( const QModelIndex &i_v0, const QModelIndex &i_v1,
                                    const QModelIndex &i_v2, const QModelIndex &i_v3 )
{ //CS_TODO : gestion erreur
  QModelIndex quadIndex;

//   QStandardItem *item_v0 = itemFromIndex(i_v0);
//   QStandardItem *item_v1 = itemFromIndex(i_v1);
//   QStandardItem *item_v2 = itemFromIndex(i_v2);
//   QStandardItem *item_v3 = itemFromIndex(i_v3);

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



QModelIndex DocumentModel::addHexaFromQuad( const QModelIndex &i_q0, const QModelIndex &i_q1, const QModelIndex &i_q2,
                                            const QModelIndex &i_q3, const QModelIndex &i_q4, const QModelIndex &i_q5 )
{ //CS_TODO : gestion erreur
  QModelIndex hexaIndex;

//   QStandardItem *item_v0 = itemFromIndex(i_q0);
//   QStandardItem *item_v1 = itemFromIndex(i_q1);
//   QStandardItem *item_v2 = itemFromIndex(i_q2);
//   QStandardItem *item_v3 = itemFromIndex(i_q3);
//   QStandardItem *item_v4 = itemFromIndex(i_q4);
//   QStandardItem *item_v5 = itemFromIndex(i_q5);
  HEXA_NS::Quad* hq0 = data(i_q0, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();
  HEXA_NS::Quad* hq1 = data(i_q1, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();
  HEXA_NS::Quad* hq2 = data(i_q2, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();
  HEXA_NS::Quad* hq3 = data(i_q3, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();
  HEXA_NS::Quad* hq4 = data(i_q4, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();
  HEXA_NS::Quad* hq5 = data(i_q5, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();


  HEXA_NS::Hexa* hh = _hexaDocument->addHexa( hq0, hq1, hq2, hq3, hq4, hq5 );

  if ( hh->isValid() ){
    HexaItem* h = new HexaItem(hh);
    _hexaDirItem->appendRow(h);
    hexaIndex = h->index();
  } else {
    delete hh;
  }

  return hexaIndex;
}



bool DocumentModel::mergeVertices( const QModelIndex &i_v0, const QModelIndex &i_v1 ) //CS_TODO : impact sur le model?
{
  bool ret = false;

  HEXA_NS::Vertex* hv0 = data(i_v0, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hv1 = data(i_v1, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();

  int r = _hexaDocument->mergeVertices( hv0, hv1 );
  if ( r == HOK ){
    std::cout << "DocumentModel:: mergeVertices => OK " << std::endl;
    ret = true;
  } else if ( r == HERR ){
    std::cout << "DocumentModel:: mergeVertices => KO " << std::endl;
    ret = false;
  }

  return ret;
}


bool DocumentModel::mergeEdges( const QModelIndex &i_e0, const QModelIndex &i_e1,
                                const QModelIndex &i_v0, const QModelIndex &i_v1 )
//CS_TODO : impact sur le model?
{
  bool ret = false;

  HEXA_NS::Edge* he0 = data(i_e0, HEXA_DATA_ROLE).value<HEXA_NS::Edge *>();
  HEXA_NS::Edge* he1 = data(i_e1, HEXA_DATA_ROLE).value<HEXA_NS::Edge *>();

  HEXA_NS::Vertex* hv0 = data(i_v0, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hv1 = data(i_v1, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();

  int r = _hexaDocument->mergeEdges( he0, he1, hv0, hv1 ); 
  if ( r == HOK ){
    ret = true;
  } else if ( r == HERR ){
    ret = false;
  }

  return ret;

}


QModelIndex DocumentModel::cutEdge( const QModelIndex &i_e0, int nbcuts )
//CS_TODO : impact sur le model?
{
  QModelIndex eltsIndex;

  HEXA_NS::Edge* he0 = data(i_e0, HEXA_DATA_ROLE).value<HEXA_NS::Edge *>();

  HEXA_NS::Elements* helts = _hexaDocument->cut( he0, nbcuts );

  if ( helts->isValid() ){
    ElementsItem* elts = new ElementsItem(helts);
    _elementsDirItem->appendRow(elts);
    eltsIndex = elts->index();
  } else {
    delete helts;
  }

  return eltsIndex;
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

void PatternDataModel::updateVertex(const QModelIndex& i_v)
{
    std::cout << "PatternDataModel::updateVertex()" << std::endl;
    VertexItem *item_v = dynamic_cast<VertexItem*>(itemFromIndex(i_v));
    DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );

    if (item_v != NULL)
    {
        std::cout << "updateVertex() : OK" << std::endl;
        m->updateView(item_v);

    }
    else
    {
        std::cout << "updateVertex() : NOT OK" << std::endl;

    }
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



QModelIndex PatternDataModel::addHexaFromQuad( const QModelIndex &i_q0, const QModelIndex &i_q1, const QModelIndex &i_q2,
                                               const QModelIndex &i_q3, const QModelIndex &i_q4, const QModelIndex &i_q5 )
{
  QModelIndex hexaIndex;
  DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
  if ( m != NULL ){
    QModelIndex sourceIndex = m->addHexaFromQuad( mapToSource(i_q0), mapToSource(i_q1), mapToSource(i_q2),
                                                  mapToSource(i_q3), mapToSource(i_q4), mapToSource(i_q5) );
    hexaIndex = mapFromSource(sourceIndex);
  }
  return hexaIndex;
}






bool PatternDataModel::mergeVertices( const QModelIndex &i_v0, const QModelIndex &i_v1 )
{
  bool ret = false;
  DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
  if ( m != NULL ){
    ret = m->mergeVertices( mapToSource(i_v0), mapToSource(i_v1) );
  }
  return ret;
}

bool PatternDataModel::mergeEdges( const QModelIndex &i_e0, const QModelIndex &i_e1,
                                   const QModelIndex &i_v0, const QModelIndex &i_v1 )
{
  bool ret = false;
  DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
  if ( m != NULL ){
    ret = m->mergeEdges( mapToSource(i_e0), mapToSource(i_e1),
                         mapToSource(i_v0), mapToSource(i_v1) );
  }
  return ret;
}

QModelIndex PatternDataModel::cutEdge( const QModelIndex &i_e0, int nbcuts )
{
  QModelIndex elementsIndex;
  DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
  if ( m != NULL ){
    QModelIndex sourceElementsIndex = m->cutEdge( mapToSource(i_e0), nbcuts );
    elementsIndex = mapFromSource(sourceElementsIndex);
  }
  return elementsIndex;
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
