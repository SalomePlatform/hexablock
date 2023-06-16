// Copyright (C) 2013-2023  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
%module hexablock_swig
%{
#include "Hex.hxx"
#include "HexDocument.hxx"
#include "HexPropagation.hxx"
#include "HexElements.hxx"
#include "HexQuad.hxx"
#include "HexVertex.hxx"
#include "HexHexa.hxx"
#include "HexEdge.hxx"
#include "HexVector.hxx"
#include "HexBiCylinder.hxx"
#include "HexNewShape.hxx"
#include "HexGroup.hxx"
%}

%include "std_vector.i"
namespace std
{
%template(VectorHexas)    std::vector <Hex::Hexa*>;
%template(VectorQuads)    std::vector <Hex::Quad*>;
%template(VectorEdges)    std::vector <Hex::Edge*>;
%template(VectorVertices) std::vector <Hex::Vertex*>;
%template(VectorShapes)   std::vector <Hex::NewShape*>;
%template(VectorReal)    std::vector <double>;
%template(VectorInt)     std::vector <int>;
}

%include "hexa_base.hxx"
%include "HexEltBase.hxx"
%include "HexVertex.hxx"
%include "HexEdge.hxx"
%include "HexQuad.hxx"
%include "HexHexa.hxx"
%include "HexVector.hxx"
%include "HexDocument.hxx"
%include "HexPropagation.hxx"
%include "Hex.hxx"
%include "HexElements.hxx"
%include "HexBiCylinder.hxx"
%include "HexNewShape.hxx"
%include "HexGroup.hxx"
