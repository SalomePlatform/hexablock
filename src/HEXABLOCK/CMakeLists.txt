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
  ${KERNEL_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
  ${CAS_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
  ${GEOM_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
  ${PROJECT_BINARY_DIR}/idl
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${KERNEL_DEFINITIONS}
  ${CAS_DEFINITIONS}
  ${BOOST_DEFINITIONS}
)

# libraries to link to
SET(_link_LIBRARIES
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
  ${PYTHON_LIBRARIES}
)

# --- headers ---

# header files
SET(HEXABLOCKEngine_HEADERS
  HexGroup.hxx
  HexEdge.hxx
  HexGlobale.hxx
  HexPropagation.hxx
  HexVector.hxx
  HexXmlWriter.hxx
  hexa_base.hxx
  HexDiagnostics.hxx
  HexElements.hxx
  HexHexa.hxx
  HexMatrix.hxx
  HexQuad.hxx
  HexQpattern.hxx
  HexVertex.hxx
  HexLaw.hxx
  HexCloner.hxx
  HexDocument.hxx
  HexEltBase.hxx
  Hex.hxx
  HexShape.hxx
  HexOldShape.hxx
  HexNewShape.hxx
  HexSubShape.hxx
  HexVertexShape.hxx
  HexEdgeShape.hxx
  HexFaceShape.hxx
  HexBiCylinderShape.hxx
  HexAssoEdge.hxx
  HexXmlTree.hxx
  HexBiCylinder.hxx
  HexDumpStudy.hxx
  HexAnaQuads.hxx
  HexKas_functions.hxx
  HexWitness.hxx
  HexCramer.hxx
  Hex_defines.hxx 
)

# --- sources ---

# sources / static
SET(HEXABLOCKEngine_SOURCES
  hexa_utils.cxx
  HexEltBase.cxx
  HexGlobale.cxx
  HexCloner.cxx
  HexVertex.cxx
  HexEdge.cxx
  HexQuad.cxx
  HexQpattern.cxx
  HexHexa.cxx
  HexHexa_disco.cxx
  HexVector.cxx
  HexElements.cxx
  HexElements_bis.cxx
  HexElements_ter.cxx
  HexElements_piq.cxx
  HexElements_asso.cxx
  HexElements_grid.cxx
  HexGroup.cxx
  HexLaw.cxx
  HexShape.cxx
  HexNewShape.cxx
  HexSubShape.cxx
  HexDocument_trans.cxx
  HexDocument_tools.cxx
  HexVertexShape.cxx
  HexEdgeShape.cxx
  HexFaceShape.cxx
  HexBiCylinderShape.cxx
  HexAssoEdge.cxx
  HexDocument_Xml.cxx
  HexDocument.cxx
  HexXmlTree.cxx
  HexXmlWriter.cxx
  HexDumpStudy.cxx
  HexBiCylinder.cxx
  Hex.cxx
  HexDocument_asso.cxx
  HexDocument_quads.cxx
  HexDocument_skin.cxx
  HexDocument_v6.cxx
  HexElements_v6.cxx
  HexElements_check.cxx
  HexWitness.cxx
  Hex_salome.cxx
  test_unit.hxx
  test_hexa6.cxx
  HexKas_functions.cxx   
)

# --- rules ---

ADD_LIBRARY(HEXABLOCKEngine ${HEXABLOCKEngine_SOURCES})
TARGET_LINK_LIBRARIES(HEXABLOCKEngine ${_link_LIBRARIES} )
INSTALL(TARGETS HEXABLOCKEngine EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${HEXABLOCKEngine_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
