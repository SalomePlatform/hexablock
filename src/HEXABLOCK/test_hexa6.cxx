
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
#include "HexBiCylinder.hxx"

#include "HexHexa.hxx"
// #include "HexQuad.hxx"
// #include "HexEdge.hxx"
// #include "HexVertex.hxx"

static Hex::Hex         mon_ex;
static Hex::Document*   docu = NULL;
static cpchar           case_name = "hexa";

// ======================================================== get_document
Hex::Document* get_document ()
{
   if (docu==NULL)
      docu = mon_ex.addDocument ();

   return docu;
}

// ======================================================== save_file
void save_file ()
{
   static int nbr_files = 0;
   char filename[20];

   nbr_files ++;
   sprintf (filename, "%s%02d.vtk", case_name, nbr_files);
   docu->saveVtk (filename);
}

// ======================================================== nearest
Hex::Vertex* nearest (Hex::Elements* grid, Hex::Vertex* vertex)
{
    int          nbre   = grid->countVertex();
    double       dmin   = 1e+6;
    Hex::Vertex* result = NULL;
    double       px = vertex->getX();
    double       py = vertex->getY();
    double       pz = vertex->getZ();
    for (int nro=0 ; nro < nbre ; ++nro)
        {
        Hex::Vertex* v1 = grid->getVertex (nro);
        double       d2 = Hex::carre(px-v1->getX()) + Hex::carre(py-v1->getY())
                                                    + Hex::carre(pz-v1->getZ()) ;
        if (d2 < dmin)
           {
           result = v1;
           dmin   = d2;
           }
        }

    cout << " nearest : " << vertex->getName()
         << " -> "        << result->getName()
         << " ( " << px << ",  " << py << ",  " << pz << ") " << endl;
    return result;
}

// ======================================================== eplucher
void eplucher (Hex::Document* doc, Hex::Elements* grid, string& radical)
{
   int nbfic = 0;
   int nbre  = grid->countHexa();
   for (int nro=nbre-1 ; nro >0 ; nro--)
       {
       nbfic++;
       char name [32];
       sprintf (name, "%s%02d.vtk", radical.c_str(), nbfic);
       grid->saveVtk (name);
       doc->removeHexa (grid->getHexa(nro));
       }
}
// ======================================================== test_cartesian6
int test_cartesian6 (int nbargs, cpchar tabargs[])
{
   case_name = "cartesian_top";
   Hex::Document* doc  = get_document ();

/***********************
   const int size_x = 3;
   const int size_y = 4;
   const int size_z = 5;

   Hex::Elements* grid1 = doc->makeCartesianTop (size_x, size_y, size_z);
   grid1->saveVtk ("makeCartesianTop.vtk");
   PutData (grid1->isValid());
   return HOK;
***************************/

   Hex::Vertex*   orig2 = doc->addVertex (0, 0, 0);
   Hex::Vector*   vx    = doc->addVector (1, 0, 0);
   Hex::Vector*   vy    = doc->addVector (0, 1, 0);
   Hex::Vector*   vz    = doc->addVector (0, 0, 1);
   int   nx = 2, ny=1, nz=2;
   double dx =nx, dy=ny, dz=nz;
   Hex::Elements* grid2 = doc->makeCartesianUni (orig2, vx, vy, vz, dx, dy, dz,
                                                                    nx, ny, nz);
   grid2->saveVtk ("makeCartesianUni.vtk");
   PutData (grid2->isValid());
   return HOK;

   Hex::Vertex*   orig3 = doc->addVertex (20, 0, 0);
   Hex::RealVector tx, ty, tz;
   for (int nro=1 ; nro<6 ; nro++)
       {
       tx.push_back (nro);
       ty.push_back (nro);
       tz.push_back (nro);
       }

   tz.push_back (1);
   Hex::Elements* grid3 = doc->makeCartesian (orig3, vx, vy, vz, tx, ty, tz);
   grid3->saveVtk ("makeCartesian.vtk");
   PutData (grid3->isValid());

   // doc->dump ();
   return HOK;
}
// ======================================================== test_cylinder60
int test_cylinder60 (int nbargs, cpchar tabargs[])
{
   case_name = "cylinders";
   const int size_x = 1;
   const int size_y = 4;
   const int size_z = 1;

   Hex::Document* doc = get_document ();
   Hex::Vertex*  orig = doc->addVertex (0, 0, 0);
   Hex::Vector*  vx   = doc->addVector (1, 0, 0);
   Hex::Vector*  vz   = doc->addVector (0, 0, 1);
   double rint =1, rext = 3, angle = 360, hauteur = 2;
   Hex::Elements* grid3 = doc->makePipeUni (orig, vx, vz,
                                                rint, rext, angle, hauteur,
                                                size_x, size_y, size_z);
   grid3->saveVtk ("makeCylinderUni.vtk");
   PutData (grid3->isValid());
   if (NOT (grid3->isValid()))
       Hex::what ();

   doc->dump ();
   return HOK;
}
// ======================================================== test_cylinder_top
int test_cylinder_top (int nbargs, cpchar tabargs[])
{
   case_name = "cylinders";
   const int size_x = 1;
   const int size_y = 3;
   const int size_z = 1;

   Hex::Document* doc   = get_document ();
   Hex::Elements* grid1 = doc->makeCylinderTop (size_x, size_y, size_z);

   grid1->saveVtk ("makeCylinderTop.vtk");
   PutData (grid1->isValid());
   return HOK;
}
// ======================================================== test_cylinder6
int test_cylinder6 (int nbargs, cpchar tabargs[])
{
   case_name = "cylinders";
   Hex::Document* doc   = get_document ();

   /********************
   const int size_x = 3;
   const int size_y = 8;
   const int size_z = 2;

   Hex::Elements* grid1 = doc->makeCylinderTop (size_x, size_y, size_z);
   grid1->saveVtk ("makeCylinderTop.vtk");
   PutData (grid1->isValid());


   Hex::Elements* grid2 = doc->makePipeTop (size_x, size_y, size_z);
   grid2->saveVtk ("makePipeTop.vtk");
   PutData (grid2->isValid());
   ******************************/

   Hex::Vertex*  orig2 = doc->addVertex (0, 0, 5);
   Hex::Vector*  vx    = doc->addVector (1, 0, 0);
   Hex::Vector*  vz    = doc->addVector (0, 0, 1);
   double rint =2, rext = 4, angle = 300, hauteur = 1;
   Hex::Elements* grid3 = doc->makeCylinderUni (orig2, vx, vz,
                                                rint, rext, angle, hauteur,
                                                2, 3, 2);
                                    //          size_x, size_y, size_z);
   grid3->saveVtk ("makeCylinderUni1.vtk");
   PutData (grid3->isValid());
   if (NOT (grid3->isValid()))
       Hex::what ();
   return HOK;

#if 0
   Hex::Vertex*   orig3 = doc->addVertex (20, 0, 0);
   Hex::RealVector tx, ty, tz;
   for (int nro=1 ; nro<6 ; nro++)
       {
       tx.push_back (nro);
       ty.push_back (nro);
       tz.push_back (nro);
       }

   tz.push_back (1);
   Hex::Elements* grid3 = doc->makeCartesian (orig3, vx, vy, vz, tx, ty, tz);
   grid3->saveVtk ("makeCartesian.vtk");
   PutData (grid3->isValid());

   // doc->dump ();
#endif
   return HOK;
}
// ======================================================== test_spherical6
int test_spherical6 (int nbargs, cpchar tabargs[])
{
   case_name = "spherical";
   const int size_x = 3;

   Hex::Document* doc  = get_document ();

   Hex::Elements* grid1 = doc->makeSphericalTop (size_x);
   PutData (grid1->isValid());

   int nbhexas = grid1->countHexa ();
   PutData (nbhexas);
   for (int nro=3 ; nro<nbhexas ; nro +=3)
       grid1->getHexa(nro)->remove();

   grid1->saveVtk ("makeSphericalTop.vtk");

   Hex::Vertex*  orig2 = doc->addVertex (0, 0, 10);
   Hex::Vector*  vx    = doc->addVector (1, 0, 0);
   Hex::Vector*  vz    = doc->addVector (0, 1, 1);
   double rayon = 1;
   Hex::Elements* grid2 = doc->makeSphericalUni (orig2, vx, vz, rayon, size_x);
   if (NOT (grid1->isValid()))
       Hex::what ();

   PutData (grid2->isValid());
   if (NOT (grid2->isValid()))
       Hex::what ();

   for (int nro=3 ; nro<nbhexas ; nro +=3)
       grid2->getHexa(nro)->remove();

   grid2->saveVtk ("makeSphericalUni.vtk");
   return HOK;
}
// ======================================================== test_rind6
int test_rind6 (int nbargs, cpchar tabargs[])
{
   case_name = "rind";
   const int size_r = 4;
   const int size_a = 8;
   const int size_h = 12;

   Hex::Document* doc  = get_document ();

   Hex::Elements* grid1 = doc->makeSphereTop (size_r, size_a, size_h);
   PutData (grid1->isValid());
   if (NOT (grid1->isValid()))
       Hex::what ();
   else
       grid1->saveVtk ("makeSphereTop.vtk");

   Hex::Elements* grid2 = doc->makeRindTop (size_r, size_a, size_h);
   PutData (grid2->isValid());
   if (NOT (grid2->isValid()))
       Hex::what ();
   else
       grid2->saveVtk ("makeRindTop.vtk");

   Hex::Vertex* orig  = doc->addVertex (0, 0, 0);
   Hex::Vertex* vplan = doc->addVertex (0, 0, -0.5);
   Hex::Vector* vx    = doc->addVector (1, 0, 0);
   Hex::Vector* vz    = doc->addVector (0, 0, 1);
   double rtrou = 1;
   double rint  = 8;
   double rext  = 10;
   double angle = 180;

   Hex::Elements* grid3 = doc->makeSphereUni (orig, vx, vz,
                                              rtrou, rext, angle, vplan,
                                              size_r, size_a, size_h);
   if (NOT (grid3->isValid()))
       Hex::what ();
   else
       grid3->saveVtk ("makeSphereUni.vtk");

   Hex::Elements* grid4 = doc->makeRindUni (orig, vx, vz,
                                            rtrou, rint, rext, angle, vplan,
                                            size_r, size_a, size_h);
   if (NOT (grid4->isValid()))
       Hex::what ();
   else
       grid4->saveVtk ("makeRindUni.vtk");

   Hex::RealVector trad, tang, tphi;
   double dr     = (rext-rtrou)/size_r;
   double dtheta = angle/size_a;
   double dphi   = 180.0/size_h;
   for (int nro=0 ; nro<=size_r ; nro++) trad.push_back (rtrou + nro*dr);
   for (int nro=0 ; nro<=size_a ; nro++) tang.push_back (        nro*dtheta);
   for (int nro=0 ; nro<=size_h ; nro++) tphi.push_back (-90   + nro*dphi);

   Hex::Elements* grid6 = doc->makeSphere (orig, vx, vz, trad, tang, tphi);
   if (NOT (grid6->isValid()))
       Hex::what ();
   else
       grid6->saveVtk ("makeSphereNew.vtk");

   trad.clear ();
   trad.push_back (rtrou);
   for (int nro=0 ; nro<=size_r ; nro++) trad.push_back (rint + nro*dr);
   Hex::Elements* grid7 = doc->makeRind (orig, vx, vz, trad, tang, tphi);
   if (NOT (grid7->isValid()))
       Hex::what ();
   else
       grid7->saveVtk ("makeRindNew.vtk");
   return HOK;
}
// ======================================================== test_pipes6
int test_pipes6 (int nbargs, cpchar tabargs[])
{
   case_name    = "pipes";
   bool t_left  = false;
   bool t_right = false;
   bool pipe    = true;

   for (int narg=1 ; narg<nbargs ; narg++)
       {
       string arg = tabargs [narg];
       if (arg=="-cyl")        pipe    = false;
       else if (arg=="-pipe")  pipe    = true;
       else if (arg=="-left")  t_left  = true;
       else if (arg=="-right") t_right = true;
       else
          {
          cout << " Syntax : " << endl;
          cout << " $ " << tabargs[0]  << " <args> " << endl;
          cout << " avec args parmi :" << endl;
          cout << " -cyl   : croisement de 2 cylindres " << endl;
          cout << " -pipe  : croisement de 2 tuyaux "    << endl;
          cout << " -left  : Forme en T, barre a gauche " << endl;
          cout << " -right : Forme en T, barre a droite " << endl;
          return HOK;
          }
       }

   string radical = pipe ? "makePipes" : "makeCylinders";

   double rext1 =  2*sqrt (2.0);
   double rext2 =  3*sqrt (2.0);
   double rint2 =  rext1;
   double rint1 =  rext1/2;
   double h1   =  16;
   double h2   =  16;
   double xl1 =  -8;

   if (t_left)
      {
      h1 = 10;
      radical += "_L";
      }
   else if (t_right)
      {
      xl1 = -2;
      h1   = 10;
      radical += "_R";
      }

   Hex::Document* doc  = get_document ();

   Hex::Vertex* ori1  = doc->addVertex ( 0, 0,  xl1);
   Hex::Vertex* ori2  = doc->addVertex (-8, 0,  0);
   Hex::Vector* vz1   = doc->addVector ( 0, 0,  1);
   Hex::Vector* vz2   = doc->addVector ( 1, 0,  0);

   Hex::BiCylinder* grid1 = NULL;
   if (pipe)
       grid1 = doc->makePipes (ori1, vz1, rint1, rext1, h1, ori2, vz2, rint2,
                               rext2, h2);
   else
       grid1 = doc->makeCylinders (ori1, vz1, rext1, h1, ori2, vz2, rext2, h2);

   PutData (grid1->isValid());
   Hex::what ();
   if (grid1->isValid())
       eplucher (doc, grid1, radical);

   return HOK;
}
// ======================================================== test_extrude6
int test_extrude6 (int nbargs, cpchar tabargs[])
{
   const int dimx = 11;
   const int dimy = 11;
   const int dimz = 2;

   Hex::Document* doc = get_document ();

   Hex::Vertex* orig1 = doc->addVertex ( 0,0,0);
   Hex::Vector* vx    = doc->addVector ( 1,0,0);
   Hex::Vector* vy    = doc->addVector ( 0,1,0);
   Hex::Vector* vz    = doc->addVector ( 0,0,1);
   Hex::Vector* dir1  = doc->addVector ( 1,1,1);
   Hex::Vector* dir2  = doc->addVector ( 1,1,-1);

   double dx =1, dy=1, dz=1;
   Hex::Elements* grid1 = doc->makeCartesianUni (orig1, vx,vy,vz, dx,dy,dz,
                                                 dimx,dimy,dimz);

   int mx = dimx/2;
   int my = dimy/2;
   Hex::Quads liste1, liste2;

   liste1.push_back (grid1->getQuadIJ (mx, my, dimz));
   liste2.push_back (grid1->getQuadIJ (mx, my, 0));
   for (int nx=0; nx<dimx; nx++)
       if (nx!=mx)
          {
          liste1.push_back (grid1->getQuadIJ (nx, my, dimz));
          liste2.push_back (grid1->getQuadIJ (nx, my, 0));
          }

   for (int ny=0; ny<dimy; ny++)
       if (ny!=my)
          {
          liste1.push_back (grid1->getQuadIJ (mx, ny, dimz));
          liste2.push_back (grid1->getQuadIJ (mx, ny, 0));
          }

   Hex::RealVector tlen;
   double dh0 = 1;
   double dh  = 0.02;
   double lh = 0;
   for (int nro=0; nro<5; nro++)
       {
       dh = 1.5*dh + 1;
       lh += dh;
       tlen.push_back (lh);
       }

   const int nbiter = 5;
   doc->saveVtk ("prisme1.vtk");
   Hex::Elements* prisme2 = doc->extrudeQuadsUni (liste2, dir2, dh0, nbiter);
   doc->saveVtk ("prisme2.vtk");

   Hex::Elements* prisme1 = doc->extrudeQuads (liste1, dir1, tlen);

   PutData (liste1.size());
   PutData (tlen.size());
   PutData (prisme1->countHexa());
   PutData (prisme1->countQuad());
   PutData (prisme1->countEdge());
   PutData (prisme1->countVertex());

   for (int nro=0 ; nro <nbiter  ; nro++)
       {
       Hex::Hexa* cell = prisme2-> getHexa (nbiter+nro);
       cell->setScalar (5);
       }

   doc->saveVtk ("prisme3.vtk");
   return HOK;
}
// ======================================================== test_extrude7
int test_extrude7 (int nbargs, cpchar tabargs[])
{
   const int nr = 2;
   const int na = 8;
   const int nh = 3;

   Hex::Document* doc   = get_document ();
   Hex::Elements* grid1 = doc->makeCylinderTop (nr, na, nh);

   Hex::Quads tquads;
   for (int k=0 ; k<nh ; k++)
       {
       Hex::Quad* quad = grid1->getQuadJK (nr, 0, k);
       quad->setColor   (5);
       tquads.push_back (quad);
       }

   doc->saveVtk ("grid0.vtk");
   Hex::Elements* grid2 = doc->extrudeQuadsTop (tquads, 4);
   doc->saveVtk ("grid1.vtk");
   return HOK;
}
// ======================================================== test_revolution6
int test_revolution6 (int nbargs, cpchar tabargs[])
{
   Hex::Document* doc = get_document ();

   Hex::Vertex* centre = doc->addVertex (0, 0, 0);
   Hex::Vector* vz     = doc->addVector (0, 0, 1);

   Hex::Vertex* v1     = doc->addVertex (10, 0, 0);
   Hex::Vertex* v2     = doc->addVertex (11, 0, 0);
   Hex::Vertex* v3     = doc->addVertex (11, 0, 2);
   Hex::Vertex* v4     = doc->addVertex (10, 0, 2);
   Hex::Quad* quad     = doc->addQuadVertices (v1, v2, v3, v4);

   doc->revolutionQuadUni  (quad, centre, vz, 180, 8);
   doc->saveVtk ("revolution1.vtk");
   return HOK;

   int nr = 1;
   int na = 6;
   int nl = 1;

   Hex::Elements* grid = doc->makeCylinderTop (nr,na,nl);

   Hex::Quads liste;
   for (int nx=0; nx<nr; nx++)
       for (int ny=0; ny<na; ny++)
           {
           Hex::Quad* cell = grid->getQuadIJ (nx, ny, nl);
           liste.push_back (cell);
           cell -> setScalar (5);
           }

   Hex::Vertex* center = doc->addVertex (0, -10, 0);
   center -> setScalar (6);
   Hex::Vector* axis   = doc->addVector (1,   0, 0);

   doc->saveVtk ("revolution1.vtk");

   Hex::Elements* grid1 = doc->revolutionQuadsUni  (liste, center, axis, 180, 9);
   Hex::what ();
   if (grid1->isValid())
       doc->saveVtk ("revolution2.vtk");

   return HOK;
}
// ======================================================== test_joint6
int test_joint6 (int nbargs, cpchar tabargs[])
{
   const int dimx = 11;
   const int dimy = 11;
   const int dimz = 2;

   Hex::Document* doc   = get_document ();

   Hex::Elements* grid1 = doc->makeCartesianTop (dimx,dimy,dimz);

   Hex::Vertex*   orig2 = doc->addVertex  (dimx/2.0,0,8);
   Hex::Vector*   vectj = doc->addVector (0,1,0);
   Hex::Vector*   vecti = doc->addVector (1,0,0);
   Hex::Elements* grid2 = doc->makeCylinderUni (orig2, vecti, vectj,
                   1.0, 2.0, 180.0, 11.0,        dimz,dimy,dimx);

   int mx = dimx/2;
   int my = dimy/2;
   Hex::Quad* prems = grid1->getQuadIJ (mx, my, dimz);
   Hex::Quad* cible = grid2->getQuadJK (dimz, mx, my);

   Hex::Vertex* va1 = prems->getVertex (0);
   Hex::Vertex* va2 = prems->getVertex (1);

   Hex::Vertex* vb1 = cible->getVertex (1);
   Hex::Vertex* vb2 = cible->getVertex (2);


   Hex::Quads liste;

   liste.push_back (prems);
   for (int nx=0; nx<dimx; nx++)
       if (nx!=mx)
          liste.push_back (grid1->getQuadIJ (nx, my, dimz));

   for (int ny=0; ny<dimy; ny++)
       if (ny!=my)
          liste.push_back (grid1->getQuadIJ (mx, ny, dimz));

   doc->saveVtk ("jointQuad1.vtk");
   const int hauteur = 5;
   Hex::Elements* joint = doc->joinQuadsUni (liste, cible, va1,vb1,va2,vb2,
                                             hauteur);
   // for (int nh=0 ; nh<hauteur ; nh++) joint->getHexa(nh)->setScalar (5);

   Hex::what ();
   if (NOT joint->isValid ())
      return HERR;

   doc->saveVtk ("jointQuad2.vtk");
   int nbr_joint_vertex =  joint->countVertex ();
   int nbr_surf_vertex  =  nbr_joint_vertex/(hauteur+1);

   HexDisplay (nbr_joint_vertex);
   HexDisplay (nbr_surf_vertex);

   int indice0 = joint->findVertex (va1);
   HexDisplay (indice0);

   for (int nh=0 ; nh<nbr_surf_vertex ; nh++)
       joint->getVertex(nh)->setScalar (5);

   for (int nh=0 ; nh<=hauteur ; nh++)
       joint->getVertex(nh*nbr_surf_vertex)->setScalar (3);

   doc->saveVtk ("jointQuad3.vtk");
   return HOK;
}
// ======================================================== test_cut6
int test_cut6 (int nbargs, cpchar tabargs[])
{
   const int size_x = 2;
   const int size_y = 1;
   const int size_z = 1;

   Hex::Document* doc = get_document ();

   Hex::Elements* grid  = doc->makeCartesianTop (size_x,size_y,size_z);
   Hex::Edge*     arete = grid->getEdgeK (0, 0, 0);

   //  doc ->dump ();
   int nvtk=0;
   doc ->saveVtk ("decoupe", nvtk);
   Hex::Elements* grid2 = doc->cutUni (arete, 2);
   Hex::what ();
   if (NOT grid2->isValid ())
      return HERR;
   doc ->saveVtk ("decoupe", nvtk);

   Hex::RealVector tablen;
   double reste     = 1;
   double abscisse  = 0;
   for (int nl=0 ; nl < 5 ; nl++)
       {
       reste     /= 2;
       abscisse  += reste;
       tablen.push_back (abscisse);
       }
   arete = grid2->getEdge (0);
   Hex::Elements* grid3 = doc->cut (arete, tablen);
   if (NOT grid3->isValid ())
      return HERR;

   doc ->saveVtk ("decoupe", nvtk);
   return HOK;
}
// ======================================================== test_six
void test_six (Hex::Document* doc, int action)
{
   cpchar tabargs[] = { "salome" };
   int    nbargs    = 1;

   docu = doc;
   switch (action)
       {
       case 101 : test_cartesian6 (nbargs, tabargs);
            break;

       case 102 : test_cylinder6 (nbargs, tabargs);
            break;

       case 103 : test_spherical6 (nbargs, tabargs);
            break;

       case 104 : test_rind6 (nbargs, tabargs);
            break;

       case 105 : test_pipes6 (nbargs, tabargs);
            break;

       case 106 : test_extrude6 (nbargs, tabargs);
            break;

       case 107 : test_joint6 (nbargs, tabargs);
            break;

       case 108 : test_cut6 (nbargs, tabargs);
            break;

       default  : ;
 }
}
// ======================================================== test_monica
int test_monica (int nbargs, cpchar tabargs[])
{
   case_name = "monica";

   Hex::Document* doc   = get_document ();
   Hex::Vertex*   orig  = doc->addVertex (0, 0, 0);
   Hex::Vertex*   orig2 = doc->addVertex (1.5, 1.5, -1);

   Hex::Vector*   vx  = doc->addVector (1, 0, 0);
   Hex::Vector*   vy  = doc->addVector (0, 1, 0);
   Hex::Vector*   vz  = doc->addVector (0, 0, 1);
   Hex::Vector*   vxy = doc->addVector (1, 1, 0);

   Hex::Elements* plaque = doc->makeCartesianUni (orig, vx,vy,vz, 1,1,1, 3,3,1);
   save_file ();

   Hex::Hexa* trou = plaque->getHexaIJK (1, 1, 0);
   vector <Hex::Vertex*> tabv0, tabv1;
   for (int nv=0 ; nv<8 ; nv++)
       tabv1.push_back (trou->getVertex(nv));

   doc->dump ();
   doc->removeHexa (trou);
   Hex::what ();
   save_file ();

   Hex::Elements* pipe = doc->makePipeUni (orig2, vxy, vz, 0.15, 0.5, 360, 3,
                                           1, 4, 3);
   doc->dump ();
   Hex::what ();
   for (int nv=0 ; nv<8 ; nv++)
       tabv0.push_back (nearest (pipe, tabv1[nv]));

   doc->setLevel (1);
   for (int nv=0 ; nv<8 ; nv++)
       {
       int ier = doc->mergeVertices (tabv0[nv], tabv1[nv]);
       cout << " nro = " << nv << ", ier = " << ier << endl;
       Hex::what ();
       save_file ();
       }

   doc->dump ();
   return HOK;
}
// ======================================================== test_abu
int test_abu (int nbargs, cpchar tabargs[])
{
   case_name = "abu";

   Hex::Document* doc   = get_document ();
   doc->makeCylinderTop (1,3,1);
   Hex::what ();
   PutData (doc->countVertex());
   save_file ();
   save_file ();
   doc->save (case_name);
   // doc->dump ();
   return HOK;
}
