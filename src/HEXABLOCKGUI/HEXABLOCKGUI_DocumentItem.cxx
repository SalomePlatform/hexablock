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
  setText(name);
//   setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );//Qt::ItemIsEditable);
  setData( VERTEX_TREE, HEXA_TREE_ROLE );
  setData( QString::number(reinterpret_cast<intptr_t>(_hexaVertex)), HEXA_ENTRY_ROLE );
//   setData( QString("toto"), HEXA_ENTRY_ROLE );
}

int VertexItem::type() const
{
  return VERTEXITEM;
}

QVariant VertexItem::data( int role ) const
{
    if ( role == HEXA_DATA_ROLE ){
      return QVariant::fromValue( _hexaVertex );
    } else {
      return QStandardItem::data( role );
    }
}

void VertexItem::setData ( const QVariant & value, int role )
{
    if ( role == HEXA_DATA_ROLE ){
      _hexaVertex = value.value<HEXA_NS::Vertex*>();
      emitDataChanged ();
    } else {
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
  setData( QString::number(reinterpret_cast<intptr_t>(_hexaEdge)), HEXA_ENTRY_ROLE );
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
  setData( QString::number(reinterpret_cast<intptr_t>(_hexaHexa)), HEXA_ENTRY_ROLE );
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




// ----------------------- ELEMENTS
ElementsItem::ElementsItem( HEXA_NS::Elements* hexaElements ):
  QStandardItem(),
  _hexaElements( hexaElements )
{
  char pName[12];
  QString name = _hexaElements->getName(pName);
  setText(name);
  setData( HEXA_TREE, HEXA_TREE_ROLE );
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

