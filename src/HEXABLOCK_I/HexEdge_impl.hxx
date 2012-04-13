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
#ifndef __HexEdge_impl__
#define __HexEdge_impl__

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(Edge)
#include "SALOME_GenericObj_i.hh"

#include "hexa_base.hxx"
#include "HexEdge.hxx"

using namespace HEXABLOCK_ORB;

class Edge_impl : public POA_HEXABLOCK_ORB::Edge,
		  public SALOME::GenericObj_i
{
  struct Assoc {
    GEOM::GEOM_Object_ptr geomObj;
    double debut;
    double fin;
  };

public:
  Edge_impl(HEXA_NS::Edge *ptrCpp);
  HEXA_NS::Edge* GetImpl() throw (SALOME::SALOME_Exception);

  ::CORBA::Boolean getWay() throw (SALOME::SALOME_Exception);
  Vertex_ptr getVertex(::CORBA::Long n) throw (SALOME::SALOME_Exception);
//   void setAssociation(GEOM::GEOM_Object_ptr geom_object_1D) throw (SALOME::SALOME_Exception);
//   GEOM::GEOM_Object_ptr getAssociation() throw (SALOME::SALOME_Exception);
  ::CORBA::Long addAssociation( GEOM::GEOM_Object_ptr geom_object_1D, double debut, double fin ) //CS_NOT_SPEC
      throw (SALOME::SALOME_Exception);
  void clearAssociation() throw (SALOME::SALOME_Exception);

  EdgeAssociations* getAssociations () //CS_NOT_SPEC
      throw (SALOME::SALOME_Exception);

  void setScalar( ::CORBA::Double val )throw (SALOME::SALOME_Exception);
  void dump() throw (SALOME::SALOME_Exception);
  void printName() throw (SALOME::SALOME_Exception);
  char* getName() throw (SALOME::SALOME_Exception);
  void  setName (const char* name) throw (SALOME::SALOME_Exception);

private:
  HEXA_NS::Edge*      _edge_cpp;
  std::vector<Assoc>  _associations;
};

#endif
