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
#include "HexPropagation_impl.hxx"
#include "HexEdge_impl.hxx"
#include "HexLaw_impl.hxx"

Propagation_impl::Propagation_impl( HEXA_NS::Propagation *ptrCpp ):_propagation_cpp(ptrCpp)
{
}



HEXA_NS::Propagation* Propagation_impl::GetImpl() throw (SALOME::SALOME_Exception)
{
  return _propagation_cpp;
}


HEXABLOCK_ORB::Edges* Propagation_impl::getEdges() throw (SALOME::SALOME_Exception)
{
  HEXABLOCK_ORB::Edges* result = new HEXABLOCK_ORB::Edges;

  const HEXA_NS::Edges& edges_cpp = _propagation_cpp->getEdges();

  CORBA::ULong i = 0;
  result->length( edges_cpp.size() );
  for ( HEXA_NS::Edges::const_iterator iter = edges_cpp.begin();
	iter != edges_cpp.end();
        ++iter){
      Edge_impl *servantEdge = new Edge_impl( *iter );
      (*result)[ i++ ] = servantEdge->_this();

  }
  return result;
}


void Propagation_impl::setLaw(HEXABLOCK_ORB::Law_ptr lawIn) throw (SALOME::SALOME_Exception)
{
  Law_impl* lawInServant = ::DownCast<Law_impl*>( lawIn );
  ASSERT( lawInServant );

  if ( lawInServant ){
    HEXA_NS::Law* law = lawInServant->GetImpl();
    _propagation_cpp->setLaw(law);
  }
}

HEXABLOCK_ORB::Law_ptr Propagation_impl::getLaw() throw (SALOME::SALOME_Exception)
{
    Law_ptr result = Law::_nil();
    HEXA_NS::Law* l = _propagation_cpp->getLaw();

    if ( l != NULL ){
      Law_impl* servantCorba = new Law_impl(l);
      result = servantCorba->_this();
    }
    return result;
}



void Propagation_impl::setWay(::CORBA::Boolean w) throw (SALOME::SALOME_Exception)
{
  _propagation_cpp->setWay(w);
}



::CORBA::Boolean Propagation_impl::getWay() throw (SALOME::SALOME_Exception)
{
//   bool way = _propagation_cpp->getWay();
//   if ( way == true ) {
//     return CORBA::TRUE;
//   else {
//     return CORBA::FALSE;
//   }
  return _propagation_cpp->getWay();
}

