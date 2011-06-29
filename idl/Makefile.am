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

# This Makefile is responsible of generating the client and server
# implementation of IDL interfaces for both C++ and python usage.
# The building process of the C++ files is in charge of each source
# package and then is not manage here.

include $(top_srcdir)/adm_local/unix/make_common_starter.am

BASEIDL_FILES = \
	Element.idl\
	Vertex.idl\
	Edge.idl\
	Quad.idl\
	Hexa.idl\
	Vector.idl\
	Elements.idl\
	Cylinder.idl\
	CrossElements.idl\
	Pipe.idl\
	Law.idl\
	Propagation.idl\
	Group.idl\
	Document.idl\
	HEXABLOCK_Gen.idl

# This variable defines the files to be installed
dist_salomeidl_DATA = $(BASEIDL_FILES)

# GUI idl common library
lib_LTLIBRARIES = libSalomeIDLHEXABLOCK.la




# Sources built from idl files
nodist_libSalomeIDLHEXABLOCK_la_SOURCES = \
	ElementSK.cc\
	ElementDynSK.cc\
	VertexSK.cc\
	VertexDynSK.cc\
	EdgeSK.cc\
	EdgeDynSK.cc\
	QuadSK.cc\
	QuadDynSK.cc\
	HexaSK.cc\
	HexaDynSK.cc\
	VectorSK.cc\
	VectorDynSK.cc\
	ElementsSK.cc\
	ElementsDynSK.cc\
	CylinderSK.cc\
	CylinderDynSK.cc\
	CrossElementsSK.cc\
	CrossElementsDynSK.cc\
	PipeSK.cc\
	PipeDynSK.cc\
	LawSK.cc\
	LawDynSK.cc\
	PropagationSK.cc\
	PropagationDynSK.cc\
	GroupSK.cc\
	GroupDynSK.cc\
	DocumentSK.cc\
	DocumentDynSK.cc\
	HEXABLOCK_GenSK.cc\
	HEXABLOCK_GenDynSK.cc

ElementDynSK.cc: ElementSK.cc
VertexDynSK.cc: VertexSK.cc
EdgeDynSK.cc: EdgeSK.cc
PropagationDynSK.cc: PropagationSK.cc
LawDynSK.cc: LawSK.cc
ElementsDynSK.cc: ElementsSK.cc
CrossElementsDynSK.cc: CrossElementsSK.cc
QuadDynSK.cc: QuadSK.cc
VectorDynSK.cc: VectorSK.cc
CylinderDynSK.cc: CylinderSK.cc
GroupDynSK.cc: GroupSK.cc
HexaDynSK.cc: HexaSK.cc
PipeDynSK.cc: PipeSK.cc
DocumentDynSK.cc: DocumentSK.cc
HEXABLOCK_GenDynSK.cc: HEXABLOCK_GenSK.cc


# header files must be exported: other modules have to use this library
nodist_salomeinclude_HEADERS = $(BASEIDL_FILES:%.idl=%.hh)

libSalomeIDLHEXABLOCK_la_CPPFLAGS =\
	@CORBA_CXXFLAGS@	\
	@CORBA_INCLUDES@	\
	$(KERNEL_CXXFLAGS)	\
	$(GEOM_CXXFLAGS)	\
	-I$(top_builddir)/salome_adm/unix \
	-I$(top_builddir)/idl


libSalomeIDLHEXABLOCK_la_LDFLAGS = -no-undefined -version-info=0:0:0

libSalomeIDLHEXABLOCK_la_LIBADD  =			\
	$(KERNEL_LDFLAGS) -lSalomeIDLKernel	\
	$(GEOM_LDFLAGS) -lSalomeIDLGEOM 	\
	@CORBA_LIBS@

# These variables defines the building process of CORBA files
OMNIORB_IDL         = @OMNIORB_IDL@
OMNIORB_IDLCXXFLAGS = @OMNIORB_IDLCXXFLAGS@
OMNIORB_IDLPYFLAGS  = \
	@OMNIORB_IDLPYFLAGS@ \
	-I$(top_builddir)/idl/salome \
	-I$(KERNEL_ROOT_DIR)/idl/salome \
	-I$(GEOM_ROOT_DIR)/idl/salome


IDLCXXFLAGS = \
	-bcxx \
	@IDLCXXFLAGS@ \
	-I$(top_builddir)/idl/salome \
	-I$(KERNEL_ROOT_DIR)/idl/salome \
	-I$(GEOM_ROOT_DIR)/idl/salome \
	-I$(top_builddir)/salome_adm/unix \
	-I$(srcdir)
# 	-I$(srcdir)/SD/Vertex
# 	-I$(srcdir)/SD/Edge

IDLPYFLAGS  = \
	@IDLPYFLAGS@ \
	-I$(KERNEL_ROOT_DIR)/idl/salome \
	-I$(GEOM_ROOT_DIR)/idl/salome
	


# potential problem on parallel make on the following - multiple outputs
SUFFIXES = .idl .hh SK.cc
.idlSK.cc:
	$(OMNIORB_IDL) $(IDLCXXFLAGS) $(OMNIORB_IDLCXXFLAGS) $<
.idl.hh:
	$(OMNIORB_IDL) $(IDLCXXFLAGS) $(OMNIORB_IDLCXXFLAGS) $<

install-exec-local: $(BASEIDL_FILES:%=$(top_srcdir)/idl/%)
	$(INSTALL) -d  $(DESTDIR)$(salomepythondir)
	ls $^ | while read file; do \
	  $(OMNIORB_IDL) $(IDLPYFLAGS) -C$(DESTDIR)$(salomepythondir) $$file ; \
	done

# uninstall-local removes too much, but it works in distcheck
uninstall-local:
	rm -rf $(DESTDIR)$(salomepythondir)/*

mostlyclean-local:
	-rm -f *.hh *.cc .depidl

# we use cpp to generate dependencies between idl files.
# option x c tells the preprocessor to consider idl as a c file.
# if an idl is modified, all idl dependencies are rebuilt

.depidl: $(BASEIDL_FILES)
	@echo "" > $@
	@for dep in $^ dummy; do \
	  if [ $$dep != "dummy" ]; then \
	    echo Building dependencies for $$dep; \
	    $(CPP) $(C_DEPEND_FLAG) -x c -I$(srcdir) -I$(KERNEL_ROOT_DIR)/idl/salome -I$(GEOM_ROOT_DIR)/idl/salome $$dep 2>/dev/null \ | \
	    sed 's/\.o/\SK.cc/' >>$@; \
	  fi; \
	done ;

-include .depidl

# -I$(GEOM_ROOT_DIR)/include/salome