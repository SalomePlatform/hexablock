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
using namespace std;
#include "HEXABLOCK.hxx"
#include "utilities.h"

#include <string>

#include "hexa_base.hxx"
#include "HexVertex_impl.hxx"
#include "HexShape.hxx"

Vertex_impl::Vertex_impl( HEXA_NS::Vertex *ptrCpp ):
_vertex_cpp( ptrCpp )
// _association( GEOM::GEOM_Object::_nil() )
{
}

HEXA_NS::Vertex* Vertex_impl::GetImpl()
{
  return _vertex_cpp;
}

CORBA::Double Vertex_impl::getX() throw(SALOME::SALOME_Exception)
{
  return _vertex_cpp->getX();
}

CORBA::Double Vertex_impl::getY() throw(SALOME::SALOME_Exception)
{
  return _vertex_cpp->getY();
}

CORBA::Double Vertex_impl::getZ() throw(SALOME::SALOME_Exception)
{
  return _vertex_cpp->getZ();
}


void Vertex_impl::setX( CORBA::Double x ) throw(SALOME::SALOME_Exception)
{
  _vertex_cpp->setX(x);
}
void Vertex_impl::setY(  CORBA::Double y ) throw(SALOME::SALOME_Exception)
{
  _vertex_cpp->setY(y);
}

void Vertex_impl::setZ(  CORBA::Double z ) throw(SALOME::SALOME_Exception)
{
  _vertex_cpp->setZ(z);
}


void Vertex_impl::setAssociation(GEOM::GEOM_Object_ptr geom_object_vertex)
  throw (SALOME::SALOME_Exception)
{
  TopoDS_Shape shape = HEXABLOCK_Gen_i::GetHEXABLOCKGen()->geomObjectToShape(geom_object_vertex);

//   HEXABLOCK::HEXABLOCK* gen = HEXABLOCK::GetHEXABLOCKGen();
//   TopoDS_Shape shape = gen->geomObjectToShape(geom_object_vertex);

  string strBrep = shape2string( shape );
  CORBA::String_var anIOR = HEXABLOCK_Gen_i::GetORB()->object_to_string( geom_object_vertex );
  HEXA_NS::Shape* msh = new HEXA_NS::Shape( strBrep );
     //geom_object_vertex->GetEntry()

  msh->setIor   (anIOR.in()); 
  msh->setIdent (geom_object_vertex->GetStudyEntry()); 
  _vertex_cpp->setAssociation(msh);

//   _association = GEOM::GEOM_Object::_duplicate( geom_object_vertex );
}


GEOM::GEOM_Object_ptr Vertex_impl::getAssociation()
  throw (SALOME::SALOME_Exception)
{
//   GEOM::GEOM_Object_var result = GEOM::GEOM_Object::_nil();
//   GEOM::GEOM_Object_ptr result = GEOM::GEOM_Object::_nil();
  GEOM::GEOM_Object_var geomObj; // = new GEOM::GEOM_Object;
  CORBA::Object_var corbaObj;

  HEXA_NS::Shape* msh = _vertex_cpp->getAssociation();
  if (msh == NULL)
     return geomObj._retn();
                     // geom object from current session
  string ior = msh->getIor();
  if (NOT ior.empty() )
     {
     corbaObj = HEXABLOCK_Gen_i::GetORB()->string_to_object( ior.c_str() );
     if (NOT CORBA::is_nil( corbaObj ) )
        {
        geomObj = GEOM::GEOM_Object::_narrow( corbaObj );
        }
      }
  else 
        // no geom object => we have to built it
      {
      geomObj = HEXABLOCK_Gen_i::GetHEXABLOCKGen()->brepToGeomObject( msh->getBrep() );
      }

  return geomObj._retn();
}


void Vertex_impl::clearAssociation()
      throw (SALOME::SALOME_Exception)
{
  _vertex_cpp->clearAssociation ();
}


void Vertex_impl::setColor (::CORBA::Double val)
     throw (SALOME::SALOME_Exception)
{
  _vertex_cpp->setColor (val);
}

void Vertex_impl::setScalar( ::CORBA::Double val )throw (SALOME::SALOME_Exception)
{
  _vertex_cpp->setScalar(val);
}

void Vertex_impl::dump() throw (SALOME::SALOME_Exception)
{
  _vertex_cpp->dump();
}


void Vertex_impl::printName()throw (SALOME::SALOME_Exception)
{
  _vertex_cpp->printName();
}


char* Vertex_impl::getName() throw (SALOME::SALOME_Exception)
{
  return CORBA::string_dup( _vertex_cpp->getName() );
}
// ========================================================= setName
void Vertex_impl::setName(const char* name) 
     throw (SALOME::SALOME_Exception)
{
  _vertex_cpp->setName (name);
}
