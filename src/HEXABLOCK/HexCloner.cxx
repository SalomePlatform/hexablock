
// C++ : Gestion des noeuds

#include "HexCloner.hxx"
#include "HexMatrix.hxx"

#include "HexHexa.hxx"
#include "HexQuad.hxx"
#include "HexEdge.hxx"
#include "HexVertex.hxx"

#include "HexElements.hxx"

BEGIN_NAMESPACE_HEXA

// ============================================================= Constructeur
Cloner::Cloner  (Matrix* mat)
{
   matrice = mat;
}
// ============================================================== clonerVertex
Vertex* Cloner::clonerVertex (Vertex* orig)
{
    if (orig == NULL)
        return orig;

    Vertex* copie = clone_vertex [orig];
    if (copie != NULL)
        return copie;

    copie = new Vertex (orig);
    matrice -> perform (copie);
    clone_vertex [orig] = copie;
    return copie;
}
// ============================================================== clonerEdge
Edge* Cloner::clonerEdge (Edge* orig)
{
    if (orig == NULL)
        return orig;

   Edge* copie = clone_edge [orig];
   if (copie != NULL)
       return copie;

   copie = new Edge (orig);

   copie->e_vertex [V_AMONT] = clonerVertex (orig->e_vertex [V_AMONT]);
   copie->e_vertex [V_AVAL]  = clonerVertex (orig->e_vertex [V_AVAL]);

   copie->majReferences ();
   clone_edge [orig] = copie;

   if (orig->debug())
      {
      copie->printName (" est la copie de ");
      orig ->printName ("\n");
      }

   return copie;
}
// ============================================================== clonerQuad
Quad* Cloner::clonerQuad (Quad* orig)
{
   if (orig == NULL)
       return orig;

   Quad* copie = clone_quad [orig];
   if (copie != NULL)
       return copie;

   copie = new Quad (orig);

   for (int nro=0 ; nro<QUAD4 ; nro++) 
       copie->q_edge [nro] = clonerEdge (orig->q_edge [nro]);

   for (int nro=0 ; nro<QUAD4 ; nro++) 
       copie->q_vertex [nro] = clonerVertex (orig->q_vertex [nro]);

   copie->majReferences ();
   clone_quad [orig] = copie;
   return copie;
}
// ============================================================== clonerHexa
Hexa* Cloner::clonerHexa (Hexa* orig)
{
   if (orig == NULL)
       return orig;

   Hexa* copie = clone_hexa [orig];
   if (copie != NULL)
       return copie;

   copie = new Hexa (orig);
   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       copie->h_quad [nro] = clonerQuad (orig->h_quad [nro]);

   for (int nro=0 ; nro<HE_MAXI ; nro++) 
       copie->h_edge [nro] = clonerEdge (orig->h_edge [nro]);

   for (int nro=0 ; nro<HV_MAXI ; nro++) 
       copie->h_vertex [nro] = clonerVertex (orig->h_vertex [nro]);

   copie->majReferences ();
   clone_hexa [orig] = copie;
   return copie;
}
// ============================================================== clonerElements
Elements* Cloner::clonerElements (Elements* orig)
{
   Elements* copie = new Elements (orig);

   int nombre = orig->countHexa ();
   for (int nro=0 ; nro<nombre ; nro++)
       {
       Hexa* elt  = orig->getHexa (nro);
       Hexa* elt2 = clonerHexa (elt);
       copie->setHexa (elt2, nro);
       }

   nombre = orig->countQuad ();
   for (int nro=0 ; nro<nombre ; nro++)
       {
       Quad* elt  = orig->getQuad (nro);
       Quad* elt2 = clonerQuad (elt);
       copie->setQuad (elt2, nro);
       }

   nombre = orig->countEdge ();
   for (int nro=0 ; nro<nombre ; nro++)
       {
       Edge* elt  = orig->getEdge (nro);
       Edge* elt2 = clonerEdge (elt);
       copie->setEdge (elt2, nro);
       }

   nombre = orig->countVertex ();
   for (int nro=0 ; nro<nombre ; nro++)
       {
       Vertex* elt  = orig->getVertex (nro);
       Vertex* elt2 = clonerVertex (elt);
       copie->setVertex (elt2, nro);
       }


   return copie;
}
END_NAMESPACE_HEXA
