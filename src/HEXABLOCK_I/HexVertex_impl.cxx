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
#include "HexNewShape_impl.hxx"

Vertex_impl::Vertex_impl( HEXA_NS::Vertex *ptrCpp ):
_vertex_cpp( ptrCpp )
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
  _vertex_cpp->setZ(z);
}

void Vertex_impl::clearAssociation()
      throw (SALOME::SALOME_Exception)
{
  _vertex_cpp->clearAssociation ();
}


void Vertex_impl::setColor (::CORBA::Double val)
     throw (SALOME::SALOME_Exception)
{
  _vertex_cpp->setColor (val);
}

void Vertex_impl::dump() throw (SALOME::SALOME_Exception)
{
  _vertex_cpp->dump();
}


void Vertex_impl::printName()throw (SALOME::SALOME_Exception)
{
  _vertex_cpp->printName();
}


char* Vertex_impl::getName() throw (SALOME::SALOME_Exception)
{
  return CORBA::string_dup( _vertex_cpp->getName() );
}
// ========================================================== setAssociation
::CORBA::Long Vertex_impl::setAssociation (NewShape_ptr  geom,
                                             ::CORBA::Long subid)
                                      throw (SALOME::SALOME_Exception)
{
  NewShape_impl*     im_shape = ::DownCast<NewShape_impl*> (geom );
  HEXA_NS::NewShape* md_shape = im_shape->GetImpl();

  ::CORBA::Long ier = _vertex_cpp->setAssociation (md_shape, subid);
  return ier;
}
// ========================================================= setName
void Vertex_impl::setName(const char* name)
     throw (SALOME::SALOME_Exception)
{
  _vertex_cpp->setName (name);
}
// ========================================================= getAssoX
CORBA::Double Vertex_impl::getAssoX() throw(SALOME::SALOME_Exception)
{
  return _vertex_cpp->getAssoX();
}
// ========================================================= getAssoY
CORBA::Double Vertex_impl::getAssoY() throw(SALOME::SALOME_Exception)
{
  return _vertex_cpp->getAssoY();
}
// ========================================================= getAssoZ
CORBA::Double Vertex_impl::getAssoZ() throw(SALOME::SALOME_Exception)
{
  return _vertex_cpp->getAssoZ();
}
