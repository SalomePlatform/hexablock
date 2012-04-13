
// C++ : La clase principale de Hexa

// Copyright (C) 2009-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include "HexEltBase.hxx"
#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexQuad.hxx"
#include "HexHexa.hxx"

#include "HexElements.hxx"
#include "HexCrossElements.hxx"

#include "HexVector.hxx"
#include "HexCylinder.hxx"
#include "HexPipe.hxx"
#include "HexMatrix.hxx"
#include "HexCloner.hxx"
#include "HexPropagation.hxx"
#include "HexLaw.hxx"

#include "HexXmlWriter.hxx"
#include "HexXmlTree.hxx"
#include "HexGlobale.hxx"

BEGIN_NAMESPACE_HEXA
// ========================================================= performTranslation
int Document::performTranslation (Elements* elts, Vector* trans)
{ 
   Matrix matrice;
   matrice.defTranslation (trans);

   int    ier = elts->transform (&matrice);
   return ier;
}
// ==================================================== performScale
int Document::performScale (Elements* elts, Vertex* ver, double k)
{ 
   Matrix matrice;
   matrice.defScale (ver, k);

   int    ier = elts->transform (&matrice);
   return ier;
}
// ==================================================== performRotation
int Document::performRotation (Elements* elts, Vertex* ver, Vector* vec, double angle)
{ 
   Matrix matrice;
   matrice.defRotation (ver, vec, angle);

   int    ier = elts->transform (&matrice);
   return ier;
}
// ==================================================== performSymmetryPoint
int Document::performSymmetryPoint (Elements* elts, Vertex* ver)
{ 
   Matrix matrice;
   matrice.defSymmetryPoint (ver);

   int    ier = elts->transform (&matrice);
   return ier;
}
// ==================================================== performSymmetryPlane
int Document::performSymmetryPlane (Elements* elts, Vertex* ver, Vector* vec)    
{ 
   Matrix matrice;
   matrice.defSymmetryPlane (ver, vec);

   int    ier = elts->transform (&matrice);
   return ier;
}
// ==================================================== performSymmetryLine
int Document::performSymmetryLine (Elements* elts, Vertex* ver, Vector* vec)
{ 
   Matrix matrice;
   matrice.defSymmetryLine (ver, vec);

   int    ier = elts->transform (&matrice);
   return ier;
}
// -------------------------------------------------------- 
// -------------------------------------------------------- 
// -------------------------------------------------------- 
// ========================================================= clonerElements
Elements* Document::clonerElements (Elements* table, Matrix* matrice)
{
   Cloner    copieur (matrice);
   Elements* grille = copieur.clonerElements (table);
   return    grille;
}
// -------------------------------------------------------- 
// -------------------------------------------------------- 
// -------------------------------------------------------- 
// ========================================================= makeTranslation
Elements* Document::makeTranslation (Elements* table, Vector* trans)
{
   Matrix  matrice;
   matrice.defTranslation (trans);

   Elements* grille = clonerElements (table, &matrice);
   return    grille;
}
// ========================================================= makeScale
Elements* Document::makeScale (Elements* table, Vertex* ver, double k)
{
   Matrix  matrice;
   matrice.defScale (ver, k);

   Elements* grille = clonerElements (table, &matrice);
   return    grille;
}
// ========================================================= makeRotation
Elements* Document::makeRotation (Elements* table, Vertex* ver, Vector* vec, 
                                double angle)
{
   Matrix  matrice;
   matrice.defRotation (ver, vec, angle);

   Elements* grille = clonerElements (table, &matrice);
   return    grille;
}
// ========================================================= makeSymmetryPoint
Elements* Document::makeSymmetryPoint (Elements* table, Vertex* ver)
{
   Matrix  matrice;
   matrice.defSymmetryPoint (ver);

   Elements* grille = clonerElements (table, &matrice);
   return    grille;
}
// ========================================================= makeSymmetryLine
Elements* Document::makeSymmetryLine (Elements* table, Vertex* ver, Vector* vec)
{
   Matrix  matrice;
   matrice.defSymmetryLine (ver, vec);

   Elements* grille = clonerElements (table, &matrice);
   return    grille;
}
// ========================================================= makeSymmetryPlane
Elements* Document::makeSymmetryPlane (Elements* table, Vertex* ver, Vector* vec)
{
   Matrix  matrice;
   matrice.defSymmetryPlane (ver, vec);

   Elements* grille = clonerElements (table, &matrice);
   return    grille;
}
// ----------------------------------------------------------------------
// ------------ Destroy
// ----------------------------------------------------------------------
// ========================================================= removeHexa
int Document::removeHexa (Hexa* cell)
{
   if (cell==NULL)
      return HERR;

   cell->remove ();
   return  HOK;
}
// ========================================================= removeQuad
int Document::removeQuad (Quad* cell)
{
   if (cell==NULL)
      return HERR;

   cell->remove ();
   return  HOK;
}
// ========================================================= removeElements
int Document::removeElements (Elements* bloc)
{
   if (bloc==NULL)
      return HERR;

   bloc->remove ();
   return  HOK;
}
// ====================================================== removeConnectedHexa
int Document::removeConnectedHexa (Hexa* cell)
{
   if (cell==NULL)
      return HERR;

   update ();
   cell->removeConnected ();
   return  HOK;
}
// ========================================================= dumpPropagation
void Document::dumpPropagation ()
{
   int nb = countPropagation ();

   for (int nro=0 ; nro<nb ; nro++)
       {
       Propagation*  prop  = getPropagation (nro);
       const Edges&  table = prop->getEdges ();
       printf (" ____________________________________ Prop nro %d\n", nro);
       for (int ned=0 ; ned<(int)table.size() ; ned++)
           {
           bool way = table [ned]->getWay ();

           table [ned]->printName (" = ");
           if (way)
              {
              printf (" +(");
              table [ned]->getVertex (0)->printName (", ");
              table [ned]->getVertex (1)->printName (")\n");
              }
          else
              {
              printf (" -(");
              table [ned]->getVertex (1)->printName (", ");
              table [ned]->getVertex (0)->printName (")\n");
              }
           }
       }
}
// ----------------------------------------------------------------------
// ------------ reorderFaces
// ----------------------------------------------------------------------
// ========================================================= reorderFaces
// ==== Ordonner les faces externes
void Document::reorderFaces ()
{
   majReferences ();
   Real3 cg, orig, pi, pj, vi, vj, vk;

   for (EltBase* elt = doc_first_elt[EL_QUAD]->next (); elt!=NULL;
                 elt = elt->next())
       {
       Quad* quad = static_cast <Quad*> (elt);
       if (quad!=NULL && quad->isHere())
           quad->setOrientation ();
       }

}
END_NAMESPACE_HEXA
