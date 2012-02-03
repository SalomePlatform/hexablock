//  Copyright (C) 2009-2011  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
using namespace std;
#include "HEXABLOCK.hxx"
#include "utilities.h"

#include <string>

#include "hexa_base.hxx"
#include "HexQuad_impl.hxx"
#include "HexVertex_impl.hxx"
#include "HexEdge_impl.hxx"
#include "HexShape.hxx"

Quad_impl::Quad_impl( HEXA_NS::Quad *ptrCpp ):_quad_cpp(ptrCpp)
{
}

HEXA_NS::Quad* Quad_impl::GetImpl()
  throw (SALOME::SALOME_Exception)
{
  return _quad_cpp;
}

Edge_ptr Quad_impl::getEdge(::CORBA::Long n)
  throw (SALOME::SALOME_Exception)
{
  Edge_ptr result = Edge::_nil();

  HEXA_NS::Edge* e = _quad_cpp->getEdge( n );
  if ( e != NULL ){
    Edge_impl* servantCorba = new Edge_impl(e);
    result = servantCorba->_this();
  }
  return result;
}


Vertex_ptr Quad_impl::getVertex(::CORBA::Long n)
   throw (SALOME::SALOME_Exception)
{
  Vertex_ptr result = Vertex::_nil();

  HEXA_NS::Vertex* v = _quad_cpp->getVertex( n );
  if ( v != NULL ){
    Vertex_impl* servantCorba = new Vertex_impl(v);
    result = servantCorba->_this();
  }
  return result;
}




::CORBA::Long Quad_impl::addAssociation( GEOM::GEOM_Object_ptr geom_object_2D)
  throw (SALOME::SALOME_Exception)
{
  ::CORBA::Long ok;

  TopoDS_Shape aShape = HEXABLOCK_Gen_i::GetHEXABLOCKGen()->geomObjectToShape( geom_object_2D );
  string      strBrep = shape2string( aShape );

  CORBA::String_var anIOR = HEXABLOCK_Gen_i::GetORB()->object_to_string( geom_object_2D );
  HEXA_NS::Shape* s = new HEXA_NS::Shape( strBrep );
  s->ior     = anIOR.in();
  s->ident   = geom_object_2D->GetStudyEntry();

  ok = _quad_cpp->addAssociation( s );
//   _associations.push_back(GEOM::GEOM_Object::_duplicate( geom_object_2D ));
  return ok;
}

void Quad_impl::clearAssociation()
      throw (SALOME::SALOME_Exception)
{
  _quad_cpp->clearAssociation ();
}



GEOM::ListOfGO* Quad_impl::getAssociations() //CS_NOT_SPEC
  throw (SALOME::SALOME_Exception)
// {
//   GEOM::ListOfGO* result = new GEOM::ListOfGO;
//   result->length( _associations.size() );
// 
//   CORBA::ULong i = 0;
//   for ( std::vector<GEOM::GEOM_Object_ptr>::const_iterator iter = _associations.begin();
// 	iter != _associations.end();
//         ++iter){
// //       (*result)[i++] = *iter;
//       (*result)[i++] = GEOM::GEOM_Object::_duplicate( *iter );
//   }
//   return result;
// }
{
  TopoDS_Shape aShape;
  const std::vector<HEXA_NS::Shape*> shapes = _quad_cpp->getAssociations();

  GEOM::ListOfGO* result = new GEOM::ListOfGO;
  result->length( shapes.size() );

  CORBA::Object_var     corbaObj;
  GEOM::GEOM_Object_var geomObj;
  HEXABLOCK_ORB::EdgeAssociation assoc;
  CORBA::ULong i = 0;
  for ( std::vector<HEXA_NS::Shape*>::const_iterator iter = shapes.begin();
	iter != shapes.end();
        ++iter ){
      if ( !(*iter)->ior.empty() ){ // geom object from current session
        corbaObj = HEXABLOCK_Gen_i::GetORB()->string_to_object( (*iter)->ior.c_str() );
        if ( !CORBA::is_nil( corbaObj ) ){
          geomObj = GEOM::GEOM_Object::_narrow( corbaObj );
        }
      } else { // no geom object => we have to built it
        geomObj = HEXABLOCK_Gen_i::GetHEXABLOCKGen()->brepToGeomObject( (*iter)->getBrep() );
      }
      (*result)[ i++ ] = geomObj._retn();
  }

  return result;
}








// void Quad_impl::setAssociation(GEOM::GEOM_Object_ptr geom_object_2D) throw (SALOME::SALOME_Exception)
// {
// }
// 
// GEOM::GEOM_Object_ptr Quad_impl::getAssociation() throw (SALOME::SALOME_Exception)
// {
// }
// 
// void Quad_impl::removeAssociation() throw (SALOME::SALOME_Exception)
// {
// }

void Quad_impl::setScalar( ::CORBA::Double val )throw (SALOME::SALOME_Exception)
{
  _quad_cpp->setScalar(val);
}

void Quad_impl::dump() throw (SALOME::SALOME_Exception)
{
  _quad_cpp->dump();
}


void Quad_impl::printName() throw (SALOME::SALOME_Exception)
{
  _quad_cpp->printName();
}

char* Quad_impl::getName() throw (SALOME::SALOME_Exception)
{
 char name[12];
 return CORBA::string_dup( _quad_cpp->getName(name) );
}
// ========================================================= setName
void Quad_impl::setName(const char* name) 
     throw (SALOME::SALOME_Exception)
{
  _quad_cpp->setName (name);
}
