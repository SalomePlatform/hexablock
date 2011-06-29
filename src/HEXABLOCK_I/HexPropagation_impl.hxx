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
#ifndef __Propagation_impl__
#define __Propagation_impl__

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(Propagation)
#include "SALOME_GenericObj_i.hh"

#include "hexa_base.hxx"
#include "HexPropagation.hxx"

using namespace HEXABLOCK_ORB;

class Propagation_impl : public POA_HEXABLOCK_ORB::Propagation,
		    public SALOME::GenericObj_i
{
public:
  Propagation_impl(HEXA_NS::Propagation *ptrCpp);
  HEXA_NS::Propagation* GetImpl() throw (SALOME::SALOME_Exception);

  Edges* getEdges() throw (SALOME::SALOME_Exception);
//   Ways* getWays() throw (SALOME::SALOME_Exception);

  void setLaw(Law_ptr l) throw (SALOME::SALOME_Exception);
  Law_ptr getLaw() throw (SALOME::SALOME_Exception);
  void setWay(::CORBA::Boolean w) throw (SALOME::SALOME_Exception);
  ::CORBA::Boolean getWay() throw (SALOME::SALOME_Exception);


private:
  HEXA_NS::Propagation *_propagation_cpp;
};

#endif
