
// C++ : Copiteur d'hexaedres

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
#include "HexCloner.hxx"
#include "HexMatrix.hxx"

#include "HexHexa.hxx"
#include "HexQuad.hxx"
#include "HexEdge.hxx"
#include "HexVertex.hxx"

#include "HexElements.hxx"
#include "HexVertexShape.hxx"
#include "HexEdgeShape.hxx"
#include "HexFaceShape.hxx"

BEGIN_NAMESPACE_HEXA

void transfo_brep (string& brep, Matrix* matrice, string& trep);
static bool db = false;

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

   if (db)
      {
      printf ( " --- Cloner::Asso (%s) -> asso (%s)\n", orig ->getName  (),
                                                        copie->getName ());
      }
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

/***********************
   // const  Shapes & new_asso = copie->getAssociations ();  TODO  New Shape
   // int    nbass             = new_asso.size();
   // if (nbass!=0)
      // return copie;

   const Shapes & tab_asso = orig->getAssociations ();
   nbass             = tab_asso.size();
   for (int nro=0 ; nro < nbass ; nro++)
       {
       Shape* shape = tab_asso [nro];
       string brep  = shape->getBrep();
       string trep;
       transfo_brep (brep, matrice, trep);
       Shape* tshape = new Shape (trep);
       tshape->setBounds (shape->getStart(), shape->getEnd());
       copie ->addAssociation (tshape);
       if (db)
          {
          printf ( " --- Cloner::Asso (%s) -> asso (%s)\n", orig ->getName (),
                                                            copie->getName ());
          // geom_dump_asso (orig );
          // geom_dump_asso (copie);
          }
       }
***************************************************/
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
/*******************************************************************
   const  Shapes & new_asso = copie->getAssociations ();
   int    nbass             = new_asso.size();
   if (nbass!=0)
      return copie;

   const Shapes & tab_asso = orig->getAssociations ();
   nbass             = tab_asso.size();
   for (int nro=0 ; nro < nbass ; nro++)
       {
       Shape* shape = tab_asso [nro];
       string brep  = shape->getBrep();
       string trep;
       transfo_brep (brep, matrice, trep);
       Shape* tshape = new Shape (trep);
       copie ->addAssociation (tshape);
       if (db)
          printf ( " --- Asso (%s) -> asso (%s)\n", orig ->getName (),
                                                    copie->getName ());
       }

***************************************************/
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
   if (db)
      {
      double             a11,a12,a13,a14, a21,a22,a23,a24, a31,a32,a33,a34;
      matrice->getCoeff (a11,a12,a13,a14, a21,a22,a23,a24, a31,a32,a33,a34);
      printf ( "\n");
      printf ( " --- Matrice = (%g, %g, %g) (%g) \n", a11,a12,a13,a14 );
      printf ( "               (%g, %g, %g) (%g) \n", a21,a22,a23,a24 );
      printf ( "               (%g, %g, %g) (%g) \n", a31,a32,a33,a34 );
      }

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
