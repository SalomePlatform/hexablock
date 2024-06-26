
// C++ : Tests unitaires sur les creations h'hexaedres

// Copyright (C) 2009-2024  CEA, EDF
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
int test_hexa_quads_5 (int nbargs, cpchar tabargs[])
{
   const int size_x = 3;
   const int size_y = 3;
   const int size_z = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Elements* grid  = doc->makeCartesianTop (size_x,size_y,size_z);

   int nvtk = 0;
   cpchar fic_vtk = "HexaQuad_5";
   doc->saveVtk (fic_vtk, nvtk);

   Hex::Hexa* hexa = grid->getHexaIJK (1,1,1);

   Hex::Quad* qa = hexa->getQuad (Hex::Q_A);
   Hex::Quad* qb = hexa->getQuad (Hex::Q_B);
   Hex::Quad* qc = hexa->getQuad (Hex::Q_C);
   Hex::Quad* qd = hexa->getQuad (Hex::Q_D);
   Hex::Quad* qe = hexa->getQuad (Hex::Q_E);
   Hex::Quad* qf = hexa->getQuad (Hex::Q_F);

   qb->setColor (5);
   doc->saveVtk (fic_vtk, nvtk);

   qb->remove ();
   doc->saveVtk (fic_vtk, nvtk);

   doc->addHexa5Quads (qa, qc, qd, qe, qf);
   doc->saveVtk (fic_vtk, nvtk);

   grid->remove ();
   HexDisplay (doc->countHexa ());
   HexDisplay (doc->countQuad ());
   HexDisplay (doc->countEdge ());
   HexDisplay (doc->countVertex ());

   doc->saveVtk (fic_vtk, nvtk);
   return HOK;
}
// ====================================================== test_hexa_quads_ab
int test_hexa_quads_ab (int nbargs, cpchar tabargs[])
{
   const int size_x = 1;
   const int size_y = 1;
   const int size_z = 3;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Elements* grid  = doc->makeCartesianTop (size_x,size_y,size_z);

   int nvtk = 0;
   cpchar fic_vtk = "HexaQuad_ab";
   doc->saveVtk (fic_vtk, nvtk);

   Hex::Hexa* hexa = grid->getHexaIJK (0, 0, 1);

   Hex::Quad* qa = hexa->getQuad (Hex::Q_A);
   Hex::Quad* qb = hexa->getQuad (Hex::Q_B);
   Hex::Quad* qc = hexa->getQuad (Hex::Q_C);
   Hex::Quad* qd = hexa->getQuad (Hex::Q_D);
   Hex::Quad* qe = hexa->getQuad (Hex::Q_E);
   Hex::Quad* qf = hexa->getQuad (Hex::Q_F);

   qc->remove ();
   qd->remove ();
   qe->remove ();
   qf->remove ();
   doc->saveVtk (fic_vtk, nvtk);

   doc->addHexa2Quads (qa, qb);
   doc->saveVtk (fic_vtk, nvtk);

   grid->remove ();
   HexDisplay (doc->countHexa ());
   HexDisplay (doc->countQuad ());
   HexDisplay (doc->countEdge ());
   HexDisplay (doc->countVertex ());

   doc->saveVtk (fic_vtk, nvtk);
   return HOK;
}
// ====================================================== test_hexa_quads_ac
int test_hexa_quads_ac (int nbargs, cpchar tabargs[])
{
   const int size_x = 2;
   const int size_y = 1;
   const int size_z = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Elements* grid  = doc->makeCartesianTop (size_x,size_y,size_z);
   int nvtk = 0;
   cpchar fic_vtk = "HexaQuad_ac";
   doc->saveVtk (fic_vtk, nvtk);

   Hex::Hexa* hexa = grid->getHexaIJK (1,0,1);

   Hex::Quad* qa = hexa->getQuad (Hex::Q_A);
   Hex::Quad* qb = hexa->getQuad (Hex::Q_B);
   Hex::Quad* qc = hexa->getQuad (Hex::Q_C);
   Hex::Quad* qd = hexa->getQuad (Hex::Q_D);
   Hex::Quad* qe = hexa->getQuad (Hex::Q_E);
   Hex::Quad* qf = hexa->getQuad (Hex::Q_F);

   qb->remove ();
   qc->remove ();
   qd->remove ();
   qf->remove ();
   doc->saveVtk (fic_vtk, nvtk);

   doc->addHexa2Quads (qa, qe);
   doc->saveVtk (fic_vtk, nvtk);

   grid->remove ();
   HexDisplay (doc->countHexa ());
   HexDisplay (doc->countQuad ());
   HexDisplay (doc->countEdge ());
   HexDisplay (doc->countVertex ());

   HexDisplay (doc->countUsedHexa ());
   HexDisplay (doc->countUsedQuad ());
   HexDisplay (doc->countUsedEdge ());
   HexDisplay (doc->countUsedVertex ());

   doc->saveVtk (fic_vtk, nvtk);
   return HOK;
}
// ====================================================== test_hexa_quads_ace
int test_hexa_quads_ace (int nbargs, cpchar tabargs[])
{
   const int size_x = 2;
   const int size_y = 2;
   const int size_z = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Elements* grid  = doc->makeCartesianTop (size_x,size_y,size_z);
   int nvtk = 0;
   cpchar fic_vtk = "HexaQuad_ace";
   doc->saveVtk (fic_vtk, nvtk);

   Hex::Hexa* hexa = grid->getHexaIJK (1,0,1);

   Hex::Quad* qa = hexa->getQuad (Hex::Q_A);
   Hex::Quad* qb = hexa->getQuad (Hex::Q_B);
   Hex::Quad* qc = hexa->getQuad (Hex::Q_C);
   Hex::Quad* qd = hexa->getQuad (Hex::Q_D);
   Hex::Quad* qe = hexa->getQuad (Hex::Q_E);
   Hex::Quad* qf = hexa->getQuad (Hex::Q_F);

   qb->remove ();
   qf->remove ();
   qc->remove ();
   doc->saveVtk (fic_vtk, nvtk);

   doc->addHexa3Quads (qa, qd, qe);
   doc->saveVtk (fic_vtk, nvtk);

   grid->remove ();
   HexDisplay (doc->countHexa ());
   HexDisplay (doc->countQuad ());
   HexDisplay (doc->countEdge ());
   HexDisplay (doc->countVertex ());

   HexDisplay (doc->countUsedHexa ());
   HexDisplay (doc->countUsedQuad ());
   HexDisplay (doc->countUsedEdge ());
   HexDisplay (doc->countUsedVertex ());

   doc->saveVtk (fic_vtk, nvtk);
   return HOK;
}
// ====================================================== test_hexa_quads_acd
int test_hexa_quads_acd (int nbargs, cpchar tabargs[])
{
   const int size_x = 3;
   const int size_y = 1;
   const int size_z = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Elements* grid  = doc->makeCartesianTop (size_x,size_y,size_z);
   int nvtk = 0;
   cpchar fic_vtk = "HexaQuad_acd";
   doc->saveVtk (fic_vtk, nvtk);

   Hex::Hexa* hexa = grid->getHexaIJK (1,0,1);

   Hex::Quad* qa = hexa->getQuad (Hex::Q_A);
   Hex::Quad* qb = hexa->getQuad (Hex::Q_B);
   Hex::Quad* qc = hexa->getQuad (Hex::Q_C);
   Hex::Quad* qd = hexa->getQuad (Hex::Q_D);
   Hex::Quad* qe = hexa->getQuad (Hex::Q_E);
   Hex::Quad* qf = hexa->getQuad (Hex::Q_F);

   qb->remove ();
   qc->remove ();
   qd->remove ();
   doc->saveVtk (fic_vtk, nvtk);

   doc->addHexa3Quads (qa, qe, qf);
   doc->saveVtk (fic_vtk, nvtk);

   grid->remove ();
   HexDisplay (doc->countHexa ());
   HexDisplay (doc->countQuad ());
   HexDisplay (doc->countEdge ());
   HexDisplay (doc->countVertex ());

   HexDisplay (doc->countUsedHexa ());
   HexDisplay (doc->countUsedQuad ());
   HexDisplay (doc->countUsedEdge ());
   HexDisplay (doc->countUsedVertex ());

   doc->saveVtk (fic_vtk, nvtk);
   return HOK;
}
// ====================================================== test_hexa_quads_abcd
int test_hexa_quads_abcd (int nbargs, cpchar tabargs[])
{
   const int size_x = 3;
   const int size_y = 1;
   const int size_z = 3;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Elements* grid  = doc->makeCartesianTop (size_x,size_y,size_z);
   int nvtk = 0;
   cpchar fic_vtk = "HexaQuad_abcd";
   doc->saveVtk (fic_vtk, nvtk);

   Hex::Hexa* hexa = grid->getHexaIJK (1,0,1);

   Hex::Quad* qa = hexa->getQuad (Hex::Q_A);
   Hex::Quad* qb = hexa->getQuad (Hex::Q_B);
   Hex::Quad* qc = hexa->getQuad (Hex::Q_C);
   Hex::Quad* qd = hexa->getQuad (Hex::Q_D);
   Hex::Quad* qe = hexa->getQuad (Hex::Q_E);
   Hex::Quad* qf = hexa->getQuad (Hex::Q_F);

   qc->remove ();
   qd->remove ();
   doc->saveVtk (fic_vtk, nvtk);

   doc->addHexa4Quads (qa, qe, qf, qb);
   doc->saveVtk (fic_vtk, nvtk);

   grid->remove ();
   HexDisplay (doc->countHexa ());
   HexDisplay (doc->countQuad ());
   HexDisplay (doc->countEdge ());
   HexDisplay (doc->countVertex ());

   HexDisplay (doc->countUsedHexa ());
   HexDisplay (doc->countUsedQuad ());
   HexDisplay (doc->countUsedEdge ());
   HexDisplay (doc->countUsedVertex ());

   doc->saveVtk (fic_vtk, nvtk);
   return HOK;
}
// ====================================================== test_hexa_quads_abce
int test_hexa_quads_abce (int nbargs, cpchar tabargs[])
{
   const int size_x = 3;
   const int size_y = 2;
   const int size_z = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Elements* grid = doc->makeCartesianTop (size_x,size_y,size_z);
   int nvtk = 0;
   cpchar fic_vtk = "HexaQuad_abce";
   doc->saveVtk (fic_vtk, nvtk);

   Hex::Hexa* hexa = grid->getHexaIJK (1,0,1);

   Hex::Quad* qa = hexa->getQuad (Hex::Q_A);
   Hex::Quad* qb = hexa->getQuad (Hex::Q_B);
   Hex::Quad* qc = hexa->getQuad (Hex::Q_C);
   Hex::Quad* qd = hexa->getQuad (Hex::Q_D);
   Hex::Quad* qe = hexa->getQuad (Hex::Q_E);
   Hex::Quad* qf = hexa->getQuad (Hex::Q_F);

   qc->remove ();
   qb->remove ();
   doc->saveVtk (fic_vtk, nvtk);

   doc->addHexa4Quads (qa, qe, qf, qd);
   doc->saveVtk (fic_vtk, nvtk);

   grid->remove ();
   HexDisplay (doc->countHexa ());
   HexDisplay (doc->countQuad ());
   HexDisplay (doc->countEdge ());
   HexDisplay (doc->countVertex ());

   HexDisplay (doc->countUsedHexa ());
   HexDisplay (doc->countUsedQuad ());
   HexDisplay (doc->countUsedEdge ());
   HexDisplay (doc->countUsedVertex ());

   doc->saveVtk (fic_vtk, nvtk);
   return HOK;
}
// ======================================================== test_cramer
int test_cramer (int nbargs, cpchar tabargs[])
{
   double matrice [] = { 2, 7, 1, 
                         0, 3, 0,
                         1, 9, 5 };
   double second  [] = { 1, 1, 1 };

   Hex::Real3  solution;
   Hex::Cramer systeme(3);

   int ier = systeme.resoudre (matrice, second, solution);
   HexDisplay (ier);
   cout << endl;
   cout << " Second membre = (" << second[Hex::dir_x] << ", "
        << second[Hex::dir_y] << ", " << second[Hex::dir_z] << ")" 
        << endl;

   cout << " Solution      = (" << solution[Hex::dir_x] << ", "
        << solution[Hex::dir_y] << ", " << solution[Hex::dir_z] << ")" 
        << endl;

   Hex::Real3 produit;
   systeme.multiply (solution, produit);
   cout << " Produit       = (" << produit[Hex::dir_x] << ", "
        << produit[Hex::dir_y] << ", " << produit[Hex::dir_z] << ")" 
        << endl;

   return HOK;
}
// ====================================================== test_hexa_quads_ac1
int test_hexa_quads_ac1 (int nbargs, cpchar tabargs[])
{
   int nbtraces = 0;
   cout << "test_hexa_quads_ac1, trace nro " << ++nbtraces <<endl;
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex* ace = doc->addVertex (0,0,0);
   Hex::Vertex* acf = doc->addVertex (1,0,0);

   /****************************************
   Hex::Vertex* ade = doc->addVertex (0,1,0);
   Hex::Vertex* adf = doc->addVertex (1,1,0);

   Hex::Vertex* bce = doc->addVertex (0,0,1);
   Hex::Vertex* bcf = doc->addVertex (1,0,1);
   Hex::Vertex* bde = doc->addVertex (0,1,1);
   Hex::Vertex* bdf = doc->addVertex (1,1,1);
   *****************************************/

   Hex::Vertex* ade = doc->addVertex (-2,2,0);
   Hex::Vertex* adf = doc->addVertex ( 2,1,0);
   Hex::Vertex* bce = doc->addVertex ( 1,0,1);
   Hex::Vertex* bcf = doc->addVertex ( 2,0,1);

   Hex::Edge* ac = doc->addEdge (ace, acf);
   Hex::Edge* af = doc->addEdge (adf, acf);
   Hex::Edge* ad = doc->addEdge (ade, adf);
   Hex::Edge* ae = doc->addEdge (ace, ade);

   Hex::Edge* bc = doc->addEdge (bce, bcf);
   // Hex::Edge* bf = doc->addEdge (bdf, bcf);
   // Hex::Edge* bd = doc->addEdge (bde, bdf);
   // Hex::Edge* be = doc->addEdge (bce, bde);

   Hex::Edge* ce = doc->addEdge (ace, bce);
   Hex::Edge* cf = doc->addEdge (acf, bcf);
   // Hex::Edge* de = doc->addEdge (ade, bde);
   // Hex::Edge* df = doc->addEdge (adf, bdf);

   Hex::Quad* qa = doc->addQuad (ac, af, ad, ae);
   Hex::Quad* qc = doc->addQuad (ac, cf, bc, ce);

   /* Hex::Hexa* hexa = */  doc->addHexa2Quads (qa, qc);

   // doc->dump();
   doc->saveVtk ("hexa_quads.vtk");
   return HOK;
}
// ======================================================== test_piquage
int test_piquage (int nbargs, cpchar tabargs[])
{
   const int size_x = 3;
   const int size_y = 3;
   const int size_z = 3;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vector* vx   = doc->addVector (1,0,0);
   Hex::Vector* vz   = doc->addVector (0,0,1);

   Hex::Elements* grid    = doc->makeCartesianTop (size_x,size_y,size_z);

   double dl = 1;

   int nr = 1;
   int na = 4;
   int nl = 1;

   Hex::Vertex*   orig2 = doc->addVertex (0,0,5);
   Hex::Elements* grid2 = doc->makeCylinderUni (orig2, vx,vz, 1.0, 5.0, 
                                                360, dl, nr, na, nl);

   Hex::Vertex* c1 = grid->getVertexIJK (1, 2, size_z);
   Hex::Vertex* c2 = grid->getVertexIJK (1, 1, size_z);
   Hex::Vertex* c3 = grid->getVertexIJK (2, 1, size_z);

   Hex::Vertex* p1 = grid2->getVertexIJK (1, 0, 1);
   Hex::Vertex* p2 = grid2->getVertexIJK (1, 1, 1);
   Hex::Vertex* p3 = grid2->getVertexIJK (1, 2, 1);

   c1->setColor (4);
   c2->setColor (6);
   c3->setColor (6);

   p1->setColor (4);
   p2->setColor (6);
   p3->setColor (6);

   int nvtk=0;
   doc->saveVtk ("piquage", nvtk);

   Hex::Quads qpattern;
   for (int ny=0; ny<4 ; ny++)
       {
       Hex::Quad* quad = grid2->getQuadIJ (0, ny, 1);
       quad->setColor(5);
       qpattern.push_back (quad);
       }
   
   doc->saveVtk ("piquage", nvtk);
   doc->replace (qpattern, p1,c1, p2,c2, p3,c3);

   doc->saveVtk ("piquage", nvtk);

   return HOK;
}
// ======================================================== test_replace0
int test_replace0 (int nbargs, cpchar tabargs[])
{
   const int size_x = 3;
   const int size_y = 3;
   const int size_z = 3;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Elements* grid    = doc->makeCartesianTop (size_x,size_y,size_z);

   Hex::Vertex* c1 = grid->getVertexIJK (1, 2, size_z);
   Hex::Vertex* c2 = grid->getVertexIJK (1, 1, size_z);
   Hex::Vertex* c3 = grid->getVertexIJK (2, 1, size_z);

   Hex::Vertex* pa1 = doc->addVertex (-1, -1);
   Hex::Vertex* pb1 = doc->addVertex ( 1, -1);
   Hex::Vertex* pc1 = doc->addVertex ( 1,  1);
   Hex::Vertex* pd1 = doc->addVertex (-1,  1);

   Hex::Vertex* pa2 = doc->addVertex (-2, -2);
   Hex::Vertex* pb2 = doc->addVertex ( 2, -2);
   Hex::Vertex* pc2 = doc->addVertex ( 2,  2);
   Hex::Vertex* pd2 = doc->addVertex (-2,  2);

   Hex::Edge* edab1 = doc->addEdge (pa1, pb1);
   Hex::Edge* edbc1 = doc->addEdge (pb1, pc1);
   Hex::Edge* edcd1 = doc->addEdge (pc1, pd1);
   Hex::Edge* edda1 = doc->addEdge (pd1, pa1);

   Hex::Edge* edab2 = doc->addEdge (pa2, pb2);
   Hex::Edge* edbc2 = doc->addEdge (pb2, pc2);
   Hex::Edge* edcd2 = doc->addEdge (pc2, pd2);
   Hex::Edge* edda2 = doc->addEdge (pd2, pa2);

   Hex::Edge* edaa = doc->addEdge (pa1, pa2);
   Hex::Edge* edbb = doc->addEdge (pb1, pb2);
   Hex::Edge* edcc = doc->addEdge (pc1, pc2);
   Hex::Edge* eddd = doc->addEdge (pd1, pd2);

   Hex::Quads qpattern;
   // qpattern.push_back (doc->addQuad (edab1, edbc1, edcd1, edda1));
   qpattern.push_back (doc->addQuad (edab1, edbb,  edab2, edaa));
   qpattern.push_back (doc->addQuad (edbc1, edcc,  edbc2, edbb));
   qpattern.push_back (doc->addQuad (edcd1, eddd,  edcd2, edcc));
   qpattern.push_back (doc->addQuad (edda1, edaa,  edda2, eddd));

   doc->saveVtk ("replace0.vtk");

   doc->replace (qpattern, pd2,c1, pa2,c2, pb2,c3);

   c1->setColor (4);
   c2->setColor (6);
   c3->setColor (6);
   doc->saveVtk ("replace1.vtk");

   return HOK;
}
// ======================================================== test_replace1
int test_replace1 (int nbargs, cpchar tabargs[])
{
   const int size_x   = 3;
   const int size_y   = 3;
   const int size_z   = 3;
   const int size_cyl = 4;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();
   Hex::Vector*   vx  = doc->addVector (1, 0, 0);
   Hex::Vector*   vz  = doc->addVector (0, 0, 1);
   Hex::Vertex*   ori = doc->addVertex (8, 0, 0);
   const double   rint  = 1;
   const double   rext  = 3;
   const double   angle = 360;
   const double   haut  = 1;
   const int      nr    = 1;
   const int      nh    = 1;

   Hex::Elements* grid = doc->makeCartesianTop (size_x,size_y,size_z);
   Hex::Elements* pipe = doc->makePipeUni  (ori, vx, vz, rint, rext, angle,
                                            haut, nr, size_cyl, nh); 

   Hex::Vertex* c1 = grid->getVertexIJK (1, 2, size_z);
   Hex::Vertex* c2 = grid->getVertexIJK (1, 1, size_z);
   Hex::Vertex* c3 = grid->getVertexIJK (2, 1, size_z);

   Hex::Vertex* p1 = pipe->getVertexIJK (1, 1, 1);
   Hex::Vertex* p2 = pipe->getVertexIJK (1, 2, 1);
   Hex::Vertex* p3 = pipe->getVertexIJK (1, 3, 1);

   Hex::Quads qpattern;
   for (int na=0; na<size_cyl;  ++na)
       {
       Hex::Quad* quad =pipe->getQuadIJ (0, na, 1);
       quad->setColor (2);
       qpattern.push_back (quad);
       }

   c1->setColor (6);
   c2->setColor (4);
   c3->setColor (2);

   p1->setColor (6);
   p2->setColor (5);
   p3->setColor (4);
   doc->saveVtk ("replace0.vtk");

   Hex::Elements* rep = doc->replaceHexa (qpattern, p1,c1, p2,c2, p3,c3);
   HexDisplay (rep->isValid());
   Hex::what ();

   doc->saveVtk ("replace1.vtk");
   doc->saveVtk ("replace2.vtk");
   doc->saveVtk ("replace3.vtk");
   return HOK;
}
// ======================================================== test_replace
int test_replace (int nbargs, cpchar tabargs[])
{
   const int size_x   = 5;
   const int size_y   = 5;
   const int size_z   = 3;
   const int size_cyl = 12;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();
   Hex::Vector*   vx  = doc->addVector (1, 0, 0);
   Hex::Vector*   vz  = doc->addVector (0, 0, 1);
   Hex::Vertex*   ori = doc->addVertex (8, 0, 0);
   const double   rint  = 2;
   const double   rext  = 3;
   const double   angle = 360;
   const double   haut  = 1;
   const int      nr    = 1;
   const int      nh    = 1;

   Hex::Elements* grid = doc->makeCartesianTop (size_x,size_y,size_z);
   Hex::Elements* pipe = doc->makePipeUni  (ori, vx, vz, rint, rext, angle,
                                            haut, nr, size_cyl, nh); 

   Hex::Vertex* c1 = grid->getVertexIJK (2, 1, size_z);
   Hex::Vertex* c2 = grid->getVertexIJK (3, 1, size_z);

   Hex::Vertex* p1 = pipe->getVertexIJK (1, 7, 1);
   Hex::Vertex* p2 = pipe->getVertexIJK (1, 8, 1);

   Hex::Quads qpattern, qtarget;
   for (int na=0; na<size_cyl;  ++na)
       {
       Hex::Quad* quad =pipe->getQuadIJ (0, na, 1);
       quad->setColor (2);
       qpattern.push_back (quad);
       }

   for (int ni=1; ni<size_x-1;  ++ni)
       for (int nj=1; nj<size_y-1;  ++nj)
           {
           cout << " grid->getQuad (" << ni << "," << nj << ")\n";
           Hex::Quad* quad = grid->getQuadIJ (ni, nj, size_z);
           quad->setColor (2);
           qtarget.push_back (quad);
           }

   c1->setColor (6);
   c2->setColor (4);

   p1->setColor (6);
   p2->setColor (4);
   doc->saveVtk ("replace0.vtk");

   Hex::Elements* rep = doc->replace (qpattern, qtarget, p1,c1, p2,c2);
   HexDisplay (rep->isValid());
   Hex::what ();

   doc->saveVtk ("replace1.vtk");
   doc->saveVtk ("replace2.vtk");
   return HOK;
}
// ======================================================== test_quads
int test_quads (int nbargs, cpchar tabargs[])
{
   cpchar option = "?";
   cpchar appel  = strrchr (tabargs[0], '/');
   if (appel != NULL)
      appel += 1;
   else 
      {
      appel  = strrchr (tabargs[0], '\\');
      if (appel!=NULL)
         appel += 1;
      else 
         appel = tabargs[0];
      }

   if (nbargs>1) 
      option = tabargs[1];

   if (Cestegal (option, "-ab") || Cestegal (option, "-AB"))
      {
      printf (" __________________ Appel de test_hexa_quads_ab \n");
      test_hexa_quads_ab (nbargs, tabargs);
      }

   else if (Cestegal (option, "-ac1") || Cestegal (option, "-AC1"))
      {
      printf (" __________________ Appel de test_hexa_quads_ac1 \n");
      test_hexa_quads_ac1 (nbargs, tabargs);
      }
   else if (Cestegal (option, "-ac") || Cestegal (option, "-AC"))
      {
      printf (" __________________ Appel de test_hexa_quads_ac \n");
      test_hexa_quads_ac (nbargs, tabargs);
      }

   else if (Cestegal (option, "-ace") || Cestegal (option, "-ACE"))
      {
      printf (" __________________ Appel de test_hexa_quads_ace \n");
      test_hexa_quads_ace (nbargs, tabargs);
      }

   else if (Cestegal (option, "-acd") || Cestegal (option, "-ACD"))
      {
      printf (" __________________ Appel de test_hexa_quads_acd \n");
      test_hexa_quads_acd (nbargs, tabargs);
      }

   else if (Cestegal (option, "-abcd") || Cestegal (option, "-ABCD"))
      {
      printf (" __________________ Appel de test_hexa_quads_abcd \n");
      test_hexa_quads_abcd (nbargs, tabargs);
      }

   else if (Cestegal (option, "-abce") || Cestegal (option, "-ABCE"))
      {
      printf (" __________________ Appel de test_hexa_quads_abce \n");
      test_hexa_quads_abce (nbargs, tabargs);
      }

   else if (Cestegal (option, "-5") || Cestegal (option, "-abcde")
                                   || Cestegal (option, "-ABCDE"))
      {
      printf (" __________________ Appel de test_hexa_quads_abcde \n");
      test_hexa_quads_5 (nbargs, tabargs);
      }

   else 
      {
      printf (" +++ Veuillez indiquer votre cas-test en argument SVP\n");
      printf (" +++ Creation d'un hexadedre a partir de quadrangles :\n");
      printf (" %s -ab   : A partir des quadrangles A et B (opposes)\n", appel);
      printf (" %s -ac   : A partir des quadrangles A et C (diedre)\n",  appel);
      printf (" %s -acd  : A partir des quadrangles A, C et D (triedre)\n", 
              appel);
      printf (" %s -ace  : A partir des quadrangles A, C et E (en U)\n", 
              appel);
      printf (" %s -acbd : A partir des quadrangles A, B, C et D (tunnel)\n", 
              appel);
      printf (" %s -acbe : A partir des quadrangles A, B, C et E (but)\n", 
              appel);
      printf (" %s -5    : A partir de 5 quadrangles (boite sans couvercle)\n", 
              appel);
      }

   return HOK;
}
