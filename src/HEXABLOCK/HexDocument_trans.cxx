
// C++ : La clase principale de Hexa

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
#include "HexDocument.hxx"

#include "HexEltBase.hxx"
#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexQuad.hxx"
#include "HexHexa.hxx"

#include "HexElements.hxx"

#include "HexVector.hxx"
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
   DumpStart ("performTranslation", elts << trans);

   Matrix matrice;
   matrice.defTranslation (trans);

   int    ier = elts->transform (&matrice);

   DumpReturn (ier);
   return ier;
}
// ==================================================== performScale
int Document::performScale (Elements* elts, Vertex* ver, double k)
{ 
   DumpStart ("performScale", elts << ver << k);

   Matrix matrice;
   matrice.defScale (ver, k);

   int    ier = elts->transform (&matrice);

   DumpReturn (ier);
   return ier;
}
// ==================================================== performRotation
int Document::performRotation (Elements* elts, Vertex* ver, Vector* vec, double angle)
{ 
   DumpStart ("performRotation", elts << ver << vec << angle);

   Matrix matrice;
   matrice.defRotation (ver, vec, angle);

   int    ier = elts->transform (&matrice);

   DumpReturn (ier);
   return ier;
}
// ==================================================== performSymmetryPoint
int Document::performSymmetryPoint (Elements* elts, Vertex* ver)
{ 
   DumpStart ("performSymmetryPoint", elts << ver);

   Matrix matrice;
   matrice.defSymmetryPoint (ver);

   int    ier = elts->transform (&matrice);

   DumpReturn (ier);
   return ier;
}
// ==================================================== performSymmetryPlane
int Document::performSymmetryPlane (Elements* elts, Vertex* ver, Vector* vec)    
{ 
   DumpStart ("performSymmetryPlane", elts << ver << vec);

   Matrix matrice;
   matrice.defSymmetryPlane (ver, vec);

   int    ier = elts->transform (&matrice);

   DumpReturn (ier);
   return ier;
}

// ==================================================== performSymmetryLine
int Document::performSymmetryLine (Elements* elts, Vertex* ver, Vector* vec)
{ 
   DumpStart ("performSymmetryLine", elts << ver << vec);

   Matrix matrice;
   matrice.defSymmetryLine (ver, vec);

   int    ier = elts->transform (&matrice);

   DumpReturn (ier);
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
   DumpStart ("makeTranslation", table  << trans);

   Matrix  matrice;
   matrice.defTranslation (trans);

   Elements* grille = clonerElements (table, &matrice);

   DumpReturn (grille);
   return  grille;
}
// ========================================================= makeScale
Elements* Document::makeScale (Elements* table, Vertex* ver, double k)
{
   DumpStart ("makeScale", table  << ver << k);

   Matrix  matrice;
   matrice.defScale (ver, k);

   Elements* grille = clonerElements (table, &matrice);

   DumpReturn (grille);
   return    grille;
}
// ========================================================= makeRotation
Elements* Document::makeRotation (Elements* table, Vertex* ver, Vector* vec, 
                                  double angle)
{
   DumpStart ("makeRotation", table  << ver << vec << angle);

   Matrix  matrice;
   matrice.defRotation (ver, vec, angle);

   Elements* grille = clonerElements (table, &matrice);

   DumpReturn (grille);
   return    grille;
}
// ========================================================= makeSymmetryPoint
Elements* Document::makeSymmetryPoint (Elements* table, Vertex* ver)
{
   DumpStart ("makeSymmetryPoint", table  << ver);

   Matrix  matrice;
   matrice.defSymmetryPoint (ver);

   Elements* grille = clonerElements (table, &matrice);

   DumpReturn (grille);
   return    grille;
}
// ========================================================= makeSymmetryLine
Elements* Document::makeSymmetryLine (Elements* table, Vertex* ver, Vector* vec)
{
   DumpStart ("makeSymmetryLine", table  << ver << vec);

   Matrix  matrice;
   matrice.defSymmetryLine (ver, vec);

   Elements* grille = clonerElements (table, &matrice);

   DumpReturn (grille);
   return    grille;
}
// ========================================================= makeSymmetryPlane
Elements* Document::makeSymmetryPlane (Elements* table, Vertex* ver, Vector* vec)
{
   DumpStart ("makeSymmetryPlane", table  << ver << vec);

   Matrix  matrice;
   matrice.defSymmetryPlane (ver, vec);

   Elements* grille = clonerElements (table, &matrice);

   DumpReturn (grille);
   return    grille;
}
// ----------------------------------------------------------------------
// ------------ Destroy
// ----------------------------------------------------------------------
// ========================================================= removeHexa
int Document::removeHexa (Hexa* cell)
{
   DumpStart ("removeHexa", cell);

   int ier = HOK;
   if (BadElement (cell))
      {
      ier = HERR;
      DumpReturn (ier);
      return ier;
      }

   update ();
   Quads menage;
   for (int nro = 0 ; nro < HQ_MAXI ; ++nro)
       {
       Quad* quad = cell->getQuad (nro);
       if (EltIsValid(quad) && quad->getNbrParents()<=1)
          menage.push_back (quad);
       }

   cell->remove ();
   int nbdel = menage.size();
   for (int nro = 0 ; nro < nbdel ; ++nro)
       menage[nro]->remove();

   DumpReturn (HOK);
   return HOK;
}
// ========================================================= removeQuad
int Document::removeQuad (Quad* cell)
{
   DumpStart ("removeQuad", cell);

   int ier = HOK;
   if (cell==NULL)
      ier = HERR;
   else
      cell->remove ();

   DumpReturn (ier);
   return ier;
}
// ========================================================= removeElements
int Document::removeElements (Elements* bloc)
{
   DumpStart ("removeElements", bloc);

   int ier = HOK;

   if (bloc==NULL)
      ier = HERR;
   else
      bloc->remove ();

   DumpReturn (ier);
   return ier;
}
// ====================================================== removeConnectedHexa
int Document::removeConnectedHexa (Hexa* cell)
{
   DumpStart ("removeConnectedHexa", cell);

   int ier = HOK;

   if (cell==NULL)
      ier = HERR;
   else
      {
      update ();
      cell->removeConnected ();
      }

   DumpReturn (ier);
   return ier;
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
   for (EltBase* elt = doc_first_elt[EL_QUAD]->next (); elt!=NULL;
                 elt = elt->next())
       {
       Quad* quad = static_cast <Quad*> (elt);
       if (quad!=NULL && quad->isHere())
           quad->setOrientation ();
       }

}
END_NAMESPACE_HEXA
