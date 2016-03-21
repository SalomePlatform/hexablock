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

#include <cppunit/extensions/HelperMacros.h>


#include "hexa_base.hxx"
#include "Hex.hxx"
#include "HexDocument.hxx"
#include "HexElements.hxx"
#include "HexCrossElements.hxx"

#include "HexHexa.hxx"
#include "HexQuad.hxx"
#include "HexEdge.hxx"
#include "HexVertex.hxx"

#include "HexPropagation.hxx"
#include "HexLaw.hxx"
#include "HexMatrix.hxx"

#include "ModelTest.hxx"

CPPUNIT_TEST_SUITE_REGISTRATION( ModelTest );


ModelTest::ModelTest(void)
{
//   _myHexEngine = new HEXA_NS::Hex();
//   _myDoc= _myHexEngine->addDocument();
}

ModelTest::~ModelTest(void)
{
//   delete _myDoc;
//   delete _myHexEngine; //CS_TODO : Crash
}


// ======================================================== print_propagations
void ModelTest::_print_propagations (Hex::Document* doc)
{
   int nb = doc->countPropagation ();
   HexDisplay (nb);
   for (int nro=0 ; nro<nb ; nro++)
       {
       Hex::Propagation*  prop  = doc ->getPropagation (nro);
       const Hex::Edges&  table = prop->getEdges ();
       printf (" ____________________________________ Prop nro %d\n", nro);
       for (int ned=0 ; ned<(int)table.size() ; ned++)
           {
           bool way = table [ned]->getWay ();
           Hex::Edge*   edge = table [ned];
           Hex::Vertex* v0   = edge->getVertex (0);
           Hex::Vertex* v1   = edge->getVertex (1);

           if (way)
              {
              printf ("     (");
              v0->printName (", ");
              v1->printName (")\n");
              }
          else
              {
              v1->printName (", ");
              v0->printName (")\n");
              printf ("     (");
              }
           }
       }
}
// // ======================================================== Test_sphere
// void ModelTest::Test_sphere ()
// {
//    Hex::Hex mon_ex;
//    Hex::Document* doc = mon_ex.addDocument ();
//    Hex::Vertex*  orig = doc->addVertex (0,0,0);
//
//    int    ncouches = 1;
//    double k = 0.8;
//    Hex::Vector*   decal  = doc->addVector (1,1,1);
//    Hex::Elements* sphere = doc->makeSpherical (orig, decal, ncouches, k);
//
//    for (int nc=0 ; nc <= ncouches ; nc++)
//        {
//        Hex::Hexa* cell = sphere->getStrate (nc, Hex::Q_A);
//        cell->remove ();
//        // sphere->getStrate (nc, Hex::Q_A)->remove ();
//        // sphere->getStrate (nc, Hex::Q_B)->remove ();
//        }
//
//    sphere->saveVtk ("sphere.vtk");
//    CPPUNIT_ASSERT( true );
// }
// ======================================================== Test_cartesi1
void ModelTest::Test_cartesi1 ()
{

   const int size_x = 15;
   const int size_y = 12;
   const int size_z = 8;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ("foo");
   Hex::Vertex* orig = doc->addVertex (0,0,0);

   Hex::Vector*   dir  = doc->addVector (1,1,1);
   Hex::Elements* grid = doc->makeCartesian (orig, dir, size_x,size_y,size_z);

   //  print_propagations (doc);

   for (int nz=0; nz<size_z ; nz++)
       for (int ny=nz+1; ny<size_y-nz-1 ; ny++)
           for (int nx=nz+1; nx<size_x-nz-1 ; nx++)
               {
               Hex::Hexa* cell = grid->getHexaIJK (nx, ny, nz);
               cell->remove ();
               }

   doc->setLevel (1);
   _print_propagations (doc);
   grid->saveVtk ("grid_cart.vtk");

   // doc->dump ();
   CPPUNIT_ASSERT( true );
}
// ======================================================== afficher
#define Afficher(elt) _afficher (#elt, elt)
int ModelTest::_afficher (cpchar nom, Hex::EltBase* elt)
{
   if (elt==NULL)
      {
      printf (" .... %s = 0x0\n", nom);
      return HOK;
      }

   printf (" .... %s = 0x%08lx = %03d\n", nom, (unsigned long) elt, elt->getId());
   return HOK;
}
// ======================================================== Test_find
void ModelTest::Test_find ()
{

   const int size_x = 2;
   const int size_y = 2;
   const int size_z = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ("foo");

   Hex::Vertex* orig = doc->addVertex (0,0,0);
   Hex::Vector* dir  = doc->addVector (1,1,1);
   Hex::Elements*  grid = doc->makeCartesian (orig, dir, size_x,size_y,size_z);

   grid->saveVtk ("mini1.vtk");
   doc->dump ();

   Hex::Vertex *v00, *v02, *v06, *v08, *v10, *v22, *v26;

   Afficher ( v00 = doc->findVertex (0, 0, 0));
   Afficher ( v02 = doc->findVertex (1, 1, 0));
   Afficher ( v06 = doc->findVertex (1, 1, 1));
   Afficher ( v08 = doc->findVertex (2, 1, 0));
   Afficher ( v10 = doc->findVertex (2, 1, 1));
   Afficher ( v22 = doc->findVertex (2, 1, 2));
   Afficher ( v26 = doc->findVertex (2, 2, 2));

   printf ("\n");

   Afficher (doc->findEdge (v06, v10));
   Afficher (doc->findEdge (v10, v06));
   printf ("\n");

   Afficher (doc->findQuad (v02, v10));
   Afficher (doc->findQuad (v06, v08));
   Afficher (doc->findQuad (v02, v06));

   printf ("\n");
   Afficher (doc->findHexa (v00, v06));
   Afficher (doc->findHexa (v06, v26));
   Afficher (doc->findHexa (v26, v06));

   CPPUNIT_ASSERT( true );
}
// ======================================================== Test_joint
void ModelTest::Test_joint ()
{

   const int dimx = 11;
   const int dimy = 11;
   const int dimz = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ("foo");

   Hex::Vertex* orig1 = doc->addVertex (0,0,0);
   Hex::Vector* dir   = doc->addVector (1,1,1);

   Hex::Elements* grid1 = doc->makeCartesian   (orig1, dir, dimx,dimy,dimz);

   Hex::Vertex*   orig2 = doc->addVertex  (dimx/2.0,0,8);
   Hex::Vector*   vectj = doc->addVector (0,1,0);
   Hex::Vector*   vecti = doc->addVector (1,0,0);
   Hex::Elements* grid2 = doc->makeCylindrical (orig2, vecti, vectj,
                   1, 180, 1,        dimz,dimy,dimx, true);

   int mx = dimx/2;
   int my = dimy/2;
   Hex::Quad* prems = grid1->getQuadIJ (mx, my, dimz);
   Hex::Quad* cible = grid2->getQuadJK (dimz, mx, my);


   Hex::Vertex* v1 = prems->getVertex (0);
   Hex::Vertex* v2 = cible->getVertex (0);
   Hex::Vertex* v3 = prems->getVertex (1);
   Hex::Vertex* v4 = cible->getVertex (3);

   Hex::Quads liste;
   Hex::Quads miroir;

   liste.push_back (prems);
   for (int nx=0; nx<dimx; nx++)
       if (nx!=mx)
          liste.push_back (grid1->getQuadIJ (nx, my, dimz));

   for (int ny=0; ny<dimy; ny++)
       if (ny!=my)
          liste.push_back (grid1->getQuadIJ (mx, ny, dimz));

   // Hex::Elements* joint =
   //
   doc->joinQuads  (liste, cible, v1, v2, v3, v4, 5);
   doc->saveVtk ("joint.vtk");

   CPPUNIT_ASSERT( true );
}
// ======================================================== Test_prism
void ModelTest::Test_prism ()
{

   const int dimx = 11;
   const int dimy = 11;
   const int dimz = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ("foo");

   Hex::Vertex* orig1 = doc->addVertex (0,0,0);
   Hex::Vector* dir   = doc->addVector (1,1,1);

   Hex::Elements* grid1 = doc->makeCartesian   (orig1, dir, dimx,dimy,dimz);

   int mx = dimx/2;
   int my = dimy/2;
   Hex::Quad* prems = grid1->getQuadIJ (mx, my, dimz);
   Hex::Quads liste;

   liste.push_back (prems);
   for (int nx=0; nx<dimx; nx++)
       if (nx!=mx)
          liste.push_back (grid1->getQuadIJ (nx, my, dimz));

   for (int ny=0; ny<dimy; ny++)
       if (ny!=my)
          liste.push_back (grid1->getQuadIJ (mx, ny, dimz));

   doc->prismQuads  (liste, dir, 5);
   doc->saveVtk ("prisme.vtk");

   CPPUNIT_ASSERT( true );
}


// ======================================================== Test_prism2
void ModelTest::Test_prism2 ()
{

    const int dimx = 11;
    const int dimy = 11;
    const int dimz = 2;

    Hex::Hex mon_ex;
    Hex::Document* doc = mon_ex.addDocument ("foo");

    Hex::Vertex* orig1 = doc->addVertex (0,0,0);
    Hex::Vector* dir   = doc->addVector (1,1,1);

    Hex::Elements* grid1 = doc->makeCartesian   (orig1, dir, dimx,dimy,dimz);

    int mx = dimx/2;
    int my = dimy/2;
    Hex::Quad* prems = grid1->getQuadIJ (mx, my, dimz);
    Hex::Elements*  prism = doc->prismQuad(prems, dir, 5);

    int nbHexa = prism->countHexa();
    std::cout << "nbHexa " << nbHexa << std::endl;
    Hex::Hexa*     hp1 = prism->getHexa(1);
    std::cout << "getHexa(1) " << hp1 << std::endl;
    Hex::Vertex*   hp1_v0 = hp1->getVertex(0);
    std::cout << "getVertex ->" << hp1_v0 << std::endl;
    doc->saveVtk ("prisme2.vtk");

    CPPUNIT_ASSERT( true );
}








// ======================================================== Test_hexa1
void ModelTest::Test_hexa1 ()
{

   const int size_x = 1;
   const int size_y = 1;
   const int size_z = 1;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ("foo");
   Hex::Vertex* orig = doc->addVertex (0,0,0);
   Hex::Vector* dir  = doc->addVector (1,1,1);
   //  Hex::Elements*  grid =
   doc->makeCartesian (orig, dir, size_x,size_y,size_z);

   doc ->dump ();

   CPPUNIT_ASSERT( true );
}
// ======================================================== Test_decoupage
void ModelTest::Test_decoupage ()
{

   const int size_x = 2;
   const int size_y = 1;
   const int size_z = 1;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ("foo");

   Hex::Vertex* orig = doc->addVertex (0,0,0);
   Hex::Vector* dir  = doc->addVector (1,1,1);

   Hex::Elements* grid  = doc->makeCartesian (orig, dir, size_x,size_y,size_z);
   Hex::Edge*     arete = grid->getEdgeK (0, 0, 0);

   doc ->dump ();
   doc ->saveVtk ("no_decoupe.vtk");
/* Hex::Elements* grid2 = */  doc->cut (arete, 1);

   doc ->dump ();
   doc ->saveVtk ("decoupe.vtk");
   // doc ->saveFile ();

   CPPUNIT_ASSERT( true );
}
// ======================================================== Test_gen_xml
void ModelTest::Test_gen_xml ()
{

   const int size_x = 2;
   const int size_y = 2;
   const int size_z = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ("foo");

   Hex::Vertex* orig = doc->addVertex (0,0,0);
   Hex::Vector* dir  = doc->addVector (1,1,1);
   Hex::Elements*  grid = doc->makeCartesian (orig, dir, size_x,size_y,size_z);

   Hex::Hexa*   cell    = grid->getHexa (0);
   Hex::Quad*   face    = cell->getQuad (0);
   Hex::Edge*   arete   = cell->getEdge (0);
   Hex::Vertex* noeud   = cell->getVertex (0);

   Hex::Law* law1 = doc->addLaw("loi1", 1);
   Hex::Law* law2 = doc->addLaw("loi2", 2);
   Hex::Law* law3 = doc->addLaw("loi3", 3);

   law1->setKind (Hex::Uniform);
   law2->setKind (Hex::Arithmetic);
   law3->setKind (Hex::Geometric);

   Hex::Propagation* prop1 = doc->getPropagation (0);
   Hex::Propagation* prop2 = doc->getPropagation (1);
   Hex::Propagation* prop3 = doc->getPropagation (2);

   prop1->setLaw (law1);
   prop2->setLaw (law2);
   prop3->setLaw (law3);

   prop1->setWay (true);
   prop2->setWay (false);
   prop3->setWay (true);

   doc ->saveVtk ("mini.vtk");
   doc ->save ("Essai");

   CPPUNIT_ASSERT( true );
}
// ======================================================== Test_relecture
void ModelTest::Test_relecture ()
{

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.loadDocument ("Essai");

   doc ->dump ();
   doc ->saveVtk ("restore.vtk");

   CPPUNIT_ASSERT( true );
}


// ======================================================== Test_clone
void ModelTest::Test_clone ()
{

   const int size_x = 2;
   const int size_y = 2;
   const int size_z = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ("foo");

   Hex::Vertex* orig = doc->addVertex (0,0,0);
   Hex::Vector* dir  = doc->addVector (1,1,1);
   Hex::Elements* grid = doc->makeCartesian (orig, dir, size_x,size_y,size_z);

   Hex::Vector*   bond  = doc->addVector (0, 0, 7);
   Hex::Elements* grid2 = doc->makeTranslation (grid, bond);

   doc ->saveVtk ("clonage.vtk");
   doc ->dump();

   HexDump (grid2->getHexa      (1));
   HexDump (grid2->getHexaIJK   (1,1,1));
   HexDump (grid2->getVertexIJK (1,1,1));

   CPPUNIT_ASSERT( true );
}
// ======================================================== Test_separ
void ModelTest::Test_separ ()
{

   const int size_x = 2;
   const int size_y = 2;
   const int size_z = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ("foo");

   Hex::Vertex* orig = doc->addVertex (0,0,0);
   Hex::Vector* dir  = doc->addVector (1,1,1);
   //  Hex::Elements*  grid =
   doc->makeCartesian (orig, dir, size_x,size_y,size_z);

   doc ->saveVtk ("separ.vtk");
   doc ->dump();

   CPPUNIT_ASSERT( true );
}
// // ======================================================== Test_cyl_karima
// void ModelTest::Test_cyl_karima ()
// {
//
//    Hex::Hex mon_ex;
//    Hex::Document* doc = mon_ex.addDocument ("foo");
//
//    Hex::Vertex* c2 = doc->addVertex (0,0,0);
//    Hex::Vector* dz = doc->addVector (0,0,1);
//    Hex::Vector* dx = doc->addVector (1,0,0);
//
//    double dr = 4.12;
//
//    double a2 = 180;
//    int    l2 = 70;
//
//    int nr2 = 5;
//    int na2 = 5;
//    int nl2 = 1;
//
// /* Hex::Elements* cyl2 = */ doc->makeCylindrical (c2, dx, dz, dr, a2, l2,
//                                                   nr2, na2, nl2, false );
//    doc->saveVtk ("cyl_karima.vtk");
//    // doc->dump ();
//
//    CPPUNIT_ASSERT( true );
// }
// ================================================== Test_grille_cyl
void ModelTest::Test_grille_cyl ()
{

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ("foo");

   Hex::Vertex* orig1 = doc->addVertex ( 0, 0,0);
   Hex::Vertex* orig2 = doc->addVertex (10, 0,0);

   Hex::Vertex* orig3 = doc->addVertex ( 0,10,0);
   Hex::Vertex* orig4 = doc->addVertex (10,10,0);

   Hex::Vertex* orig5 = doc->addVertex ( 0,20,0);
   Hex::Vertex* orig6 = doc->addVertex (10,20,0);

   Hex::Vector* vz = doc->addVector (0,0,1);
   Hex::Vector* vx = doc->addVector (1,0,0);

   double dr = 1;
   double dl = 1;
   int    nr = 2;
   int    nl = 5;
   nl = 1;

   doc->makeCylindrical(orig1,vx,vz,dr, 360, dl,nr,4,nl, true);
   doc->makeCylindrical(orig2,vx,vz,dr, 360, dl,nr,8,nl, true);
   doc->makeCylindrical(orig3,vx,vz,dr, 270, dl,nr,8,nl, true);
   doc->makeCylindrical(orig6,vx,vz,dr, 360, dl,nr,6,nl, true);
   doc->makeCylindrical(orig4,vx,vz,dr, 270, dl,nr,7,nl, true);
   doc->makeCylindrical(orig5,vx,vz,dr, 360, dl,nr,5,nl, true);
   doc->saveVtk ("cylindres.vtk");
   // doc->dump ();

   CPPUNIT_ASSERT( true );
}
// ===================================================== Test_cylinder
void ModelTest::Test_cylinder ()
{

   int    nvtk    = 0;
   cpchar fic_vtk = "cylindre";

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ("foo");

   Hex::Vertex* orig1 = doc->addVertex (0, 0,0);
   Hex::Vertex* orig2 = doc->addVertex (50,0,0);
   Hex::Vector* vz    = doc->addVector (0,0,1);
   Hex::Vector* vx    = doc->addVector (1,0,0);

   int nr  = 4;
   int nri = 3;
   int nre = nr;
   int na = 9;
   int nl = 5;

   Hex::Cylinder* cyl  = doc->addCylinder   (orig1, vz, nr, nl);
   Hex::Pipe*     pipe = doc->addPipe       (orig2, vz, nri, nre, nl);

   doc->makeCylinder (cyl,  vx, nr, na, nl);
   doc ->saveVtk (fic_vtk, nvtk);

   doc->makePipe     (pipe, vx, nr, na, nl);
   doc ->saveVtk (fic_vtk, nvtk);

   CPPUNIT_ASSERT( true );
}



void ModelTest::myTest_croix ()
{
  Hex::Hex mon_ex;
  Hex::Document* doc = mon_ex.addDocument ("foo");


//   Hex::Vector* dx1 = doc->addVector( -1, 0, 0 );
//   Hex::Vector* dz1 = doc->addVector( 0, 0, 1 );
//
//   Hex::Vertex* c_pte_1 = doc->addVertex ( 195., 0., 220. );
//   Hex::Vertex* c_grd_1 = doc->addVertex ( 300., 0., 400. );
//   Hex::Cylinder* cyl_pte_1 = doc->addCylinder( c_pte_1 , dz1 , 25 , 360 );
//   Hex::Cylinder* cyl_grd_1 = doc->addCylinder( c_grd_1 , dx1 , 50 , 140 );
//   Hex::CrossElements* cross1 = doc->makeCylinders( cyl_pte_1 , cyl_grd_1 );

  Hex::Vector* dx2 = doc->addVector(-1, 0, 0 );
  Hex::Vector* dz2 = doc->addVector( 0, 0, 1 );

  Hex::Vertex* c_pte_2 = doc->addVertex( 110., 0. , 400. );
  Hex::Vertex* c_grd_2 = doc->addVertex( 0.  , 0. , 0. );

  Hex::Cylinder* cyl_pte_2 = doc->addCylinder( c_pte_2 , dx2 , 50, 220 );
  Hex::Cylinder* cyl_grd_2 = doc->addCylinder( c_grd_2 , dz2 , 100, 800 );
  Hex::CrossElements* cross2    = doc->makeCylinders( cyl_pte_2, cyl_grd_2 );


/*
  Hex::Vector* dx2 = doc->addVector( 1, 0, 0 );
  Hex::Vector* dz2 = doc->addVector( 0, 0, 1 );

  Hex::Vertex* c_pte_2 = doc->addVertex( -130. , 0. , 400. );
  Hex::Vertex* c_grd_2 = doc->addVertex( 0. , 0. , 0. );

  Hex::Cylinder* cyl_pte_2 = doc->addCylinder( c_pte_2 , dx2 , 50, 260 );
  Hex::Cylinder* cyl_grd_2 = doc->addCylinder( c_grd_2 , dz2 , 100, 800 );

  Hex::CrossElements* cross2 = doc->makeCylinders( cyl_pte_2, cyl_grd_2 );*/
  doc->saveVtk ("/tmp/ffffffff.vtk");
  doc->dump ();

  CPPUNIT_ASSERT( true );
}




// ======================================================== Test_croix
void ModelTest::Test_croix ()
{

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ("foo");

   Hex::Vertex* ori1 = doc->addVertex ( 0,0,0);
   Hex::Vertex* ori2 = doc->addVertex (-5,0,5);
   Hex::Vector* vz   = doc->addVector ( 0,0,1);
   Hex::Vector* vx   = doc->addVector ( 1,0,0);

   int nr1 = 2;
   int nl1 = 10;
   int nr2 = 1;
   int nl2 = 10;

   Hex::Cylinder* cyl1  = doc->addCylinder (ori1, vz, nr1, nl1);
   Hex::Cylinder* cyl2  = doc->addCylinder (ori2, vx, nr2, nl2);
   Hex::CrossElements* grid = doc->makeCylinders (cyl1, cyl2);

   grid->dump();
   grid->dumpVertex();

#if 0
   for (int ny=0; ny<Hex::S_MAXI; ny++)
       {
       grid->getHexaIJK (Hex::Cyl1, 1, ny, 0)->remove ();
       grid->getHexaIJK (Hex::Cyl1, 1, ny, 1)->remove ();
       grid->getHexaIJK (Hex::Cyl1, 1, ny, 2)->remove ();
       grid->getHexaIJK (Hex::Cyl1, 1, ny, 3)->remove ();
       grid->getHexaIJK (Hex::Cyl1, 1, ny, 4)->remove ();
       grid->getHexaIJK (Hex::Cyl1, 1, ny, 5)->remove ();

       grid->getHexaIJK (Hex::Cyl2, 1, ny, 0)->remove ();
       // grid->getHexaIJK (Hex::Cyl2, 1, ny, 1)->remove ();
       // grid->getHexaIJK (Hex::Cyl2, 1, ny, 2)->remove ();
       grid->getHexaIJK (Hex::Cyl2, 1, ny, 3)->remove ();
       }

   for (int ny=0; ny<4 ;  ny++)
       {
       grid->getHexaIJK (Hex::Cyl1, 0, ny, 0)->remove ();
       grid->getHexaIJK (Hex::Cyl1, 0, ny, 1)->remove ();
       grid->getHexaIJK (Hex::Cyl1, 0, ny, 2)->remove ();
       grid->getHexaIJK (Hex::Cyl1, 0, ny, 3)->remove ();
       grid->getHexaIJK (Hex::Cyl1, 0, ny, 4)->remove ();
       grid->getHexaIJK (Hex::Cyl1, 0, ny, 5)->remove ();

       // grid->getHexaIJK (Hex::Cyl2, 0, ny, 0)->remove ();
       // grid->getHexaIJK (Hex::Cyl2, 0, ny, 1)->remove ();
       // grid->getHexaIJK (Hex::Cyl2, 0, ny, 2)->remove ();
       // grid->getHexaIJK (Hex::Cyl2, 0, ny, 3)->remove ();
       }

   /*********************************
   grid->getHexaIJK (Hex::Cyl2, 0, 3, 1)->remove ();
   grid->getHexaIJK (Hex::Cyl2, 1, Hex::S_SE, 2)->remove ();
   grid->getHexaIJK (Hex::Cyl2, 1, Hex::S_SE, 1)->remove ();

   grid->getHexaIJK (Hex::Cyl1, 1, Hex::S_SE, 3)->remove ();
   grid->getHexaIJK (Hex::Cyl1, 1, Hex::S_SE, 2)->remove ();
   grid->getHexaIJK (Hex::Cyl1, 1, Hex::S_SE, 1)->remove ();
    ********************************* */
#endif

   doc->saveVtk ("croix.vtk");
   doc->dump ();

   CPPUNIT_ASSERT( true );
}
// ======================================================== Test_pipes
void ModelTest::Test_pipes ()
{

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ("foo");

   Hex::Vertex* ori1 = doc->addVertex ( 0,0,0);
   Hex::Vertex* ori2 = doc->addVertex (-5,0,5);
   Hex::Vector* vz   = doc->addVector ( 0,0,1);
   Hex::Vector* vx   = doc->addVector ( 1,0,0);

   double h1  = 10, h2  = 10;
   double ri1 = 1,  ri2 = 1.5;
   double re1 = 2,  re2 = 3;

   Hex::Pipe* pipe1  = doc->addPipe (ori1, vz, ri1, re1, h1);
   Hex::Pipe* pipe2  = doc->addPipe (ori2, vx, ri2, re2, h2);
   Hex::CrossElements* grid = doc->makePipes (pipe1, pipe2);

   grid->dump();
   grid->dumpVertex();

   doc->saveVtk ("pipes.vtk");
   doc->dump ();
   CPPUNIT_ASSERT( true );
}
// ======================================================== Test_lorraine
void ModelTest::Test_lorraine()
{

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ("foo");

   Hex::Vertex* ori1 = doc->addVertex ( 0,0,0);
   Hex::Vertex* ori2 = doc->addVertex (-5,0,5);
   Hex::Vertex* ori3 = doc->addVertex ( 0,0,12);
   Hex::Vertex* ori4 = doc->addVertex (-5,0,17);

   Hex::Vector* vz   = doc->addVector ( 0,0,1);
   Hex::Vector* vx   = doc->addVector ( 1,0,0);

   int nl1 = 10;
   int nl2 = 10;

   double rsmall = 1;
   double rmoy   = 2;
   double rbig   = 3;

   Hex::Cylinder* cyl1  = doc->addCylinder (ori1, vz, rmoy,   nl1);
   Hex::Cylinder* cyl2  = doc->addCylinder (ori2, vx, rsmall, nl2);

   Hex::Cylinder* cyl3  = doc->addCylinder (ori3, vz, rmoy, nl1);
   Hex::Cylinder* cyl4  = doc->addCylinder (ori4, vx, rbig, nl2);

   Hex::CrossElements* grid1 = doc->makeCylinders (cyl1, cyl2);
   Hex::CrossElements* grid2 = doc->makeCylinders (cyl4, cyl3);

#define Imprimer(x) printf (#x " = ") ; if (x) x->dump() ; else printf ("NULL\n")
   const int nx_int = 0;
   const int nx_ext = 1;

   //           vc2 = grid1->getVertexIJK (Hex::Cyl2, 0,0,0);
   //           vc3 = grid2->getVertexIJK (Hex::Cyl1, 0,0,0);
                                     //    Cyl     i     j     k
   Hex::Quad* qb = grid1-> getQuadIJ (Hex::Cyl2, nx_ext, Hex::S_E, 4);
   Hex::Quad* qh = grid2-> getQuadIJ (Hex::Cyl1, nx_ext, Hex::S_N, 0);

   Hex::Vertex* vb0 = qb->getVertex (3);
   Hex::Vertex* vb1 = qb->getVertex (2);
   Hex::Vertex* vh0 = qh->getVertex (0);
   Hex::Vertex* vh1 = qh->getVertex (1);

   vb0 = grid1->getVertexIJK (Hex::Cyl2, 2, Hex::S_E,  4);  // cible
   vb1 = grid1->getVertexIJK (Hex::Cyl2, 2, Hex::S_NE, 4);
   vh0 = grid2->getVertexIJK (Hex::Cyl1, 2, Hex::S_N,  0);   // depart
   vh1 = grid2->getVertexIJK (Hex::Cyl1, 2, Hex::S_NW, 0);

   Imprimer (vh0);
   Imprimer (vh1);
   Imprimer (vb0);
   Imprimer (vb1);

   // qb->remove ();
   // qh->remove ();
   Hex::Quads hliste;

   hliste.push_back (qh);
   for (int ny=1; ny<Hex::S_MAXI; ny++)
       {
       int ns = (ny + Hex::S_N) MODULO Hex::S_MAXI;
       hliste.push_back (grid2->getQuadIJ (Hex::Cyl1, nx_ext, ns, 0));
       }

   for (int ny=0; ny<4 ;  ny++)
       hliste.push_back (grid2->getQuadIJ (Hex::Cyl1, nx_int, ny, 0));

   doc->joinQuads  (hliste, qb, vh0, vb0, vh1, vb1, 5);
   doc->saveVtk ("lorraine.vtk");
   // doc->dump ();
   CPPUNIT_ASSERT( true );
}
// ======================================================== Test_disconnect
void ModelTest::Test_disconnect ()
{

   const int size_x = 2;
   const int size_y = 2;
   const int size_z = 1;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ("foo");

   Hex::Vertex*   orig1 = doc->addVertex (0,0,0);
   Hex::Vertex*   orig2 = doc->addVertex (4,0,0);
   Hex::Vertex*   orig3 = doc->addVertex (8,0,0);

   Hex::Vector*   dir  = doc->addVector (1,1,1);
   Hex::Elements* grid1 = doc->makeCartesian (orig1, dir, size_x,size_y,size_z);
   Hex::Elements* grid2 = doc->makeCartesian (orig2, dir, size_x,size_y,size_z);
   Hex::Elements* grid3 = doc->makeCartesian (orig3, dir, size_x,size_y,size_z);

   int nvtk = 0;
   doc->setLevel (1);
   Hex::Matrix  matrice;
   Hex::Vector* ecart  = doc->addVector (0.5,0.5,0);
   matrice.defTranslation (ecart);

   Hex::Hexa* hexa1 = grid1->getHexaIJK (1,1,0);
   Hex::Hexa* hexa2 = grid2->getHexaIJK (1,1,0);
   Hex::Hexa* hexa3 = grid3->getHexaIJK   (1,1,0);

   Hex::Quad* quad  = grid1->getQuadJK  (1,1,0);
   Hex::Edge* edge  = grid2->getEdgeK   (1,2,0);
   Hex::Vertex* vertex = grid3->getVertexIJK (1,1,1);

   quad->setScalar   (5);
   edge->setScalar   (5);
   vertex->setScalar (5);

   doc->saveVtk ("Test_disco", nvtk);
   doc->disconnectQuad (hexa1, quad);

   hexa1 ->transform (&matrice);
   doc->saveVtk ("Test_disco", nvtk);

   doc->disconnectEdge (hexa2, edge);

   hexa2->transform (&matrice);
   doc->saveVtk ("Test_disco", nvtk);

   doc->disconnectVertex (hexa3, vertex);

   hexa3->transform (&matrice);
   doc->saveVtk ("Test_disco", nvtk);

   doc->dumpPropagation ();
   // doc->dump  ();

   CPPUNIT_ASSERT( true );
}
// ======================================================== Test_propagation
void ModelTest::Test_propagation ()
{

   const int size_x = 2;
   const int size_y = 1;
   const int size_z = 1;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ("foo");

   Hex::Vertex*   orig = doc->addVertex (0,0,0);
   Hex::Vector*   dir  = doc->addVector (1,1,1);
   //  Hex::Elements* grid =
   doc->makeCartesian (orig, dir, size_x,size_y,size_z);

   int nb = doc->countPropagation ();
   for (int nro=0 ; nro<nb ; nro++)
       {
       Hex::Propagation*  prop  = doc ->getPropagation (nro);
       const Hex::Edges&  table = prop->getEdges ();
       printf (" ____________________________________ Prop nro %d\n", nro);
       for (int ned=0 ; ned<(int)table.size() ; ned++)
           {
           bool way = table [ned]->getWay ();

           if (way)
              {
              printf ("     (");
              table [ned]->getVertex (0)->printName (", ");
              table [ned]->getVertex (1)->printName (")\n");
              }
          else
              {
              printf ("     (");
              table [ned]->getVertex (1)->printName (", ");
              table [ned]->getVertex (0)->printName (")\n");
              }
           }
       }

   doc->dump  ();
   doc->saveVtk ("Test_propagation.vtk");
   doc->save    ("Test_propagation");

   CPPUNIT_ASSERT( true );
}
// ======================================================== Test_move
void ModelTest::Test_move ()
{

   const int size_x = 1;
   const int size_y = 1;
   const int size_z = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ("foo");

   Hex::Vertex* orig = doc->addVertex (0,0,0);
   Hex::Vector* dir  = doc->addVector (1,1,1);
   Hex::Elements*  grid = doc->makeCartesian (orig, dir, size_x,size_y,size_z);

   Hex::Vector*   enhaut  = doc->addVector (0, 0, 5);
   Hex::Vector*   devant  = doc->addVector (5, 0, 0);
   // Hex::Vector*   agauche = doc->addVector (0, 5, 0);

   Hex::Matrix matrice;
   matrice.defTranslation (enhaut);

   Hex::Hexa* cube    = grid->getHexa (1);
   Hex::Quad* dessous = cube->getQuad (Hex::Q_A);
   dessous->dump();

   Hex::Elements* grid2 = doc->makeTranslation (grid, devant);
   /* Hex::Elements* grid3 = doc->makeTranslation (grid, agauche); */
   Hex::Hexa* cube2     = grid2->getHexa (1);

   doc ->saveVtk ("move0.vtk");

   cube ->disconnectQuad (dessous);
   cube ->transform (&matrice);
   cube2->transform (&matrice);

   doc ->saveVtk ("move1.vtk");
   doc ->dump();

   CPPUNIT_ASSERT( true );
}
// ======================================================== Test_deux_cyl
void ModelTest::Test_deux_cyl ()
{

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ("foo");

   Hex::Vertex* orig1 = doc->addVertex (0,0,-5);
   Hex::Vertex* orig2 = doc->addVertex (-5,0,0);

   Hex::Vector* vx = doc->addVector (1,0,0);
   Hex::Vector* vy = doc->addVector (0,1,0);
   Hex::Vector* vz = doc->addVector (0,0,1);

   double dr = 1;
   double da = 360; // degres
   double dl = 1.0;
   int    nr = 2;
   int    na = 6;
   int    nl = 10;

   /* Hex::Elements* cyl1 = */ doc->makeCylindrical(orig1, vy,vz, dr, da,
                                       dl,nr,na,nl, false);

   /* Hex::Elements* cyl2 = */ doc->makeCylindrical(orig2, vz,vx, dr/2, da,
                                       dl,nr,na,nl, false);
   doc->saveVtk ("deux_cylindres.vtk");
   CPPUNIT_ASSERT( true );
}
// ======================================================== Test_move2
void ModelTest::Test_transfo ()
{

   const int size_x = 1;
   const int size_y = 1;
   const int size_z = 2;

   int    nvtk    = 0;
   cpchar fic_vtk = "transfo";

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ("foo");
   doc ->setLevel (1);

   Hex::Vertex* orig = doc->addVertex (0,0,0);
   Hex::Vector* dir  = doc->addVector (1,1,1);
   Hex::Elements* grid = doc->makeCartesian (orig, dir, size_x, size_y,
                                                        size_z);
   if (grid==NULL)
      CPPUNIT_ASSERT( false );

   orig->setScalar(2);

   doc ->saveVtk (fic_vtk, nvtk);

   Hex::Vector*   devant  = doc->addVector (5, 0, 0);

   Hex::Elements* grid2 = doc->makeTranslation (grid, devant);
   if (grid2==NULL)
      CPPUNIT_ASSERT( false );
   doc ->saveVtk (fic_vtk, nvtk);

   Hex::Elements* grid3  = doc->makeScale (grid2, orig, 2);
   if (grid3==NULL)
      CPPUNIT_ASSERT( false );
   doc ->saveVtk (fic_vtk, nvtk);

   Hex::Elements* grid4 = doc->makeRotation (grid2, orig, dir, 45);
   if (grid4==NULL)
      CPPUNIT_ASSERT( false );
   doc ->saveVtk (fic_vtk, nvtk);

   Hex::Elements* grid5 = doc->makeSymmetryPoint (grid4, orig);
   if (grid5==NULL)
      CPPUNIT_ASSERT( false );

   doc ->saveVtk (fic_vtk, nvtk);

   Hex::Vector* dir1  = doc->addVector (1,0,0);
   Hex::Elements* grid6 = doc->makeSymmetryLine (grid4, orig, dir1);
   if (grid6==NULL)
      CPPUNIT_ASSERT( false );

   grid4->getHexa(0)->getVertex(0)->setScalar(3);
   grid6->getHexa(0)->getVertex(0)->setScalar(3);
   doc ->saveVtk (fic_vtk, nvtk);

   grid4->getHexa(0)->getVertex(0)->setScalar(0);
   grid6->getHexa(0)->getVertex(0)->setScalar(0);

   Hex::Elements* grid7 = doc->makeSymmetryLine (grid2, orig, dir1);
   if (grid7==NULL)
      CPPUNIT_ASSERT( false );

   grid2->getHexa(0)->getVertex(0)->setScalar(4);
   grid7->getHexa(0)->getVertex(0)->setScalar(4);
   doc ->saveVtk (fic_vtk, nvtk);

   grid2->getHexa(0)->getVertex(0)->setScalar(0);
   grid7->getHexa(0)->getVertex(0)->setScalar(0);

   Hex::Elements* grid8 = doc->makeSymmetryPlane (grid2, orig, dir1);
   if (grid8==NULL)
      CPPUNIT_ASSERT( false );

   grid2->getHexa(0)->getVertex(0)->setScalar(4);
   grid8->getHexa(0)->getVertex(0)->setScalar(4);
   doc ->saveVtk (fic_vtk, nvtk);
   grid2->getHexa(0)->getVertex(0)->setScalar(0);
   grid8->getHexa(0)->getVertex(0)->setScalar(0);

   Hex::Elements* grid9 = doc->makeSymmetryPlane (grid3, orig, dir);
   if (grid9==NULL)
      CPPUNIT_ASSERT( false );

   grid3->getHexa(0)->getVertex(0)->setScalar(4);
   grid9->getHexa(0)->getVertex(0)->setScalar(4);
   doc ->saveVtk (fic_vtk, nvtk);

   grid9->getHexa(0)->removeConnected ();
   doc ->saveVtk (fic_vtk, nvtk);

   CPPUNIT_ASSERT( true );
}





