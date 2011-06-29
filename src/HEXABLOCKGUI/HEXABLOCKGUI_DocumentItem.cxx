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

#include "HEXABLOCKGUI_DocumentItem.hxx"
#include <inttypes.h>

/*
#include <QVariant>*/


//#define _DEVDEBUG_


using namespace std;
using namespace HEXABLOCK::GUI;


// ----------------------- VERTEX

VertexItem::VertexItem( HEXA_NS::Vertex* hexaVertex ):
  QStandardItem(),
  _hexaVertex( hexaVertex )
{
  char pName[12];
  QString name = _hexaVertex->getName(pName);
//   QString docEntry = model()->invisibleRootItem()->data(HEXA_ENTRY_ROLE).toString();

  setText(name);
//   setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );//Qt::ItemIsEditable);
  setData( VERTEX_TREE, HEXA_TREE_ROLE );
  setData( QString::number( reinterpret_cast<intptr_t>(_hexaVertex) ), HEXA_ENTRY_ROLE );
//   setData( QString::number( reinterpret_cast<intptr_t>(_hexaVertex->dad()) ), HEXA_DOC_ENTRY_ROLE );
  setData( QString("HEXA_ENTRY:%1").arg( QString::number( reinterpret_cast<intptr_t>(_hexaVertex->dad()) ) ), HEXA_DOC_ENTRY_ROLE );


//   setData( QString("0:1:1:1"), HEXA_DOC_ENTRY_ROLE );
//   setData( QString("toto"), HEXA_ENTRY_ROLE );
}

int VertexItem::type() const
{
  return VERTEXITEM;
}

QVariant VertexItem::data( int role ) const
{
    if ( role == HEXA_DATA_ROLE ){
//       std::cout << " VertexItem::data(HEXA_DATA_ROLE) >>>>>>>>>>>>>>>>>>>>>" << std::endl;
      return QVariant::fromValue( _hexaVertex );
    } else {
      return QStandardItem::data( role );
    }
}

void VertexItem::setData ( const QVariant & value, int role )
{
    if ( role == HEXA_DATA_ROLE ){
//       std::cout << " VertexItem::setData(HEXA_DATA_ROLE) >>>>>>>>>>>>>>>>>>" << std::endl;
      _hexaVertex = value.value<HEXA_NS::Vertex*>();
      emitDataChanged ();
    } else {
//       std::cout << " VertexItem::setData( " << role << ", "<< value.toString().toStdString() << " )" <<std::endl;
      QStandardItem::setData ( value, role );
    }
}



// ----------------------- EDGE

EdgeItem::EdgeItem( HEXA_NS::Edge* hexaEdge ):
  QStandardItem(),
  _hexaEdge( hexaEdge )
{
  char pName[12];
  QString name = _hexaEdge->getName(pName);
  setText(name);
  setData( EDGE_TREE, HEXA_TREE_ROLE );
  setData( QString::number( reinterpret_cast<intptr_t>(_hexaEdge) ), HEXA_ENTRY_ROLE );
//   setData( QString::number( reinterpret_cast<intptr_t>(_hexaEdge->dad()) ), HEXA_DOC_ENTRY_ROLE );
setData( QString("HEXA_ENTRY:%1").arg( QString::number( reinterpret_cast<intptr_t>(_hexaEdge->dad()) ) ), HEXA_DOC_ENTRY_ROLE );
}

int EdgeItem::type() const
{
  return EDGEITEM;
}

QVariant EdgeItem::data( int role ) const
{
    if ( role == HEXA_DATA_ROLE ){
      return QVariant::fromValue( _hexaEdge );
    } else {
      return QStandardItem::data( role );
    }
}

void EdgeItem::setData ( const QVariant & value, int role )
{
    if ( role == HEXA_DATA_ROLE ){
      _hexaEdge = value.value<HEXA_NS::Edge*>();
      emitDataChanged ();
    } else {
      QStandardItem::setData ( value, role );
    }
}




// ----------------------- QUAD
QuadItem::QuadItem( HEXA_NS::Quad* hexaQuad ):
  QStandardItem(),
  _hexaQuad( hexaQuad )
{
  char pName[12];
  QString name = _hexaQuad->getName(pName);
  setText(name);
  setData( QUAD_TREE, HEXA_TREE_ROLE );
  setData( QString::number(reinterpret_cast<intptr_t>(_hexaQuad)), HEXA_ENTRY_ROLE );
//   setData( QString::number( reinterpret_cast<intptr_t>(_hexaQuad->dad()) ), HEXA_DOC_ENTRY_ROLE );
setData( QString("HEXA_ENTRY:%1").arg( QString::number( reinterpret_cast<intptr_t>(_hexaQuad->dad()) ) ), HEXA_DOC_ENTRY_ROLE );
}


int QuadItem::type() const
{
  return QUADITEM;
}

QVariant QuadItem::data( int role ) const
{
    if ( role == HEXA_DATA_ROLE ){
      return QVariant::fromValue( _hexaQuad );
    } else {
      return QStandardItem::data( role );
    }
}

void QuadItem::setData ( const QVariant & value, int role )
{
    if ( role == HEXA_DATA_ROLE ){
      _hexaQuad = value.value<HEXA_NS::Quad*>();
      emitDataChanged ();
    } else {
      QStandardItem::setData ( value, role );
    }
}





// ----------------------- HEXA

HexaItem::HexaItem( HEXA_NS::Hexa* hexaHexa ):
  QStandardItem(),
  _hexaHexa( hexaHexa )
{
  char pName[12];
  QString name = _hexaHexa->getName(pName);
  setText(name);
  setData( HEXA_TREE, HEXA_TREE_ROLE );
  setData( QString::number( reinterpret_cast<intptr_t>(_hexaHexa)), HEXA_ENTRY_ROLE );
//   setData( QString::number( reinterpret_cast<intptr_t>(_hexaHexa->dad()) ), HEXA_DOC_ENTRY_ROLE );
  setData( QString("HEXA_ENTRY:%1").arg( QString::number( reinterpret_cast<intptr_t>(_hexaHexa->dad()) ) ), HEXA_DOC_ENTRY_ROLE );
}


int HexaItem::type() const
{
  return HEXAITEM;
}

QVariant HexaItem::data( int role ) const
{
    if ( role == HEXA_DATA_ROLE ){
      return QVariant::fromValue( _hexaHexa );
    } else {
      return QStandardItem::data( role );
    }
}

void HexaItem::setData ( const QVariant & value, int role )
{
    if ( role == HEXA_DATA_ROLE ){
      _hexaHexa = value.value<HEXA_NS::Hexa*>();
      emitDataChanged ();
    } else {
      QStandardItem::setData ( value, role );
    }
}





// ----------------------- VECTOR
VectorItem::VectorItem( HEXA_NS::Vector* hexaVector ):
  QStandardItem(),
  _hexaVector( hexaVector )
{
  char pName[12];
  QString name = _hexaVector->getName(pName);
  setText(name);
  setData( VECTOR_TREE, HEXA_TREE_ROLE );
  setData( QString::number(reinterpret_cast<intptr_t>(_hexaVector)), HEXA_ENTRY_ROLE );
}

int VectorItem::type() const
{
  return VECTORITEM;
}

QVariant VectorItem::data( int role ) const 
{ 
    if ( role == HEXA_DATA_ROLE ){
      return QVariant::fromValue( _hexaVector );
    } else {
      return QStandardItem::data( role );
    }
}

void VectorItem::setData ( const QVariant & value, int role )
{
    if ( role == HEXA_DATA_ROLE ){
      _hexaVector = value.value<HEXA_NS::Vector*>();
      emitDataChanged ();
    } else {
      QStandardItem::setData ( value, role );
    }
}




// ----------------------- CYLINDER
CylinderItem::CylinderItem( HEXA_NS::Cylinder* hexaCylinder):
  QStandardItem(),
  _hexaCylinder( hexaCylinder )
{
  char pName[12];
  QString name = _hexaCylinder->getName(pName);
  setText(name);
  setData( CYLINDER_TREE, HEXA_TREE_ROLE );
  setData( QString::number(reinterpret_cast<intptr_t>(_hexaCylinder)), HEXA_ENTRY_ROLE );
}

int CylinderItem::type() const
{
  return CYLINDERITEM;
}

QVariant CylinderItem::data( int role ) const 
{ 
    if ( role == HEXA_DATA_ROLE ){
      return QVariant::fromValue( _hexaCylinder );
    } else {
      return QStandardItem::data( role );
    }
}

void CylinderItem::setData ( const QVariant & value, int role )
{
    if ( role == HEXA_DATA_ROLE ){
      _hexaCylinder = value.value<HEXA_NS::Cylinder*>();
      emitDataChanged ();
    } else {
      QStandardItem::setData ( value, role );
    }
}




// ----------------------- PIPE
PipeItem::PipeItem( HEXA_NS::Pipe* hexaPipe ):
  QStandardItem(),
  _hexaPipe( hexaPipe )
{
  char pName[12];
  QString name = _hexaPipe->getName(pName);
  setText(name);
  setData( PIPE_TREE, HEXA_TREE_ROLE );
  setData( QString::number(reinterpret_cast<intptr_t>(_hexaPipe)), HEXA_ENTRY_ROLE );
}

int PipeItem::type() const
{
  return PIPEITEM;
}

QVariant PipeItem::data( int role ) const 
{ 
    if ( role == HEXA_DATA_ROLE ){
      return QVariant::fromValue( _hexaPipe );
    } else {
      return QStandardItem::data( role );
    }
}

void PipeItem::setData ( const QVariant & value, int role )
{
    if ( role == HEXA_DATA_ROLE ){
      _hexaPipe = value.value<HEXA_NS::Pipe*>();
      emitDataChanged ();
    } else {
      QStandardItem::setData ( value, role );
    }
}



// ----------------------- ELEMENTS
ElementsItem::ElementsItem( HEXA_NS::Elements* hexaElements ):
  QStandardItem(),
  _hexaElements( hexaElements )
{
  char pName[12];
  QString name = _hexaElements->getName(pName);
  setText(name);
  setData( ELEMENTS_TREE, HEXA_TREE_ROLE );
  setData( QString::number(reinterpret_cast<intptr_t>(_hexaElements)), HEXA_ENTRY_ROLE );
}

int ElementsItem::type() const
{
  return ELEMENTSITEM;
}

QVariant ElementsItem::data( int role ) const 
{ 
    if ( role == HEXA_DATA_ROLE ){
      return QVariant::fromValue( _hexaElements );
    } else {
      return QStandardItem::data( role );
    }
}

void ElementsItem::setData ( const QVariant & value, int role )
{
    if ( role == HEXA_DATA_ROLE ){
      _hexaElements = value.value<HEXA_NS::Elements*>();
      emitDataChanged ();
    } else {
      QStandardItem::setData ( value, role );
    }
}



// ----------------------- CROSSELEMENTS
CrossElementsItem::CrossElementsItem( HEXA_NS::CrossElements* hexaCrossElts ):
  QStandardItem(),
  _hexaCrossElts( hexaCrossElts )
{
  char pName[12];
  QString name = _hexaCrossElts->getName(pName);
  setText(name);
  setData( CROSSELEMENTS_TREE, HEXA_TREE_ROLE );
  setData( QString::number(reinterpret_cast<intptr_t>(_hexaCrossElts)), HEXA_ENTRY_ROLE );
}

int CrossElementsItem::type() const
{
  return CROSSELEMENTSITEM;
}

QVariant CrossElementsItem::data( int role ) const 
{ 
    if ( role == HEXA_DATA_ROLE ){
      return QVariant::fromValue( _hexaCrossElts );
    } else {
      return QStandardItem::data( role );
    }
}

void CrossElementsItem::setData ( const QVariant & value, int role )
{
    if ( role == HEXA_DATA_ROLE ){
      _hexaCrossElts = value.value<HEXA_NS::CrossElements*>();
      emitDataChanged ();
    } else {
      QStandardItem::setData ( value, role );
    }
}



// ----------------------- GROUP
GroupItem::GroupItem( HEXA_NS::Group* hexaGroup ):
  QStandardItem(),
  _hexaGroup( hexaGroup )
{
//   char pName[12];
  QString name = _hexaGroup->getName();//pName);
  setText(name);
  setData( GROUP_TREE, HEXA_TREE_ROLE );
  setData( QString::number(reinterpret_cast<intptr_t>(_hexaGroup)), HEXA_ENTRY_ROLE );
}

int GroupItem::type() const
{
  return GROUPITEM;
}

QVariant GroupItem::data( int role ) const 
{ 
    if ( role == HEXA_DATA_ROLE ){
      return QVariant::fromValue( _hexaGroup );
    } else {
      return QStandardItem::data( role );
    }
}

void GroupItem::setData ( const QVariant& value, int role )
{
    if ( role == HEXA_DATA_ROLE ){
      _hexaGroup = value.value<HEXA_NS::Group*>();
      emitDataChanged ();
    } else {
      QStandardItem::setData ( value, role );
    }
}



// ----------------------- LAW
LawItem::LawItem( HEXA_NS::Law* hexaLaw ):
  QStandardItem(),
  _hexaLaw( hexaLaw )
{
//   char pName[12];
  QString name = _hexaLaw->getName();//pName);
  setText(name);
  setData( LAW_TREE, HEXA_TREE_ROLE );
  setData( QString::number(reinterpret_cast<intptr_t>(_hexaLaw)), HEXA_ENTRY_ROLE );
}

int LawItem::type() const
{
  return LAWITEM;
}

QVariant LawItem::data( int role ) const 
{ 
    if ( role == HEXA_DATA_ROLE ){
      return QVariant::fromValue( _hexaLaw);
    } else {
      return QStandardItem::data( role );
    }
}

void LawItem::setData ( const QVariant & value, int role )
{
    if ( role == HEXA_DATA_ROLE ){
      _hexaLaw = value.value<HEXA_NS::Law*>();
      emitDataChanged ();
    } else {
      QStandardItem::setData ( value, role );
    }
}



// ----------------------- PROPAGATIONITEM
PropagationItem::PropagationItem( HEXA_NS::Propagation* hexaPropagation ):
  QStandardItem(),
  _hexaPropagation( hexaPropagation )
{
//   char pName[12];
//   QString name = _hexaPropagation->getName(pName);
  QString name = "Propagation";
  setText(name);
  setData( PROPAGATION_TREE, HEXA_TREE_ROLE );
  setData( QString::number(reinterpret_cast<intptr_t>(_hexaPropagation)), HEXA_ENTRY_ROLE );
}

int PropagationItem::type() const
{
  return PROPAGATIONITEM;
}

QVariant PropagationItem::data( int role ) const 
{ 
    if ( role == HEXA_DATA_ROLE ){
      return QVariant::fromValue( _hexaPropagation );
    } else {
      return QStandardItem::data( role );
    }
}

void PropagationItem::setData ( const QVariant & value, int role )
{
    if ( role == HEXA_DATA_ROLE ){
      _hexaPropagation = value.value<HEXA_NS::Propagation*>();
      emitDataChanged ();
    } else {
      QStandardItem::setData ( value, role );
    }
}
