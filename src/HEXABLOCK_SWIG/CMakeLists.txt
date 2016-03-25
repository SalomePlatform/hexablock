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

INCLUDE(${SWIG_USE_FILE})

# --- options ---
# additional include directories
INCLUDE_DIRECTORIES(
  ${KERNEL_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
  ${CAS_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
  ${GEOM_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/src/HEXABLOCK
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${KERNEL_DEFINITIONS}
  ${CAS_DEFINITIONS}
  ${BOOST_DEFINITIONS}
)

# libraries to link to
SET(_link_LIBRARIES
  ${PYTHON_LIBRARIES}
  ${GEOM_GEOMBase}
  ${CAS_TKBRep}
  ${CAS_TKPrim}
  ${CAS_TKG3d}
  ${CAS_TKG2d}
  ${CAS_TKTopAlgo}
  ${CAS_TKGeomBase}
  ${CAS_TKGeomAlgo}
  ${CAS_TKBool}
  ${CAS_TKBO}
  ${KERNEL_SALOMELocalTrace}
  ${KERNEL_SALOMEBasics}
  HEXABLOCKEngine
)

# swig flags
SET_SOURCE_FILES_PROPERTIES(hexablock_swig.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(hexablock_swig.i PROPERTIES SWIG_DEFINITIONS "-shadow")

# --- sources ---

# sources / static
SET(_hexablock_swig_SOURCES
  hexablock_swig_wrap.cc 
)

# --- scripts ---

# scripts / static
SET(_bin_SCRIPTS
  hexablock_swig.py 
  hexablock.py
)

# --- rules ---

ADD_LIBRARY(hexablock_swig_lib ${_hexablock_swig_SOURCES})
TARGET_LINK_LIBRARIES(hexablock_swig_lib ${_link_LIBRARIES} )

SWIG_ADD_MODULE(hexablock_swig python ${_hexablock_swig_SOURCES})
SWIG_LINK_LIBRARIES(hexablock_swig ${_link_LIBRARIES} ${hexablock_swig_lib})
INSTALL(TARGETS _hexablock_swig DESTINATION ${SALOME_INSTALL_LIBS})

SALOME_INSTALL_SCRIPTS("${_bin_SCRIPTS}" ${SALOME_INSTALL_SCRIPT_PYTHON})
