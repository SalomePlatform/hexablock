
// C++ : Fonctions de creation HexaBlock v6

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

#include "HexDocument.hxx"

#include "HexElements.hxx"
#include "HexBiCylinder.hxx"
#include "HexVector.hxx"
#include "HexQuad.hxx"
#include "HexHexa.hxx"

#include "HexGlobale.hxx"

BEGIN_NAMESPACE_HEXA


// ======================================================== make_normale
int make_normale (Quads& tquads, double axe[], double& largeur)
{
   double lgmoy = 0;
   Real3  cf, cg, vn;
   int    nbre = tquads.size();
   if (nbre==0)
       return HERR;

   double coeff = 0;
   for (int nro=0 ; nro<nbre ; ++nro)
       {
       Quad* quad = tquads[nro];
       int nbp    = quad->getNbrParents();
       if (nbp!=1)
          return HERR;
                        // Calcul des cotes
       for (int nc=0 ; nc<QUAD4 ; ++nc) 
           lgmoy += quad->getEdge(nc)->getLength ();
                        // Calcul de la normale sortante
       Hexa* hexa = quad->getParent (0);
       hexa->getCenter (cg);
       quad->getCenter (cf);
       calc_vecteur (cg, cf, vn);
       for (int nc=0 ; nc<DIM3 ; ++nc) 
           axe[nc] = coeff*axe[nc] + vn[nc];
       coeff = 1;
       }
   
   normer_vecteur (axe);
   largeur = lgmoy / (4*nbre);
   return HOK;
}
// ======================================================== makeCartesianTop
Elements* Document::makeCartesianTop (int nx, int ny, int nz)
{
   DumpStart ("makeCartesianTop", nx << ny << nz );
   Elements* grid = new Elements (this);

   RealVector tx, ty, tz;
   Real3 rx = { 1, 0, 0 };
   Real3 ry = { 0, 1, 0 };
   Real3 rz = { 0, 0, 1 };

   grid->checkSize (1, nx, ny, nz, false);
   if (grid->isValid())
      {
      Vertex* orig = addVertex (0, 0, 0);
      for (int nro=1 ; nro<=nx; ++nro) tx.push_back (nro); 
      for (int nro=1 ; nro<=ny; ++nro) ty.push_back (nro); 
      for (int nro=1 ; nro<=nz; ++nro) tz.push_back (nro); 
      grid->makeCartesian (orig, rx, ry, rz, tx, ty, tz);
      }

   DumpReturn (grid);
   return grid;
}
// ======================================================== makeCartesianUni
Elements* Document::makeCartesianUni (Vertex* orig, 
                                      Vector* vx, Vector* vy, Vector* vz,
                                      double  lx, double  ly, double  lz, 
                                      int     nx, int     ny, int     nz)
{
   DumpStart ("makeCartesianUni", orig << vx << vy << vz 
                                       << lx << ly << lz
                                       << nx << ny << nz);

   Elements* grid = new Elements (this);

   Real3 rx, ry, rz;
   grid->checkOrig   (1, orig);
   grid->checkSize   (8, nx, ny, nz, false);
   grid->checkSystem (2, vx, vy, vz, rx, ry, rz);

   if (grid->isValid())
      {
      RealVector tx, ty, tz;
      double dx = lx / nx;
      double dy = ly / ny;
      double dz = lz / nz;
      for (int nro=1 ; nro<=nx; ++nro) tx.push_back (nro*dx); 
      for (int nro=1 ; nro<=ny; ++nro) ty.push_back (nro*dy); 
      for (int nro=1 ; nro<=nz; ++nro) tz.push_back (nro*dz); 

      grid->makeCartesian (orig, rx, ry, rz, tx, ty, tz);
      }

   DumpReturn (grid);
   return grid;
}
// ======================================================== makeCartesian
Elements* Document::makeCartesian (Vertex*    orig, 
                                   Vector*    vx, Vector*    vy, Vector*    vz,
                                   RealVector tx, RealVector ty, RealVector tz)
{
   DumpStart ("makeCartesian", orig << vx << vy << vz << tx << ty << tz);

   Elements* grid = new Elements (this);

   Real3 rx, ry, rz;
   grid->checkOrig   (1, orig);
   grid->checkSystem (2, vx, vy, vz, rx, ry, rz);
   grid->checkVector (5, tx);
   grid->checkVector (6, ty);
   grid->checkVector (7, tz);

   if (grid->isValid())
       grid->makeCartesian (orig, rx, ry, rz, tx, ty, tz);

   DumpReturn (grid);
   return grid;
}
// ======================================================== makeCylinderTop
Elements* Document::makeCylinderTop (int nr, int na, int nh)
{
   DumpStart ("makeCylinderTop", nr << na << nh);

   Elements* grid = new Elements (this);

   RealVector tray, tang, thaut;
   Real3 rx = { 1, 0, 0 };
   Real3 rz = { 0, 0, 1 };

   grid->checkSize (1, nr, na, nh, true);
   if (grid->isValid())
      {
      double da = 360.0 / na;
      for (int nro=0 ; nro<=nr; ++nro) tray .push_back (nro+1); 
      for (int nro=0 ; nro<=na; ++nro) tang .push_back (nro*da); 
      for (int nro=0 ; nro<=nh; ++nro) thaut.push_back (nro); 
      grid->makeCylinder (NULL, rx, rz, tray, tang, thaut, true);
      }

   DumpReturn (grid);
   return grid;
}
// ======================================================== makeCylinderUni
Elements* Document::makeCylinderUni (Vertex* orig, Vector* vx, Vector* vz, 
                           double rint, double rext, double ang, double haut,
                           int nr, int na, int nh)
{
   DumpStart ("makeCylinderUni", orig << vx << vz << rint << rext 
                                      << ang << haut << nr << na << nh);

   Elements* grid = new Elements (this);

   Real3 rx, rz;
   grid->checkOrig   (1, orig);
   grid->checkSize   (8, nr, na, nh, true);
   grid->checkPipe   (4, rint, rext, ang, haut);
   grid->checkSystem (2, vx, vz, rx, rz);

   if (grid->isValid())
      {
      double dray  = (rext-rint) / nr;
      double dang  = ang  / na;
      double dhaut = haut / nh;
      RealVector tang, tray, thaut;

      for (int nro=0 ; nro<=nr; ++nro) tray .push_back (rint + nro*dray); 
      for (int nro=0 ; nro<=na; ++nro) tang .push_back (nro*dang); 
      for (int nro=0 ; nro<=nh; ++nro) thaut.push_back (nro*dhaut); 

      grid->makeCylinder (orig, rx, rz, tray, tang, thaut, true);
      }

   DumpReturn (grid);
   return grid;
}
// ======================================================== makeCylinder
Elements* Document::makeCylinder (Vertex* orig, Vector* vx, Vector* vz, 
                        RealVector tray, RealVector tang, RealVector thaut)
{
   DumpStart ("makeCylinder", orig << vx << vz << tray << tang << thaut);

   Elements* grid = new Elements (this);

   Real3 rx, rz;
   grid->checkOrig   (1, orig);
   grid->checkSystem (2, vx, vz, rx, rz);
   grid->checkVector (4, tray);
   grid->checkVector (5, tang,  3);
   grid->checkVector (6, thaut);

   if (grid->isValid())
      grid->makeCylinder (orig, rx, rz, tray, tang, thaut, true);

   DumpReturn (grid);
   return grid;
}
// ======================================================== makePipeTop
Elements* Document::makePipeTop (int nr, int na, int nh)
{
   DumpStart ("makePipeTop", nr << na << nh);

   Elements* grid = new Elements (this);

   RealVector tray, tang, thaut;
   Real3 rx = { 1, 0, 0 };
   Real3 rz = { 0, 0, 1 };

   grid->checkSize (1,nr, na, nh, true);
   if (grid->isValid())
      {
      double da = 360.0 / na;
      for (int nro=0 ; nro<=nr; ++nro) tray .push_back (nro+1);
      for (int nro=0 ; nro<=na; ++nro) tang .push_back (nro*da); 
      for (int nro=0 ; nro<=nh; ++nro) thaut.push_back (nro); 
      grid->makeCylinder (NULL, rx, rz, tray, tang, thaut, false);
      }

   DumpReturn (grid);
   return grid;
}
// ======================================================== makePipeUni
Elements* Document::makePipeUni (Vertex* orig, Vector* vx, Vector* vz, 
                       double rint, double rext, double angle, double hauteur,
                       int nr, int na, int nh)
{
   DumpStart ("makePipeUni", orig << vx << vz << rint << rext 
                                  << angle << hauteur << nr << na << nh);

   Elements* grid = new Elements (this);

   Real3 rx, rz;
   RealVector tray, tang, thaut;
   grid->checkOrig   (1, orig);
   grid->checkSize   (8, nr, na, nh, true);
   grid->checkPipe   (4, rint, rext, angle, hauteur);
   grid->checkSystem (1, vx, vz, rx, rz);

   if (grid->isValid())
      {
      double dray  = (rext-rint) / nr;
      double dang  = angle   / na;
      double dhaut = hauteur / nh;
      for (int nro=0 ; nro<=nr; ++nro) tray .push_back (rint + nro*dray); 
      for (int nro=0 ; nro<=na; ++nro) tang .push_back (nro  * dang); 
      for (int nro=0 ; nro<=nh; ++nro) thaut.push_back (nro  * dhaut); 

      grid->makeCylinder (orig, rx, rz, tray, tang, thaut, false);
      }
   DumpReturn (grid);
   return grid;
}
// ======================================================== makePipe
Elements* Document::makePipe (Vertex* orig, Vector* vx, Vector* vz, 
                       RealVector tray, RealVector tang, RealVector thaut)
{
   DumpStart ("makePipe", orig << vx << vz << tray << tang << thaut);

   Elements* grid = new Elements (this);

   Real3 rx, rz;
   grid->checkOrig   (1, orig);
   grid->checkSystem (2, vx, vz, rx, rz);
   grid->checkVector (4, tray,  2);
   grid->checkVector (5, tang,  3);
   grid->checkVector (6, thaut, 2);

   if (grid->isValid())
      grid->makeCylinder (orig, rx, rz, tray, tang, thaut, false);

   DumpReturn (grid);
   return grid;
}
// ======================================================== makeSphericalTop
Elements* Document::makeSphericalTop (int nbre, int crit)
{
   DumpStart ("makeSphericalTop", nbre << crit);
   Elements* grid = new Elements (this);

   Real3 orig = { 0, 0, 0 };
   Real3 rx   = { 1, 0, 0 };
   Real3 rz   = { 0, 0, 1 };

   if (nbre<2) 
      {
      grid->setError (HERR);
      Mess << "Argument number 1 :";
      Mess << "Nbr slices must be greather or equal than 2"; 
      }

   if (grid->isValid())
      {
      RealVector trayon;
      for (int nro=1 ; nro<=nbre; ++nro) trayon.push_back (nro); 
      grid->makeSpherical (orig, rx, rz, trayon, crit);
      }

   DumpReturn (grid);
   return grid;
}
// ======================================================== makeSphericalUni
Elements* Document::makeSphericalUni (Vertex* center, Vector* vx, Vector* vz, 
                                      double rayon, int nbre, int crit)
{
   DumpStart ("makeSphericalUni", center << vx << vz << rayon << nbre << crit);
   Elements* grid = new Elements (this);

   Real3 rx, rz;
   grid->checkOrig   (1, center);
   grid->checkSystem (2, vx, vz, rx, rz);

   if (rayon<=1e-5) 
      {
      grid->setError (HERR);
      Mess << "Argument number 4 :";
      Mess << "Radius must be positive"; 
      }

   if (nbre<2) 
      {
      grid->setError (HERR);
      Mess << "Argument number 5 :";
      Mess << "Nbr slices must be greather or equal than 2"; 
      }

   if (grid->isValid())
      {
      Real3      orig;
      RealVector trayon;
      center->getPoint (orig);
      for (int nro=1 ; nro<=nbre; ++nro) trayon.push_back (nro*rayon); 
      grid->makeSpherical (orig, rx, rz, trayon, crit);
      }

   DumpReturn (grid);
   return grid;
}
// ======================================================== makeSpherical
Elements* Document::makeSpherical (Vertex* center, Vector* vx, Vector* vz, 
                                   RealVector trayon, int crit)
{
   DumpStart ("makeSpherical", center << vx << vz << trayon << crit);
   Elements* grid = new Elements (this);

   Real3 rx, rz;
   grid->checkOrig   (1, center);
   grid->checkSystem (2, vx, vz, rx, rz);
   grid->checkVector (4, trayon, 2);

   if (grid->isValid())
      {
      Real3 orig;
      center->getPoint (orig);
      grid->makeSpherical (orig, rx, rz, trayon, crit);
      }

   
   DumpReturn (grid);
   return grid;
}
// ======================================================== makeSphereTop
Elements* Document::makeSphereTop (int nr, int na, int nh)
{
   DumpStart ("makeSphereTop", nr << na << nh);
   Elements* grid = new Elements (this);

   Real3 orig = { 0, 0, 0 };
   Real3 rx   = { 1, 0, 0 };
   Real3 rz   = { 0, 0, 1 };

   double rtrou = 1;
   double rext  = 10;
   double angle = 360;
   double phi0, phi1;

   grid->checkPhi (true, orig, rz, rext, rtrou, NULL, phi0, phi1);

   if (nr<2) 
      {
      grid->setError (HERR);
      Mess << "Argument number 1 :";
      Mess << "Nbr slices must be greather or equal than 2"; 
      }

   if (na<3) 
      {
      grid->setError (HERR);
      Mess << "Argument number 2 :";
      Mess << "Nbr sectors must be greather or equal than 3"; 
      }

   if (nh<2) 
      {
      grid->setError (HERR);
      Mess << "Argument number 3 :";
      Mess << "Nbr slices must be greather or equal than 2"; 
      }

   if (grid->isValid())
      {
      double dr = (rext-rtrou)/nr;
      double da = angle / na;
      double dh = (phi1-phi0) / nh;
      RealVector tray, tang, thaut;
      for (int nro=0 ; nro<=nr; ++nro) tray .push_back (rtrou + nro*dr);
      for (int nro=0 ; nro<=na; ++nro) tang .push_back (nro*da);
      for (int nro=0 ; nro<=nh; ++nro) thaut.push_back (phi0  + nro*dh);
      grid->makeRind (GR_HEMISPHERIC, orig, rx, rz, tray, tang, thaut);
      }

   DumpReturn (grid);
   return grid;
}
// ======================================================== makeSphereUni
Elements* Document::makeSphereUni (Vertex* center, Vector* vx, Vector* vz, 
                            double rtrou, double rext, double ang, 
                            Vertex* hplan, int nr, int na, int nh)
{
   DumpStart ("makeSphereUni", center << vx << vz << rtrou << rext << ang 
                                      << hplan << nr << na << nh);
   Elements* grid = new Elements (this);

   Real3 rx, rz;
   grid->checkOrig   (1, center);
   grid->checkSystem (2, vx, vz, rx, rz);

   Real3 orig = { 0, 0, 0 };
   if (center != NULL) 
       center->getPoint (orig);

   double phi0, phi1;
   grid->checkPhi (true, orig, rz, rext, rtrou, hplan, phi0, phi1);

   if (rtrou<=1e-5) 
      {
      grid->setError (HERR);
      Mess << "Argument number 4 :";
      Mess << "Hole radius must be positive"; 
      }

   if (rext<rtrou) 
      {
      grid->setError (HERR);
      Mess << "Argument number 5 :";
      Mess << "External radius must be greather than hole radius"; 
      }

   if (ang<1.0)
      {
      grid->setError (HERR);
      Mess << "Argument number 6 :";
      Mess << "Angle must be positive"; 
      }

   if (nr<2) 
      {
      grid->setError (HERR);
      Mess << "Argument number 7 :";
      Mess << "Nbr slices must be greather or equal than 2"; 
      }

   if (na<3) 
      {
      grid->setError (HERR);
      Mess << "Argument number 8 :";
      Mess << "Nbr sectors must be greather or equal than 3"; 
      }

   if (nh<2) 
      {
      grid->setError (HERR);
      Mess << "Argument number 9 :";
      Mess << "Nbr slices must be greather or equal than 2"; 
      }

   EnumGrid type = ang>= 359.9 ? GR_HEMISPHERIC : GR_PART_SPHERIC;
   if (grid->isValid())
      {
      double dr = (rext-rtrou)/nr;
      double dh = (phi1-phi0) /nh;
      double da = ang / na;
      RealVector tray, tang, thaut;
      for (int nro=0 ; nro<=nr; ++nro) tray .push_back (rtrou + nro*dr);
      for (int nro=0 ; nro<=na; ++nro) tang .push_back (nro*da);
      for (int nro=0 ; nro<=nh; ++nro) thaut.push_back (phi0  + nro*dh);
      grid->makeRind (type, orig, rx, rz, tray, tang, thaut);
      }

   DumpReturn (grid);
   return grid;
}
// ======================================================== makeSphere
Elements* Document::makeSphere (Vertex* center, Vector* vx, Vector* vz, 
                            RealVector tray, RealVector tang, RealVector thaut)
{
   DumpStart ("makeSphere", center << vx << vz << tray << tang << thaut );
   Elements* grid = new Elements (this);

   Real3 rx, rz;
   grid->checkOrig   (1, center);
   grid->checkSystem (2, vx, vz, rx, rz);
   grid->checkVector (4, tray,  2);
   grid->checkVector (5, tang,  3);
   grid->checkVector (6, thaut, 2);

   int    nb    = tang.size()-1;
   double angle = nb <0 ? 0 : tang[nb] ;
   EnumGrid type = angle >= 359.9 ? GR_HEMISPHERIC : GR_PART_SPHERIC;
   if (grid->isValid())
      {
      Real3 orig;
      center->getPoint (orig);
      grid->makeRind (type, orig, rx, rz, tray, tang, thaut);
      }

   
   DumpReturn (grid);
   return grid;
}

// ======================================================== makeRindTop
Elements* Document::makeRindTop (int nr, int na, int nh)
{
   DumpStart ("makeRindTop", nr << na << nh);
   Elements* grid = new Elements (this);

   Real3 orig = { 0, 0, 0 };
   Real3 rx   = { 1, 0, 0 };
   Real3 rz   = { 0, 0, 1 };

   double rtrou = 1;
   double rint  = 8;
   double rext  = 10;
   double angle = 360;
   double phi0, phi1;

   grid->checkPhi (false, orig, rz, rext, rtrou, NULL, phi0, phi1);

   if (nr<2) 
      {
      grid->setError (HERR);
      Mess << "Argument number 1 :";
      Mess << "Nbr slices must be greather or equal than 2"; 
      }

   if (na<3) 
      {
      grid->setError (HERR);
      Mess << "Argument number 2 :";
      Mess << "Nbr sectors must be greather or equal than 3"; 
      }

   if (nh<2) 
      {
      grid->setError (HERR);
      Mess << "Argument number 3 :";
      Mess << "Nbr slices must be greather or equal than 2"; 
      }

   if (grid->isValid())
      {
      double dr = (rext-rint)/nr;
      double da = angle / na;
      double dh = (phi1-phi0) / nh;
      RealVector tray, tang, thaut;
      tray .push_back (rtrou);
      for (int nro=0 ; nro<=nr; ++nro) tray .push_back (rint + nro*dr);
      for (int nro=0 ; nro<=na; ++nro) tang .push_back (nro*da);
      for (int nro=0 ; nro<=nh; ++nro) thaut.push_back (phi0  + nro*dh);
      grid->makeRind (GR_RIND, orig, rx, rz, tray, tang, thaut);
      }

   DumpReturn (grid);
   return grid;
}
// ======================================================== makeRindUni
Elements* Document::makeRindUni (Vertex* center, Vector* vx, Vector* vz, 
                          double rtrou, double rint, double rext,
                          double ang, Vertex* hplan, int nr, int na, int nh)
{
   DumpStart ("makeRindUni", center << vx << vz << rtrou << rint << rext << ang 
                                    << hplan << nr << na << nh);
   Elements* grid = new Elements (this);

   Real3 rx, rz;
   grid->checkOrig   (1, center);
   grid->checkSystem (2, vx, vz, rx, rz);
   grid->checkDiams  (4, rtrou, rint);
   grid->checkDiams  (5, rint,  rext);

   Real3 orig = { 0, 0, 0 };
   if (center != NULL) 
       center->getPoint (orig);

   double phi0, phi1;
   grid->checkPhi (false, orig, rz, rext, rtrou, hplan, phi0, phi1);

   if (rtrou<=1e-5) 
      {
      grid->setError (HERR);
      Mess << "Argument number 4 :";
      Mess << "Hole radius must be positive"; 
      }

   if (rint<rtrou) 
      {
      grid->setError (HERR);
      Mess << "Argument number 5 :";
      Mess << "Internal radius must be greather than hole radius"; 
      }

   if (rext<rtrou) 
      {
      grid->setError (HERR);
      Mess << "Argument number 6 :";
      Mess << "External radius must be greather than internal radius"; 
      }

   if (ang<1.0)
      {
      grid->setError (HERR);
      Mess << "Argument number 7 :";
      Mess << "Angle must be positive"; 
      }

   if (nr<2) 
      {
      grid->setError (HERR);
      Mess << "Argument number 8 :";
      Mess << "Nbr slices must be greather or equal than 2"; 
      }

   if (na<3) 
      {
      grid->setError (HERR);
      Mess << "Argument number 9 :";
      Mess << "Nbr sectors must be greather or equal than 3"; 
      }

   if (nh<2) 
      {
      grid->setError (HERR);
      Mess << "Argument number 10 :";
      Mess << "Nbr slices must be greather or equal than 2"; 
      }

   EnumGrid type = ang>= 359.9 ? GR_RIND : GR_PART_RIND;
   if (grid->isValid())
      {
      double dr = (rext-rint)/nr;
      double dh = (phi1-phi0) /nh;
      double da = ang / na;
      RealVector tray, tang, thaut;
      tray .push_back (rtrou);
      for (int nro=0 ; nro<=nr; ++nro) tray .push_back (rint + nro*dr);
      for (int nro=0 ; nro<=na; ++nro) tang .push_back (nro*da);
      for (int nro=0 ; nro<=nh; ++nro) thaut.push_back (phi0 + nro*dh);
      grid->makeRind (type, orig, rx, rz, tray, tang, thaut);
      }

   DumpReturn (grid);
   return grid;
}
// ======================================================== makeRind
Elements* Document::makeRind (Vertex* center, Vector* vx, Vector* vz, 
                          RealVector tray, RealVector tang, RealVector thaut)
{
   DumpStart ("makeRind", center << vx << vz << tray << tang << thaut );
   Elements* grid = new Elements (this);

   Real3 rx, rz;
   grid->checkOrig   (1, center);
   grid->checkSystem (2, vx, vz, rx, rz);
   grid->checkVector (4, tray,  2);
   grid->checkVector (5, tang,  3);
   grid->checkVector (6, thaut, 2);

   int    nb    = tang.size()-1;
   double angle = nb <0 ? 0 : tang[nb] ;
   EnumGrid type = angle >= 359.9 ? GR_RIND : GR_PART_RIND;
   if (grid->isValid())
      {
      Real3 orig;
      center->getPoint (orig);
      grid->makeRind (type, orig, rx, rz, tray, tang, thaut);
      }

   
   DumpReturn (grid);
   return grid;
}
// ======================================================== makeCylinders
BiCylinder* Document::makeCylinders (Vertex* ori1, Vector* vz1, double ray1, 
                double h1, Vertex* ori2, Vector* vz2, double ray2, double h2)
{
   DumpStart ("makeCylinders", ori1 << vz1 << ray1 << h1 << ori2 << vz2 
                                           << ray2<< h2);
   BiCylinder* grid = new BiCylinder (this);

   Real3 rz1, rz2;
   grid->checkOrig   (1, ori1);
   grid->checkOrig   (5, ori2);
   grid->checkSystem (2, vz1, vz2, rz1, rz2);

   if (grid->isValid())
      {
      if (ray1 < ray2) 
          grid->makeCylinders (ori1, rz1, ray1, h1, ori2, rz2, ray2, h2);
      else
          grid->makeCylinders (ori2, rz2, ray2, h2, ori1, rz1, ray1, h1);
      }

   
   DumpReturn (grid);
   return grid;
}
// ======================================================== makePipes
BiCylinder* Document::makePipes (Vertex* ori1, Vector* vz1, double rint1, 
                                 double rext1, double h1, 
      Vertex* ori2, Vector* vz2, double rint2, double rext2, double h2)
{
   DumpStart ("makePipes", ori1 << vz1 << rint1 << rext1 << h1 
                        << ori2 << vz2 << rint2 << rext2 << h2);
   BiCylinder* grid = new BiCylinder (this);

   Real3 rz1, rz2;
   grid->checkOrig   (1, ori1);
   grid->checkOrig   (6, ori2);
   grid->checkSystem (2, vz1, vz2, rz1, rz2);
   grid->checkDiams  (3, rint1,  rext1);
   grid->checkDiams  (8, rint2,  rext2);

   if (grid->isValid())
      {
      if (rext1 < rext2) 
          grid->makePipes (ori1,rz1,rint1,rext1,h1, ori2,rz2,rint2,rext2,h2);
      else
          grid->makePipes (ori2,rz2,rint2,rext2,h2, ori1,rz1,rint1,rext1,h1);
      }

   DumpReturn (grid);
   return grid;
}
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ========================================================= extrudeQuadTop
Elements* Document::extrudeQuadTop (Quad* start, int nbre)
{
   DumpStart ("extrudeQuadTop", start << nbre);

   Quads      tstart;
   RealVector tlen;
   Real3      axe  = { 0, 0, 1 };
   double     larg = 1;
   Elements*  grid = new Elements (this);

   tstart.push_back  (start);
   grid->checkQuad   (start);
   make_normale (tstart, axe, larg);
   for (int nro=0 ; nro<nbre; ++nro) tlen.push_back ((nro+1)*larg);

   if (grid->isValid())
      {
      grid->extrudeQuads  (tstart, axe, tlen);
      }

   DumpReturn (grid);
   return grid;
}
// ========================================================= extrudeQuadUni
Elements* Document::extrudeQuadUni (Quad* start, Vector* axis, double len,
                                    int nbre)
{
   DumpStart ("extrudeQuadUni", start << axis << len << nbre);

   Quads      tstart;
   RealVector tlen;
   Real3      axe;
   Elements*  grid = new Elements (this);

   grid->checkQuad  (start);
   grid->checkAxis  (axis, axe);

   tstart.push_back (start);

   for (int nro=0 ; nro<nbre; ++nro) tlen.push_back ((nro+1)*len);

   if (grid->isValid())
      {
      grid->extrudeQuads  (tstart, axe, tlen);
      }

   DumpReturn (grid);
   return grid;
}
// ========================================================= extrudeQuad
Elements* Document::extrudeQuad (Quad* start, Vector* axis, RealVector tlen)
{
   DumpStart ("extrudeQuad", start << axis << tlen);

   Quads     tstart;
   Real3     axe;
   Elements* grid = new Elements (this);

   grid->checkQuad   (start);
   grid->checkAxis   (axis, axe);
   grid->checkVector (3, tlen);

   tstart.push_back (start);

   if (grid->isValid())
      {
      grid->extrudeQuads  (tstart, axe, tlen);
      }

   DumpReturn (grid);
   return grid;
}

// ========================================================= extrudeQuadsTop
Elements* Document::extrudeQuadsTop (Quads tstart, int nbre)
{
   DumpStart ("extrudeQuadsTop", tstart << nbre);

   RealVector tlen;
   Real3      axe  = { 0, 0, 1 };
   double     larg = 1;
   Elements* grid = new Elements (this);

   grid->checkQuads (tstart);
   make_normale     (tstart, axe, larg);
   for (int nro=0 ; nro<nbre; ++nro) tlen.push_back ((nro+1)*larg);

   if (grid->isValid())
      {
      grid->extrudeQuads  (tstart, axe, tlen);
      }

   DumpReturn (grid);
   return grid;
}
// ========================================================= extrudeQuadsUni
Elements* Document::extrudeQuadsUni (Quads tstart, Vector* axis, double len,
                                     int nbre)
{
   DumpStart ("extrudeQuadsUni", tstart << axis << len << nbre);

   RealVector tlen;
   Real3      axe;
   Elements*  grid = new Elements (this);

   grid->checkQuads (tstart);
   grid->checkAxis  (axis, axe);

   for (int nro=0 ; nro<nbre; ++nro) tlen.push_back ((nro+1)*len);

   if (grid->isValid())
      {
      grid->extrudeQuads  (tstart, axe, tlen);
      }

   DumpReturn (grid);
   return grid;
}
// ========================================================= extrudeQuads
Elements* Document::extrudeQuads (Quads tstart, Vector* axis, RealVector tlen)
{
   DumpStart ("extrudeQuads", tstart << axis << tlen);

   Real3     axe;
   Elements* grid = new Elements (this);

   grid->checkQuads  (tstart);
   grid->checkAxis   (axis, axe);
   grid->checkVector (3, tlen);

   if (grid->isValid())
      {
      grid->extrudeQuads  (tstart, axe, tlen);
      }

   DumpReturn (grid);
   return grid;
}
// ========================================================= revolutionQuadUni
Elements* Document::revolutionQuadUni (Quad* start, Vertex* center,
                                       Vector* axis, double angle, int nbre)
{
   DumpStart ("revolutionQuadUni", start << center << axis << angle << nbre);

   Quads      tstart;
   RealVector angles;
   Real3      axe;
   Elements* grid = new Elements (this);
   double   alpha = angle / std::max (nbre, 1); 

   grid->checkQuad (start);
   grid->checkOrig (2, center);
   grid->checkAxis (axis, axe);

   tstart.push_back (start);
   for (int nro=0 ; nro<nbre; ++nro) angles.push_back ((nro+1)*alpha);

   if (grid->isValid())
      {
      grid->extrudeQuads  (tstart, axe, angles, true, center);
      }

   DumpReturn (grid);
   return grid;
}
// ========================================================= revolutionQuad
Elements* Document::revolutionQuad (Quad* start, Vertex* center, Vector* axis,
                                    RealVector angles)
{
   DumpStart ("revolutionQuads", start << center << axis << angles);

   Quads      tstart;
   Real3      axe;
   Elements*  grid = new Elements (this);

   grid->checkQuad (start);
   grid->checkOrig (2, center);
   grid->checkAxis (axis, axe);

   tstart.push_back (start);

   if (grid->isValid())
      {
      grid->extrudeQuads  (tstart, axe, angles, true, center);
      }

   DumpReturn (grid);
   return grid;
}
// ========================================================= revolutionQuadsUni
Elements* Document::revolutionQuadsUni (Quads tstart, Vertex* center, 
                                        Vector* axis, double angle, int nbre)
{
   DumpStart ("revolutionQuadsUni", tstart << center << axis << angle << nbre);

   RealVector angles;
   Real3      axe;
   Elements* grid  = new Elements (this);
   double    alpha = angle / std::max (nbre, 1); 

   grid->checkQuads (tstart);
   grid->checkOrig  (2, center);
   grid->checkAxis  (axis, axe);

   for (int nro=0 ; nro<nbre; ++nro) angles.push_back ((nro+1)*alpha);

   if (grid->isValid())
      {
      grid->extrudeQuads  (tstart, axe, angles, true, center);
      }

   DumpReturn (grid);
   return grid;
}
// ========================================================= revolutionQuads
Elements* Document::revolutionQuads (Quads tstart, Vertex* center, Vector* axis,
                                     RealVector angles)
{
   DumpStart ("revolutionQuads", tstart << center << axis << angles);

   Real3     axe;
   Elements* grid = new Elements (this);

   grid->checkQuads  (tstart);
   grid->checkOrig   (2, center);
   grid->checkAxis   (axis,   axe);
   grid->checkVector (4, angles);

   if (grid->isValid())
      {
      grid->extrudeQuads  (tstart, axe, angles, true, center);
      }

   DumpReturn (grid);
   return grid;
}
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ========================================================= joinQuadUni
Elements* Document::joinQuadUni (Quad* quada, Quad* quadb, Vertex* va1,
                              Vertex* vb1, Vertex* va2, Vertex* vb2, int nbre)
{
   DumpStart ("joinQuadUni", quada << quadb << va1<<vb1 << va2<<vb2 << nbre);

   Quads      tquada;
   RealVector tlen;
   Elements*  grid = new Elements (this);

   update ();
   grid->checkQuad  (quada);
   grid->checkQuad  (quadb, -1);
   grid->checkSense (3, va1, va2, quada);
   grid->checkSense (6, vb1, vb2, quadb);

   nbre = nbre < 0 ? 0 : nbre;
   double dist = 1.0 / (nbre+1);

   tquada.push_back (quada);
   for (int nro=0 ; nro<nbre; ++nro) tlen.push_back ((nro+1)*dist);

   if (grid->isValid())
      {
      grid->joinQuads  (tquada, quadb, va1, vb1, va2, vb2, tlen);
      }

   DumpReturn (grid);
   return grid;
}
// ========================================================= joinQuad
Elements* Document::joinQuad (Quad* quada, Quad* quadb, Vertex* va1,
                    Vertex* vb1, Vertex* va2, Vertex* vb2, RealVector& tlen)
{
   DumpStart ("joinQuad", quada << quadb << va1 << vb1 << va2 << vb2 << tlen);

   Quads      tquada;
   Elements*  grid = new Elements (this);

   update ();
   grid->checkQuad   (quada);
   grid->checkQuad   (quadb, -1);
   grid->checkSense  (3, va1, va2, quada);
   grid->checkSense  (6, vb1, vb2, quadb);
   grid->checkVector (7, tlen, 0, true);

   tquada.push_back (quada);

   if (grid->isValid())
      {
      grid->joinQuads  (tquada, quadb, va1, vb1, va2, vb2, tlen);
      }

   DumpReturn (grid);
   return grid;
}
// ========================================================= joinQuadsUni
Elements* Document::joinQuadsUni (Quads tquada, Quad* quadb, Vertex* va1,
                           Vertex* vb1, Vertex* va2, Vertex* vb2, int nbre)
{
   DumpStart ("joinQuadsUni", tquada << quadb << va1 << vb1 << va2 << vb2 
                                     << nbre);
   RealVector tlen;
   Elements* grid = new Elements (this);

   update ();
   grid->checkQuads (tquada);
   grid->checkQuad  (quadb, -1);
   if (tquada.size()>0) 
       grid->checkSense (3, va1, va2, tquada[0]);
   grid->checkSense (6, vb1, vb2, quadb);

   nbre = nbre < 0 ? 0 : nbre;
   double dist = 1.0 / (nbre+1);

   for (int nro=0 ; nro<nbre; ++nro) tlen.push_back ((nro+1)*dist);

   if (grid->isValid())
      {
      grid->joinQuads  (tquada, quadb, va1, vb1, va2, vb2, tlen);
      }

   DumpReturn (grid);
   return grid;
}
// ========================================================= joinQuads
Elements* Document::joinQuads (Quads tquada, Quad* quadb, Vertex* va1, 
                      Vertex* vb1, Vertex* va2, Vertex* vb2, RealVector& tlen)
{
   DumpStart ("joinQuads", tquada << quadb << va1 << vb1 << va2 << vb2 << tlen);

   Elements* grid = new Elements (this);

   update ();
   grid->checkQuads (tquada);
   grid->checkQuad  (quadb, -1);
   if (tquada.size()>0) 
       grid->checkSense (3, va1, va2, tquada[0]);
   grid->checkSense  (6, vb1, vb2, quadb);
   grid->checkVector (7, tlen, 1, true);

   if (grid->isValid())
      {
      grid->joinQuads  (tquada, quadb, va1, vb1, va2, vb2, tlen);
      }

   DumpReturn (grid);
   return grid;
}
// ========================================================= cutUni
Elements* Document::cutUni (Edge* edge, int nbre)
{
   DumpStart ("cutUni", edge << nbre);

   Elements*  grid = new Elements (this);

   if (BadElement (edge))
      {
      grid->setError (HERR);
      Mess << "Argument number 1 : edge is not valid";
      }

   if (nbre <= 0) 
      {
      grid->setError (HERR);
      Mess << "Argument number 2 : number od subdivisions must be positive";
      nbre = 0;
      }

   double dist = 1.0 / (nbre+1);

   RealVector tlen;
   for (int nro=0 ; nro<nbre; ++nro) tlen.push_back ((nro+1)*dist);

   if (grid->isValid())
      {
      grid->cutEdge  (edge, tlen);
      }

   DumpReturn (grid);
   return grid;
}
// ========================================================= cut
Elements* Document::cut (Edge* edge, RealVector& tlen)
{
   DumpStart ("cut", edge << tlen);

   Elements* grid = new Elements (this);

   if (BadElement (edge))
      {
      grid->setError (HERR);
      Mess << "Argument number 1 : edge is not valid";
      }

   grid->checkVector (2, tlen, 1, true);

   if (grid->isValid())
      {
      grid->cutEdge  (edge, tlen);
      }

   DumpReturn (grid);
   return grid;
}
END_NAMESPACE_HEXA
