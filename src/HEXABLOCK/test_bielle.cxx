
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

   double R = 1;
   double h = 1;

    // Taille du grand cylindre
   double dr_grd = R;
   double da_grd = 360;
   double dl_grd = h;

   int nr_grd = 1;
   int na_grd = 4;
   int nl_grd = 1;

   Hex::Vector* dx    = doc->addVector (1, 0, 0);
   Hex::Vector* dz    = doc->addVector (0, 0, 1);
   Hex::Vector* decal = doc->addVector (5, 5, 0);

   Hex::Vertex* center = doc->addVertex (0, 0, 0);

    //=================================================
    // Creation de la grande grille cylindrique
    //=================================================

   Hex::Elements* grille1 = doc->makeCylindrical (center, dx, dz, 
                 dr_grd, da_grd, dl_grd, nr_grd, na_grd, nl_grd, false);

   doc->setFile ("bielle1");
   doc->saveFile ();

   int nvtk = 0;
   //  doc->saveVtk ("bielle", nvtk);

   Hex::Elements* grille2 = doc->makeTranslation( grille1, decal );

   doc->setFile ("bielle2");
   doc->saveFile ();
   doc->saveVtk ("bielle", nvtk);

   Hex::Quad* facea = grille1 ->getQuadJK (1,0,0);
   Hex::Quad* faceb = grille2 ->getQuadJK (1,2,0);

   //  facea->setScalar (4);
   //  faceb->setScalar (4);

   Hex::Vertex* va1 = facea->getVertex (0);
   Hex::Vertex* vb1 = faceb->getVertex (1);

   Hex::Vertex* va2 = facea->getVertex (3);
   Hex::Vertex* vb2 = faceb->getVertex (2);

   // va1->setScalar (4);
   // vb1->setScalar (4);
   va2->setScalar (7);
   vb2->setScalar (7);

   //  doc->saveVtk ("bielle", nvtk);
   Hex::Quads quad_list;
   quad_list.push_back (facea);
   return HOK;

   doc->joinQuads (quad_list, faceb, va1, vb1, va2, vb2, 3);

   // doc->saveVtk ("bielle", nvtk);
   doc->setFile ("bielle");
   doc->saveFile ();
   return HOK;
}
