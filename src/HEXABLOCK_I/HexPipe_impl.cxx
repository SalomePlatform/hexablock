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
using namespace std;
#include "HEXABLOCK.hxx"
#include "utilities.h"

#include <string>

#include "hexa_base.hxx"
#include "HexVertex_impl.hxx"
#include "HexVector_impl.hxx"
#include "HexPipe_impl.hxx"

Pipe_impl::Pipe_impl( HEXA_NS::Pipe *ptrCpp ):_pipe_cpp(ptrCpp)
{
}

HEXA_NS::Pipe* Pipe_impl::GetImpl() throw (SALOME::SALOME_Exception)
{
  return _pipe_cpp;
}

Vertex_ptr Pipe_impl::getBase() throw (SALOME::SALOME_Exception)
{
  Vertex_ptr result = Vertex::_nil();

  HEXA_NS::Vertex* v = _pipe_cpp->getBase();
  if ( v!= NULL ){  
    Vertex_impl* servantCorba = new Vertex_impl(v);
    result = servantCorba->_this();
  }
  return result;
}

Vector_ptr Pipe_impl::getDirection() throw (SALOME::SALOME_Exception)
{
  Vector_ptr result = Vector::_nil();

  HEXA_NS::Vector* v = _pipe_cpp->getDirection();

  if ( v!= NULL ){  
    Vector_impl* servantCorba = new Vector_impl(v);
    result = servantCorba->_this();
  }
  return result;
}

::CORBA::Double Pipe_impl::getInternal_radius() throw (SALOME::SALOME_Exception)
{
//   ::CORBA::Double d = _pipe_cpp->getInternal_radius(); CS_TODO
  ::CORBA::Double d = _pipe_cpp->getInternalRadius();
  return d;
}

::CORBA::Double Pipe_impl::getRadius() throw (SALOME::SALOME_Exception)
{
  ::CORBA::Double d = _pipe_cpp->getRadius();
  return d;
}

::CORBA::Double Pipe_impl::getHeight() throw (SALOME::SALOME_Exception)
{
  ::CORBA::Double d = _pipe_cpp->getHeight();
  return d;
}


void Pipe_impl::dump() throw (SALOME::SALOME_Exception)
{
  _pipe_cpp->dump();
}


void Pipe_impl::printName() throw (SALOME::SALOME_Exception)
{
  _pipe_cpp->printName();
}


char* Pipe_impl::getName() throw (SALOME::SALOME_Exception)
{
  return CORBA::string_dup( _pipe_cpp->getName() );
}
// ========================================================= setName
void Pipe_impl::setName(const char* name) 
     throw (SALOME::SALOME_Exception)
{
  _pipe_cpp->setName (name);
}
