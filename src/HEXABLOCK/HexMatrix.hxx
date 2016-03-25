
// class : Les matrices

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

#ifndef __MATRIX_H
#define __MATRIX_H

#include "HexVertex.hxx"
#include "HexVector.hxx"

#include <cmath>

BEGIN_NAMESPACE_HEXA

class HexaExport Matrix
{
public:
    Matrix ();
    int defTranslation   (Vector* depl);
    int defTranslation   (double* depl);
    int defScale         (Vertex* center, double scale);
    int defScale         (double* center, double scale);
    int defRotation      (Vertex* center, Vector* axe, double degres);
    int defRotation      (Vertex* center, double* axe, double degres);
    int defSymmetryPoint (Vertex* center);
    int defSymmetryLine  (Vertex* center, Vector* dir);
    int defSymmetryPlane (Vertex* center, Vector* normale);

    void define (double* orig, double* iprim, double* jprim, double* kprim);

    void perform (Vertex* noeud);
    void perform (double* point);
    void perform (double* point, double* result);

    void getCoeff (double& a11, double& a12, double& a13, double& a14,
                   double& a21, double& a22, double& a23, double& a24,
                   double& a31, double& a32, double& a33, double& a34);
private:
    void erase();
private:
    double mat11, mat12, mat13, mat14;
    double mat21, mat22, mat23, mat24;
    double mat31, mat32, mat33, mat34;
};
// ------------------------------------------- Inlining
// ========================================================= Constructeur
inline Matrix::Matrix ()
{
   erase ();
}
// ========================================================= erase
inline void Matrix::erase ()
{
   mat12 = mat13 = mat14 = 0.0;
   mat21 = mat23 = mat24 = 0.0;
   mat31 = mat32 = mat34 = 0.0;
   mat11 = mat22 = mat33 = 1.0;
}
// ========================================================= perform
inline void Matrix::perform (Vertex* noeud)
{
   double px, py, pz;
   px = mat11*noeud->getX()+mat12*noeud->getY()+mat13*noeud->getZ()+mat14;
   py = mat21*noeud->getX()+mat22*noeud->getY()+mat23*noeud->getZ()+mat24;
   pz = mat31*noeud->getX()+mat32*noeud->getY()+mat33*noeud->getZ()+mat34;

   noeud->setCoord (px, py, pz);
}
// ========================================================= defTranslation
inline int Matrix::defTranslation (Vector* boulevard)
{
   erase();
   mat11 = mat22 = mat33 = 1.0;

   mat14 = boulevard->getDx ();
   mat24 = boulevard->getDy ();
   mat34 = boulevard->getDz ();

   return HOK;
}
// ========================================================= defTranslation (2)
inline int Matrix::defTranslation (double* decal)
{
   erase();
   mat11 = mat22 = mat33 = 1.0;

   mat14 = decal [dir_x];
   mat24 = decal [dir_y];
   mat34 = decal [dir_z];
   return HOK;
}
// ========================================================= defScale
inline int Matrix::defScale (double* center, double scale)
{
   erase();
   mat11 = mat22 = mat33 = scale;

   mat14 = (1-scale) * center[dir_x];
   mat24 = (1-scale) * center[dir_y];
   mat34 = (1-scale) * center[dir_z];

   return HOK;
}
// ========================================================= defScale
inline int Matrix::defScale (Vertex* center, double scale)
{
   if (center==NULL)
      {
      erase();
      return HERR;
      }

   Real3 coord;
   int ier = defScale (center->getPoint (coord ), scale);
   return ier;
}
// ========================================================= defRotation
inline int Matrix::defRotation (Vertex* center, double* axe, double degres)
{
   erase();
   if (BadElement (center))
      return HERR;

   double norme = calc_norme (axe);
   if (norme< 1e-30)
      return HERR;

   double ux = axe [dir_x] / norme;
   double uy = axe [dir_y] / norme;
   double uz = axe [dir_z] / norme;

   double cx = center->getX ();
   double cy = center->getY ();
   double cz = center->getZ ();

   double cost = cos (degres*M_PI/180);
   double sint = sin (degres*M_PI/180);

   mat11 = ux*ux * (1-cost) + cost;
   mat12 = ux*uy * (1-cost) - uz*sint;
   mat13 = ux*uz * (1-cost) + uy*sint;

   mat21 = ux*uy * (1-cost) + uz*sint;
   mat22 = uy*uy * (1-cost) + cost;
   mat23 = uy*uz * (1-cost) - ux*sint;

   mat31 = ux*uz * (1-cost) - uy*sint;
   mat32 = uy*uz * (1-cost) + ux*sint;
   mat33 = uz*uz * (1-cost) + cost;

   mat14 = cx - mat11*cx - mat12*cy - mat13*cz;
   mat24 = cy - mat21*cx - mat22*cy - mat23*cz;
   mat34 = cz - mat31*cx - mat32*cy - mat33*cz;

   return HOK;
}
// ========================================================= defRotation
inline int Matrix::defRotation (Vertex* center, Vector* dir, double degres)
{
   if (BadElement (dir))
      return HERR;

   Real3 axe;
   dir->getCoord (axe);
   int ier = defRotation (center, axe, degres);
   return ier;
}
// ========================================================= defSymmetryPoint
inline int Matrix::defSymmetryPoint (Vertex* center)
{
   erase();

   mat11 = mat22 = mat33 = -1;

   mat14 = 2 * center->getX();
   mat24 = 2 * center->getY();
   mat34 = 2 * center->getZ();

   return HOK;
}
// ========================================================= defSymmetryLine
//     MH.d = 0        (1)
//     CH  = lambda*d  (2)
//     MM' = 2MH       (3)
//
// (1) et (2) => lambda = ((x-xc)*xd + (y-yc)*yd + (z-zc)*zd) / norme(d)
//
//     MM' = 2MH (3)
// <=> MO + OM' =  2 (MO + OC + CH)
// <=>      OM' =  MO + 2.OC + 2.CH
// <=>      OM' = -OM + 2.OC + 2.lambda.d   (2) et (3)
//
//           x' = -x  + 2*xc + 2*xd*((x-xc)*xd + (y-yc)*yd + (z-zc)*zd)
//           y' = -y  + 2*yc + 2*yd*((x-xc)*xd + (y-yc)*yd + (z-zc)*zd)
//           z' = -z  + 2*zc + 2*zd*((x-xc)*xd + (y-yc)*yd + (z-zc)*zd)
//
inline int Matrix::defSymmetryLine (Vertex* center, Vector* dir)
{
   erase ();

   double normed = dir->getNorm ();
   if (normed< 1e-30)
      return HERR;

   double xc =  center->getX();
   double yc =  center->getY();
   double zc =  center->getZ();

   double xd = dir->getDx() / normed;
   double yd = dir->getDy() / normed;
   double zd = dir->getDz() / normed;

   mat11 = 2*xd*xd -1;
   mat12 = 2*xd*yd;
   mat13 = 2*xd*zd;
   mat14 = 2*(xc - xd*(xc*xd + yc*yd + zc*zd));

   mat21 = 2*yd*xd;
   mat22 = 2*yd*yd - 1;
   mat23 = 2*yd*zd;
   mat24 = 2*(yc - yd*(xc*xd + yc*yd + zc*zd));

   mat31 = 2*zd*xd;
   mat32 = 2*zd*yd;
   mat33 = 2*zd*zd - 1;
   mat34 = 2*(zc - zd*(xc*xd + yc*yd + zc*zd));

   return HOK;
}
// ========================================================= defSymmetryPlane
//     CH.n = 0         (1)
//     MH   = lambda*n  (2)
//     MM'  = 2MH       (3)
//
// (1) et (2) => lambda = ((x-xc)*xn + (y-yc)*yn + (z-zc)*zn) / norme(n)
//
//     MM' = 2MH (3)
// <=> MO + OM' =  2.lambda.n
// <=>      OM' =  OM + 2.lambda.n
//
//           x' = x + 2*lambda*xn
//           y' = y + 2*lambda*yn
//           z' = z + 2*lambda*zn
//
//           x' = x + 2*xn*((x-xc)*xn + (y-yc)*yn + (z-zc)*zn)
//           y' = y + 2*yn*((x-xc)*xn + (y-yc)*yn + (z-zc)*zn)
//           z' = z + 2*zn*((x-xc)*xn + (y-yc)*yn + (z-zc)*zn)
//
inline int Matrix::defSymmetryPlane (Vertex* center, Vector* normale)
{
   erase ();

   double normed = normale->getNorm ();
   if (normed< 1e-30)
      return HERR;

   double xc =  center->getX();
   double yc =  center->getY();
   double zc =  center->getZ();

   double xn = normale->getDx() / normed;
   double yn = normale->getDy() / normed;
   double zn = normale->getDz() / normed;

   mat11 = -2*xn*xn + 1;
   mat12 = -2*xn*yn;
   mat13 = -2*xn*zn;
   mat14 =  2*xn*(xc*xn + yc*yn + zc*zn);

   mat21 = -2*yn*xn;
   mat22 = -2*yn*yn + 1;
   mat23 = -2*yn*zn;
   mat24 =  2*yn*(xc*xn + yc*yn + zc*zn);

   mat31 = -2*zn*xn;
   mat32 = -2*zn*yn;
   mat33 = -2*zn*zn + 1;
   mat34 =  2*zn*(xc*xn + yc*yn + zc*zn);

   return HOK;
}
// ========================================================= define
inline void Matrix::define (double* omega, double* iprim, double* jprim, 
                                                          double* kprim)
{
   mat11 = iprim [dir_x];
   mat21 = iprim [dir_y];
   mat31 = iprim [dir_z];

   mat12 = jprim [dir_x];
   mat22 = jprim [dir_y];
   mat32 = jprim [dir_z];

   mat13 = kprim [dir_x];
   mat23 = kprim [dir_y];
   mat33 = kprim [dir_z];

   mat14 = omega [dir_x];
   mat24 = omega [dir_y];
   mat34 = omega [dir_z];
}
// ========================================================= getCoeff
inline void Matrix::getCoeff(double& a11, double& a12, double& a13, double& a14,
                             double& a21, double& a22, double& a23, double& a24,
                             double& a31, double& a32, double& a33, double& a34)
{
   a11 = mat11;
   a12 = mat12;
   a13 = mat13;
   a14 = mat14;

   a21 = mat21;
   a22 = mat22;
   a23 = mat23;
   a24 = mat24;

   a31 = mat31;
   a32 = mat32;
   a33 = mat33;
   a34 = mat34;
}
// ========================================================= perform
inline void Matrix::perform (double* point)
{
   double px = point [dir_x];
   double py = point [dir_y];
   double pz = point [dir_z];

   point [dir_x] = mat11*px + mat12*py + mat13*pz + mat14;
   point [dir_y] = mat21*px + mat22*py + mat23*pz + mat24;
   point [dir_z] = mat31*px + mat32*py + mat33*pz + mat34;
}
// ========================================================= perform
inline void Matrix::perform (double* point, double* result)
{
   double px = point [dir_x];
   double py = point [dir_y];
   double pz = point [dir_z];

   result [dir_x] = mat11*px + mat12*py + mat13*pz + mat14;
   result [dir_y] = mat21*px + mat22*py + mat23*pz + mat24;
   result [dir_z] = mat31*px + mat32*py + mat33*pz + mat34;
}
END_NAMESPACE_HEXA
#endif
