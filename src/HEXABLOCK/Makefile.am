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
	HexOldShape.hxx \
	HexNewShape.hxx \
	HexSubShape.hxx \
	HexVertexShape.hxx \
	HexEdgeShape.hxx \
	HexFaceShape.hxx \
	HexBiCylinderShape.hxx \
	HexAssoEdge.hxx \
	HexXmlTree.hxx \
	HexCrossElements.hxx \
	HexBiCylinder.hxx \
	HexDumpStudy.hxx \
	HexAnaQuads.hxx \
	HexKas_functions.hxx \
	HexWitness.hxx \
	HexCramer.hxx


# Libraries targets
# SD/Propagation/HexPropagation.cxx \
# SD/Law/HexLaw.cxx \
#
lib_LTLIBRARIES = libHEXABLOCKEngine.la

dist_libHEXABLOCKEngine_la_SOURCES = \
	hexa_utils.cxx \
	HexEltBase.cxx \
	HexGlobale.cxx \
	HexCloner.cxx	\
	HexVertex.cxx \
	HexEdge.cxx \
	HexQuad.cxx \
	HexHexa.cxx \
	HexHexa_disco.cxx \
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
	HexNewShape.cxx \
	HexSubShape.cxx \
	HexDocument_trans.cxx \
	HexDocument_tools.cxx \
	HexVertexShape.cxx \
	HexEdgeShape.cxx \
	HexFaceShape.cxx \
	HexBiCylinderShape.cxx \
	HexAssoEdge.cxx \
	HexDocument_Xml.cxx \
	HexDocument.cxx \
	HexXmlTree.cxx \
	HexXmlWriter.cxx \
	HexDumpStudy.cxx \
	HexCrossElements.cxx \
	HexCrossElements_build.cxx \
	HexBiCylinder.cxx \
	Hex.cxx              \
	HexDocument_asso.cxx \
	HexDocument_quads.cxx \
	HexDocument_del.cxx \
	HexDocument_skin.cxx \
	HexDocument_v6.cxx \
	HexElements_v6.cxx \
	HexElements_check.cxx \
	HexElements_del.cxx \
	HexWitness.cxx \
	Hex_salome.cxx \
	test_hexa6.cxx \
	HexKas_functions.cxx 

# libHEXABLOCKEngine_la_CPPFLAGS = \
# 	-I$(top_builddir)/salome_adm/unix
#
# libHEXABLOCKEngine_la_LDFLAGS = \
# 	-lOpUtil

libHEXABLOCKEngine_la_CPPFLAGS = \
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
	-I$(top_builddir)/idl

libHEXABLOCKEngine_la_LDFLAGS = \
	$(PYTHON_LDFLAGS)     \
	$(CAS_KERNEL) -lTKBRep -lTKPrim -lTKG2d -lTKG3d -lTKTopAlgo -lTKGeomBase -lTKGeomAlgo  -lTKBool -lTKBO \
	$(KERNEL_LDFLAGS) -lSALOMELocalTrace -lSALOMEBasics


