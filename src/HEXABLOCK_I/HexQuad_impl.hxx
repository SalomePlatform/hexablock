#ifndef __HexQuad_impl__
#define __HexQuad_impl__

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)
#include CORBA_SERVER_HEADER(Quad)
#include "SALOME_GenericObj_i.hh"

#include "hexa_base.hxx"
#include "HexQuad.hxx"

using namespace HEXABLOCK_ORB;

class Quad_impl : public POA_HEXABLOCK_ORB::Quad,
		  public SALOME::GenericObj_i
{
public:
  Quad_impl(HEXA_NS::Quad *ptrCpp);
  HEXA_NS::Quad* GetImpl() throw (SALOME::SALOME_Exception);

  Edge_ptr getEdge(::CORBA::Long n) throw (SALOME::SALOME_Exception);
  Vertex_ptr getVertex(::CORBA::Long n) throw (SALOME::SALOME_Exception);
//   void setAssociation(GEOM::GEOM_Object_ptr geom_object_2D) throw (SALOME::SALOME_Exception);
//   GEOM::GEOM_Object_ptr getAssociation() throw (SALOME::SALOME_Exception);
  ::CORBA::Long addAssociation ( GEOM::GEOM_Object_ptr geom_object_2D) //CS_NOT_SPEC
      throw (SALOME::SALOME_Exception);
  GEOM::ListOfGO* getAssociations () //CS_NOT_SPEC
      throw (SALOME::SALOME_Exception);


//   void removeAssociation() throw (SALOME::SALOME_Exception);

  void setScalar( ::CORBA::Double val )throw (SALOME::SALOME_Exception);
  void dump() throw (SALOME::SALOME_Exception);
  void printName() throw (SALOME::SALOME_Exception);
private:
  HEXA_NS::Quad *_quad_cpp;
//   std::vector<GEOM::GEOM_Object_ptr> _associations;
};

#endif
