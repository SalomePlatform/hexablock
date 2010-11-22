using namespace std;
#include "HEXABLOCK.hxx"
#include "utilities.h"

#include <string>

#include "hexa_base.hxx"
#include "HexEltBase.hxx"
#include "HexGroups.hxx"

#include "HexElements_impl.hxx"

#include "HexVertex_impl.hxx"
#include "HexEdge_impl.hxx"
#include "HexQuad_impl.hxx"
#include "HexHexa_impl.hxx"
#include "HexGroup_impl.hxx"

Group_impl::Group_impl( HEXA_NS::Group *ptrCpp ):_group_cpp(ptrCpp)
{
}

HEXA_NS::Group* Group_impl::GetImpl() throw (SALOME::SALOME_Exception)
{
  return _group_cpp;
}

char* Group_impl::getName() throw (SALOME::SALOME_Exception)
{
  _group_cpp->getName();
}

void Group_impl::setName(const char* name) throw (SALOME::SALOME_Exception)
{
  _group_cpp->setName( name );
}


GroupKind Group_impl::getKind() throw (SALOME::SALOME_Exception)
{
  HEXA_NS::EnumGroup k = _group_cpp->getKind();

  switch (k){
    case HEXA_NS::HexaCell:  return HEXA_GROUP;
    case HEXA_NS::QuadCell:  return QUAD_GROUP;
    case HEXA_NS::EdgeCell:  return EDGE_GROUP;
    case HEXA_NS::HexaNode:  return HEXANODE_GROUP;
    case HEXA_NS::QuadNode:  return QUADNODE_GROUP;
    case HEXA_NS::EdgeNode:  return EDGENODE_GROUP;
    case HEXA_NS::Vertex_Node:  return VERTEXNODE_GROUP;
    default : ASSERT(false);
  }
}


::CORBA::Long  Group_impl::addElement(Element_ptr eIn) throw (SALOME::SALOME_Exception)
{
  ::CORBA::Long ok;
  Vertex_impl* vInServant = ::DownCast<Vertex_impl*>( eIn );
  Edge_impl*   eInServant = ::DownCast<Edge_impl*>( eIn );
  Quad_impl*   qInServant = ::DownCast<Quad_impl*>( eIn );
  Hexa_impl*   hInServant = ::DownCast<Hexa_impl*>( eIn );

  ASSERT( vInServant or eInServant or qInServant or hInServant );
  HEXA_NS::EltBase* e = NULL;
  if ( vInServant ) {
    e = vInServant->GetImpl();
  } else if ( eInServant ) {
    e = eInServant->GetImpl();
  } else if ( qInServant ) {
    e = qInServant->GetImpl();
  } else if ( hInServant ) {
    e = hInServant->GetImpl();
  }
  ok = _group_cpp->addElement(e);

  return ok;
}


::CORBA::Long Group_impl::countElement() throw (SALOME::SALOME_Exception)
{
  return _group_cpp->countElement();
}


Element_ptr Group_impl::getElement(::CORBA::Long index) throw (SALOME::SALOME_Exception)
{
  HEXA_NS::EltBase* elt = _group_cpp->getElement(index);

  HEXA_NS::Vertex* v = dynamic_cast<HEXA_NS::Vertex*>(elt);
  HEXA_NS::Edge* e   = dynamic_cast<HEXA_NS::Edge*>(elt);
  HEXA_NS::Quad* q   = dynamic_cast<HEXA_NS::Quad*>(elt);
  HEXA_NS::Hexa* h   = dynamic_cast<HEXA_NS::Hexa*>(elt);

  ASSERT( v or e or q or h );

  if (v){
    Vertex_impl* servantCorba = new Vertex_impl(v);
    return servantCorba->_this();
  } else if (e){
    Edge_impl*   servantCorba = new Edge_impl(e);
    return servantCorba->_this();
  } else if (q){
    Quad_impl*   servantCorba = new Quad_impl(q);
    return servantCorba->_this();
  } else if (h){
    Hexa_impl*   servantCorba = new Hexa_impl(h);
    return servantCorba->_this();
  } else {
    return Element::_nil();
  }
}


::CORBA::Long Group_impl::removeElement(::CORBA::Long index) throw (SALOME::SALOME_Exception)
{
  return _group_cpp->removeElement(index);
}

void Group_impl::clearElement() throw (SALOME::SALOME_Exception)
{
  _group_cpp->clearElement();
}

