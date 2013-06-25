
// C++ : Grilles

// Copyright (C) 2009-2013  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include "HexElements.hxx"
#include "HexDocument.hxx"
#include "HexVector.hxx"
#include "HexVertex.hxx"
#include "HexHexa.hxx"
#include "HexEdge.hxx"
#include "HexGlobale.hxx"
#include "HexCylinder.hxx"

#include <cmath>

BEGIN_NAMESPACE_HEXA

// ====================================================== makeCartesianGrid
int Elements::makeCartesianGrid (Vertex* orig, Vector* v1, Vector* v2,
                   Vector* v3, int px, int py, int pz, int mx, int my, int mz)
{
   return HERR;
}
// ====================================================== makeCartesianNodes
int Elements::makeCartesianNodes (Vertex* orig, Vector* v1, Vector* v2,
                   Vector* v3, int px, int py, int pz, int mx, int my, int mz)
{
   return HERR;
}
// ====================================================== makeBasicCylinder
int Elements::makeBasicCylinder (double dr, double da, double dl, int nr, 
                                 int na, int nl, bool fill)
{
   return HERR;
}
// ====================================================== transfoVertices
void Elements::transfoVertices (Vertex* orig, Vector* base, Vector* haut)
{
}
// ====================================================== transfoVertices
void Elements::transfoVertices (Vertex* orig, Vector* iprim, Vector* jprim,
                                Vector* kprim)
{
}
// ====================================================== makeSphericalGrid
int Elements::makeSphericalGrid (Vertex* c, double rayon, int nb, double  k)
{
   return HERR;
}
// ====================================================== makeSphericalGrid
// ==== Deja perimee en Hexa5
int Elements::makeSphericalGrid (Vertex* c, Vector* dv, int nb, double  k)
{
   return HERR;
}
// ====================================================== addStrate
int Elements::addStrate (Quad* i_quad[], Edge* i_edge[], Vertex* i_node[],
                        Vertex* center,  double lambda)
{
   return HERR;
}
// ====================================================== makeCylinder
int Elements::makeCylinder (Cylinder* cyl, Vector* vx, int nr, int na, int nl)
{
   return HERR;
}
// ====================================================== makePipe
int Elements::makePipe (Cylinder* cyl, Vector* vx, int nr, int na, int nl)
{
   return HERR;
}
//
// ---------------------------------------- prism Quads
//
// ====================================================== prismQuads
int Elements::prismQuads (Quads& tstart, Vector* dir, int nbiter)
{
   return HERR;
}
// ====================================================== prismQuadsVec
int Elements::prismQuadsVec (Quads& tstart, Vector* dir, RealVector& tlen,
                             int mode)
{
   return HERR;
}
// ======================================================== revolutionQuads
int Elements::revolutionQuads (Quads& start, Vertex* center, Vector* axis,
                               RealVector &angles)
{
   return HERR;
}
// ====================================================== joinQuads
int Elements::joinQuads (Quads& orig, int nb, Vertex* v1, Vertex* v2,
                  Vertex* v3, Vertex* v4, Quad* cible)
{
   return HERR;
}
// ====================================================== makeCylindricalGrid
// ==== Version avec vecteurs   --> TODO DELETE ????
int Elements::makeCylindricalGrid (Vertex* orig, Vector* base, Vector* haut, 
                            RealVector& tdr, RealVector& tda, RealVector& tdh, 
                            bool fill)
{
   return HOK;
}
// ====================================================== makeCylindricalGrid
int Elements::makeCylindricalGrid (Vertex* c, Vector* b, Vector* h,
         double dr, double da, double dl, int nr, int na, int nl, bool fill)
{
   return HOK;
}

END_NAMESPACE_HEXA
