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
