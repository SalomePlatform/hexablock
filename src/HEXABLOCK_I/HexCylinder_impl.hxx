#ifndef __HexCylinder_impl__
#define __HexCylinder_impl__

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(Cylinder)
#include "SALOME_GenericObj_i.hh"

#include "hexa_base.hxx"
#include "HexCylinder.hxx"

using namespace HEXABLOCK_ORB;

class Cylinder_impl : public POA_HEXABLOCK_ORB::Cylinder,
		    public SALOME::GenericObj_i
{
public:
  Cylinder_impl(HEXA_NS::Cylinder *ptrCpp);
  HEXA_NS::Cylinder* GetImpl() throw (SALOME::SALOME_Exception);

  Vertex_ptr getBase() throw (SALOME::SALOME_Exception);
  Vector_ptr getDirection() throw (SALOME::SALOME_Exception);
  ::CORBA::Double getRadius() throw (SALOME::SALOME_Exception);
  ::CORBA::Double getHeight() throw (SALOME::SALOME_Exception);

//   ::CORBA::Long saveVtk(const char* fname) throw (SALOME::SALOME_Exception);
  void dump() throw (SALOME::SALOME_Exception);
  void printName() throw (SALOME::SALOME_Exception);

private:
  HEXA_NS::Cylinder *_cylinder_cpp;
};

#endif
