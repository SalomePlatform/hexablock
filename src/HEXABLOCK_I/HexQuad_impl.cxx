// Copyright (C) 2009-2013  CEA/DEN, EDF R&D
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
#include "HexQuad_impl.hxx"
#include "HexVertex_impl.hxx"
#include "HexEdge_impl.hxx"

Quad_impl::Quad_impl( HEXA_NS::Quad *ptrCpp ):_quad_cpp(ptrCpp)
{
}

HEXA_NS::Quad* Quad_impl::GetImpl()
  throw (SALOME::SALOME_Exception)
{
  return _quad_cpp;
}

Edge_ptr Quad_impl::getEdge(::CORBA::Long n)
  throw (SALOME::SALOME_Exception)
{
  Edge_ptr result = Edge::_nil();

  HEXA_NS::Edge* e = _quad_cpp->getEdge( n );
  if ( e != NULL ){
    Edge_impl* servantCorba = new Edge_impl(e);
    result = servantCorba->_this();
  }
  return result;
}


Vertex_ptr Quad_impl::getVertex(::CORBA::Long n)
   throw (SALOME::SALOME_Exception)
{
  Vertex_ptr result = Vertex::_nil();

  HEXA_NS::Vertex* v = _quad_cpp->getVertex( n );
  if ( v != NULL ){
    Vertex_impl* servantCorba = new Vertex_impl(v);
    result = servantCorba->_this();
  }
  return result;
}




::CORBA::Long Quad_impl::addAssociation(NewShape_ptr geom, ::CORBA::Long subid)
  throw (SALOME::SALOME_Exception)
{
  NewShape_impl*     im_shape = ::DownCast<NewShape_impl*> (geom );
  HEXA_NS::NewShape* md_shape = im_shape->GetImpl();

  ::CORBA::Long ier = _quad_cpp->addAssociation (md_shape, subid);
  return ier;
}

void Quad_impl::clearAssociation()
      throw (SALOME::SALOME_Exception)
{
  _quad_cpp->clearAssociation ();
}

void Quad_impl::setColor (::CORBA::Double val)
     throw (SALOME::SALOME_Exception)
{
  _quad_cpp->setColor (val);
}

void Quad_impl::setScalar( ::CORBA::Double val )throw (SALOME::SALOME_Exception)
{
  _quad_cpp->setScalar(val);
}

void Quad_impl::dump() throw (SALOME::SALOME_Exception)
{
  _quad_cpp->dump();
}


void Quad_impl::printName() throw (SALOME::SALOME_Exception)
{
  _quad_cpp->printName();
}

char* Quad_impl::getName() throw (SALOME::SALOME_Exception)
{
 return CORBA::string_dup( _quad_cpp->getName() );
}
// ========================================================= setName
void Quad_impl::setName(const char* name)
     throw (SALOME::SALOME_Exception)
{
  _quad_cpp->setName (name);
}
