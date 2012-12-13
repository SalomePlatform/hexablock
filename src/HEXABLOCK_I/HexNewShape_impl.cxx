// Copyright (C) 2009-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "HEXABLOCK.hxx"
#include "utilities.h"

#include <string>

#include "HexNewShape_impl.hxx"

// ============================================================== Constructeur
NewShape_impl::NewShape_impl( HEXA_NS::NewShape *ptrCpp )
{
   _shape_cpp = ptrCpp;
}
// ============================================================== GetImpl
HEXA_NS::NewShape* NewShape_impl::GetImpl()
    throw (SALOME::SALOME_Exception)
{
  return _shape_cpp;
}
// ========================================================= countVertex
::CORBA::Long NewShape_impl::countVertex ()
              throw (SALOME::SALOME_Exception)
{
   return _shape_cpp->countVertex ();
}
// ========================================================= countEdge
::CORBA::Long NewShape_impl::countEdge ()
              throw (SALOME::SALOME_Exception)
{
   return _shape_cpp->countEdge ();
}
// ========================================================= countFace
::CORBA::Long NewShape_impl::countFace ()
              throw (SALOME::SALOME_Exception)
{
   return _shape_cpp->countFace ();
}
// ---------------------------------------------------------------------
// ========================================================= getVertex
::CORBA::Long NewShape_impl::getVertex (::CORBA::Long nro)
              throw (SALOME::SALOME_Exception)
{
   return _shape_cpp->getVertex (nro);
}
// ========================================================= getEdge
::CORBA::Long NewShape_impl::getEdge (::CORBA::Long nro)
              throw (SALOME::SALOME_Exception)
{
   return _shape_cpp->getEdge (nro);
}
// ========================================================= getFace
::CORBA::Long NewShape_impl::getFace (::CORBA::Long nro)
              throw (SALOME::SALOME_Exception)
{
   return _shape_cpp->getFace (nro);
}
// ---------------------------------------------------------------------
// ========================================================= getNameVertex
char* NewShape_impl::getNameVertex (::CORBA::Long nro)
            throw (SALOME::SALOME_Exception)
{
   return CORBA::string_dup (_shape_cpp->getNameVertex (nro));
}
// ========================================================= getNameEdge
char* NewShape_impl::getNameEdge (::CORBA::Long nro)
            throw (SALOME::SALOME_Exception)
{
   return CORBA::string_dup (_shape_cpp->getNameEdge (nro));
}
// ========================================================= getNameFace
char* NewShape_impl::getNameFace (::CORBA::Long nro)
            throw (SALOME::SALOME_Exception)
{
   return CORBA::string_dup (_shape_cpp->getNameFace (nro));
}
// ========================================================= dump
void NewShape_impl::dump() throw (SALOME::SALOME_Exception)
{

}
// ========================================================= setName
void NewShape_impl::printName() throw (SALOME::SALOME_Exception)
{
  _shape_cpp->printName();
}
// ========================================================= setName
char* NewShape_impl::getName() throw (SALOME::SALOME_Exception)
{
  return CORBA::string_dup( _shape_cpp->getName() );
}
// ========================================================= setName
void NewShape_impl::setName(const char* name) 
     throw (SALOME::SALOME_Exception)
{
  _shape_cpp->setName (name);
}
