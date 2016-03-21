// Copyright (C) 2009-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

#include <cppunit/extensions/HelperMacros.h>
#include "hexa_base.hxx"
#include "HexVertex.hxx"
#include "HexVertexTest.hxx"

CPPUNIT_TEST_SUITE_REGISTRATION( VertexTest );


VertexTest::VertexTest(void)
{
  _myHexEngine = new HEXA_NS::Hex();
  _myDoc= _myHexEngine->addDocument();
}

VertexTest::~VertexTest(void)
{
  delete _myDoc;
  delete _myHexEngine; //CS_TODO : Bug
}


void VertexTest::TestGetX(void)
{
  double xValue = 1.;
//   std::cout<<"Hellllllooooooo TestGetX";

//   HEXA_NS::Vertex myVertex( xValue, 8. ,4. );
  HEXA_NS::Vertex *myVertex = _myDoc->addVertex( xValue, 8. ,4. );

  CPPUNIT_ASSERT(  myVertex->getX() == xValue );
// 	CPPUNIT_ASSERT_ASSERTION_FAIL( CPPUNIT_ASSERT( myClassa.getVar() == testValue ) );
}

void VertexTest::TestGetY(void)
{
  double yValue = 8.;
//   HEXA_NS::Vertex myVertex( 1. ,yValue ,4. );
  HEXA_NS::Vertex *myVertex = _myDoc->addVertex( 1. ,yValue ,4. );

  CPPUNIT_ASSERT(  myVertex->getY() == yValue );
// 	CPPUNIT_ASSERT_ASSERTION_FAIL( CPPUNIT_ASSERT( myClassa.getVar() == testValue ) );
}


void VertexTest::TestGetZ(void)
{
  double zValue = 1.;

//   HEXA_NS::Vertex myVertex( 1. ,8. ,zValue );
  HEXA_NS::Vertex *myVertex = _myDoc->addVertex( 1. ,8. ,zValue );

  CPPUNIT_ASSERT(  myVertex->getZ() == zValue );
// 	CPPUNIT_ASSERT_ASSERTION_FAIL( CPPUNIT_ASSERT( myClassa.getVar() == testValue ) );
}
