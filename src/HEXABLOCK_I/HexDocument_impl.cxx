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

#include "utilities.h"

#include <string>

#include "HEXABLOCK.hxx"
#include "HexDocument_impl.hxx"
// 

#include "HexVertex_impl.hxx"
#include "HexEdge_impl.hxx"
#include "HexQuad_impl.hxx"
#include "HexHexa_impl.hxx"
#include "HexVector_impl.hxx"
#include "HexElements_impl.hxx"
#include "HexCrossElements_impl.hxx"
#include "HexCylinder_impl.hxx"
#include "HexPipe_impl.hxx"
#include "HexLaw_impl.hxx"
#include "HexPropagation_impl.hxx"
#include "HexGroup_impl.hxx"
#include "HexPipe_impl.hxx"

#include "HexShape.hxx"


using namespace HEXABLOCK_ORB;


Document_impl::Document_impl( PortableServer::POA_ptr thePOA, HEXA_NS::Document *ptrCpp):
SALOME::GenericObj_i( thePOA ),
_document_cpp(ptrCpp)
{
}


// Document_impl::Document_impl(HEXA_NS::Document *ptrCpp):_document_cpp(ptrCpp)
// {
// }

::CORBA::Long Document_impl::saveVtk(const char* fname) throw (SALOME::SALOME_Exception)
{
  ::CORBA::Long ret = _document_cpp->saveVtk( fname );
  return ret;
}
void Document_impl::purge() throw (SALOME::SALOME_Exception)
{
  _document_cpp->purge();
}
void Document_impl::dump() throw (SALOME::SALOME_Exception)
{
  _document_cpp->dump();
}

HEXA_NS::Document* Document_impl::GetImpl() throw (SALOME::SALOME_Exception)
{
  return _document_cpp;
}

::CORBA::Long Document_impl::save(const char* fileName)
                             throw (SALOME::SALOME_Exception)
{
  ::CORBA::Long ret = _document_cpp->save(fileName);
  return ret;
}

::CORBA::Long Document_impl::setName(const char* name) 
                             throw (SALOME::SALOME_Exception)
{
  ::CORBA::Long ret = _document_cpp->setName(name);
  return ret;
}

void Document_impl::setTolerance(::CORBA::Double tol)
                    throw (SALOME::SALOME_Exception)
{
  _document_cpp->setTolerance(tol);
}

::CORBA::Double Document_impl::getTolerance() throw (SALOME::SALOME_Exception)
{
  return   _document_cpp->getTolerance();
}

Vertex_ptr Document_impl::addVertex(::CORBA::Double x, ::CORBA::Double y, ::CORBA::Double z)
  throw(SALOME::SALOME_Exception)
{
  Vertex_ptr result   = Vertex::_nil();

  HEXA_NS::Vertex* v  = _document_cpp->addVertex( x, y, z );
  if ( v != NULL ){
    Vertex_impl* servantCorba = new Vertex_impl(v);
    result = servantCorba->_this();
  }
  return result;
}

::CORBA::Long Document_impl::countVertex() throw (SALOME::SALOME_Exception)
{
  return  _document_cpp->countVertex();
}

Vertex_ptr Document_impl::getVertex(::CORBA::Long i) throw (SALOME::SALOME_Exception)
{
  Vertex_ptr result   = Vertex::_nil();

  HEXA_NS::Vertex* v = _document_cpp->getVertex(i);
  if ( v != NULL ){
    Vertex_impl* servantCorba = new Vertex_impl(v);
    result = servantCorba->_this();
  }
  return result;
}


Vertex_ptr Document_impl::findVertex(::CORBA::Double x, ::CORBA::Double y, ::CORBA::Double z)
  throw (SALOME::SALOME_Exception)
{
  Vertex_ptr result = Vertex::_nil();

  HEXA_NS::Vertex* v = _document_cpp->findVertex( x, y, z);
  if ( v != NULL ){
    Vertex_impl* servantCorba = new Vertex_impl(v);
    result = servantCorba->_this();
  }
  return result;
}


Edge_ptr Document_impl::addEdge(Vertex_ptr v0In, Vertex_ptr v1In)
  throw(SALOME::SALOME_Exception)
{
  Edge_ptr result = Edge::_nil();

  Vertex_impl* v0InServant = ::DownCast<Vertex_impl*>( v0In );
  Vertex_impl* v1InServant = ::DownCast<Vertex_impl*>( v1In );
  ASSERT( v0InServant );
  ASSERT( v1InServant );
  if ( v0InServant &&  v1InServant ) {
    HEXA_NS::Vertex* v0 = v0InServant->GetImpl();
    HEXA_NS::Vertex* v1 = v1InServant->GetImpl();
    HEXA_NS::Edge* e = _document_cpp->addEdge( v0, v1 );
    if ( e != NULL ){
      Edge_impl* servantCorba = new Edge_impl(e);
      result = servantCorba->_this();
    }
  }
  return result;
}


::CORBA::Long Document_impl::countEdge() throw (SALOME::SALOME_Exception)
{
  return  _document_cpp->countEdge();
}

Edge_ptr Document_impl::getEdge(::CORBA::Long i) throw (SALOME::SALOME_Exception)
{
  Edge_ptr result = Edge::_nil();

  HEXA_NS::Edge* e = _document_cpp->getEdge(i);
  if ( e != NULL ){
    Edge_impl* servantCorba = new Edge_impl(e);
    result = servantCorba->_this();
  }
  return result;
}

Edge_ptr Document_impl::findEdge(Vertex_ptr p0In, Vertex_ptr p1In)
  throw (SALOME::SALOME_Exception)
{
  Edge_ptr result = Edge::_nil();

  Vertex_impl* p0InServant = ::DownCast<Vertex_impl*>( p0In );
  Vertex_impl* p1InServant = ::DownCast<Vertex_impl*>( p1In );
  ASSERT( p0InServant );
  ASSERT( p1InServant );

  if ( p0InServant &&  p1InServant ) {
    HEXA_NS::Vertex* p0 = p0InServant->GetImpl();
    HEXA_NS::Vertex* p1 = p1InServant->GetImpl();
    HEXA_NS::Edge* e = _document_cpp->findEdge( p0, p1 );

    if ( e != NULL ){
      Edge_impl* servantCorba = new Edge_impl(e);
      result = servantCorba->_this();
    }
  }
  return result;
}


Quad_ptr Document_impl::addQuad(Edge_ptr e0In, Edge_ptr e1In, Edge_ptr e2In, Edge_ptr e3In)
throw (SALOME::SALOME_Exception)
{
  Quad_ptr result = Quad::_nil();

  Edge_impl* e0InServant = ::DownCast<Edge_impl*>( e0In );
  Edge_impl* e1InServant = ::DownCast<Edge_impl*>( e1In );
  Edge_impl* e2InServant = ::DownCast<Edge_impl*>( e2In );
  Edge_impl* e3InServant = ::DownCast<Edge_impl*>( e3In );
  ASSERT( e0InServant );
  ASSERT( e1InServant );
  ASSERT( e2InServant );
  ASSERT( e3InServant );
  if ( e0InServant &&  e1InServant && e2InServant &&  e3InServant) {
    HEXA_NS::Edge* e0 = e0InServant->GetImpl();
    HEXA_NS::Edge* e1 = e1InServant->GetImpl();
    HEXA_NS::Edge* e2 = e2InServant->GetImpl();
    HEXA_NS::Edge* e3 = e3InServant->GetImpl();
    HEXA_NS::Quad* q = _document_cpp->addQuad( e0, e1, e2, e3 );
    if ( q !=  NULL ){
      Quad_impl* servantCorba = new Quad_impl(q);
      result = servantCorba->_this();
    }
  }
  return result;
}

Quad_ptr Document_impl::addQuadVertices( Vertex_ptr v0In, Vertex_ptr v1In,
                                 Vertex_ptr v2In, Vertex_ptr v3In )
  throw (SALOME::SALOME_Exception)
{
  Quad_ptr result = Quad::_nil();

  Vertex_impl* v0InServant = ::DownCast<Vertex_impl*>( v0In );
  Vertex_impl* v1InServant = ::DownCast<Vertex_impl*>( v1In );
  Vertex_impl* v2InServant = ::DownCast<Vertex_impl*>( v2In );
  Vertex_impl* v3InServant = ::DownCast<Vertex_impl*>( v3In );
  ASSERT( v0InServant );
  ASSERT( v1InServant );
  ASSERT( v2InServant );
  ASSERT( v3InServant );

  if ( v0InServant &&  v1InServant && v2InServant &&  v3InServant) {
    HEXA_NS::Vertex* v0 = v0InServant->GetImpl();
    HEXA_NS::Vertex* v1 = v1InServant->GetImpl();
    HEXA_NS::Vertex* v2 = v2InServant->GetImpl();
    HEXA_NS::Vertex* v3 = v3InServant->GetImpl();
    HEXA_NS::Quad* q = _document_cpp->addQuadVertices( v0, v1, v2, v3 );
    if ( q !=  NULL ){
      Quad_impl* servantCorba = new Quad_impl(q);
      result = servantCorba->_this();
    }
  }
  return result;
}

::CORBA::Long Document_impl::countQuad() throw (SALOME::SALOME_Exception)
{
  return  _document_cpp->countQuad();
}

Quad_ptr Document_impl::getQuad(::CORBA::Long i) throw (SALOME::SALOME_Exception)
{
  Quad_ptr result = Quad::_nil();

  HEXA_NS::Quad* q = _document_cpp->getQuad(i);
  if ( q != NULL ){
    Quad_impl* servantCorba = new Quad_impl(q);
    result = servantCorba->_this();
  }
  return result;
}

Quad_ptr Document_impl::findQuad(Vertex_ptr p0In, Vertex_ptr p1In) throw (SALOME::SALOME_Exception)
{
  Quad_ptr result = Quad::_nil();

  Vertex_impl* p0InServant = ::DownCast<Vertex_impl*>( p0In );
  Vertex_impl* p1InServant = ::DownCast<Vertex_impl*>( p1In );
  ASSERT( p0InServant );
  ASSERT( p1InServant );

  if ( p0InServant &&  p1InServant ) {
    HEXA_NS::Vertex* p0 = p0InServant->GetImpl();
    HEXA_NS::Vertex* p1 = p1InServant->GetImpl();

    HEXA_NS::Quad* q = _document_cpp->findQuad( p0, p1 );
    if ( q != NULL ){
      Quad_impl* servantCorba = new Quad_impl(q);
      result = servantCorba->_this();
    }
  }
  return result;
}


Hexa_ptr Document_impl::addHexa(Quad_ptr q0In, Quad_ptr q1In, Quad_ptr q2In, Quad_ptr q3In, Quad_ptr q4In, Quad_ptr q5In)
  throw (SALOME::SALOME_Exception)
{
  Hexa_ptr result = Hexa::_nil();

  Quad_impl* q0InServant = ::DownCast<Quad_impl*>( q0In );
  Quad_impl* q1InServant = ::DownCast<Quad_impl*>( q1In );
  Quad_impl* q2InServant = ::DownCast<Quad_impl*>( q2In );
  Quad_impl* q3InServant = ::DownCast<Quad_impl*>( q3In );
  Quad_impl* q4InServant = ::DownCast<Quad_impl*>( q4In );
  Quad_impl* q5InServant = ::DownCast<Quad_impl*>( q5In );

  ASSERT( q0InServant );
  ASSERT( q1InServant );
  ASSERT( q2InServant );
  ASSERT( q3InServant );
  ASSERT( q4InServant );
  ASSERT( q5InServant );

  if ( q0InServant &&  q1InServant && q2InServant &&  q3InServant
        && q4InServant &&  q5InServant ){
    HEXA_NS::Quad* q0 = q0InServant->GetImpl();
    HEXA_NS::Quad* q1 = q1InServant->GetImpl();
    HEXA_NS::Quad* q2 = q2InServant->GetImpl();
    HEXA_NS::Quad* q3 = q3InServant->GetImpl();
    HEXA_NS::Quad* q4 = q4InServant->GetImpl();
    HEXA_NS::Quad* q5 = q5InServant->GetImpl();
    HEXA_NS::Hexa* h = _document_cpp->addHexa( q0, q1, q2, q3, q4, q5 );
    if ( h != NULL ){
      Hexa_impl* servantCorba = new Hexa_impl(h);
      result = servantCorba->_this();
    }
  }
  return result;
}



Hexa_ptr Document_impl::addHexaVertices( Vertex_ptr v0In, Vertex_ptr v1In, Vertex_ptr v2In, Vertex_ptr v3In, Vertex_ptr v4In, Vertex_ptr v5In, Vertex_ptr v6In, Vertex_ptr v7In )
  throw (SALOME::SALOME_Exception)
{
  Hexa_ptr result = Hexa::_nil();

  Vertex_impl* v0InServant = ::DownCast<Vertex_impl*>( v0In );
  Vertex_impl* v1InServant = ::DownCast<Vertex_impl*>( v1In );
  Vertex_impl* v2InServant = ::DownCast<Vertex_impl*>( v2In );
  Vertex_impl* v3InServant = ::DownCast<Vertex_impl*>( v3In );
  Vertex_impl* v4InServant = ::DownCast<Vertex_impl*>( v4In );
  Vertex_impl* v5InServant = ::DownCast<Vertex_impl*>( v5In );
  Vertex_impl* v6InServant = ::DownCast<Vertex_impl*>( v6In );
  Vertex_impl* v7InServant = ::DownCast<Vertex_impl*>( v7In );
  ASSERT( v0InServant );
  ASSERT( v1InServant );
  ASSERT( v2InServant );
  ASSERT( v3InServant );
  ASSERT( v4InServant );
  ASSERT( v5InServant );
  ASSERT( v6InServant );
  ASSERT( v7InServant );

  if ( v0InServant &&  v1InServant && v2InServant &&  v3InServant
        && v4InServant &&  v5InServant && v6InServant &&  v7InServant ){
    HEXA_NS::Vertex* v0 = v0InServant->GetImpl();
    HEXA_NS::Vertex* v1 = v1InServant->GetImpl();
    HEXA_NS::Vertex* v2 = v2InServant->GetImpl();
    HEXA_NS::Vertex* v3 = v3InServant->GetImpl();
    HEXA_NS::Vertex* v4 = v4InServant->GetImpl();
    HEXA_NS::Vertex* v5 = v5InServant->GetImpl();
    HEXA_NS::Vertex* v6 = v6InServant->GetImpl();
    HEXA_NS::Vertex* v7 = v7InServant->GetImpl();

    HEXA_NS::Hexa* h = _document_cpp->addHexaVertices( v0, v1, v2, v3, v4, v5, v6, v7 );
    if ( h != NULL ){
      Hexa_impl* servantCorba = new Hexa_impl(h);
      result =  servantCorba->_this();
    }
  }
  return result;
}



::CORBA::Long Document_impl::countHexa() throw (SALOME::SALOME_Exception)
{
  return  _document_cpp->countHexa();
}

Hexa_ptr Document_impl::getHexa(::CORBA::Long i) throw (SALOME::SALOME_Exception)
{
  Hexa_ptr result = Hexa::_nil();

  HEXA_NS::Hexa* h = _document_cpp->getHexa(i);
  if ( h!= NULL ){
    Hexa_impl* servantCorba = new Hexa_impl(h);
    result = servantCorba->_this();
  }
  return result;
}


Hexa_ptr Document_impl::findHexa(Vertex_ptr p0In, Vertex_ptr p1In) throw (SALOME::SALOME_Exception)
{
  Hexa_ptr result = Hexa::_nil();

  Vertex_impl* v0InServant = ::DownCast<Vertex_impl*>( p0In );
  Vertex_impl* v1InServant = ::DownCast<Vertex_impl*>( p1In );
  ASSERT( v0InServant );
  ASSERT( v1InServant );

  if ( v0InServant &&  v1InServant ) {
    HEXA_NS::Vertex* v0 = v0InServant->GetImpl();
    HEXA_NS::Vertex* v1 = v1InServant->GetImpl();
    HEXA_NS::Hexa* h = _document_cpp->findHexa( v0, v1 );
    if ( h != NULL ){
      Hexa_impl* servantCorba = new Hexa_impl(h);
      result = servantCorba->_this();
    }
  }
  return result;
}

Vector_ptr Document_impl::addVector( ::CORBA::Double dx, ::CORBA::Double dy, ::CORBA::Double dz )
    throw(SALOME::SALOME_Exception)
{
  Vector_ptr result = Vector::_nil();
  HEXA_NS::Vector* v = _document_cpp->addVector( dx, dy, dz );

  if ( v != NULL ){
    Vector_impl* servantCorba = new Vector_impl(v);
    result = servantCorba->_this();
  }
  return result;
}

Vector_ptr Document_impl::addVectorVertices( Vertex_ptr v0In, Vertex_ptr v1In )
    throw(SALOME::SALOME_Exception)
{
  Vector_ptr result = Vector::_nil();

  Vertex_impl* v0InServant = ::DownCast<Vertex_impl*>( v0In );
  Vertex_impl* v1InServant = ::DownCast<Vertex_impl*>( v1In );
  ASSERT( v0InServant );
  ASSERT( v1InServant );

  if ( v0InServant &&  v1InServant ) {
    HEXA_NS::Vertex* v0 = v0InServant->GetImpl();
    HEXA_NS::Vertex* v1 = v1InServant->GetImpl();
    HEXA_NS::Vector* v = _document_cpp->addVectorVertices( v0, v1 );
    if ( v != NULL ){
      Vector_impl* servantCorba = new Vector_impl(v);
      result = servantCorba->_this();
    }
  }
  return result;
}


// ::CORBA::Long Document_impl::countVector() throw (SALOME::SALOME_Exception)
// {
//   return  _document_cpp->countVector();
// }
// 
// Vector_ptr Document_impl::getVector(::CORBA::Long i) throw (SALOME::SALOME_Exception)
// {
//   HEXA_NS::Vector* h = _document_cpp->getVector(i);
//   Vector_impl* servantCorba = new Vector_impl(h);
//   return servantCorba->_this();
// }


Cylinder_ptr Document_impl::addCylinder( Vertex_ptr baseIn, Vector_ptr directionIn,
      ::CORBA::Double radius, ::CORBA::Double height)
throw (SALOME::SALOME_Exception)
{
  Cylinder_ptr result = Cylinder::_nil();

  Vertex_impl* baseServant      = ::DownCast<Vertex_impl*>( baseIn );
  Vector_impl* directionServant = ::DownCast<Vector_impl*>( directionIn );
  ASSERT( baseServant );
  ASSERT( directionServant );

  if ( baseServant &&  directionServant ) {
    HEXA_NS::Vertex* base = baseServant->GetImpl();
    HEXA_NS::Vector* direction = directionServant->GetImpl();
    HEXA_NS::Cylinder* c = _document_cpp->addCylinder( base, direction, radius, height );
    if ( c!= NULL ){
      Cylinder_impl* servantCorba = new Cylinder_impl(c);
      result = servantCorba->_this();
    }
  }

  return result;
}

// ::CORBA::Long Document_impl::countCylinder() throw (SALOME::SALOME_Exception)
// {
//   return  _document_cpp->countCylinder();
// }
// 
// 
// Cylinder_ptr Document_impl::getCylinder(::CORBA::Long i) throw (SALOME::SALOME_Exception)
// {
//   HEXA_NS::Cylinder* c = _document_cpp->getCylinder(i);
//   Cylinder_impl* servantCorba = new Cylinder_impl(c);
//   return servantCorba->_this();
// }


Pipe_ptr Document_impl::addPipe( Vertex_ptr baseIn, Vector_ptr directionIn,
                  ::CORBA::Double int_radius, ::CORBA::Double ext_radius, ::CORBA::Double height)
throw (SALOME::SALOME_Exception)
{
  Pipe_ptr result = Pipe::_nil();

  Vertex_impl* baseServant      = ::DownCast<Vertex_impl*>( baseIn );
  Vector_impl* directionServant = ::DownCast<Vector_impl*>( directionIn );
  ASSERT( baseServant );
  ASSERT( directionServant );

  if ( baseServant &&  directionServant ) {
    HEXA_NS::Vertex* base = baseServant->GetImpl();
    HEXA_NS::Vector* direction = directionServant->GetImpl();
    HEXA_NS::Pipe* c = _document_cpp->addPipe( base, direction, int_radius, ext_radius, height );
    if ( c!= NULL ){
      Pipe_impl* servantCorba = new Pipe_impl(c);
      result = servantCorba->_this();
    }
  }

  return result;
}


// ::CORBA::Long Document_impl::countPipe() throw (SALOME::SALOME_Exception)
// {
//   return  _document_cpp->countPipe();
// }
// 
// Pipe_ptr Document_impl::getPipe(::CORBA::Long i) throw (SALOME::SALOME_Exception)
// {
//   HEXA_NS::Pipe* p = _document_cpp->getPipe(i);
//   Pipe_impl* servantCorba = new Pipe_impl(p);
//   return servantCorba->_this();
// }

::CORBA::Boolean Document_impl::removeHexa(Hexa_ptr hIn) throw (SALOME::SALOME_Exception)
{
  Hexa_impl* hServant = ::DownCast<Hexa_impl*>( hIn );
  ASSERT( hServant );

  if ( hServant ) {
    HEXA_NS::Hexa* h = hServant->GetImpl();
    int ok = _document_cpp->removeHexa(h);
    if ( ok == HOK )
      return true;
    else
      return false;
  }
}


::CORBA::Boolean Document_impl::removeConnectedHexa(Hexa_ptr hIn) throw (SALOME::SALOME_Exception)
{
  Hexa_impl* hServant = ::DownCast<Hexa_impl*>( hIn );
  ASSERT( hServant );

  if ( hServant ) {
    HEXA_NS::Hexa* h = hServant->GetImpl();
    int ok = _document_cpp->removeConnectedHexa(h);
    if ( ok == HOK )
      return true;
    else
      return false; 
  }
}


Elements_ptr Document_impl::makeCartesian( Vertex_ptr ptIn,
                            Vector_ptr vxIn, Vector_ptr vyIn, Vector_ptr vzIn,
                            ::CORBA::Long nx, ::CORBA::Long ny, ::CORBA::Long nz)
  throw (SALOME::SALOME_Exception)
{
  Elements_ptr result = Elements::_nil();

  Vertex_impl* ptInServant = ::DownCast<Vertex_impl*>( ptIn);
  Vector_impl* vxInServant = ::DownCast<Vector_impl*>( vxIn );
  Vector_impl* vyInServant = ::DownCast<Vector_impl*>( vyIn );
  Vector_impl* vzInServant = ::DownCast<Vector_impl*>( vzIn );
  ASSERT( ptInServant );
  ASSERT( vxInServant );
  ASSERT( vyInServant );
  ASSERT( vzInServant );

  if ( ptInServant &&  vxInServant &&  vyInServant && vzInServant ){
    HEXA_NS::Vertex* pt = ptInServant->GetImpl();
    HEXA_NS::Vector* vx = vxInServant->GetImpl();
    HEXA_NS::Vector* vy = vyInServant->GetImpl();
    HEXA_NS::Vector* vz = vzInServant->GetImpl();

    HEXA_NS::Elements* l = _document_cpp->makeCartesian(
        pt,
        vx, vy, vz,
        nx, ny, nz );
    if ( l != NULL ){
      Elements_impl* servantCorba = new Elements_impl(l);
      result = servantCorba->_this();
    }
  }
  return result;

}



Elements_ptr Document_impl::makeCartesian1(
    Vertex_ptr vxIn, 
    Vector_ptr vr1In,
    ::CORBA::Long px, ::CORBA::Long py, ::CORBA::Long pz,
    ::CORBA::Long mx, ::CORBA::Long my, ::CORBA::Long mz )
    throw(SALOME::SALOME_Exception)
{
  Elements_ptr result = Elements::_nil();

  Vertex_impl* vxInServant  = ::DownCast<Vertex_impl*>( vxIn );
  Vector_impl* vr1InServant = ::DownCast<Vector_impl*>( vr1In );
  ASSERT( vxInServant );
  ASSERT( vr1InServant );

  if ( vxInServant &&  vr1InServant ){
    HEXA_NS::Vertex* vx  = vxInServant->GetImpl();
    HEXA_NS::Vector* vr1 = vr1InServant->GetImpl();

    HEXA_NS::Elements* l = _document_cpp->makeCartesian(
        vx,
        vr1,
        px, py, pz,
        mx, my, mz );

    if ( l != NULL ){
      Elements_impl* servantCorba = new Elements_impl(l);
      result = servantCorba->_this();
    }
  }
  return result;
}


Elements_ptr Document_impl::makeCylindrical( Vertex_ptr ptIn,
                              Vector_ptr vexIn, Vector_ptr vezIn,
                              ::CORBA::Double dr, ::CORBA::Double da, ::CORBA::Double dl,
                              ::CORBA::Long nr,   ::CORBA::Long na, ::CORBA::Long nl,
                              ::CORBA::Boolean fill)
  throw (SALOME::SALOME_Exception)
{
  Elements_ptr result = Elements::_nil();

  Vertex_impl* ptServant = ::DownCast<Vertex_impl*>( ptIn );
  Vector_impl* vexServant = ::DownCast<Vector_impl*>( vexIn );
  Vector_impl* vezServant = ::DownCast<Vector_impl*>( vezIn );
  ASSERT( ptServant );
  ASSERT( vexServant );
  ASSERT( vezServant );

  if ( ptServant && vexServant && vezServant) {
    HEXA_NS::Vertex* pt  = ptServant->GetImpl();
    HEXA_NS::Vector* vex = vexServant->GetImpl();
    HEXA_NS::Vector* vez = vezServant->GetImpl();
    HEXA_NS::Elements* l = _document_cpp->makeCylindrical( pt,
                                          vex, vez,
                                          dr, da, dl, nr, na, nl, fill );
    if ( l != NULL ){
      Elements_impl* servantCorba = new Elements_impl(l);
      result = servantCorba->_this();
    }
  }
  return result;
}

// =================================================== makeCylindricals
Elements_ptr Document_impl::makeCylindricals (Vertex_ptr ptin, 
                           Vector_ptr vexin, Vector_ptr vezin,
        const RealVector& tdr, const RealVector& tda, const RealVector& tdl,
       	::CORBA::Boolean fill)
        throw (SALOME::SALOME_Exception)
{
  Elements_ptr result = Elements::_nil();

  Vertex_impl* ptServant  = ::DownCast<Vertex_impl*>( ptin );
  Vector_impl* vexServant = ::DownCast<Vector_impl*>( vexin );
  Vector_impl* vezServant = ::DownCast<Vector_impl*>( vezin );

  ASSERT( ptServant );
  ASSERT( vexServant );
  ASSERT( vezServant );

  if ( ptServant==NULL  || vexServant == NULL || vezServant== NULL )
     return result;

  std::vector <CORBA::Double> cdr, cda, cdl;
  for ( int nro = 0; nro < tdr.length(); nro++) 
      cdr.push_back (tdr[nro]);

  for ( int nro = 0; nro < tda.length(); nro++) 
      cda.push_back (tda[nro]);

  for ( int nro = 0; nro < tdl.length(); nro++) 
      cdl.push_back (tdl[nro]);

  HEXA_NS::Vertex*  pt  = ptServant->GetImpl();
  HEXA_NS::Vector*  vex = vexServant->GetImpl();
  HEXA_NS::Vector*  vez = vezServant->GetImpl();
  HEXA_NS::Elements* grid = _document_cpp->makeCylindricals ( pt, vex, vez, 
                                                      cdr, cda, cdl, fill );
  if ( grid != NULL )
     {
     Elements_impl* servantCorba = new Elements_impl (grid);
     result = servantCorba->_this();
     }

  return result;
}



// Elements_ptr Document_impl::makeSpherical( Vertex_ptr ptIn,
//     ::CORBA::Double dx, ::CORBA::Double dy, ::CORBA::Double dz,
//     ::CORBA::Long n)
//   throw (SALOME::SALOME_Exception)
// {
//   Vertex_impl* ptServant = ::DownCast<Vertex_impl*>( ptIn );
//   ASSERT( ptServant );
// 
//   if ( ptServant ) {
//     HEXA_NS::Vertex* pt = ptServant->GetImpl();
//     HEXA_NS::Elements* l = _document_cpp->makeSpherical( pt,  dx, dy, dz, n );
//     Elements_impl* servantCorba = new Elements_impl(l);
//     return servantCorba->_this();
//   }
// }

Elements_ptr Document_impl::makeSpherical(
    Vertex_ptr ptIn,
    ::CORBA::Double rayon,
    ::CORBA::Long n,
    ::CORBA::Double k )
  throw (SALOME::SALOME_Exception) //CS_TODO
{
  Elements_ptr result = Elements::_nil();

  Vertex_impl* ptServant = ::DownCast<Vertex_impl*>( ptIn );
  ASSERT( ptServant );

  if ( ptServant ) {
    HEXA_NS::Vertex* pt = ptServant->GetImpl();
    HEXA_NS::Elements* l = _document_cpp->makeSpherical( pt, rayon, n, k );
    if ( l != NULL ){
      Elements_impl* servantCorba = new Elements_impl(l);
      result = servantCorba->_this();
    }
  }
  return result;
}



  


// Elements_ptr Document_impl::makeCylinder(Cylinder_ptr cylIn, ::CORBA::Long nr, ::CORBA::Long na, ::CORBA::Long nl)
Elements_ptr Document_impl::makeCylinder(Cylinder_ptr cylIn, Vector_ptr vrIn, ::CORBA::Long nr, ::CORBA::Long na, ::CORBA::Long nl)
  throw (SALOME::SALOME_Exception)
{
  Elements_ptr result = Elements::_nil();

  Cylinder_impl* cylServant = ::DownCast<Cylinder_impl*>( cylIn );
  Vector_impl* vrServant = ::DownCast<Vector_impl*>( vrIn );
  ASSERT( cylServant );
  ASSERT( vrServant );

  if ( cylServant && vrServant ) {
    HEXA_NS::Cylinder* cyl  = cylServant->GetImpl();
    HEXA_NS::Vector* vr     = vrServant->GetImpl();
    HEXA_NS::Elements* l = _document_cpp->makeCylinder( cyl, vr,  nr, na, nl );
    if ( l != NULL ){
      Elements_impl* servantCorba = new Elements_impl(l);
      result = servantCorba->_this();
    }
  }
  return result;
}

Elements_ptr Document_impl::makePipe( Pipe_ptr pIn, Vector_ptr vrIn,  
                         ::CORBA::Long nr, ::CORBA::Long na, ::CORBA::Long nl)
   throw (SALOME::SALOME_Exception)
{
   Pipe_impl*   pServant  = ::DownCast<Pipe_impl*>( pIn );
   Vector_impl* vrServant = ::DownCast<Vector_impl*>( vrIn );
   ASSERT( pServant );
 
   if ( pServant ) {
     HEXA_NS::Pipe*   p   = pServant ->GetImpl();
     HEXA_NS::Vector* vr  = vrServant->GetImpl();
     HEXA_NS::Elements* l = _document_cpp->makePipe( p, vr, nr, na, nl );
     Elements_impl* servantCorba = new Elements_impl(l);
     return servantCorba->_this();
   }
}


CrossElements_ptr Document_impl::makeCylinders(Cylinder_ptr c1In, Cylinder_ptr c2In)
  throw (SALOME::SALOME_Exception)
{
  CrossElements_ptr result = CrossElements::_nil();

  Cylinder_impl* c1Servant = ::DownCast<Cylinder_impl*>( c1In );
  Cylinder_impl* c2Servant = ::DownCast<Cylinder_impl*>( c2In );
  ASSERT( c1Servant );
  ASSERT( c2Servant );

  if ( c1Servant && c2Servant ) {
    HEXA_NS::Cylinder* c1= c1Servant->GetImpl();
    HEXA_NS::Cylinder* c2= c2Servant->GetImpl();

    HEXA_NS::CrossElements* l = _document_cpp->makeCylinders(c1,c2);
    if (l != NULL ){
      CrossElements_impl* servantCorba = new CrossElements_impl(l);
      result = servantCorba->_this();
    }
  }
  return result;
}

Elements_ptr Document_impl::makePipes( Pipe_ptr p1In, Pipe_ptr p2In)
   throw (SALOME::SALOME_Exception)
{
   Pipe_impl* p1Servant = ::DownCast<Pipe_impl*>( p1In );
   Pipe_impl* p2Servant = ::DownCast<Pipe_impl*>( p2In );
   ASSERT( p1Servant );
   ASSERT( p2Servant );
 
   if ( p1Servant && p2Servant ) {
     HEXA_NS::Pipe* p1= p1Servant->GetImpl();
     HEXA_NS::Pipe* p2= p2Servant->GetImpl();
 
     HEXA_NS::Elements* l = _document_cpp->makePipes( p1, p2);
     Elements_impl* servantCorba = new Elements_impl(l);
     return servantCorba->_this();
   }
}



Elements_ptr Document_impl::prismQuad(Quad_ptr qdIn, Vector_ptr vIn, ::CORBA::Long nb) throw (SALOME::SALOME_Exception)
{
  Elements_ptr result = Elements::_nil();

  Quad_impl* qServant   = ::DownCast<Quad_impl*>( qdIn );
  Vector_impl* vServant = ::DownCast<Vector_impl*>( vIn );
  ASSERT( qServant );
  ASSERT( vServant);

  if ( qServant && vServant ) {
    HEXA_NS::Quad* qd = qServant->GetImpl();
    HEXA_NS::Vector* v = vServant->GetImpl();

    HEXA_NS::Elements* l = _document_cpp->prismQuad(qd, v, nb);
    if ( l!= NULL ){
      Elements_impl* servantCorba = new Elements_impl(l);
      result = servantCorba->_this();
    }
  }

  return result;
}


//CS_Bruno

// // loop on meshes
//       for ( int i = 0; i < theMeshesArray.length(); i++) {
//         SMESH::SMESH_Mesh_var anInitMesh = theMeshesArray[i];
//         if ( !anInitMesh->_is_nil() ) {
//           SMESH_Mesh_i* anInitImpl = dynamic_cast<SMESH_Mesh_i*>( GetServant( anInitMesh ).in() );
//           if ( anInitImpl ) {
//             ::SMESH_Mesh& aInitLocMesh = anInitImpl->GetImpl();

Elements_ptr Document_impl::prismQuads(const Quads& qdsIn, Vector_ptr vIn, ::CORBA::Long nb) throw (SALOME::SALOME_Exception)
{
  Elements_ptr result = Elements::_nil();

  HEXA_NS::Quads qds;
  for ( int i = 0; i < qdsIn.length(); i++) {
    Quad_impl* qServant = ::DownCast<Quad_impl*>( qdsIn[i] );
    ASSERT( qServant );
    HEXA_NS::Quad* qd = qServant->GetImpl();
    qds.push_back(qd);
  }

  Vector_impl* vServant = ::DownCast<Vector_impl*>( vIn );
  ASSERT( vServant);

  if ( vServant ) {
    HEXA_NS::Vector* v = vServant->GetImpl();
    HEXA_NS::Elements* l = _document_cpp->prismQuads(qds, v, nb);
    if ( l!= NULL ){
      Elements_impl* servantCorba = new Elements_impl(l);
      result = servantCorba->_this();
    }
  }

  return result;
}


Elements_ptr Document_impl::joinQuad(Quad_ptr qaIn, Quad_ptr qbIn, Vertex_ptr va1In, Vertex_ptr vb1In, Vertex_ptr va2In, Vertex_ptr vb2In, ::CORBA::Long nb) throw (SALOME::SALOME_Exception)
{
  Elements_ptr result = Elements::_nil();

  Quad_impl* qaServant = ::DownCast<Quad_impl*>( qaIn );
  Quad_impl* qbServant = ::DownCast<Quad_impl*>( qbIn );

  Vertex_impl* va1Servant = ::DownCast<Vertex_impl*>( va1In );
  Vertex_impl* vb1Servant = ::DownCast<Vertex_impl*>( vb1In );
  Vertex_impl* va2Servant = ::DownCast<Vertex_impl*>( va2In );
  Vertex_impl* vb2Servant = ::DownCast<Vertex_impl*>( vb2In );

  ASSERT( qaServant );
  ASSERT( qbServant );
  ASSERT( va1Servant );
  ASSERT( vb1Servant );
  ASSERT( va2Servant );
  ASSERT( vb2Servant );

  if ( qaServant && qbServant && va1Servant && vb1Servant && va2Servant && vb2Servant ){
    HEXA_NS::Quad* qa = qaServant->GetImpl();
    HEXA_NS::Quad* qb = qbServant->GetImpl();
    HEXA_NS::Vertex* va1 = va1Servant->GetImpl();
    HEXA_NS::Vertex* vb1 = vb1Servant->GetImpl();
    HEXA_NS::Vertex* va2 = va2Servant->GetImpl();
    HEXA_NS::Vertex* vb2 = vb2Servant->GetImpl();

    HEXA_NS::Elements* l = _document_cpp->joinQuad(qa, qb, va1, vb1, va2, vb2, nb);
    if ( l!= NULL ){
      Elements_impl* servantCorba = new Elements_impl(l);
      result = servantCorba->_this();
    }
  }

  return result;
}



Elements_ptr Document_impl::joinQuads(const Quads& qdsIn, Quad_ptr qbIn, Vertex_ptr va1In, Vertex_ptr vb1In, Vertex_ptr va2In, Vertex_ptr vb2In, ::CORBA::Long nb) throw (SALOME::SALOME_Exception)
{
  Elements_ptr result = Elements::_nil();

  Quad_impl*   qbServant  = ::DownCast<Quad_impl*>( qbIn );
  Vertex_impl* va1Servant = ::DownCast<Vertex_impl*>( va1In );
  Vertex_impl* vb1Servant = ::DownCast<Vertex_impl*>( vb1In );
  Vertex_impl* va2Servant = ::DownCast<Vertex_impl*>( va2In );
  Vertex_impl* vb2Servant = ::DownCast<Vertex_impl*>( vb2In );

  ASSERT( qbServant );
  ASSERT( va1Servant );
  ASSERT( vb1Servant );
  ASSERT( va2Servant );
  ASSERT( vb2Servant );

  if ( qbServant && va1Servant && vb1Servant && va2Servant && vb2Servant ) {
    HEXA_NS::Quad*    qb = qbServant->GetImpl();
    HEXA_NS::Vertex* va1 = va1Servant->GetImpl();
    HEXA_NS::Vertex* vb1 = vb1Servant->GetImpl();
    HEXA_NS::Vertex* va2 = va2Servant->GetImpl();
    HEXA_NS::Vertex* vb2 = vb2Servant->GetImpl();

//     MESSAGE("joinQuads  AAAAAAAAAAAAA");
    HEXA_NS::Quads qds;
    for ( int i = 0; i < qdsIn.length(); i++) {
      Quad_impl* qServant = ::DownCast<Quad_impl*>( qdsIn[i] );
      ASSERT( qServant );
      HEXA_NS::Quad* qd = qServant->GetImpl();
      qds.push_back(qd);
    }

    HEXA_NS::Elements* l = _document_cpp->joinQuads(qds, qb, va1, vb1, va2, vb2, nb);
    if ( l!= NULL ){
      Elements_impl* servantCorba = new Elements_impl(l);
      result = servantCorba->_this();
    }
  }

  return result;
}






::CORBA::Long Document_impl::mergeQuads(Quad_ptr qaIn, Quad_ptr qbIn, Vertex_ptr va1In, Vertex_ptr vb1In, Vertex_ptr va2In, Vertex_ptr vb2In) throw (SALOME::SALOME_Exception)
{
  Quad_impl* qaServant = ::DownCast<Quad_impl*>( qaIn );
  Quad_impl* qbServant = ::DownCast<Quad_impl*>( qbIn );

  Vertex_impl* va1Servant = ::DownCast<Vertex_impl*>( va1In );
  Vertex_impl* vb1Servant = ::DownCast<Vertex_impl*>( vb1In );
  Vertex_impl* va2Servant = ::DownCast<Vertex_impl*>( va2In );
  Vertex_impl* vb2Servant = ::DownCast<Vertex_impl*>( vb2In );

  ASSERT( qaServant );
  ASSERT( qbServant );
  //   ASSERT( va1Servant ); Controle supprime Abu 
  //   ASSERT( vb1Servant );
  //   ASSERT( va2Servant );
  //   ASSERT( vb2Servant );

  if ( qaServant && qbServant)
     {
     HEXA_NS::Vertex* va1 = NULL;
     HEXA_NS::Vertex* vb1 = NULL;
     HEXA_NS::Vertex* va2 = NULL;
     HEXA_NS::Vertex* vb2 = NULL;

     if (va1Servant && vb1Servant && va2Servant && vb2Servant )
        {
        va1 = va1Servant->GetImpl();
        vb1 = vb1Servant->GetImpl();
        va2 = va2Servant->GetImpl();
        vb2 = vb2Servant->GetImpl();
        }

    HEXA_NS::Quad* qa = qaServant->GetImpl();
    HEXA_NS::Quad* qb = qbServant->GetImpl();

    return _document_cpp->mergeQuads(qa, qb, va1, vb1, va2, vb2);
//     HEXA_NS::Elements* l = _document_cpp->mergeQuads(qa, qb, va1, vb1, va2, vb2);
//     Elements_impl* servantCorba = new Elements_impl(l);
//     return servantCorba->_this();
  }
}



::CORBA::Long Document_impl::mergeEdges(Edge_ptr e1In, Edge_ptr e2In, Vertex_ptr v1In, Vertex_ptr v2In) throw (SALOME::SALOME_Exception)
{
  Edge_impl* e1Servant = ::DownCast<Edge_impl*>( e1In );
  Edge_impl* e2Servant = ::DownCast<Edge_impl*>( e2In );

  Vertex_impl* v1Servant = ::DownCast<Vertex_impl*>( v1In );
  Vertex_impl* v2Servant = ::DownCast<Vertex_impl*>( v2In );

  ASSERT( e1Servant );
  ASSERT( e2Servant );
  ASSERT( v1Servant );
  ASSERT( v2Servant );

  if ( e1Servant && e2Servant && v1Servant && v2Servant ){
    HEXA_NS::Edge* e1 = e1Servant->GetImpl();
    HEXA_NS::Edge* e2 = e2Servant->GetImpl();

    HEXA_NS::Vertex* v1 = v1Servant->GetImpl();
    HEXA_NS::Vertex* v2 = v2Servant->GetImpl();

//     HEXA_NS::Elements* l = _document_cpp->mergeEdges(e1, e2, v1, v2);
//     Elements_impl* servantCorba = new Elements_impl(l);
//     return servantCorba->_this();

    return _document_cpp->mergeEdges(e1, e2, v1, v2);
  }
}

::CORBA::Long Document_impl::mergeVertices(Vertex_ptr v1In, Vertex_ptr v2In) throw (SALOME::SALOME_Exception)
{
  Vertex_impl* v1Servant = ::DownCast<Vertex_impl*>( v1In );
  Vertex_impl* v2Servant = ::DownCast<Vertex_impl*>( v2In );

  ASSERT( v1Servant );
  ASSERT( v2Servant );

  if ( v1Servant && v2Servant ){
    HEXA_NS::Vertex* v1 = v1Servant->GetImpl();
    HEXA_NS::Vertex* v2 = v2Servant->GetImpl();

//     HEXA_NS::Elements* l = _document_cpp->mergeVertices(v1, v2);
//     Elements_impl* servantCorba = new Elements_impl(l);
//     return servantCorba->_this();
    return _document_cpp->mergeVertices(v1, v2);
  }
}


Elements_ptr Document_impl::disconnectQuad(Hexa_ptr hexIn, Quad_ptr quadIn) 
             throw (SALOME::SALOME_Exception)
{
  Elements_ptr result = Elements::_nil();
//   Quad_ptr result = Quad::_nil();
  Hexa_impl* hServant = ::DownCast<Hexa_impl*>( hexIn );
  Quad_impl* qServant = ::DownCast<Quad_impl*>( quadIn );

  ASSERT( hServant );
  ASSERT( qServant );

  if ( hServant && qServant ){
    HEXA_NS::Hexa* h = hServant->GetImpl();
    HEXA_NS::Quad* q = qServant->GetImpl();

    HEXA_NS::Elements* elements = _document_cpp->disconnectQuad(h, q);
    if (elements != NULL ){
       Elements_impl* servantCorba = new Elements_impl(elements);
       result = servantCorba->_this();
    }
  }
  return result;
}

Elements_ptr Document_impl::disconnectEdge(Hexa_ptr hexIn, Edge_ptr edgeIn) 
             throw (SALOME::SALOME_Exception)
{
  Elements_ptr result = Elements::_nil();

  Hexa_impl* hServant = ::DownCast<Hexa_impl*>( hexIn );
  Edge_impl* eServant = ::DownCast<Edge_impl*>( edgeIn );

  ASSERT( hServant );
  ASSERT( eServant );

  if ( hServant && eServant ){
    HEXA_NS::Hexa* h = hServant->GetImpl();
    HEXA_NS::Edge* e = eServant->GetImpl();

    HEXA_NS::Elements* r = _document_cpp->disconnectEdge(h, e);
    if ( r!= NULL ){
      Elements_impl* servantCorba = new Elements_impl(r);
      result = servantCorba->_this();
    }
  }
  return result;
}

Elements_ptr Document_impl::disconnectVertex(Hexa_ptr hexIn, Vertex_ptr vxIn) 
             throw (SALOME::SALOME_Exception)
{
  Elements_ptr result = Elements::_nil();

  Hexa_impl* hServant   = ::DownCast<Hexa_impl*>( hexIn );
  Vertex_impl* vServant = ::DownCast<Vertex_impl*>( vxIn );

  ASSERT( hServant );
  ASSERT( vServant );

  if ( hServant && vServant ){
    HEXA_NS::Hexa* h = hServant->GetImpl();
    HEXA_NS::Vertex* v = vServant->GetImpl();

    HEXA_NS::Elements* r = _document_cpp->disconnectVertex(h, v);
    if ( r!= NULL ){
      Elements_impl* servantCorba = new Elements_impl(r);
      result = servantCorba->_this();
    }
  }

  return result;
}
// ====================================================== disconnectEdges
Elements_ptr Document_impl::disconnectEdges (const Hexas& hexas_in, 
                                             const Edges& edges_in) 
             throw (SALOME::SALOME_Exception)
{
   Elements_ptr result = Elements::_nil();

   std::vector <HEXA_NS::Hexa*> tab_hexas;
   for (int i = 0; i < hexas_in.length(); i++) 
       {
       Hexa_impl* im_hexa = ::DownCast<Hexa_impl*> ( hexas_in[i] );
       ASSERT( im_hexa );
       HEXA_NS::Hexa* un_hexa = im_hexa->GetImpl();
       tab_hexas.push_back (un_hexa);
       }

   std::vector <HEXA_NS::Edge*> tab_edges;
   for (int i = 0; i < edges_in.length(); i++) 
       {
       Edge_impl* im_edge = ::DownCast<Edge_impl*> ( edges_in[i] );
       ASSERT( im_edge );
       HEXA_NS::Edge* un_edge = im_edge->GetImpl();
       tab_edges.push_back (un_edge);
       }

   HEXA_NS::Elements* r = _document_cpp->disconnectEdges (tab_hexas, tab_edges);

   if ( r!= NULL )
      {
      Elements_impl* servantCorba = new Elements_impl (r);
      result = servantCorba->_this();
      }

  return result;
}
// ====================================================== cut
Elements_ptr Document_impl::cut(Edge_ptr eIn, ::CORBA::Long nb_of_cuts) 
                            throw (SALOME::SALOME_Exception)
{
  Elements_ptr result = Elements::_nil();

  Edge_impl* eServant = ::DownCast<Edge_impl*>( eIn );
  ASSERT( eServant );

  if ( eServant ){
    HEXA_NS::Edge* e = eServant->GetImpl();
    HEXA_NS::Elements* r = _document_cpp->cut(e, nb_of_cuts);
    if ( r!= NULL ){
      Elements_impl* servantCorba = new Elements_impl(r);
      result = servantCorba->_this();
    }
  }

  return result;
}


Elements_ptr Document_impl::makeTranslation(Elements_ptr lIn, Vector_ptr vecIn) throw (SALOME::SALOME_Exception)
{
  Elements_ptr result = Elements::_nil();

  Elements_impl* lServant = ::DownCast<Elements_impl*>( lIn );
  Vector_impl* vecServant = ::DownCast<Vector_impl*>( vecIn );
  ASSERT( lServant );
  ASSERT( vecServant );

  if ( lServant && vecServant ){
    HEXA_NS::Elements* l = lServant->GetImpl();
    HEXA_NS::Vector* vec = vecServant->GetImpl();

    HEXA_NS::Elements* r = _document_cpp->makeTranslation(l, vec);
    if ( r != NULL ){
      Elements_impl* servantCorba = new Elements_impl(r);
      result = servantCorba->_this();
    }
  }
  return result;
}


Elements_ptr Document_impl::makeRotation(Elements_ptr lIn, Vertex_ptr pIn, Vector_ptr vecIn, ::CORBA::Double angle) throw (SALOME::SALOME_Exception)
{
  Elements_ptr result = Elements::_nil();

  Elements_impl* lServant = ::DownCast<Elements_impl*>( lIn );
  Vertex_impl*   pServant = ::DownCast<Vertex_impl*>( pIn );
  Vector_impl* vecServant = ::DownCast<Vector_impl*>( vecIn );

  ASSERT( lServant );
  ASSERT( pServant );
  ASSERT( vecServant );

  if ( lServant && pServant && vecServant ){
    HEXA_NS::Elements* l = lServant->GetImpl();
    HEXA_NS::Vertex*   p = pServant->GetImpl();
    HEXA_NS::Vector* vec = vecServant->GetImpl();

    HEXA_NS::Elements* r = _document_cpp->makeRotation(l, p, vec, angle);
    if ( r != NULL ){
      Elements_impl* servantCorba = new Elements_impl(r);
      result = servantCorba->_this();
    }
  }
  return result;
}
void Document_impl::performTranslation(Elements_ptr lIn, Vector_ptr vecIn) throw (SALOME::SALOME_Exception)
{
  Elements_impl* lServant  = ::DownCast<Elements_impl*>( lIn );
  Vector_impl* vecServant = ::DownCast<Vector_impl*>( vecIn );

  ASSERT( lServant );
  ASSERT( vecServant );

  if ( lServant && vecServant ){
    HEXA_NS::Elements* l = lServant->GetImpl();
    HEXA_NS::Vector* vec = vecServant->GetImpl();
    _document_cpp->performTranslation(l, vec);
  }
}


void Document_impl::performRotation(Elements_ptr lIn, Vertex_ptr pIn, Vector_ptr vecIn, ::CORBA::Double angle) throw (SALOME::SALOME_Exception)
{
  Elements_impl* lServant = ::DownCast<Elements_impl*>( lIn );
  Vertex_impl*   pServant = ::DownCast<Vertex_impl*>( pIn );
  Vector_impl* vecServant = ::DownCast<Vector_impl*>( vecIn );

  ASSERT( lServant );
  ASSERT( pServant );
  ASSERT( vecServant );

  if ( lServant && pServant && vecServant ){
    HEXA_NS::Elements* l = lServant->GetImpl();
    HEXA_NS::Vertex*   p = pServant->GetImpl();
    HEXA_NS::Vector* vec = vecServant->GetImpl();
    _document_cpp->performRotation(l, p, vec, angle);
  }
}

Group_ptr Document_impl::addHexaGroup(const char* name) throw (SALOME::SALOME_Exception)
{
  Group_ptr result = Group::_nil();
  HEXA_NS::Group* g = _document_cpp->addGroup( name, HEXA_NS::HexaCell );
  if ( g != NULL ){
    Group_impl* servantCorba = new Group_impl(g);
    result = servantCorba->_this();
  }
  return result;
}


Group_ptr Document_impl::addQuadGroup(const char* name) throw (SALOME::SALOME_Exception)
{
  Group_ptr result = Group::_nil();
  HEXA_NS::Group* g = _document_cpp->addGroup( name, HEXA_NS::QuadCell );
  if ( g != NULL ){
    Group_impl* servantCorba = new Group_impl(g);
    result = servantCorba->_this();
  }
  return result;
}

Group_ptr Document_impl::addEdgeGroup(const char* name) throw (SALOME::SALOME_Exception)
{
  Group_ptr result = Group::_nil();
  HEXA_NS::Group* g = _document_cpp->addGroup( name, HEXA_NS::EdgeCell );
  if ( g != NULL ){
    Group_impl* servantCorba = new Group_impl(g);
    result = servantCorba->_this();
  }
  return result;
}


Group_ptr Document_impl::addHexaNodeGroup(const char* name) throw (SALOME::SALOME_Exception)
{
  Group_ptr result = Group::_nil();
  HEXA_NS::Group* g = _document_cpp->addGroup( name, HEXA_NS::HexaNode );
  if ( g != NULL ){
    Group_impl* servantCorba = new Group_impl(g);
    result = servantCorba->_this();
  }
  return result;
}

Group_ptr Document_impl::addQuadNodeGroup(const char* name) throw (SALOME::SALOME_Exception)
{
  Group_ptr result = Group::_nil();
  HEXA_NS::Group* g = _document_cpp->addGroup( name, HEXA_NS::QuadNode );
  if ( g != NULL ){
    Group_impl* servantCorba = new Group_impl(g);
    result = servantCorba->_this();
  }
  return result;
}


Group_ptr Document_impl::addEdgeNodeGroup(const char* name) throw (SALOME::SALOME_Exception)
{
  Group_ptr result = Group::_nil();
  HEXA_NS::Group* g = _document_cpp->addGroup( name, HEXA_NS::EdgeNode );
  if ( g != NULL ){
    Group_impl* servantCorba = new Group_impl(g);
    result = servantCorba->_this();
  }
  return result;
}


Group_ptr Document_impl::addVertexNodeGroup(const char* name) throw (SALOME::SALOME_Exception)
{
  Group_ptr result = Group::_nil();
  HEXA_NS::Group* g = _document_cpp->addGroup( name, HEXA_NS::VertexNode );
  if ( g != NULL ){
    Group_impl* servantCorba = new Group_impl(g);
    result = servantCorba->_this();
  }
  return result;
}


::CORBA::Long Document_impl::removeGroup(Group_ptr gIn) throw (SALOME::SALOME_Exception)
{
  Group_impl* gInServant = ::DownCast<Group_impl*>( gIn );
  ASSERT( gInServant );

  if ( gInServant ) {
    HEXA_NS::Group* g = gInServant->GetImpl();
    int ok = _document_cpp->removeGroup(g);
    return ok;
  }
}

::CORBA::Long Document_impl::countGroup() throw (SALOME::SALOME_Exception)
{
  return _document_cpp->countGroup();
}


Group_ptr Document_impl::getGroup(::CORBA::Long i) throw (SALOME::SALOME_Exception)
{
  Group_ptr result  = Group::_nil();
  HEXA_NS::Group* g = _document_cpp->getGroup(i);
  if ( g != NULL ){ //CS_TODO 
    Group_impl* servantCorba = new Group_impl(g);
    result = servantCorba->_this();
  }
  return result;
}


Group_ptr Document_impl::findGroup(const char* name) throw (SALOME::SALOME_Exception)
{
  Group_ptr result  = Group::_nil();
  HEXA_NS::Group* g = _document_cpp->findGroup(name);
  if ( g != NULL ){ //CS_TODO 
    Group_impl* servantCorba = new Group_impl(g);
    result = servantCorba->_this();
  }
  return result;
}


Law_ptr Document_impl::addLaw(const char* name, ::CORBA::Long nb_nodes) throw (SALOME::SALOME_Exception)
{
  Law_ptr result  = Law::_nil();

  HEXA_NS::Law* l = _document_cpp->addLaw(name, nb_nodes);
  if ( l != NULL ){
    Law_impl* servantCorba = new Law_impl(l);
    result = servantCorba->_this(); 
  }

  return result;
}


::CORBA::Long Document_impl::countLaw() throw (SALOME::SALOME_Exception)
{
  return _document_cpp->countLaw();
}

Law_ptr Document_impl::getLaw(::CORBA::Long i) throw (SALOME::SALOME_Exception)
{
  Law_ptr result = Law::_nil();

  HEXA_NS::Law* l = _document_cpp->getLaw(i);
  if ( l != NULL ){
    Law_impl* servantCorba = new Law_impl(l);
    result = servantCorba->_this();
  }
  return result;
}


void Document_impl::removeLaw(Law_ptr l) throw (SALOME::SALOME_Exception)
{
  Law_impl* lServant = ::DownCast<Law_impl*>(l);
  ASSERT( lServant );
  if ( lServant ) {
    HEXA_NS::Law* l = lServant->GetImpl();
    _document_cpp->removeLaw(l);
  }
}



Law_ptr Document_impl::findLaw(const char* name) throw (SALOME::SALOME_Exception)
{
  Law_ptr result  = Law::_nil();

  HEXA_NS::Law* l = _document_cpp->findLaw(name);
  if ( l != NULL ){
    Law_impl* servantCorba = new Law_impl(l);
    result = servantCorba->_this();
  }

  return result;
}


::CORBA::Long Document_impl::countPropagation() throw (SALOME::SALOME_Exception)
{
  return _document_cpp->countPropagation();
}


Propagation_ptr Document_impl::getPropagation(::CORBA::Long i) throw (SALOME::SALOME_Exception)
{
  Propagation_ptr result  = Propagation::_nil();

  HEXA_NS::Propagation* p = _document_cpp->getPropagation(i);
//   //CS_Test
// 
//   const HEXA_NS::Edges& edges_cpp = p->getEdges();
// 
// //   HEXA_NS::Edges::const_iterator itertest = edges_cpp.begin();
// //   itertest != edges_cpp.end();
//   for ( HEXA_NS::Edges::const_iterator iter = edges_cpp.begin();
// 	iter != edges_cpp.end();
//         ++iter ){
//     *iter;
//   }
//   //CS_Test

  if ( p != NULL ){
    Propagation_impl* servantCorba = new Propagation_impl(p);
    result = servantCorba->_this();
  }
  return result;
}

Propagation_ptr Document_impl::findPropagation(Edge_ptr eIn) throw (SALOME::SALOME_Exception)
{
  Propagation_ptr result  = Propagation::_nil();

  Edge_impl* eServant = ::DownCast<Edge_impl*>( eIn );
  ASSERT( eServant );

  if ( eServant ) {
    HEXA_NS::Edge* e= eServant->GetImpl();

    HEXA_NS::Propagation* p = _document_cpp->findPropagation(e);
    if ( p != NULL ){
      Propagation_impl* servantCorba = new Propagation_impl(p);
      result = servantCorba->_this();
    }
  }

  return result;
}
// ======================================================= associateOpenedLine
::CORBA::Long Document_impl::associateOpenedLine (Edge_ptr   mstart, 
                            const Edges&          mline, 
                            GEOM::GEOM_Object_ptr gstart, 
                            ::CORBA::Double       pstart, 
                            const Shapes&         gline, 
                            ::CORBA::Double       pend) 
                      throw (SALOME::SALOME_Exception)
{
   Edge_impl* im_start = ::DownCast<Edge_impl*>( mstart );

   ASSERT ( im_start );
   if (im_start == NULL)
      return HERR;

   std::vector <HEXA_NS::Edge*> md_line;
   for (int i = 0; i < mline.length(); i++) 
       {
       Edge_impl* im_edge = ::DownCast<Edge_impl*> ( mline[i] );
       ASSERT( im_edge );
       HEXA_NS::Edge* un_edge = im_edge->GetImpl();
       md_line.push_back (un_edge);
       }

   TopoDS_Shape shape = HEXABLOCK_Gen_i::GetHEXABLOCKGen()->geomObjectToShape(gstart);
   string       b_rep = shape2string( shape );
   HEXA_NS::Shape* gg_start = new HEXA_NS::Shape( b_rep );

   std::vector <HEXA_NS::Shape*> gg_line;
   for (int i = 0; i < gline.length(); i++) 
       {
       shape = HEXABLOCK_Gen_i::GetHEXABLOCKGen()->geomObjectToShape(gline[i]);
       b_rep = shape2string( shape );
       HEXA_NS::Shape* gg_edge = new HEXA_NS::Shape( b_rep );
       gg_line.push_back (gg_edge);
       }

                // Call model

  HEXA_NS::Edge* md_start = im_start->GetImpl();

  ::CORBA::Long ier = _document_cpp->associateOpenedLine (md_start, md_line, 
                                          gg_start, pstart, gg_line, pend);
  return ier;
}

// ======================================================= associateClosedLine
::CORBA::Long Document_impl::associateClosedLine (Vertex_ptr mfirst, 
                            Edge_ptr                mstart, 
                            const Edges&            mline, 
                            GEOM::GEOM_Object_ptr   gstart, 
                            ::CORBA::Double         pstart, 
                            ::CORBA::Boolean        inv, 
                            const Shapes&           gline)
                      throw (SALOME::SALOME_Exception)
{
   std::vector <HEXA_NS::Edge*> md_line;
   for (int i = 0; i < mline.length(); i++) 
       {
       Edge_impl* im_edge = ::DownCast<Edge_impl*> ( mline[i] );
       ASSERT( im_edge );
       HEXA_NS::Edge* un_edge = im_edge->GetImpl();
       md_line.push_back (un_edge);
       }

   TopoDS_Shape shape = HEXABLOCK_Gen_i::GetHEXABLOCKGen()->geomObjectToShape(gstart);
   string       b_rep = shape2string( shape );
   HEXA_NS::Shape* gg_start = new HEXA_NS::Shape( b_rep );

   std::vector <HEXA_NS::Shape*> gg_line;
   for (int i = 0; i < gline.length(); i++) 
       {
       shape = HEXABLOCK_Gen_i::GetHEXABLOCKGen()->geomObjectToShape(gline[i]);
       b_rep = shape2string( shape );
       HEXA_NS::Shape* gg_edge = new HEXA_NS::Shape( b_rep );
       gg_line.push_back (gg_edge);
       }

                // Call model

  Edge_impl*   im_start = ::DownCast<Edge_impl*>  ( mstart );
  Vertex_impl* im_first = ::DownCast<Vertex_impl*>( mfirst );

  HEXA_NS::Vertex* md_first = im_first->GetImpl();
  HEXA_NS::Edge*   md_start = im_start->GetImpl();

  printf (" +++ HexDocument_impl.cxx : Appel de associateClosedLine \n");

  ::CORBA::Long ier = _document_cpp->associateClosedLine (md_first, md_start, 
                                     md_line, gg_start, pstart, inv, gg_line);
  HexDisplay (ier);
  return ier;
}
// ---------------------------------------------- Ajouts Abu Sept 2011
// ===================================================== setShape
void Document_impl::setShape (GEOM::GEOM_Object_ptr geom_object)
  throw(SALOME::SALOME_Exception)
{
  TopoDS_Shape shape = HEXABLOCK_Gen_i::GetHEXABLOCKGen()
                       ->geomObjectToShape(geom_object);
  CORBA::String_var anIOR = HEXABLOCK_Gen_i::GetORB()->object_to_string( geom_object);

  string strBrep = shape2string( shape );
  std::cout << "setShape ---------> len(strBrep) = "
            << strBrep.size() << std::endl;
  HEXA_NS::Shape* s = new HEXA_NS::Shape( strBrep );

  s->ior   = anIOR.in(); 
  s->ident = geom_object->GetStudyEntry();

  std::cout << " ............  Shape creee" << std::endl;
  _document_cpp->setShape (s);
  std::cout << " ............  Shape associee" << std::endl;
}
// ===================================================== getShape
GEOM::GEOM_Object_ptr Document_impl::getShape ()
  throw (SALOME::SALOME_Exception)
{
  HEXA_NS::Shape* s = _document_cpp->getShape ();
  CORBA::Object_var corbaObj;
  GEOM::GEOM_Object_var geomObj; // = new GEOM::GEOM_Object;

  if (s != NULL)
     {
     if ( !s->ior.empty() )
        { // geom object from current session
        corbaObj = HEXABLOCK_Gen_i::GetORB()->string_to_object( s->ior.c_str() );
        if ( !CORBA::is_nil( corbaObj ) )
           {
           geomObj = GEOM::GEOM_Object::_narrow( corbaObj );
           }
        } 
      else  // no geom object => we have to built it
        {
        geomObj = HEXABLOCK_Gen_i::GetHEXABLOCKGen()->brepToGeomObject( s->getBrep() );
        }
      }
  return geomObj._retn();
}


/* ***********************************************************
  GEOM::GEOM_Object_var result; // = new GEOM::GEOM_Object;

  HEXA_NS::Shape* s = _document_cpp->getShape();

  std::cout << "getShape ->" << s << std::endl;

  if (s != NULL)
     {
     string strBrep = s->getBrep();
     std::cout << "getShape -->len (getBrep) = " 
               << strBrep.size() << std::endl;
     TopoDS_Shape shape = string2shape( strBrep );
     std::cout << "getShape -->string2shape->" << std::endl;
     result = HEXABLOCK_Gen_i::GetHEXABLOCKGen()->shapeToGeomObject(shape);
     std::cout << "getShape -->GetHEXABLOCKGen--" << std::endl;
     }

  return result._retn();
  **************************************************************** */
// ===================================================== getBrep
char* Document_impl::getBrep () throw (SALOME::SALOME_Exception)
{
   const char* brep = NULL;

   HEXA_NS::Shape* shape = _document_cpp->getShape();
   if (shape != NULL)
       {    
       string b_rep = shape->getBrep();
       brep = b_rep.c_str();
       // brep = shape->getBrep().c_str();
       }    
     
  if (brep == NULL)
      brep = "";
  return CORBA::string_dup (brep);
}
// ===================================================== getName
char* Document_impl::getName () throw (SALOME::SALOME_Exception)
{
  const char* name = _document_cpp->getName();
  return CORBA::string_dup (name);
}
// ===================================================== countUsedVertex
::CORBA::Long Document_impl::countUsedVertex() 
         throw (SALOME::SALOME_Exception)
{
  return  _document_cpp->countUsedVertex();
}
// ===================================================== getUsedVertex
Vertex_ptr Document_impl::getUsedVertex(::CORBA::Long i) 
        throw (SALOME::SALOME_Exception)
{
  Vertex_ptr result   = Vertex::_nil();

  HEXA_NS::Vertex* v = _document_cpp->getUsedVertex(i);
  if ( v != NULL )
     {
     Vertex_impl* servantCorba = new Vertex_impl(v);
     result = servantCorba->_this();
     }
  return result;
}
// ===================================================== countUsedEdge
::CORBA::Long Document_impl::countUsedEdge() 
         throw (SALOME::SALOME_Exception)
{
  return  _document_cpp->countUsedEdge();
}
// ===================================================== getUsedEdge
Edge_ptr Document_impl::getUsedEdge(::CORBA::Long i) 
        throw (SALOME::SALOME_Exception)
{
  Edge_ptr result   = Edge::_nil();

  HEXA_NS::Edge* v = _document_cpp->getUsedEdge(i);
  if ( v != NULL )
     {
     Edge_impl* servantCorba = new Edge_impl(v);
     result = servantCorba->_this();
     }
  return result;
}
// ===================================================== countUsedQuad
::CORBA::Long Document_impl::countUsedQuad() 
         throw (SALOME::SALOME_Exception)
{
  return  _document_cpp->countUsedQuad();
}
// ===================================================== getUsedQuad
Quad_ptr Document_impl::getUsedQuad(::CORBA::Long i) 
        throw (SALOME::SALOME_Exception)
{
  Quad_ptr result   = Quad::_nil();

  HEXA_NS::Quad* v = _document_cpp->getUsedQuad(i);
  if ( v != NULL )
     {
     Quad_impl* servantCorba = new Quad_impl(v);
     result = servantCorba->_this();
     }
  return result;
}
// ===================================================== countUsedHexa
::CORBA::Long Document_impl::countUsedHexa() 
         throw (SALOME::SALOME_Exception)
{
  return  _document_cpp->countUsedHexa();
}
// ===================================================== getUsedHexa
Hexa_ptr Document_impl::getUsedHexa(::CORBA::Long i) 
        throw (SALOME::SALOME_Exception)
{
  Hexa_ptr result   = Hexa::_nil();

  HEXA_NS::Hexa* v = _document_cpp->getUsedHexa(i);
  if ( v != NULL )
     {
     Hexa_impl* servantCorba = new Hexa_impl(v);
     result = servantCorba->_this();
     }
  return result;
}
// ===================================================== addHexa5Quads
Hexa_ptr Document_impl::addHexa5Quads (Quad_ptr q1, Quad_ptr q2, Quad_ptr q3, 
                                       Quad_ptr q4, Quad_ptr q5)
                        throw (SALOME::SALOME_Exception)
{
  Hexa_ptr result = Hexa::_nil();

  Quad_impl* q1InServant = ::DownCast<Quad_impl*>( q1 );
  Quad_impl* q2InServant = ::DownCast<Quad_impl*>( q2 );
  Quad_impl* q3InServant = ::DownCast<Quad_impl*>( q3 );
  Quad_impl* q4InServant = ::DownCast<Quad_impl*>( q4 );
  Quad_impl* q5InServant = ::DownCast<Quad_impl*>( q5 );

  ASSERT( q1InServant );
  ASSERT( q2InServant );
  ASSERT( q3InServant );
  ASSERT( q4InServant );
  ASSERT( q5InServant );

  if (q1InServant && q2InServant && q3InServant && q4InServant && q5InServant)
     {
     HEXA_NS::Quad* q1 = q1InServant->GetImpl();
     HEXA_NS::Quad* q2 = q2InServant->GetImpl();
     HEXA_NS::Quad* q3 = q3InServant->GetImpl();
     HEXA_NS::Quad* q4 = q4InServant->GetImpl();
     HEXA_NS::Quad* q5 = q5InServant->GetImpl();
     HEXA_NS::Hexa* h = _document_cpp->addHexa5Quads (q1, q2, q3, q4, q5);

     if ( h != NULL )
        {
        Hexa_impl* servantCorba = new Hexa_impl(h);
        result = servantCorba->_this();
        }
     }
  return result;
}
// ===================================================== addHexa4Quads
Hexa_ptr Document_impl::addHexa4Quads (Quad_ptr q1, Quad_ptr q2, Quad_ptr q3, 
                                       Quad_ptr q4)
                        throw (SALOME::SALOME_Exception)
{
  Hexa_ptr result = Hexa::_nil();

  Quad_impl* q1InServant = ::DownCast<Quad_impl*>( q1 );
  Quad_impl* q2InServant = ::DownCast<Quad_impl*>( q2 );
  Quad_impl* q3InServant = ::DownCast<Quad_impl*>( q3 );
  Quad_impl* q4InServant = ::DownCast<Quad_impl*>( q4 );

  ASSERT( q1InServant );
  ASSERT( q2InServant );
  ASSERT( q3InServant );
  ASSERT( q4InServant );

  if (q1InServant && q2InServant && q3InServant && q4InServant)
     {
     HEXA_NS::Quad* q1 = q1InServant->GetImpl();
     HEXA_NS::Quad* q2 = q2InServant->GetImpl();
     HEXA_NS::Quad* q3 = q3InServant->GetImpl();
     HEXA_NS::Quad* q4 = q4InServant->GetImpl();
     HEXA_NS::Hexa* h = _document_cpp->addHexa4Quads (q1, q2, q3, q4);

     if ( h != NULL )
        {
        Hexa_impl* servantCorba = new Hexa_impl(h);
        result = servantCorba->_this();
        }
     }
  return result;
}
// ===================================================== addHexa3Quads
Hexa_ptr Document_impl::addHexa3Quads (Quad_ptr q1, Quad_ptr q2, Quad_ptr q3)
                        throw (SALOME::SALOME_Exception)
{
  Hexa_ptr result = Hexa::_nil();

  Quad_impl* q1InServant = ::DownCast<Quad_impl*>( q1 );
  Quad_impl* q2InServant = ::DownCast<Quad_impl*>( q2 );
  Quad_impl* q3InServant = ::DownCast<Quad_impl*>( q3 );

  ASSERT( q1InServant );
  ASSERT( q2InServant );
  ASSERT( q3InServant );

  if (q1InServant && q2InServant && q3InServant)
     {
     HEXA_NS::Quad* q1 = q1InServant->GetImpl();
     HEXA_NS::Quad* q2 = q2InServant->GetImpl();
     HEXA_NS::Quad* q3 = q3InServant->GetImpl();
     HEXA_NS::Hexa* h = _document_cpp->addHexa3Quads (q1, q2, q3);

     if ( h != NULL )
        {
        Hexa_impl* servantCorba = new Hexa_impl(h);
        result = servantCorba->_this();
        }
     }

  return result;
}
// ===================================================== addHexa2Quads
Hexa_ptr Document_impl::addHexa2Quads (Quad_ptr q1, Quad_ptr q2)
                        throw (SALOME::SALOME_Exception)
{
  Hexa_ptr result = Hexa::_nil();

  Quad_impl* q1InServant = ::DownCast<Quad_impl*>( q1 );
  Quad_impl* q2InServant = ::DownCast<Quad_impl*>( q2 );

  ASSERT( q1InServant );
  ASSERT( q2InServant );

  if (q1InServant && q2InServant)
     {
     HEXA_NS::Quad* q1 = q1InServant->GetImpl();
     HEXA_NS::Quad* q2 = q2InServant->GetImpl();
     HEXA_NS::Hexa* h = _document_cpp->addHexa2Quads (q1, q2);

     if ( h != NULL )
        {
        Hexa_impl* servantCorba = new Hexa_impl(h);
        result = servantCorba->_this();
        }
     }

  return result;
}

// ===================================================== removeQuad
::CORBA::Boolean Document_impl::removeQuad(Quad_ptr quad) 
                 throw (SALOME::SALOME_Exception)
{
  Quad_impl* hServant = ::DownCast<Quad_impl*>( quad );
  ASSERT( hServant );

  if ( hServant ) {
    HEXA_NS::Quad* q = hServant->GetImpl();
    int ok = _document_cpp->removeQuad(q);
    if ( ok == HOK )
      return true;
    else
      return false;
  }
}
// ===================================================== removeElements
::CORBA::Boolean Document_impl::removeElements(Elements_ptr bloc) 
                 throw (SALOME::SALOME_Exception)
{
  Elements_impl* hServant = ::DownCast<Elements_impl*>( bloc );
  ASSERT( hServant );

  if ( hServant ) {
    HEXA_NS::Elements* q = hServant->GetImpl();
    int ok = _document_cpp->removeElements(q);
    if ( ok == HOK )
      return true;
    else
      return false;
  }
}
// ===================================================== revolutionQuads
Elements_ptr Document_impl::revolutionQuads (const Quads& start, 
                                    Vertex_ptr center, Vector_ptr axis, 
                                    const RealVector &angles)
           throw (SALOME::SALOME_Exception)
{
   Elements_ptr result = Elements::_nil();

   Vector_impl* v_axis   = ::DownCast<Vector_impl*> (axis);
   Vertex_impl* v_center = ::DownCast<Vertex_impl*> (center);

   ASSERT (v_axis);
   ASSERT (v_center);

   if (v_axis==NULL || v_center==NULL)
      return result;

   HEXA_NS::Quads t_start;
   for ( int nq = 0; nq < start.length(); nq++) 
       {
       Quad_impl* v_quad = ::DownCast<Quad_impl*> (start[nq]);
       ASSERT( v_quad );
       HEXA_NS::Quad* i_quad = v_quad->GetImpl();
       t_start.push_back(i_quad);
       }

   std::vector <CORBA::Double> t_angles;
   for ( int na = 0; na < angles.length(); na++) 
       {
       CORBA::Double alpha = angles[na];
       t_angles.push_back (alpha);
       }

   HEXA_NS::Vector*   i_axis   = v_axis  ->GetImpl();
   HEXA_NS::Vertex*   i_center = v_center->GetImpl();
   HEXA_NS::Elements* i_elts   = _document_cpp->revolutionQuads (t_start, 
                                 i_center, i_axis, t_angles);
   if (i_elts != NULL)
      {
      Elements_impl* servantCorba = new Elements_impl(i_elts);
      result = servantCorba->_this();
      }

   return result;
}
// ===================================================== prismQuadsVec
Elements_ptr Document_impl::prismQuadsVec (const Quads& start, Vector_ptr dir, 
                                    const RealVector &thaut, ::CORBA::Long opt)
           throw (SALOME::SALOME_Exception)
{
   Elements_ptr result = Elements::_nil();
   Vector_impl* v_dir  = ::DownCast<Vector_impl*> (dir);

   ASSERT (v_dir);

   if (v_dir==NULL)
      return result;

   HEXA_NS::Quads t_start;
   for ( int nq = 0; nq < start.length(); nq++) 
       {
       Quad_impl* v_quad = ::DownCast<Quad_impl*> (start[nq]);
       ASSERT( v_quad );
       HEXA_NS::Quad* i_quad = v_quad->GetImpl();
       t_start.push_back(i_quad);
       }

   std::vector <CORBA::Double> t_haut;
   for ( int na = 0; na < thaut.length(); na++) 
       {
       CORBA::Double alpha = thaut[na];
       t_haut.push_back (alpha);
       }

   HEXA_NS::Vector*   i_dir  = v_dir  ->GetImpl();
   HEXA_NS::Elements* i_elts = _document_cpp->prismQuadsVec (t_start, 
                                 i_dir, t_haut, opt);
   if (i_elts != NULL)
      {
      Elements_impl* servantCorba = new Elements_impl(i_elts);
      result = servantCorba->_this();
      }

   return result;
}
// ===================================================== makeSphere

Elements_ptr Document_impl::replace (const Quads& pattern, 
                                     Vertex_ptr p1, Vertex_ptr c1, 
                                     Vertex_ptr p2, Vertex_ptr c2,
                                     Vertex_ptr p3, Vertex_ptr c3)
           throw (SALOME::SALOME_Exception)
{
   Elements_ptr result = Elements::_nil();

   Vertex_impl* v_p1 = ::DownCast<Vertex_impl*> (p1);
   Vertex_impl* v_p2 = ::DownCast<Vertex_impl*> (p2);
   Vertex_impl* v_p3 = ::DownCast<Vertex_impl*> (p3);

   Vertex_impl* v_c1 = ::DownCast<Vertex_impl*> (c1);
   Vertex_impl* v_c2 = ::DownCast<Vertex_impl*> (c2);
   Vertex_impl* v_c3 = ::DownCast<Vertex_impl*> (c3);

   ASSERT (v_p1);    ASSERT (v_p2);    ASSERT (v_p3);
   ASSERT (v_c1);    ASSERT (v_c2);    ASSERT (v_c3);

   if (   v_c1==NULL || v_c2==NULL || v_c3==NULL
       || v_p1==NULL || v_p2==NULL || v_p3==NULL)
      return result;

   HEXA_NS::Quads t_pattern;
   for ( int nq = 0; nq < pattern.length(); nq++) 
       {
       Quad_impl* v_quad = ::DownCast<Quad_impl*> (pattern[nq]);
       ASSERT( v_quad );
       HEXA_NS::Quad* i_quad = v_quad->GetImpl();
       t_pattern.push_back(i_quad);
       }


   HEXA_NS::Vertex*   i_p1 = v_p1->GetImpl();
   HEXA_NS::Vertex*   i_p2 = v_p2->GetImpl();
   HEXA_NS::Vertex*   i_p3 = v_p3->GetImpl();

   HEXA_NS::Vertex*   i_c1 = v_c1->GetImpl();
   HEXA_NS::Vertex*   i_c2 = v_c2->GetImpl();
   HEXA_NS::Vertex*   i_c3 = v_c3->GetImpl();

   HEXA_NS::Elements* i_elts   = _document_cpp->replace (t_pattern, 
                                       i_p1, i_c1, i_p2, i_c2, i_p3, i_c3);
   if (i_elts != NULL)
      {
      Elements_impl* servantCorba = new Elements_impl(i_elts);
      result = servantCorba->_this();
      }

   return result;
}
// ===================================================== makeSphere
Elements_ptr Document_impl::makeSphere (Vertex_ptr center, Vector_ptr vx, 
                            Vector_ptr vz, ::CORBA::Double radius, 
                            ::CORBA::Double radhole, Vertex_ptr plorig, 
                 ::CORBA::Long nrad, ::CORBA::Long nang, ::CORBA::Long nhaut)
           throw (SALOME::SALOME_Exception)
{
   Elements_ptr result = Elements::_nil();

   Vertex_impl* v_center = ::DownCast<Vertex_impl*> (center);
   Vertex_impl* v_orig   = ::DownCast<Vertex_impl*> (plorig);
   Vector_impl* v_x      = ::DownCast<Vector_impl*> (vx);
   Vector_impl* v_z      = ::DownCast<Vector_impl*> (vz);

   ASSERT (v_center);
   ASSERT (v_orig);
   ASSERT (v_x);
   ASSERT (v_z);

   HEXA_NS::Vertex* i_center = v_center->GetImpl();
   HEXA_NS::Vertex* i_orig   = v_orig  ->GetImpl();
   HEXA_NS::Vector* i_x      = v_x     ->GetImpl();
   HEXA_NS::Vector* i_z      = v_z     ->GetImpl();

   HEXA_NS::Elements* i_elts  = _document_cpp->makeSphere (i_center, i_x, i_z,
                                radius, radhole, i_orig, nrad, nang, nhaut);

   if (i_elts != NULL)
      {
      Elements_impl* servantCorba = new Elements_impl(i_elts);
      result = servantCorba->_this();
      }

   return result;
}
// ===================================================== makePartSphere
Elements_ptr Document_impl::makePartSphere (Vertex_ptr center, Vector_ptr vx, 
                            Vector_ptr vz, ::CORBA::Double  radius, 
                            ::CORBA::Double radhole,
                             Vertex_ptr plorig, ::CORBA::Double angle,
                 ::CORBA::Long nrad, ::CORBA::Long nang, ::CORBA::Long nhaut)
           throw (SALOME::SALOME_Exception)
{
   Elements_ptr result = Elements::_nil();


   Vertex_impl* v_center = ::DownCast<Vertex_impl*> (center);
   Vertex_impl* v_orig   = ::DownCast<Vertex_impl*> (plorig);
   Vector_impl* v_x      = ::DownCast<Vector_impl*> (vx);
   Vector_impl* v_z      = ::DownCast<Vector_impl*> (vz);

   ASSERT (v_center);
   ASSERT (v_orig);
   ASSERT (v_x);
   ASSERT (v_z);

   HEXA_NS::Vertex* i_center = v_center->GetImpl();
   HEXA_NS::Vertex* i_orig   = v_orig  ->GetImpl();
   HEXA_NS::Vector* i_x      = v_x     ->GetImpl();
   HEXA_NS::Vector* i_z      = v_z     ->GetImpl();

   HEXA_NS::Elements* i_elts  = _document_cpp->makePartSphere (i_center, i_x, 
                      i_z, radius, radhole, i_orig, angle, nrad, nang, nhaut);

   if (i_elts != NULL)
      {
      Elements_impl* servantCorba = new Elements_impl(i_elts);
      result = servantCorba->_this();
      }

   return result;
}
// ===================================================== makeRind
Elements_ptr Document_impl::makeRind (Vertex_ptr center, Vector_ptr vx, 
                                      Vector_ptr vz, 
                 ::CORBA::Double  radext, ::CORBA::Double radint, 
                 ::CORBA::Double radhole, Vertex_ptr plorig, 
                 ::CORBA::Long nrad, ::CORBA::Long nang, ::CORBA::Long nhaut)
           throw (SALOME::SALOME_Exception)
{
   Elements_ptr result = Elements::_nil();

   Vertex_impl* v_center = ::DownCast<Vertex_impl*> (center);
   Vertex_impl* v_orig   = ::DownCast<Vertex_impl*> (plorig);
   Vector_impl* v_x      = ::DownCast<Vector_impl*> (vx);
   Vector_impl* v_z      = ::DownCast<Vector_impl*> (vz);

   ASSERT (v_center);
   ASSERT (v_orig);
   ASSERT (v_x);
   ASSERT (v_z);

   HEXA_NS::Vertex* i_center = v_center->GetImpl();
   HEXA_NS::Vertex* i_orig   = v_orig  ->GetImpl();
   HEXA_NS::Vector* i_x      = v_x     ->GetImpl();
   HEXA_NS::Vector* i_z      = v_z     ->GetImpl();

   HEXA_NS::Elements* i_elts  = _document_cpp->makeRind (i_center, i_x, i_z, 
                    radext, radint, radhole, i_orig, nrad, nang, nhaut);

   if (i_elts != NULL)
      {
      Elements_impl* servantCorba = new Elements_impl(i_elts);
      result = servantCorba->_this();
      }

   return result;
}
// ===================================================== makePartRind
Elements_ptr Document_impl::makePartRind (Vertex_ptr center, Vector_ptr vx, 
                                          Vector_ptr vz, 
                       ::CORBA::Double radext, ::CORBA::Double radint, 
                       ::CORBA::Double radhole,
                       Vertex_ptr plorig, ::CORBA::Double angle,
                 ::CORBA::Long nrad, ::CORBA::Long nang, ::CORBA::Long nhaut)
           throw (SALOME::SALOME_Exception)
{
   Elements_ptr result = Elements::_nil();


   Vertex_impl* v_center = ::DownCast<Vertex_impl*> (center);
   Vertex_impl* v_orig   = ::DownCast<Vertex_impl*> (plorig);
   Vector_impl* v_x      = ::DownCast<Vector_impl*> (vx);
   Vector_impl* v_z      = ::DownCast<Vector_impl*> (vz);

   ASSERT (v_center);
   ASSERT (v_orig);
   ASSERT (v_x);
   ASSERT (v_z);

   HEXA_NS::Vertex* i_center = v_center->GetImpl();
   HEXA_NS::Vertex* i_orig   = v_orig  ->GetImpl();
   HEXA_NS::Vector* i_x      = v_x     ->GetImpl();
   HEXA_NS::Vector* i_z      = v_z     ->GetImpl();

   HEXA_NS::Elements* i_elts = _document_cpp->makePartRind (i_center, i_x, i_z,
                    radext, radint, radhole, i_orig, angle, nrad, nang, nhaut);
   if (i_elts != NULL)
      {
      Elements_impl* servantCorba = new Elements_impl(i_elts);
      result = servantCorba->_this();
      }

   return result;
}
// ================================================== makeScale
Elements_ptr Document_impl::makeScale (Elements_ptr lIn, Vertex_ptr pIn, 
                                       ::CORBA::Double k)
                            throw (SALOME::SALOME_Exception)
{
  Elements_ptr result = Elements::_nil();

  Elements_impl* lServant = ::DownCast <Elements_impl*> ( lIn );
  Vertex_impl*   pServant = ::DownCast <Vertex_impl*>   ( pIn );

  ASSERT (lServant);
  ASSERT (pServant);

  if ( lServant && pServant ){
    HEXA_NS::Elements* l = lServant->GetImpl();
    HEXA_NS::Vertex*   p = pServant->GetImpl();

    HEXA_NS::Elements* r = _document_cpp->makeScale(l, p, k);
    if ( r != NULL ){
      Elements_impl* servantCorba = new Elements_impl(r);
      result = servantCorba->_this();
    }
  }
  return result;
}
// ================================================== makeSymmetryLine
Elements_ptr Document_impl::makeSymmetryLine (Elements_ptr lIn, Vertex_ptr pIn,
                                              Vector_ptr vecIn)
             throw (SALOME::SALOME_Exception)
{
  Elements_ptr result = Elements::_nil();

  Elements_impl* lServant = ::DownCast<Elements_impl*>( lIn );
  Vertex_impl*   pServant = ::DownCast<Vertex_impl*>( pIn );
  Vector_impl* vecServant = ::DownCast<Vector_impl*>( vecIn );

  ASSERT( lServant );
  ASSERT( pServant );
  ASSERT( vecServant );

  if ( lServant && pServant && vecServant ){
    HEXA_NS::Elements* l = lServant->GetImpl();
    HEXA_NS::Vertex*   p = pServant->GetImpl();
    HEXA_NS::Vector* vec = vecServant->GetImpl();

    HEXA_NS::Elements* r = _document_cpp->makeSymmetryLine(l, p, vec);
    if ( r != NULL ){
      Elements_impl* servantCorba = new Elements_impl(r);
      result = servantCorba->_this();
    }
  }
  return result;
}
// ================================================== makeSymmetryPlane
Elements_ptr Document_impl::makeSymmetryPlane (Elements_ptr lIn, Vertex_ptr pIn,
                                              Vector_ptr vecIn)
             throw (SALOME::SALOME_Exception)
{
  Elements_ptr result = Elements::_nil();

  Elements_impl* lServant = ::DownCast<Elements_impl*>( lIn );
  Vertex_impl*   pServant = ::DownCast<Vertex_impl*>( pIn );
  Vector_impl* vecServant = ::DownCast<Vector_impl*>( vecIn );

  ASSERT( lServant );
  ASSERT( pServant );
  ASSERT( vecServant );

  if ( lServant && pServant && vecServant ){
    HEXA_NS::Elements* l = lServant->GetImpl();
    HEXA_NS::Vertex*   p = pServant->GetImpl();
    HEXA_NS::Vector* vec = vecServant->GetImpl();

    HEXA_NS::Elements* r = _document_cpp->makeSymmetryPlane(l, p, vec);
    if ( r != NULL ){
      Elements_impl* servantCorba = new Elements_impl(r);
      result = servantCorba->_this();
    }
  }
  return result;
}
// ================================================== makeSymmetryPoint
Elements_ptr Document_impl::makeSymmetryPoint (Elements_ptr lIn, Vertex_ptr pIn)
             throw (SALOME::SALOME_Exception)
{
  Elements_ptr result = Elements::_nil();

  Elements_impl* lServant = ::DownCast<Elements_impl*>( lIn );
  Vertex_impl*   pServant = ::DownCast<Vertex_impl*>( pIn );

  ASSERT( lServant );
  ASSERT( pServant );

  if ( lServant && pServant ){
    HEXA_NS::Elements* l = lServant->GetImpl();
    HEXA_NS::Vertex*   p = pServant->GetImpl();

    HEXA_NS::Elements* r = _document_cpp->makeSymmetryPoint(l, p);
    if ( r != NULL ){
      Elements_impl* servantCorba = new Elements_impl(r);
      result = servantCorba->_this();
    }
  }
  return result;
}
// ================================================== performScale
void Document_impl::performScale (Elements_ptr lIn, Vertex_ptr pIn, 
                                  ::CORBA::Double k) 
     throw (SALOME::SALOME_Exception)
{
  Elements_impl* lServant = ::DownCast<Elements_impl*>( lIn );
  Vertex_impl*   pServant = ::DownCast<Vertex_impl*>( pIn );

  ASSERT( lServant );
  ASSERT( pServant );

  if ( lServant && pServant ){
    HEXA_NS::Elements* l = lServant->GetImpl();
    HEXA_NS::Vertex*   p = pServant->GetImpl();
    _document_cpp->performScale (l, p, k);
  }
}
// ================================================== performSymmetryPlane
void Document_impl::performSymmetryPlane (Elements_ptr lIn, Vertex_ptr pIn, 
                                          Vector_ptr vecIn) 
     throw (SALOME::SALOME_Exception)
{
  Elements_impl* lServant = ::DownCast<Elements_impl*>( lIn );
  Vertex_impl*   pServant = ::DownCast<Vertex_impl*>( pIn );
  Vector_impl* vecServant = ::DownCast<Vector_impl*>( vecIn );

  ASSERT( lServant );
  ASSERT( pServant );
  ASSERT( vecServant );

  if ( lServant && pServant && vecServant ){
    HEXA_NS::Elements* l = lServant->GetImpl();
    HEXA_NS::Vertex*   p = pServant->GetImpl();
    HEXA_NS::Vector* vec = vecServant->GetImpl();
    _document_cpp->performSymmetryPlane (l, p, vec);
  }
}
// ================================================== performSymmetryLine
void Document_impl::performSymmetryLine (Elements_ptr lIn, Vertex_ptr pIn, 
                                          Vector_ptr vecIn) 
     throw (SALOME::SALOME_Exception)
{
  Elements_impl* lServant = ::DownCast<Elements_impl*>( lIn );
  Vertex_impl*   pServant = ::DownCast<Vertex_impl*>( pIn );
  Vector_impl* vecServant = ::DownCast<Vector_impl*>( vecIn );

  ASSERT( lServant );
  ASSERT( pServant );
  ASSERT( vecServant );

  if ( lServant && pServant && vecServant ){
    HEXA_NS::Elements* l = lServant->GetImpl();
    HEXA_NS::Vertex*   p = pServant->GetImpl();
    HEXA_NS::Vector* vec = vecServant->GetImpl();
    _document_cpp->performSymmetryLine (l, p, vec);
  }
}
// ================================================== performSymmetryPoint
void Document_impl::performSymmetryPoint (Elements_ptr lIn, Vertex_ptr pIn)
     throw (SALOME::SALOME_Exception)
{
  Elements_impl* lServant = ::DownCast<Elements_impl*>( lIn );
  Vertex_impl*   pServant = ::DownCast<Vertex_impl*>( pIn );

  ASSERT( lServant );
  ASSERT( pServant );

  if ( lServant && pServant ){
    HEXA_NS::Elements* l = lServant->GetImpl();
    HEXA_NS::Vertex*   p = pServant->GetImpl();
    _document_cpp->performSymmetryPoint (l, p);
  }
}
// ================================================== setLevel
void Document_impl::setLevel(::CORBA::Long debug)
     throw (SALOME::SALOME_Exception)
{
    _document_cpp->setLevel (debug);
}
// ================================================== clearAssociation
void Document_impl::clearAssociation ()
     throw (SALOME::SALOME_Exception)
{
    _document_cpp->clearAssociation ();
}
