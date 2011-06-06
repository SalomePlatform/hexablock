#  Copyright (C) 2009-2011  CEA/DEN, EDF R&D
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

# UIC_FILES =       \
#   ui_FormComponent.h 
# BUILT_SOURCES =       \
#   $(UIC_FILES)
# 
# EXTRA_DIST =        \
#   FormComponent.ui  \
#   FormContainer.ui  \
# salomeresdir = $(prefix)/share/salome/resources/@MODULE_NAME@
# include $(top_srcdir)/adm/unix/make_begin.am


include $(top_srcdir)/adm_local/unix/make_common_starter.am
# salomeresdir = $(prefix)/share/salome/resources/@MODULE_NAME@

# header files 
salomeinclude_HEADERS = \
	HEXABLOCKGUI_Export.hxx \
	HEXABLOCKGUI.hxx

# Libraries targets
lib_LTLIBRARIES = libHEXABLOCK.la

dist_libHEXABLOCK_la_SOURCES = \
    Resource.hxx \
    Resource.cxx \
    HEXABLOCKGUI_Exception.hxx \
    HEXABLOCKGUI_Exception.cxx \
    HEXABLOCKGUI_Trace.hxx \
    HEXABLOCKGUI_Trace.cxx \
    HEXABLOCKGUI_Resource.hxx \
    HEXABLOCKGUI_Resource.cxx \
    HEXABLOCKGUI_Data.hxx \
    HEXABLOCKGUI_Data.cxx \
    HEXABLOCKGUI_DataObject.hxx \
    HEXABLOCKGUI_DataObject.cxx \
    HEXABLOCKGUI_DataModel.hxx \
    HEXABLOCKGUI_DataModel.cxx \
    HEXABLOCKGUI_DocumentGraphicView.hxx \
    HEXABLOCKGUI_DocumentGraphicView.cxx \
    HEXABLOCKGUI_DocumentModel.hxx \
    HEXABLOCKGUI_DocumentModel.cxx \
    HEXABLOCKGUI_DocumentSelectionModel.hxx \
    HEXABLOCKGUI_DocumentSelectionModel.cxx \
    HEXABLOCKGUI_DocumentItem.hxx \
    HEXABLOCKGUI_DocumentItem.cxx \
    HEXABLOCKGUI_DocumentDelegate.hxx \
    HEXABLOCKGUI_DocumentDelegate.cxx \
    HEXABLOCKGUI_DocumentPanel.hxx \
    HEXABLOCKGUI_DocumentPanel.cxx \
    HEXABLOCKGUI.hxx \
    HEXABLOCKGUI.cxx \
    kmodelindexproxymapper.hxx \
    kmodelindexproxymapper.cxx \
    klinkitemselectionmodel.hxx \
    klinkitemselectionmodel.cxx


UIC_FILES = \
	ui_Vertex_QTD.h \
	ui_Edge_QTD.h \
	ui_Quad_QTD.h \
	ui_Hexa_QTD.h \
	ui_Vector_QTD.h \
	ui_Cylinder_QTD.h \
	ui_Pipe_QTD.h \
	ui_MakeGrid_QTD.h \
	ui_MakeCylinder_QTD.h \
	ui_MakePipe_QTD.h \
	ui_MakeCylinders_QTD.h \
	ui_MakePipes_QTD.h \
	ui_RemoveHexa_QTD.h \
	ui_PrismQuad_QTD.h \
	ui_JoinQuad_QTD.h \
	ui_Merge_QTD.h \
	ui_Disconnect_QTD.h \
	ui_CutEdge_QTD.h \
	ui_Transformation_QTD.h \
	ui_Symmetry_QTD.h \
	ui_Group_QTD.h \
	ui_Law_QTD.h \
	ui_Propagation_QTD.h


# MOC pre-processing
MOC_FILES = \
    HEXABLOCKGUI_moc.cxx \
    HEXABLOCKGUI_DocumentGraphicView_moc.cxx \
    HEXABLOCKGUI_DocumentPanel_moc.cxx \
    HEXABLOCKGUI_DocumentDelegate_moc.cxx \
    HEXABLOCKGUI_DocumentSelectionModel_moc.cxx \
    HEXABLOCKGUI_DocumentModel_moc.cxx \
    kmodelindexproxymapper_moc.cxx \
    klinkitemselectionmodel_moc.cxx

nodist_libHEXABLOCK_la_SOURCES = $(MOC_FILES) \
			$(UIC_FILES)

libHEXABLOCK_la_CXXFLAGS = \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	$(OMNIORB_CXXFLAGS) \
	$(OMNIORB_INCLUDES) \
	$(LIBXML_INCLUDES) \
	$(BOOST_CPPFLAGS) \
	$(CAS_CPPFLAGS) \
	$(VTK_INCLUDES) \
	-I$(KERNEL_ROOT_DIR)/include/salome \
	-I$(GUI_ROOT_DIR)/include/salome \
	-I$(GEOM_ROOT_DIR)/include/salome \
	-I$(srcdir)/../HEXABLOCK \
	-I../../idl \
	$(qt4_cppflags) \
	$(qsci4_cppflags)


libHEXABLOCK_la_LDFLAGS  = \
	../../idl/libSalomeIDLHEXABLOCK.la \
	../HEXABLOCK_I/libHEXABLOCKEngine.la \
	../HEXABLOCK/libHEXABLOCKimpl.la \
	 -L$(GUI_ROOT_DIR)/lib/salome -lEvent -L$(KERNEL_ROOT_DIR)/lib/salome \
	 $(qt4_ldflags) $(qsci4_ldflags) \
	$(KERNEL_LDFLAGS) -lSalomeLifeCycleCORBA \
	$(GUI_LDFLAGS) \
	-lSalomeApp


libHEXABLOCK_la_LIBADD  = \
	$(qt4_libs) $(qsci4_libs) \
	-lSalomeApp

BUILT_SOURCES = $(UIC_FILES)

# resources files
ICONS =  \
	resources/SalomeApp.xml \
	resources/HEXABLOCKCatalog.xml \
	resources/ModuleHexablock.png \
	resources/ExecHEXABLOCK.png

QMFILES = \
	HEXABLOCK_msg_en.qm \
	HEXABLOCK_msg_fr.qm \
	HEXABLOCK_icons.qm

dist_salomeres_DATA = ${ICONS} 

nodist_salomeres_DATA = $(QMFILES)