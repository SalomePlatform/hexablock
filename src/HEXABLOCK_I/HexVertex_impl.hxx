#ifndef __HexVertex_impl__
#define __HexVertex_impl__

// #include <TopoDS_Shape.hxx>

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(Vertex)
#include "SALOME_GenericObj_i.hh"

#include "hexa_base.hxx"

#include "hexa_base.hxx"
#include "HexVertex.hxx"

class Vertex_impl : public POA_HEXABLOCK_ORB::Vertex,
		    public SALOME::GenericObj_i
{
public:
  Vertex_impl(HEXA_NS::Vertex *ptrCpp);
  HEXA_NS::Vertex* GetImpl();

  ::CORBA::Double getX() throw (SALOME::SALOME_Exception);
  ::CORBA::Double getY() throw (SALOME::SALOME_Exception);
  ::CORBA::Double getZ() throw (SALOME::SALOME_Exception);
  void setX(::CORBA::Double x) throw (SALOME::SALOME_Exception);
  void setY(::CORBA::Double y) throw (SALOME::SALOME_Exception);
  void setZ(::CORBA::Double z) throw (SALOME::SALOME_Exception);
  void setAssociation(GEOM::GEOM_Object_ptr geom_object_vertex) throw (SALOME::SALOME_Exception);
  GEOM::GEOM_Object_ptr getAssociation() throw (SALOME::SALOME_Exception);
//   void removeAssociation() throw (SALOME::SALOME_Exception);

  void setScalar( ::CORBA::Double val )throw (SALOME::SALOME_Exception);

  void dump() throw (SALOME::SALOME_Exception);
  void printName() throw (SALOME::SALOME_Exception);

private:
  HEXA_NS::Vertex* _vertex_cpp;
//   GEOM::GEOM_Object_ptr _association;
};

#endif
