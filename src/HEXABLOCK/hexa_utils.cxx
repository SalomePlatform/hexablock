
// C++ : Fonctions utilitaires de Hexa

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

#include "hexa_base.hxx"

#include <cstdlib>
#include <cmath>
#include <ctime>
#include <sys/stat.h>

BEGIN_NAMESPACE_HEXA


// ========================================================= niv_debug
int niv_debug ()
{
   static int debug_level = NOTHING;
   if (debug_level == NOTHING)
      {
      cpchar rep = getenv ("HEXA_DB");
      if (rep!=NULL)
          debug_level = atoi (rep);
      debug_level = std::max (debug_level, 0);
      }
   return debug_level;
}
// ========================================================= on_debug
bool on_debug ()
{
   return niv_debug () > 0;
}
// ========================================================= in_test
bool in_test ()
{
   static int test_level = NOTHING;
   if (test_level == NOTHING)
      {
      cpchar rep = getenv ("HEXA_TEST");
      if (rep!=NULL)
          test_level = atoi (rep);
      test_level = std::max (test_level, 0);
      }
   return test_level > 0;

}
// ======================================================== set_minus
void set_minus (string& chaine)
{
   int lg = chaine.size();
   for (int nc=0 ; nc<lg ; nc++)
       chaine [lg] = tolower (chaine[lg]);
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
// ======================================================== calc_d2
double calc_d2  (double v1[], double v2[])
{
   double dd = carre (v2[dir_x]-v1[dir_x]) + carre (v2[dir_y]-v1[dir_y]) 
             + carre (v2[dir_z]-v1[dir_z]);
   return dd;
}
// ========================================================= calc_vecteur
void calc_vecteur  (double pta[], double ptb[], double vab[])
{
   vab [dir_x] = ptb [dir_x] - pta [dir_x];
   vab [dir_y] = ptb [dir_y] - pta [dir_y];
   vab [dir_z] = ptb [dir_z] - pta [dir_z];
}
// ========================================================= copy_vecteur
void copy_vecteur  (double va[], double vb[])
{
   vb [dir_x] = va [dir_x];
   vb [dir_y] = va [dir_y];
   vb [dir_z] = va [dir_z];
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
bool same_coords (double* pa, double* pb, double epsilon2)
{
   double d2 = carre (pb[dir_x]-pa[dir_x]) + carre (pb[dir_y]-pa[dir_y])
             + carre (pb[dir_z]-pa[dir_z]);
   return d2 < epsilon2;
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
static int current_option = NOTHING;
// ====================================================== special_option
bool special_option ()
{
   if (current_option == NOTHING)
      {
      cpchar rep = getenv ("HEXA_OPTION");
      if (rep!=NULL)
          current_option = atoi (rep);
      current_option = std::max (current_option, 0);
      }
   return current_option > 0;
}
// ====================================================== set_special_option
void set_special_option (bool opt)
{
   current_option = opt ? 1 : 0;
}
// ====================================================== sizeof_file
int sizeof_file (cpchar filename)
{
   struct stat status;

   int ier = stat  (filename, &status);
   if (ier == HOK)
      return status.st_size;
   else
      return NOTHING;
}
// ====================================================== read_file
char* read_file (cpchar filename, int& size)
{
   size = 0;

   struct stat status;
   int ier = stat  (filename, &status);
   if (ier != HOK)
      return  NULL;

   FILE* fic = fopen (filename, "r");
   if (fic == NULL)
      return  NULL;

   int   lgalloc = status.st_size;
   char* buffer  = (char*) malloc (lgalloc+1);
   if (buffer == NULL)
      return  NULL;

   for (int car = fgetc (fic) ; car!=EOF && size<lgalloc ; car = fgetc(fic))
       buffer [size++] = car;
   fclose (fic);

   printf ("read_file : lgalloc=%d, size=%d\n", lgalloc, size);
   buffer [size++] = EOS;
   return buffer;
}
// ====================================================== get_time
cpchar get_time (string& buffer)
{
   char   quand[24];
   time_t tps;
   time (&tps);
   struct tm *temps = localtime (&tps);

   sprintf (quand, "%d/%02d/%02d %02d:%02d:%02d",
            1900 + temps->tm_year, temps->tm_mon+1, temps->tm_mday,
            temps->tm_hour, temps->tm_min, temps->tm_sec);

    buffer = quand;
    return buffer.c_str();
}
// ======================================================== requals
bool requals (const double v1, const double v2)
{
   static const double Epsilon = 1e-6;
   return v1 >= v2 - Epsilon && v1 <= v2 + Epsilon;
}
// ======================================================== requals
bool requals (const double* lun, const double* lautre)
{
   return    lun!=NULL && lautre!=NULL     && requals (lun [0], lautre [0])
          && requals (lun [1], lautre [1]) && requals (lun [2], lautre [2]) ;
}
// ======================================================== make_basename
int make_basename (cpchar filename, string& base)
{
   const char slash = '/';
   const char antis = '\\';
   int  lg   = strlen (filename);
   int  ifin = -1;
   int  ideb = 0;
   bool more = true;

   for (int nc = lg-1 ; more && nc>=0 ; --nc)
       {
       char car = filename[nc];
       if (car==slash || car==antis)
          {
          if (ifin >0) 
             {
             ideb = nc + 1;
             more = false;
             }
          }
       else if (ifin<0)
          ifin = nc;
       } 

   if (ifin  <0)
      {
      base = "undefined";
      return HERR;
      }

   base = "";
   for (int nc=ideb ; nc <= ifin ; ++nc)
       base += filename[nc];
   return HOK;
}
END_NAMESPACE_HEXA
