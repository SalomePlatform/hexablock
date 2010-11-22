#ifndef __VertexTest__
#define __VertexTest__

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "hexa_base.hxx"
#include "Hex.hxx"
#include "HexDocument.hxx"

class VertexTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( VertexTest );
  CPPUNIT_TEST( TestGetX );
  CPPUNIT_TEST( TestGetY );
  CPPUNIT_TEST( TestGetZ );
  CPPUNIT_TEST_SUITE_END();
  
  public:
    VertexTest(void);
    ~VertexTest(void);
    //~ Call before tests
    void setUp(void) {}
    //~ Call after tests
    void tearDown(void) {}
    
    void TestGetX(void);
    void TestGetY(void);
    void TestGetZ(void);
  private:
    HEXA_NS::Hex *_myHexEngine;
    HEXA_NS::Document *_myDoc;

};

#endif
