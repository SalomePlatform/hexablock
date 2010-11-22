#ifndef __HexCrossElements_impl__
#define __HexCrossElements_impl__

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(CrossElements)
#include "SALOME_GenericObj_i.hh"

#include "hexa_base.hxx"
#include "HexCrossElements.hxx"

using namespace HEXABLOCK_ORB;

class CrossElements_impl : public POA_HEXABLOCK_ORB::CrossElements,
		           public SALOME::GenericObj_i
{
public:
    CrossElements_impl(HEXA_NS::CrossElements *ptrCpp);
    HEXA_NS::CrossElements* GetImpl();

    Vertex_ptr getVertexIJK(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z) throw (SALOME::SALOME_Exception);

    Edge_ptr getEdgeI(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z) throw (SALOME::SALOME_Exception);
    Edge_ptr getEdgeJ(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z) throw (SALOME::SALOME_Exception);
    Edge_ptr getEdgeK(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z) throw (SALOME::SALOME_Exception);

    Quad_ptr getQuadIJ(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z) throw (SALOME::SALOME_Exception);
    Quad_ptr getQuadIK(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z) throw (SALOME::SALOME_Exception);
    Quad_ptr getQuadJK(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z) throw (SALOME::SALOME_Exception);

    Hexa_ptr getHexaIJK(::CORBA::Long part, ::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z) throw (SALOME::SALOME_Exception);

    ::CORBA::Long crossCylinders(Cylinder_ptr cyl1, Cylinder_ptr cyl2) throw (SALOME::SALOME_Exception);

    ::CORBA::Long saveVtk(const char* fname) throw (SALOME::SALOME_Exception);
  void dump() throw (SALOME::SALOME_Exception);
  void printName() throw (SALOME::SALOME_Exception);

private:
    HEXA_NS::CrossElements *_crossElements_cpp;
};

#endif
