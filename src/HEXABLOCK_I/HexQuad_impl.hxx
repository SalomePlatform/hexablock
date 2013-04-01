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
#ifndef __HexQuad_impl__
#define __HexQuad_impl__

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)
#include CORBA_SERVER_HEADER(Quad)
#include "SALOME_GenericObj_i.hh"

#include "hexa_base.hxx"
#include "HexQuad.hxx"

using namespace HEXABLOCK_ORB;

class Quad_impl : public POA_HEXABLOCK_ORB::Quad,
		  public SALOME::GenericObj_i
{
public:
  Quad_impl(HEXA_NS::Quad *ptrCpp);
  HEXA_NS::Quad* GetImpl() throw (SALOME::SALOME_Exception);

  Edge_ptr getEdge(::CORBA::Long n) throw (SALOME::SALOME_Exception);
  Vertex_ptr getVertex(::CORBA::Long n) throw (SALOME::SALOME_Exception);

  ::CORBA::Long addAssociation (NewShape_ptr geom, ::CORBA::Long subid)
                         throw (SALOME::SALOME_Exception);

  GEOM::ListOfGO* getAssociations () //CS_NOT_SPEC
      throw (SALOME::SALOME_Exception);


  void clearAssociation() throw (SALOME::SALOME_Exception);

  void setColor (::CORBA::Double val)  throw (SALOME::SALOME_Exception);
  void setScalar( ::CORBA::Double val )throw (SALOME::SALOME_Exception);
  void dump() throw (SALOME::SALOME_Exception);
  void printName() throw (SALOME::SALOME_Exception);
  char* getName() throw (SALOME::SALOME_Exception);
  void  setName (const char* name) throw (SALOME::SALOME_Exception);

private:
  HEXA_NS::Quad *_quad_cpp;
};

#endif
