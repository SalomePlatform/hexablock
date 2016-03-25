// Copyright (C) 2009-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

#include "HEXABLOCKGUI_SalomeTools.hxx"
#include "HEXABLOCKGUI_DocumentItem.hxx"
#include "HexShape.hxx"

#include "HEXABLOCKGUI_DocumentModel.hxx"
#include "HexAssoEdge.hxx"

#ifndef WIN32
#include <inttypes.h>
#endif

using namespace std;
using namespace HEXABLOCK::GUI;

// ElementItem --------------------------------------------------
ElementItem::ElementItem( HEXA_NS::EltBase* docElement, QString entry, HexaType ttype, HexaTreeRole treeRole):
QStandardItem()
{
	m_DocElt = docElement;
	m_type   = ttype;

	if (m_DocElt != NULL)
	{
		setText(m_DocElt->getName());
		if ( m_DocElt->isAssociated() )
		    setData(QColor(Qt::darkGreen), Qt::ForegroundRole);
	}
	setData( treeRole,    HEXA_TREE_ROLE );
	setData( entry,       HEXA_DOC_ENTRY_ROLE);
	setData( IDptr(),     HEXA_ENTRY_ROLE );

	//Init assocs entry
	if (m_DocElt!=NULL && m_DocElt->isAssociated())
	{
		QString entry;
		if (m_type == VERTEXITEM) //Vertex
		{
		    HEXA_NS::Vertex* vertex = (HEXA_NS::Vertex*) m_DocElt;
		    HEXA_NS::VertexShape* assoc = vertex->getAssociation();
		    if (assoc != NULL)
		    {
		        HEXA_NS::NewShape* mainSh = assoc->getParentShape();
		        if (mainSh != NULL)
		        {
		            entry = QString(mainSh->getName())+","+QString::number(assoc->getIdent())+";";
		            setData( entry, HEXA_ASSOC_ENTRY_ROLE );
		        }
		    }
		}
		else if (m_type == EDGEITEM)
		{
			QString entries;
			HEXA_NS::Edge* edge = (HEXA_NS::Edge*) m_DocElt;
			HEXA_NS::NewShape* mainShape;
			HEXA_NS::EdgeShape* geomEdge;
			HEXA_NS::AssoEdge* anEdgeAssoc;
			int nbAssocs = edge->countAssociation();
			for (int i = 0; i < nbAssocs; ++i)
			{
			    anEdgeAssoc = edge->getAssociation(i);
			    if (anEdgeAssoc == NULL) continue;
			    geomEdge = anEdgeAssoc->getEdgeShape();
			    if (geomEdge == NULL) continue;
			    mainShape = geomEdge->getParentShape();
			    if (mainShape == NULL) continue;
			    entries += QString(mainShape->getName())+","+QString::number(geomEdge->getIdent())+";";
			}
			if ( !entries.isEmpty() )
				setData( entries, HEXA_ASSOC_ENTRY_ROLE );
		}
		else if (m_type == QUADITEM)
		{
			QString entries;
			HEXA_NS::Quad* quad = (HEXA_NS::Quad*) m_DocElt;
			HEXA_NS::NewShape* mainShape;
			HEXA_NS::FaceShape* geomFace;
			int nbAssocs = quad->countAssociation();
			for (int i = 0; i < nbAssocs; ++i)
			{
			    geomFace = quad->getAssociation(i);
			    if (geomFace == NULL) continue;
			    mainShape = geomFace->getParentShape();
			    if (mainShape == NULL) continue;
			    entries += QString(mainShape->getName()) + "," + QString::number(geomFace->getIdent()) + ";";
			}
			if ( !entries.isEmpty() )
				setData( entries, HEXA_ASSOC_ENTRY_ROLE );
		}
	}
}

//---------------------------------------------------------------
QVariant ElementItem::data( int role ) const
{
    if ( role == HEXA_DATA_ROLE ){
        switch(m_type)
        {
        case VERTEXITEM: return QVariant::fromValue( (HEXA_NS::Vertex*)m_DocElt );
        case EDGEITEM: return QVariant::fromValue( (HEXA_NS::Edge*)m_DocElt );
        case QUADITEM: return QVariant::fromValue( (HEXA_NS::Quad*)m_DocElt );
        case HEXAITEM: return QVariant::fromValue( (HEXA_NS::Hexa*)m_DocElt );
        case VECTORITEM: return QVariant::fromValue( (HEXA_NS::Vector*)m_DocElt );
//        case CYLINDERITEM: return QVariant::fromValue( (HEXA_NS::Cylinder*)m_DocElt );
//        case PIPEITEM: return QVariant::fromValue( (HEXA_NS::Pipe*)m_DocElt );
        case ELEMENTSITEM: return QVariant::fromValue( (HEXA_NS::Elements*)m_DocElt );
//      case CROSSELEMENTSITEM: return QVariant::fromValue( (HEXA_NS::CrossElements*)m_DocElt );
        case GEOMSHAPEITEM: return QVariant::fromValue( (HEXA_NS::NewShape*) m_DocElt );
        case GEOMPOINTITEM: return QVariant::fromValue( (HEXA_NS::VertexShape*) m_DocElt );
        case GEOMEDGEITEM:  return QVariant::fromValue( (HEXA_NS::EdgeShape*) m_DocElt );
        case GEOMFACEITEM:  return QVariant::fromValue( (HEXA_NS::FaceShape*) m_DocElt );
        default: return QVariant::fromValue( m_DocElt );
        }
    }

    if (role == Qt::ForegroundRole && m_DocElt != NULL && !m_DocElt->isAssociated())
        return QColor(Qt::black);

    return QStandardItem::data( role );
}

//---------------------------------------------------------------
void ElementItem::setData ( const QVariant& valcont, int role )
{
	if ( role == HEXA_DATA_ROLE ){
		m_DocElt = valcont.value<HEXA_NS::EltBase*>();
		emitDataChanged ();

	} else {
		QStandardItem::setData ( valcont, role );
	}
}
//---------------------------------------------------------------
int ElementItem::type () const {return m_type;}
//---------------------------------------------------------------
bool ElementItem::isAssoc () const {return m_DocElt->isAssociated();}
//---------------------------------------------------------------
QString ElementItem::IDptr() const {  return QString::number( reinterpret_cast<intptr_t>(m_DocElt) ); }


// ----------------------- VERTEX (DATA)
VertexItem::VertexItem( HEXA_NS::Vertex* hexaVertex, QString entry):
		GraphicElementItem(hexaVertex, entry, VERTEXITEM, VERTEX_TREE)
{
}

// ----------------------- EDGE   (DATA)
EdgeItem::EdgeItem( HEXA_NS::Edge* hexaEdge, QString entry ):
		GraphicElementItem(hexaEdge, entry, EDGEITEM, EDGE_TREE)
{
}

// ----------------------- QUAD   (DATA)
QuadItem::QuadItem( HEXA_NS::Quad* hexaQuad, QString entry):
		GraphicElementItem(hexaQuad, entry, QUADITEM, QUAD_TREE)
{
}

// ----------------------- HEXA   (DATA)
HexaItem::HexaItem( HEXA_NS::Hexa* hexaHexa , QString entry):
		GraphicElementItem(hexaHexa,entry, HEXAITEM, HEXA_TREE)
{
}

// ----------------------- VECTOR   (BUILDER)
VectorItem::VectorItem( HEXA_NS::Vector* hexaVector, QString entry ):
		StandardElementItem(hexaVector, entry, VECTORITEM, VECTOR_TREE)
{
}

//// ----------------------- CYLINDER (BUILDER)
//CylinderItem::CylinderItem( HEXA_NS::Cylinder* hexaCylinder, QString entry):
//		StandardElementItem(hexaCylinder, entry, CYLINDERITEM, CYLINDER_TREE)
//{
//}
//
//// ----------------------- PIPE     (BUILDER)
//PipeItem::PipeItem( HEXA_NS::Pipe* hexaPipe, QString entry ):
//		StandardElementItem(hexaPipe, entry, PIPEITEM, PIPE_TREE)
//{
//}

// ----------------------- ELEMENTS      (NOT USED)
ElementsItem::ElementsItem( HEXA_NS::Elements* hexaElements, QString entry ):
		StandardElementItem(hexaElements, entry, ELEMENTSITEM, ELEMENTS_TREE)
{
}

// ----------------------- CROSSELEMENTS (NOT USED)
//CrossElementsItem::CrossElementsItem( HEXA_NS::CrossElements* hexaCrossElts, QString entry ):
//		StandardElementItem(hexaCrossElts, entry, CROSSELEMENTSITEM, CROSSELEMENTS_TREE)
//{
//}

// ----------------------- GEOM     (GEOMETRY)
GeomItem::GeomItem( HEXA_NS::EltBase* geomShape, QString entry, HexaType ttype, HexaTreeRole treeRole, HEXA_NS::EltBase* assoc ):
        StandardElementItem(geomShape, entry, ttype, treeRole),
        association(assoc)
{
}

// ----------------------- GEOM SHAPE     (GEOMETRY)
GeomShapeItem::GeomShapeItem( HEXA_NS::NewShape* shape, HEXA_NS::EltBase* assoc ):
        GeomItem(shape, QString(), GEOMSHAPEITEM, GEOMSHAPE_TREE, assoc)
{
}

// ----------------------- GEOM POINT     (GEOMETRY)
GeomPointItem::GeomPointItem( HEXA_NS::VertexShape* geomPoint, HEXA_NS::Vertex* associatedVertex ):
        GeomItem(geomPoint, QString(), GEOMPOINTITEM, GEOMPOINT_TREE, associatedVertex)
{
}

// ----------------------- GEOM EDGE     (GEOMETRY)
GeomEdgeItem::GeomEdgeItem( HEXA_NS::EdgeShape* geomEdge, HEXA_NS::Edge* associatedEdge ):
        GeomItem(geomEdge, QString(), GEOMEDGEITEM, GEOMEDGE_TREE, associatedEdge)
{
}

// ----------------------- GEOM FACE     (GEOMETRY)
GeomFaceItem::GeomFaceItem( HEXA_NS::FaceShape* geomFace, HEXA_NS::Quad* associatedQuad ):
        GeomItem(geomFace, QString(), GEOMFACEITEM, GEOMFACE_TREE, associatedQuad)
{
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
//-------------------------------------------------


