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
#ifndef __HexPipe_impl__
#define __HexPipe_impl__

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(Pipe)
#include "SALOME_GenericObj_i.hh"

#include "hexa_base.hxx"
#include "HexPipe.hxx"

using namespace HEXABLOCK_ORB;

class Pipe_impl : public POA_HEXABLOCK_ORB::Pipe,
		  public SALOME::GenericObj_i
{
public:
  Pipe_impl(HEXA_NS::Pipe *ptrCpp);
  HEXA_NS::Pipe* GetImpl() throw (SALOME::SALOME_Exception);

  Vertex_ptr getBase() throw (SALOME::SALOME_Exception);
  Vector_ptr getDirection() throw (SALOME::SALOME_Exception);
  ::CORBA::Double getInternal_radius() throw (SALOME::SALOME_Exception);
  ::CORBA::Double getRadius() throw (SALOME::SALOME_Exception);
  ::CORBA::Double getHeight() throw (SALOME::SALOME_Exception);
  void dump() throw (SALOME::SALOME_Exception);
  void printName() throw (SALOME::SALOME_Exception);
  char* getName() throw (SALOME::SALOME_Exception);
  void  setName (const char* name) throw (SALOME::SALOME_Exception);

private:
  HEXA_NS::Pipe *_pipe_cpp;
};

#endif
