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
#ifndef __Law_impl__
#define __Law_impl__

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(Law)
#include "SALOME_GenericObj_i.hh"

#include "hexa_base.hxx"
#include "HexLaw.hxx"


using namespace HEXABLOCK_ORB;

class Law_impl : public POA_HEXABLOCK_ORB::Law,
		    public SALOME::GenericObj_i
{
public:
  Law_impl(HEXA_NS::Law *ptrCpp);
  HEXA_NS::Law* GetImpl() throw (SALOME::SALOME_Exception);

  void setName(const char* name) throw (SALOME::SALOME_Exception);
  char* getName() throw (SALOME::SALOME_Exception);
  void setNodes(::CORBA::Long n) throw (SALOME::SALOME_Exception);
  ::CORBA::Long getNodes() throw (SALOME::SALOME_Exception);
  void setKind(KindLaw k) throw (SALOME::SALOME_Exception);
  KindLaw getKind() throw (SALOME::SALOME_Exception);
  void setCoefficient(::CORBA::Double c) throw (SALOME::SALOME_Exception);
  ::CORBA::Double getCoefficient() throw (SALOME::SALOME_Exception);

private:
  HEXA_NS::Law *_law_cpp;
};

#endif
