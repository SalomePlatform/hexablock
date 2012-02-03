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
#ifndef __HexElements_impl__
#define __HexElements_impl__

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(Elements)
// #include CORBA_SERVER_HEADER(Vertex)
// #include CORBA_SERVER_HEADER(Edge)
// #include CORBA_SERVER_HEADER(Quad)
// #include CORBA_SERVER_HEADER(Hexa)

#include "SALOME_GenericObj_i.hh"

#include "hexa_base.hxx"
#include "HexElements.hxx"

using namespace HEXABLOCK_ORB;

class Elements_impl : public POA_HEXABLOCK_ORB::Elements,
		      public SALOME::GenericObj_i
{
public:
    Elements_impl(HEXA_NS::Elements *ptrCpp);
    HEXA_NS::Elements* GetImpl();

    ::CORBA::Long countVertex() throw (SALOME::SALOME_Exception);
    ::CORBA::Long countEdge() throw (SALOME::SALOME_Exception);
    ::CORBA::Long countQuad() throw (SALOME::SALOME_Exception);
    ::CORBA::Long countHexa() throw (SALOME::SALOME_Exception);
    ::CORBA::Long findVertex( Vertex_ptr p ) throw (SALOME::SALOME_Exception);
    Vertex_ptr getVertex(::CORBA::Long n) throw (SALOME::SALOME_Exception);
    Edge_ptr getEdge(::CORBA::Long n) throw (SALOME::SALOME_Exception);
    Quad_ptr getQuad(::CORBA::Long n) throw (SALOME::SALOME_Exception);
    Hexa_ptr getHexa(::CORBA::Long n) throw (SALOME::SALOME_Exception);

    Vertex_ptr getVertexIJK(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z) throw (SALOME::SALOME_Exception);

    Edge_ptr getEdgeI(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z) throw (SALOME::SALOME_Exception);
    Edge_ptr getEdgeJ(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z) throw (SALOME::SALOME_Exception);
    Edge_ptr getEdgeK(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z) throw (SALOME::SALOME_Exception);

    Quad_ptr getQuadIJ(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z) throw (SALOME::SALOME_Exception);
    Quad_ptr getQuadIK(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z) throw (SALOME::SALOME_Exception);
    Quad_ptr getQuadJK(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z) throw (SALOME::SALOME_Exception);

    Hexa_ptr getHexaIJK(::CORBA::Long x, ::CORBA::Long y, ::CORBA::Long z) throw (SALOME::SALOME_Exception);


//     Quad_ptr getQuad1(EnumCoord dir, ::CORBA::Long nx, ::CORBA::Long ny, ::CORBA::Long nz)
//       throw (SALOME::SALOME_Exception);
// 
//     Edge_ptr getEdge1(EnumCoord dir, ::CORBA::Long nx, ::CORBA::Long ny, ::CORBA::Long nz)
//       throw (SALOME::SALOME_Exception);

    ::CORBA::Long saveVtk(const char* fname) throw (SALOME::SALOME_Exception);
    void dump() throw (SALOME::SALOME_Exception);
    void printName() throw (SALOME::SALOME_Exception);
    char* getName() throw (SALOME::SALOME_Exception);
    void  setName (const char* name) throw (SALOME::SALOME_Exception);

private:
  HEXA_NS::Elements *_elements_cpp;
//   void *_elements_cpp;

};

#endif
