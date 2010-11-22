#include <cppunit/extensions/HelperMacros.h>
#include "hexa_base.hxx"
#include "HexEdge.hxx"
#include "HexEdgeTest.hxx"

CPPUNIT_TEST_SUITE_REGISTRATION( EdgeTest );


EdgeTest::EdgeTest(void)
{
  _myHexEngine = new HEXA_NS::Hex();
  _myDoc= _myHexEngine->addDocument();
}

EdgeTest::~EdgeTest(void)
{
  delete _myDoc;
  delete _myHexEngine; //CS_TODO : Bug
}


void EdgeTest::TestGetVertex(void)
{
  HEXA_NS::Vertex *myVertex0 = NULL;
  HEXA_NS::Vertex *myVertex1 = NULL;

  // first vertex of the edge
//   double x0Value = 1.;
//   double y0Value = 5.;
//   double z0Value = 3.;
//   myVertex0 = _myDoc->addVertex( x0Value, y0Value, z0Value );
  myVertex0 = _myDoc->addVertex( 1., 5., 3. );

  // second vertex of the edge
  myVertex1 = _myDoc->addVertex( 8., 4., 6.);

  HEXA_NS::Edge *myEdge = _myDoc->addEdge( myVertex0, myVertex1 );

  // Testing first Vertex
  HEXA_NS::Vertex *docVertex0 = myEdge->getVertex(0);
  CPPUNIT_ASSERT(  docVertex0->getX() == myVertex0->getX() );
  CPPUNIT_ASSERT(  docVertex0->getY() == myVertex0->getY() );
  CPPUNIT_ASSERT(  docVertex0->getZ() == myVertex0->getZ() );

  // Testing second Vertex
  HEXA_NS::Vertex *docVertex1 = myEdge->getVertex(1);
  CPPUNIT_ASSERT(  docVertex1->getX() == myVertex1->getX() );
  CPPUNIT_ASSERT(  docVertex1->getY() == myVertex1->getY() );
  CPPUNIT_ASSERT(  docVertex1->getZ() == myVertex1->getZ() );
// 	CPPUNIT_ASSERT_ASSERTION_FAIL( CPPUNIT_ASSERT( myClassa.getVar() == testValue ) );
}
