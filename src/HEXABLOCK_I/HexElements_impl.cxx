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
#include "hexa_base.hxx"
#include "HexElements_impl.hxx"
#include "HexVertex_impl.hxx"
#include "HexEdge_impl.hxx"
#include "HexQuad_impl.hxx"
#include "HexHexa_impl.hxx"


Elements_impl::Elements_impl( HEXA_NS::Elements *ptrCpp ):_elements_cpp(ptrCpp)
{
}

HEXA_NS::Elements* Elements_impl::GetImpl()
{
  return _elements_cpp;
}

::CORBA::Long Elements_impl::countVertex() throw (SALOME::SALOME_Exception)
{
  ::CORBA::Long n = _elements_cpp->countVertex();
  return n;
}

::CORBA::Long Elements_impl::countEdge() throw (SALOME::SALOME_Exception)
{
  ::CORBA::Long n = _elements_cpp->countEdge();
  return n;
}

::CORBA::Long Elements_impl::countQuad() throw (SALOME::SALOME_Exception)
{
  ::CORBA::Long n = _elements_cpp->countQuad();
  return n;
}

::CORBA::Long Elements_impl::countHexa() throw (SALOME::SALOME_Exception)
{
  ::CORBA::Long n = _elements_cpp->countHexa();
  return n;
}

::CORBA::Long Elements_impl::findVertex( Vertex_ptr pIn ) throw (SALOME::SALOME_Exception)
{
  ::CORBA::Long n;
  Vertex_impl* pInServant = ::DownCast<Vertex_impl*>( pIn );
  ASSERT( pInServant );
  HEXA_NS::Vertex* p = pInServant->GetImpl();
  n = _elements_cpp->findVertex( p );
  return n;
}



Vertex_ptr Elements_impl::getVertex(::CORBA::Long n) throw (SALOME::SALOME_Exception)
{
  Vertex_ptr result  = Vertex::_nil();

  HEXA_NS::Vertex* v = _elements_cpp->getVertex(n);
  if ( v != NULL ){
    Vertex_impl* servantCorba = new Vertex_impl(v);
    result = servantCorba->_this();
  }
  return result;
}

Edge_ptr Elements_impl::getEdge(::CORBA::Long n) throw (SALOME::SALOME_Exception)
{
  Edge_ptr result  = Edge::_nil();

  HEXA_NS::Edge* e = _elements_cpp->getEdge(n);
  if ( e != NULL ){
    Edge_impl* servantCorba = new Edge_impl(e);
    result = servantCorba->_this();
  }
  return result;
}

Quad_ptr Elements_impl::getQuad(::CORBA::Long n) throw (SALOME::SALOME_Exception)
{
  Quad_ptr result = Quad::_nil();

  HEXA_NS::Quad* q = _elements_cpp->getQuad(n);
  if ( q != NULL ){
    Quad_impl* servantCorba = new Quad_impl(q);
    result = servantCorba->_this();
  }
  return result;
}

Hexa_ptr Elements_impl::getHexa(::CORBA::Long n) throw (SALOME::SALOME_Exception)
{
  Hexa_ptr result = Hexa::_nil();

  HEXA_NS::Hexa* h = _elements_cpp->getHexa(n);
  if ( h != NULL ){
    Hexa_impl* servantCorba = new Hexa_impl(h);
    result = servantCorba->_this();
  }
  return result;
}


::CORBA::Long Elements_impl::saveVtk( const char* fname ) throw (SALOME::SALOME_Exception)
{
  ::CORBA::Long ret = _elements_cpp->saveVtk( fname );
  return ret;
}


void Elements_impl::dump() throw (SALOME::SALOME_Exception)
{
  _elements_cpp->dump();
}


void Elements_impl::printName() throw (SALOME::SALOME_Exception)
{
  _elements_cpp->printName();
}

char* Elements_impl::getName() throw (SALOME::SALOME_Exception)
{
   char name[12];
   return CORBA::string_dup( _elements_cpp->getName(name) );
}

// ========================================================= setName
void Elements_impl::setName(const char* name) 
     throw (SALOME::SALOME_Exception)
{
  _elements_cpp->setName (name);
}




Vertex_ptr Elements_impl::getVertexIJK(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Vertex_ptr result = Vertex::_nil();
  HEXA_NS::Vertex* v = _elements_cpp->getVertexIJK(x, y, z);
  if ( v != NULL ){
    Vertex_impl* servantCorba = new Vertex_impl(v);
    result = servantCorba->_this();
  }
  return result;
}


Edge_ptr Elements_impl::getEdgeI(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Edge_ptr result = Edge::_nil();
  HEXA_NS::Edge* e = _elements_cpp->getEdgeI(x, y, z);
  if ( e != NULL ){
    Edge_impl* servantCorba = new Edge_impl(e);
    result = servantCorba->_this();
  }
  return result;
}


Edge_ptr Elements_impl::getEdgeJ(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Edge_ptr result = Edge::_nil();
  HEXA_NS::Edge* e = _elements_cpp->getEdgeJ(x, y, z);
  if ( e != NULL ){
    Edge_impl* servantCorba = new Edge_impl(e);
    result = servantCorba->_this();
  }
  return result;
}


Edge_ptr Elements_impl::getEdgeK(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Edge_ptr result = Edge::_nil();
  HEXA_NS::Edge* e = _elements_cpp->getEdgeK(x, y, z);
  if ( e != NULL ){
    Edge_impl* servantCorba = new Edge_impl(e);
    result = servantCorba->_this();
  }
  return result;
}

Quad_ptr Elements_impl::getQuadIJ(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Quad_ptr result = Quad::_nil();
  HEXA_NS::Quad* q = _elements_cpp->getQuadIJ(x, y, z);
  if ( q != NULL ){
    Quad_impl* servantCorba = new Quad_impl(q);
    result = servantCorba->_this();
  }
  return result;
}


Quad_ptr Elements_impl::getQuadIK(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Quad_ptr result = Quad::_nil();
  HEXA_NS::Quad* q = _elements_cpp->getQuadIK(x, y, z);
  if ( q != NULL ){
    Quad_impl* servantCorba = new Quad_impl(q);
    result = servantCorba->_this();
  }
  return result;
}


Quad_ptr Elements_impl::getQuadJK(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Quad_ptr result = Quad::_nil();
  HEXA_NS::Quad* q = _elements_cpp->getQuadJK(x, y, z);
  if ( q != NULL ){
    Quad_impl* servantCorba = new Quad_impl(q);
    result = servantCorba->_this();
  }
  return result;
}


Hexa_ptr Elements_impl::getHexaIJK(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z) throw (SALOME::SALOME_Exception)
{
  Hexa_ptr result = Hexa::_nil();
  HEXA_NS::Hexa* h = _elements_cpp->getHexaIJK(x, y, z);
  if ( h != NULL ){
    Hexa_impl* servantCorba = new Hexa_impl(h);
    result = servantCorba->_this();
  }
  return result;
}


// Quad_ptr Elements_impl::getQuad1(EnumCoord dirIn, ::CORBA::Long nx, ::CORBA::Long ny, ::CORBA::Long nz)
//       throw (SALOME::SALOME_Exception)
// {
//   Quad_ptr result = Quad::_nil();
// 
// //   EnumCoord k;
// //   HEXA_NS::EnumCoord k_cpp;
//   HEXA_NS::EnumCoord dir_cpp;
// 
//   switch (dirIn) {
//     case DIR_X : dir_cpp = HEXA_NS::dir_x; break;
//     case DIR_Y : dir_cpp = HEXA_NS::dir_y; break;
//     case DIR_Z : dir_cpp = HEXA_NS::dir_z; break;
//     case DIM3:   dir_cpp = HEXA_NS::DIM3; break;
//   }
// 
//   HEXA_NS::Quad* q = _elements_cpp->getQuad(dir_cpp, nx, ny, nz);
//   if ( q != NULL ){
//     Quad_impl* servantCorba = new Quad_impl(q);
//     result = servantCorba->_this();
//   }
// 
//   return result;
// }
// 
// Edge_ptr Elements_impl::getEdge1(EnumCoord dirIn, ::CORBA::Long nx, ::CORBA::Long ny, ::CORBA::Long nz)
//       throw (SALOME::SALOME_Exception)
// {
//   Edge_ptr result = Edge::_nil();
// 
// //   EnumCoord k;
// //   HEXA_NS::EnumCoord k_cpp;
//   HEXA_NS::EnumCoord dir_cpp;
// 
//   switch (dirIn) {
//     case DIR_X : dir_cpp = HEXA_NS::dir_x; break;
//     case DIR_Y : dir_cpp = HEXA_NS::dir_y; break;
//     case DIR_Z : dir_cpp = HEXA_NS::dir_z; break;
//     case DIM3:   dir_cpp = HEXA_NS::DIM3; break;
//   }
// 
//   HEXA_NS::Edge* e = _elements_cpp->getEdge(dir_cpp, nx, ny, nz);
//   if ( e != NULL ){
//     Edge_impl* servantCorba = new Edge_impl(e);
//     result = servantCorba->_this();
//   }
// 
//   return result;
// }

