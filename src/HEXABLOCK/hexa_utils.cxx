
// C++ : Fonctions utilitaires de Hexa

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
//  See http://www.salome-platform.org/ or email 
//  webmaster.salome@opencascade.com

#include "hexa_base.hxx"

#include <cstdlib>
#include <cmath>
#include <sys/stat.h>

BEGIN_NAMESPACE_HEXA

static int debug_level = NOTHING;

// ========================================================= prod_scalaire
bool on_debug ()
{
   if (debug_level == NOTHING)
      {
      cpchar rep = getenv ("HEXA_DB");
      if (rep!=NULL)
          debug_level = atoi (rep);
      debug_level = std::max (debug_level, 0); 
      }
   return debug_level > 0;
    
}
// ======================================================== get_temp_name
pchar get_temp_name (cpchar format, pchar nomfic)
{
   int nro = 0;
   while (true)
         {
         nro ++;
         sprintf (nomfic, format, nro);
         
         struct stat buffer;
         int rep = stat (nomfic, &buffer);
         if (rep<0) 
            return nomfic;
         }
}
// ========================================================= prod_scalaire
double prod_scalaire (double v1[], double v2[])
{
   double prod = v1[dir_x]*v2[dir_x] + v1[dir_y]*v2[dir_y] 
                                     + v1[dir_z]*v2[dir_z];
   return prod;
}
// ========================================================= calc_norme
double calc_norme (double vect[])
{
    double norme = vect[dir_x]*vect[dir_x] + vect[dir_y]*vect[dir_y] 
	                                   + vect[dir_z]*vect[dir_z];
    return sqrt (norme);
}
// ======================================================== calc_distance
double calc_distance  (double v1[], double v2[])
{
   Real3 vv = { v2[dir_x]-v1[dir_x], v2[dir_y]-v1[dir_y], v2[dir_z]-v1[dir_z] };
   return calc_norme (vv);
}
// ========================================================= calc_vecteur
void calc_vecteur  (double pta[], double ptb[], double vab[])
{
   vab [dir_x] = ptb [dir_x] - pta [dir_x];
   vab [dir_y] = ptb [dir_y] - pta [dir_y];
   vab [dir_z] = ptb [dir_z] - pta [dir_z];
}
// ========================================================= calc_milieu
void calc_milieu  (double pta[], double ptb[], double milieu[])
{
   milieu [dir_x] = (ptb [dir_x] + pta [dir_x])/2.0;
   milieu [dir_y] = (ptb [dir_y] + pta [dir_y])/2.0;
   milieu [dir_z] = (ptb [dir_z] + pta [dir_z])/2.0;
}
// ========================================================= normer_vecteur
int normer_vecteur (double vect[])
{
   double dn = calc_norme (vect);
   if (dn < 1e-30) 
      return HERR;

   vect [dir_x] /= dn;
   vect [dir_y] /= dn;
   vect [dir_z] /= dn;
   return HOK;
}
// ========================================================= prod_vectoriel
double* prod_vectoriel (double v1[], double v2[], double prod[])
{
   prod [dir_x] = v1[dir_y] * v2[dir_z] - v2[dir_y] * v1[dir_z];
   prod [dir_y] = v1[dir_z] * v2[dir_x] - v2[dir_z] * v1[dir_x];
   prod [dir_z] = v1[dir_x] * v2[dir_y] - v2[dir_x] * v1[dir_y];

   return prod;
}
// ====================================================== carre
double carre (double val)
{
   return val*val;
}
// ====================================================== same_coords
bool same_coords (double* pa, double* pb, double epsilon)
{

   double d2 = carre (pb[dir_x]-pa[dir_x]) + carre (pb[dir_y]-pa[dir_y]) 
             + carre (pb[dir_z]-pa[dir_z]); 
   return d2 < epsilon;
}
// ========================================================= prod_mixte
double prod_mixte (double vi[], double vj[], double vk[])
{
   double pmixte = 0;

   for (int nc=0 ; nc<DIM3 ; nc++) 
       {
       int nc1 = (nc + 1) MODULO DIM3;
       int nc2 = (nc + 2) MODULO DIM3;
       pmixte +=  vk[nc] * (vi [nc1] * vj [nc2] - vj [nc1] * vi [nc2]);
       }

   return pmixte;
}
// ========================================================= fatal_error
void fatal_error (cpchar format, cpchar info1, cpchar info2)
{
   char     buffer [240];
   sprintf (buffer, format, info1, info2);
   printf ("****\n");
   printf ("**** %s\n", buffer);
   printf ("****\n");
#ifdef NO_CASCADE
   printf ("                ............ Erreur fatale\n");
   exit (94);
#endif
   printf ("\n");
}
END_NAMESPACE_HEXA
