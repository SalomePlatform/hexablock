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

// C++ : Construction de la bride

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

// ======================================================== sauver_schema
bool is_here (int masque, int option)
{
   bool   rep = (masque IAND option) > 0;
   return rep;
}
// ======================================================== sauver_schema
void sauver_schema (Hex::Document* doc)
{
   static int numero = 0;
   char nomfic [32];
   sprintf (nomfic, "debride%d.vtk", numero);
   numero++;

   doc->saveVtk (nomfic);
}
// ======================================================== merge_quads
void merge_quads (Hex::Document* doc, Hex::Elements* quart, Hex::Elements* demi,
                  int ni1, int nj1, int ni2, int nj2, int mask=0)
{
   int  inv     = is_here (mask, OPT_INV_EDGE);
   bool quad_ik = is_here (mask, OPT_QUAD_IK);
   int  iq1     = 0;
   int  iq3     = 1;

   if (quad_ik || inv) 
      {
      iq1 = 1;
      iq3 = 0;
      }

   Hex::Quad* orig  = quad_ik ? quart->getQuadIK (ni1, nj1, k1)
                              : quart->getQuadJK (ni1, nj1, k1);
   Hex::Quad* dest  = demi->getQuadJK  (ni2, nj2, k1);

   if (quad_ik) orig->dumpPlus();

   Hex::Vertex* v1  = dest->getVertex (iq1);
   Hex::Vertex* v3  = dest->getVertex (iq3);

   Hex::Vertex* v2  = orig->getVertex (0);
   Hex::Vertex* v4  = orig->getVertex (1);

   doc->mergeQuads (dest, orig, v1, v2, v3, v4);
   sauver_schema (doc);
}
// ======================================================== test_debride
int test_debride (int nbargs, cpchar tabargs [])
{
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex* orig1 = doc->addVertex (0,0,0);
   Hex::Vertex* orig2 = doc->addVertex (7,0,0);

   Hex::Vector* dz = doc->addVector (0,0,1);
   Hex::Vector* dx = doc->addVector (1,0,0);

   const int dim_z  = 5;
   double drd = 0.5;
   double drq = 2*drd;
   double dl  = 1;
   int    nrq = 10;
   int    nrd = 5;
   int    naq = 4;
   int    nad = 10;
                               // Les grilles initiales

   Hex::Elements* quart = doc->makeCylindrical (orig1,dx,dz,drq, 45, 
                                                dl,nrq,naq,dim_z, true);

   Hex::Elements* demi  = doc->makeCylindrical (orig2,dx,dz,drd, 180, 
                                                dl,nrd,nad, dim_z, true);
   sauver_schema (doc);
   for (int nk= 2; nk<dim_z ; nk++)
       {
                               // Elagage du quart (de brie)
       for (int nj= 0; nj<naq ; nj++)
          {
          int ideb = nk==dim_z-1 ? 1 : 2;
          for (int ni=ideb; ni<nrq ; ni++)
             quart->getHexaIJK (ni, nj, nk)->remove ();
          }
                               // Elagage du demi (reblochon)
       for (int nj= 0; nj<nad ; nj++)
          for (int ni=nrd-nk; ni<nrd ; ni++)
             demi->getHexaIJK (ni, nj, nk)->remove ();
       }

   sauver_schema (doc);
                               // La semelle
   for (int nj= 0; nj<naq ; nj++)
       {
       for (int ni=2; ni<nrq ; ni++)
           quart->getHexaIJK (ni, nj, k0)->remove ();
       }
   sauver_schema (doc);

                               // Creuser les fondations de demi dans quart
   int jmax= nrq-1;
   for (int nj= 0; nj<2 ; nj++)
       {
       for (int ni=3; ni<jmax-nj ; ni++)
           {
           Hex::Hexa* cell = quart->getHexaIJK (ni, nj, k1);
           cell->remove ();
           }
   }
   sauver_schema (doc);
                               // On fusionne les bords
   merge_quads (doc, quart, demi, 9, 0,   nrd, 0);
   merge_quads (doc, quart, demi, 8, 1,   nrd, 1, OPT_QUAD_IK);
   merge_quads (doc, quart, demi, 8, 1,   nrd, 2);
   merge_quads (doc, quart, demi, 7, 2,   nrd, 3, OPT_QUAD_IK);
   merge_quads (doc, quart, demi, 6, 2,   nrd, 4, OPT_QUAD_IK);
   merge_quads (doc, quart, demi, 5, 2,   nrd, 5, OPT_QUAD_IK);
   merge_quads (doc, quart, demi, 4, 2,   nrd, 6, OPT_QUAD_IK);
   merge_quads (doc, quart, demi, 3, 2,   nrd, 7, OPT_QUAD_IK);
   merge_quads (doc, quart, demi, 3, 1,   nrd, 8, OPT_INV_EDGE);
   merge_quads (doc, quart, demi, 3, 0,   nrd, 9, OPT_INV_EDGE);

   //doc->dump ();
   int nbc = doc->countPropagation ();
   doc->purge ();
   //doc->dump ();
   for (int nro=0 ; nro<doc->countHexa() ; nro++)
       {
       printf ( "Hexa[%03d] = ", nro);
       doc->getHexa(nro)->printName ("\n");
       }

   sauver_schema (doc);
   return HOK;
}
