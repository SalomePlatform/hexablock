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
#include "HexEdge_impl.hxx"
#include "HexShape.hxx"

using namespace HEXABLOCK_ORB;

Edge_impl::Edge_impl( HEXA_NS::Edge *ptrCpp ):
_edge_cpp(ptrCpp)
{
}

HEXA_NS::Edge* Edge_impl::GetImpl()
  throw (SALOME::SALOME_Exception)
{
  return _edge_cpp;
}


::CORBA::Boolean Edge_impl::getWay() throw (SALOME::SALOME_Exception)
{
  return _edge_cpp->getWay();
}


Vertex_ptr Edge_impl::getVertex(::CORBA::Long n)
  throw (SALOME::SALOME_Exception)
{
  Vertex_ptr result = Vertex::_nil();

  HEXA_NS::Vertex* v = _edge_cpp->getVertex(n);
  if ( v!= NULL ){
    Vertex_impl* servantCorba = new Vertex_impl(v);
    result = servantCorba->_this();
  }
  return result ;
}


void Edge_impl::dump() throw (SALOME::SALOME_Exception)
{
  _edge_cpp->dump();
}


void Edge_impl::printName()throw (SALOME::SALOME_Exception)
{
  _edge_cpp->printName();
}

char* Edge_impl::getName() throw (SALOME::SALOME_Exception)
{
  return CORBA::string_dup( _edge_cpp->getName() );
}

// ========================================================= setName
void Edge_impl::setName(const char* name) 
     throw (SALOME::SALOME_Exception)
{
  _edge_cpp->setName (name);
}

void Edge_impl::setColor (::CORBA::Double val)
     throw (SALOME::SALOME_Exception)
{
  _edge_cpp->setColor (val);
}

void Edge_impl::setScalar( ::CORBA::Double val )throw (SALOME::SALOME_Exception)
{
  _edge_cpp->setScalar(val);
}


::CORBA::Long Edge_impl::addAssociation (GEOM::GEOM_Object_ptr geom_object_1D, 
                                         double debut, double fin )
  throw (SALOME::SALOME_Exception)
{
  ::CORBA::Long ok;
  TopoDS_Shape aShape = HEXABLOCK_Gen_i::GetHEXABLOCKGen()->geomObjectToShape( geom_object_1D );
  string      strBrep = shape2string( aShape );
  CORBA::String_var anIOR = HEXABLOCK_Gen_i::GetORB()->object_to_string( geom_object_1D );
  HEXA_NS::Shape* hshape = new HEXA_NS::Shape( strBrep );

  hshape->setBounds (debut, fin);
  hshape->setIdent  (geom_object_1D->GetStudyEntry());
  hshape->setIor    (anIOR.in());        //geom_object_1D->GetStudyEntry(); GetEntry()
  ok  = _edge_cpp->addAssociation( hshape );

  return ok;
}

void Edge_impl::clearAssociation()
      throw (SALOME::SALOME_Exception)
{
  _edge_cpp->clearAssociation ();
}



EdgeAssociations* Edge_impl::getAssociations ()
  throw (SALOME::SALOME_Exception)
{
  HEXABLOCK_ORB::EdgeAssociations* result = new HEXABLOCK_ORB::EdgeAssociations;
//   HEXABLOCK_ORB::GEOM_Object_ptr ge;
//   GEOM::GEOM_Object_ptr ge;
  TopoDS_Shape aShape;
  const std::vector<HEXA_NS::Shape*> shapes = _edge_cpp->getAssociations();

  result->length( shapes.size() );
  HEXABLOCK_ORB::EdgeAssociation assoc;
  CORBA::ULong i = 0;
  CORBA::Object_var corbaObj;
  GEOM::GEOM_Object_var geomObj;

  int nbass = shapes.size();
  for (int ns =0 ; ns < nbass ; ns++)
      {
      Hex::Shape* msh = shapes[ns];
      string      ior = msh->getIor();
      assoc.debut     = msh->getStart();
      assoc.fin       = msh->getEnd();

      if ( NOT ior.empty() )  // geom object from current session
         {
         corbaObj = HEXABLOCK_Gen_i::GetORB()->string_to_object( ior.c_str() );
         if (NOT CORBA::is_nil( corbaObj ) )
            {
            geomObj = GEOM::GEOM_Object::_narrow( corbaObj );
            assoc.geomObj = geomObj._retn();
            }
         } 
      else   // no geom object => we have to built it
         { 
         assoc.geomObj = HEXABLOCK_Gen_i::GetHEXABLOCKGen()->brepToGeomObject(msh->getBrep() );
         }

      (*result)[ i++ ] = assoc;
  }
  return result;
}
