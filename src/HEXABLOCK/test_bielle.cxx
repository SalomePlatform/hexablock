
// C++ : Test bielle

//  Copyright (C) 2009-2011  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ 
//  or email : webmaster.salome@opencascade.com

#include <cmath>

#include "Hex.hxx"
#include "HexDocument.hxx"
#include "HexElements.hxx"

#include "HexHexa.hxx"
#include "HexQuad.hxx"
#include "HexEdge.hxx"
#include "HexVertex.hxx"

#include "HexPropagation.hxx"
#include "HexShape.hxx"
#include "HexLaw.hxx"

#define DumpElt(x) { printf(" ... "#x " = ") ; if (x) x->dump() ; else printf ("0x\n") ; } 

// ======================================================== test_bielle
int test_bielle (int nbargs, cpchar tabargs[])
{
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   double R   = 40.0;
   double l = 200.0;
   double h = 40.0;

    // Taille du grand cylindre
   double dr_grd = R;
   double da_grd = 360;
   double dl_grd = h;

   int nr_grd = 1;
   int na_grd = 4;
   int nl_grd = 1;

   Hex::Vector* dx  = doc->addVector (l, 0, 0);
   Hex::Vector* dxy = doc->addVector (l, l, 0);
   Hex::Vector* dz  = doc->addVector (0, 0, l);

   Hex::Vertex* c_grd = doc->addVertex(0,   0, 0);
   Hex::Vertex* c_pte = doc->addVertex(2*l, 0, 0);
   Hex::Vector* dx_prime = doc->addVectorVertices( c_grd, c_pte );

    //=================================================
    // Creation de la grande grille cylindrique
    //=================================================

   Hex::Elements* grille1 = doc->makeCylindrical (c_grd, dxy, dz, 
                 dr_grd, da_grd, dl_grd, nr_grd, na_grd, nl_grd, false);

   int nvtk = 0;
   doc->saveVtk ("bielle", nvtk);

   Hex::Elements* grille2 = doc->makeTranslation( grille1, dx_prime );
   doc->saveVtk ("bielle", nvtk);

   Hex::Quad* facea = grille1 ->getQuadJK (1,3,0);
   Hex::Quad* faceb = grille2 ->getQuadJK (1,1,0);

   Hex::Vertex* va1 = facea->getVertex (0);
   Hex::Vertex* va2 = facea->getVertex (1);
   Hex::Vertex* vb1 = faceb->getVertex (1);
   Hex::Vertex* vb2 = faceb->getVertex (0);

   va1->setScalar (4);
   vb1->setScalar (4);
   va2->setScalar (7);
   vb2->setScalar (7);

   doc->saveVtk ("bielle", nvtk);
   Hex::Quads quad_list;
   quad_list.push_back (facea);
   doc->joinQuads (quad_list, faceb, va1, vb1, va2, vb2, 1);

   doc->saveVtk ("bielle", nvtk);
   doc->setFile ("bielle");
   doc->saveFile ();
   return HOK;
}
