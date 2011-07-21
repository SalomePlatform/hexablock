//  Copyright (C) 2009-2011  CEA/DEN, EDF R&D
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

//CS_TODO: relever les fonctions qui nécessitent updateData().
//        addGroupElement à tester

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
// DocumentModel::DocumentModel(QObject * parent):
DocumentModel::DocumentModel(HEXA_NS::Document* docIn, const QString& entryIn, QObject * parent):
  QStandardItemModel(parent),
  _hexaFile( new QTemporaryFile() ),
  _hexaDocument( docIn ),
  _entry( entryIn ),
//   _hexaDocument(  new HEXA_NS::Document("/tmp/doc.hex") ), //CS_TODO

  _vertexDirItem( new QStandardItem("Vertex") ),
  _edgeDirItem(   new QStandardItem("Edge") ),
  _quadDirItem(   new QStandardItem("Quad") ),
  _hexaDirItem(   new QStandardItem("Hexa") ),

  _vectorDirItem(        new QStandardItem("Vector") ),
  _cylinderDirItem(      new QStandardItem("Cylinder") ),
  _pipeDirItem(          new QStandardItem("Pipe") ),
  _elementsDirItem(      new QStandardItem("Elements") ),
  _crossElementsDirItem( new QStandardItem("CrossElements") ),

  _groupDirItem( new QStandardItem("Group") ),

  _lawDirItem( new QStandardItem("Law") ),
  _propagationDirItem( new QStandardItem("Propagation") ),

  _vertexItemFlags( Qt::NoItemFlags ),
  _edgeItemFlags( Qt::NoItemFlags ),
  _quadItemFlags( Qt::NoItemFlags ),
  _hexaItemFlags( Qt::NoItemFlags ),
  _vectorItemFlags( Qt::NoItemFlags ),
  _cylinderItemFlags( Qt::NoItemFlags ),
  _pipeItemFlags( Qt::NoItemFlags ),
  _elementsItemFlags( Qt::NoItemFlags ),
  _crossElementsItemFlags( Qt::NoItemFlags ),
  _disallowEdition( false )
{
  if ( !_hexaDocument && _hexaFile->open() ){
    _hexaDocument =  new HEXA_NS::Document( _hexaFile->fileName().toLatin1() );
    _hexaDocument->reorderFaces();
  }

  QStandardItem *parentItem = invisibleRootItem();
//   parentItem->setData( QString::number( reinterpret_cast<intptr_t>(_hexaDocument) ), HEXA_ENTRY_ROLE );
  parentItem->setData( _entry, HEXA_ENTRY_ROLE );

  _vertexDirItem->setData( VERTEX_DIR_TREE,           HEXA_TREE_ROLE );
  _edgeDirItem->setData( EDGE_DIR_TREE,               HEXA_TREE_ROLE );
  _quadDirItem->setData( QUAD_DIR_TREE,               HEXA_TREE_ROLE );
  _hexaDirItem->setData( HEXA_DIR_TREE,               HEXA_TREE_ROLE );

  _vectorDirItem->setData( VECTOR_DIR_TREE,           HEXA_TREE_ROLE );
  _cylinderDirItem->setData( CYLINDER_DIR_TREE,       HEXA_TREE_ROLE );
  _pipeDirItem->setData( PIPE_DIR_TREE,                   HEXA_TREE_ROLE );
  _elementsDirItem->setData( ELEMENTS_DIR_TREE,           HEXA_TREE_ROLE );
  _crossElementsDirItem->setData( CROSSELEMENTS_DIR_TREE, HEXA_TREE_ROLE );

  _groupDirItem->setData( GROUP_DIR_TREE, HEXA_TREE_ROLE );

  //CS_TODO associations

  _lawDirItem->setData( LAW_DIR_TREE, HEXA_TREE_ROLE );
  _propagationDirItem->setData( PROPAGATION_DIR_TREE, HEXA_TREE_ROLE );


  parentItem->appendRow(_vertexDirItem);
  parentItem->appendRow(_edgeDirItem);
  parentItem->appendRow(_quadDirItem);
  parentItem->appendRow(_hexaDirItem);
  parentItem->appendRow(_vectorDirItem);
  parentItem->appendRow(_cylinderDirItem);
  parentItem->appendRow(_pipeDirItem);
  parentItem->appendRow(_elementsDirItem);
  parentItem->appendRow(_crossElementsDirItem);
  parentItem->appendRow(_groupDirItem);
  parentItem->appendRow(_lawDirItem);
  parentItem->appendRow(_propagationDirItem);

}

DocumentModel::~DocumentModel()
{
  delete _hexaDocument;
  delete _hexaFile;
}


void DocumentModel::load( const QString& xmlFileName ) // Fill Data
{
  QString tmp = xmlFileName.section('.', 0, 0); //CS_BP bof
  _hexaDocument->setFile( tmp.toLocal8Bit().constData() );
  _hexaDocument->loadXml();
  std::cout << "DocumentModel::load ->" << tmp.toStdString() << std::endl;
  clearAll();

  fillData();
  fillBuilder();
  fillAssociation();
  fillGroups();
  fillMesh();

  tmp = "/tmp/load.vtk";
  _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );

  emit patternDataChanged();

  // BUILDER, ASSOCIATION, GROUPS, ... CS_TODO _fillBuilderFrom( _hexaDocument );
}

void DocumentModel::updateData()
{
  std::cout<<"DocumentModel::updateData()  begin"<<std::endl;
  clearData();
  fillData();
  QString tmp = "/tmp/load.vtk";
  _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  emit patternDataChanged();
  std::cout<<"DocumentModel::updateData()  end"<<std::endl;
}

void DocumentModel::clearAll() 
{
  clearData();
  clearBuilder();
  clearAssociation();
  clearGroups();
  clearMesh();
  //dataChanged( const QModelIndex & topLeft, const QModelIndex & bottomRight )
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

void DocumentModel::clearAssociation() 
{
  //CS_TODO
}

void DocumentModel::clearGroups()
{
  _groupDirItem->removeRows(0, _groupDirItem->rowCount() );
}

void DocumentModel::clearMesh()
{
  _lawDirItem->removeRows(0, _lawDirItem->rowCount() );
  _propagationDirItem->removeRows(0, _propagationDirItem->rowCount() );
}


void DocumentModel::fillData()
{
  std::cout<<"DocumentModel::fillData()  begin"<<std::endl;
  // DATA
  HEXA_NS::Vertex *v     = NULL;
  VertexItem      *vItem = NULL;
  for ( int i=0; i<_hexaDocument->countVertex(); ++i ){
    v = _hexaDocument->getVertex(i);

    char pName[12];
    std::string name = v->getName(pName);
    std::cout<<name<<std::endl;

    vItem = new VertexItem(v);
    vItem->setData( _entry, HEXA_DOC_ENTRY_ROLE );
    _vertexDirItem->appendRow(vItem);
//     _vertexDirItem->setEditable( false );//CS_TEST
  }

  HEXA_NS::Edge *e     = NULL;
  EdgeItem      *eItem = NULL;
  for ( int i=0; i<_hexaDocument->countEdge(); ++i ){
    e = _hexaDocument->getEdge(i);
    eItem = new EdgeItem(e);
    eItem->setData( _entry, HEXA_DOC_ENTRY_ROLE );
    _edgeDirItem->appendRow(eItem);
  }

  HEXA_NS::Quad *q     = NULL;
  QuadItem      *qItem = NULL;
  for ( int i=0; i<_hexaDocument->countQuad(); ++i ){
    q = _hexaDocument->getQuad(i);
    qItem = new QuadItem(q);
    qItem->setData( _entry, HEXA_DOC_ENTRY_ROLE );
    _quadDirItem->appendRow(qItem);
  }

  HEXA_NS::Hexa *h     = NULL;
  HexaItem      *hItem = NULL;
  for ( int i=0; i<_hexaDocument->countHexa(); ++i ){
    h = _hexaDocument->getHexa(i);
    hItem = new HexaItem(h);
    hItem->setData( _entry, HEXA_DOC_ENTRY_ROLE );
    _hexaDirItem->appendRow(hItem);
  }
  std::cout<<"DocumentModel::fillData()  end"<<std::endl;
}


void DocumentModel::fillBuilder() 
{
  std::cout<<"DocumentModel::fillBuilder()  begin"<<std::endl;

  HEXA_NS::Vector *v     = NULL;
  VectorItem      *vItem = NULL;
  for ( int i=0; i<_hexaDocument->countVector(); ++i ){
    v = _hexaDocument->getVector(i);
    vItem = new VectorItem(v);
    _vectorDirItem->appendRow(vItem);
  }

  //   _cylinderDirItem
  HEXA_NS::Cylinder *c     = NULL;
  CylinderItem      *cItem = NULL;
  for ( int i=0; i<_hexaDocument->countCylinder(); ++i ){
    c = _hexaDocument->getCylinder(i);
    cItem = new CylinderItem(c);
    _cylinderDirItem->appendRow(cItem);
  }

//   _pipeDirItem
  HEXA_NS::Pipe *p     = NULL;
  PipeItem      *pItem = NULL;
  for ( int i=0; i<_hexaDocument->countPipe(); ++i ){
    p = _hexaDocument->getPipe(i);
    pItem = new PipeItem(p);
    _pipeDirItem->appendRow(pItem);

  } 
//   _elementsDirItem     CS_TODO : no elements
//   HEXA_NS::Elements *e     = NULL;
//   ElementsItem      *eItem = NULL;
//   for ( int i=0; i<_hexaDocument->countElements(); ++i ){
//     e = _hexaDocument->getElements(i);
//     eItem = new ElementsItem(e);
//     _elementsDirItem->appendRow(eItem);
//   }

//   _crossElementsDirItem
//   HEXA_NS::CrossElements *c = NULL;
//   CrossElementsItem      *cItem = NULL;
//   for ( int i=0; i<_hexaDocument->countCrossElements(); ++i ){
//     c = _hexaDocument->getCrossElements(i);
//     cItem = new CrossElementsItem(c);
//     _crossElementsDirItem->appendRow(cItem);
//   }

}

void DocumentModel::fillAssociation()
{

}

void DocumentModel::fillGroups()
{
  std::cout<<"DocumentModel::fillGroups()  begin"<<std::endl;
//   _groupDirItem
  HEXA_NS::Group *g     = NULL;
  GroupItem      *gItem = NULL;
  std::cout<<"countGroup => "<< _hexaDocument->countGroup() << std::endl;
  for ( int i=0; i<_hexaDocument->countGroup(); ++i ){
    g = _hexaDocument->getGroup(i);
    std::cout<<"getGroup => "<< i << std::endl;
    gItem = new GroupItem(g);
    _groupDirItem->appendRow(gItem);
  } 
  std::cout<<"DocumentModel::fillGroups()  end"<<std::endl;
}

void DocumentModel::fillMesh()
{
  //   _lawDirItem
  HEXA_NS::Law *l     = NULL;
  LawItem      *lItem = NULL;
  std::cout<<"countLaw => "<< _hexaDocument->countLaw() << std::endl;
  for ( int i=0; i<_hexaDocument->countLaw(); ++i ){
    l = _hexaDocument->getLaw(i);
    std::cout<<"getLaw => "<< i << std::endl;
    lItem = new LawItem(l);
    _lawDirItem->appendRow(lItem);
  }

  //   _propagationDirItem
  HEXA_NS::Propagation *p     = NULL;
  PropagationItem      *pItem = NULL;
  std::cout<<"countPropagation => "<< _hexaDocument->countPropagation() << std::endl;
  for ( int i=0; i<_hexaDocument->countPropagation(); ++i ){
    p = _hexaDocument->getPropagation(i);
//     std::cout<<"getPropagation => "<< i << std::endl;
    pItem = new PropagationItem(p);
    pItem->setText(QString("Propagation%1").arg(i) );
    _propagationDirItem->appendRow(pItem);
  }


}


// Qt::ItemFlags DocumentModel::flags(const QModelIndex &index) const
// {
//   Qt::ItemFlags flags;
// 
//   if (!index.isValid()){
//       std::cout<<"!index.isValid()"<<std::endl;
//       return Qt::ItemIsEnabled;
//   }
// 
//   QStandardItem *item = itemFromIndex ( index );
// 
//   if ( (item->type() == VERTEXITEM) && (_vertexItemFlags != Qt::NoItemFlags) ){
//     flags = _vertexItemFlags;
//   } else if ( (item->type() == EDGEITEM) && (_edgeItemFlags != Qt::NoItemFlags) ){
//     flags = _edgeItemFlags;
//   } else if ( (item->type() == QUADITEM) && (_quadItemFlags != Qt::NoItemFlags) ){
//     flags = _quadItemFlags;
//   } else if ( (item->type() == HEXAITEM) && (_hexaItemFlags != Qt::NoItemFlags) ){
//     flags = _hexaItemFlags;
//   } else if ( (item->type() == VECTORITEM) && (_vectorItemFlags != Qt::NoItemFlags) ){
//     flags = _vectorItemFlags;
//   } else if ( (item->type() == CYLINDERITEM) && (_cylinderItemFlags != Qt::NoItemFlags) ){
//     flags = _cylinderItemFlags;
//   } else if ( (item->type() == PIPEITEM ) && (_pipeItemFlags != Qt::NoItemFlags) ){
//     flags = _pipeItemFlags;
//   } else if ( (item->type() == ELEMENTSITEM) && (_elementsItemFlags != Qt::NoItemFlags) ){
//     flags = _elementsItemFlags;
//   } else if ( (item->type() == CROSSELEMENTSITEM) && (_crossElementsItemFlags != Qt::NoItemFlags) ){
//     flags = _crossElementsItemFlags;
//   } else if ( (item->type() == GROUPITEM) && (_groupItemFlags != Qt::NoItemFlags) ){
//     flags = _groupItemFlags;
//   } else if ( (item->type() == LAWITEM ) && (_lawItemFlags != Qt::NoItemFlags) ){
//     flags = _lawItemFlags;
//   } else if ( (item->type() == PROPAGATIONITEM ) && (_propagationItemFlags != Qt::NoItemFlags) ){
//     flags = _propagationItemFlags;
//   } else {
//     flags = item->flags();
//   }
// 
// 
//   return flags;
// }



Qt::ItemFlags DocumentModel::flags(const QModelIndex &index) const
{
  Qt::ItemFlags flags;

  if (!index.isValid()){
      std::cout<<"!index.isValid()"<<std::endl;
      return Qt::ItemIsEnabled;
  }
  QStandardItem *item = itemFromIndex ( index );
  if ( _disallowEdition ){
    flags = Qt::ItemFlags( ~Qt::ItemIsEditable );
//     std::cout<<"_disallowEdition"<< std::endl;
  } else {
    flags = item->flags();
//     flags = Qt::ItemFlags( Qt::ItemIsEditable );
//     std::cout<<"allowEdition"<< std::endl;
  }
//   std::cout<<"flags"<< flags << std::endl;

  return flags;
}


void DocumentModel::allowEdition()
{
  std::cout<<"allowEdition "<< this << std::endl;
  _disallowEdition = false;
}

void DocumentModel::disallowEdition()
{
  std::cout<<"disallowEdition "<< this << std::endl;
  _disallowEdition = true;
}


// disallowEdition
// void DocumentModel::allowAllSelection()
// {
//   _vertexItemFlags = Qt::NoItemFlags;
//   _edgeItemFlags   = Qt::NoItemFlags;
//   _quadItemFlags   = Qt::NoItemFlags;
//   _hexaItemFlags   = Qt::NoItemFlags;
// 
//   _vectorItemFlags   = Qt::NoItemFlags;
//   _cylinderItemFlags = Qt::NoItemFlags;
//   _pipeItemFlags     = Qt::NoItemFlags;
//   _elementsItemFlags      = Qt::NoItemFlags;
//   _crossElementsItemFlags = Qt::NoItemFlags;
// 
//   _groupItemFlags = Qt::NoItemFlags;
//   _lawItemFlags   = Qt::NoItemFlags;
//   _propagationItemFlags = Qt::NoItemFlags;
// }



void DocumentModel::allowDataSelectionOnly()
{
  _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );
  _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );
  _quadItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );
  _hexaItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );

  _vectorItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _cylinderItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _pipeItemFlags     = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}





// void DocumentModel::allowVertexSelectionOnly()
// {
//     std::cout << "allowVertexSelectionOnly() allowVertexSelectionOnly() allowVertexSelectionOnly() "<< std::endl;
//     _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable);
//     _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
//     _quadItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
//     _hexaItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
// 
//     _vectorItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
//     _cylinderItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
//     _pipeItemFlags     = Qt::ItemFlags( ~Qt::ItemIsEnabled );
//     _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
//     _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
// 
//     _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
//     _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
//     _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
// 
// //     emit layoutChanged();
// }

void DocumentModel::allowVertexSelectionOnly()
{
    std::cout << "allowVertexSelectionOnly() allowVertexSelectionOnly() allowVertexSelectionOnly() "<< std::endl;
    _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable);
    _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _quadItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _hexaItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );

    _vectorItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _cylinderItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _pipeItemFlags     = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );

    _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );
    _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );

//     emit layoutChanged();
}






void DocumentModel::allowEdgeSelectionOnly()
{
  std::cout << "allowEdgeSelectionOnly() allowEdgeSelectionOnly() allowEdgeSelectionOnly() "<< std::endl;
  _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );
  _quadItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _hexaItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _vectorItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _cylinderItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _pipeItemFlags     = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  
  _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}

void DocumentModel::allowQuadSelectionOnly()
{
  std::cout << "allowQuadSelectionOnly() allowQuadSelectionOnly() allowQuadSelectionOnly() "<< std::endl;
  _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _quadItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );
  _hexaItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _vectorItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _cylinderItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _pipeItemFlags     = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}


void DocumentModel::allowHexaSelectionOnly()
{
  _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _quadItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _hexaItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );

  _vectorItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _cylinderItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _pipeItemFlags     = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}


void DocumentModel::allowVectorSelectionOnly()
{
  std::cout << "allowVectorSelectionOnly() allowVectorSelectionOnly() allowVectorSelectionOnly() "<< std::endl;
  _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _quadItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _hexaItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _vectorItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );
  _cylinderItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _pipeItemFlags     = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}


void DocumentModel::allowCylinderSelectionOnly()
{
  _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _quadItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _hexaItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _vectorItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _cylinderItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );
  _pipeItemFlags     = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}

void DocumentModel::allowPipeSelectionOnly()
{
  _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _quadItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _hexaItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _vectorItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _cylinderItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _pipeItemFlags     = Qt::ItemFlags( ~Qt::ItemIsEditable );
  _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}




/*
void DocumentModel::allowPipeItemFlags()
{
  _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _quadItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _hexaItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _vectorItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _cylinderItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );
  _pipeItemFlags     = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}*/

void DocumentModel::allowElementsSelectionOnly()
{
  std::cout << "allowElementsSelectionOnly() allowElementsSelectionOnly() allowElementsSelectionOnly() "<< std::endl;
  _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _quadItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _hexaItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _vectorItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _cylinderItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _pipeItemFlags     = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );
  _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}

void DocumentModel::allowCrossElementsSelectionOnly()
{
  _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _quadItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _hexaItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _vectorItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _cylinderItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _pipeItemFlags     = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _crossElementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEditable );

  _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}




void DocumentModel::allowLawSelectionOnly()
{
  _vertexItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _edgeItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _quadItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _hexaItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );

  _vectorItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _cylinderItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _pipeItemFlags     = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _elementsItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _crossElementsItemFlags = Qt::ItemFlags( Qt::ItemIsEnabled );

  _groupItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
  _lawItemFlags   = Qt::ItemFlags( ~Qt::ItemIsEditable );
  _propagationItemFlags = Qt::ItemFlags( ~Qt::ItemIsEnabled );
}


QModelIndex DocumentModel::addVertex( double x, double y, double z )
{
  QModelIndex vertexIndex;

  HEXA_NS::Vertex* hv = _hexaDocument->addVertex(x, y, z);

  if ( hv->isValid() ){
    VertexItem* v = new VertexItem(hv);
    _vertexDirItem->appendRow(v);
    vertexIndex = v->index();
    emit patternDataChanged();
  } else {
    delete hv;
  }

  return vertexIndex;
}


QModelIndex DocumentModel::addEdgeVertices (const QModelIndex &i_v0, const QModelIndex &i_v1 )
{
  QModelIndex edgeIndex;

  HEXA_NS::Vertex* hv0 = data(i_v0, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hv1 = data(i_v1, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();

  HEXA_NS::Edge* he = _hexaDocument->addEdge( hv0, hv1 );

  if ( he->isValid() ){
    EdgeItem* e = new EdgeItem(he);
    _edgeDirItem->appendRow(e);
    edgeIndex = e->index();
    emit patternDataChanged();
    QString tmp = "/tmp/addEdgeVertices.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete he;
  }
  return edgeIndex;
}

QModelIndex DocumentModel::addEdgeVector( const QModelIndex &i_v, const QModelIndex &i_vec )
{
  QModelIndex edgeIndex;

  HEXA_NS::Vertex* hv   = data(i_v, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vector* hvec = data(i_vec, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

  HEXA_NS::Edge* he = _hexaDocument->addEdge( hv, hvec );

  if ( he->isValid() ){
    EdgeItem* e = new EdgeItem(he);
    _edgeDirItem->appendRow(e);
    edgeIndex = e->index();
    emit patternDataChanged();
    QString tmp = "/tmp/addEdgeVector.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete he;
  }
  return edgeIndex;
}

QModelIndex DocumentModel::addQuadVertices( const QModelIndex &i_v0, const QModelIndex &i_v1,
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

  if ( hv0 and hv1 and hv2 and hv3 ){
    HEXA_NS::Quad* hq = _hexaDocument->addQuadVertices( hv0, hv1, hv2, hv3 );
    if ( hq->isValid() ){
      QuadItem* q = new QuadItem(hq);
      _quadDirItem->appendRow(q);
      quadIndex = q->index();
      emit patternDataChanged();
      QString tmp = "/tmp/addQuadVertices.vtk";
      _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
    } else {
      delete hq;
    }
  }
  return quadIndex;
}



QModelIndex DocumentModel::addQuadEdges( const QModelIndex &e0, const QModelIndex &e1,
                                         const QModelIndex &e2, const QModelIndex &e3 )
{ //CS_TODO
  QModelIndex quadIndex;

  HEXA_NS::Edge* he0 = data(e0, HEXA_DATA_ROLE).value<HEXA_NS::Edge *>();
  HEXA_NS::Edge* he1 = data(e1, HEXA_DATA_ROLE).value<HEXA_NS::Edge *>();
  HEXA_NS::Edge* he2 = data(e2, HEXA_DATA_ROLE).value<HEXA_NS::Edge *>();
  HEXA_NS::Edge* he3 = data(e3, HEXA_DATA_ROLE).value<HEXA_NS::Edge *>();

  if ( he0 and he1 and he2 and he3 ){
    HEXA_NS::Quad* hq = _hexaDocument->addQuad( he0, he1, he2, he3 );
    if ( hq->isValid() ){
      QuadItem* q = new QuadItem(hq);
      _quadDirItem->appendRow(q);
      quadIndex = q->index();
      emit patternDataChanged();
      QString tmp = "/tmp/addQuadEdges.vtk";
      _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
    } else {
      delete hq;
    }
  }
  return quadIndex;
}



QModelIndex DocumentModel::addHexaVertices( 
            const QModelIndex &iv0, const QModelIndex &iv1,
            const QModelIndex &iv2, const QModelIndex &iv3,
            const QModelIndex &iv4, const QModelIndex &iv5,
            const QModelIndex &iv6, const QModelIndex &iv7 )
{ 
  QModelIndex iHexa;

  HEXA_NS::Vertex* hv0 = data(iv0, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hv1 = data(iv1, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hv2 = data(iv2, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hv3 = data(iv3, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hv4 = data(iv4, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hv5 = data(iv5, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hv6 = data(iv6, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hv7 = data(iv7, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();

  HEXA_NS::Hexa* hh = _hexaDocument->addHexaVertices( hv0, hv1, hv2, hv3,
                                                      hv4, hv5, hv6, hv7 );

  if ( hh->isValid() ){
    HexaItem* h = new HexaItem(hh);
    _hexaDirItem->appendRow(h);
    iHexa = h->index();
    emit patternDataChanged();
    QString tmp = "/tmp/addHexaVertices.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hh;
  }

  return iHexa;
}



QModelIndex DocumentModel::addHexaQuad( const QModelIndex &i_q0, const QModelIndex &i_q1, const QModelIndex &i_q2,const QModelIndex &i_q3, const QModelIndex &i_q4, const QModelIndex &i_q5 )
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
    emit patternDataChanged();
    QString tmp = "/tmp/addHexaQuad.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
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


QModelIndex DocumentModel::addVectorVertices( const QModelIndex &iv0, const QModelIndex &iv1 )
{
  QModelIndex iVec;

  HEXA_NS::Vertex* hv0 = data(iv0, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hv1 = data(iv1, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();

  HEXA_NS::Vector* hvec = _hexaDocument->addVectorVertices( hv0, hv1 );

  if ( hvec->isValid() ){
    VectorItem* vec = new VectorItem(hvec);
    _vectorDirItem->appendRow(vec);
    iVec = vec->index();
    QString tmp = "/tmp/addVectorVertices.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hvec;
  }

  return iVec;
}


QModelIndex DocumentModel::addCylinder( const QModelIndex &iv, const QModelIndex &ivec, double r,  double h )
{
  QModelIndex iCyl;

  HEXA_NS::Vertex* hv   = data(iv, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vector* hvec = data(ivec, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

  HEXA_NS::Cylinder* hcyl = _hexaDocument->addCylinder( hv, hvec, r, h );

  if ( hcyl->isValid() ){
    CylinderItem* cyl = new CylinderItem(hcyl);
    _cylinderDirItem->appendRow(cyl);
    iCyl = cyl->index();
    QString tmp = "/tmp/addCylinder.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hcyl;
  }

  return iCyl;
}




QModelIndex DocumentModel::addPipe( const QModelIndex &iv, const QModelIndex &ivec, double ri, double re, double h )
{
  QModelIndex iPipe;

  HEXA_NS::Vertex* hv   = data(iv, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vector* hvec = data(ivec, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

  HEXA_NS::Pipe* hPipe = _hexaDocument->addPipe( hv, hvec, ri, re, h );

  if ( hPipe->isValid() ){
    PipeItem* pipe = new PipeItem(hPipe);
    _pipeDirItem->appendRow(pipe);
    iPipe = pipe->index();
    QString tmp = "/tmp/addPipe.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hPipe;
  }

  return iPipe;
}





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
    updateData(); //CS_TODO more or less?
    ElementsItem* eltsItem = new ElementsItem(new_helts);
    _elementsDirItem->appendRow(eltsItem);
    eltsIndex = eltsItem->index();
    QString tmp = "/tmp/makeCartesian1.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    std::cout<<"makeCartesian KO!!!"<<std::endl; 
    delete new_helts;
  }

  return eltsIndex;
}




QModelIndex DocumentModel::makeCartesian( const QModelIndex& ivex,
                                          const QModelIndex& ivec,
                                          int nx, int ny, int nz )
{
  QModelIndex iElts;

  HEXA_NS::Vertex* hVex = data(ivex, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vector* hVec = data(ivec, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

  HEXA_NS::Elements* hElts = _hexaDocument->makeCartesian( hVex,
                                                           hVec,
                                                           nx, ny, nz );
  if ( hElts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
    QString tmp = "/tmp/makeCartesian2.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hElts;
  }

  return iElts;
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
    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(new_helts);
    _elementsDirItem->appendRow(eltsItem);
    eltsIndex = eltsItem->index();
    QString tmp = "/tmp/makeCylindrical.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete new_helts;
  }

  return eltsIndex;
}


QModelIndex DocumentModel::makeSpherical( const QModelIndex& iv, const QModelIndex& ivec, int nb, double k)
{
  QModelIndex iElts;

  HEXA_NS::Vertex* hv   = data(iv, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vector* hvec = data(ivec, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

  HEXA_NS::Elements* hElts = _hexaDocument->makeSpherical( hv, hvec, nb, k );

  if ( hElts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
    QString tmp = "/tmp/makeSpherical.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hElts;
  }

  return iElts;
}


QModelIndex DocumentModel::makeCylinder( const QModelIndex& icyl, const QModelIndex& ivec,
                                         int nr, int na, int nl )
{
  QModelIndex iElts;

  HEXA_NS::Cylinder* hcyl = data(icyl, HEXA_DATA_ROLE).value<HEXA_NS::Cylinder *>();
  HEXA_NS::Vector* hvec   = data(ivec, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

  HEXA_NS::Elements* hElts = _hexaDocument->makeCylinder( hcyl, hvec, nr, na, nl );
  if ( hElts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
    QString tmp = "/tmp/makeCylinder.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hElts;
  }

  return iElts;
}



QModelIndex DocumentModel::makePipe( const QModelIndex& ipipe, const QModelIndex& ivecx, 
                                     int nr, int na, int nl )
{ 
  QModelIndex iElts;

  HEXA_NS::Pipe*   hPipe  = data(ipipe, HEXA_DATA_ROLE).value<HEXA_NS::Pipe *>();
  HEXA_NS::Vector* hVecx  = data(ivecx, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

  HEXA_NS::Elements* hElts = _hexaDocument->makePipe( hPipe, hVecx, nr, na, nl );

  if ( hElts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
    QString tmp = "/tmp/makePipe.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hElts;
  }

  return iElts;
}



QModelIndex DocumentModel::makeCylinders(const QModelIndex& icyl1, const QModelIndex& icyl2)
{ //CS_TODO
  QModelIndex iCrossElts;
  
  HEXA_NS::Cylinder* hCyl1  = data(icyl1, HEXA_DATA_ROLE).value<HEXA_NS::Cylinder *>();
  HEXA_NS::Cylinder* hCyl2  = data(icyl2, HEXA_DATA_ROLE).value<HEXA_NS::Cylinder *>();

  HEXA_NS::CrossElements* hCrossElts = _hexaDocument->makeCylinders( hCyl1, hCyl2 );
  
  if ( hCrossElts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* crossElts = new ElementsItem(hCrossElts);
    _crossElementsDirItem->appendRow(crossElts);
    iCrossElts = crossElts->index();    
    QString tmp = "/tmp/makeCylinders.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hCrossElts;
  }
    
  return iCrossElts;
}

//
QModelIndex DocumentModel::makePipes( const QModelIndex& ipipe1, const QModelIndex& ipipe2 )
{ 
  QModelIndex iCrossElts;
  
  HEXA_NS::Pipe* hPipe1  = data(ipipe1, HEXA_DATA_ROLE).value<HEXA_NS::Pipe *>();
  HEXA_NS::Pipe* hPipe2  = data(ipipe2, HEXA_DATA_ROLE).value<HEXA_NS::Pipe *>();

  HEXA_NS::CrossElements* hCrossElts = _hexaDocument->makePipes( hPipe1, hPipe2 );
  
  if ( hCrossElts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* crossElts = new ElementsItem(hCrossElts);
    _crossElementsDirItem->appendRow(crossElts);
    iCrossElts = crossElts->index();
    QString tmp = "/tmp/makePipes.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hCrossElts;
  }
  
  return iCrossElts;
}





// ************  EDIT HEXABLOCK MODEL ************

bool DocumentModel::updateVertex( const QModelIndex& ivertex, double x, double y, double z )
{
  bool ret = false;

//   cout << "DocumentModel::updateVertex" << ivertex.data().toString().toStdString() << endl;
//   cout << "DocumentModel::updateVertex" << ivertex.data(HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>() << endl;
//   cout << "DocumentModel::updateVertex" << ivertex.data(HEXA_TREE_ROLE).toString().toStdString() << endl;
//   cout << "DocumentModel::updateVertex" << ivertex.data(HEXA_DOC_ENTRY_ROLE).toString().toStdString() << endl;

//   HEXA_NS::Vertex* hVertex = data(ivertex, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hVertex = ivertex.data(HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>(); //CS_TODO?  pareil pour toutes les autres méthodes du modèle?

  cout << "DocumentModel::updateVertex hVertex = " << hVertex << endl;
  if ( hVertex ){
    hVertex->setX ( x );
    hVertex->setY ( y );
    hVertex->setZ ( z );
    emit patternDataChanged();
    ret = true;
  }

  return ret;
}

bool DocumentModel::removeHexa( const QModelIndex& ihexa )
{
  bool ret = false;
  HEXA_NS::Hexa* hHexa = data(ihexa, HEXA_DATA_ROLE).value<HEXA_NS::Hexa *>();

  int r = _hexaDocument->removeHexa( hHexa );
  if ( r == HOK ){
    updateData();
    QString tmp = "/tmp/removeHexa.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
    ret = true;
  } else if ( r == HERR ){    
    ret = false;
  }
  
  return ret;
}



bool DocumentModel::removeConnectedHexa( const QModelIndex& ihexa )
{
  bool ret = false;
  HEXA_NS::Hexa* hHexa = data(ihexa, HEXA_DATA_ROLE).value<HEXA_NS::Hexa *>();
  
  int r = _hexaDocument->removeConnectedHexa( hHexa );
  
  if ( r == HOK ){    
    updateData();
    QString tmp = "/tmp/removeConnectedHexa.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
    ret = true;
  } else if ( r == HERR ){    
    ret = false;
  }
  
  return ret;
}

 
QModelIndex DocumentModel::prismQuad( const QModelIndex& iquad, const QModelIndex& ivec, int nb)
{ 
  QModelIndex iElts;

  HEXA_NS::Quad*   hQuad = data(iquad, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();
  HEXA_NS::Vector* hVect = data(ivec, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

  HEXA_NS::Elements* hElts = _hexaDocument->prismQuad( hQuad, hVect, nb );

  if ( hElts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
    QString tmp = "/tmp/prismQuad.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hElts;
  }

  return iElts;
}


QModelIndex DocumentModel::prismQuads( const QModelIndexList& iquads, const QModelIndex& ivec, int nb)
{
  QModelIndex iElts;

  HEXA_NS::Quads   hQuads;
  HEXA_NS::Quad*   hQuad = NULL;
  foreach( const QModelIndex& iquad, iquads ){
    hQuad = data( iquad, HEXA_DATA_ROLE ).value<HEXA_NS::Quad *>();
    hQuads.push_back( hQuad );
  }
  HEXA_NS::Vector* hVect = data(ivec, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

  HEXA_NS::Elements* hElts = _hexaDocument->prismQuads( hQuads, hVect, nb );
  if ( hElts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
    QString tmp = "/tmp/prismQuads.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hElts;
  }
  return iElts;
}


//
QModelIndex DocumentModel::joinQuad(
      const QModelIndex& iquadstart, const QModelIndex& iquaddest,
      const QModelIndex& iv0, const QModelIndex& iv1,
      const QModelIndex& iv2, const QModelIndex& iv3,
      int nb )
{
  QModelIndex iElts;

  HEXA_NS::Quad*   hQuadStart  = data(iquadstart, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();
  HEXA_NS::Quad*   hQuadDest   = data(iquaddest, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();

  HEXA_NS::Vertex* hVertex0 = data(iv0, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hVertex1 = data(iv1, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hVertex2 = data(iv2, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hVertex3 = data(iv3, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();

  HEXA_NS::Elements* hElts = _hexaDocument->joinQuad( hQuadStart, hQuadDest,
                        hVertex0,  hVertex1,  hVertex2,  hVertex3, nb );

  if ( hElts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
    QString tmp = "/tmp/joinQuad.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hElts;
  }

  return iElts;
}


QModelIndex DocumentModel::joinQuads( 
      const QModelIndexList& iquadsstart, const QModelIndex& iquaddest,
      const QModelIndex& iv0, const QModelIndex& iv1,
      const QModelIndex& iv2, const QModelIndex& iv3,
      int nb )
{
  QModelIndex iElts;

  HEXA_NS::Quad*   hQuadStart;
  HEXA_NS::Quads  hQuadsStart;

  foreach( const QModelIndex& iquad, iquadsstart ){
    hQuadStart = data( iquad, HEXA_DATA_ROLE ).value<HEXA_NS::Quad *>();
    hQuadsStart.push_back( hQuadStart );
  }
  HEXA_NS::Quad*   hQuadDest = data( iquaddest, HEXA_DATA_ROLE ).value<HEXA_NS::Quad *>();

  HEXA_NS::Vertex* hVertex0 = data(iv0, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hVertex1 = data(iv1, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hVertex2 = data(iv2, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hVertex3 = data(iv3, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();

  HEXA_NS::Elements* hElts = _hexaDocument->joinQuads(
                        hQuadsStart, hQuadDest,
                        hVertex0,  hVertex1,  hVertex2,  hVertex3,
                        nb );

  if ( hElts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
    QString tmp = "/tmp/joinQuads.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hElts;
  }

  return iElts;
}





bool DocumentModel::mergeVertices( const QModelIndex &iv0, const QModelIndex &iv1 ) //CS_TODO : impact sur le model?
{
  bool ret = false;

  HEXA_NS::Vertex* hv0 = data(iv0, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hv1 = data(iv1, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();

  int r = _hexaDocument->mergeVertices( hv0, hv1 );
  if ( r == HOK ){
    updateData(); //CS_TODO more or less?
    std::cout << "DocumentModel:: mergeVertices => OK " << std::endl;
    QString tmp = "/tmp/mergeVertices.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
    ret = true;
  } else if ( r == HERR ){
    std::cout << "DocumentModel:: mergeVertices => KO " << std::endl;
    ret = false;
  }

  return ret;
}


bool DocumentModel::mergeEdges( const QModelIndex &ie0, const QModelIndex &ie1,
                                const QModelIndex &iv0, const QModelIndex &iv1 )
//CS_TODO : impact sur le model?
{
  bool ret = false;

  HEXA_NS::Edge* he0 = data(ie0, HEXA_DATA_ROLE).value<HEXA_NS::Edge *>();
  HEXA_NS::Edge* he1 = data(ie1, HEXA_DATA_ROLE).value<HEXA_NS::Edge *>();

  HEXA_NS::Vertex* hv0 = data(iv0, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hv1 = data(iv1, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();

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

bool DocumentModel::mergeQuads( const QModelIndex& iquad0, const QModelIndex& iquad1,
                                const QModelIndex& iv0, const QModelIndex& iv1,
                                const QModelIndex& iv2, const QModelIndex& iv3 )
{
  bool ret = false;

  HEXA_NS::Quad* hquad0 = data(iquad0, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();
  HEXA_NS::Quad* hquad1 = data(iquad1, HEXA_DATA_ROLE).value<HEXA_NS::Quad *>();

  HEXA_NS::Vertex* hv0 = data(iv0, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hv1 = data(iv1, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hv2 = data(iv2, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vertex* hv3 = data(iv3, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();

  int r = _hexaDocument->mergeQuads( hquad0, hquad1, hv0, hv1, hv2, hv3 );
  if ( r == HOK ){
    updateData();
    ret = true;
    QString tmp = "/tmp/mergeQuads.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else if ( r == HERR ){
    ret = false;
  }

  return ret;
}



//
QModelIndex DocumentModel::disconnectVertex( const QModelIndex& ihexa, const QModelIndex& ivertex )
{
  QModelIndex iElts;

  HEXA_NS::Hexa*   hHexa   = data( ihexa, HEXA_DATA_ROLE ).value<HEXA_NS::Hexa *>();
  HEXA_NS::Vertex* hVertex = data( ivertex, HEXA_DATA_ROLE ).value<HEXA_NS::Vertex *>();

  HEXA_NS::Elements* hElts = _hexaDocument->disconnectVertex( hHexa, hVertex );

  if ( hElts && hElts->isValid() ){
    updateData(); //CS_TO_CHECK
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
    QString tmp = "/tmp/disconnectVertex.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hElts;
  }

  return iElts;
}



QModelIndex DocumentModel::disconnectEdge( const QModelIndex& ihexa, const QModelIndex& iedge )
{
  QModelIndex iElts;

  HEXA_NS::Hexa* hHexa = data( ihexa, HEXA_DATA_ROLE ).value<HEXA_NS::Hexa *>();
  HEXA_NS::Edge* hEdge = data( iedge, HEXA_DATA_ROLE ).value<HEXA_NS::Edge *>();

  HEXA_NS::Elements* hElts = _hexaDocument->disconnectEdge( hHexa, hEdge );

  if ( hElts->isValid() ){
    updateData(); //CS_TO_CHECK
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
    QString tmp = "/tmp/disconnectEdge.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hElts;
  }

  return iElts;
}

QModelIndex DocumentModel::disconnectQuad( const QModelIndex& ihexa, const QModelIndex& iquad )
{
  QModelIndex iElts;

  HEXA_NS::Hexa* hHexa = data( ihexa, HEXA_DATA_ROLE ).value<HEXA_NS::Hexa *>();
  HEXA_NS::Quad* hQuad = data( iquad, HEXA_DATA_ROLE ).value<HEXA_NS::Quad *>();

  HEXA_NS::Elements* hElts = _hexaDocument->disconnectQuad( hHexa, hQuad );

  if ( hElts->isValid() ){
    updateData(); //CS_TO_CHECK
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
    QString tmp = "/tmp/disconnectQuad.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hElts;
  }

  return iElts;
}


QModelIndex DocumentModel::cutEdge( const QModelIndex &i_e0, int nbcuts )
//CS_TODO : impact sur le model?
{
  QModelIndex iElts;

  HEXA_NS::Edge* he0 = data(i_e0, HEXA_DATA_ROLE).value<HEXA_NS::Edge *>();
  HEXA_NS::Elements* helts = _hexaDocument->cut( he0, nbcuts );

  if ( helts->isValid() ){
    updateData(); //CS_TODO more?
    ElementsItem* elts = new ElementsItem(helts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
    QString tmp = "/tmp/cutEdge.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete helts;
  }

  return iElts;
}




// Elements makeTranslation( in Elements l, in Vector vec )
//         raises (SALOME::SALOME_Exception);
QModelIndex DocumentModel::makeTranslation( const QModelIndex& ielts, const QModelIndex& ivec )
{
  QModelIndex iElts;

  HEXA_NS::Elements* hElts = data(ielts, HEXA_DATA_ROLE).value<HEXA_NS::Elements *>();
  HEXA_NS::Vector*   hVec  = data(ivec, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

  HEXA_NS::Elements* hNewElts = _hexaDocument->makeTranslation( hElts, hVec );

  if ( hNewElts->isValid() ){
    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(hNewElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();
    QString tmp = "/tmp/makeTranslation.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hNewElts;
  }

  return iElts;
}



QModelIndex DocumentModel::makeScale( const QModelIndex& ielts, const QModelIndex& ivex, double k )
{
  QModelIndex iElts;

  HEXA_NS::Elements* hElts = data(ielts, HEXA_DATA_ROLE).value<HEXA_NS::Elements *>();
  HEXA_NS::Vertex*   hVex  = data(ivex, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();

  HEXA_NS::Elements* hNewElts = _hexaDocument->makeScale( hElts, hVex, k );

  if ( hNewElts->isValid() ){
    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(hNewElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();
    QString tmp = "/tmp/makeScale.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hNewElts;
  }

  return iElts;
}



QModelIndex DocumentModel::makeRotation( const QModelIndex& ielts, 
                                         const QModelIndex& iv, 
                                         const QModelIndex& ivec, double angle )
{
  QModelIndex iElts;

  HEXA_NS::Elements* hElts = data(ielts, HEXA_DATA_ROLE).value<HEXA_NS::Elements *>();
  HEXA_NS::Vertex*   hVex  = data(iv, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vector*   hVec  = data(ivec, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

  HEXA_NS::Elements* hNewElts = _hexaDocument->makeRotation( hElts, hVex, hVec, angle );

  if ( hNewElts->isValid() ){
    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(hNewElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();
    QString tmp = "/tmp/makeRotation.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hNewElts;
  }

  return iElts;
}


QModelIndex DocumentModel::makeSymmetryPoint( const QModelIndex& ielts, const QModelIndex& iv )
{
  QModelIndex iElts;

  HEXA_NS::Elements* hElts = data(ielts, HEXA_DATA_ROLE).value<HEXA_NS::Elements *>();
  HEXA_NS::Vertex*   hVex  = data(iv, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>(); 

  HEXA_NS::Elements* hNewElts = _hexaDocument->makeSymmetryPoint (hElts, hVex);

  if ( hNewElts->isValid() ){
    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(hNewElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();
    QString tmp = "/tmp/makeSymmetryPoint.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hNewElts;
  }

  return iElts;
}


QModelIndex DocumentModel::makeSymmetryLine( const QModelIndex& ielts, 
                                             const QModelIndex& iv, 
                                             const QModelIndex& ivec )
{
  QModelIndex iElts;

  HEXA_NS::Elements* hElts = data(ielts, HEXA_DATA_ROLE).value<HEXA_NS::Elements *>();
  HEXA_NS::Vertex*   hVex  = data(iv, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vector*   hVec  = data(ivec, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

  HEXA_NS::Elements* hNewElts = _hexaDocument->makeSymmetryLine( hElts, hVex, hVec );

  if ( hNewElts->isValid() ){
    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(hNewElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();
    QString tmp = "/tmp/makeSymmetryLine.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hNewElts;
  }

  return iElts;
}


QModelIndex DocumentModel::makeSymmetryPlane( const QModelIndex& ielts, const QModelIndex& iv, const QModelIndex& ivec )
{
  QModelIndex iElts;

  HEXA_NS::Elements* hElts = data(ielts, HEXA_DATA_ROLE).value<HEXA_NS::Elements *>();
  HEXA_NS::Vertex*   hVex  = data(iv, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vector*   hVec  = data(ivec, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

  HEXA_NS::Elements* hNewElts = _hexaDocument->makeSymmetryPlane( hElts, hVex, hVec );

  if ( hNewElts->isValid() ){
    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(hNewElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();
    QString tmp = "/tmp/makeSymmetryPlane.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else {
    delete hNewElts;
  }

  return iElts;
}


bool DocumentModel::performTranslation( const QModelIndex& ielts, const QModelIndex& ivec )
{
  bool ret = false;

  HEXA_NS::Elements* hElts = data(ielts, HEXA_DATA_ROLE).value<HEXA_NS::Elements *>();
  HEXA_NS::Vector*   hVec  = data(ivec, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

  int r = _hexaDocument->performTranslation (hElts, hVec);
  if ( r == HOK ){
    updateData();
    ret = true;
    QString tmp = "/tmp/performTranslation.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else if ( r == HERR ){
    ret = false;
  }

  return ret;
}


//
bool DocumentModel::performScale( const QModelIndex& ielts, const QModelIndex& ivex, double k )
{
  bool ret = false;

  HEXA_NS::Elements* hElts = data(ielts, HEXA_DATA_ROLE).value<HEXA_NS::Elements *>();
  HEXA_NS::Vertex*   hVex  = data(ivex, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();

  int r = _hexaDocument->performScale (hElts, hVex, k);
  if ( r == HOK ){
    updateData();
    ret = true;
    QString tmp = "/tmp/performScale.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else if ( r == HERR ){
    ret = false;
  }

  return ret;
}

//
bool DocumentModel::performRotation( const QModelIndex& ielts, const QModelIndex& ivex, const QModelIndex& ivec, double angle )
{
  bool ret = false;

  HEXA_NS::Elements* hElts = data(ielts, HEXA_DATA_ROLE).value<HEXA_NS::Elements *>();
  HEXA_NS::Vertex*   hVex  = data(ivex, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vector*   hVec  = data(ivec, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

  int r = _hexaDocument-> performRotation( hElts, hVex, hVec, angle );
  if ( r == HOK ){
    updateData();
    ret = true;
    QString tmp = "/tmp/performRotation.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else if ( r == HERR ){
    ret = false;
  }

  return ret;
}


//
bool DocumentModel::performSymmetryPoint( const QModelIndex& ielts, const QModelIndex& ivex )
{
  bool ret = false;

  HEXA_NS::Elements* hElts = data(ielts, HEXA_DATA_ROLE).value<HEXA_NS::Elements *>();
  HEXA_NS::Vertex*   hVex  = data(ivex, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();

  int r = _hexaDocument->performSymmetryPoint( hElts, hVex );
  if ( r == HOK ){
    updateData();
    ret = true;
    QString tmp = "/tmp/performSymmetryPoint.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else if ( r == HERR ){
    ret = false;
  }

  return ret;
}


bool DocumentModel::performSymmetryLine( const QModelIndex& ielts, const QModelIndex& ivex, const QModelIndex& ivec )
{
  bool ret = false;

  HEXA_NS::Elements* hElts = data(ielts, HEXA_DATA_ROLE).value<HEXA_NS::Elements *>();
  HEXA_NS::Vertex*   hVex  = data(ivex, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vector*   hVec  = data(ivec, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

  int r = _hexaDocument->performSymmetryLine( hElts, hVex, hVec );
  if ( r == HOK ){
    updateData();
    ret = true;
    QString tmp = "/tmp/performSymmetryLine.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else if ( r == HERR ){
    ret = false;
  }

  return ret;
}


bool DocumentModel::performSymmetryPlane( const QModelIndex& ielts,
                                          const QModelIndex& ivex,
                                          const QModelIndex& ivec )
{
  bool ret = false;

  HEXA_NS::Elements* hElts = data(ielts, HEXA_DATA_ROLE).value<HEXA_NS::Elements *>();
  HEXA_NS::Vertex*   hVex  = data(ivex, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
  HEXA_NS::Vector*   hVec  = data(ivec, HEXA_DATA_ROLE).value<HEXA_NS::Vector *>();

  int r = _hexaDocument->performSymmetryPlane( hElts, hVex, hVec );
  if ( r == HOK ){
    updateData();
    ret = true;
    QString tmp = "/tmp/performSymmetryPlane.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else if ( r == HERR ){
    ret = false;
  }

  return ret;
}


// ************  GROUPS  ************
//
QModelIndex DocumentModel::addGroup( const QString& name, Group kind )
{
  QModelIndex iGroup;

  HEXA_NS::Group* hGroup = _hexaDocument->addGroup( name.toLocal8Bit().constData(), kind );

  GroupItem* groupItem = new GroupItem(hGroup);
  _groupDirItem->appendRow(groupItem);
  iGroup = groupItem->index();
  QString tmp = "/tmp/addGroup.vtk";
  _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );

  return iGroup;
}




//
bool DocumentModel::removeGroup( const QModelIndex& igrp )
{
  bool ret = false;

  HEXA_NS::Group* hGroup = data(igrp, HEXA_DATA_ROLE).value<HEXA_NS::Group *>();
  int r = _hexaDocument->removeGroup ( hGroup );

  if ( r == HOK ){
    removeRow( igrp.row(), igrp.parent());
    ret = true;
    QString tmp = "/tmp/removeGroup.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else if ( r == HERR ){
    ret = false;
  }
}



// 7.4 Boite: éditer un groupe
void DocumentModel::setGroupName( const QModelIndex& igrp, const QString& name )
{
  HEXA_NS::Group* hGroup = data(igrp, HEXA_DATA_ROLE).value<HEXA_NS::Group *>();

  if ( hGroup ){
    hGroup->setName( name.toLocal8Bit().constData() );
    setData(igrp, QVariant::fromValue( name ) );
  }


  QString tmp = "/tmp/setGroupName.vtk";
  _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
}


bool DocumentModel::addGroupElement( const QModelIndex& igrp, const QModelIndex& ielt )
{ //CS_TODO : check input? add child?
// int       addElement    (EltBase* elt);

  HEXA_NS::Group*   hGroup = data(igrp, HEXA_DATA_ROLE).value<HEXA_NS::Group *>();
  HEXA_NS::EltBase* hElt   = data(ielt, HEXA_DATA_ROLE).value<HEXA_NS::EltBase *>();

  if ( hGroup and hElt )
    hGroup->addElement( hElt );

  QString tmp = "/tmp/addGroupElement.vtk";
  _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
}


bool DocumentModel::removeGroupElement( const QModelIndex& igrp, int nro )
{ //CS_TODO : remove child?
  HEXA_NS::Group* hGroup = data(igrp, HEXA_DATA_ROLE).value<HEXA_NS::Group *>();

  if ( hGroup )
    hGroup->removeElement( nro );

  QString tmp = "/tmp/removeGroupElement.vtk";
  _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
}


bool DocumentModel::clearGroupElement( const QModelIndex& igrp )
{
  HEXA_NS::Group* hGroup = data(igrp, HEXA_DATA_ROLE).value<HEXA_NS::Group *>();

  if ( hGroup )
    hGroup->clearElement();

  QString tmp = "/tmp/clearGroupElement.vtk";
  _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
}

// ************  LAWS  ************

//
QModelIndex DocumentModel::addLaw( const QString& name, int nbnodes )
{
  QModelIndex iLaw;

  HEXA_NS::Law* hLaw = _hexaDocument->addLaw( name.toLocal8Bit().constData(), nbnodes );

  LawItem* lawItem = new LawItem(hLaw);
  _lawDirItem->appendRow(lawItem);
  iLaw = lawItem->index();
  QString tmp = "/tmp/addLaw.vtk";
  _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );

  return iLaw;
}




bool DocumentModel::setLaw( const QModelIndex& ilaw, int nbnodes, double coeff, KindLaw type )
{
  bool ret = false;

  HEXA_NS::Law* hLaw = data(ilaw, HEXA_DATA_ROLE).value<HEXA_NS::Law *>();

  if ( hLaw ){
    int ok;
    ok = hLaw->setNodes( nbnodes );
    ( ok == HOK) ? ret = true : ret = false;
    ok = hLaw->setCoefficient( coeff );
    hLaw->setKind(type);
  }

  QString tmp = "/tmp/setLaw.vtk";
  _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  return ret;
}


// 
bool  DocumentModel::removeLaw( const QModelIndex& ilaw )
{
  bool ret = false;
  HEXA_NS::Law* hLaw = data(ilaw, HEXA_DATA_ROLE).value<HEXA_NS::Law *>();
  int r = _hexaDocument->removeLaw( hLaw );

  if ( r == HOK ){
    removeRow( ilaw.row(),  ilaw.parent());
    ret = true;
    QString tmp = "/tmp/removeLaw.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else if ( r == HERR ){
    ret = false;
  }

  return ret;
}


bool DocumentModel::setPropagation( const QModelIndex& iPropagation, const QModelIndex& iLaw, bool way )
{
  bool ret = false;
  HEXA_NS::Propagation* hPropagation = data(iPropagation, HEXA_DATA_ROLE).value<HEXA_NS::Propagation *>();
  HEXA_NS::Law* hLaw = data(iLaw, HEXA_DATA_ROLE).value<HEXA_NS::Law *>();

  int r = hPropagation->setLaw( hLaw );
  hPropagation->setWay( way );

  if ( r == HOK ){
    ret = true;
    QString tmp = "/tmp/setPropagation.vtk";
    _hexaDocument->saveVtk( tmp.toLocal8Bit().constData() );
  } else if ( r == HERR ){
    ret = false;
  }
  return ret;
}





HEXA_NS::Document* DocumentModel::documentImpl()
{
  return _hexaDocument;
}


QString DocumentModel::documentEntry()
{
  return _entry;
}





// 8.3 Boite: éditer une loi  CS_TODO
// (idem création)







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


HEXA_NS::Document* PatternDataModel::documentImpl()
{
  HEXA_NS::Document* doc = NULL;
  DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
  if (m) doc = m->documentImpl();
  return doc;
}

QString PatternDataModel::documentEntry()
{
  QString entry;
  DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
  if (m) entry = m->documentEntry();
  return entry;
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





AssociationsModel::AssociationsModel( QObject * parent ) :
  QSortFilterProxyModel( parent )
{
  QString assocRegExp;// =QString("(%1|%2)").arg(GROUP_TREE).arg(GROUP_DIR_TREE); CS_TODO

  setFilterRole( HEXA_TREE_ROLE );
  setFilterRegExp ( QRegExp(assocRegExp) ); 
}


AssociationsModel::~AssociationsModel() 
{
}



Qt::ItemFlags AssociationsModel::flags(const QModelIndex &index) const
{
//   std::cout<<"AssociationsModel::flags()"<<std::endl;
  Qt::ItemFlags flags;

  DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
  if ( m != NULL ){
    flags = m->flags( mapToSource(index) );
  }
  return flags;
}


QVariant AssociationsModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
  if ( section == 0 and orientation == Qt::Horizontal and role == Qt::DisplayRole ){
      return QVariant( "Associations" );
  } else {
      return QSortFilterProxyModel::headerData ( section, orientation, role );
  }
}


QStandardItem* AssociationsModel::itemFromIndex ( const QModelIndex & index ) const
{
  QStandardItem *item = NULL;
  DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
  if ( m != NULL ){
    item = m->itemFromIndex( mapToSource(index) );
  }
  return item;
}









GroupsModel::GroupsModel( QObject * parent ) :
  QSortFilterProxyModel( parent )
{
  QString groupsRegExp =QString("(%1|%2)").arg(GROUP_TREE).arg(GROUP_DIR_TREE);

  setFilterRole( HEXA_TREE_ROLE );
  setFilterRegExp ( QRegExp(groupsRegExp ) ); 
}



GroupsModel::~GroupsModel() 
{
}




Qt::ItemFlags GroupsModel::flags(const QModelIndex &index) const
{
//   std::cout<<"GroupsModel::flags()"<<std::endl;
  Qt::ItemFlags flags;

  DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
  if ( m != NULL ){
    flags = m->flags( mapToSource(index) );
  }
  return flags;
}



QVariant GroupsModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
  if ( section == 0 and orientation == Qt::Horizontal and role == Qt::DisplayRole ){
      return QVariant( "Groups" );
  } else {
      return QSortFilterProxyModel::headerData ( section, orientation, role );
  }
}



QStandardItem* GroupsModel::itemFromIndex ( const QModelIndex & index ) const
{
  QStandardItem *item = NULL;
  DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
  if ( m != NULL ){
    item = m->itemFromIndex( mapToSource(index) );
  }
  return item;
}








MeshModel::MeshModel( QObject * parent ) :
  QSortFilterProxyModel( parent )
{
  QString meshRegExp =QString("(%1|%2|%3|%4)").arg(LAW_TREE).arg(LAW_DIR_TREE)                            .arg(PROPAGATION_TREE).arg(PROPAGATION_DIR_TREE);

  setFilterRole( HEXA_TREE_ROLE );
  setFilterRegExp ( QRegExp(meshRegExp) );
}




MeshModel::~MeshModel() 
{
}




Qt::ItemFlags MeshModel::flags(const QModelIndex &index) const
{
//   std::cout<<"MeshModel::flags()"<<std::endl;
  Qt::ItemFlags flags;

  DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
  if ( m != NULL ){
    flags = m->flags( mapToSource(index) );
  }
  return flags;
}


QVariant MeshModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
  if ( section == 0 and orientation == Qt::Horizontal and role == Qt::DisplayRole ){
      return QVariant( "Mesh" );
  } else {
      return QSortFilterProxyModel::headerData ( section, orientation, role );
  }
}


QStandardItem* MeshModel::itemFromIndex ( const QModelIndex & index ) const
{
  QStandardItem *item = NULL;
  DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
  if ( m != NULL ){
    item = m->itemFromIndex( mapToSource(index) );
  }
  return item;
}
