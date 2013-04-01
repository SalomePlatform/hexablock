# Copyright (C) 2009-2013  CEA/DEN, EDF R&D
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
#  File   : Makefile.in
#  Author : , CEA
#  Modified by : Alexander BORODIN (OCN) - autotools usage
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am



# ===============================================================
# Files to be installed
# ===============================================================
#
# header files  
salomeinclude_HEADERS = \
	HEXABLOCK.hxx \
	HexCrossElements_impl.hxx \
	HexCylinder_impl.hxx \
	HexDocument_impl.hxx \
	HexEdge_impl.hxx \
	HexElements_impl.hxx \
	HexGroup_impl.hxx \
	HexHexa_impl.hxx \
	HexLaw_impl.hxx \
	HexPipe_impl.hxx \
	HexPropagation_impl.hxx \
	HexQuad_impl.hxx \
	HexVector_impl.hxx \
	HexNewShape_impl.hxx \
	HexVertex_impl.hxx

# Scripts to be installed
dist_salomescript_PYTHON = hexablock.py

# Libraries targets
# SD/Propagation/HexPropagation.cxx \
# SD/Law/HexLaw.cxx \
#
lib_LTLIBRARIES = libHEXABLOCKEngine.la
dist_libHEXABLOCKEngine_la_SOURCES = \
	HexVertex_impl.cxx \
	HexEdge_impl.cxx \
	HexQuad_impl.cxx \
	HexHexa_impl.cxx \
	HexVector_impl.cxx \
	HexNewShape_impl.cxx \
	HexElements_impl.cxx \
	HexCrossElements_impl.cxx \
	HexCylinder_impl.cxx \
	HexPipe_impl.cxx \
	HexPropagation_impl.cxx \
	HexLaw_impl.cxx \
	HexGroup_impl.cxx \
	HexDocument_impl.cxx \
	HEXABLOCK.cxx \
	HEXABLOCK_1.cxx



libHEXABLOCKEngine_la_CPPFLAGS = \
	$(CORBA_CXXFLAGS) \
	$(CORBA_INCLUDES) \
	$(KERNEL_CXXFLAGS) \
	$(GEOM_CXXFLAGS) \
	$(CAS_CPPFLAGS) \
	-I$(srcdir)/../HEXABLOCK \
	-I$(top_builddir) \
	-I$(top_builddir)/idl \
	-I$(top_builddir)/salome_adm/unix

libHEXABLOCKEngine_la_LDFLAGS = \
	../../idl/libSalomeIDLHEXABLOCK.la \
	../HEXABLOCK/libHEXABLOCKimpl.la \
	$(KERNEL_LDFLAGS) -lOpUtil -lSalomeNS -lSalomeContainer -lSalomeGenericObj -lSalomeLifeCycleCORBA \
	$(CAS_LDPATH) \
        $(GEOM_LDFLAGS) -lGEOMClient

