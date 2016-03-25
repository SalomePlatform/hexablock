
// C++ : Tests unitaires sur les creations h'hexaedres

// Copyright (C) 2009-2016  CEA/DEN, EDF R&D
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
#include "Hex.hxx"

#include <unistd.h>

#include "HexDocument.hxx"
#include "HexElements.hxx"

#include "HexHexa.hxx"
#include "HexQuad.hxx"
#include "HexEdge.hxx"
#include "HexVertex.hxx"

#include "HexCramer.hxx"

// ======================================================== test_hexa_quads_5
int test_replace5 (int nbargs, cpchar tabargs[])
{
   const int size_x = 4;
   const int size_y = 4;
   const int size_z = 2;

   double lgcell = 3;
   double decal  = lgcell/sqrt (2.0);

   int nvtk = 0;
   cpchar son_nom = "replace5";

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument (son_nom);

   Hex::Vector* vx   = doc->addVector (3,0,0);
   Hex::Vector* vz   = doc->addVector (0,0,1);

   Hex::Elements* grid1 = doc->makeCartesianTop (size_x,size_y,size_z);
   doc->saveVtk (son_nom, nvtk);

   for (int nz=0 ; nz<=size_z ; ++nz)
       {
       grid1->getVertexIJK (1, 1, nz)->setX (-decal);
       grid1->getVertexIJK (1, 1, nz)->setY (-decal);

       grid1->getVertexIJK (1, 3, nz)->setX (-decal);
       grid1->getVertexIJK (1, 3, nz)->setY ( decal);

       grid1->getVertexIJK (3, 1, nz)->setX ( decal);
       grid1->getVertexIJK (3, 1, nz)->setY (-decal);

       grid1->getVertexIJK (3, 3, nz)->setX ( decal);
       grid1->getVertexIJK (3, 3, nz)->setY ( decal);
       }

   doc->saveVtk (son_nom, nvtk);

   Hex::Vertex* ori0 = doc->addVertex (0,-10,0);
   Hex::Vertex* ori2 = doc->addVertex (0,0,0);
   const double da = 360;
   const double dl = 1;
   const int    nr = 2;
   const int    na = 8;
   const int    nl = 3;


   Hex::Elements* grid0 = doc->makePipeUni (ori0, vx, vz, 
                                            1.0,5.0, da, dl, nr, na, nl);

   doc->saveVtk (son_nom, nvtk);

   // Hex::Elements* grid2 = 
   doc->makePipeUni (ori2, vx, vz, 1.0,5.0, da, dl, nr, na, nl);

   doc->saveVtk (son_nom, nvtk);
   grid0->remove ();
   doc->saveVtk (son_nom, nvtk);
   return HOK;
}
// ======================================================== test_get
int test_get (int nbargs, cpchar tabargs[])
{
   const int size_x = 2;
   const int size_y = 2;
   const int size_z = 2;

   int nvtk = 0;
   cpchar son_nom = "test_get";

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument (son_nom);

   Hex::Elements* grid = doc->makeCartesianTop (size_x,size_y,size_z);
   doc->saveVtk (son_nom, nvtk);
   grid->getVertexIJK (2, 1, 1)->setColor (4);
   doc->saveVtk (son_nom, nvtk);

   grid->getVertexIJK (2, 1, 1)->setColor (0);
   grid->getEdgeK     (1, 0, 1)->setColor (4);
   doc->saveVtk (son_nom, nvtk);

   grid->getEdgeK     (1, 0, 1)->setColor (0);
   grid->getQuadIJ    (1, 0, 2)->setColor (4);
   doc->saveVtk (son_nom, nvtk);

   grid->getHexaIJK   (1, 0, 1)->setColor (4);
   doc->saveVtk (son_nom, nvtk);
   doc->saveVtk (son_nom, nvtk);
   doc->saveVtk (son_nom, nvtk);
   doc->saveVtk (son_nom, nvtk);
   doc->saveVtk (son_nom, nvtk);
   doc->saveVtk (son_nom, nvtk);
   doc->saveVtk (son_nom, nvtk);
   doc->saveVtk (son_nom, nvtk);
   return HOK;
}
// ======================================================== test_dump
int test_dump (int nbargs, cpchar tabargs[])
{
   const int size_x = 4;
   const int size_y = 4;
   const int size_z = 2;

   int nvtk = 0;
   cpchar son_nom = "test_dump";

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument (son_nom);

   doc->makeCartesianTop (size_x,size_y,size_z);
   doc->saveVtk (son_nom, nvtk);
   return HOK;
}
