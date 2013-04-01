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
#ifndef __HexNewShape_impl__
#define __HexNewShape_impl__

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(NewShape)
#include "SALOME_GenericObj_i.hh"

#include "hexa_base.hxx"
#include "HexNewShape.hxx"

using namespace HEXABLOCK_ORB;

class NewShape_impl : public POA_HEXABLOCK_ORB::NewShape,
                      public SALOME::GenericObj_i
{
public:
  NewShape_impl (HEXA_NS::NewShape *ptrCpp);
  HEXA_NS::NewShape* GetImpl() throw (SALOME::SALOME_Exception);


  ::CORBA::Long countVertex () 
                                       throw (SALOME::SALOME_Exception);
  ::CORBA::Long countEdge   ()   
                                       throw (SALOME::SALOME_Exception);
  ::CORBA::Long countFace   ()   
                                       throw (SALOME::SALOME_Exception);

  ::CORBA::Long getVertex (::CORBA::Long nro)
                                       throw (SALOME::SALOME_Exception);
  ::CORBA::Long getEdge   (::CORBA::Long nro)
                                       throw (SALOME::SALOME_Exception);
  ::CORBA::Long getFace   (::CORBA::Long nro)
                                       throw (SALOME::SALOME_Exception);

  char* getNameVertex (::CORBA::Long nro)
                                       throw (SALOME::SALOME_Exception);
  char* getNameEdge   (::CORBA::Long nro)
                                       throw (SALOME::SALOME_Exception);
  char* getNameFace   (::CORBA::Long nro)
                                       throw (SALOME::SALOME_Exception);

  void  dump()      throw (SALOME::SALOME_Exception);
  void  printName() throw (SALOME::SALOME_Exception);
  char* getName()   throw (SALOME::SALOME_Exception);
  void  setName (const char* name) throw (SALOME::SALOME_Exception);


private:
  HEXA_NS::NewShape*  _shape_cpp;
};

#endif
