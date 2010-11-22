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

private:
  HEXA_NS::Pipe *_pipe_cpp;
};

#endif
