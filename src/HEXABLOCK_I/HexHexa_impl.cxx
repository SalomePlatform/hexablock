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
#include "HexHexa_impl.hxx"
#include "HexQuad_impl.hxx"
#include "HexEdge_impl.hxx"
#include "HexVertex_impl.hxx"

Hexa_impl::Hexa_impl( HEXA_NS::Hexa *ptrCpp ):_hexa_cpp(ptrCpp)
{
}

HEXA_NS::Hexa* Hexa_impl::GetImpl()
    throw (SALOME::SALOME_Exception)
{
  return _hexa_cpp;
}

Quad_ptr Hexa_impl::getQuad(::CORBA::Long n)
    throw (SALOME::SALOME_Exception)
{
  Quad_ptr result = Quad::_nil();

  HEXA_NS::Quad* q = _hexa_cpp->getQuad(n);
  if ( q != NULL ){
    Quad_impl* servantCorba = new Quad_impl(q);
    result = servantCorba->_this();
  }
  return result;
}

Edge_ptr Hexa_impl::getEdge(::CORBA::Long n)
    throw (SALOME::SALOME_Exception)
{
  Edge_ptr result = Edge::_nil();

  HEXA_NS::Edge* e = _hexa_cpp->getEdge(n);
  if ( e != NULL ){
    Edge_impl* servantCorba = new Edge_impl(e);
    result = servantCorba->_this();
  }
  return result;
}

Vertex_ptr Hexa_impl::getVertex(::CORBA::Long n)
    throw (SALOME::SALOME_Exception)
{
  Vertex_ptr result = Vertex::_nil();

  HEXA_NS::Vertex* v = _hexa_cpp->getVertex(n);
  if ( v != NULL ){
    Vertex_impl* servantCorba = new Vertex_impl(v);
    result = servantCorba->_this();
  }
  return result;
}

void Hexa_impl::setScalar( ::CORBA::Double val )throw (SALOME::SALOME_Exception)
{
  _hexa_cpp->setScalar(val);
}

void Hexa_impl::dump() throw (SALOME::SALOME_Exception)
{
  _hexa_cpp->dump();
}


void Hexa_impl::printName() throw (SALOME::SALOME_Exception)
{
  _hexa_cpp->printName();
}


char* Hexa_impl::getName() throw (SALOME::SALOME_Exception)
{
   return CORBA::string_dup( _hexa_cpp->getName() );
}

// ========================================================= setName
void Hexa_impl::setName(const char* name) 
     throw (SALOME::SALOME_Exception)
{
  _hexa_cpp->setName (name);
}
