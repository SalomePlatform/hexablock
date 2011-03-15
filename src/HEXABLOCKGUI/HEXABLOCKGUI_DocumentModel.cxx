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

#include "HEXABLOCKGUI.hxx"
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
  _crossElementsDirItem( new QStandardItem("CrossElements") ),
  _vertexItemFlags( Qt::NoItemFlags ),
  _edgeItemFlags( Qt::NoItemFlags ),
  _quadItemFlags( Qt::NoItemFlags ),
  _hexaItemFlags( Qt::NoItemFlags ),
  _vectorItemFlags( Qt::NoItemFlags ),
  _elementsItemFlags( Qt::NoItemFlags )
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

DocumentModel::~DocumentModel()
{
}


void DocumentModel::load( const QString& xmlFileName ) // Fill Data
{
  QString tmp = xmlFileName.section('.', 0, 0); //CS_BP bof
  _hexaDocument->setFile( tmp.toLocal8Bit().constData() );
  _hexaDocument->loadXml();
  std::cout << "DocumentModel::load ->" << tmp.toStdString() << std::endl;
  clearAll();

  fillData();

  // BUILDER, ASSOCIATION, GROUPS, ... CS_TODO _fillBuilderFrom( _hexaDocument );
}

void DocumentModel::updateData()
{
  std::cout<<"DocumentModel::updateData()  begin"<<std::endl;
  clearData();
  fillData();
  std::cout<<"DocumentModel::updateData()  end"<<std::endl;
}

void DocumentModel::clearAll() 
{
  clearData();
  clearBuilder();
  //CS_TODO : todo : association, groups, mesh
}


void DocumentModel::clearData()
{
  _vertexDirItem->removeRows( 0, _vertexDirItem->rowCount() );
  _edgeDirItem->removeRows(0, _edgeDirItem->rowCount() );
  _quadDirItem->removeRows(0, _quadDirItem->rowCount() );
  _hexaDirItem->removeRows(0, _hexaDirItem->rowCount() );
}


void DocumentModel::clearBuilder()
{
  _vectorDirItem->removeRows(0, _vectorDirItem->rowCount() );
  _cylinderDirItem->removeRows(0, _cylinderDirItem->rowCount() );
  _pipeDirItem->removeRows(0, _pipeDirItem->rowCount() );
  _elementsDirItem->removeRows(0, _elementsDirItem->rowCount() );
  _crossElementsDirItem->removeRows(0, _crossElementsDirItem->rowCount() );
}


void DocumentModel::fillData()
{
  std::cout<<"DocumentModel::fillData()  begin"<<std::endl;
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
  std::cout<<"DocumentModel::fillData()  end"<<std::endl;

}


Qt::ItemFlags DocumentModel::flags(const QModelIndex &index) const
{
//   std::cout<<"DocumentModel::flags()"<<std::endl;
  Qt::ItemFlags flags;

  if (!index.isValid()){
      std::cout<<"!index.isValid()"<<std::endl;
      return Qt::ItemIsEnabled;
  }

  QStandardItem *item = itemFromIndex ( index );

  if ( (item->type() == VERTEXITEM) && (_vertexItemFlags != Qt::NoItemFlags) ){
    flags = _vertexItemFlags;
//     std::cout<<"_vertexItemFlags"<<_vertexItemFlags<<std::endl;
  } else if ( (item->type() == EDGEITEM) && (_edgeItemFlags != Qt::NoItemFlags) ){
    flags = _edgeItemFlags;
//     std::cout<<"_edgeItemFlags"<<_edgeItemFlags<<std::endl;
  } else if ( (item->type() == QUADITEM) && (_quadItemFlags != Qt::NoItemFlags) ){
    flags = _quadItemFlags;
//     std::cout<<"_quadItemFlags"<<_quadItemFlags<<std::endl;
  } else if ( (item->type() == HEXAITEM) && (_hexaItemFlags != Qt::NoItemFlags) ){
    flags = _hexaItemFlags;
//     std::cout<<"_hexaItemFlags"<<_hexaItemFlags<<std::endl;
  } else if ( (item->type() == VECTORITEM) && (_vectorItemFlags != Qt::NoItemFlags) ){
    flags = _vectorItemFlags;
//     std::cout<<"_vectorItemFlags"<<_vectorItemFlags<<std::endl;
  } else if ( (item->type() == ELEMENTSITEM) && (_elementsItemFlags != Qt::NoItemFlags) ){
    flags = _elementsItemFlags;
//     std::cout<<"_elementsItemFlags"<<_elementsItemFlags<<std::endl;
  } else {
    flags = item->flags();
  }
  return flags;
}


void DocumentModel::allowAllSelection()
{
  _vertexItemFlags = Qt::NoItemFlags;
  _edgeItemFlags   = Qt::NoItemFlags;
  _quadItemFlags   = Qt::NoItemFlags;
  _hexaItemFlags   = Qt::NoItemFlags;

  _vectorItemFlags   = Qt::NoItemFlags;
  _elementsItemFlags = Qt::NoItemFlags;
}


void DocumentModel::allowVertexSelectionOnly()
{
    std::cout << "allowVertexSelectionOnly() allowVertexSelectionOnly() allowVertexSelectionOnly() "<< std::endl;
    _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable);
    _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _quadItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _hexaItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );

    _vectorItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
    _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}

void DocumentModel::allowEdgeSelectionOnly()
{
  std::cout << "allowEdgeSelectionOnly() allowEdgeSelectionOnly() allowEdgeSelectionOnly() "<< std::endl;
  _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );
  _quadItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _hexaItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _vectorItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}

void DocumentModel::allowQuadSelectionOnly()
{
  std::cout << "allowQuadSelectionOnly() allowQuadSelectionOnly() allowQuadSelectionOnly() "<< std::endl;
  _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _quadItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );
  _hexaItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _vectorItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

}


void DocumentModel::allowVectorSelectionOnly()
{
  std::cout << "allowVectorSelectionOnly() allowVectorSelectionOnly() allowVectorSelectionOnly() "<< std::endl;
  _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _quadItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _hexaItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _vectorItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );
  _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}


void DocumentModel::allowElementsSelectionOnly()
{
  std::cout << "allowElementsSelectionOnly() allowElementsSelectionOnly() allowElementsSelectionOnly() "<< std::endl;
  _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _quadItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _hexaItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _vectorItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );
}




QModelIndex DocumentModel::addVertex( double x, double y, double z )
{
  QModelIndex vertexIndex;

  HEXA_NS::Vertex* hv = _hexaDocument->addVertex(x, y, z);

  if ( hv->isValid() ){
    VertexItem* v = new VertexItem(hv);
    _vertexDirItem->appendRow(v);
    vertexIndex = v->index();
  } else {
    delete hv;
  }

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

  if ( hq->isValid() ){
    QuadItem* q = new QuadItem(hq);
    _quadDirItem->appendRow(q);
    quadIndex = q->index();
    QString tmp = "/tmp/addQuad.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hq;
  }
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


// Vector addVector( in double dx, in double dy, in double dz )
//         raises (SALOME::SALOME_Exception);
QModelIndex DocumentModel::addVector( double dx, double dy, double dz )
{
  std::cout << "DocumentModel::addVector" << std::endl;
  QModelIndex vectorIndex;

  HEXA_NS::Vector* hv = _hexaDocument->addVector(dx, dy, dz);

  if ( hv->isValid() ){
    std::cout << "hv->isValid()" << std::endl;
    VectorItem* v = new VectorItem(hv);
    _vectorDirItem->appendRow(v);
    vectorIndex = v->index();
//     QStandardItem *toto = new QStandardItem("Toto");
//     std::cout << "before" << std::endl;
//     _vectorDirItem->appendRow(toto);
//     std::cout << "after" << std::endl;
//     vectorIndex = toto->index();
    QString tmp = "/tmp/addVector.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );

  } else {
    std::cout << "delete hv" << std::endl;
    delete hv;
  }

  return vectorIndex;
}




//      Elements makeCartesian( in Vertex pt, 
//             in Vector vx, in Vector vy, in Vector vz,
//             in long nx, in long ny, in long nz)
//         raises (SALOME::SALOME_Exception);
QModelIndex DocumentModel::makeCartesian( const QModelIndex& i_pt,
      const QModelIndex& i_vec_x, const QModelIndex& i_vec_y, const QModelIndex& i_vec_z,
      long nx, long ny, long nz)
{
  QModelIndex eltsIndex;
  std::cout<<"makeCartesian begin"<<std::endl; 

  HEXA_NS::Vertex* hpt    = data(i_pt,    HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vector* hvec_x = data(i_vec_x, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();
  HEXA_NS::Vector* hvec_y = data(i_vec_y, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();
  HEXA_NS::Vector* hvec_z = data(i_vec_z, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();


  std::cout<<"hpt "<<hpt<<std::endl; 
  std::cout<<"hvec_x "<<hvec_x<<std::endl; 
  std::cout<<"hvec_y "<<hvec_y<<std::endl; 
  std::cout<<"hvec_z "<<hvec_z<<std::endl; 
  

  HEXA_NS::Elements* new_helts = _hexaDocument->makeCartesian( hpt,
                                                               hvec_x, hvec_y, hvec_z,
                                                               nx, ny, nz );

  if ( new_helts->isValid() ){
    std::cout<<"makeCartesian OK!!!"<<std::endl; 
    ElementsItem* eltsItem = new ElementsItem(new_helts);
    _elementsDirItem->appendRow(eltsItem);
    eltsIndex = eltsItem->index();
    updateData(); //CS_TODO more or less?

    QString tmp = "/tmp/makeCartesian.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    std::cout<<"makeCartesian KO!!!"<<std::endl; 
    delete new_helts;
  }

  return eltsIndex;
}

// Elements makeCylindrical( in Vertex pt,
//           in Vector vex, in Vector vez,
//           in double dr, in double da, in double dl,
//           in long nr, in long na, in long nl,
//           in boolean fill )
//         raises (SALOME::SALOME_Exception);
QModelIndex DocumentModel::makeCylindrical( const QModelIndex& i_pt,
        const QModelIndex& i_vec_x, const QModelIndex& i_vec_z,
        double dr, double da, double dl,
        long nr, long na, long nl,
        bool fill )
{

  QModelIndex eltsIndex;

  HEXA_NS::Vertex* hpt = data(i_pt, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vector* hvec_x = data(i_vec_x, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();
  HEXA_NS::Vector* hvec_z = data(i_vec_z, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

  HEXA_NS::Elements* new_helts = _hexaDocument->makeCylindrical( hpt, hvec_x, hvec_z, dr, da, dl, nr, na, nl, fill );

  if ( new_helts->isValid() ){
    ElementsItem* eltsItem = new ElementsItem(new_helts);
    _elementsDirItem->appendRow(eltsItem);
    eltsIndex = eltsItem->index();
    updateData(); //CS_TODO  more or less?

    QString tmp = "/tmp/makeCylindrical.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete new_helts;
  }

  return eltsIndex;
}


// Elements makeTranslation( in Elements l, in Vector vec )
//         raises (SALOME::SALOME_Exception);
QModelIndex DocumentModel::makeTranslation( const QModelIndex& i_elts, const QModelIndex& i_vec )
{
  QModelIndex eltsIndex;

  HEXA_NS::Elements* helts = data(i_elts, HEXA_DATA_ROLE).value<HEXA_NS::Elements *>();
  HEXA_NS::Vector*    hvec = data(i_vec, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

  HEXA_NS::Elements* new_helts = _hexaDocument->makeTranslation( helts, hvec );

  if ( new_helts->isValid() ){
    ElementsItem* eltsItem = new ElementsItem(new_helts);
    _elementsDirItem->appendRow(eltsItem);
    eltsIndex = eltsItem->index();
    updateData(); //CS_TODO  more or less?

    QString tmp = "/tmp/makeTranslation.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete new_helts;
  }

  return eltsIndex;
}


bool DocumentModel::mergeVertices( const QModelIndex &i_v0, const QModelIndex &i_v1 ) //CS_TODO : impact sur le model?
{
  bool ret = false;

  HEXA_NS::Vertex* hv0 = data(i_v0, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hv1 = data(i_v1, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();

  int r = _hexaDocument->mergeVertices( hv0, hv1 );
  if ( r == HOK ){
    std::cout << "DocumentModel:: mergeVertices => OK " << std::endl;
    updateData();
    QString tmp = "/tmp/mergeVertices.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
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
    updateData();
    ret = true;
    QString tmp = "/tmp/mergeEdges.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
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

    updateData(); //CS_TODO more?

    QString tmp = "/tmp/cutEdge.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );

  } else {
    delete helts;
  }

  return eltsIndex;
}













/*****************************************************************
                      PatternDataModel
*****************************************************************/

PatternDataModel::PatternDataModel( QObject * parent ) :
  QSortFilterProxyModel( parent )
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
//   std::cout<<"PatternDataModel::flags()"<<std::endl;
  Qt::ItemFlags flags;

  DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
  if ( m != NULL ){
    flags = m->flags( mapToSource(index) );
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

// QModelIndex PatternDataModel::addVertex( double x, double y, double z )
// {
//   QModelIndex vertexIndex;
//   DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
//   if ( m != NULL ){
//     QModelIndex sourceIndex = m->addVertex( x, y, z );
//     vertexIndex = mapFromSource(sourceIndex);
//   }
//   return vertexIndex;
// }
// 
// void PatternDataModel::updateVertex(const QModelIndex& i_v)
// {
//     std::cout << "PatternDataModel::updateVertex()" << std::endl;
//     VertexItem *item_v = dynamic_cast<VertexItem*>(itemFromIndex(i_v));
//     DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
// 
//     if (item_v != NULL)
//     {
//         std::cout << "updateVertex() : OK" << std::endl;
//         m->updateView(item_v);
// 
//     }
//     else
//     {
//         std::cout << "updateVertex() : NOT OK" << std::endl;
// 
//     }
// }
// 
// 
// QModelIndex PatternDataModel::addQuad( const QModelIndex &i_v0, const QModelIndex &i_v1,
//                                        const QModelIndex &i_v2, const QModelIndex &i_v3 )
// {
//   QModelIndex quadIndex;
//   DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
//   if ( m != NULL ){
//     QModelIndex sourceIndex = m->addQuad( mapToSource(i_v0), mapToSource(i_v1),
//                                           mapToSource(i_v2), mapToSource(i_v3) );
//     quadIndex = mapFromSource(sourceIndex);
//   }
//   return quadIndex;
// }
// 
// 
// 
// QModelIndex PatternDataModel::addHexaFromQuad( const QModelIndex &i_q0, const QModelIndex &i_q1, const QModelIndex &i_q2,
//                                                const QModelIndex &i_q3, const QModelIndex &i_q4, const QModelIndex &i_q5 )
// {
//   QModelIndex hexaIndex;
//   DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
//   if ( m != NULL ){
//     QModelIndex sourceIndex = m->addHexaFromQuad( mapToSource(i_q0), mapToSource(i_q1), mapToSource(i_q2),
//                                                   mapToSource(i_q3), mapToSource(i_q4), mapToSource(i_q5) );
//     hexaIndex = mapFromSource(sourceIndex);
//   }
//   return hexaIndex;
// }
// 
// 
// 
// 
// 
// 
// bool PatternDataModel::mergeVertices( const QModelIndex &i_v0, const QModelIndex &i_v1 )
// {
//   bool ret = false;
//   DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
//   if ( m != NULL ){
//     ret = m->mergeVertices( mapToSource(i_v0), mapToSource(i_v1) );
//   }
//   return ret;
// }
// 
// bool PatternDataModel::mergeEdges( const QModelIndex &i_e0, const QModelIndex &i_e1,
//                                    const QModelIndex &i_v0, const QModelIndex &i_v1 )
// {
//   bool ret = false;
//   DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
//   if ( m != NULL ){
//     ret = m->mergeEdges( mapToSource(i_e0), mapToSource(i_e1),
//                          mapToSource(i_v0), mapToSource(i_v1) );
//   }
//   return ret;
// }
// 
// QModelIndex PatternDataModel::cutEdge( const QModelIndex &i_e0, int nbcuts )
// {
//   QModelIndex elementsIndex;
//   DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
//   if ( m != NULL ){
//     QModelIndex sourceElementsIndex = m->cutEdge( mapToSource(i_e0), nbcuts );
//     std::cout << " sourceElementsIndex.isValid() => " << sourceElementsIndex.isValid() << std::endl;
//     elementsIndex = mapFromSource(sourceElementsIndex);
//     std::cout << " elementsIndex.isValid() => " << elementsIndex.isValid() << std::endl;
//   }
//   return elementsIndex;
// }


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
  QString builderRegExp =QString("(%1|%2|%3|%4|%5|%6|%7|%8|%9|%10)").
    arg(VECTOR_TREE).arg(CYLINDER_TREE).arg(PIPE_TREE).arg(ELEMENTS_TREE).arg(CROSSELEMENTS_TREE).arg(VECTOR_DIR_TREE).arg(CYLINDER_DIR_TREE).arg(PIPE_DIR_TREE).arg(ELEMENTS_DIR_TREE).arg(CROSSELEMENTS_DIR_TREE);

  setFilterRole( HEXA_TREE_ROLE );
  setFilterRegExp ( QRegExp(builderRegExp ) ); 
}


PatternBuilderModel::~PatternBuilderModel() 
{
}



Qt::ItemFlags PatternBuilderModel::flags(const QModelIndex &index) const
{
//   std::cout<<"PatternBuilderModel::flags()"<<std::endl;
  Qt::ItemFlags flags;

  DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
  if ( m != NULL ){
    flags = m->flags( mapToSource(index) );
  }
  return flags;
}


QVariant PatternBuilderModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
  if ( section == 0 and orientation == Qt::Horizontal and role == Qt::DisplayRole ){
      return QVariant( "Builder" );
  } else {
      return QSortFilterProxyModel::headerData ( section, orientation, role );
  }
}


QStandardItem* PatternBuilderModel::itemFromIndex ( const QModelIndex & index ) const
{
  QStandardItem *item = NULL;
  DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
  if ( m != NULL ){
    item = m->itemFromIndex( mapToSource(index) );
  }
  return item;
}


// QModelIndex PatternBuilderModel::addVector( double dx, double dy, double dz )
// {
//   std::cout << "PatternBuilderModel::addVector" << std::endl;
//   QModelIndex vectorIndex;
// 
//   DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
//   if ( m != NULL ){
//     std::cout << "PatternBuilderModel::addVector m != NULL " << std::endl;
//     QModelIndex sourceIndex = m->addVector( dx, dy, dz );
//     vectorIndex = mapFromSource(sourceIndex);
//   }
//   return vectorIndex;
// }
// 
// 
// QModelIndex PatternBuilderModel::makeCartesian( const QModelIndex& i_pt,
//                                    const QModelIndex& i_vec_x, const QModelIndex& i_vec_y, const QModelIndex& i_vec_z,
//                                    long nx, long ny, long nz)
// {
//   std::cout << " PatternBuilderModel::makeCartesian()"<< std::endl;
//   QModelIndex elementsIndex;
//   DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
//   if ( m != NULL ){
//     QModelIndex sourceElementsIndex = m->makeCartesian( 
//                     mapToSource(i_pt),
//                     mapToSource(i_vec_x), mapToSource(i_vec_y), mapToSource(i_vec_z),
//                     nx, ny, nz );
//     std::cout << " sourceElementsIndex.isValid() => " << sourceElementsIndex.isValid() << std::endl;
//     elementsIndex = mapFromSource(sourceElementsIndex);
//     std::cout << " elementsIndex.isValid() => " << elementsIndex.isValid() << std::endl;
//   }
//   return elementsIndex;
// }
