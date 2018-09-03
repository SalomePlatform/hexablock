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
  ${OpenCASCADE_INCLUDE_DIR}
  ${VTK_INCLUDE_DIRS}
  ${GEOM_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/src/HEXABLOCK
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${KERNEL_DEFINITIONS}
  ${OpenCASCADE_DEFINITIONS}
  ${BOOST_DEFINITIONS}
)

# libraries to link to
SET(_link_LIBRARIES
  ${PYTHON_LIBRARIES}
  ${GEOM_GEOMBase}
  ${OpenCASCADE_ModelingData_LIBRARIES}
  ${OpenCASCADE_ModelingAlgorithms_LIBRARIES}
  ${KERNEL_SALOMELocalTrace}
  ${KERNEL_SALOMEBasics}
  HEXABLOCKEngine
)

# swig flags
SET_SOURCE_FILES_PROPERTIES(hexablock_swig.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(hexablock_swig.i PROPERTIES SWIG_FLAGS "-py3")

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

add_library(_hexablock_swig MODULE ${_hexablock_swig_SOURCES})
target_link_libraries(_hexablock_swig ${_link_LIBRARIES} ${hexablock_swig_lib}) # does nothing but reproduces previous SWIG_LINK_LIBRARIES line
set_target_properties(_hexablock_swig PROPERTIES PREFIX "")
INSTALL(TARGETS _hexablock_swig DESTINATION ${SALOME_INSTALL_LIBS})

SALOME_INSTALL_SCRIPTS("${_bin_SCRIPTS}" ${SALOME_INSTALL_SCRIPT_PYTHON})
