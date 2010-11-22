
// C++ : Gestion des noeuds

#include "HexShape.hxx"

BEGIN_NAMESPACE_HEXA

// int Shape::nbr_shapes = 0;

// ====================================================== Constructeur
Shape::Shape  (string brep)
{
   b_rep = brep;
   debut = 0;
   fin   = 1;

   // nbr_shapes++;
   // nro_shape = nbr_shapes;
}

// ========================================================= getName 
/***************************************************
char* Shape::getName  (char* buffer)
{
   sprintf (buffer, "Shape0x%04d", nro_shape);
   return   buffer;
}
  *************************************************** */
END_NAMESPACE_HEXA
