# Copyright (C) 2009-2012  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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

include $(top_srcdir)/adm_local/unix/make_common_starter.am

#
# ===============================================================
# Files to be installed
# ===============================================================
#
# header files  
# salomeinclude_HEADERS= ../SD/VertexTest.hxx

# Scripts to be installed


#
# ===============================================================
# Local definitions
# ===============================================================
#

# This directory defines the subdirectory src in the top source directory.
# RPATH=../..
RPATH=..


# This local variable defines the list of CPPFLAGS common to all target in this package.
COMMON_CPPFLAGS = \
	@CPPUNIT_INCLUDES@ \
	-I$(srcdir)/$(RPATH)/HEXABLOCK/ \
	-I$(top_builddir)/idl \
	@CORBA_CXXFLAGS@ @CORBA_INCLUDES@


# This local variable defines the list of dependant libraries common to all target in this package.
COMMON_LIBS =\
	@CPPUNIT_LIBS@ \
	$(RPATH)/HEXABLOCK/libHEXABLOCKimpl.la \
	$(KERNEL_LDFLAGS) \
	-lSalomeGenericObj

# ===============================================================
# Libraries targets
# 	HexVertexTest.cxx \
# 	HexEdgeTest.cxx \
# ===============================================================
lib_LTLIBRARIES = libHEXABLOCKTest.la
libHEXABLOCKTest_la_SOURCES  = \
	test_unit.hxx           \
	test_hexa1.cxx              \
	test_quads.cxx              \
	ModelTest.cxx ModelTest.hxx

libHEXABLOCKTest_la_CPPFLAGS = $(COMMON_CPPFLAGS)
libHEXABLOCKTest_la_LDFLAGS  = -no-undefined -version-info=0:0:0
libHEXABLOCKTest_la_LIBADD    = $(COMMON_LIBS)


# ===============================================================
# Executables targets
# ===============================================================
bin_PROGRAMS = Test_HEXABLOCK
Test_HEXABLOCK_SOURCES  = Test_HEXABLOCK.cxx
Test_HEXABLOCK_CPPFLAGS = $(COMMON_CPPFLAGS)
Test_HEXABLOCK_LDADD    = \
	libHEXABLOCKTest.la \
	../HEXABLOCK/libHEXABLOCKimpl.la \
	$(COMMON_LIBS)

UNIT_TEST_PROG = ./Test_HEXABLOCK

check : tests

# bin_PROGRAMS = bielle 
# 	test_hexa1 \
# 	test_cartesi1 \
# 	test_clone \
# 	test_cyl \
# 	test_decoupage \
# 	test_find \
# 	test_gen_xml \
# 	test_joint \
# 	test_propagation \
# 	test_separ \
# 	tuyau
# 	test_lecture


# UNIT TEST
# test_cartesi1_SOURCES  = test_cartesi1.cpp	
# test_cartesi1_CPPFLAGS = $(COMMON_CPPFLAGS)
# test_cartesi1_LDADD    = \
# 	libHEXATest.la \
# 	../HEXA/libHEXAimpl.la \
# 	$(COMMON_LIBS)
# 
# test_clone_SOURCES  = test_clone.cpp
# test_clone_CPPFLAGS = $(COMMON_CPPFLAGS)
# test_clone_LDADD    = \
# 	libHEXATest.la \
# 	../HEXA/libHEXAimpl.la \
# 	$(COMMON_LIBS)
# 
# test_cyl_SOURCES  = test_cyl.cpp
# test_cyl_CPPFLAGS = $(COMMON_CPPFLAGS)
# test_cyl_LDADD    = \
# 	libHEXATest.la \
# 	../HEXA/libHEXAimpl.la \
# 	$(COMMON_LIBS)
# 
# test_decoupage_SOURCES  = test_decoupage.cpp
# test_decoupage_CPPFLAGS = $(COMMON_CPPFLAGS)
# test_decoupage_LDADD    = \
# 	libHEXATest.la \
# 	../HEXA/libHEXAimpl.la \
# 	$(COMMON_LIBS)
# 
# test_find_SOURCES  = test_find.cpp
# test_find_CPPFLAGS = $(COMMON_CPPFLAGS)
# test_find_LDADD    = \
# 	libHEXATest.la \
# 	../HEXA/libHEXAimpl.la \
# 	$(COMMON_LIBS)
# 
# test_gen_xml_SOURCES  = test_gen_xml.cpp
# test_gen_xml_CPPFLAGS = $(COMMON_CPPFLAGS)
# test_gen_xml_LDADD    = \
# 	libHEXATest.la \
# 	../HEXA/libHEXAimpl.la \
# 	$(COMMON_LIBS)
# 
# test_joint_SOURCES  = test_joint.cpp
# test_joint_CPPFLAGS = $(COMMON_CPPFLAGS)
# test_joint_LDADD    = \
# 	libHEXATest.la \
# 	../HEXA/libHEXAimpl.la \
# 	$(COMMON_LIBS)

# test_lecture_SOURCES  = test_lecture.cpp
# test_lecture_CPPFLAGS = $(COMMON_CPPFLAGS)
# test_lecture_LDADD    = \
# 	libHEXATest.la \
# 	../HEXA/libHEXAimpl.la \
# 	$(COMMON_LIBS)

# test_propagation_SOURCES  = test_propagation.cpp
# test_propagation_CPPFLAGS = $(COMMON_CPPFLAGS)
# test_propagation_LDADD    = \
# 	libHEXATest.la \
# 	../HEXA/libHEXAimpl.la \
# 	$(COMMON_LIBS)
# 
# 
# test_separ_SOURCES  = test_separ.cpp
# test_separ_CPPFLAGS = $(COMMON_CPPFLAGS)
# test_separ_LDADD    = \
# 	libHEXATest.la \
# 	../HEXA/libHEXAimpl.la \
# 	$(COMMON_LIBS)



# test_hexa1_SOURCES  = test_hexa1.cpp
# test_hexa1_CPPFLAGS = $(COMMON_CPPFLAGS)
# test_hexa1_LDADD    = \
# 	libHEXATest.la \
# 	../HEXA/libHEXAimpl.la \
# 	$(COMMON_LIBS)
# 
# UNIT_TEST_PROG = ./test_hexa1

# libHEXATest.la \
# USER CASE
# bielle_SOURCES  = bielle.cpp
# bielle_CPPFLAGS = $(COMMON_CPPFLAGS)
# bielle_LDADD    = \
# 	../HEXA/libHEXAimpl.la \
# 	$(COMMON_LIBS)

# tuyau_SOURCES  = tuyau.cpp
# tuyau_CPPFLAGS = $(COMMON_CPPFLAGS)
# tuyau_LDADD    = \
# 	libHEXATest.la \
# 	../HEXA/libHEXAimpl.la \
# 	$(COMMON_LIBS)



