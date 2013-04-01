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

#include "HexVertex_impl.hxx"
#include "HexEdge_impl.hxx"
#include "HexNewShape_impl.hxx"

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

char* Edge_impl::getName() throw (SALOME::SALOME_Exception)
{
  return CORBA::string_dup( _edge_cpp->getName() );
}

// ========================================================= setName
void Edge_impl::setName(const char* name)
     throw (SALOME::SALOME_Exception)
{
  _edge_cpp->setName (name);
}

// ========================================================= setColor
void Edge_impl::setColor (::CORBA::Double val)
     throw (SALOME::SALOME_Exception)
{
  _edge_cpp->setColor (val);
}

// ========================================================= clearAssociation
void Edge_impl::clearAssociation()
      throw (SALOME::SALOME_Exception)
{
  _edge_cpp->clearAssociation ();
}

// =========================================================== countAssociation
::CORBA::Long Edge_impl::countAssociation ()
  throw (SALOME::SALOME_Exception)
{
  return _edge_cpp->countAssociation ();
}
// ========================================================= addAssociation
::CORBA::Long Edge_impl::addAssociation (NewShape_ptr geom,
                                         ::CORBA::Long subid,
                                         double debut, double fin )
                                    throw (SALOME::SALOME_Exception)
{
  NewShape_impl*     im_shape = ::DownCast<NewShape_impl*> (geom );
  HEXA_NS::NewShape* md_shape = im_shape->GetImpl();

  ::CORBA::Long ier = _edge_cpp->addAssociation (md_shape, subid, debut, fin);
  return ier;
}
