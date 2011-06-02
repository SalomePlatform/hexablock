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
#include "HexShape.hxx"

Vertex_impl::Vertex_impl( HEXA_NS::Vertex *ptrCpp ):
_vertex_cpp( ptrCpp )
// _association( GEOM::GEOM_Object::_nil() )
{
}

HEXA_NS::Vertex* Vertex_impl::GetImpl()
{
  return _vertex_cpp;
}

CORBA::Double Vertex_impl::getX() throw(SALOME::SALOME_Exception)
{
  return _vertex_cpp->getX();
}

CORBA::Double Vertex_impl::getY() throw(SALOME::SALOME_Exception)
{
  return _vertex_cpp->getY();
}

CORBA::Double Vertex_impl::getZ() throw(SALOME::SALOME_Exception)
{
  return _vertex_cpp->getZ();
}


void Vertex_impl::setX( CORBA::Double x ) throw(SALOME::SALOME_Exception)
{
  _vertex_cpp->setX(x);
}
void Vertex_impl::setY(  CORBA::Double y ) throw(SALOME::SALOME_Exception)
{
  _vertex_cpp->setY(y);
}

void Vertex_impl::setZ(  CORBA::Double z ) throw(SALOME::SALOME_Exception)
{
  _vertex_cpp->setY(z);
}


void Vertex_impl::setAssociation(GEOM::GEOM_Object_ptr geom_object_vertex)
  throw (SALOME::SALOME_Exception)
{
  TopoDS_Shape shape = HEXABLOCK::GetHEXABLOCKGen()->geomObjectToShape(geom_object_vertex);
//   HEXABLOCK::HEXABLOCK* gen = HEXABLOCK::GetHEXABLOCKGen();
//   TopoDS_Shape shape = gen->geomObjectToShape(geom_object_vertex);
  string strBrep = shape2string( shape );
  HEXA_NS::Shape* s = new HEXA_NS::Shape( strBrep );
  _vertex_cpp->setAssociation(s);

//   _association = GEOM::GEOM_Object::_duplicate( geom_object_vertex );
}


GEOM::GEOM_Object_ptr Vertex_impl::getAssociation()
  throw (SALOME::SALOME_Exception)
{
//   GEOM::GEOM_Object_var result = GEOM::GEOM_Object::_nil();
//   GEOM::GEOM_Object_ptr result = GEOM::GEOM_Object::_nil();

  GEOM::GEOM_Object_var result; // = new GEOM::GEOM_Object;

  HEXA_NS::Shape* s = _vertex_cpp->getAssociation();

  std::cout << "getAssociation ->" << s << std::endl;

  if (s != NULL){
    string strBrep = s->getBrep();
    TopoDS_Shape shape = string2shape( strBrep );
    result = HEXABLOCK::GetHEXABLOCKGen()->shapeToGeomObject(shape);
  }

// //   _vertex_cpp->getAssociation()->_topo;
// //   result = HEXABLOCK::GetHEXABLOCKGen()->shapeToGeomObject(_shape);

//   return result;
  return result._retn();
//   return GEOM::GEOM_Object::_duplicate( _association );
}



// void Vertex_impl::removeAssociation()
//   throw (SALOME::SALOME_Exception)
// {
// }


void Vertex_impl::setScalar( ::CORBA::Double val )throw (SALOME::SALOME_Exception)
{
  _vertex_cpp->setScalar(val);
}

void Vertex_impl::dump() throw (SALOME::SALOME_Exception)
{
  _vertex_cpp->dump();
}


void Vertex_impl::printName()throw (SALOME::SALOME_Exception)
{
  _vertex_cpp->printName();
}
