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
