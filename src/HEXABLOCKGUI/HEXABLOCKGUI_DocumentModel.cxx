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

//CS_TODO: relever les fonctions qui nécessitent updateData().
//        addGroupElement à tester
#include <algorithm>
#include <string>

#include "utilities.h"

#include "HEXABLOCKGUI_DocumentModel.hxx"
#include "HEXABLOCKGUI_DocumentItem.hxx"

#include "HEXABLOCKGUI.hxx"
#include "HEXABLOCKGUI_Trace.hxx"

#include "HexVertex.hxx"
#include "HexShape.hxx"

#include <GEOMBase.h>
#include "HEXABLOCKGUI_SalomeTools.hxx"

#include <qglobal.h>
//#define _DEVDEBUG_


using namespace std;
//using namespace HEXA_NS;
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

  _vertexDirItem( new QStandardItem(tr("TREE_ITEM_VERTEX")) ),
  _edgeDirItem(   new QStandardItem(tr("TREE_ITEM_EDGE")) ),
  _quadDirItem(   new QStandardItem(tr("TREE_ITEM_QUAD")) ),
  _hexaDirItem(   new QStandardItem(tr("TREE_ITEM_HEXA")) ),

  _vectorDirItem(        new QStandardItem(tr("TREE_ITEM_VECTOR")) ),
  _cylinderDirItem(      new QStandardItem(tr("TREE_ITEM_CYLINDER")) ),
  _pipeDirItem(          new QStandardItem(tr("TREE_ITEM_PIPE")) ),
  _elementsDirItem(      new QStandardItem(tr("TREE_ITEM_ELEMENT")) ),
  _crossElementsDirItem( new QStandardItem(tr("TREE_ITEM_CROSSELEMENT")) ),

  _groupDirItem( new QStandardItem(tr("TREE_ITEM_GROUP")) ),

  _lawDirItem( new QStandardItem(tr("TREE_ITEM_LAW")) ),
  _propagationDirItem( new QStandardItem(tr("TREE_ITEM_PROPAGATION")) ),

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
//   setColumnCount( 4 ); //CS_TEST
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

void DocumentModel::setName(const QString& name)
{
  _hexaDocument->setName( name.toLocal8Bit().constData() );
  emit nameChanged(name);
}

QString DocumentModel::getName()
{
  return _hexaDocument->getName();
}

HEXA_NS::EltBase* DocumentModel::getHexaPtr(const QModelIndex& iElt)
{
	HEXA_NS::EltBase *elt = NULL;
	switch ( /*data(iElt, HEXA_TREE_ROLE).toInt()*/iElt.data(HEXA_TREE_ROLE).toInt() ){
		case VERTEX_TREE : elt = getHexaPtr<HEXA_NS::Vertex*>(iElt); break;
		case EDGE_TREE : elt = getHexaPtr<HEXA_NS::Edge*>(iElt); break;
		case QUAD_TREE : elt = getHexaPtr<HEXA_NS::Quad*>(iElt); break;
		case HEXA_TREE : elt = getHexaPtr<HEXA_NS::Hexa*>(iElt); break;
		case VECTOR_TREE : elt = getHexaPtr<HEXA_NS::Vector*>(iElt); break;
		case CYLINDER_TREE : elt = getHexaPtr<HEXA_NS::Cylinder*>(iElt); break;
		case PIPE_TREE : elt = getHexaPtr<HEXA_NS::Pipe*>(iElt); break;
		case ELEMENTS_TREE : elt = getHexaPtr<HEXA_NS::Elements*>(iElt); break;
		case CROSSELEMENTS_TREE : elt = getHexaPtr<HEXA_NS::CrossElements*>(iElt); break;
	}
	return elt;
}

DocumentModel::GeomObj* DocumentModel::convertToGeomObj(GEOM::GeomObjPtr geomObjPtr)
{
	TopoDS_Shape aShape;
	DocumentModel::GeomObj *res = NULL;

	if ( geomObjPtr && GEOMBase::GetShape(geomObjPtr.get(), aShape) && !aShape.IsNull() )
	{
		res = new DocumentModel::GeomObj;
		if (res == NULL) return NULL;
		QString mainShapeEntry;
		int     subId = -1;
		QString brep;

		if ( geomObjPtr->IsMainShape() ){
			mainShapeEntry = geomObjPtr->GetStudyEntry();
			brep =  shape2string( aShape ).c_str();
		} else {
			TopoDS_Shape shape;
			TopoDS_Shape subshape;
			GEOM::GEOM_Object_var mainShape = geomObjPtr->GetMainShape();
			mainShapeEntry = mainShape->GetStudyEntry();
			// CS_TODO : à optimiser
			bool okShape = GEOMBase::GetShape( mainShape , shape);//,const TopAbs_ShapeEnum type = TopAbs_SHAPE );
			bool oksubShape = GEOMBase::GetShape( geomObjPtr.get(), subshape );//,const TopAbs_ShapeEnum type = TopAbs_SHAPE );
			if ( okShape && oksubShape ){
				brep =  shape2string( subshape ).c_str();
				subId = GEOMBase::GetIndex( subshape, shape );
			}
		}
		res->name  = GEOMBase::GetName( geomObjPtr.get() );
		res->entry = mainShapeEntry;
		res->subid = QString::number(subId);
		res->brep  = brep;
		res->start = 0.;
		res->end   = 1.;
	}

	return res;
}


void DocumentModel::load( const QString& xmlFileName ) // Fill Data
{
  MESSAGE("HEXABLOCKGUI::load()  => "<<xmlFileName.toStdString());
  _hexaDocument->loadXml(xmlFileName.toLocal8Bit().constData() );
  clearAll();

  fillData();
  fillBuilder();
  fillAssociation();
  fillGroups();
  fillMesh();

  emit patternDataChanged();

  // BUILDER, ASSOCIATION, GROUPS, ... CS_TODO _fillBuilderFrom( _hexaDocument );
}

void DocumentModel::save( const QString& xmlFileName )
{
  MESSAGE("HEXABLOCKGUI::save()  => "<<xmlFileName.toStdString());
  _hexaDocument->save( xmlFileName.toLocal8Bit().constData() );
}

void DocumentModel::updateData()
{
  clearData();
  fillData();
  clearMesh();
  fillMesh();
  emit patternDataChanged();
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
  // DATA
  HEXA_NS::Vertex *v     = NULL;
  VertexItem      *vItem = NULL;
  for ( int i=0; i<_hexaDocument->countVertex(); ++i ){
    v = _hexaDocument->getVertex(i);

    vItem = new VertexItem(v, _entry);
    _vertexDirItem->appendRow(vItem);
  }

  HEXA_NS::Edge *e     = NULL;
  EdgeItem      *eItem = NULL;
  for ( int i=0; i<_hexaDocument->countEdge(); ++i ){
    e = _hexaDocument->getEdge(i);
    eItem = new EdgeItem(e, _entry);
    _edgeDirItem->appendRow(eItem);
  }

  HEXA_NS::Quad *q     = NULL;
  QuadItem      *qItem = NULL;
  for ( int i=0; i<_hexaDocument->countQuad(); ++i ){
    q = _hexaDocument->getQuad(i);
    qItem = new QuadItem(q, _entry);
    _quadDirItem->appendRow(qItem);
  }

  HEXA_NS::Hexa *h     = NULL;
  HexaItem      *hItem = NULL;
  for ( int i=0; i<_hexaDocument->countHexa(); ++i ){
    h = _hexaDocument->getHexa(i);
    hItem = new HexaItem(h, _entry);
    _hexaDirItem->appendRow(hItem);
  }
}


void DocumentModel::fillBuilder() 
{
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
    cItem = new CylinderItem(c, _entry);
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
}
void DocumentModel::fillAssociation()
{

}

//------------------------------------------- NEW ITEMS
//void DocumentModel::fillGroups()
//{
//  HEXA_NS::Group *g     = NULL;
//  GroupItem      *gItem = NULL;
//  for ( int i=0; i<_hexaDocument->countGroup(); ++i ){
//    g = _hexaDocument->getGroup(i);
//    gItem = new GroupItem(g, _entry);
//    _groupDirItem->appendRow(gItem);
//  }
//}
//
//void DocumentModel::fillMesh()
//{
//  HEXA_NS::Law *l     = NULL;
//  LawItem      *lItem = NULL;
//  for ( int i=0; i<_hexaDocument->countLaw(); ++i ){
//    l = _hexaDocument->getLaw(i);
//    lItem = new LawItem(l);
//    _lawDirItem->appendRow(lItem);
//  }
//
//  HEXA_NS::Propagation *p     = NULL;
//  PropagationItem      *pItem = NULL;
//  for ( int i=0; i<_hexaDocument->countPropagation(); ++i ){
//    p = _hexaDocument->getPropagation(i);
//    pItem = new PropagationItem(p, _entry);
//    pItem->setText(QString("Propagation%1").arg(i) );
//    _propagationDirItem->appendRow(pItem);
//  }
//}
//----------------------------------------------- END NEW ITEMS

void DocumentModel::fillGroups()
{
  HEXA_NS::Group *g     = NULL;
  GroupItem      *gItem = NULL;
  for ( int i=0; i<_hexaDocument->countGroup(); ++i ){
    g = _hexaDocument->getGroup(i);
    //std::cout<<"getGroup => "<< i << std::endl;
    gItem = new GroupItem(g);
    gItem->setData( _entry, HEXA_DOC_ENTRY_ROLE );
    _groupDirItem->appendRow(gItem);
  } 
}

void DocumentModel::fillMesh()
{
  //   _lawDirItem
  HEXA_NS::Law *l     = NULL;
  LawItem      *lItem = NULL;
  for ( int i=0; i<_hexaDocument->countLaw(); ++i ){
    l = _hexaDocument->getLaw(i);
    lItem = new LawItem(l);
    _lawDirItem->appendRow(lItem);
  }

  //   _propagationDirItem
  HEXA_NS::Propagation *p     = NULL;
  PropagationItem      *pItem = NULL;
  for ( int i=0; i<_hexaDocument->countPropagation(); ++i ){
    p = _hexaDocument->getPropagation(i);
    pItem = new PropagationItem(p);
    pItem->setText(QString("Propagation%1").arg(i) );
    pItem->setData( _entry, HEXA_DOC_ENTRY_ROLE );
    _propagationDirItem->appendRow(pItem);
  }
}

HEXA_NS::Hexa* DocumentModel::getQuadHexa(HEXA_NS::Quad* quad)
{
	HEXA_NS::Hexa* hexa;
	for ( int i=0; i<_hexaDocument->countHexa(); ++i ){
		hexa = _hexaDocument->getHexa(i);
		if (hexa->findQuad(quad) > -1) return hexa;
	}
	return NULL;
}


Qt::ItemFlags DocumentModel::flags(const QModelIndex &index) const
{
  Qt::ItemFlags flags;

  if (!index.isValid()) return Qt::ItemIsEnabled;

  if ( _disallowEdition ){
    return QAbstractItemModel::flags(index) | Qt::ItemFlags( ~Qt::ItemIsEditable );
  } else {
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
  }
}

void DocumentModel::allowEdition()
{
  MESSAGE("DocumentModel::allowEdition(){");
  _disallowEdition = false;
  MESSAGE("}");
}

void DocumentModel::disallowEdition()
{
  MESSAGE("DocumentModel::disallowEdition (){");
  _disallowEdition = true;
  MESSAGE("}");
}

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

void DocumentModel::allowVertexSelectionOnly()
{
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

void DocumentModel::allowElementsSelectionOnly()
{
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


void DocumentModel::setName( const QModelIndex& iElt, const QString& name )
{
	HEXA_NS::EltBase *elt = NULL;

	switch ( data(iElt, HEXA_TREE_ROLE).toInt() ){
	case GROUP_TREE :
	{
		HEXA_NS::Group* grp = iElt.data( HEXA_DATA_ROLE ).value< HEXA_NS::Group* >();
		grp->setName( name.toLatin1().data() );
		break;
	}
	case LAW_TREE :
	{
		HEXA_NS::Law* l = iElt.data( HEXA_DATA_ROLE ).value< HEXA_NS::Law* >();
		l->setName( name.toLatin1().data() );
		break;
	}
	//   case PROPAGATION_TREE : elt = iElt.data( HEXA_DATA_ROLE ).value< HEXA_NS::Propagation* >(); break;
	default: elt = getHexaPtr(iElt);
	}

	if ( elt != NULL ) elt->setName( name.toStdString() );
	setData( iElt, name );

}

bool DocumentModel::clearEltAssociations( const QModelIndex& iElt )
{
  bool isOk = false;
  HEXA_NS::EltBase *elt = getHexaPtr(iElt);

  if ( elt != NULL ){
    elt->clearAssociation();
    setData( iElt, QVariant(), HEXA_ASSOC_ENTRY_ROLE );
    isOk = true;
  }

  return isOk;
}

QModelIndex DocumentModel::addVertex( double x, double y, double z )
{
  QModelIndex vertexIndex;

  HEXA_NS::Vertex* hv = _hexaDocument->addVertex(x, y, z);
  if ( hv->isValid() ){
    VertexItem* v = new VertexItem(hv, _entry);
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

  HEXA_NS::Vertex* hv0 = getHexaPtr<HEXA_NS::Vertex*>(i_v0);
  HEXA_NS::Vertex* hv1 = getHexaPtr<HEXA_NS::Vertex*>(i_v1);

  HEXA_NS::Edge* he = _hexaDocument->addEdge( hv0, hv1 );

  if ( he->isValid() ){
    EdgeItem* e = new EdgeItem(he, _entry);
    _edgeDirItem->appendRow(e);
    edgeIndex = e->index();
    emit patternDataChanged();
  } else {
    delete he;
  }
  return edgeIndex;
}

QModelIndex DocumentModel::addEdgeVector( const QModelIndex &i_v, const QModelIndex &i_vec )
{
  QModelIndex edgeIndex;

  HEXA_NS::Vertex* hv   = getHexaPtr<HEXA_NS::Vertex*>(i_v);
  HEXA_NS::Vector* hvec = getHexaPtr<HEXA_NS::Vector*>(i_vec);

  if (!hv || !hvec) return edgeIndex;

  HEXA_NS::Edge* he = _hexaDocument->addEdge( hv, hvec );
  if (!he) return edgeIndex;
  HEXA_NS::Vertex* hv2 = he->getAval(); //the new vertex resulting from the creation of the edge
  if (!hv2) return edgeIndex;

  //ADD the elements in the treeview
  if ( he->isValid() ){
	//The Edge
    EdgeItem* e = new EdgeItem(he, _entry);
    _edgeDirItem->appendRow(e);

    //The Vertex
    VertexItem* v = new VertexItem(hv2, _entry);
    _vertexDirItem->appendRow(v);

    edgeIndex = e->index();
    emit patternDataChanged();
  } else
    delete he;

  return edgeIndex;
}

QModelIndex DocumentModel::addQuadVertices( const QModelIndex &i_v0, const QModelIndex &i_v1,
                                            const QModelIndex &i_v2, const QModelIndex &i_v3 )
{ //CS_TODO : gestion erreur
  QModelIndex quadIndex;

  HEXA_NS::Vertex* hv0 = getHexaPtr<HEXA_NS::Vertex*>(i_v0);
  HEXA_NS::Vertex* hv1 = getHexaPtr<HEXA_NS::Vertex*>(i_v1);
  HEXA_NS::Vertex* hv2 = getHexaPtr<HEXA_NS::Vertex*>(i_v2);
  HEXA_NS::Vertex* hv3 = getHexaPtr<HEXA_NS::Vertex*>(i_v3);

  if ( hv0 and hv1 and hv2 and hv3 ){
    HEXA_NS::Quad* hq = _hexaDocument->addQuadVertices( hv0, hv1, hv2, hv3 );
    if ( hq->isValid() ){
      QuadItem* q = new QuadItem(hq, _entry);
      _quadDirItem->appendRow(q);
      quadIndex = q->index();
      emit patternDataChanged();
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

  HEXA_NS::Edge* he0 = getHexaPtr<HEXA_NS::Edge*>(e0);
  HEXA_NS::Edge* he1 = getHexaPtr<HEXA_NS::Edge*>(e1);
  HEXA_NS::Edge* he2 = getHexaPtr<HEXA_NS::Edge*>(e2);
  HEXA_NS::Edge* he3 = getHexaPtr<HEXA_NS::Edge*>(e3);

  if ( he0 and he1 and he2 and he3 ){
    HEXA_NS::Quad* hq = _hexaDocument->addQuad( he0, he1, he2, he3 );
    if ( hq->isValid() ){
      QuadItem* q = new QuadItem(hq, _entry);
      _quadDirItem->appendRow(q);
      quadIndex = q->index();
      emit patternDataChanged();
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

  HEXA_NS::Vertex* hv0 = getHexaPtr<HEXA_NS::Vertex*>(iv0);
  HEXA_NS::Vertex* hv1 = getHexaPtr<HEXA_NS::Vertex*>(iv1);
  HEXA_NS::Vertex* hv2 = getHexaPtr<HEXA_NS::Vertex*>(iv2);
  HEXA_NS::Vertex* hv3 = getHexaPtr<HEXA_NS::Vertex*>(iv3);
  HEXA_NS::Vertex* hv4 = getHexaPtr<HEXA_NS::Vertex*>(iv4);
  HEXA_NS::Vertex* hv5 = getHexaPtr<HEXA_NS::Vertex*>(iv5);
  HEXA_NS::Vertex* hv6 = getHexaPtr<HEXA_NS::Vertex*>(iv6);
  HEXA_NS::Vertex* hv7 = getHexaPtr<HEXA_NS::Vertex*>(iv7);

  HEXA_NS::Hexa* hh = _hexaDocument->addHexaVertices( hv0, hv1, hv2, hv3,
                                                      hv4, hv5, hv6, hv7 );

  if ( hh->isValid() ){
    HexaItem* h = new HexaItem(hh, _entry);
    _hexaDirItem->appendRow(h);
    iHexa = h->index();
    emit patternDataChanged();
  } else {
    delete hh;
  }

  return iHexa;
}

QModelIndex DocumentModel::addHexaQuad( const QModelIndex &i_q0, const QModelIndex &i_q1,
    const QModelIndex &i_q2,const QModelIndex &i_q3, const QModelIndex &i_q4, const QModelIndex &i_q5 )
{ //CS_TODO : gestion erreur
  QModelIndex hexaIndex;

  HEXA_NS::Quad* hq0 = getHexaPtr<HEXA_NS::Quad*>(i_q0);
  HEXA_NS::Quad* hq1 = getHexaPtr<HEXA_NS::Quad*>(i_q1);
  HEXA_NS::Quad* hq2 = getHexaPtr<HEXA_NS::Quad*>(i_q2);
  HEXA_NS::Quad* hq3 = getHexaPtr<HEXA_NS::Quad*>(i_q3);
  HEXA_NS::Quad* hq4 = getHexaPtr<HEXA_NS::Quad*>(i_q4);
  HEXA_NS::Quad* hq5 = getHexaPtr<HEXA_NS::Quad*>(i_q5);

  HEXA_NS::Hexa* hh = _hexaDocument->addHexa( hq0, hq1, hq2, hq3, hq4, hq5 );

  if ( hh->isValid() ){
    HexaItem* h = new HexaItem(hh, _entry);
    _hexaDirItem->appendRow(h);
    hexaIndex = h->index();
    emit patternDataChanged();
  } else
    delete hh;

  return hexaIndex;
}

QModelIndex DocumentModel::addHexaQuads( const QModelIndexList &iquads)
{ 
  QModelIndex hexaIndex;

  HEXA_NS::Hexa* hh = NULL;
  HEXA_NS::Quad* hq0, *hq1, *hq2, *hq3, *hq4, *hq5 = NULL;

  hq0 = getHexaPtr<HEXA_NS::Quad*>(iquads.value(0));
  hq1 = getHexaPtr<HEXA_NS::Quad*>(iquads.value(1));
  hq2 = getHexaPtr<HEXA_NS::Quad*>(iquads.value(2));
  hq3 = getHexaPtr<HEXA_NS::Quad*>(iquads.value(3));
  hq4 = getHexaPtr<HEXA_NS::Quad*>(iquads.value(4));
  hq5 = getHexaPtr<HEXA_NS::Quad*>(iquads.value(5));

  if ( hq0 && hq1 && hq2 && hq3 && hq4 && hq5 )
	  hh = _hexaDocument->addHexa( hq0, hq1, hq2, hq3, hq4, hq5 );
  else if ( hq0 && hq1 && hq2 && hq3 && hq4 )
	  hh = _hexaDocument->addHexa5Quads( hq0, hq1, hq2, hq3, hq4 );
  else if ( hq0 && hq1 && hq2 && hq3 )
	  hh = _hexaDocument->addHexa4Quads( hq0, hq1, hq2, hq3 );
  else if ( hq0 && hq1 && hq2)
	  hh = _hexaDocument->addHexa3Quads( hq0, hq1, hq2 );
  else if ( hq0 && hq1 )
	  hh = _hexaDocument->addHexa2Quads( hq0, hq1);

  if ( hh && hh->isValid() ){
	updateData();
	unsigned int nbRows = _hexaDirItem->rowCount();
	if (nbRows > 0)
		hexaIndex = _hexaDirItem->child(nbRows-1)->index();
  } else
    delete hh;

  return hexaIndex;
}

// Vector addVector( in double dx, in double dy, in double dz )
//         raises (SALOME::SALOME_Exception);
QModelIndex DocumentModel::addVector( double dx, double dy, double dz )
{
  QModelIndex vectorIndex;

  HEXA_NS::Vector* hv = _hexaDocument->addVector(dx, dy, dz);

  if ( hv->isValid() ){
    VectorItem* v = new VectorItem(hv);
    _vectorDirItem->appendRow(v);
    vectorIndex = v->index();

  } else
    delete hv;

  return vectorIndex;
}

QModelIndex DocumentModel::addVectorVertices( const QModelIndex &iv0, const QModelIndex &iv1 )
{
  QModelIndex iVec;

  HEXA_NS::Vertex* hv0 = getHexaPtr<HEXA_NS::Vertex*>(iv0);
  HEXA_NS::Vertex* hv1 = getHexaPtr<HEXA_NS::Vertex*>(iv1);

  HEXA_NS::Vector* hvec = _hexaDocument->addVectorVertices( hv0, hv1 );

  if ( hvec->isValid() ){
    VectorItem* vec = new VectorItem(hvec);
    _vectorDirItem->appendRow(vec);
    iVec = vec->index();
  } else {
    delete hvec;
  }

  return iVec;
}

QModelIndex DocumentModel::addCylinder( const QModelIndex &iv, const QModelIndex &ivec, double r,  double h )
{
  QModelIndex iCyl;

  HEXA_NS::Vertex* hv   = getHexaPtr<HEXA_NS::Vertex*>(iv);
  HEXA_NS::Vector* hvec = getHexaPtr<HEXA_NS::Vector*>(ivec);

  HEXA_NS::Cylinder* hcyl = _hexaDocument->addCylinder( hv, hvec, r, h );

  if ( hcyl->isValid() ){
    CylinderItem* cyl = new CylinderItem(hcyl);
    _cylinderDirItem->appendRow(cyl);
    iCyl = cyl->index();
  } else {
    delete hcyl;
  }

  return iCyl;
}

QModelIndex DocumentModel::addPipe( const QModelIndex &iv, const QModelIndex &ivec, double ri, double re, double h )
{
  QModelIndex iPipe;

  HEXA_NS::Vertex* hv   = getHexaPtr<HEXA_NS::Vertex*>(iv);
  HEXA_NS::Vector* hvec = getHexaPtr<HEXA_NS::Vector*>(ivec);

  HEXA_NS::Pipe* hPipe = _hexaDocument->addPipe( hv, hvec, ri, re, h );

  if ( hPipe->isValid() ){
    PipeItem* pipe = new PipeItem(hPipe);
    _pipeDirItem->appendRow(pipe);
    iPipe = pipe->index();
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
  //std::cout<<"makeCartesian begin"<<std::endl; 

  HEXA_NS::Vertex* hpt    = getHexaPtr<HEXA_NS::Vertex*>(i_pt);
  HEXA_NS::Vector* hvec_x = getHexaPtr<HEXA_NS::Vector*>(i_vec_x);
  HEXA_NS::Vector* hvec_y = getHexaPtr<HEXA_NS::Vector*>(i_vec_y);
  HEXA_NS::Vector* hvec_z = getHexaPtr<HEXA_NS::Vector*>(i_vec_z);

  HEXA_NS::Elements* new_helts = _hexaDocument->makeCartesian( hpt,
                                                               hvec_x, hvec_y, hvec_z,
                                                               nx, ny, nz );

  if ( new_helts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* eltsItem = new ElementsItem(new_helts);
    _elementsDirItem->appendRow(eltsItem);
    eltsIndex = eltsItem->index();
  } else
    delete new_helts;

  return eltsIndex;
}

QModelIndex DocumentModel::makeCartesian( const QModelIndex& ivex,
                                          const QModelIndex& ivec,
                                          int nx, int ny, int nz )
{
  QModelIndex iElts;

  HEXA_NS::Vertex* hVex = getHexaPtr<HEXA_NS::Vertex*>(ivex);
  HEXA_NS::Vector* hVec = getHexaPtr<HEXA_NS::Vector*>(ivec);

  HEXA_NS::Elements* hElts = _hexaDocument->makeCartesian( hVex,
                                                           hVec,
                                                           nx, ny, nz );
  if ( hElts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
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

  HEXA_NS::Vertex* hpt    = getHexaPtr<HEXA_NS::Vertex*>(i_pt);
  HEXA_NS::Vector* hvec_x = getHexaPtr<HEXA_NS::Vector*>(i_vec_x);
  HEXA_NS::Vector* hvec_z = getHexaPtr<HEXA_NS::Vector*>(i_vec_z);

  HEXA_NS::Elements* new_helts = _hexaDocument->makeCylindrical( hpt, hvec_x, hvec_z, dr, da, dl, nr, na, nl, fill );

  if ( new_helts->isValid() ){
    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(new_helts);
    _elementsDirItem->appendRow(eltsItem);
    eltsIndex = eltsItem->index();
  } else {
    delete new_helts;
  }

  return eltsIndex;
}

QModelIndex DocumentModel::makeCylindricals( 
    const QModelIndex& icenter, const QModelIndex& ibase, const QModelIndex& iheight,
    QList< double> radius, QList<double> angles, QList<double> heights, 
    bool fill ) //HEXA3
{
  QModelIndex eltsIndex;

  HEXA_NS::Vertex* hcenter = getHexaPtr<HEXA_NS::Vertex*>(icenter);
  HEXA_NS::Vector* hbase   =getHexaPtr<HEXA_NS::Vector*>(ibase);
  HEXA_NS::Vector* hheight = getHexaPtr<HEXA_NS::Vector*>(iheight);

//   HEXA_NS::Elements* helts;
  std::vector<double> r = radius.toVector().toStdVector();
  std::vector<double> a = angles.toVector().toStdVector();
  std::vector<double> h = heights.toVector().toStdVector();

  HEXA_NS::Elements* helts = _hexaDocument->makeCylindricals(
             hcenter, hbase, hheight,
             r, a, h,
             fill );

  if ( helts->isValid() ){
    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(helts);
    _elementsDirItem->appendRow(eltsItem);
    eltsIndex = eltsItem->index();
  } else {
    delete helts;
  }
  return eltsIndex;
}

QModelIndex DocumentModel::makeSpherical( const QModelIndex& iv, const QModelIndex& ivec, int nb, double k)
{
  QModelIndex iElts;

  HEXA_NS::Vertex* hv   = getHexaPtr<HEXA_NS::Vertex*>(iv);
  HEXA_NS::Vector* hvec = getHexaPtr<HEXA_NS::Vector*>(ivec);

  HEXA_NS::Elements* hElts = _hexaDocument->makeSpherical( hv, hvec, nb, k );

  if ( hElts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
  } else {
    delete hElts;
  }

  return iElts;
}

QModelIndex DocumentModel::makeSpherical( const QModelIndex& icenter, double radius, int nb, double k )
{
  QModelIndex iElts;

  HEXA_NS::Vertex* hcenter = getHexaPtr<HEXA_NS::Vertex*>(icenter);

  HEXA_NS::Elements* helts = _hexaDocument->makeSpherical( hcenter, radius, nb, k );

  if ( helts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* eltsItem = new ElementsItem(helts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();
  } else {
    delete helts;
  }

  return iElts;
}

QModelIndex DocumentModel::makeCylinder( const QModelIndex& icyl, const QModelIndex& ivec,
                                         int nr, int na, int nl )
{
  QModelIndex iElts;

  HEXA_NS::Cylinder* hcyl = getHexaPtr<HEXA_NS::Cylinder*>(icyl);
  HEXA_NS::Vector* hvec   = getHexaPtr<HEXA_NS::Vector*>(ivec);

  HEXA_NS::Elements* hElts = _hexaDocument->makeCylinder( hcyl, hvec, nr, na, nl );
  if ( hElts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
  } else {
    delete hElts;
  }

  return iElts;
}

QModelIndex DocumentModel::makePipe( const QModelIndex& ipipe, const QModelIndex& ivecx, 
                                     int nr, int na, int nl )
{ 
  QModelIndex iElts;

  HEXA_NS::Pipe*   hPipe  = getHexaPtr<HEXA_NS::Pipe*>(ipipe);
  HEXA_NS::Vector* hVecx  = getHexaPtr<HEXA_NS::Vector*>(ivecx);

  HEXA_NS::Elements* hElts = _hexaDocument->makePipe( hPipe, hVecx, nr, na, nl );

  if ( hElts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
  } else {
    delete hElts;
  }

  return iElts;
}

QModelIndex DocumentModel::makeCylinders(const QModelIndex& icyl1, const QModelIndex& icyl2)
{ //CS_TODO
  QModelIndex iCrossElts;
  
  HEXA_NS::Cylinder* hCyl1  = getHexaPtr<HEXA_NS::Cylinder*>(icyl1);
  HEXA_NS::Cylinder* hCyl2  = getHexaPtr<HEXA_NS::Cylinder*>(icyl2);

  HEXA_NS::CrossElements* hCrossElts = _hexaDocument->makeCylinders( hCyl1, hCyl2 );
  
  if ( hCrossElts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* crossElts = new ElementsItem(hCrossElts);
    _crossElementsDirItem->appendRow(crossElts);
    iCrossElts = crossElts->index();    
  } else {
    delete hCrossElts;
  }
    
  return iCrossElts;
}

//
QModelIndex DocumentModel::makePipes( const QModelIndex& ipipe1, const QModelIndex& ipipe2 )
{ 
  QModelIndex iCrossElts;
  
  HEXA_NS::Pipe* hPipe1  = getHexaPtr<HEXA_NS::Pipe*>(ipipe1);
  HEXA_NS::Pipe* hPipe2  = getHexaPtr<HEXA_NS::Pipe*>(ipipe2);

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

QModelIndex DocumentModel::makeRind( const QModelIndex& icenter, 
                    const QModelIndex& ivecx, const QModelIndex& ivecz,
                    double  radext, double radint, double radhole,
                    const QModelIndex& iplorig,
                    int nrad, int nang, int nhaut )
{
  QModelIndex iElts;

  HEXA_NS::Vertex* hcenter = getHexaPtr<HEXA_NS::Vertex*>(icenter);
  HEXA_NS::Vector* hvecx   = getHexaPtr<HEXA_NS::Vector*>(ivecx);
  HEXA_NS::Vector* hvecz   = getHexaPtr<HEXA_NS::Vector*>(ivecz);
  HEXA_NS::Vertex* hplorig = getHexaPtr<HEXA_NS::Vertex*>(iplorig);

  HEXA_NS::Elements* hElts = _hexaDocument->makeRind( hcenter,
                            hvecx, hvecz,
                            radext, radint, radhole,
                            hplorig,
                            nrad, nang, nhaut );

  if ( hElts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* eltsItem = new ElementsItem(hElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();
  } else {
    delete hElts;
  }

  return iElts;
}

QModelIndex DocumentModel::makePartRind( const QModelIndex& icenter, 
                    const QModelIndex& ivecx, const QModelIndex& ivecz,
                    double  radext, double radint, double radhole,
                    const QModelIndex& iplorig, double angle,
                    int nrad, int nang, int nhaut )
{
  QModelIndex iElts;

  HEXA_NS::Vertex* hcenter = getHexaPtr<HEXA_NS::Vertex*>(icenter);
  HEXA_NS::Vector* hvecx   = getHexaPtr<HEXA_NS::Vector*>(ivecx);
  HEXA_NS::Vector* hvecz   = getHexaPtr<HEXA_NS::Vector*>(ivecz);
  HEXA_NS::Vertex* hplorig = getHexaPtr<HEXA_NS::Vertex*>(iplorig);

  HEXA_NS::Elements* hElts = _hexaDocument->makePartRind( hcenter,
                            hvecx, hvecz,
                            radext, radint, radhole,
                            hplorig, angle,
                            nrad, nang, nhaut );

  if ( hElts->isValid() ){
    updateData();
    ElementsItem* eltsItem = new ElementsItem(hElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();
  } else {
    delete hElts;
  }

  return iElts;
}

QModelIndex DocumentModel::makeSphere( const QModelIndex& icenter, 
                    const QModelIndex& ivecx, const QModelIndex& ivecz,
                    double radius, double radhole,
                    const QModelIndex& iplorig,
                    int nrad, int nang, int nhaut )
{
  QModelIndex iElts;

  HEXA_NS::Vertex* hcenter = getHexaPtr<HEXA_NS::Vertex*>(icenter);
  HEXA_NS::Vector* hvecx   = getHexaPtr<HEXA_NS::Vector*>(ivecx);
  HEXA_NS::Vector* hvecz   = getHexaPtr<HEXA_NS::Vector*>(ivecz);
  HEXA_NS::Vertex* hplorig = getHexaPtr<HEXA_NS::Vertex*>(iplorig);

  HEXA_NS::Elements* hElts = _hexaDocument->makeSphere( hcenter,
                                                hvecx, hvecz, 
                                                radius, radhole,
                                                hplorig, 
                                                nrad, nang, nhaut);

  if ( hElts->isValid() ){
    updateData();
    ElementsItem* eltsItem = new ElementsItem(hElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();
  } else {
    delete hElts;
  }

  return iElts;
}

QModelIndex DocumentModel::makePartSphere( const QModelIndex& icenter, 
                    const QModelIndex& ivecx, const QModelIndex& ivecz,
                    double  radius, double radhole,
                    const QModelIndex& iplorig, double angle,
                    int nrad, int nang, int nhaut )
{
  QModelIndex iElts;

  HEXA_NS::Vertex* hcenter = getHexaPtr<HEXA_NS::Vertex*>(icenter);
  HEXA_NS::Vector* hvecx   = getHexaPtr<HEXA_NS::Vector*>(ivecx);
  HEXA_NS::Vector* hvecz   = getHexaPtr<HEXA_NS::Vector*>(ivecz);
  HEXA_NS::Vertex* hplorig = getHexaPtr<HEXA_NS::Vertex*>(iplorig);

  HEXA_NS::Elements* hElts = _hexaDocument->makePartSphere( hcenter,
                                                hvecx, hvecz,
                                                radius, radhole,
                                                hplorig, angle, 
                                                nrad, nang, nhaut);

  if ( hElts->isValid() ){
    updateData();
    ElementsItem* eltsItem = new ElementsItem(hElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();
  } else {
    delete hElts;
  }

  return iElts;
}

// ************  EDIT HEXABLOCK MODEL ************

bool DocumentModel::updateVertex( const QModelIndex& ivertex, double x, double y, double z )
{
  bool ret = false;

  //HEXA_NS::Vertex* hVertex = ivertex.data(HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>(); //CS_TODO?  pareil pour toutes les autres méthodes du modèle?
  HEXA_NS::Vertex* hVertex = getHexaPtr<HEXA_NS::Vertex*>(ivertex);

  if ( hVertex ){
//     hVertex->setName( name.toStdString() );
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
  HEXA_NS::Hexa* hHexa = getHexaPtr<HEXA_NS::Hexa*>(ihexa);


  int r = _hexaDocument->removeHexa( hHexa );
  if ( r == HOK ){
    updateData();
    ret = true;
  } else if ( r == HERR ){    
    ret = false;
  }
  
  return ret;
}

bool DocumentModel::removeConnectedHexa( const QModelIndex& ihexa )
{
  bool ret = false;
  HEXA_NS::Hexa* hHexa = getHexaPtr<HEXA_NS::Hexa*>(ihexa);
  
  int r = _hexaDocument->removeConnectedHexa( hHexa );
  
  if ( r == HOK ){    
    updateData();
    ret = true;
  } else if ( r == HERR ){    
    ret = false;
  }
  
  return ret;
}

QModelIndex DocumentModel::prismQuad( const QModelIndex& iquad, const QModelIndex& ivec, int nb)
{ 
  QModelIndex iElts;

  HEXA_NS::Quad*   hQuad = getHexaPtr<HEXA_NS::Quad*>(iquad);
  HEXA_NS::Vector* hVect = getHexaPtr<HEXA_NS::Vector*>(ivec);

  HEXA_NS::Elements* hElts = _hexaDocument->prismQuad( hQuad, hVect, nb );

  if ( hElts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
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
    hQuad = getHexaPtr<HEXA_NS::Quad*>(iquad);
    hQuads.push_back( hQuad );
  }
  HEXA_NS::Vector* hVect = getHexaPtr<HEXA_NS::Vector*>(ivec);

  HEXA_NS::Elements* hElts = _hexaDocument->prismQuads( hQuads, hVect, nb );
  if ( hElts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
  } else {
    delete hElts;
  }
  return iElts;
}

QModelIndex DocumentModel::prismQuads( const QModelIndexList& iquads, const QModelIndex& ivec, std::vector<double> layersSize, int nb)
{
	QModelIndex iElts;

	 HEXA_NS::Quads   hQuads;
	  HEXA_NS::Quad*   hQuad = NULL;
	  foreach( const QModelIndex& iquad, iquads ){
	    hQuad = getHexaPtr<HEXA_NS::Quad*>(iquad);
	    hQuads.push_back( hQuad );
	  }
	  HEXA_NS::Vector* hVect = getHexaPtr<HEXA_NS::Vector*>(ivec);

	HEXA_NS::Elements* hElts = _hexaDocument->prismQuadsVec( hQuads, hVect, layersSize, nb );
	if ( hElts->isValid() ){
		updateData(); //CS_TODO more or less?
		ElementsItem* elts = new ElementsItem(hElts);
		_elementsDirItem->appendRow(elts);
		iElts = elts->index();
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

  HEXA_NS::Quad*   hQuadStart  = getHexaPtr<HEXA_NS::Quad*>(iquadstart);
  HEXA_NS::Quad*   hQuadDest   = getHexaPtr<HEXA_NS::Quad*>(iquaddest);

  HEXA_NS::Vertex* hVertex0 = getHexaPtr<HEXA_NS::Vertex*>(iv0);
  HEXA_NS::Vertex* hVertex1 = getHexaPtr<HEXA_NS::Vertex*>(iv1);
  HEXA_NS::Vertex* hVertex2 = getHexaPtr<HEXA_NS::Vertex*>(iv2);
  HEXA_NS::Vertex* hVertex3 = getHexaPtr<HEXA_NS::Vertex*>(iv3);

  HEXA_NS::Elements* hElts = _hexaDocument->joinQuad( hQuadStart, hQuadDest,
                        hVertex0,  hVertex1,  hVertex2,  hVertex3, nb );

  if ( hElts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
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

  HEXA_NS::Vertex* hVertex0 = getHexaPtr<HEXA_NS::Vertex*>(iv0);
  HEXA_NS::Vertex* hVertex1 = getHexaPtr<HEXA_NS::Vertex*>(iv1);
  HEXA_NS::Vertex* hVertex2 = getHexaPtr<HEXA_NS::Vertex*>(iv2);
  HEXA_NS::Vertex* hVertex3 = getHexaPtr<HEXA_NS::Vertex*>(iv3);

  HEXA_NS::Elements* hElts = _hexaDocument->joinQuads(
                        hQuadsStart, hQuadDest,
                        hVertex0,  hVertex1,  hVertex2,  hVertex3,
                        nb );

  if ( hElts->isValid() ){
    updateData(); //CS_TODO more or less?
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
  } else {
    delete hElts;
  }

  return iElts;
}

bool DocumentModel::mergeVertices( const QModelIndex &iv0, const QModelIndex &iv1 ) //CS_TODO : impact sur le model?
{
  bool ret = false;

  HEXA_NS::Vertex* hv0 = getHexaPtr<HEXA_NS::Vertex*>(iv0);
  HEXA_NS::Vertex* hv1 = getHexaPtr<HEXA_NS::Vertex*>(iv1);

  int r = _hexaDocument->mergeVertices( hv0, hv1 );
  if ( r == HOK ){
    updateData(); //CS_TODO more or less?
    ret = true;
  } else if ( r == HERR ){
    ret = false;
  }

  return ret;
}

bool DocumentModel::mergeEdges( const QModelIndex &ie0, const QModelIndex &ie1,
                                const QModelIndex &iv0, const QModelIndex &iv1 )
//CS_TODO : impact sur le model?
{
  bool ret = false;

  HEXA_NS::Edge* he0 = getHexaPtr<HEXA_NS::Edge*>(ie0);
  HEXA_NS::Edge* he1 = getHexaPtr<HEXA_NS::Edge*>(ie1);

  HEXA_NS::Vertex* hv0 = getHexaPtr<HEXA_NS::Vertex*>(iv0);
  HEXA_NS::Vertex* hv1 = getHexaPtr<HEXA_NS::Vertex*>(iv1);

  int r = _hexaDocument->mergeEdges( he0, he1, hv0, hv1 ); 
  if ( r == HOK ){
    updateData();
    ret = true;
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

  HEXA_NS::Vertex* hv0 = getHexaPtr<HEXA_NS::Vertex*>(iv0);
  HEXA_NS::Vertex* hv1 = getHexaPtr<HEXA_NS::Vertex*>(iv1);
  HEXA_NS::Vertex* hv2 = getHexaPtr<HEXA_NS::Vertex*>(iv2);
  HEXA_NS::Vertex* hv3 = getHexaPtr<HEXA_NS::Vertex*>(iv3);

  int r = _hexaDocument->mergeQuads( hquad0, hquad1, hv0, hv1, hv2, hv3 );
  if ( r == HOK ){
    updateData();
    ret = true;
  } else if ( r == HERR ){
    ret = false;
  }

  return ret;
}

//
QModelIndex DocumentModel::disconnectVertex( const QModelIndex& ihexa, const QModelIndex& ivertex )
{
  QModelIndex iElts;

  HEXA_NS::Hexa*   hHexa   = getHexaPtr<HEXA_NS::Hexa*>(ihexa);
  HEXA_NS::Vertex* hVertex = getHexaPtr<HEXA_NS::Vertex*>(ivertex);

  HEXA_NS::Elements* hElts = _hexaDocument->disconnectVertex( hHexa, hVertex );

  if ( hElts && hElts->isValid() ){
    updateData(); //CS_TO_CHECK
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
  } else {
    delete hElts;
  }

  return iElts;
}

QModelIndex DocumentModel::disconnectEdge( const QModelIndex& ihexa, const QModelIndex& iedge )
{
  QModelIndex iElts;

  HEXA_NS::Hexa* hHexa = getHexaPtr<HEXA_NS::Hexa*>(ihexa);
  HEXA_NS::Edge* hEdge = getHexaPtr<HEXA_NS::Edge*>(iedge);

  HEXA_NS::Elements* hElts = _hexaDocument->disconnectEdge( hHexa, hEdge );

  if ( hElts->isValid() ){
    updateData(); //CS_TO_CHECK
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
  } else {
    delete hElts;
  }

  return iElts;
}


QModelIndex DocumentModel::disconnectEdges( const QModelIndexList& ihexas, const QModelIndexList& iedges )
{
  QModelIndex iElts;

  HEXA_NS::Hexas hHexas;
  HEXA_NS::Edges hEdges;

  //Construction de la liste des edges
  HEXA_NS::Edge* hedge = NULL;
  foreach( const QModelIndex& iedge, iedges ){
    hedge = getHexaPtr<HEXA_NS::Edge*>(iedge);
    hEdges.push_back( hedge );
  }

  //Construction de la liste des hexas
  HEXA_NS::Hexa* hhexa = NULL;
  foreach( const QModelIndex& ihexa, ihexas ){
    hhexa = getHexaPtr<HEXA_NS::Hexa*>(ihexa);
    hHexas.push_back( hhexa );
  }


  HEXA_NS::Elements* hElts = _hexaDocument->disconnectEdges( hHexas, hEdges );

  if ( hElts->isValid() ){
    updateData(); //CS_TO_CHECK
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
  } else {
    delete hElts;
  }

  return iElts;
}


QModelIndex DocumentModel::disconnectQuad( const QModelIndex& ihexa, const QModelIndex& iquad )
{
  QModelIndex iElts;

  HEXA_NS::Hexa* hHexa = getHexaPtr<HEXA_NS::Hexa*>(ihexa);
  HEXA_NS::Quad* hQuad = getHexaPtr<HEXA_NS::Quad*>(iquad);

  HEXA_NS::Elements* hElts = _hexaDocument->disconnectQuad( hHexa, hQuad );

  if ( hElts->isValid() ){
    updateData(); //CS_TO_CHECK
    ElementsItem* elts = new ElementsItem(hElts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
  } else {
    delete hElts;
  }

  return iElts;
}


QModelIndex DocumentModel::cutEdge( const QModelIndex &i_e0, int nbcuts )
//CS_TODO : impact sur le model?
{
  QModelIndex iElts;

  HEXA_NS::Edge* he0 = getHexaPtr<HEXA_NS::Edge*>(i_e0);
  HEXA_NS::Elements* helts = _hexaDocument->cut( he0, nbcuts );

  if ( helts->isValid() ){
    updateData(); //CS_TODO more?
    ElementsItem* elts = new ElementsItem(helts);
    _elementsDirItem->appendRow(elts);
    iElts = elts->index();
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

  HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
  HEXA_NS::Vector*   hVec  = getHexaPtr<HEXA_NS::Vector*>(ivec);

  HEXA_NS::Elements* hNewElts = _hexaDocument->makeTranslation( hElts, hVec );

  if ( hNewElts->isValid() ){
    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(hNewElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();
  } else {
    delete hNewElts;
  }

  return iElts;
}



QModelIndex DocumentModel::makeScale( const QModelIndex& ielts, const QModelIndex& ivex, double k )
{
  QModelIndex iElts;

  HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
  HEXA_NS::Vertex*   hVex  = getHexaPtr<HEXA_NS::Vertex*>(ivex);

  HEXA_NS::Elements* hNewElts = _hexaDocument->makeScale( hElts, hVex, k );

  if ( hNewElts->isValid() ){
    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(hNewElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();
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

  HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
  HEXA_NS::Vertex*   hVex  = getHexaPtr<HEXA_NS::Vertex*>(iv);
  HEXA_NS::Vector*   hVec  = getHexaPtr<HEXA_NS::Vector*>(ivec);

  HEXA_NS::Elements* hNewElts = _hexaDocument->makeRotation( hElts, hVex, hVec, angle );

  if ( hNewElts->isValid() ){
    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(hNewElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();
  } else {
    delete hNewElts;
  }

  return iElts;
}


QModelIndex DocumentModel::makeSymmetryPoint( const QModelIndex& ielts, const QModelIndex& iv )
{
  QModelIndex iElts;

  HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
  HEXA_NS::Vertex*   hVex  = getHexaPtr<HEXA_NS::Vertex*>(iv);

  HEXA_NS::Elements* hNewElts = _hexaDocument->makeSymmetryPoint (hElts, hVex);

  if ( hNewElts->isValid() ){
    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(hNewElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();
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

  HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
  HEXA_NS::Vertex*   hVex  = getHexaPtr<HEXA_NS::Vertex*>(iv);
  HEXA_NS::Vector*   hVec  = getHexaPtr<HEXA_NS::Vector*>(ivec);


  HEXA_NS::Elements* hNewElts = _hexaDocument->makeSymmetryLine( hElts, hVex, hVec );

  if ( hNewElts->isValid() ){
    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(hNewElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();
  } else {
    delete hNewElts;
  }

  return iElts;
}


QModelIndex DocumentModel::makeSymmetryPlane( const QModelIndex& ielts, const QModelIndex& iv, const QModelIndex& ivec )
{
  QModelIndex iElts;

  HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
  HEXA_NS::Vertex*   hVex  = getHexaPtr<HEXA_NS::Vertex*>(iv);
  HEXA_NS::Vector*   hVec  = getHexaPtr<HEXA_NS::Vector*>(ivec);

  HEXA_NS::Elements* hNewElts = _hexaDocument->makeSymmetryPlane( hElts, hVex, hVec );

  if ( hNewElts->isValid() ){
    updateData(); //CS_TODO  more or less?
    ElementsItem* eltsItem = new ElementsItem(hNewElts);
    _elementsDirItem->appendRow(eltsItem);
    iElts = eltsItem->index();
  } else {
    delete hNewElts;
  }

  return iElts;
}


bool DocumentModel::performTranslation( const QModelIndex& ielts, const QModelIndex& ivec )
{
  bool ret = false;

  HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
  HEXA_NS::Vector*   hVec  = getHexaPtr<HEXA_NS::Vector*>(ivec);

  int r = _hexaDocument->performTranslation (hElts, hVec);
  if ( r == HOK ){
    updateData();
    ret = true;
  } else if ( r == HERR ){
    ret = false;
  }

  return ret;
}


//
bool DocumentModel::performScale( const QModelIndex& ielts, const QModelIndex& ivex, double k )
{
  bool ret = false;

  HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
  HEXA_NS::Vertex*   hVex  = getHexaPtr<HEXA_NS::Vertex*>(ivex);

  int r = _hexaDocument->performScale (hElts, hVex, k);
  if ( r == HOK ){
    updateData();
    ret = true;
  } else if ( r == HERR ){
    ret = false;
  }

  return ret;
}

//
bool DocumentModel::performRotation( const QModelIndex& ielts, const QModelIndex& ivex, const QModelIndex& ivec, double angle )
{
  bool ret = false;

  HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
  HEXA_NS::Vertex*   hVex  = getHexaPtr<HEXA_NS::Vertex*>(ivex);
  HEXA_NS::Vector*   hVec  = getHexaPtr<HEXA_NS::Vector*>(ivec);

  int r = _hexaDocument-> performRotation( hElts, hVex, hVec, angle );
  if ( r == HOK ){
    updateData();
    ret = true;
  } else if ( r == HERR ){
    ret = false;
  }

  return ret;
}


//
bool DocumentModel::performSymmetryPoint( const QModelIndex& ielts, const QModelIndex& ivex )
{
  bool ret = false;

  HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
  HEXA_NS::Vertex*   hVex  = getHexaPtr<HEXA_NS::Vertex*>(ivex);

  int r = _hexaDocument->performSymmetryPoint( hElts, hVex );
  if ( r == HOK ){
    updateData();
    ret = true;
  } else if ( r == HERR ){
    ret = false;
  }

  return ret;
}


bool DocumentModel::performSymmetryLine( const QModelIndex& ielts, const QModelIndex& ivex, const QModelIndex& ivec )
{
  bool ret = false;

  HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
  HEXA_NS::Vertex*   hVex  = getHexaPtr<HEXA_NS::Vertex*>(ivex);
  HEXA_NS::Vector*   hVec  = getHexaPtr<HEXA_NS::Vector*>(ivec);

  int r = _hexaDocument->performSymmetryLine( hElts, hVex, hVec );
  if ( r == HOK ){
    updateData();
    ret = true;
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

  HEXA_NS::Elements* hElts = getHexaPtr<HEXA_NS::Elements*>(ielts);
  HEXA_NS::Vertex*   hVex  = getHexaPtr<HEXA_NS::Vertex*>(ivex);
  HEXA_NS::Vector*   hVec  = getHexaPtr<HEXA_NS::Vector*>(ivec);

  int r = _hexaDocument->performSymmetryPlane( hElts, hVex, hVec );
  if ( r == HOK ){
    updateData();
    ret = true;
  } else if ( r == HERR ){
    ret = false;
  }

  return ret;
}

QModelIndex DocumentModel::revolutionQuads( const QModelIndexList& istartquads,
                                            const QModelIndex& icenter, 
                                            const QModelIndex& ivecaxis, 
                                            const QList<double>& angles )
{
  QModelIndex ielts;

  HEXA_NS::Quads   hstartquads;
  HEXA_NS::Quad*   hquad = NULL;
  foreach( const QModelIndex& iquad, istartquads){
    hquad = data( iquad, HEXA_DATA_ROLE ).value<HEXA_NS::Quad*>();
    hstartquads.push_back( hquad );
  }
  HEXA_NS::Vertex* hcenter = getHexaPtr<HEXA_NS::Vertex*>(icenter);
  HEXA_NS::Vector* haxis   = getHexaPtr<HEXA_NS::Vector*>(ivecaxis);
  std::vector<double> hangles = angles.toVector().toStdVector();

  HEXA_NS::Elements* helts = _hexaDocument->revolutionQuads( hstartquads, hcenter, haxis, hangles );

  if ( helts && helts->isValid() ){
    updateData();
    ElementsItem* eltsItem = new ElementsItem(helts);
    _elementsDirItem->appendRow(eltsItem);
    ielts = eltsItem->index();
  } else {
    delete helts;
  }

  return ielts;
}

QModelIndex DocumentModel::replace( const QModelIndexList& iquadsPattern,
                             const QModelIndex& ip1, const QModelIndex& ic1,
                             const QModelIndex& ip2, const QModelIndex& ic2,
                             const QModelIndex& ip3, const QModelIndex& ic3 )
{
  QModelIndex ielts;

  HEXA_NS::Vertex* hp1 = getHexaPtr<HEXA_NS::Vertex*>(ip1);
  HEXA_NS::Vertex* hc1 = getHexaPtr<HEXA_NS::Vertex*>(ic1);
  HEXA_NS::Vertex* hp2 = getHexaPtr<HEXA_NS::Vertex*>(ip2);
  HEXA_NS::Vertex* hc2 = getHexaPtr<HEXA_NS::Vertex*>(ic2);
  HEXA_NS::Vertex* hp3 = getHexaPtr<HEXA_NS::Vertex*>(ip3);
  HEXA_NS::Vertex* hc3 = getHexaPtr<HEXA_NS::Vertex*>(ic3);

  HEXA_NS::Quads   hquads;
  HEXA_NS::Quad*   hquad = NULL;
  foreach( const QModelIndex& iquad, iquadsPattern ){
    hquad = getHexaPtr<HEXA_NS::Quad*>(iquad);
    hquads.push_back( hquad );
  }

  HEXA_NS::Elements* helts = _hexaDocument->replace( hquads,
                                hp1, hc1, hp2, hc2, hp3, hc3 );
//   HEXA_NS::Elements* helts = NULL;

  if ( helts && helts->isValid() ){
    updateData();
    ElementsItem* eltsItem = new ElementsItem(helts);
    _elementsDirItem->appendRow(eltsItem);
    ielts = eltsItem->index();
  } else {
    delete helts;
  }

  return ielts;
}

// ************  ADD ASSOCIATION ************
//
void DocumentModel::addAssociation( const QModelIndex& iElt, const DocumentModel::GeomObj& assocIn )
{
//   assocIn.name;
  HEXA_NS::Shape* assoc = new HEXA_NS::Shape( assocIn.brep.toStdString() );//CS_TODO : delete assoc
  assoc->debut =  assocIn.start;
  assoc->fin   =  assocIn.end;
  assoc->ident =  ( assocIn.entry + "," + assocIn.subid ).toStdString();
  _assocName[ assocIn.entry ] = assocIn.name; // for getAssociations()

  QString currentAssoc, newAssoc;

  if ( data(iElt, HEXA_TREE_ROLE) == VERTEX_TREE ){
    HEXA_NS::Vertex* hVex = getHexaPtr<HEXA_NS::Vertex*>(iElt);
    hVex->setAssociation( assoc );
  } else if ( data(iElt, HEXA_TREE_ROLE) == EDGE_TREE ){
    HEXA_NS::Edge*   hEdge = getHexaPtr<HEXA_NS::Edge*>(iElt);
    hEdge->addAssociation( assoc );
  } else if ( data(iElt, HEXA_TREE_ROLE) == QUAD_TREE ){
    HEXA_NS::Quad*   hQuad  = getHexaPtr<HEXA_NS::Quad*>(iElt);
    hQuad->addAssociation( assoc );
  }

  currentAssoc = data( iElt, HEXA_ASSOC_ENTRY_ROLE ).toString();
  if ( !currentAssoc.isEmpty() ){
    newAssoc = currentAssoc + assocIn.entry + "," + assocIn.subid + ";";
  } else {
    newAssoc = assocIn.entry + "," + assocIn.subid + ";";
  }

  setData( iElt, QVariant::fromValue(newAssoc), HEXA_ASSOC_ENTRY_ROLE );
}


QList<DocumentModel::GeomObj> DocumentModel::getAssociations( const QModelIndex& iElt )
{
  MESSAGE("DocumentModel::getAssociations( "<< iElt.data().toString().toStdString() << " )" );
  QList<DocumentModel::GeomObj> res;
  DocumentModel::GeomObj        assoc;

  //std::cout << "getAssociations() start"  << std::endl;
  if ( data(iElt, HEXA_TREE_ROLE) == VERTEX_TREE ){
    MESSAGE("*  of a Vertex");
//    HEXA_NS::Vertex* hVex = data(iElt, HEXA_DATA_ROLE).value<HEXA_NS::Vertex *>();
    HEXA_NS::Vertex *hVex = getHexaPtr<HEXA_NS::Vertex *>(iElt);
    HEXA_NS::Shape* hShape = hVex->getAssociation();
    QStringList shapeID;
    if ( hShape != NULL ){
      assoc.entry = "";
      assoc.subid = QString::number(-1);
      assoc.name  = "";
      assoc.brep  = hShape->getBrep().c_str();
      assoc.start = hShape->debut;
      assoc.end   = hShape->fin;

      shapeID = QString( hShape->ident.c_str() ).split(",");
      if ( shapeID.count() == 2 ){
        assoc.entry = shapeID[0];
        assoc.subid = shapeID[1].isEmpty()? QString::number(-1) : shapeID[1];
        assoc.name  = _assocName[assoc.entry];
      }
      MESSAGE("*  assoc.entry"  << assoc.entry.toStdString() );
      MESSAGE("*  assoc.subid"  << assoc.subid.toStdString() );
      MESSAGE("*  -----------" );
      res << assoc;
    }
  } else if ( data(iElt, HEXA_TREE_ROLE) == EDGE_TREE ){
    MESSAGE("*  of an Edge");
    HEXA_NS::Edge*   hEdge = getHexaPtr<HEXA_NS::Edge *>(iElt);
    HEXA_NS::Shapes  hShapes = hEdge->getAssociations();
    QStringList shapeID;
    for ( HEXA_NS::Shapes::iterator it = hShapes.begin(); it != hShapes.end(); ++it){
      assoc.entry = "";
      assoc.subid = QString::number(-1);
      assoc.name  = "";
      assoc.brep  = (*it)->getBrep().c_str();
      assoc.start = (*it)->debut;
      assoc.end   = (*it)->fin;

      MESSAGE("*  assoc.ident " << (*it)->ident );
      shapeID = QString( (*it)->ident.c_str() ).split(",");
      if ( shapeID.count() == 2 ){
        assoc.entry = shapeID[0];
        assoc.subid = shapeID[1].isEmpty()? QString::number(-1) : shapeID[1];
        assoc.name  = _assocName[assoc.entry];
      }
      MESSAGE("*  assoc.entry" << assoc.entry.toStdString() );
      MESSAGE("*  assoc.subid" << assoc.subid.toStdString() );
      MESSAGE("*  assoc.brep"  << assoc.brep.toStdString() );
      MESSAGE("*  assoc.start" << assoc.start );
      MESSAGE("*  assoc.end"   << assoc.end );
      MESSAGE("*  -----------" );
      res << assoc;
    }
  } else if ( data(iElt, HEXA_TREE_ROLE) == QUAD_TREE ){
    MESSAGE("*  of a Quad");
    HEXA_NS::Quad*   hQuad  = getHexaPtr<HEXA_NS::Quad*>(iElt);
    HEXA_NS::Shapes  hShapes = hQuad->getAssociations();
    QStringList shapeID;
    for ( HEXA_NS::Shapes::iterator it = hShapes.begin(); it != hShapes.end(); ++it){
      assoc.entry = "";
      assoc.subid = QString::number(-1);
      assoc.name  = "";
      assoc.brep  = (*it)->getBrep().c_str();
      assoc.start = (*it)->debut;
      assoc.end   = (*it)->fin;
      MESSAGE("*  assoc.entry" << assoc.entry.toStdString() );
      MESSAGE("*  assoc.subid" << assoc.subid.toStdString() );
//       MESSAGE("*  assoc.brep"  << assoc.brep.toStdString() );
//       MESSAGE("*  assoc.start" << assoc.start );
//       MESSAGE("*  assoc.end"   << assoc.end );
      MESSAGE("*  -----------" );
      shapeID = QString( (*it)->ident.c_str() ).split(",");
      if ( shapeID.count() == 2 ){
        assoc.entry = shapeID[0];
        assoc.subid = shapeID[1].isEmpty()? QString::number(-1) : shapeID[1];
        assoc.name  = _assocName[assoc.entry];
      }

      res << assoc;
    }
  }

  MESSAGE("}" );
  return res;
}

//         int associateOpenedLine (Edge*  mstart, Edges&  mline, Shape* gstart, 
//                              double pstart, Shapes& gline, double pend);
//         int  associateClosedLine (Vertex* mfirst, Edge*  mstart, Edges&  mline, 
//                              Shape*  gstart, double pstart, Shapes& gline);
bool DocumentModel::associateOpenedLine( const QModelIndexList& iedges,
                                         const GeomObjList&     assocs,
                                         double pstart,
                                         double pend )
{
    MESSAGE("DocumentModel::associateOpenedLine() ");
    bool ret = false;
    HEXA_NS::Edge*  mstart = NULL;
    HEXA_NS::Edges  mline;
    HEXA_NS::Shape* gstart = NULL;
//     double pstart;
    HEXA_NS::Shapes gline;
//     double pend;

    HEXA_NS::Edge* hedge = NULL;
    foreach( const QModelIndex& iedge, iedges ){
        hedge = getHexaPtr<HEXA_NS::Edge*>(iedge);
        if ( mstart == NULL ){
            mstart = hedge;
            MESSAGE("*    mstart" << iedge.data().toString().toStdString() );
        } else {
            mline.push_back( hedge );
            MESSAGE("*    mline :" << iedge.data().toString().toStdString() );
        }
    }

    HEXA_NS::Shape* hshape = NULL;
    foreach( const GeomObj& anAssoc, assocs ){
        hshape = new HEXA_NS::Shape( anAssoc.brep.toStdString() );
        hshape->debut = anAssoc.start;  //0.;
        hshape->fin   = anAssoc.end;    //1.;
        hshape->ident = ( anAssoc.entry + "," + anAssoc.subid ).toStdString(); //anAssoc.entry.toStdString();

        if ( gstart == NULL ){
            gstart = hshape; // CS_TODO :gstart.debut = pstart ??
            MESSAGE("*    gstart->debut" << gstart->debut);
            MESSAGE("*    gstart->fin"   << gstart->fin);
            MESSAGE("*    gstart->ident" << gstart->ident);
            MESSAGE("*    gstart->getBrep()" << gstart->getBrep());
//             pstart = anAssoc.start;
        } else {
            gline.push_back( hshape ); //CS_TODO : hshape.fin = pend ??
            MESSAGE("*    gline->debut" << hshape->debut);
            MESSAGE("*    gline->fin"   << hshape->fin);
            MESSAGE("*    gline->ident" << hshape->ident);
            MESSAGE("*    gline->getBrep()" << hshape->getBrep());
        }
    }
//     pend = assocs.last().end;
    MESSAGE("*    pstart" << pstart );
    MESSAGE("*    pend"   << pend );

    int r = _hexaDocument->associateOpenedLine( mstart, mline,
                                                gstart, pstart, gline, pend );
    if ( r == HOK ){
        updateData();
        MESSAGE("*    DocumentModel:: associateOpenedLine() => OK ");
        ret = true;
    } else if ( r == HERR ){
        MESSAGE("*    DocumentModel:: associateOpenedLine() => ERR ");
        ret = false;
    }
    delete gstart; //CS_TODO : delete gline?
    return ret;
}

bool DocumentModel::associateClosedLine( const  QModelIndex& ivertex,
                                         const  QModelIndexList& iedges,
                                         const  GeomObjList&     assocs,
                                         double pstart,
					 bool   inv )
{
    MESSAGE("DocumentModel::associateClosedLine() ");
    bool ret = false;
    HEXA_NS::Vertex* mfirst = getHexaPtr<HEXA_NS::Vertex*>(ivertex);
    HEXA_NS::Edge*   mstart = NULL;
    HEXA_NS::Edges   mline;
    HEXA_NS::Shape*  gstart = NULL;
//     double  pstart;
    HEXA_NS::Shapes  gline;


    HEXA_NS::Edge* hedge = NULL;
    foreach( const QModelIndex& iedge, iedges ){
        hedge = getHexaPtr<HEXA_NS::Edge*>(iedge);
        if ( mstart == NULL ){
            mstart = hedge;
            MESSAGE("*    mstart" << iedge.data().toString().toStdString() );
        } else {
            mline.push_back( hedge );
            MESSAGE("*    mline :" << iedge.data().toString().toStdString() );
        }
    }

    HEXA_NS::Shape* hshape = NULL;
    foreach( const GeomObj& anAssoc, assocs ){
        hshape = new HEXA_NS::Shape( anAssoc.brep.toStdString() );
        hshape->debut = anAssoc.start; //0.;
        hshape->fin   = anAssoc.end; //1.;
        hshape->ident = ( anAssoc.entry + "," + anAssoc.subid ).toStdString();//anAssoc.entry.toStdString();

        if ( gstart == NULL ){
            gstart = hshape; // CS_TODO :gstart.debut = pstart ??
//             pstart = anAssoc.start;
            MESSAGE("*    gstart->debut" << gstart->debut);
            MESSAGE("*    gstart->fin"   << gstart->fin);
            MESSAGE("*    gstart->ident" << gstart->ident);
            MESSAGE("*    gstart->getBrep()" << gstart->getBrep());
        } else {
            gline.push_back( hshape ); //CS_TODO : hshape.fin = pend ??
            MESSAGE("*    gline->debut" << hshape->debut);
            MESSAGE("*    gline->fin"   << hshape->fin);
            MESSAGE("*    gline->ident" << hshape->ident);
            MESSAGE("*    gline->getBrep()" << hshape->getBrep());
        }
    }
    MESSAGE("*    pstart" << pstart );

    int r = _hexaDocument->associateClosedLine( mfirst, mstart, mline,
                                                gstart, pstart, inv, gline );
    if ( r == HOK ){
        updateData();
        ret = true;
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
  groupItem->setData( _entry, HEXA_DOC_ENTRY_ROLE );
  _groupDirItem->appendRow(groupItem);
  iGroup = groupItem->index();

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
  } else if ( r == HERR ){
    ret = false;
  }
  return ret;
}
/*
QModelIndex* DocumentModel::found(eltBase)
{
	TODO_JMD
}
*/

QModelIndexList DocumentModel::getGroupElements( const QModelIndex& iGroup, DocumentModel::Group& kind ) const
{
  QModelIndexList iElements;

  HEXA_NS::Group* g = iGroup.data(HEXA_DATA_ROLE).value<HEXA_NS::Group *>();
  if ( !g ) return iElements;

  QModelIndexList iFound;
  QVariant q;
  HEXA_NS::EltBase* eltBase = NULL;
  for ( int nr = 0; nr < g->countElement(); ++nr ){
    eltBase = g->getElement( nr );
    kind = g->getKind();
    switch ( kind ){
      case HEXA_NS::HexaCell: case HEXA_NS::HexaNode: q = QVariant::fromValue( (HEXA_NS::Hexa *)eltBase ); break;
      case HEXA_NS::QuadCell: case HEXA_NS::QuadNode: q = QVariant::fromValue( (HEXA_NS::Quad *)eltBase ); break;
      case HEXA_NS::EdgeCell: case HEXA_NS::EdgeNode: q = QVariant::fromValue( (HEXA_NS::Edge *)eltBase ); break;
      case HEXA_NS::VertexNode: q = QVariant::fromValue( (HEXA_NS::Vertex *)eltBase ); break;
    }
    iFound = match( index(0, 0),
                    HEXA_DATA_ROLE,
                    q,
                    1,
                    Qt::MatchRecursive );
    if ( !iFound.isEmpty() )
      iElements << iFound[0];
  }
  return iElements;
}

// 7.4 Boite: éditer un groupe
void DocumentModel::setGroupName( const QModelIndex& igrp, const QString& name )
{
  HEXA_NS::Group* hGroup = data(igrp, HEXA_DATA_ROLE).value<HEXA_NS::Group *>();

  if ( hGroup ){
    hGroup->setName( name.toLocal8Bit().constData() );
    setData(igrp, QVariant::fromValue( name ) );
  }

}

bool DocumentModel::addGroupElement( const QModelIndex& igrp, const QModelIndex& ielt )
{ //CS_TODO : check input? add child?
// int       addElement    (EltBase* elt);

  bool addOk = false;
  HEXA_NS::Group*   hGroup = data(igrp, HEXA_DATA_ROLE).value<HEXA_NS::Group *>();
  HEXA_NS::EltBase* hElt   = NULL;
  switch ( hGroup->getKind() ){
  case HEXA_NS::HexaCell: case HEXA_NS::HexaNode: hElt = getHexaPtr<HEXA_NS::Hexa*>(ielt); break;
  case HEXA_NS::QuadCell: case HEXA_NS::QuadNode: hElt = getHexaPtr<HEXA_NS::Quad*>(ielt); break;
  case HEXA_NS::EdgeCell: case HEXA_NS::EdgeNode: hElt = getHexaPtr<HEXA_NS::Edge*>(ielt); break;
  case HEXA_NS::VertexNode: hElt = getHexaPtr<HEXA_NS::Vertex*>(ielt); break;
  }

  int res = HERR;   
  if ( hGroup and hElt )
    res = hGroup->addElement( hElt );

  if ( res == HOK ) addOk = true;

  return addOk;
}

bool DocumentModel::removeGroupElement( const QModelIndex& igrp, int nro )
{ //CS_TODO : remove child?
  HEXA_NS::Group* hGroup = data(igrp, HEXA_DATA_ROLE).value<HEXA_NS::Group *>();

  if ( hGroup )
  {
    if (hGroup->removeElement( nro ) == HOK)
    	return true;
  }
  return false;
}

bool DocumentModel::clearGroupElement( const QModelIndex& igrp )
{
  HEXA_NS::Group* hGroup = data(igrp, HEXA_DATA_ROLE).value<HEXA_NS::Group *>();

  if ( hGroup )
  {
    hGroup->clearElement();
    return true;
  }
  return false;
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
  } else if ( r == HERR ){
    ret = false;
  }
  return ret;
}

QModelIndexList DocumentModel::getPropagation( const QModelIndex& iPropagation ) const
{
  QModelIndexList iEdges;

  QModelIndexList iFound;
  HEXA_NS::Propagation* propa = iPropagation.data(HEXA_DATA_ROLE).value<HEXA_NS::Propagation *>();
  if ( !propa ) return iEdges;

  const HEXA_NS::Edges& edges = propa->getEdges();
  for ( HEXA_NS::Edges::const_iterator anEdge = edges.begin();
        anEdge != edges.end();
        ++anEdge ){
    iFound = match( index(0, 0),
              HEXA_DATA_ROLE,
              QVariant::fromValue( *anEdge ),
              1,
              Qt::MatchRecursive);
    if ( !iFound.isEmpty() )
      iEdges << iFound[0];
  }

  return iEdges;
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
        arg(VERTEX_TREE).arg(EDGE_TREE).arg(QUAD_TREE).arg(HEXA_TREE).
        arg(VERTEX_DIR_TREE).arg(EDGE_DIR_TREE).arg(QUAD_DIR_TREE).arg(HEXA_DIR_TREE);

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
//   MESSAGE("PatternDataModel::flags() "<< index.data().toString().toStdString() );
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
    arg(VECTOR_TREE).arg(CYLINDER_TREE).arg(PIPE_TREE).arg(ELEMENTS_TREE).
    arg(CROSSELEMENTS_TREE).arg(VECTOR_DIR_TREE).arg(CYLINDER_DIR_TREE).
    arg(PIPE_DIR_TREE).arg(ELEMENTS_DIR_TREE).arg(CROSSELEMENTS_DIR_TREE);

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

QModelIndexList GroupsModel::getGroupElements( const QModelIndex& iGroup, DocumentModel::Group& kind ) const
{
  QModelIndexList elements;
  DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
  if ( m != NULL ){
    elements = m->getGroupElements( mapToSource(iGroup), kind );
  }
  return elements;
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

QModelIndexList MeshModel::getPropagation( const QModelIndex& iPropagation ) const
{
  QModelIndexList edges;
  DocumentModel *m = dynamic_cast<DocumentModel *>( sourceModel() );
  if ( m != NULL ){
    edges = m->getPropagation( mapToSource(iPropagation) );
  }
  return edges;
}
