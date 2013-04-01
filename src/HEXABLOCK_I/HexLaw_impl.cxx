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
#include "HexLaw_impl.hxx"

Law_impl::Law_impl( HEXA_NS::Law *ptrCpp ):_law_cpp(ptrCpp)
{
}

HEXA_NS::Law* Law_impl::GetImpl()
throw (SALOME::SALOME_Exception)
{
  return _law_cpp;
}

// HEXA_NS::Vertex* v = _hexa_cpp->getVertex(n);
//   Vertex_impl* servantCorba = new Vertex_impl(v);
//   return servantCorba->_this();


void Law_impl::setName(const char* name)
throw (SALOME::SALOME_Exception)
{
  _law_cpp->setName(name);
}

char* Law_impl::getName()
throw (SALOME::SALOME_Exception)
{
  return CORBA::string_dup( _law_cpp->getName() );
}

void Law_impl::setNodes(::CORBA::Long n)
throw (SALOME::SALOME_Exception)
{
  _law_cpp->setNodes(n);
}

::CORBA::Long Law_impl::getNodes()
throw (SALOME::SALOME_Exception)
{
  return _law_cpp->getNodes();
}

void Law_impl::setKind( HEXABLOCK_ORB::KindLaw kIn )
throw (SALOME::SALOME_Exception)
{
  HEXA_NS::KindLaw k_impl;
  switch (kIn) {
    case HEXABLOCK_ORB::UNIFORM    : k_impl = HEXA_NS::Uniform; break;
    case HEXABLOCK_ORB::ARITHMETIC : k_impl = HEXA_NS::Arithmetic;  break;
    case HEXABLOCK_ORB::GEOMETRIC  : k_impl = HEXA_NS::Geometric;   break;
  }
  _law_cpp->setKind(k_impl);
}

HEXABLOCK_ORB::KindLaw Law_impl::getKind()
throw (SALOME::SALOME_Exception)
{
  HEXABLOCK_ORB::KindLaw k;
  HEXA_NS::KindLaw k_cpp = _law_cpp->getKind();
  switch (k_cpp) {
    case HEXA_NS::Uniform :    k = HEXABLOCK_ORB::UNIFORM; break;
    case HEXA_NS::Arithmetic : k = HEXABLOCK_ORB::ARITHMETIC; break;
    case HEXA_NS::Geometric :  k = HEXABLOCK_ORB::GEOMETRIC; break;
    default :                  ASSERT( false ); break;
  }
  return k;
}


void Law_impl::setCoefficient(::CORBA::Double c)
throw (SALOME::SALOME_Exception)
{
  _law_cpp->setCoefficient(c);
}

::CORBA::Double Law_impl::getCoefficient()
throw (SALOME::SALOME_Exception)
{
  return _law_cpp->getCoefficient();
}
