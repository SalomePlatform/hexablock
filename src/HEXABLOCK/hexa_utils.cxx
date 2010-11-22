
// C++ : Fonctions utilitaires de Hexa

#include "hexa_base.hxx"

#include <sys/stat.h>

BEGIN_NAMESPACE_HEXA

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

END_NAMESPACE_HEXA
