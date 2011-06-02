// Copyright (C) 2009-2011  CEA/DEN, EDF R&D
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

#ifndef __HexVector_impl__
#define __HexVector_impl__

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(Vector)
#include "SALOME_GenericObj_i.hh"

#include "hexa_base.hxx"
#include "HexVector.hxx"

using namespace HEXABLOCK_ORB;

class Vector_impl : public POA_HEXABLOCK_ORB::Vector,
		    public SALOME::GenericObj_i
{
  public:
    Vector_impl(HEXA_NS::Vector *ptrCpp);
    HEXA_NS::Vector* GetImpl() throw (SALOME::SALOME_Exception);

    ::CORBA::Double getDX() throw(SALOME::SALOME_Exception);
    ::CORBA::Double getDY() throw(SALOME::SALOME_Exception);
    ::CORBA::Double getDZ() throw(SALOME::SALOME_Exception);
   void dump() throw (SALOME::SALOME_Exception);
   void printName() throw (SALOME::SALOME_Exception);
  private:
    HEXA_NS::Vector *_vector_cpp;

};

#endif
