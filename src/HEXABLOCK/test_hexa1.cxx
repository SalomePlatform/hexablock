
// C++ : Tests unitaires

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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "Hex.hxx"
#include "HexDocument.hxx"
#include "HexElements.hxx"
#include "HexCrossElements.hxx"

#include "HexHexa.hxx"
#include "HexQuad.hxx"
#include "HexEdge.hxx"
#include "HexVertex.hxx"

#include "HexPropagation.hxx"
#include "HexShape.hxx"
#include "HexLaw.hxx"
#include "HexMatrix.hxx"
#include "HexCramer.hxx"
#include "HexGroup.hxx"

// ======================================================== print_propagations
void print_propagations (Hex::Document* doc)
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
// ======================================================== test_sphere
int test_sphere ()
{
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();
   Hex::Vertex*  orig = doc->addVertex (0,0,0);

   int    ncouches = 1;
   double k = 0.8;
   Hex::Vector*   decal  = doc->addVector (1,1,1);
   Hex::Elements* sphere = doc->makeSpherical (orig, decal, ncouches, k);

   for (int nc=0 ; nc <= ncouches ; nc++)
       {
       Hex::Hexa* cell = sphere->getStrate (nc, Hex::Q_A);
       cell->remove ();
       // sphere->getStrate (nc, Hex::Q_A)->remove ();
       // sphere->getStrate (nc, Hex::Q_B)->remove ();
       }

   sphere->saveVtk ("sphere.vtk");
   return HOK;
}
// ======================================================== test_cartesi1
int test_cartesi1 ()
{
   const int size_x = 15;
   const int size_y = 12;
   const int size_z = 8;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();
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
   print_propagations (doc);
   grid->saveVtk ("grid_cart.vtk");

   // doc->dump ();
   return HOK;
}
// ======================================================== afficher
#define Afficher(elt) afficher (#elt, elt)
int afficher (cpchar nom, Hex::EltBase* elt)
{
   if (elt==NULL)
      {
      printf (" .... %s = 0x0\n", nom);
      return HOK;
      }

   printf (" .... %s = 0x%08lx = %03d\n", nom, (unsigned long) elt, elt->getId());
   return HOK;
}
// ======================================================== test_find
int test_find ()
{
   const int size_x = 2;
   const int size_y = 2;
   const int size_z = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

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

   return HOK;
}
// ======================================================== test_joint
int test_joint (int nbargs, cpchar tabargs[])
{
   const int dimx = 11;
   const int dimy = 11;
   const int dimz = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex* orig1 = doc->addVertex (0,0,0);
   Hex::Vector* dir   = doc->addVector (1,1,1);

   Hex::Elements* grid1 = doc->makeCartesian   (orig1, dir, dimx,dimy,dimz);

   Hex::Vertex*   orig2 = doc->addVertex  (dimx/2.0,0,8);
   Hex::Vector*   vectj = doc->addVector (0,1,0);
   Hex::Vector*   vecti = doc->addVector (1,0,0);
   Hex::Elements* grid2 = doc->makeCylindrical (orig2, vecti, vectj, 
                   1, 180, 1,        dimz,dimy,dimx);

   int mx = dimx/2;
   int my = dimy/2;
   Hex::Quad* prems = grid1->getQuadIJ (mx, my, dimz); 
   Hex::Quad* cible = grid2->getQuadJK (dimz, mx, my); 

   Hex::Vertex* v1 = prems->getVertex (0);
   Hex::Vertex* v3 = prems->getVertex (1);

   Hex::Vertex* v2 = cible->getVertex (1);
   Hex::Vertex* v4 = cible->getVertex (2);

   // v1->setScalar (3);
   // v2->setScalar (3);
   // v3->setScalar (6);
   // v4->setScalar (6);

   Hex::Quads liste;

   liste.push_back (prems);
   for (int nx=0; nx<dimx; nx++)
       if (nx!=mx) 
          liste.push_back (grid1->getQuadIJ (nx, my, dimz)); 

   for (int ny=0; ny<dimy; ny++)
       if (ny!=my) 
          liste.push_back (grid1->getQuadIJ (mx, ny, dimz)); 

   doc->saveVtk ("joint1.vtk");
   const int hauteur = 5;
   Hex::Elements* joint = doc->joinQuads(liste, cible, v1,v2,v3,v4, hauteur);
   // for (int nh=0 ; nh<hauteur ; nh++) joint->getHexa(nh)->setScalar (5);

   int nbr_joint_vertex =  joint->countVertex ();
   int nbr_surf_vertex  =  nbr_joint_vertex/(hauteur+1);

   HexDisplay (nbr_joint_vertex);
   HexDisplay (nbr_surf_vertex);

   int indice0 = joint->findVertex (v1);
   HexDisplay (indice0);

   for (int nh=0 ; nh<nbr_surf_vertex ; nh++) 
       joint->getVertex(nh)->setScalar (5);

   for (int nh=0 ; nh<=hauteur ; nh++) 
       joint->getVertex(nh*nbr_surf_vertex)->setScalar (3);

   doc->saveVtk ("joint2.vtk");
   return HOK;
}
// ======================================================== test_prism
int test_prism ()
{
   const int dimx = 11;
   const int dimy = 11;
   const int dimz = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

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

   doc->saveVtk ("prisme1.vtk");
   /* Hex::Elements* prisme = */ doc->prismQuads  (liste, dir, 5);
   doc->saveVtk ("prisme2.vtk");

   return HOK;
}
// ======================================================== test_revolution
int test_revolution ()
{
   const int dimx = 11;
   const int dimy = 11;
   const int dimz = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex* orig1 = doc->addVertex (0,0,0);
   Hex::Vector* dir   = doc->addVector (1,1,1);

   Hex::Elements* grid1 = doc->makeCartesian   (orig1, dir, dimx,dimy,dimz);

   int mx = dimx/2;
   int my = dimy/2;
   Hex::Quad* prems = grid1->getQuadIJ (mx, my, dimz); 
   Hex::Quads liste;

   liste.push_back (prems);
   prems -> setScalar (5);
   for (int nx=0; nx<dimx; nx++)
       if (nx!=mx) 
          {
          Hex::Quad* cell = grid1->getQuadIJ (nx, my, dimz); 
          liste.push_back (cell);
          cell -> setScalar (5);
          }

   for (int ny=0; ny<dimy; ny++)
       if (ny!=my) 
          {
          Hex::Quad* cell = grid1->getQuadIJ (mx, ny, dimz); 
          liste.push_back (cell);
          cell -> setScalar (5);
          }


   Hex::Vertex* center = doc->addVertex (0, -10, 0);
   Hex::Vector* axis   = doc->addVector (1, 0, 0);
   Hex::RealVector  angles;

   Hex::Vector*   dir1  = doc->addVector (10,0.3,0.3);
   Hex::Elements* grid2 = doc->makeCartesian (center, dir1, 1,1,1);
   Hex::Hexa*     cell  = grid2->getHexaIJK (0,0,0);
   cell->setScalar (5);

   doc->saveVtk ("revolution1.vtk");

   double alpha = 5;
   int niter    = 5;
   double coeff = 1.5;
   for (int na=0 ; na<niter ; na++)
       {
       angles.push_back (alpha);
       alpha *= coeff;
       }
   for (int na=1 ; na<niter ; na++)
       {
       alpha /= coeff;
       angles.push_back (alpha);
       }


   Hex::Elements* bloc = doc->revolutionQuads  (liste, center, axis, angles);
   if (bloc != NULL)
       doc->saveVtk ("revolution2.vtk");

   return HOK;
}
// ======================================================== test_count
int test_count (int nbargs, cpchar tabargs[])
{
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex* orig1 = doc->addVertex ( 0, 0,0);
   Hex::Vector* vx    = doc->addVector (1,0,0);
   Hex::Vector* vz    = doc->addVector (0,0,1);

   double dr = 1;
   double dl = 1;
   int    nr = 2;
   int    nl = 3;

   Hex::Elements* c1;

   c1 = doc->makeCylindrical (orig1, vx,vz,dr, 360, dl,nr, 10, nl, false);

   HexDisplay (doc->countVertex ());
   HexDisplay (doc->countUsedVertex ());
   doc ->saveVtk ("hexa1.vtk");

   return HOK;
}
// ======================================================== test_decoupage
int test_decoupage (int nbargs, cpchar tabargs[])
{
   const int size_x = 2;
   const int size_y = 1;
   const int size_z = 1;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex* orig = doc->addVertex (0,0,0);
   Hex::Vector* dir  = doc->addVector (1,1,1);

   Hex::Elements* grid  = doc->makeCartesian (orig, dir, size_x,size_y,size_z);
   Hex::Edge*     arete = grid->getEdgeK (0, 0, 0);

   //  doc ->dump ();
   int nvtk=0;
   doc ->saveVtk ("decoupe", nvtk);
/* Hex::Elements* grid2 = */  doc->cut (arete, 1);

   ///  doc ->dump ();
   doc ->saveVtk ("decoupe", nvtk);
   // doc ->saveFile ();

   return HOK;
}
// ======================================================== test_gen_xml
int test_gen_xml (int nbargs, cpchar tabargs[])
{
   const int size_x = 2;
   const int size_y = 2;
   const int size_z = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex* orig = doc->addVertex (0,0,0);
   Hex::Vector* dir  = doc->addVector (1,1,1);
   Hex::Elements*  grid = doc->makeCartesian (orig, dir, size_x,size_y,size_z);

   Hex::Hexa*   cell    = grid->getHexa (0);
   Hex::Quad*   face    = cell->getQuad (0);
   Hex::Edge*   arete   = cell->getEdge (0);
   Hex::Vertex* noeud   = cell->getVertex (0);

   Hex::Shape* shape1 = new Hex::Shape("riri");
   Hex::Shape* shape2 = new Hex::Shape("fifi");
   Hex::Shape* shape3 = new Hex::Shape("loulou");

   noeud->setAssociation (shape1);
   arete->setAssociation (shape2);
   face ->setAssociation (shape3);

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
   doc ->setFile ("Essai");
   doc ->saveFile ();

   return HOK;
}
// ======================================================== test_string_xml
int test_string_xml (int nbargs, cpchar tabargs[])
{
   const int size_x = 2;
   const int size_y = 2;
   const int size_z = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex* orig = doc->addVertex (0,0,0);
   Hex::Vector* dir  = doc->addVector (1,1,1);
   Hex::Elements*  grid = doc->makeCartesian (orig, dir, size_x,size_y,size_z);

   Hex::Hexa*   cell    = grid->getHexa (0);
   Hex::Quad*   face    = cell->getQuad (0);
   Hex::Edge*   arete   = cell->getEdge (0);
   Hex::Vertex* noeud   = cell->getVertex (0);

   Hex::Shape* shape1 = new Hex::Shape("riri");
   Hex::Shape* shape2 = new Hex::Shape("fifi");
   Hex::Shape* shape3 = new Hex::Shape("loulou");

   noeud->setAssociation (shape1);
   arete->setAssociation (shape2);
   face ->setAssociation (shape3);

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
   doc ->setFile ("Essai");

   cpchar flux = doc ->getXml ();
   Hex::Document* docbis = mon_ex.addDocument ();
   docbis->setXml (flux);
   docbis ->saveVtk ("clone.vtk");

   return HOK;
}
// ======================================================== test_relecture
int test_relecture (int nbargs, cpchar tabargs[])
{
   Hex::Hex mon_ex;
   cpchar nomdoc = "Essai";
   if (nbargs>1) 
      nomdoc = tabargs[1];
   Hex::Document* doc = mon_ex.loadDocument (nomdoc);

/*********************
   Hex::Vertex* v4 = doc->findVertex (80.0, 0.0,  0.0);
   Hex::Vertex* v5 = doc->findVertex (80.0, 0.0, 40.0);
   Hex::Edge*   e4 = doc->findEdge   (v4, v5);

   HexDump (v4);
   HexDump (v5);
   HexDump (e4);

   e4->setScalar (5);
***********************/

   doc ->dump ();
   doc ->saveVtk ("restore.vtk");
   doc ->setFile ("restore");
   doc ->saveFile ();

   // doc ->reorderFaces ();
   // doc ->dump ();

   // Hex::Elements* grid2 = doc->cut (e4, 2);
   return HOK;
}
// ======================================================== test_clone
int test_clone ()
{
   const int size_x = 2;
   const int size_y = 2;
   const int size_z = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

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

   return HOK;
}
// ======================================================== test_separ
int test_separ ()
{
   const int size_x = 2;
   const int size_y = 2;
   const int size_z = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex* orig = doc->addVertex (0,0,0);
   Hex::Vector* dir  = doc->addVector (1,1,1);
   //  Hex::Elements*  grid = 
   doc->makeCartesian (orig, dir, size_x,size_y,size_z);

   doc ->saveVtk ("separ.vtk");
   doc ->dump();

   return HOK;
}
// ======================================================== test_shperical
int test_spherical (int nbargs, const char* tabargs[])
{
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex* orig = doc->addVertex (0,0,0);
   Hex::Vector* dir  = doc->addVector (1,1,1);
   int          nbr    = 3;

   Hex::Elements* grid = doc->makeSpherical (orig, dir, nbr);

   int nbhexas = grid->countHexa ();
   HexDisplay (nbhexas);
   for (int nro=0 ; nro<nbhexas ; nro +=3)
       grid->getHexa(nro)->remove();
   HexDisplay (doc->countHexa ());
   doc->saveVtk ("shperical.vtk");
   // doc->dump ();

   return HOK;
}
// ================================================== test_grille_cyl
int test_grille_cyl ()
{
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

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
   int    nl = 3;

   Hex::Elements *c1, *c2, *c3, *c4, *c5, *c6;

   c1 = doc->makeCylindrical (orig1, vx,vz,dr, 360, dl,nr, 4, nl, true);
   c2 = doc->makeCylindrical (orig2, vx,vz,dr, 360, dl,nr, 8, nl, true);
   c3 = doc->makeCylindrical (orig3, vx,vz,dr, 270, dl,nr, 8, nl, true);
   c4 = doc->makeCylindrical (orig4, vx,vz,dr, 270, dl,nr, 7, nl, true);
   c5 = doc->makeCylindrical (orig5, vx,vz,dr, 360, dl,nr, 5, nl, true);
   c6 = doc->makeCylindrical (orig6, vx,vz,dr, 360, dl,nr, 6, nl, true);

   int base2 = nr*nl*8;
   c2->getHexa(base2 + 0)->setScalar (5);
   c2->getHexa(base2 + 1)->setScalar (5);
   c2->getHexa(base2 + 2)->setScalar (5);

   doc->saveVtk ("cylindres.vtk");
   // doc->dump ();

   return HOK;
}
// ================================================== test_asso_line
int test_asso_line (int nbargs, cpchar tabargs[])
{
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex* orig1 = doc->addVertex ( 0, 0,0);

   Hex::Vector* vz = doc->addVector (0,0,1);
   Hex::Vector* vx = doc->addVector (1,0,0);

   double dr = 1;
   double dl = 1;
   int    nr = 2;
   int    nl = 3;
   int    ntheta = 8;

   Hex::Elements *c1 = NULL;

   c1 = doc->makeCylindrical (orig1, vx,vz,dr, 360, dl,nr, ntheta, nl, true);

   Hex::Edges m_line;
   Hex::Edge* m_start = c1->getEdgeJ (nr, 1, 0);

   for (int na=2 ; na<ntheta ; na++)
       {
       Hex::Edge*  arete = c1->getEdgeJ (nr, na, 0);
       arete->setScalar (5);
       m_line.push_back (arete);
       }

   // m_line.push_back (c1->getEdgeJ (nr, 0, 2));
   // m_line.push_back (NULL);
   Hex::Shape* gstart = NULL;
   Hex::Shapes gline;
   double pstart = 0 , pend = 0;

   int ier = doc-> associateOpenedLine (m_start, m_line, 
                                        gstart,  pstart, gline, pend);
   HexDisplay (ier);
   doc->saveVtk ("asso_line.vtk");

  m_line.push_back (c1->getEdgeJ (nr, 0, 0));
   Hex::Vertex* m_first = m_start->getVertex (Hex::V_AMONT);
   ier = doc-> associateClosedLine (m_first, m_start, m_line, 
                                        gstart,  pstart, gline);
   HexDisplay (ier);
   // doc->dump ();

   return HOK;
}
// ===================================================== test_cylinder
int test_cylinder (int nbargs, cpchar tabargs[])
{
   int    nvtk    = 0;
   cpchar fic_vtk = "cylindre";

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

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

   Hex::Elements* grid = doc->makeCylinder (cyl,  vx, nr, na, nl);
   doc ->saveVtk (fic_vtk, nvtk);

   Hex::Group* groupe = doc->addGroup ("GroupeAMA", Hex::HexaCell);
   groupe->addElement (grid->getHexaIJK (0,0,0));
   groupe->addElement (grid->getHexaIJK (1,0,0));
   groupe->addElement (grid->getHexaIJK (0,1,0));
   groupe->addElement (grid->getHexaIJK (1,1,0));
   groupe->addElement (grid->getHexaIJK (2,1,0));

   doc->makePipe     (pipe, vx, nr, na, nl);
   doc ->saveVtk (fic_vtk, nvtk);
   doc->setFile ("cylindre");
   doc->saveFile ();

   return HOK;
}
// ======================================================== test_croix
int test_croix ()
{
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

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

   grid->getVertexIJK (Hex::Cyl1, 2, Hex::S_E, 0)->setScalar (5);
   grid->getVertexIJK (Hex::Cyl1, 2, Hex::S_E, 1)->setScalar (5);

   grid->getHexaIJK (Hex::Cyl2, 1, Hex::S_E,  0)->setScalar (5);
   grid->getHexaIJK (Hex::Cyl2, 1, Hex::S_SW, 0)->setScalar (5);

   doc->saveVtk ("croix.vtk");
   //  doc->dump ();

   return HOK;
}
// ======================================================== test_pipes
int test_pipes ()
{
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

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

   grid->getHexaIJK (0, 1,1,1)->setScalar (3);
   grid->getHexaIJK (1, 1,1,1)->setScalar (6);
   grid->dump();
   grid->dumpHexas();

   doc->saveVtk ("pipes.vtk");
   /// doc->dump ();
   return HOK;
}
// ======================================================== test_lorraine
int test_lorraine()
{
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

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

   int hauteur = 3;
   doc->joinQuads  (hliste, qb, vh0, vb0, vh1, vb1, hauteur);
   doc->saveVtk ("lorraine.vtk");


   // doc->dump ();
   return HOK;
}
// ======================================================== test_disconnect2
// === Disconnect Edge seul
int test_disconnect2 (int nbargs, cpchar tabargs[])
{
   const int size_x = 2;
   const int size_y = 2;
   const int size_z = 1;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex*   orig2 = doc->addVertex (0,0,0);
   Hex::Vector*   dir   = doc->addVector (1,1,1);
   Hex::Elements* grid2 = doc->makeCartesian (orig2, dir, size_x,size_y,size_z);

   doc->dump ();

   int nvtk = 0;
   doc->setLevel (1);
   Hex::Matrix  matrice;
   Hex::Vector* ecart  = doc->addVector (0.5,0.5,0);
   matrice.defTranslation (ecart);

   Hex::Hexa* hexa2 = grid2->getHexaIJK (1,1,0);
   Hex::Edge* edge  = grid2->getEdgeK   (1,2,0);

   edge->setScalar   (5);

   doc->saveVtk ("test_disco", nvtk);

   doc->setLevel (4);

   Hex::Elements* disco_edges =  doc->disconnectEdge (hexa2, edge);
   HexDisplay (disco_edges->countVertex());
   HexDisplay (disco_edges->countEdge());
   HexDisplay (disco_edges->countQuad());
   HexDisplay (disco_edges->countHexa());

   // hexa2->transform (&matrice);
   for (int ns=0; ns<disco_edges->countVertex(); ns++)
       {
       Hex::Vertex* sommet = disco_edges->getVertex(ns);
       sommet->setX (sommet->getX()+0.5);
       sommet->setY (sommet->getY()+0.5);
       }

   doc->saveVtk ("test_disco", nvtk);
   doc->setFile ("test_disco");
   doc->dump ();
   hexa2->dumpFull ();

   doc->setLevel (4);
   doc->saveFile ();
   return HOK;
}
// ======================================================== test_disconnect
// ==== Disconnect Quad
int test_disconnect1 (int nbargs, cpchar tabargs[])
{
   const int size_x = 2;
   const int size_y = 2;
   const int size_z = 1;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex*   orig1 = doc->addVertex (0,0,0);
   Hex::Vector*   dir   = doc->addVector (1,1,1);
   Hex::Elements* grid1 = doc->makeCartesian (orig1, dir, size_x,size_y,size_z);

   int nvtk = 0;
   doc->setLevel (1);
   Hex::Matrix  matrice;
   Hex::Vector* ecart  = doc->addVector (0.5,0.5,0);
   matrice.defTranslation (ecart);

   Hex::Hexa* hexa1 = grid1->getHexaIJK (1,1,0);
   Hex::Quad* quad  = grid1->getQuadJK  (1,1,0);

   quad->setScalar   (5);

   doc->saveVtk ("test_disco", nvtk);
   doc->disconnectQuad (hexa1, quad);
   hexa1 ->transform (&matrice);
   doc->saveVtk ("test_disco", nvtk);

   // doc->dumpPropagation ();
   // doc->dump  ();

   doc->setFile  ("disco_all");
   doc->saveFile ();
   return HOK;
}
// ======================================================== test_disconnect
// ==== disconnectVertex
int test_disconnect (int nbargs, cpchar tabargs[])
{
   const int size_x = 2;
   const int size_y = 2;
   const int size_z = 1;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex*   orig1 = doc->addVertex (0,0,0);

   Hex::Vector*   dir   = doc->addVector (1,1,1);
   Hex::Elements* grid1 = doc->makeCartesian (orig1, dir, size_x,size_y,size_z);

   int nvtk = 0;
   doc->setLevel (1);
   Hex::Matrix  matrice;
   Hex::Vector* ecart  = doc->addVector (0.5,0.5,0);
   matrice.defTranslation (ecart);

   Hex::Hexa* hexa1 = grid1->getHexaIJK (1,1,0);
   Hex::Vertex* vertex = grid1->getVertexIJK (1,1,1);

   vertex->setScalar (5);

   doc->saveVtk ("test_disco", nvtk);

   doc->disconnectVertex (hexa1, vertex);
   hexa1->transform (&matrice);
   doc->saveVtk ("test_disco", nvtk);

   // doc->dumpPropagation ();
   // doc->dump  ();

   doc->setFile  ("disco_all");
   doc->saveFile ();
   return HOK;
}
// ======================================================== test_disconnect
// ==== Les 3 disconnect
int test_disconnect_all (int nbargs, cpchar tabargs[])
{
   const int size_x = 2;
   const int size_y = 2;
   const int size_z = 1;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex*   orig1 = doc->addVertex (0,0,0);
   Hex::Vertex*   orig2 = doc->addVertex (4,0,0);
   Hex::Vertex*   orig3 = doc->addVertex (8,0,0);

   Hex::Vector*   dir   = doc->addVector (1,1,1);
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
   Hex::Hexa* hexa3 = grid3->getHexaIJK (1,1,0);

   Hex::Quad* quad  = grid1->getQuadJK  (1,1,0);
   Hex::Edge* edge  = grid2->getEdgeK   (1,2,0);
   Hex::Vertex* vertex = grid3->getVertexIJK (1,1,1);

   quad->setScalar   (5);
   edge->setScalar   (5);
   vertex->setScalar (5);

   doc->saveVtk ("test_disco", nvtk);
   doc->disconnectQuad (hexa1, quad);
   hexa1 ->transform (&matrice);
   doc->saveVtk ("test_disco", nvtk);

   doc->disconnectEdge (hexa2, edge);
   hexa2->transform (&matrice);
   doc->saveVtk ("test_disco", nvtk);

   doc->disconnectVertex (hexa3, vertex);
   hexa3->transform (&matrice);
   doc->saveVtk ("test_disco", nvtk);

   // doc->dumpPropagation ();
   // doc->dump  ();

   doc->setFile  ("disco_all");
   doc->saveFile ();
   return HOK;
}
// ======================================================== test_propagation
int test_propagation ()
{
   const int size_x = 2;
   const int size_y = 1;
   const int size_z = 1;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

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
   doc->saveVtk ("test_propagation.vtk");
   doc->setFile ("test_propagation");
   doc->saveFile ();

   return HOK;
}
// ======================================================== test_move
int test_move ()
{
   const int size_x = 1;
   const int size_y = 1;
   const int size_z = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

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

   return HOK;
}
// ======================================================== test_move2
int test_transfo (int nbargs, cpchar tabargs[])
{
   const int size_x = 1;
   const int size_y = 1;
   const int size_z = 2;

   int    nvtk    = 0;
   cpchar fic_vtk = "transfo";

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();
   doc ->setLevel (1);

   Hex::Vertex* orig = doc->addVertex (0,0,0);
   Hex::Vector* dir  = doc->addVector (1,1,1);
   Hex::Elements* grid = doc->makeCartesian (orig, dir, size_x, size_y,
                                                        size_z);
   if (grid==NULL)
      return HERR;

   orig->setScalar(2);

   doc ->saveVtk (fic_vtk, nvtk);

   Hex::Vector*   devant  = doc->addVector (5, 0, 0);

   Hex::Elements* grid2 = doc->makeTranslation (grid, devant);
   if (grid2==NULL)
      return HERR;
   doc ->saveVtk (fic_vtk, nvtk);

   Hex::Elements* grid3  = doc->makeScale (grid2, orig, 2);
   if (grid3==NULL)
      return HERR;
   doc ->saveVtk (fic_vtk, nvtk);

   Hex::Elements* grid4 = doc->makeRotation (grid2, orig, dir, 45);
   if (grid4==NULL)
      return HERR;
   doc ->saveVtk (fic_vtk, nvtk);

   Hex::Elements* grid5 = doc->makeSymmetryPoint (grid4, orig);
   if (grid5==NULL)
      return HERR;

   doc ->saveVtk (fic_vtk, nvtk);

   Hex::Vector* dir1  = doc->addVector (1,0,0);
   Hex::Elements* grid6 = doc->makeSymmetryLine (grid4, orig, dir1);
   if (grid6==NULL)
      return HERR;

   grid4->getHexa(0)->getVertex(0)->setScalar(3);
   grid6->getHexa(0)->getVertex(0)->setScalar(3);
   doc ->saveVtk (fic_vtk, nvtk);

   grid4->getHexa(0)->getVertex(0)->setScalar(0);
   grid6->getHexa(0)->getVertex(0)->setScalar(0);

   Hex::Elements* grid7 = doc->makeSymmetryLine (grid2, orig, dir1);
   if (grid7==NULL)
      return HERR;

   grid2->getHexa(0)->getVertex(0)->setScalar(4);
   grid7->getHexa(0)->getVertex(0)->setScalar(4);
   doc ->saveVtk (fic_vtk, nvtk);

   grid2->getHexa(0)->getVertex(0)->setScalar(0);
   grid7->getHexa(0)->getVertex(0)->setScalar(0);

   Hex::Elements* grid8 = doc->makeSymmetryPlane (grid2, orig, dir1);
   if (grid8==NULL)
      return HERR;

   grid2->getHexa(0)->getVertex(0)->setScalar(4);
   grid8->getHexa(0)->getVertex(0)->setScalar(4);
   doc ->saveVtk (fic_vtk, nvtk);
   grid2->getHexa(0)->getVertex(0)->setScalar(0);
   grid8->getHexa(0)->getVertex(0)->setScalar(0);

   Hex::Elements* grid9 = doc->makeSymmetryPlane (grid3, orig, dir);
   if (grid9==NULL)
      return HERR;

   grid3->getHexa(0)->getVertex(0)->setScalar(4);
   grid9->getHexa(0)->getVertex(0)->setScalar(4);
   doc ->saveVtk (fic_vtk, nvtk);

   grid9->getHexa(0)->removeConnected ();
   doc ->saveVtk (fic_vtk, nvtk);

   return HOK;
}
// ======================================================== test_copy_document
int test_copy_document (int nbargs, cpchar tabargs[])
{
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.loadDocument ("Essai");
   doc ->saveVtk ("restore1.vtk");

   Hex::Document* clone = doc->copyDocument();
   clone->saveVtk ("restore2.vtk");

   return HOK;
}
// ======================================================== test_remove
int test_remove ()
{
   const int size_x = 2;
   const int size_y = 2;
   const int size_z = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex* orig  = doc->addVertex (0,0,0);
   Hex::Vertex* orig1 = doc->addVertex (6,0,0);
   Hex::Vector* dir   = doc->addVector (1,1,1);
   Hex::Elements* grid  = doc->makeCartesian (orig, dir,  size_x,size_y,size_z);
   doc->makeCartesian (orig1, dir, 1,1,1);
   doc->saveVtk ("removeConn1.vtk");

   Echo ("--------- Avant destruction");
   HexDisplay (doc->countVertex ());
   HexDisplay (doc->countEdge ());
   HexDisplay (doc->countQuad ());
   HexDisplay (doc->countHexa ());
   HexDisplay (doc->countUsedVertex ());
   HexDisplay (doc->countUsedEdge ());
   HexDisplay (doc->countUsedQuad ());
   HexDisplay (doc->countUsedHexa ());


   doc->removeConnectedHexa (grid->getHexaIJK (0,0,0));

   Echo ("--------- Apres destruction");
   HexDisplay (doc->countVertex ());
   HexDisplay (doc->countEdge ());
   HexDisplay (doc->countQuad ());
   HexDisplay (doc->countHexa ());

   HexDisplay (doc->countUsedVertex ());
   HexDisplay (doc->countUsedEdge ());
   HexDisplay (doc->countUsedQuad ());
   HexDisplay (doc->countUsedHexa ());
   doc->saveVtk ("removeConn2.vtk");

   return HOK;
}

int test_quads (int nbargs, cpchar tabargs[]);
int test_hemispheres (int nbargs, cpchar tabargs[]);
// ======================================================== test_hexa1
int test_hexa1 (int nbargs, cpchar tabargs[])
{
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex* orig  = doc->addVertex (0,0,0);
   Hex::Vector* dir   = doc->addVector (1,1,1);
   Hex::Elements* grid  = doc->makeCartesian (orig, dir,  2, 2, 2);

   doc->saveVtk ("cartes.vtk");

   
}
