#ifndef __HexGroup_impl__
#define __HexGroup_impl__

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(Group)
#include "SALOME_GenericObj_i.hh"

#include "hexa_base.hxx"
#include "HexGroups.hxx"


using namespace HEXABLOCK_ORB;
class Group_impl : public POA_HEXABLOCK_ORB::Group,
		   public SALOME::GenericObj_i
{
  public:
    Group_impl(HEXA_NS::Group *ptrCpp);
    HEXA_NS::Group* GetImpl() throw (SALOME::SALOME_Exception);

    char* getName() throw (SALOME::SALOME_Exception);
    void setName(const char* name) throw (SALOME::SALOME_Exception);
    GroupKind getKind() throw (SALOME::SALOME_Exception);
    ::CORBA::Long addElement(Element_ptr e) throw (SALOME::SALOME_Exception);
    ::CORBA::Long countElement() throw (SALOME::SALOME_Exception);
    Element_ptr getElement(::CORBA::Long index) throw (SALOME::SALOME_Exception);
    ::CORBA::Long removeElement(::CORBA::Long index) throw (SALOME::SALOME_Exception);
    void clearElement() throw (SALOME::SALOME_Exception);

private:
  HEXA_NS::Group *_group_cpp;
};


// class HexaGroup_impl : public Group_impl {};
// class QuadGroup_impl : public Group_impl {};
// class EdgeGroup_impl : public Group_impl {};
// class HexaNodeGroup_impl : public Group_impl {};
// class QuadNodeGroup_impl : public Group_impl {};
// class EdgeNodeGroup_impl : public Group_impl {};
// class VertexNodeGroup_impl : public Group_impl {};


#endif
