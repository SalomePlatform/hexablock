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
