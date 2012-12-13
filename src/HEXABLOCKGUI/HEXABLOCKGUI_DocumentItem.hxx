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
#include <HexNewShape.hxx>
#include <HexVertexShape.hxx>
#include <HexEdgeShape.hxx>
#include <HexFaceShape.hxx>



#include <HexGroup.hxx>
#include <HexLaw.hxx>
#include <HexPropagation.hxx>

#include "vtkActor.h"


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
Q_DECLARE_METATYPE( HEXA_NS::NewShape* );
Q_DECLARE_METATYPE( HEXA_NS::VertexShape* );
Q_DECLARE_METATYPE( HEXA_NS::EdgeShape* );
Q_DECLARE_METATYPE( HEXA_NS::FaceShape* );

Q_DECLARE_METATYPE( HEXA_NS::Group* );
Q_DECLARE_METATYPE( HEXA_NS::Law* );
Q_DECLARE_METATYPE( HEXA_NS::Propagation* );



// Q_DECLARE_METATYPE( QList<HEXA_NS::Shape*> ); // association

namespace HEXABLOCK
{
  namespace GUI
  {
    enum HexaType {
      VERTEXITEM = QStandardItem::UserType + 1,
      EDGEITEM,
      QUADITEM,
      HEXAITEM,

      VECTORITEM,
      CYLINDERITEM,
      PIPEITEM,
      ELEMENTSITEM,
      CROSSELEMENTSITEM,

      EXPSHAPEITEM,
      IMPSHAPEITEM,
      CLOUDOFPOINTSITEM,
      GEOMSHAPEITEM,
      GEOMPOINTITEM,
      GEOMEDGEITEM,
      GEOMFACEITEM,

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

      EXPLICIT_SHAPES_TREE,
      IMPLICIT_SHAPES_TREE,
      CLOUD_OF_POINTS_TREE,
      GEOMSHAPE_TREE,
      GEOMPOINT_TREE,
      GEOMEDGE_TREE,
      GEOMFACE_TREE,


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

      EXPLICIT_SHAPES_DIR_TREE,
      IMPLICIT_SHAPES_DIR_TREE,
      CLOUD_OF_POINTS_DIR_TREE,
      GEOMSHAPE_DIR_TREE,
      GEOMPOINT_DIR_TREE,
      GEOMEDGE_DIR_TREE,
      GEOMFACE_DIR_TREE,

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

    //===================================================================================
    class ElementItem : public QStandardItem
    {
      public:
    	ElementItem( HEXA_NS::EltBase* docElement, QString entry, HexaType ttype, HexaTreeRole treeRole);
    	ElementItem( HEXA_NS::EltBase* docElement, HexaType ttype, HexaTreeRole treeRole);
    	virtual QVariant data( int role ) const;
    	virtual void     setData ( const QVariant& valcont, int role );
    	int      type () const;
    	virtual bool     isAssoc() const;
    	virtual QString  IDptr() const;

      private:
        int                m_type;
        HEXA_NS::EltBase*  m_DocElt; // Vertex and so.
    };
    //===================================================================================

    class GraphicElementItem : public ElementItem
    {
      public:
    	GraphicElementItem( HEXA_NS::EltBase* docElement, QString entry, HexaType ttype, HexaTreeRole treeRole):
    		ElementItem( docElement, entry, ttype, treeRole)
    	{
    	}

      private:
        vtkActor* actor;
        int       IDinActor;
    };

    class StandardElementItem : public ElementItem
    {
       public:
    	StandardElementItem( HEXA_NS::EltBase* docElement, QString entry, HexaType ttype, HexaTreeRole treeRole):
    		ElementItem( docElement, entry, ttype, treeRole)
    	{
    	}
    };

    class VertexItem : public GraphicElementItem
    {
      public:
        VertexItem( HEXA_NS::Vertex* hexaVertex, QString entry = "");
    };

    class EdgeItem : public GraphicElementItem
    {
      public:
        EdgeItem( HEXA_NS::Edge* hexaEdge, QString entry = "");
    };
    //-----------------------------------------
    class QuadItem : public GraphicElementItem
    {
      public:
        QuadItem( HEXA_NS::Quad* hexaQuad, QString entry = "");
    };
    //-----------------------------------------

    class HexaItem : public GraphicElementItem
    {
      public:
        HexaItem( HEXA_NS::Hexa* hexaHexa, QString entry = "");
    };
    //-----------------------------------------

    class VectorItem : public StandardElementItem
    {
      public:
        VectorItem( HEXA_NS::Vector* hexaVector, QString entry = "");
    };
    //-----------------------------------------

    class CylinderItem : public StandardElementItem
    {
      public:
        CylinderItem( HEXA_NS::Cylinder* hexaCyl, QString entry = "");
    };
    //-----------------------------------------

    class PipeItem : public StandardElementItem
    {
      public:
        PipeItem( HEXA_NS::Pipe* hexaPipe, QString entry = "");
    };
    //-----------------------------------------

    class ElementsItem : public StandardElementItem
    {
      public:
        ElementsItem( HEXA_NS::Elements* hexaElements, QString entry = "" );
    };
    //-----------------------------------------
    class CrossElementsItem : public StandardElementItem
    {
      public:
        CrossElementsItem( HEXA_NS::CrossElements* hexaCrossElts, QString entry = "");
    };

    //------------------------------------------------
    class GeomItem: public StandardElementItem
    {
    public:
        GeomItem( HEXA_NS::EltBase* geomShape, QString entry, HexaType ttype, HexaTreeRole treeRole, HEXA_NS::EltBase* assoc = NULL );
        HEXA_NS::EltBase* getAssociation() const { return association; }
        void setAssociation(HEXA_NS::EltBase* assoc) { association = assoc; }

    private:
        HEXA_NS::EltBase* association;
    };

    //------------------------------------------------
    class GeomShapeItem: public GeomItem
    {
    public:
        GeomShapeItem( HEXA_NS::NewShape* shape, HEXA_NS::EltBase* assoc = NULL);
    };

    //------------------------------------------------
    class GeomPointItem: public GeomItem
    {
    public:
        GeomPointItem( HEXA_NS::VertexShape* geomPoint, HEXA_NS::Vertex* associatedVertex = NULL);
    };

    //------------------------------------------------
    class GeomEdgeItem: public GeomItem
    {
    public:
        GeomEdgeItem( HEXA_NS::EdgeShape* geomEdge, HEXA_NS::Edge* associatedEdge = NULL);
    };

    //------------------------------------------------
    class GeomFaceItem: public GeomItem
    {
    public:
        GeomFaceItem( HEXA_NS::FaceShape* geomFace, HEXA_NS::Quad* associatedQuad = NULL);
    };

    //-----------------------------------------
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
    //-------------------------------------------------

  }
}

#endif

