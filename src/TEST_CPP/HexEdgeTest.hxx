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

#ifndef __EdgeTest__
#define __EdgeTest__

#include "HexTest.hxx"

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "hexa_base.hxx"
#include "Hex.hxx"
#include "HexDocument.hxx"

class HEXABLOCKTEST_EXPORT EdgeTest : public CppUnit::TestFixture
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
