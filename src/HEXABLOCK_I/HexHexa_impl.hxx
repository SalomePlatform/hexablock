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

#ifndef __HexHexa_impl__
#define __HexHexa_impl__

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(Hexa)
#include "SALOME_GenericObj_i.hh"

#include "hexa_base.hxx"
#include "HexHexa.hxx"

using namespace HEXABLOCK_ORB;

class Hexa_impl : public POA_HEXABLOCK_ORB::Hexa,
		    public SALOME::GenericObj_i
{
public:
  Hexa_impl(HEXA_NS::Hexa *ptrCpp);
  HEXA_NS::Hexa* GetImpl() throw (SALOME::SALOME_Exception);

  Quad_ptr getQuad(::CORBA::Long n) throw (SALOME::SALOME_Exception);
  Edge_ptr getEdge(::CORBA::Long n) throw (SALOME::SALOME_Exception);
  Vertex_ptr getVertex(::CORBA::Long n) throw (SALOME::SALOME_Exception);

  void setScalar( ::CORBA::Double val ) throw (SALOME::SALOME_Exception);
  void dump() throw (SALOME::SALOME_Exception);
  void printName() throw (SALOME::SALOME_Exception);
private:
  HEXA_NS::Hexa *_hexa_cpp;
};

#endif
