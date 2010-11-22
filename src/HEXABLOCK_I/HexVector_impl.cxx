using namespace std;
#include "HEXABLOCK.hxx"
#include "utilities.h"

#include <string>

#include "hexa_base.hxx"
#include "HexVector_impl.hxx"

using namespace HEXABLOCK_ORB;

Vector_impl::Vector_impl( HEXA_NS::Vector *ptrCpp ):_vector_cpp(ptrCpp)
{
}

HEXA_NS::Vector* Vector_impl::GetImpl() throw (SALOME::SALOME_Exception)
{
  return _vector_cpp;
}

::CORBA::Double Vector_impl::getDX() throw (SALOME::SALOME_Exception)
{
  ::CORBA::Double dx = _vector_cpp->getDx();
  return dx;
}
::CORBA::Double Vector_impl::getDY() throw (SALOME::SALOME_Exception)
{
  ::CORBA::Double dy = _vector_cpp->getDy();
  return dy;
}
::CORBA::Double Vector_impl::getDZ() throw (SALOME::SALOME_Exception)
{
  ::CORBA::Double dz = _vector_cpp->getDz();
  return dz;
}


void Vector_impl::dump() throw (SALOME::SALOME_Exception)
{
  _vector_cpp->dump();
}


void Vector_impl::printName() throw (SALOME::SALOME_Exception)
{
  _vector_cpp->printName();
}
