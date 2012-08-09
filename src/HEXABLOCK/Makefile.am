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
# 	
# header files  
salomeinclude_HEADERS = \
	HexGroup.hxx \
	HexCylinder.hxx \
	HexEdge.hxx \
	HexGlobale.hxx \
	HexPropagation.hxx \
	HexVector.hxx \
	HexXmlWriter.hxx \
	hexa_base.hxx \
	HexDiagnostics.hxx \
	HexElements.hxx \
	HexHexa.hxx \
	HexMatrix.hxx \
	HexQuad.hxx \
	HexVertex.hxx \
	HexLaw.hxx \
	HexCloner.hxx \
	HexDocument.hxx \
	HexEltBase.hxx \
	Hex.hxx \
	HexPipe.hxx \
	HexShape.hxx \
	HexXmlTree.hxx \
	HexCrossElements.hxx \
	HexAnaQuads.hxx \
	HexKasPoint.hxx \
	HexKasLine.hxx \
	HexKasBiCylinder.hxx \
	HexKas_functions.hxx \
	HexCramer.hxx


# Libraries targets
# SD/Propagation/HexPropagation.cxx \
# SD/Law/HexLaw.cxx \
#
lib_LTLIBRARIES = libHEXABLOCKimpl.la

dist_libHEXABLOCKimpl_la_SOURCES = \
	hexa_utils.cxx \
	HexEltBase.cxx \
	HexGlobale.cxx \
	HexCloner.cxx	\
	HexVertex.cxx \
	HexEdge.cxx \
	HexQuad.cxx \
	HexHexa.cxx \
	HexVector.cxx \
 	HexElements.cxx \
	HexElements_bis.cxx \
	HexElements_ter.cxx \
	HexElements_piq.cxx \
	HexElements_asso.cxx \
	HexElements_grid.cxx \
	HexCylinder.cxx \
	HexGroup.cxx \
	HexPipe.cxx \
	HexLaw.cxx \
	HexShape.cxx \
	HexDocument_trans.cxx \
	HexDocument_Xml.cxx \
	HexDocument.cxx \
	HexXmlTree.cxx \
	HexXmlWriter.cxx \
	HexCrossElements.cxx \
	HexCrossElements_build.cxx \
	Hex.cxx              \
	HexDocument_asso.cxx \
	HexDocument_quads.cxx \
	HexKasPoint.cxx \
	HexKasLine.cxx \
	HexKasBiCylinder.cxx \
	HexKas_functions.cxx

# libHEXABLOCKimpl_la_CPPFLAGS = \
# 	-I$(top_builddir)/salome_adm/unix
# 
# libHEXABLOCKimpl_la_LDFLAGS = \
# 	-lOpUtil

libHEXABLOCKimpl_la_CPPFLAGS = \
	$(KERNEL_CXXFLAGS)    \
	$(CAS_CPPFLAGS)       \
        $(VTK_INCLUDES)       \
	$(GEOM_CXXFLAGS)      \
	$(HEXABLOCK_CXXFLAGS) \
	$(SMESH_CXXFLAGS)     \
	$(BOOST_CPPFLAGS)     \
	$(CORBA_CXXFLAGS)     \
	$(CORBA_INCLUDES)     \
	-I$(top_builddir)/idl

libHEXABLOCKimpl_la_LDFLAGS = \
	$(CAS_KERNEL) -lTKBRep -lTKPrim -lTKG2d -lTKG3d -lTKTopAlgo -lTKGeomBase -lTKGeomAlgo  -lTKBool -lTKBO \
	$(KERNEL_LDFLAGS) -lSalomeGenericObj -lSALOMELocalTrace -lSALOMEBasics

####  libHexaBlockimpl_la_CPPFLAGS =  \
	####  $(KERNEL_CXXFLAGS)    \
	####  $(CAS_CPPFLAGS)       \
        ####  $(VTK_INCLUDES)       \
	####  $(GEOM_CXXFLAGS)      \
	####  $(HEXABLOCK_CXXFLAGS) \
	####  $(MED_CXXFLAGS)       \
	####  $(SMESH_CXXFLAGS)     \
	####  $(BOOST_CPPFLAGS)     \
	####  $(CORBA_CXXFLAGS)     \
	####  $(CORBA_INCLUDES)     \
	####  -I$(top_builddir)/idl

####  libHexaBlockimpl_la_LDFLAGS  = \
	####  ../../idl/libSalomeIDLHEXABLOCKPLUGIN.la \
	####  $(CAS_KERNEL) -lTKBRep -lTKG2d -lTKG3d -lTKTopAlgo -lTKGeomBase -lTKGeomAlgo \
	####  $(HEXABLOCK_LDFLAGS) -lHEXABLOCKEngine \
	####  $(MED_LDFLAGS) -lSalomeIDLMED \
	####  $(SMESH_LDFLAGS) -lSMESHimpl -lSMESHEngine -lSMESHDS -lSMDS \
	####  $(KERNEL_LDFLAGS) -lSalomeGenericObj -lSALOMELocalTrace -lSALOMEBasics
