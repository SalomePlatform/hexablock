// Copyright (C) 2009-2011  CEA/DEN, EDF R&D
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

using namespace std;
#include "HEXABLOCK.hxx"
#include "utilities.h"

#include <string>
#include "hexa_base.hxx"

#include "HexVertex_impl.hxx"
#include "HexEdge_impl.hxx"
#include "HexShape.hxx"

using namespace HEXABLOCK_ORB;

Edge_impl::Edge_impl( HEXA_NS::Edge *ptrCpp ):
_edge_cpp(ptrCpp)
{
}

HEXA_NS::Edge* Edge_impl::GetImpl()
  throw (SALOME::SALOME_Exception)
{
  return _edge_cpp;
}


::CORBA::Boolean Edge_impl::getWay() throw (SALOME::SALOME_Exception)
{
//   bool way = _edge_cpp->getWay();
//   if ( way == true ) {
//     return CORBA::TRUE;
//   else {
//     return CORBA::FALSE;
//   }
  return _edge_cpp->getWay();
}


Vertex_ptr Edge_impl::getVertex(::CORBA::Long n)
  throw (SALOME::SALOME_Exception)
{
  Vertex_ptr result = Vertex::_nil();

  HEXA_NS::Vertex* v = _edge_cpp->getVertex(n);
  if ( v!= NULL ){
    Vertex_impl* servantCorba = new Vertex_impl(v);
    result = servantCorba->_this();
  }
  return result ;
}


void Edge_impl::dump() throw (SALOME::SALOME_Exception)
{
  _edge_cpp->dump();
}


void Edge_impl::printName()throw (SALOME::SALOME_Exception)
{
  _edge_cpp->printName();
}



void Edge_impl::setScalar( ::CORBA::Double val )throw (SALOME::SALOME_Exception)
{
  _edge_cpp->setScalar(val);
}


::CORBA::Long Edge_impl::addAssociation( GEOM::GEOM_Object_ptr geom_object_1D, double debut, double fin )
  throw (SALOME::SALOME_Exception)
{
  ::CORBA::Long ok;
  TopoDS_Shape aShape = HEXABLOCK::GetHEXABLOCKGen()->geomObjectToShape( geom_object_1D );
  string strBrep = shape2string( aShape );
  HEXA_NS::Shape* s = new HEXA_NS::Shape( strBrep );
  s->debut = debut;
  s->fin   = fin;
  ok = _edge_cpp->addAssociation( s );


  /// Edge_impl::Assoc assoc;
  /// assoc.geomObj = GEOM::GEOM_Object::_duplicate( geom_object_1D );
  /// assoc.debut   = debut;
  /// assoc.fin     = fin;
  /// _associations.push_back(assoc);

  return ok;
}



// EdgeAssociations* Edge_impl::getAssociations() //CS_NOT_SPEC
//   throw (SALOME::SALOME_Exception)
// {
//   HEXABLOCK_ORB::EdgeAssociations* result = new HEXABLOCK_ORB::EdgeAssociations;
//   result->length( _associations.size() );
// 
//   HEXABLOCK_ORB::EdgeAssociation assoc;
//   CORBA::ULong i = 0;
//   for ( std::vector<Edge_impl::Assoc>::const_iterator iter = _associations.begin();
// 	iter != _associations.end();
//         ++iter){
// //       assoc.geomObj = (*iter).geomObj;
//       assoc.geomObj = GEOM::GEOM_Object::_duplicate( (*iter).geomObj );
//       assoc.debut = (*iter).debut;
//       assoc.fin   = (*iter).fin;
//       (*result)[i++] = assoc;
//   }
//   return result;
// }




EdgeAssociations* Edge_impl::getAssociations ()
  throw (SALOME::SALOME_Exception)
{
  HEXABLOCK_ORB::EdgeAssociations* result = new HEXABLOCK_ORB::EdgeAssociations;
//   HEXABLOCK_ORB::GEOM_Object_ptr ge;
//   GEOM::GEOM_Object_ptr ge;
  TopoDS_Shape aShape;
  const std::vector<HEXA_NS::Shape*> shapes = _edge_cpp->getAssociations();

  result->length( shapes.size() );
  HEXABLOCK_ORB::EdgeAssociation assoc;
  CORBA::ULong i = 0;
  for ( std::vector<HEXA_NS::Shape*>::const_iterator iter = shapes.begin();
	iter != shapes.end();
        ++iter ){
      aShape = string2shape( (*iter)->getBrep());
//       ge = GEOM::GEOM_Object::_duplicate(HEXABLOCK::GetHEXABLOCKGen()->shapeToGeomObject( aShape )); 
//       ge = HEXABLOCK_ORB::GEOM_Edge::_duplicate(HEXABLOCK::GetHEXABLOCKGen()->shapeToGeomObject( aShape ));
//       ge = HEXABLOCK::GetHEXABLOCKGen()->shapeToGeomObject( aShape );
//       ge->debut( (*iter)->debut );
//       ge->fin( (*iter)->fin );

//       assoc.geomObj = GEOM::GEOM_Object::_duplicate( (*iter).geomObj );
      std::cout <<"AAAAAA"<<std::endl;
      assoc.geomObj = HEXABLOCK::GetHEXABLOCKGen()->shapeToGeomObject( aShape );
      assoc.debut = (*iter)->debut;
      assoc.fin   = (*iter)->fin;
      (*result)[ i++ ] = assoc;
  }

  return result;
}


