#ifndef __EdgeTest__
#define __EdgeTest__

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "hexa_base.hxx"
#include "Hex.hxx"
#include "HexDocument.hxx"

class EdgeTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( EdgeTest );
  CPPUNIT_TEST( TestGetVertex );
  CPPUNIT_TEST_SUITE_END();
  
  public:
    EdgeTest(void);
    ~EdgeTest(void);

    //~ Call before tests
    void setUp(void) {}
    //~ Call after tests
    void tearDown(void) {}
    void TestGetVertex(void);

  private:
    HEXA_NS::Hex *_myHexEngine;
    HEXA_NS::Document *_myDoc;

};

#endif
