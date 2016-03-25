# Copyright (C) 2012-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

# --- options ---
# additional include directories
INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/HEXABLOCK
  ${PROJECT_BINARY_DIR}/idl
  ${CPPUNIT_INCLUDE_DIR}
)

# libraries to link to
SET(_link_LIBRARIES
  HEXABLOCKEngine
  ${CPPUNIT_LIBRARIES}
)

ADD_DEFINITIONS(${CPPUNIT_DEFINITIONS})

# --- sources ---

# sources / static
SET(HEXABLOCKTest_SOURCES
  test_unit.hxx
  test_hexa1.cxx
  test_tools.cxx
  test_quads.cxx
  ModelTest.cxx 
  ModelTest.hxx
)

# bin programs
SET(Test_HEXABLOCK_bin_SOURCES
  Test_HEXABLOCK.cxx
)

# --- rules ---

ADD_LIBRARY(HEXABLOCKTest ${HEXABLOCKTest_SOURCES})
TARGET_LINK_LIBRARIES(HEXABLOCKTest ${_link_LIBRARIES} )
INSTALL(TARGETS HEXABLOCKTest EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

ADD_EXECUTABLE(Test_HEXABLOCK ${Test_HEXABLOCK_bin_SOURCES})
TARGET_LINK_LIBRARIES(Test_HEXABLOCK ${_link_LIBRARIES} HEXABLOCKTest)
INSTALL(TARGETS Test_HEXABLOCK EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_BINS})
