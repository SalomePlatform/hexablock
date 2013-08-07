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

include $(top_srcdir)/adm_local/unix/make_common_starter.am

#
# ===============================================================
# Files to be installed
# ===============================================================
#
#
# header files

SWIG_OPTIONS = $(SWIG_PYTHON_OPT) -python -c++ -I$(srcdir)/../HEXABLOCK
salomeinclude_HEADERS = 

dist_salomescript_PYTHON = hexablock_swig.py hexablock.py

lib_LTLIBRARIES = _hexablock_swig.la

BUILT_SOURCES = \
	hexablock_swig_wrap.cc 

_hexablock_swig_la_SOURCES = \
	hexablock_swig_wrap.cc hexablock_swig.i

_hexablock_swig_la_CPPFLAGS = \
	$(KERNEL_CXXFLAGS)    \
	$(PYTHON_CPPFLAGS)    \
	$(CAS_CPPFLAGS)       \
        $(VTK_INCLUDES)       \
	$(GEOM_CXXFLAGS)      \
	$(HEXABLOCK_CXXFLAGS) \
	$(SMESH_CXXFLAGS)     \
	$(BOOST_CPPFLAGS)     \
	$(CORBA_CXXFLAGS)     \
	$(CORBA_INCLUDES)     \
	-I$(srcdir)/../HEXABLOCK

_hexablock_swig_la_LDFLAGS = -module \
	../HEXABLOCK/libHEXABLOCKEngine.la \
	$(PYTHON_LDFLAGS)     \
	$(GEOM_LDFLAGS) -lGEOMBase \
	$(CAS_KERNEL) -lTKBRep -lTKPrim -lTKG2d -lTKG3d -lTKTopAlgo -lTKGeomBase -lTKGeomAlgo  -lTKBool -lTKBO \
	$(KERNEL_LDFLAGS) -lSALOMELocalTrace -lSALOMEBasics

$(srcdir)/hexablock_swig_wrap.cc : hexablock_swig.i
	echo SWIG_PYTHON_OPT = $(SWIG_PYTHON_OPT) 
	$(SWIG) $(SWIG_OPTIONS) -o $(srcdir)hexablock_swig_wrap.cc $(srcdir)/hexablock_swig.i

hexablock_swig.py : hexablock_swig_wrap.cc

