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
using namespace std;
#include "HEXABLOCK.hxx"
#include "utilities.h"

#include <string>

#include "hexa_base.hxx"
#include "HexCylinder_impl.hxx"
#include "HexVertex_impl.hxx"
#include "HexVector_impl.hxx"

Cylinder_impl::Cylinder_impl( HEXA_NS::Cylinder *ptrCpp ):_cylinder_cpp(ptrCpp)
{
}

HEXA_NS::Cylinder* Cylinder_impl::GetImpl()
  throw (SALOME::SALOME_Exception)
{
  return _cylinder_cpp;
}


Vertex_ptr Cylinder_impl::getBase()
  throw (SALOME::SALOME_Exception)
{
  Vertex_ptr result = Vertex::_nil();

  HEXA_NS::Vertex* v = _cylinder_cpp->getBase();
  if ( v!= NULL ){
    Vertex_impl* servantCorba = new Vertex_impl(v);
    result = servantCorba->_this();
  }

  return result;
}

Vector_ptr Cylinder_impl::getDirection()
  throw (SALOME::SALOME_Exception)
{
  Vector_ptr result = Vector::_nil();

  HEXA_NS::Vector* v = _cylinder_cpp->getDirection();
  if ( v!= NULL ){
    Vector_impl* servantCorba = new Vector_impl(v);
    result = servantCorba->_this();
  }
  return result;
}


::CORBA::Double Cylinder_impl::getRadius()
  throw (SALOME::SALOME_Exception)
{
 ::CORBA::Double d = _cylinder_cpp->getRadius();
  return d;
}


::CORBA::Double Cylinder_impl::getHeight()
  throw (SALOME::SALOME_Exception)
{
 ::CORBA::Double d = _cylinder_cpp->getHeight();
  return d;
}


void Cylinder_impl::dump() throw (SALOME::SALOME_Exception)
{
  _cylinder_cpp->dump();
}


void Cylinder_impl::printName() throw (SALOME::SALOME_Exception)
{
  _cylinder_cpp->printName();
}


char* Cylinder_impl::getName() throw (SALOME::SALOME_Exception)
{
  char name[12];
  return CORBA::string_dup( _cylinder_cpp->getName(name) );
}
// ========================================================= setName
void Cylinder_impl::setName(const char* name) 
     throw (SALOME::SALOME_Exception)
{
  _cylinder_cpp->setName (name);
}

// ::CORBA::Long Cylinder_impl::saveVtk(const char* fname) throw (SALOME::SALOME_Exception)
// {
//   ::CORBA::Long ret = _cylinder_cpp->saveVtk( fname );
//   return ret;
// }


