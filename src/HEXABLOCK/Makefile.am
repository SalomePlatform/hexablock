#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
#  SuperVisionTest AddComponent : example of component that adds two numbers
# ddDIST_SUBDIRS = Test
# ddSUBDIRS = Test
#  File   : Makefile.in
#  Author : , CEA
#  Modified by : Alexander BORODIN (OCN) - autotools usage
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
	HexGroups.hxx \
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
	HexCrossElements.hxx



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
	HexElements_grid.cxx \
	HexCylinder.cxx \
	HexPipe.cxx \
	HexShape.cxx \
	HexDocument_trans.cxx \
	HexDocument_Xml.cxx \
	HexDocument.cxx \
	HexXmlTree.cxx \
	HexXmlWriter.cxx \
	HexCrossElements.cxx \
	Hex.cxx

# libHEXABLOCKimpl_la_CPPFLAGS = \
# 	-I$(top_builddir)/salome_adm/unix
# 
# libHEXABLOCKimpl_la_LDFLAGS = \
# 	-lOpUtil