
// C++ : Tests unitaires

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

#include "test_unit.hxx"

#include "Hex.hxx"
#include "HexDocument.hxx"
#include "HexElements.hxx"

#include "HexHexa.hxx"
#include "HexQuad.hxx"
#include "HexEdge.hxx"
#include "HexVertex.hxx"

#include "HexPropagation.hxx"
#include "HexLaw.hxx"
#include "HexMatrix.hxx"
#include "HexCramer.hxx"
#include "HexGroup.hxx"


#include <cstdlib>

static int nbr_vtk = 0;
static cpchar case_name = "hexa";
static Hex::Document*   docu = NULL;

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
// ======================================================== save_vtk
void save_vtk ()
{
   if (docu==NULL)
      return;

   docu->saveVtk (case_name, nbr_vtk);
}
// ======================================================== remove_hexa
void remove_hexa (Hex::Hexa* hexa)
{
   if (hexa==NULL)
      return;

   hexa->remove();

   if (docu==NULL)
      return;

   docu->saveVtk (case_name, nbr_vtk);
}
// ======================================================== test_sphere
int test_sphere (int nbargs, cpchar tabargs[])
{
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   int    ncouches = 2;
   double k      = 1;
   Hex::Elements* sphere = doc->makeSphericalTop (ncouches, k);

   if (ncouches>0)
      {
   for (int nc=0 ; nc <= ncouches ; nc++)
       {
       Hex::Hexa* cell = sphere->getStrate (nc, Hex::Q_A);
       if (nc==0)
          cell->dumpFull();
       cell->remove ();
       // sphere->getStrate (nc, Hex::Q_A)->remove ();
       // sphere->getStrate (nc, Hex::Q_B)->remove ();
       }
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

   Hex::Elements* grid = doc->makeCartesianTop (size_x,size_y,size_z);

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

   Hex::Elements*  grid = doc->makeCartesianTop (size_x,size_y,size_z);

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

   Hex::Elements* grid1 = doc->makeCartesianTop   (dimx,dimy,dimz);

   Hex::Vertex*   orig2 = doc->addVertex  (dimx/2.0,0,8);
   Hex::Vector*   vectj = doc->addVector (0,1,0);
   Hex::Vector*   vecti = doc->addVector (1,0,0);
   Hex::Elements* grid2 = doc->makeCylinderUni (orig2, vecti, vectj,
                   1, 180, 1,        dimz,dimy,dimx, true);

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
   Hex::Elements* joint = doc->joinQuadsUni(liste,cible, v1,v2,v3,v4, hauteur);
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
// ======================================================== test_count
int test_count (int nbargs, cpchar tabargs[])
{
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   int    nr = 2;
   int    nl = 3;

// Hex::Elements* c1 =
   doc->makeCylinderTop (nr, 10, nl);

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

   Hex::Elements* grid  = doc->makeCartesianTop (size_x,size_y,size_z);
   Hex::Edge*     arete = grid->getEdgeK (0, 0, 0);

   //  doc ->dump ();
   int nvtk=0;
   doc ->saveVtk ("decoupe", nvtk);
/* Hex::Elements* grid2 = */  doc->cutUni (arete, 1);

   ///  doc ->dump ();
   doc ->saveVtk ("decoupe", nvtk);

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

   // Hex::Elements*  grid =
   doc->makeCartesianTop (size_x,size_y,size_z);

   // Hex::Hexa*   cell    = grid->getHexa (0);
   // Hex::Quad*   face    = cell->getQuad (0);
   // Hex::Edge*   arete   = cell->getEdge (0);
   // Hex::Vertex* noeud   = cell->getVertex (0);

   // Hex::Shape* shape1 = new Hex::Shape("riri");
   // Hex::Shape* shape2 = new Hex::Shape("fifi");
   // Hex::Shape* shape3 = new Hex::Shape("loulou");

   // noeud->setAssociation (shape1);
   // arete->setAssociation (shape2);
   // face ->setAssociation (shape3);

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

   // Hex::Elements*  grid =
   doc->makeCartesianTop (size_x,size_y,size_z);

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

   cpchar flux = doc ->getXml ();
   Hex::Document* docbis = mon_ex.addDocument ();
   docbis->setXml  (flux);
   docbis->saveVtk ("clone.vtk");

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
   doc ->save ("restore");

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

   Hex::Elements* grid  = doc->makeCartesianTop (size_x,size_y,size_z);
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

   doc->makeCartesianTop (size_x,size_y,size_z);

   doc ->saveVtk ("separ.vtk");
   doc ->dump();

   return HOK;
}
// ======================================================== test_shperical
int test_spherical (int nbargs, const char* tabargs[])
{
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   int          nbr    = 3;

   Hex::Elements* grid = doc->makeSphericalTop (nbr, 1);

   int nbhexas = grid->countHexa ();
   HexDisplay (nbhexas);
   for (int nro=3 ; nro<nbhexas ; nro +=3)
       grid->getHexa(nro)->remove();
   HexDisplay (doc->countHexa ());
   doc->saveVtk ("shperical.vtk");
   // doc->dump ();

   return HOK;
}
// ================================================== test_grille_cyl
int test_grille_cyl (int nbargs, cpchar tabargs[])
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

   double rint = 5;
   double rext = 1;
   double dl = 1;
   int    nr = 2;
   int    nl = 3;

   doc->makeCylinderUni (orig1, vx,vz, rint,rext, 360,dl,nr, 4, nl);
   Hex::Elements* c2 =
   doc->makeCylinderUni (orig2, vx,vz, rint,rext, 360,dl,nr, 8, nl);
   doc->makeCylinderUni (orig3, vx,vz, rint,rext, 270,dl,nr, 8, nl);
   doc->makeCylinderUni (orig4, vx,vz, rint,rext, 270,dl,nr, 7, nl);
   doc->makeCylinderUni (orig5, vx,vz, rint,rext, 360,dl,nr, 5, nl);
   doc->makeCylinderUni (orig6, vx,vz, rint,rext, 360,dl,nr, 6, nl);

   int base2 = nr*nl*8;
   c2->getHexa(base2 + 0)->setScalar (5);
   c2->getHexa(base2 + 1)->setScalar (5);
   c2->getHexa(base2 + 2)->setScalar (5);

   doc->saveVtk ("cylindres.vtk");
   // doc->dump ();

   return HOK;
}
// ===================================================== test_cylindrical
int test_cylindrical (int nbargs, cpchar tabargs[])
{
   cpchar fic_vtk = "cylindre";

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex* orig = doc->addVertex (0, 0, 0);
   Hex::Vector* vx   = doc->addVector (1, 0, 0);
   Hex::Vector* vz   = doc->addVector (0, 0, 1);

   double da = 360;
   double dl = 1;

   int nr = 1;
   int na = 8;
   int nl = 2;

   if (nbargs>1)
      {
      na = atoi (tabargs[1]);
      HexDisplay (na);
      if (na <= 2)
          na = 8;
      }

   if (nbargs>2)
      {
      da = atof (tabargs[2]);
      HexDisplay (da);
      }

   doc->makeCylinderUni (orig,vx, vz, 5.0,1.0,da,dl, nr,na, nl);
   doc->saveVtk (fic_vtk, na);
   return HOK;
}
// ======================================================== test_joint2
int test_joint2 (int nbargs, cpchar tabargs[])
{
   Hex::Hex mon_ex;
   docu = mon_ex.addDocument ();
   case_name = "pb_joint";

   Hex::Vector* vx    = docu->addVector (1, 0, 0);
   Hex::Vector* vz    = docu->addVector (0, 0, 1);
   Hex::Vertex* hori  = docu->addVertex (0, 0, 0);

   double da = 360;
   double dl = 1;
   int    nr = 1;
   int    na = 8;
   int    nl = 1;

   Hex::Elements *bgrid=NULL, *hgrid=NULL;

   hgrid = docu->makeCylinderUni (hori, vx,vz, 5.0,1.0,da,dl, nr,na,nl);
   docu->dump ();
   save_vtk ();

   Hex::Vertex* bori  = docu->addVertex (0, 0, -5);
   bgrid = docu->makeCylinderUni (bori, vx,vz, 5.0,1.0,da,dl, nr,na,nl);
   save_vtk ();

   Hex::Quads qsource, qdest;
   printf (" Source = ");
   for (int ny=0 ; ny< na ; ny++)
       {
       Hex::Quad* quad = hgrid->getQuadIJ (0, ny, 0);
       PrintName (quad);
       qsource.push_back (quad);
       }
   printf ("\n          ");
   for (int ny=0 ; ny<4 ; ny++)
       {
       Hex::Quad* quad = hgrid->getKerHQuad (ny);
       PrintName (quad);
       qsource.push_back (quad);
       }
   printf ("\n");


   printf (" Cible  = ");
   for (int ny=0 ; ny< na ; ny++)
       {
       Hex::Quad* quad = bgrid->getQuadIJ (0, ny, 1);
       PrintName (quad);
       qdest.push_back (quad);
       }
   printf ("\n          ");
   for (int ny=0 ; ny<4 ; ny++)
       {
       Hex::Quad* quad = bgrid->getKerHQuad (ny+4);
       PrintName (quad);
       qdest.push_back (quad);
       }
   printf ("\n");
   docu ->setLevel (1);
   Hex::Quad*   cible = bgrid->getQuadIJ    (0, 1, 1);
   Hex::Vertex* vc1   = bgrid->getVertexIJK (0, 1, 1);
   Hex::Vertex* vc2   = bgrid->getVertexIJK (1, 1, 1);

   Hex::Vertex* vs1 = hgrid->getVertexIJK (0, 0, 0);
   Hex::Vertex* vs2 = hgrid->getVertexIJK (1, 0, 0);

   docu->joinQuadsUni (qsource, cible, vs1, vc1, vs2, vc2, 1);
   save_vtk ();

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
   Hex::Document* doc   = mon_ex.addDocument ();
   Hex::Elements* grid2 = doc->makeCartesianTop (size_x,size_y,size_z);

   int nvtk = 0;
   doc->setLevel (1);

   Hex::Hexa* hexa2 = grid2->getHexaIJK (1,1,0);
   Hex::Edge* edge  = grid2->getEdgeK   (1,2,0);

   hexa2->setColor  (2);
   edge->setColor   (5);

   doc->saveVtk ("test_disco", nvtk);

   doc->setLevel (4);

   Hex::Elements* disco_edges =  doc->disconnectEdge (hexa2, edge);
   HexDisplay (disco_edges->countVertex());
   HexDisplay (disco_edges->countEdge());
   HexDisplay (disco_edges->countQuad());
   HexDisplay (disco_edges->countHexa());

   doc->saveVtk ("test_disco", nvtk);
   doc->save ("test_disco");
   // doc->dump ();
   // hexa2->dumpFull ();

   // doc->setLevel (4);
   return HOK;
}
// ======================================================== test_disconnect4
// === Disconnect Edges
int test_disconnect4 (int nbargs, cpchar tabargs[])
{
   const int size_x = 1;
   const int size_y = 2;
   const int size_z = 2;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Elements* grid2 = doc->makeCartesianTop  (size_x,size_y,size_z);

   int nvtk = 0;
   doc->setLevel (1);

   Hex::Hexas t_hexas;
   Hex::Edges t_edges;
   for (int nk=0 ; nk< size_z; nk++)
       {
       Hex::Hexa* hexa2 = grid2->getHexaIJK (0,0,nk);
       Hex::Edge* edge  = grid2->getEdgeK   (0,1,nk);

       hexa2->setScalar  (2);
       edge->setScalar   (5);
       t_hexas.push_back (hexa2);
       t_edges.push_back (edge);

       doc->setLevel (4);
       }

   doc->dump ();
   doc->saveVtk ("test_disco", nvtk);
   /* Hex::Elements* disco_edges = */  doc->disconnectEdges (t_hexas, t_edges);
   doc->saveVtk ("test_disco", nvtk);
   doc->dump ();
   // hexa2->dumpFull ();

   doc->setLevel (4);
   return HOK;
}
// ======================================================== test_disconnect1
// ==== Disconnect Quad
int test_disconnect1 (int nbargs, cpchar tabargs[])
{
   const int size_x = 2;
   const int size_y = 2;
   const int size_z = 1;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Elements* grid1 = doc->makeCartesianTop (size_x,size_y,size_z);

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
   // hexa1 ->transform (&matrice);
   doc->saveVtk ("test_disco", nvtk);

   // doc->dumpPropagation ();
   // doc->dump  ();

   doc->save  ("disco_all");
   return HOK;
}
// ======================================================== test_disconnect3
// ==== disconnectVertex
int test_disconnect3 (int nbargs, cpchar tabargs[])
{
   const int size_x = 2;
   const int size_y = 2;
   const int size_z = 1;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Elements* grid1 = doc->makeCartesianTop (size_x,size_y,size_z);

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
   // hexa1->transform (&matrice);
   doc->saveVtk ("test_disco", nvtk);

   // doc->dumpPropagation ();
   // doc->dump  ();

   doc->save  ("disco_all");
   return HOK;
}
// ======================================================== contraction
void contraction (Hex::Hexa* hexa, Hex::Elements* grid)
{
   return;
   Hex::Real3 cg = { 0, 0, 0 };

   for (int nro=0; nro<Hex::HV_MAXI ; nro++)
       {
       cg [0] += hexa->getVertex(nro)->getX()/Hex::HV_MAXI;
       cg [1] += hexa->getVertex(nro)->getY()/Hex::HV_MAXI;
       cg [2] += hexa->getVertex(nro)->getZ()/Hex::HV_MAXI;
       }

   int nbvertex = grid->countVertex();
   const double coeff = 0.5;
   for (int nro=0; nro<nbvertex ; nro++)
       {
       Hex::Vertex* pv = grid->getVertex(nro);
       Hex::Real3 pold = { pv->getX(), pv->getY(), pv->getZ() };
       Hex::Real3 pnew;
       for (int dd=0; dd<3 ; dd++)
           pnew [dd] = cg[dd] + coeff * (pold[dd]-cg[dd]);

       pv->setX (pnew[0]);
       pv->setY (pnew[1]);
       pv->setZ (pnew[2]);
       }
}
// ======================================================== test_disconnect
// ==== Les 3 disconnect
int test_disconnect (int nbargs, cpchar tabargs[])
{
   const int size_x = 2;
   const int size_y = 2;
   const int size_z = 1;

   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex*   orig1 = doc->addVertex (0,0,0);
   Hex::Vertex*   orig2 = doc->addVertex (4,0,0);
   Hex::Vertex*   orig3 = doc->addVertex (8,0,0);

   Hex::Vector*   vx   = doc->addVector (1,0,0);
   Hex::Vector*   vy   = doc->addVector (0,1,0);
   Hex::Vector*   vz   = doc->addVector (0,0,1);
   double dx=1, dy=1, dz=1;

   Hex::Elements* grid1 = doc->makeCartesianUni (orig1, vx,vy,vz, dx,dy,dz, 
                                                 size_x,size_y,size_z);
   Hex::Elements* grid2 = doc->makeCartesianUni (orig2, vx,vy,vz, dx,dy,dz, 
                                                 size_x,size_y,size_z);
   Hex::Elements* grid3 = doc->makeCartesianUni (orig3, vx,vy,vz, dx,dy,dz, 
                                                 size_x,size_y,size_z);

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
   doc->saveVtk ("test_disco", nvtk);

   doc->disconnectEdge (hexa2, edge);
   doc->saveVtk ("test_disco", nvtk);

   doc->disconnectVertex (hexa3, vertex);
   doc->saveVtk ("test_disco", nvtk);

   // doc->dumpPropagation ();
   // doc->dump  ();

   doc->save  ("disco_all");
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

   doc->makeCartesianTop (size_x,size_y,size_z);

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
   doc->save ("test_propagation");

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

   Hex::Elements*  grid = doc->makeCartesianTop (size_x,size_y,size_z);

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
// ======================================================== test_transfo2
int test_transfo2 (int nbargs, cpchar tabargs[])
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
   Hex::Elements* grid = doc->makeCartesianTop (size_x, size_y, size_z);
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

   Hex::Elements* grid  = doc->makeCartesianTop (size_x,size_y,size_z);
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
// ================================================== init_vec
void init_vec (Hex::RealVector& tab, double n0=0, double n1=0, double n2=0,
               double n3=0, double n4=0, double n5=0, double n6=0,
               double n7=0, double n8=0, double n9=0, double n10=0,
               double n11=0, double n12=0, double n13=0, double n14=0,
               double n15=0, double n16=0)
{
   if (n0>0.0) tab.push_back (n0);
   if (n1>0.0) tab.push_back (n1);
   if (n2>0.0) tab.push_back (n2);
   if (n3>0.0) tab.push_back (n3);
   if (n4>0.0) tab.push_back (n4);
   if (n5>0.0) tab.push_back (n5);
   if (n6>0.0) tab.push_back (n6);
   if (n7>0.0) tab.push_back (n7);
   if (n8>0.0) tab.push_back (n8);
   if (n9>0.0) tab.push_back (n9);

   if (n10>0.0) tab.push_back (n10);
   if (n11>0.0) tab.push_back (n11);
   if (n12>0.0) tab.push_back (n12);
   if (n13>0.0) tab.push_back (n13);
   if (n14>0.0) tab.push_back (n14);
   if (n15>0.0) tab.push_back (n15);
   if (n16>0.0) tab.push_back (n16);
}
// ======================================================== test_edge
int test_edge (int nbargs, cpchar tabargs[])
{
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex* orig = doc->addVertex (0, 0, 0);
   Hex::Vector* vx   = doc->addVector (1 ,0, 0);
   doc->addEdgeVector   (orig, vx);

   HexDisplay (doc->countVertex());
   HexDisplay (doc->countEdge());
   doc->dump ();
   return HOK;
}
// ======================================================== test_hexa
int test_hexa (int nbargs, cpchar tabargs[])
{
   int ier = 0;
   Hex::Hex mon_ex;
   Hex::Document* doc1 = mon_ex.loadDocument ("bielle_triang");
   Hex::Document* doc2 = mon_ex.loadDocument ("bielle_triang");
   PutData (doc1->getName ());
   PutData (doc2->getName ());
   return ier;


   goto_workspace ();
   free_workspace ();

   return ier;
}
