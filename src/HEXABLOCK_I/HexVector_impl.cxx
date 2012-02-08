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
#include "HexVector_impl.hxx"

using namespace HEXABLOCK_ORB;

Vector_impl::Vector_impl( HEXA_NS::Vector *ptrCpp ):_vector_cpp(ptrCpp)
{
}

HEXA_NS::Vector* Vector_impl::GetImpl() throw (SALOME::SALOME_Exception)
{
  return _vector_cpp;
}

::CORBA::Double Vector_impl::getDX() throw (SALOME::SALOME_Exception)
{
  ::CORBA::Double dx = _vector_cpp->getDx();
  return dx;
}
::CORBA::Double Vector_impl::getDY() throw (SALOME::SALOME_Exception)
{
  ::CORBA::Double dy = _vector_cpp->getDy();
  return dy;
}
::CORBA::Double Vector_impl::getDZ() throw (SALOME::SALOME_Exception)
{
  ::CORBA::Double dz = _vector_cpp->getDz();
  return dz;
}


void Vector_impl::dump() throw (SALOME::SALOME_Exception)
{
  _vector_cpp->dump();
}


void Vector_impl::printName() throw (SALOME::SALOME_Exception)
{
  _vector_cpp->printName();
}

char* Vector_impl::getName() throw (SALOME::SALOME_Exception)
{
   return CORBA::string_dup( _vector_cpp->getName() );
}
// ========================================================= setName
void Vector_impl::setName(const char* name) 
     throw (SALOME::SALOME_Exception)
{
  _vector_cpp->setName (name);
}
