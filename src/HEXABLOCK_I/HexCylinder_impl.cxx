using namespace std;
#include "HEXABLOCK.hxx"
#include "utilities.h"

#include <string>

#include "hexa_base.hxx"
#include "HexCylinder_impl.hxx"
#include "HexVertex_impl.hxx"
#include "HexVector_impl.hxx"

Cylinder_impl::Cylinder_impl( HEXA_NS::Cylinder *ptrCpp ):_cylinder_cpp(ptrCpp)
{
}

HEXA_NS::Cylinder* Cylinder_impl::GetImpl()
  throw (SALOME::SALOME_Exception)
{
  return _cylinder_cpp;
}


Vertex_ptr Cylinder_impl::getBase()
  throw (SALOME::SALOME_Exception)
{
  Vertex_ptr result = Vertex::_nil();

  HEXA_NS::Vertex* v = _cylinder_cpp->getBase();
  if ( v!= NULL ){
    Vertex_impl* servantCorba = new Vertex_impl(v);
    result = servantCorba->_this();
  }

  return result;
}

Vector_ptr Cylinder_impl::getDirection()
  throw (SALOME::SALOME_Exception)
{
  Vector_ptr result = Vector::_nil();

  HEXA_NS::Vector* v = _cylinder_cpp->getDirection();
  if ( v!= NULL ){
    Vector_impl* servantCorba = new Vector_impl(v);
    result = servantCorba->_this();
  }
  return result;
}


::CORBA::Double Cylinder_impl::getRadius()
  throw (SALOME::SALOME_Exception)
{
 ::CORBA::Double d = _cylinder_cpp->getRadius();
  return d;
}


::CORBA::Double Cylinder_impl::getHeight()
  throw (SALOME::SALOME_Exception)
{
 ::CORBA::Double d = _cylinder_cpp->getHeight();
  return d;
}


void Cylinder_impl::dump() throw (SALOME::SALOME_Exception)
{
  _cylinder_cpp->dump();
}


void Cylinder_impl::printName() throw (SALOME::SALOME_Exception)
{
  _cylinder_cpp->printName();
}

// ::CORBA::Long Cylinder_impl::saveVtk(const char* fname) throw (SALOME::SALOME_Exception)
// {
//   ::CORBA::Long ret = _cylinder_cpp->saveVtk( fname );
//   return ret;
// }


