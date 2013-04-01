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
#include "hexa_base.hxx"
#include "HexVertex_impl.hxx"
#include "HexEdge_impl.hxx"
#include "HexQuad_impl.hxx"
#include "HexHexa_impl.hxx"
#include "HexCylinder_impl.hxx"
#include "HexBiCylinder_impl.hxx"
#include "HEXABLOCK.hxx"


BiCylinder_impl::BiCylinder_impl( HEXA_NS::BiCylinder *ptrCpp )
{
   bi_cylinder_cpp = ptrCpp;
}


HEXA_NS::BiCylinder* BiCylinder_impl::GetImpl()
{
  return bi_cylinder_cpp;
}


Vertex_ptr BiCylinder_impl::getVertexIJK(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Vertex_ptr result = Vertex::_nil();
  HEXA_NS::Vertex* v = bi_cylinder_cpp->getVertexIJK(part, x, y, z);
  if ( v != NULL ){
    Vertex_impl* servantCorba = new Vertex_impl(v);
    result = servantCorba->_this();
  }
  return result;
}


Edge_ptr BiCylinder_impl::getEdgeI(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Edge_ptr result = Edge::_nil();
  HEXA_NS::Edge* e = bi_cylinder_cpp->getEdgeI(part, x, y, z);
  if ( e != NULL ){
    Edge_impl* servantCorba = new Edge_impl(e);
    result = servantCorba->_this();
  }
  return result;
}


Edge_ptr BiCylinder_impl::getEdgeJ(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Edge_ptr result = Edge::_nil();
  HEXA_NS::Edge* e = bi_cylinder_cpp->getEdgeJ(part, x, y, z);
  if ( e != NULL ){
    Edge_impl* servantCorba = new Edge_impl(e);
    result = servantCorba->_this();
  }
  return result;
}


Edge_ptr BiCylinder_impl::getEdgeK(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Edge_ptr result = Edge::_nil();
  HEXA_NS::Edge* e = bi_cylinder_cpp->getEdgeK(part, x, y, z);
  if ( e != NULL ){
    Edge_impl* servantCorba = new Edge_impl(e);
    result = servantCorba->_this();
  }
  return result;
}

Quad_ptr BiCylinder_impl::getQuadIJ(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Quad_ptr result = Quad::_nil();
  HEXA_NS::Quad* q = bi_cylinder_cpp->getQuadIJ(part, x, y, z);
  if ( q != NULL ){
    Quad_impl* servantCorba = new Quad_impl(q);
    result = servantCorba->_this();
  }
  return result;
}


Quad_ptr BiCylinder_impl::getQuadIK(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Quad_ptr result = Quad::_nil();
  HEXA_NS::Quad* q = bi_cylinder_cpp->getQuadIK(part, x, y, z);
  if ( q != NULL ){
    Quad_impl* servantCorba = new Quad_impl(q);
    result = servantCorba->_this();
  }
  return result;
}


Quad_ptr BiCylinder_impl::getQuadJK(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Quad_ptr result = Quad::_nil();
  HEXA_NS::Quad* q = bi_cylinder_cpp->getQuadJK(part, x, y, z);
  if ( q != NULL ){
    Quad_impl* servantCorba = new Quad_impl(q);
    result = servantCorba->_this();
  }
  return result;
}


Hexa_ptr BiCylinder_impl::getHexaIJK(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z) throw (SALOME::SALOME_Exception)
{
  Hexa_ptr result = Hexa::_nil();
  HEXA_NS::Hexa* h = bi_cylinder_cpp->getHexaIJK(part, x, y, z);
  if ( h != NULL ){
    Hexa_impl* servantCorba = new Hexa_impl(h);
    result = servantCorba->_this();
  }
  return result;
}
::CORBA::Long BiCylinder_impl::saveVtk(const char* fname) throw (SALOME::SALOME_Exception)
{
  ::CORBA::Long ret = bi_cylinder_cpp->saveVtk( fname );
  return ret;
}

void BiCylinder_impl::dump() throw (SALOME::SALOME_Exception)
{
  bi_cylinder_cpp->dump();
}


void BiCylinder_impl::printName() throw (SALOME::SALOME_Exception)
{
  bi_cylinder_cpp->printName();
}

char* BiCylinder_impl::getName() throw (SALOME::SALOME_Exception)
{
  return CORBA::string_dup( bi_cylinder_cpp->getName() );
}

// ========================================================= setName
void BiCylinder_impl::setName(const char* name)
     throw (SALOME::SALOME_Exception)
{
  bi_cylinder_cpp->setName (name);
}

