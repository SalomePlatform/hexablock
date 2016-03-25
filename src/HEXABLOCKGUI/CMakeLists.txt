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

ADD_SUBDIRECTORY(MyDlgRef)

INCLUDE(UseQtExt)

# --- options ---
# additional include directories
INCLUDE_DIRECTORIES(
  ${QT_INCLUDES}
  ${PYTHON_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
  ${LIBXML2_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${CAS_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
  ${KERNEL_INCLUDE_DIRS}
  ${GUI_INCLUDE_DIRS}
  ${GEOM_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}
  ${PROJECT_BINARY_DIR}/idl
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/MyDlgRef
  ${CMAKE_CURRENT_BINARY_DIR}/MyDlgRef
  ${PROJECT_SOURCE_DIR}/src/HEXABLOCK
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${BOOST_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  ${CAS_DEFINITIONS}
  ${KERNEL_DEFINITIONS}
)

IF(WIN32)
 ADD_DEFINITIONS(-DNOGDI -DNOMINMAX)
ENDIF(WIN32)

# libraries to link to
SET(_link_LIBRARIES
  ${CORBA_LIBS}
  ${QT_LIBRARIES}
  ${GEOM_SalomeIDLGEOM}
  ${GEOM_GEOMBase}
  ${GEOM_BasicGUI}
  ${KERNEL_SalomeLifeCycleCORBA}
  MyDlgRef
  HEXABLOCKEngine
)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS
  HEXABLOCKGUI.hxx
  HEXABLOCKGUI_VtkDocumentGraphicView.hxx
  HEXABLOCKGUI_DocumentPanel.hxx
  HEXABLOCKGUI_DocumentSelectionModel.hxx
  HEXABLOCKGUI_DocumentModel.hxx
  kmodelindexproxymapper.hxx
  klinkitemselectionmodel.hxx
  MyBasicGUI_PointDlg.hxx
  MyGEOMBase_Skeleton.hxx
)

# header files / no moc processing
SET(HEXABLOCK_HEADERS
  HEXABLOCKGUI_Export.hxx
  HEXABLOCKGUI.hxx
)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  Resource.hxx
  Resource.cxx
  HEXABLOCKGUI_Exception.hxx
  HEXABLOCKGUI_Exception.cxx
  HEXABLOCKGUI_Trace.hxx
  HEXABLOCKGUI_Trace.cxx
  HEXABLOCKGUI_Resource.hxx
  HEXABLOCKGUI_Resource.cxx
  HEXABLOCKGUI_VtkDocumentGraphicView.hxx
  HEXABLOCKGUI_VtkDocumentGraphicView.cxx
  HEXABLOCKGUI_OccGraphicView.hxx
  HEXABLOCKGUI_OccGraphicView.cxx
  HEXABLOCKGUI_OCCSelector.h
  HEXABLOCKGUI_OCCSelector.cxx
  HEXABLOCKGUI_GraphicViewsHandler.hxx
  HEXABLOCKGUI_GraphicViewsHandler.cxx
  HEXABLOCKGUI_DocumentModel.hxx
  HEXABLOCKGUI_DocumentModel.cxx
  HEXABLOCKGUI_DocumentSelectionModel.hxx
  HEXABLOCKGUI_DocumentSelectionModel.cxx
  HEXABLOCKGUI_DocumentItem.hxx
  HEXABLOCKGUI_DocumentItem.cxx
  HEXABLOCKGUI_DocumentPanel.hxx
  HEXABLOCKGUI_DocumentPanel.cxx
  HEXABLOCKGUI_SalomeTools.hxx
  HEXABLOCKGUI_SalomeTools.cxx
  HEXABLOCKGUI.hxx
  HEXABLOCKGUI.cxx
  kmodelindexproxymapper.hxx
  kmodelindexproxymapper.cxx
  klinkitemselectionmodel.hxx
  klinkitemselectionmodel.cxx
  MyGEOMBase_Skeleton.hxx
  MyGEOMBase_Skeleton.cxx
  MyBasicGUI_PointDlg.hxx
  MyBasicGUI_PointDlg.cxx
)

# uic files / to be processed by pyuic
SET(_pyuic_files
  Vertex_QTD.ui
  Edge_QTD.ui
  Quad_QTD.ui
  Hexa_QTD.ui
  Vector_QTD.ui
  Cylinder_QTD.ui
  Pipe_QTD.ui
  MakeGrid_QTD.ui
  MakeCylinder_QTD.ui
  MakePipe_QTD.ui
  MakeCylinders_QTD.ui
  MakePipes_QTD.ui
  RemoveHexa_QTD.ui
  PrismQuad_QTD.ui
  JoinQuad_QTD.ui
  Merge_QTD.ui
  Disconnect_QTD.ui
  CutEdge_QTD.ui
  Transformation_QTD.ui
  Symmetry_QTD.ui
  Group_QTD.ui
  Law_QTD.ui
  Propagation_QTD.ui
  QuadAssoc_QTD.ui
  EdgeAssoc_QTD.ui
  QuadRevolution_QTD.ui
  MakeRind_QTD.ui
  ReplaceHexa_QTD.ui
  MakeHemiSphere_QTD.ui
  ModelInfo_QTD.ui
  AddShapeDialog_QTD.ui
  ComputeMesh_QTD.ui
)

# scripts / pyuic wrappings
QT_WRAP_UIC(_uic_HEADERS ${_pyuic_files})

# sources / to compile
SET(HEXABLOCK_SOURCES ${_other_SOURCES} ${_moc_SOURCES} ${_uic_HEADERS})

# --- resources ---

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  resources/HEXABLOCK_msg_en.ts
  resources/HEXABLOCK_msg_fr.ts
  resources/HEXABLOCK_msg_ja.ts
  resources/HEXABLOCK_icons.ts
) 

SALOME_CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/resources/SalomeApp.xml.in ${CMAKE_CURRENT_BINARY_DIR}/resources/SalomeApp.xml INSTALL ${SALOME_HEXABLOCK_INSTALL_RES_DATA})
SALOME_CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/resources/HEXABLOCKCatalog.xml.in ${CMAKE_CURRENT_BINARY_DIR}/resources/HEXABLOCKCatalog.xml INSTALL ${SALOME_HEXABLOCK_INSTALL_RES_DATA})

# resource files / static
SET(_other_RESOURCES
  resources/LightApp.xml
  resources/ModuleHexablock.png
  resources/ModuleHexablock15x15.png
  resources/new_document.png
  resources/load_document.png
  resources/save_document.png
  resources/add_vertex.png
  resources/add_edge.png
  resources/add_quad.png
  resources/add_hexa.png
  resources/add_vector.png
  resources/add_cylinder.png
  resources/add_pipe.png
  resources/make_grid.png
  resources/make_cylinder.png
  resources/make_pipe.png
  resources/make_cylinders.png
  resources/make_pipes.png
  resources/remove_hexa.png
  resources/prism_quad.png
  resources/join_quad.png
  resources/merge.png
  resources/disconnect.png
  resources/cut_edge.png
  resources/make_transformation.png
  resources/make_symmetry.png
  resources/perform_transformation.png
  resources/perform_symmetry.png
  resources/assoc_vertex.png
  resources/assoc_edge.png
  resources/assoc_quad.png
  resources/add_geom.png
  resources/model_info.png
  resources/add_group.png
  resources/remove_group.png
  resources/add_law.png
  resources/remove_law.png
  resources/set_propagation.png
  resources/compute_mesh.png
  resources/point2.png
  resources/pointonedge.png
  resources/select1.png
  resources/point3.png
  resources/point_line_intersection.png 
  resources/pointonface.png
  resources/line.png
  resources/build_wire.png
  resources/quad_revolution.png
  resources/replace_hexa.png
  resources/make_hemisphere.png
)

# --- rules ---

ADD_LIBRARY(HEXABLOCK ${HEXABLOCK_SOURCES})
TARGET_LINK_LIBRARIES(HEXABLOCK ${_link_LIBRARIES} )
SET_TARGET_PROPERTIES(HEXABLOCK PROPERTIES COMPILE_DEFINITIONS QT_DISABLE_DEPRECATED_BEFORE=0 )
INSTALL(TARGETS HEXABLOCK EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${HEXABLOCK_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_HEXABLOCK_INSTALL_RES_DATA}")

INSTALL(FILES ${_other_RESOURCES}
        DESTINATION ${SALOME_HEXABLOCK_INSTALL_RES_DATA})
