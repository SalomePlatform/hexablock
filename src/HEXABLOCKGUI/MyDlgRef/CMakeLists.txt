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

INCLUDE(UseQtExt)

# --- options ---
# additional include directories
INCLUDE_DIRECTORIES(
  ${QT_INCLUDES}
  ${GUI_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
)

# libraries to link to
SET(_link_LIBRARIES
  ${GUI_qtx}
  ${GUI_SalomeApp}
)

# --- headers ---

# header files / to be processed by moc
SET(MyDlgRef_HEADERS
  MyDlgRef.hxx
)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${MyDlgRef_HEADERS})

# sources / static
SET(_other_SOURCES
  MyDlgRef.hxx
  MyDlgRef.cxx
)

# uic files / to be processed by pyuic
SET(_pyuic_files
  DlgRef_1Sel2Spin_QTD.ui
  DlgRef_1Sel3Spin_QTD.ui
  DlgRef_2Sel1Spin_QTD.ui
  DlgRef_2Sel_QTD.ui
  DlgRef_3Spin_QTD.ui
  DlgRef_Skeleton_QTD.ui
)

# scripts / pyuic wrappings
QT_WRAP_UIC(_uic_HEADERS ${_pyuic_files})

# sources / to compile
SET(MyDlgRef_SOURCES ${_other_SOURCES} ${_moc_SOURCES} ${_uic_HEADERS})

# --- rules ---

ADD_LIBRARY(MyDlgRef ${MyDlgRef_SOURCES})
TARGET_LINK_LIBRARIES(MyDlgRef ${_link_LIBRARIES} )
INSTALL(TARGETS MyDlgRef EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
