
// C++ : Gestion des aretes

#include "HexVector.hxx"

BEGIN_NAMESPACE_HEXA

// ======================================================== Constructeur
Vector::Vector (Document* doc, double dx, double dy, double dz)
      : EltBase (doc)
{
    v_dx = dx;
    v_dy = dy;
    v_dz = dz;
}
// ======================================================== Constructeur bis
Vector::Vector (Vector* lautre)
      : EltBase (lautre->el_root)
{
    v_dx = lautre -> v_dx;
    v_dy = lautre -> v_dy;
    v_dz = lautre -> v_dz;
}

// ======================================================== calculNormale
// === Calcule un vecteur orthogonal et norme
int Vector::calculNormale (double& nx, double& ny, double& nz)
{
    double epsil = (v_dx * v_dx + v_dy * v_dy + v_dz * v_dz)*1e-6;
    int    forme = 2000;
    if  (v_dx > epsil || v_dx < -epsil) forme += 100;
    if  (v_dy > epsil || v_dy < -epsil) forme +=  10;
    if  (v_dz > epsil || v_dz < -epsil) forme +=   1;

    nx = ny = nz = 0;
    switch (forme)
       {
                             // dx=dy=dz=0 : retour vecteur i + erreur
       case 2000 : nx = 1; 
                   return HERR;
                             // dx = 0 : on prend le vecteur i
       case 2001 :  
       case 2010 :  
       case 2011 : nx = 1;
                   return HOK;  
                             // dy = 0 : on prend le vecteur j
       case 2100 :  
       case 2101 : ny = 1; 
                   return HOK;  
                             // dz = 0 : on prend le vecteur k
       case 2110 : nz = 1;
                   return HOK;  
                             // dx, dy, dy non nuls : cas general;
       default :;
       }

   double norme = sqrt (v_dx*v_dx + v_dy*v_dy);
   nx =  v_dy / norme;
   ny = -v_dx / norme;
   return HOK;  
}

END_NAMESPACE_HEXA
