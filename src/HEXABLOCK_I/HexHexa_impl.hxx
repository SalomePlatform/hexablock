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
