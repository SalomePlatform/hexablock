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
#include "hexa_base.hxx"
#include "HexVertex_impl.hxx"
#include "HexEdge_impl.hxx"
#include "HexQuad_impl.hxx"
#include "HexHexa_impl.hxx"
#include "HexCylinder_impl.hxx"
#include "HexCrossElements_impl.hxx"
#include "HEXABLOCK.hxx"


CrossElements_impl::CrossElements_impl( HEXA_NS::CrossElements *ptrCpp ):_crossElements_cpp(ptrCpp)
{
}


HEXA_NS::CrossElements* CrossElements_impl::GetImpl()
{
  return _crossElements_cpp;
}


Vertex_ptr CrossElements_impl::getVertexIJK(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Vertex_ptr result = Vertex::_nil();
  HEXA_NS::Vertex* v = _crossElements_cpp->getVertexIJK(part, x, y, z);
  if ( v != NULL ){
    Vertex_impl* servantCorba = new Vertex_impl(v);
    result = servantCorba->_this();
  }
  return result;
}


Edge_ptr CrossElements_impl::getEdgeI(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Edge_ptr result = Edge::_nil();
  HEXA_NS::Edge* e = _crossElements_cpp->getEdgeI(part, x, y, z);
  if ( e != NULL ){
    Edge_impl* servantCorba = new Edge_impl(e);
    result = servantCorba->_this();
  }
  return result;
}


Edge_ptr CrossElements_impl::getEdgeJ(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Edge_ptr result = Edge::_nil();
  HEXA_NS::Edge* e = _crossElements_cpp->getEdgeJ(part, x, y, z);
  if ( e != NULL ){
    Edge_impl* servantCorba = new Edge_impl(e);
    result = servantCorba->_this();
  }
  return result;
}


Edge_ptr CrossElements_impl::getEdgeK(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Edge_ptr result = Edge::_nil();
  HEXA_NS::Edge* e = _crossElements_cpp->getEdgeK(part, x, y, z);
  if ( e != NULL ){
    Edge_impl* servantCorba = new Edge_impl(e);
    result = servantCorba->_this();
  }
  return result;
}

Quad_ptr CrossElements_impl::getQuadIJ(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Quad_ptr result = Quad::_nil();
  HEXA_NS::Quad* q = _crossElements_cpp->getQuadIJ(part, x, y, z);
  if ( q != NULL ){
    Quad_impl* servantCorba = new Quad_impl(q);
    result = servantCorba->_this();
  }
  return result;
}


Quad_ptr CrossElements_impl::getQuadIK(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Quad_ptr result = Quad::_nil();
  HEXA_NS::Quad* q = _crossElements_cpp->getQuadIK(part, x, y, z);
  if ( q != NULL ){
    Quad_impl* servantCorba = new Quad_impl(q);
    result = servantCorba->_this();
  }
  return result;
}


Quad_ptr CrossElements_impl::getQuadJK(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
  throw (SALOME::SALOME_Exception)
{
  Quad_ptr result = Quad::_nil();
  HEXA_NS::Quad* q = _crossElements_cpp->getQuadJK(part, x, y, z);
  if ( q != NULL ){
    Quad_impl* servantCorba = new Quad_impl(q);
    result = servantCorba->_this();
  }
  return result;
}


Hexa_ptr CrossElements_impl::getHexaIJK(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z) throw (SALOME::SALOME_Exception)
{
  Hexa_ptr result = Hexa::_nil();
  HEXA_NS::Hexa* h = _crossElements_cpp->getHexaIJK(part, x, y, z);
  if ( h != NULL ){
    Hexa_impl* servantCorba = new Hexa_impl(h);
    result = servantCorba->_this();
  }
  return result;
}

::CORBA::Long CrossElements_impl::crossCylinders(Cylinder_ptr c1In, Cylinder_ptr  c2In)
throw (SALOME::SALOME_Exception)
{
  ::CORBA::Long result;
  Cylinder_impl* c1InServant = ::DownCast<Cylinder_impl*>( c1In );
  Cylinder_impl* c2InServant = ::DownCast<Cylinder_impl*>( c2In );

  ASSERT( c1InServant );
  ASSERT( c2InServant );

  if ( c1InServant &&  c2InServant ) {
    HEXA_NS::Cylinder* c1 = c1InServant->GetImpl();
    HEXA_NS::Cylinder* c2 = c2InServant->GetImpl();

    result = _crossElements_cpp->crossCylinders(c1,c2);
  }
  return result;

}



::CORBA::Long CrossElements_impl::saveVtk(const char* fname) throw (SALOME::SALOME_Exception)
{
  ::CORBA::Long ret = _crossElements_cpp->saveVtk( fname );
  return ret;
}



void CrossElements_impl::dump() throw (SALOME::SALOME_Exception)
{
  _crossElements_cpp->dump();
}


void CrossElements_impl::printName() throw (SALOME::SALOME_Exception)
{
  _crossElements_cpp->printName();
}

char* CrossElements_impl::getName() throw (SALOME::SALOME_Exception)
{
  char name[12];
  return CORBA::string_dup( _crossElements_cpp->getName(name) );
}

// ========================================================= setName
void CrossElements_impl::setName(const char* name) 
     throw (SALOME::SALOME_Exception)
{
  _crossElements_cpp->setName (name);
}

// ::CORBA::Long CrossElements_impl::countVertex() throw (SALOME::SALOME_Exception)
// {
//   ::CORBA::Long n = _crossElements_cpp->countVertex();
//   return n;
// }
// 
// ::CORBA::Long CrossElements_impl::countEdge() throw (SALOME::SALOME_Exception)
// {
//   ::CORBA::Long n = _crossElements_cpp->countEdge();
//   return n;
// }
// 
// ::CORBA::Long CrossElements_impl::countQuad() throw (SALOME::SALOME_Exception)
// {
//   ::CORBA::Long n = _crossElements_cpp->countQuad();
//   return n;
// }
// 
// ::CORBA::Long CrossElements_impl::countHexa() throw (SALOME::SALOME_Exception)
// {
//   ::CORBA::Long n = _crossElements_cpp->countHexa();
//   return n;
// }
// 
// 
// 
// Vertex_ptr CrossElements_impl::getVertex(::CORBA::Long n) throw (SALOME::SALOME_Exception)
// {
//   HEXA_NS::Vertex* v = _crossElements_cpp->getVertex(n);
//   Vertex_impl* servantCorba = new Vertex_impl(v);
//   return servantCorba->_this();
// }
// 
// Edge_ptr CrossElements_impl::getEdge(::CORBA::Long n) throw (SALOME::SALOME_Exception)
// {
//   HEXA_NS::Edge* e = _crossElements_cpp->getEdge(n);
//   Edge_impl* servantCorba = new Edge_impl(e);
//   return servantCorba->_this();
// }
// 
// Quad_ptr CrossElements_impl::getQuad(::CORBA::Long n) throw (SALOME::SALOME_Exception)
// {
//   HEXA_NS::Quad* q = _crossElements_cpp->getQuad(n);
//   Quad_impl* servantCorba = new Quad_impl(q);
//   return servantCorba->_this();
// }
// 
// Hexa_ptr CrossElements_impl::getHexa(::CORBA::Long n) throw (SALOME::SALOME_Exception)
// {
//   HEXA_NS::Hexa* h = _crossElements_cpp->getHexa(n);
//   Hexa_impl* servantCorba = new Hexa_impl(h);
//   return servantCorba->_this();
// }
// 
// 
// 
// ::CORBA::Long CrossElements_impl::saveVtk( const char* fname ) throw (SALOME::SALOME_Exception)
// {
//   ::CORBA::Long ret = _crossElements_cpp->saveVtk( fname );
//   return ret;
// }
// 
// 
// 
// Vertex_ptr CrossElements_impl::getVertexIJK(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
//   throw (SALOME::SALOME_Exception)
// {
//   Vertex_ptr result = Vertex::_nil();
// //   HEXA_NS::Vertex* v = _crossElements_cpp->getVertexIJK(x, y,z);
//   HEXA_NS::Vertex* v = NULL;
//   _crossElements_cpp->toto();
//   if ( v != NULL ){
//     Vertex_impl* servantCorba = new Vertex_impl(v);
//     result = servantCorba->_this();
//   }
//   return result;
// }
// 
// 
// Edge_ptr CrossElements_impl::getEdgeI(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
//   throw (SALOME::SALOME_Exception)
// {
//   Edge_ptr result = Edge::_nil();
//   HEXA_NS::Edge* e = _crossElements_cpp->getEdgeI(x, y, z);
//   if ( e != NULL ){
//     Edge_impl* servantCorba = new Edge_impl(e);
//     result = servantCorba->_this();
//   }
//   return result;
// }
// 
// 
// Edge_ptr CrossElements_impl::getEdgeJ(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
//   throw (SALOME::SALOME_Exception)
// {
//   Edge_ptr result = Edge::_nil();
//   HEXA_NS::Edge* e = _crossElements_cpp->getEdgeJ(x, y, z);
//   if ( e != NULL ){
//     Edge_impl* servantCorba = new Edge_impl(e);
//     result = servantCorba->_this();
//   }
//   return result;
// }
// 
// 
// Edge_ptr CrossElements_impl::getEdgeK(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
//   throw (SALOME::SALOME_Exception)
// {
//   Edge_ptr result = Edge::_nil();
//   HEXA_NS::Edge* e = _crossElements_cpp->getEdgeK(x, y, z);
//   if ( e != NULL ){
//     Edge_impl* servantCorba = new Edge_impl(e);
//     result = servantCorba->_this();
//   }
//   return result;
// }
// 
// Quad_ptr CrossElements_impl::getQuadIJ(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
//   throw (SALOME::SALOME_Exception)
// {
//   Quad_ptr result = Quad::_nil();
//   HEXA_NS::Quad* q = _crossElements_cpp->getQuadIJ(x, y, z);
//   if ( q != NULL ){
//     Quad_impl* servantCorba = new Quad_impl(q);
//     result = servantCorba->_this();
//   }
//   return result;
// }
// 
// 
// Quad_ptr CrossElements_impl::getQuadIK(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
//   throw (SALOME::SALOME_Exception)
// {
//   Quad_ptr result = Quad::_nil();
//   HEXA_NS::Quad* q = _crossElements_cpp->getQuadIK(x, y, z);
//   if ( q != NULL ){
//     Quad_impl* servantCorba = new Quad_impl(q);
//     result = servantCorba->_this();
//   }
//   return result;
// }
// 
// 
// Quad_ptr CrossElements_impl::getQuadJK(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z)
//   throw (SALOME::SALOME_Exception)
// {
//   Quad_ptr result = Quad::_nil();
//   HEXA_NS::Quad* q = _crossElements_cpp->getQuadJK(x, y, z);
//   if ( q != NULL ){
//     Quad_impl* servantCorba = new Quad_impl(q);
//     result = servantCorba->_this();
//   }
//   return result;
// }
// 
// 
// Hexa_ptr CrossElements_impl::getHexaIJK(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z) throw (SALOME::SALOME_Exception)
// {
//   Hexa_ptr result = Hexa::_nil();
//   HEXA_NS::Hexa* h = _crossElements_cpp->getHexaIJK(x, y, z);
//   if ( h != NULL ){
//     Hexa_impl* servantCorba = new Hexa_impl(h);
//     result = servantCorba->_this();
//   }
//   return result;
// }


