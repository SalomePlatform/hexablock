
// C++ : Construction de la bride

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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "Hex.hxx"
#include "HexDocument.hxx"
#include "HexElements.hxx"
#include "HexHexa.hxx"
#include "HexQuad.hxx"
#include "HexEdge.hxx"
#include "HexVertex.hxx"

static const int k0 = 0;
static const int k1 = 1;
static const int OPT_INV_EDGE = 1;
static const int OPT_QUAD_IK = 2;

// ======================================================== test_bug1
int test_bug1 (int nbargs, cpchar tabargs[])
{
   int nvtk = 0;
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex* centre = doc->addVertex (0,0,0);

   Hex::Vector* vecteur_px = doc->addVector (1,0,0);
   Hex::Vector* vecteur_pz = doc->addVector (0,0,1);

   int nr = 5;
   int na = 5;
   int nl = 5;
   Hex::Elements* grid = doc->makeCylindrical (centre, vecteur_px, 
                        vecteur_pz, 1, 180, 1, nr, na, nl, true);
   doc->saveVtk ("bug_bride", nvtk);

                     // Elagage trou
   for (int j=0; j<na ; j++)
       for (int k=0; k<na ; k++)
           doc->removeHexa (grid->getHexaIJK (1, j, k));

   doc->saveVtk ("bug_bride", nvtk);
                 // elagage des parties inferieures et superieures :
   for (int j=0; j<na ; j++)
       for (int i=1; i<nr ; i++)
           for (int k=0; k<2 ; k++)
               doc->removeHexa (grid->getHexaIJK (i, j, 4*k));

   doc->saveVtk ("bug_bride", nvtk);

                 // Escalier
   for (int j=0; j<na ; j++)
       {
       doc->removeHexa (grid->getHexaIJK (4, j, 2));
       for (int i=3; i<=4 ; i++)
           doc->removeHexa (grid->getHexaIJK (i, j, 3));
       }

   doc->removeHexa (grid->getHexaIJK (4, 3, 1));
   doc->saveVtk ("bug_bride", nvtk);

   Hex::Vector* vecteur_a = doc->addVector (-1, 2, 0);
   Hex::Quads   quads (2);
   quads [0] = grid->getQuadIK (4, 4, 1);
   quads [1] = grid->getQuadJK (5, 4, 1);

         // JPL (le 28/07/2011) : la 2e partie du prisme est vraiment utile ?
   Hex::Elements* prisme_a = doc->prismQuads (quads, vecteur_a, 3);
   doc->saveVtk ("bug_bride", nvtk);

   doc->removeHexa (prisme_a->getHexa (2));
   doc->saveVtk ("bug_bride", nvtk);

   Hex::Vector* vecteur_mz = doc->addVector (0, 0, -1);
   Hex::Hexa*   cheminee   = prisme_a->getHexa (5);
   Hex::Quad*   quad_b1    = cheminee->getQuad (2);
   Hex::Quad*   quad_b2    = cheminee->getQuad (3);

   Hex::Elements* prisme_b1 = doc->prismQuad (quad_b1, vecteur_mz, 1);
   doc->saveVtk ("bug_bride", nvtk);
                 // Au dessus
   Hex::Elements* prisme_b2 = doc->prismQuad (quad_b2, vecteur_pz, 1);
   doc->saveVtk ("bug_bride", nvtk);
                 // Au dessus

   Hex::Hexa* hexa_ca = prisme_a->getHexa(0);
   Hex::Hexa* hexa_cb = prisme_a->getHexa(1);

   Hex::Quad* quad_ca = hexa_ca->getQuad(5);
   Hex::Quad* quad_cb = hexa_cb->getQuad(5);

   Hex::Vertex* point_cb = quad_ca->getVertex(0);
   Hex::Vertex* point_cc = quad_cb->getVertex(0);

   Hex::Quad*   quad_c1  = grid->getQuadJK(4, 3, 1);
// Hex::Quad*   quad_c2  = grid->getQuadIK(3, 3, 1);
   Hex::Quad*   quad_c2  = grid->getQuadIK(4, 3, 1);

   Hex::Vertex* point_c1 = grid->getVertexIJK(4, 4, 1);
   Hex::Vertex* point_c2 = grid->getVertexIJK(4, 3, 1);
   Hex::Vertex* point_c3 = grid->getVertexIJK(5, 3, 1);

//  # JPL : cette ligne fait planter Salome (probleme connu du moteur Hexablock)
   //         quad_ca->setScalar (1);
   //         doc->saveVtk ("bug_bride", nvtk);
   //         quad_c1->setScalar (2);

   char c1[10];
   Hex::Vertex* point_cx = quad_c1->getVertex(3);
   HexDisplay (point_cx->getName(c1));
   // point_cx->setScalar (2);
   // doc->saveVtk ("bug_bride", nvtk);

   Hex::Quad*  quad_c1x = grid->getQuadJK  (4, 3, 2);
   Hex::Hexa*  hexa_c1x = grid->getHexaIJK (3, 2, 2);
   HexDisplay (quad_c2->getName(c1));
   quad_c2->setScalar (5);
   doc->saveVtk ("bug_bride", nvtk);

   // hexa_c1x->setScalar (5);
   // doc->saveVtk ("bug_bride", nvtk);

   // hexa_c1x->disconnectVertex (point_cx);
   // doc->saveVtk ("bug_bride", nvtk);
   // quad_c1x->setScalar (5);

   // doc->mergeQuads(quad_c1, quad_ca,  point_c1, point_c1,  point_c2, point_cb);
   doc->closeQuads(quad_c1, quad_ca);
   doc->saveVtk ("bug_bride", nvtk);

   doc->setLevel (9);
   int ier = doc->mergeQuads(quad_c2, quad_cb,  
                             point_c2, point_cb, point_c3, point_cc);
   /// doc->closeQuads(quad_c2, quad_cb);
   HexDisplay (ier);
   doc->saveVtk ("bug_bride", nvtk);

   return HOK;
}

#if _pipo_

# Fusionner la semelle à la grille
# --------------------------------

hexa_ca = prisme_a.getHexa(0)
hexa_cb = prisme_a.getHexa(1)

quad_ca = hexa_ca.getQuad(5)
quad_cb = hexa_cb.getQuad(5)

point_cb = quad_ca.getVertex(0)
point_cc = quad_cb.getVertex(0)

quad_c1 = grid.getQuadJK(4, 3, 1)
quad_c2 = grid.getQuadIK(3, 3, 1)

point_c1 = grid.getVertexIJK(4, 4, 1)
point_c2 = grid.getVertexIJK(4, 3, 1)
point_c3 = grid.getVertexIJK(5, 3, 1)

# JPL : cette ligne fait planter Salome (probleme connu du moteur Hexablock)
doc.mergeQuads(quad_ca, quad_c1,  point_c1, point_c1,  point_cb, point_c2)

## # TEST : point_ca au lieu de point_c1 (confondus normalement)
## point_ca = quad_ca.getVertex(1)
## doc.mergeQuads(quad_ca, quad_c1, point_ca, point_c1, point_cb, point_c2)
## # end TEST

doc.mergeQuads(quad_cb, quad_c2, point_cb, point_c2, point_cc, point_c3)

# temporaire : sauvegarde du modele de blocs :
save_schema(doc)
# fin temporaire


# geometrie :
# ===========

bride_geom = geompy.ImportFile(BREP_PATH, "BREP")
geompy.addToStudy(bride_geom, "bride_geom")
all_edges_bride = geompy.SubShapeAllSorted(bride_geom, geompy.ShapeType["EDGE"])

# => 92 edges au total dans la geometrie

for i, edge in enumerate(all_edges_bride):
    geompy.addToStudy(edge, "edge_" + str(i))

#endif
