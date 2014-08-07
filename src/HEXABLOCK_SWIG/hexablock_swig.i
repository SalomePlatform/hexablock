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
%template(VectorHexas)    vector <Hex::Hexa*>;
%template(VectorQuads)    vector <Hex::Quad*>;
%template(VectorEdges)    vector <Hex::Edge*>;
%template(VectorVertices) vector <Hex::Vertex*>;
%template(VectorShapes)   vector <Hex::NewShape*>;
%template(VectorReal)    vector <double>;
%template(VectorInt)     vector <int>;
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
