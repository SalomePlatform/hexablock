
// class : Resolution d'un systeme de Cramer

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
#ifndef __CRAMER_H_
#define __CRAMER_H_

#include "hexa_base.hxx"
// #include <cmath>

BEGIN_NAMESPACE_HEXA

class HexaExport Cramer 
{
public:
    Cramer (int size);
   ~Cramer ();

    void defMatrix (double matuser[]);
    int  resoudre  (double matr[], double second[],  double sol[]);
    void multiply  (double vect[], double result[]);

private:
    int  resoudre  (double msecond[], double solution[]);
    double determinant ();
    void   defCofacteur (Cramer* orig, int lig, int col);
    bool   cestNul (double x) { return x > -1e-10 &&  x < 1e-10 ; }
private:
    const int mat_size;
    double** matrix;
    double*  col_saved;
    Cramer*  co_facteur;
};
// ========================================================= Constructeur
inline Cramer::Cramer (int size)
             : mat_size(size)
{
   matrix     = new double* [mat_size];
   col_saved  = new double  [mat_size];
   co_facteur = NULL;
   if (mat_size > 2)
      co_facteur =  new Cramer (mat_size-1);

   for (int nl=0 ; nl<mat_size ; nl++)
       {
       matrix[nl] = new double [mat_size];
       for (int nc=0 ; nc<mat_size ; nc++)
           matrix [nl][nc] = 0;
       }
}
// ========================================================= Destructeur
inline Cramer::~Cramer ()
{
   for (int nl=0 ; nl<mat_size ; nl++)
       delete [] matrix[nl];

   delete [] matrix;
   delete [] col_saved;
   delete    co_facteur;
}
// ========================================================= defMatrix
inline void Cramer::defMatrix (double matuser[])
{
   for (int nl=0 ; nl<mat_size ; nl++)
       for (int nc=0 ; nc<mat_size ; nc++)
           matrix [nl][nc] = matuser [nl*mat_size + nc];
}
// ========================================================= determinant
inline double Cramer::determinant ()
{
   double det = 0;
   if (mat_size <= 1)
      return matrix[0][0];

   else if (mat_size == 2)
      {
      det =matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
      return det;
      }

   for (int ni=0 ; ni<mat_size ; ni++)
       {
       if (NOT cestNul (matrix[ni][0]))
          {
          co_facteur->defCofacteur (this, ni, 0);
          double signe = (ni MODULO 2) ? -1.0 : 1.0;
          det += signe*matrix[ni][0] * co_facteur->determinant ();
          }
       }
   return det;
}
// ========================================================= defCofacteur
inline void Cramer::defCofacteur (Cramer* orig, int lig, int col)
{
   for (int nl=0 ; nl<mat_size ; nl++)
       {
       int ni = nl < lig ? nl : nl+1;
       for (int nc=0 ; nc<mat_size ; nc++)
           {
           int nj = nc < col ? nc : nc+1;
           matrix[nl][nc]  = orig->matrix[ni][nj];
           }
       }
}
// ========================================================= resoudre
inline int Cramer::resoudre (double mat[], double second[],  double sol[])
{
   defMatrix (mat);
   int ier = resoudre (second, sol);
   return ier;
}
// ========================================================= multiply
inline void Cramer::multiply (double facteur[],  double produit[])
{
   for (int ni=0 ; ni<mat_size ; ni++)
       {
       produit [ni] = 0;
       for (int nj=0 ; nj<mat_size ; nj++)
           produit [ni] += matrix  [ni][nj] * facteur [nj]; 
       }
}
// ========================================================= resoudre
inline int Cramer::resoudre (double msecond[],  double solution[])
{
   double det = determinant();
   if (cestNul (det))
      return HERR;
   
   for (int nj=0 ; nj<mat_size ; nj++)
       {
       for (int ni=0 ; ni<mat_size ; ni++)
           {
           col_saved [ni]  = matrix  [ni][nj];
           matrix [ni][nj] = msecond [ni];
           }
       solution [nj] = determinant() / det;

       for (int ni=0 ; ni<mat_size ; ni++)
           matrix [ni][nj] = col_saved [ni];
       }
   return HOK;
}
END_NAMESPACE_HEXA
#endif
