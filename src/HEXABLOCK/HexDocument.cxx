
// C++ : La clase principale de Hexa

//  Copyright (C) 2009-2012  CEA/DEN, EDF R&D
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
//  See http://www.salome-platform.org/
//  or email : webmaster.salome@opencascade.com

#include "HexDocument.hxx"

#include "HexEltBase.hxx"
#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexQuad.hxx"
#include "HexHexa.hxx"

#include "HexElements.hxx"
#include "HexCrossElements.hxx"
#include "HexBiCylinder.hxx"

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
#include "HexGroup.hxx"
#include "Hex.hxx"
#include "HexNewShape.hxx"

BEGIN_NAMESPACE_HEXA

int Document::doc_number = 0;

// ======================================================== Constructeur
Document::Document (cpchar name, Hex* dad)
{
   el_name    = name;
   hex_parent = dad;
   glob       = Globale::getInstance ();
   el_root    = this;

   doc_db        = 0;
   nbr_errors    = 0;
   doc_modified  = true;
   doc_saved     = false;
   doc_tolerance = 1e-6;
   nbr_laws      = 0;
   nbr_propagations = 0;
   maj_propagation  = true;
   doc_xml          = NULL;

   nbr_used_hexas = nbr_used_quads = nbr_used_edges = nbr_used_vertex = 0;

   for (EnumElt type = EL_NONE ; type < EL_MAXI ; type=(EnumElt) (type+1))
       {
       doc_first_elt [type] = doc_last_elt [type] = new EltBase ();
       doc_nbr_elt   [type] = 0;
       }

   if (doc_number == 0)
      strcpy (doc_ident, "doc");
   else
      sprintf (doc_ident, "doc%02d", doc_number);
   doc_number++;

   DumpLock;
   defaultLaw = addLaw ("DefaultLaw", 0);
   DumpRestore;

   addCloud ();
}
// ======================================================== Destructeur
Document::~Document ()
{
   delete doc_xml;

   for (EnumElt type = EL_NONE ; type < EL_MAXI ; type=(EnumElt) (type+1))
       {
       //  printf ("____________________________ Type=%d\n", type);
       EltBase* next = NULL;
       for (EltBase* elt=doc_first_elt [type] ; elt != NULL ; elt=next)
           {
           next = elt->next();
           delete elt;
           }
       }
}
// ======================================================== addVector
Vector* Document::addVector (double dx, double dy, double dz)
{
   DumpStart ("addVector", dx  << dy  << dz);
   Vector* pv = new Vector (this, dx, dy, dz);
   doc_vector.push_back (pv);

   DumpReturn (pv);
   return  pv;
}
// ======================================================== addVectorVertices
Vector* Document::addVectorVertices (Vertex* va, Vertex* vb)
{
   DumpStart ("addVectorVertices", va  << vb);

   Vector* pv = addVector (vb->getX () - va->getX (),
                           vb->getY () - va->getY (),
                           vb->getZ () - va->getZ ());

   DumpReturn (pv);
   return  pv;
}
// ======================================================== addVertex
Vertex* Document::addVertex (double x, double y, double z)
{
   DumpStart ("addVertex", x  << y  << z);

   Vertex* pv   = new Vertex (this, x, y, z);
   DumpReturn (pv);
   return pv;
}
// ======================================================== addEdge
Edge* Document::addEdge (Vertex* va, Vertex* vb)
{
   DumpStart ("addEdge", va  << vb );

   if (va==NULL || vb==NULL)
      return NULL;

   Edge* arete  = new Edge (va, vb);
   DumpReturn (arete);
   return arete;
}
// ======================================================== addEdgeVector
Edge* Document::addEdgeVector (Vertex* va, Vector* vec)
{
   DumpStart ("addEdgeVector", va  << vec );

   Vertex* vb   = addVertex (va->getX () + vec->getDx(),
                             va->getY () + vec->getDy(),
                             va->getZ () + vec->getDz());

   Edge*  arete = addEdge (va, vb);
   DumpReturn (arete);
   return arete;
}
// ======================================================== addQuadVertices
Quad* Document::addQuadVertices (Vertex* va, Vertex* vb, Vertex* vc, Vertex* vd)
{
   DumpStart ("addQuadVertices", va << vb << vc << vd );

   Quad*  face   = new Quad (va, vb, vc, vd);
   DumpReturn (face);
   return face;
}
// ======================================================== addQuad
Quad* Document::addQuad (Edge* ea, Edge* eb, Edge* ec, Edge* ed)
{
   DumpStart ("addQuad", ea << eb << ec << ed );

   Quad*  face   = new Quad (ea, eb, ec, ed);
   DumpReturn (face);
   return face;
}
// ======================================================== addHexaVertices
Hexa* Document::addHexaVertices (Vertex* va, Vertex* vb, Vertex* vc, Vertex* vd,
                                 Vertex* ve, Vertex* vf, Vertex* vg, Vertex* vh)
{
   DumpStart ("addHexaVertices", va << vb << vc << vd << ve << vf << vg << vh);

   Hexa*  pave   = new Hexa (va, vb, vc, vd, ve, vf, vg, vh);
   DumpReturn (pave);
   return pave;
}
// ======================================================== addHexa
Hexa* Document::addHexa (Quad* qa, Quad* qb, Quad* qc, Quad* qd, Quad* qe,
                         Quad* qf)
{
   DumpStart ("addHexa", qa << qb << qc << qd << qe << qf );

   Hexa*  pave   = new Hexa (qa, qb, qc, qd, qe, qf);
   DumpReturn (pave);
   return pave;
}
// ======================================================== addCylinder
Cylinder* Document::addCylinder (Vertex* b, Vector* d, double r,  double h)
{
   DumpStart ("addCylinder", b << d << r << h);

   Cylinder* cyl = new  Cylinder (b, d, r, h);
   doc_cylinder.push_back (cyl);

   DumpReturn (cyl);
   return    cyl;
}
// ======================================================== addPipe
Pipe* Document::addPipe (Vertex* b, Vector* d, double ri, double re, double h)
{
   DumpStart ("addPipe", b << d << ri << re << h);

   Pipe*  tuyau = new  Pipe (b, d, ri, re, h);
   doc_pipe.push_back (tuyau);

   DumpReturn (tuyau);
   return tuyau;
}
// ======================================================== makeCartesian
Elements* Document::makeCartesian (Vertex* v, Vector* dir,
                            int px, int py, int pz, int mx, int my, int mz)
{
   DumpStart ("makeCartesianDiag", v << dir << px << py << pz << mx << my << mz);

   Vector* v1 = new Vector (this, dir->getDx(), 0, 0);
   Vector* v2 = new Vector (this, 0, dir->getDy(), 0);
   Vector* v3 = new Vector (this, 0,0,  dir->getDz());
   Elements*  grille = new Elements (this);
   grille->makeCartesianGrid (v, v1, v2, v3, px, py, pz, mx, my, mz);

   DumpReturn (grille);
   return grille;
}
// ======================================================== makeCartesian
Elements* Document::makeCartesian (Vertex* v, Vector* v1, Vector* v2,
                                   Vector* v3, int px, int py, int pz)
{
   DumpStart ("makeCartesian", v << v1 << v2 << v3 << px << py << pz);

   Elements* grille = new Elements (this);
   grille->makeCartesianGrid (v, v1, v2, v3, px, py, pz);

   DumpReturn (grille);
   return grille;
}
// ======================================================== makeCartesian1
Elements* Document::makeCartesian1 (Vertex* v, Vector* v1, Vector* v2,
                    Vector* v3, int px, int py, int pz, int mx, int my, int mz)
{
   DumpStart ("makeCartesian1", v << v1 << v2 << v3 << px << py << pz
                              << mx << my << mz);

   Elements* grille = new Elements (this);
   grille->makeCartesianGrid (v, v1, v2, v3, px, py, pz, mx, my, mz);

   DumpReturn (grille);
   return grille;
}
// ======================================================== makeSpherical
Elements* Document::makeSpherical (Vertex* c, double rayon, int nb, double k)
{
   DumpStart ("makeSpherical", c << rayon << nb << k );

   Elements* grille = new Elements (this);
   grille->makeSphericalGrid (c, rayon, nb, k);

   DumpReturn (grille);
   return grille;
}
// ======================================================== makeCylindrical
Elements* Document::makeCylindrical (Vertex* c, Vector* b, Vector* h,
        double dr, double da, double dl, int nr, int na, int nl, bool fill)
{
   DumpStart ("makeCylindrical", c << b << h << dr << da << dl << nr << na
                                   << nl << fill);

   Elements* grille = new Elements (this);
   grille->makeCylindricalGrid (c, b, h, dr, da, dl, nr, na, nl, fill);

   DumpReturn (grille);
   return grille;
}
// ======================================================== makeSphere
Elements* Document::makeSphere (Vertex* center, Vector* vx, Vector* vz,
                                double radius, double radhole, Vertex* plorig,
                                int nrad, int nang, int nhaut)
{
   DumpStart ("makeSphere", center << vx << vz << radius << radhole << plorig
                  << nrad << nang << nhaut);

   Elements* grille = new Elements (this);
   double radint = (radhole + radius)*DEMI;
   grille->makeRind (GR_HEMISPHERIC, center, vx, vz, radius, radint, radhole,
                     plorig, 360.0, nrad, nang, nhaut);

   DumpReturn (grille);
   return grille;
}
// ======================================================== makePartSphere
Elements* Document::makePartSphere (Vertex* center, Vector* vx, Vector* vz,
                                 double  radius, double radhole,
                                 Vertex* plorig, double angle,
                                 int nrad, int nang, int nhaut)
{
   DumpStart ("makePartSphere", center << vx << vz
                  << radius << radhole
                  << plorig << angle
                  << nrad   << nang << nhaut);

   Elements* grille = new Elements (this);
   double radint = (radhole + radius)*DEMI;
   grille->makeRind (GR_PART_SPHERIC, center, vx, vz, radius, radint, radhole,
                     plorig, angle, nrad, nang, nhaut);

   DumpReturn (grille);
   return grille;
}
// ======================================================== makeRind
Elements* Document::makeRind (Vertex* center, Vector* vx, Vector* vz,
                              double  radext, double radint, double radhole,
                              Vertex* plorig, int nrad, int nang, int nhaut)
{
   DumpStart ("makeRind", center << vx << vz
                  << radext << radint << radhole
                  << plorig << nrad   << nang << nhaut);

   Elements* grille = new Elements (this);
   grille->makeRind (GR_RIND, center, vx, vz, radext, radint, radhole,
                     plorig, 360.0, nrad, nang, nhaut);

   DumpReturn (grille);
   return grille;
}
// ======================================================== makePartRind
Elements* Document::makePartRind (Vertex* center, Vector* vx, Vector* vz,
                           double  radext, double radint, double radhole,
                           Vertex* plorig, double angle,
                               int nrad, int nang, int nhaut)
{
   DumpStart ("makePartRind", center << vx << vz
                  << radext << radint << radhole
                  << plorig << angle  << nrad  << nang << nhaut);

   Elements* grille = new Elements (this);
   grille->makeRind (GR_PART_RIND, center, vx, vz, radext, radint, radhole,
                     plorig, angle, nrad, nang, nhaut);

   DumpReturn (grille);
   return grille;
}
// ======================================================== findVertex
Vertex* Document::findVertex (double vx, double vy, double vz)
{
   double xmin = vx - doc_tolerance;
   double xmax = vx + doc_tolerance;
   double ymin = vy - doc_tolerance;
   double ymax = vy + doc_tolerance;
   double zmin = vz - doc_tolerance;
   double zmax = vz + doc_tolerance;

   for (EltBase* elt = doc_first_elt[EL_VERTEX]->next (); elt!=NULL;
                 elt = elt->next())
       {
       if (elt->isHere())
          {
          Vertex* node = static_cast <Vertex*> (elt);
          if (node->isin (xmin, xmax, ymin, ymax, zmin, zmax))
             return node;
          }
       }
   return NULL;
}
// ======================================================== findEdge
Edge* Document::findEdge (Vertex* v1, Vertex* v2)
{
   for (EltBase* elt = doc_first_elt[EL_EDGE]->next (); elt!=NULL;
                 elt = elt->next())
       {
       Edge* candidat  = static_cast <Edge*> (elt);
       if (candidat->definedBy (v1, v2))
          return candidat;
       }
   return NULL;
}
// ======================================================== findQuad
Quad* Document::findQuad (Vertex* v1, Vertex* v2)
{
   for (EltBase* elt = doc_first_elt[EL_QUAD]->next (); elt!=NULL;
                 elt = elt->next())
       {
       Quad* candidat  = static_cast <Quad*> (elt);
       if (candidat->definedBy (v1, v2))
          return candidat;
       }
   return NULL;
}
// ======================================================== findHexa
Hexa* Document::findHexa (Vertex* v1, Vertex* v2)
{
   for (EltBase* elt = doc_first_elt[EL_HEXA]->next (); elt!=NULL;
                 elt = elt->next())
       {
       Hexa* candidat  = static_cast <Hexa*> (elt);
       if (candidat->definedBy (v1, v2))
          return candidat;
       }
   return NULL;
}
// ======================================================== findElement
EltBase* Document::findElement (EnumElt type, int ident)
{
   for (EltBase* elt = doc_first_elt[type]->next (); elt!=NULL;
                 elt = elt->next())
       {
       if (elt->isHere() && elt->getId()==ident)
          return elt;
       }
   return NULL;
}
// ======================================================== findVertex
Vertex* Document::findVertex (int ident)
{
   Vertex* node = static_cast <Vertex*> (findElement (EL_VERTEX, ident));
   return  node;

}
// ======================================================== index_tv
int index_tv (Vertex* table[], Vertex* elt)
{
   for (int nro=0; nro<QUAD4; nro++)
        if (elt == table[nro])
           return nro;

   return NOTHING;
}
// ======================================================== mergeQuads
int Document::mergeQuads (Quad* dest, Quad* orig, Vertex* v1, Vertex* v2,
                                                Vertex* v3, Vertex* v4)
{
   DumpStart ("mergeQuads", dest << orig << v1 << v2  << v3 << v4);

   update ();

   if (dest==orig)
      {
      printf (" *** Quads identiques dans mergeQuads\n");
      DumpEnd;
      return HERR;
      }
   else if (dest==NULL || dest->isDeleted())
      {
      printf (" *** Quad nro 1 incorrect dans mergeQuads \n");
      DumpEnd;
      return HERR;
      }
   else if (orig==NULL || orig->isDeleted() )
      {
      printf (" *** Quad nro 2 incorrect dans mergeQuads \n");
      DumpEnd;
      return HERR;
      }

   int nbcomm = 0;
   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       int norig = orig->indexVertex (dest->getVertex(nro));
       if (norig != NOTHING)
          {
          Vertex* uv = dest->getVertex(nro);
          char nom[12];
          nbcomm ++;
          if (nbcomm==1)
             {
             printf ("  +++ Sommets communs dans mergeQuads");
             printf (" (%s,",  dest->getName (nom));
             printf (" %s)\n", orig->getName (nom));
             }
          printf ("  +++ quad1[%d] = quad2[%d] = %s\n", nro,  norig,
                                                        uv->getName (nom));
          }
       }

   if (nbcomm == 2)
      {
      printf ("  +++ Les vertex passes en arguments sont ignores\n");
      int ier = closeQuads (dest, orig);
      DumpEnd;
      return ier;
      }
   else if (nbcomm != 0)
      {
      printf ("  *** _____________________ mergeQuads refuse: \n");
      printf (" il y a %d sommet(s) sont commun(s)\n", nbcomm);
      DumpEnd;
      return HERR;
      }

   if (   v1 ==NULL || v1 ->isDeleted() || v2 ==NULL || v2 ->isDeleted()
       || v3 ==NULL || v3 ->isDeleted() || v4 ==NULL || v4 ->isDeleted())
      {
      printf ("  *** _____________________ mergeQuads refuse: \n");
      printf (" un sommet est incorrect\n");
      DumpEnd;
      return HERR;
      }


   if (debug())
      {
      printf ("  ----------------- mergeQuads : \n");
      HexDump (orig);
      HexDump (dest);
      HexDump (v1);
      HexDump (v2);
      HexDump (v3);
      HexDump (v4);
      }

   Vertex *tv1 [QUAD4], *tv2 [QUAD4];
   Edge   *te1 [QUAD4], *te2 [QUAD4];

   int ier1 = dest->ordoVertex (v1, v3, tv1);
   int ier2 = orig->ordoVertex (v2, v4, tv2);
   if (ier1 != HOK)
      {
      DumpEnd;
      return ier1;
      }
   else if (ier2 != HOK)
      {
      DumpEnd;
      return ier2;
      }

   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       te1 [nro] = dest->getEdge(nro);
       Vertex* va1 = te1[nro]->getVertex(V_AMONT);
       Vertex* vb1 = te1[nro]->getVertex(V_AVAL);
       int na = index_tv  (tv1, va1);
       int nb = index_tv  (tv1, vb1);
       if (na==NOTHING || nb==NOTHING)
          {
          DumpEnd;
          return HERR;
          }

       te2 [nro] = orig->findEdge (tv2[na], tv2[nb]);
       if (te2[nro]==NULL)
          {
          DumpEnd;
          return HERR;
          }
       }

   if (debug())
      {
      printf ("  ----------------- Correspondances mergeQuads : \n");
      for (int nro=0 ; nro<QUAD4 ; nro++)
          {
          printf ("  %d  : ", nro);
          tv2 [nro]->printName(" -> ");
          tv1 [nro]->printName("\n");
          }
      for (int nro=0 ; nro<QUAD4 ; nro++)
          {
          printf ("  %d  : ", nro);
          te2 [nro]->printName(" (");
          te2 [nro]->getVertex(0)->printName(", ");
          te2 [nro]->getVertex(1)->printName(") -> ");
          te1 [nro]->printName(" (");
          te1 [nro]->getVertex(0)->printName(", ");
          te1 [nro]->getVertex(1)->printName(")\n");
          }
      }

   replaceQuad (orig, dest);
   for (int nro=0 ; nro<QUAD4 ; nro++)
       replaceEdge   (te2[nro], te1[nro]);
   for (int nro=0 ; nro<QUAD4 ; nro++)
       replaceVertex (tv2[nro], tv1[nro]);

   maj_connection = false;
   purge_elements = false;

   DumpEnd;
   return HOK;
}
// ======================================================== closeQuads
int Document::closeQuads (Quad* dest, Quad* orig)
{
   DumpStart ("closeQuads", dest << orig);

   update ();
   char nom[12];

   if (dest==orig)
      {
      printf (" *** Quads identiques dans closeQuads : %s\n",
                 dest->getName(nom));
      DumpEnd;
      return HERR;
      }
   else if (dest==NULL || dest->isDeleted())
      {
      printf (" *** Quad nro 1 incorrect dans closeQuads \n");
      DumpEnd;
      return HERR;
      }
   else if (orig==NULL || orig->isDeleted() )
      {
      printf (" *** Quad nro 2 incorrect dans closeQuads \n");
      DumpEnd;
      return HERR;
      }

   Edge* edc = NULL;
   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       int norig = orig->indexEdge (dest->getEdge(nro));
       if (norig != NOTHING)
          {
          if (edc != NULL)
             {
             printf ("  *** Plus d'une arete commune dans closeQuads");
             printf (" (%s,",  dest->getName (nom));
             printf (" %s)\n", orig->getName (nom));
             DumpEnd;
             return HERR;
             }
          edc = dest->getEdge (nro);
          printf ("  +++ quad1[%d] = quad2[%d] = %s\n", nro,  norig,
                                                        edc->getName (nom));
          }
       }

   Vertex* va = edc->getVertex (V_AMONT);
   Vertex* vb = edc->getVertex (V_AVAL);
   Vertex *tv1 [QUAD4], *tv2 [QUAD4];
   Edge   *te1 [QUAD4], *te2 [QUAD4];

   int ier1 = dest->ordoVertex (va, vb, tv1);
   int ier2 = orig->ordoVertex (va, vb, tv2);

   if (ier1 != HOK)
      {
      DumpEnd;
      return ier1;
      }
   else if (ier2 != HOK)
      {
      DumpEnd;
      return ier2;
      }

   for (int nro=0 ; nro<QUAD4 ; nro++)
       {
       te1 [nro] = dest->getEdge(nro);
       Vertex* va1 = te1[nro]->getVertex(V_AMONT);
       Vertex* vb1 = te1[nro]->getVertex(V_AVAL);
       int na = index_tv  (tv1, va1);
       int nb = index_tv  (tv1, vb1);
       if (na==NOTHING || nb==NOTHING)
          {
          DumpEnd;
          return HERR;
          }

       te2 [nro] = orig->findEdge (tv2[na], tv2[nb]);
       if (te2[nro]==NULL)
          {
          DumpEnd;
          return HERR;
          }
       }

   if (debug())
      {
      printf ("  ----------------- Correspondances mergeQuads : \n");
      for (int nro=0 ; nro<QUAD4 ; nro++)
          {
          printf ("  %d  : ", nro);
          tv2 [nro]->printName(" -> ");
          tv1 [nro]->printName("\n");
          }
      for (int nro=0 ; nro<QUAD4 ; nro++)
          {
          printf ("  %d  : ", nro);
          te2 [nro]->printName(" (");
          te2 [nro]->getVertex(0)->printName(", ");
          te2 [nro]->getVertex(1)->printName(") -> ");
          te1 [nro]->printName(" (");
          te1 [nro]->getVertex(0)->printName(", ");
          te1 [nro]->getVertex(1)->printName(")\n");
          }
      }

   replaceQuad (orig, dest);
   for (int nro=0 ; nro<QUAD4 ; nro++)
       replaceEdge   (te2[nro], te1[nro]);
   for (int nro=0 ; nro<QUAD4 ; nro++)
       replaceVertex (tv2[nro], tv1[nro]);

   maj_connection = false;
   purge_elements = false;

   DumpEnd;
   return HOK;
}
// ======================================================== mergeEdges
int Document::mergeEdges (Edge* e1, Edge* e2, Vertex* v1, Vertex* v2)
{
   DumpStart ("mergeEdges", e1 << e2 << v1 << v2);

   if (e1==NULL || e1->isDeleted() || e2==NULL || e2->isDeleted())
      {
      DumpEnd;
      return HERR;
      }

   for (int nro=0 ; nro<V_TWO ; nro++)
       if (e1->index (e2->getVertex(nro))!= NOTHING)
          {
          DumpEnd;
          return HERR+nro+1;
          }

   Vertex *tv1 [V_TWO], *tv2 [V_TWO];

   int ier1 = e1->anaMerge (v1, tv1);
   int ier2 = e2->anaMerge (v2, tv2);

   if (ier1 != HOK)
      {
      DumpEnd;
      return ier1;
      }
   else if (ier2 != HOK)
      {
      DumpEnd;
      return ier2;
      }

   replaceEdge (e2, e1);
   for (int nro=0 ; nro<V_TWO ; nro++)
       replaceVertex (tv2[nro], tv1[nro]);

   maj_connection = false;
   DumpEnd;
   return HOK;
}
// ======================================================== mergeVertices
int Document::mergeVertices (Vertex* v1, Vertex* v2)
{
   DumpStart ("mergeVertices", v1 << v2);

   if (v1==v2 || v1==NULL || v1->isDeleted()
              || v2==NULL || v2->isDeleted())
      {
      DumpEnd;
      return HERR;
      }

   replaceVertex (v2, v1);

   maj_connection = false;
   DumpEnd;
   return HOK;
}
// ======================================================== replaceVertex
void Document::replaceVertex (Vertex* old, Vertex* par)
{
   DumpStart ("replaceVertex", old << par);
   if (BadElement(old) || BadElement(par) ||  old==par)
      {
      DumpEnd;
      return;
      }

   // par->replaceAssociation (old);   TODO

   for (int type=EL_EDGE ; type <= EL_HEXA ; type++)
       {
       for (EltBase* elt = doc_first_elt[type]->next (); elt!=NULL;
                     elt = elt->next())
           if (elt->isHere ())
               elt->replaceVertex (old, par);
       }
   old->suppress ();
   DumpEnd;
}
// ======================================================== replaceEdge
void Document::replaceEdge (Edge* old, Edge* par)
{
   DumpStart ("replaceEdge", old << par);
   if (BadElement(old) || BadElement(par) ||  old==par)
      {
      DumpEnd;
      return;
      }

   // par->replaceAssociation (old);   TODO

   for (int type=EL_QUAD ; type <= EL_HEXA ; type++)
       {
       for (EltBase* elt = doc_first_elt[type]->next (); elt!=NULL;
                     elt = elt->next())
           if (elt->isHere ())
               elt->replaceEdge (old, par);
       }

   old->suppress ();
   DumpEnd;
}
// ======================================================== replaceQuad
void Document::replaceQuad (Quad* old, Quad* par)
{
   DumpStart ("replaceQuad", old << par);
   if (BadElement(old) || BadElement(par) ||  old==par)
      {
      DumpEnd;
      return;
      }

   // par->replaceAssociation (old);   TODO

   for (EltBase* elt = doc_first_elt[EL_HEXA]->next (); elt!=NULL;
                 elt = elt->next())
       if (elt->isHere ())
          {
          Hexa* cell = static_cast <Hexa*> (elt);
          cell->replaceQuad (old, par);
          }
   old->suppress ();
}
// ======================================================== prismQuad
Elements* Document::prismQuad  (Quad* qstart, Vector* dir, int nb)
{
   DumpStart ("prismQuad", qstart << dir << nb);

   Quads  tstart;
   tstart.push_back (qstart);

   update ();
   Elements* prisme = prismQuads (tstart, dir, nb);

   DumpReturn (prisme);
   return      prisme;
}
// ======================================================== prismQuads
Elements* Document::prismQuads (Quads& tstart, Vector* dir, int nb)
{
   DumpStart ("prismQuads", tstart << dir << nb);

   Elements*  prisme = new Elements (this);

   prisme->prismQuads (tstart, dir, nb);

   DumpReturn (prisme);
   return prisme;
}
// ======================================================== prismQuadsVec
Elements* Document::prismQuadsVec (Quads& tstart, Vector* dir, RealVector& tlen,
                                   int crit)
{
   DumpStart ("prismQuadVec", tstart << dir << tlen << crit);
   Elements*  prisme = new Elements (this);
   if (tlen.size()<=0) return prisme;

   prisme->prismQuadsVec (tstart, dir, tlen, crit);

   DumpReturn (prisme);
   return prisme;
}
// ======================================================== joinQuads
Elements* Document::joinQuads (Quads& tstart, Quad* dest, Vertex* v1,
                               Vertex* v2, Vertex* v3, Vertex* v4, int nb)
{
   DumpStart ("joinQuads", tstart << dest << v1 << v2 << v3 << v4 << nb);
   update ();
   Elements*  joint = new Elements (this);
   if (nb<=0)      return joint;

   int ier = joint->joinQuads (tstart, nb, v1, v2, v3, v4, dest);
   if (ier !=HOK)
      printf ("\n ****\n **** Error in joinQuad(s)\n ****\n");

   DumpReturn (joint);
   return joint;
}
// ======================================================== joinQuad
Elements* Document::joinQuad (Quad* qstart, Quad* dest, Vertex* v1,
                              Vertex* v2,  Vertex* v3, Vertex* v4, int nb)
{
   DumpStart ("joinQuad", qstart << dest << v1 << v2 << v3 << v4 << nb);

   Quads  tstart;
   tstart.push_back (qstart);

   Elements* joint = joinQuads (tstart, dest, v1, v2, v3, v4, nb);

   DumpReturn (joint);
   return    joint;
}
// ========================================================== getHexa
Hexa* Document::getHexa (int nro)
{
   return static_cast <Hexa*> (getElement (EL_HEXA, nro));
}
// ========================================================== getQuad
Quad* Document::getQuad (int nro)
{
   return static_cast <Quad*> (getElement (EL_QUAD, nro));
}
// ========================================================== getEdge
Edge* Document::getEdge (int nro)
{
   return static_cast <Edge*> (getElement (EL_EDGE, nro));
}
// ========================================================== getVertex
Vertex* Document::getVertex (int nro)
{
   return static_cast <Vertex*> (getElement (EL_VERTEX, nro));
}
// ========================================================== countElement
int Document::countElement (EnumElt type)
{
   int compteur = 0;
   for (EltBase* elt = doc_first_elt[type]->next (); elt!=NULL;
                 elt = elt->next())
       if (elt->isHere())
          compteur ++;

   return compteur;
}
// ========================================================== getElement
EltBase* Document::getElement (EnumElt type, int nro)
{
   int compteur = 0;
   for (EltBase* elt = doc_first_elt[type]->next (); elt!=NULL;
                 elt = elt->next())
       {
       if (elt->isHere())
          {
          if (compteur>=nro)
             return elt;
          compteur ++;
          }
       }
   return NULL;
}
// ========================================================= addLaw
Law* Document::addLaw (const char* name, int nbnodes)
{
   DumpStart ("addLaw", name << nbnodes);
   Law* loi = new Law (this, name, nbnodes);
   addLaw (loi);
   DumpReturn (loi);
   return loi;
}
// ========================================================= addLaw
Law* Document::addLaw (Law* loi)
{
   doc_laws.push_back (loi);
   nbr_laws ++;
   return loi;
}
// ========================================================= GetLaw
Law* Document::getLaw (int nro)
{
   if (nro <0 || nro>= nbr_laws)
      return NULL;

   return doc_laws [nro];
}
// ========================================================= FindLaw
Law* Document::findLaw (const char* name)
{
   string nom = name;
   for (int nro=0 ; nro<nbr_laws; nro++)
       if (doc_laws [nro]->getName() == nom)
          return doc_laws [nro];

   return NULL;
}
// ========================================================= removeLaw
int Document::removeLaw (Law* loi)
{
   for (int nro=1 ; nro<nbr_laws; nro++)
       if (doc_laws [nro] == loi)
          {
          //All propagations having this law should now have the default law.
          for (int nl=0 ; nl<nbr_propagations ; nl++)
              {
              if ( doc_propagation [nl]->getLaw() == loi )
                   doc_propagation [nl]->setLaw(defaultLaw);
              }

          delete doc_laws [nro];
          doc_laws.erase (doc_laws.begin()+nro);
          nbr_laws = doc_laws.size();
          return HOK;
          }

   return HERR;
}
// ========================================================= majPropagation
void Document::majPropagation ()
{
   DumpLock;
   majReferences ();
   update ();
   // if (purge_elements)
       // purge ();

   for (int nro=0 ; nro<nbr_propagations ; nro++)
       {
       delete doc_propagation [nro];
       }

   doc_propagation.clear ();
   nbr_propagations = 0;
   maj_propagation  = false;

   for (EltBase* elt = doc_first_elt[EL_EDGE]->next (); elt!=NULL;
                 elt = elt->next())
       {
       Edge* arete = static_cast <Edge*> (elt);
       if (arete!=NULL)
           arete->setPropag (NOTHING, true);
       }

   //  markAll (NO_COUNTED, EL_EDGE);

   for (EltBase* elt = doc_first_elt[EL_HEXA]->next (); elt!=NULL;
                 elt = elt->next())
       {
       Hexa* cell = static_cast <Hexa*> (elt);
       if (cell!=NULL && cell->isHere())
          {
          for (int ne=0 ; ne<HE_MAXI ; ne++)
              {
              Edge* arete = cell->getEdge(ne);
              if (arete->getPropag()<0)
                 {
                 Propagation* prop = new Propagation ();
                 doc_propagation.push_back (prop);
                 arete->propager (prop, nbr_propagations);
                 nbr_propagations ++;
                 prop->majLaw();
                 }
              }
          }
       }
   DumpRestore;
}
// ======================================================== countPropagation
int Document::countPropagation ()
{
   if (maj_propagation)
       majPropagation ();

   return nbr_propagations;
}
// ======================================================== getPropagation
Propagation* Document::getPropagation (int nro)
{
   if (maj_propagation)
       majPropagation ();

   if (nro < 0 || nro >= nbr_propagations)
       return NULL;

   return doc_propagation [nro];
}
// ======================================================== findPropagation
Propagation* Document::findPropagation (Edge* arete)
{
   if (arete==NULL)
       return NULL;

   if (maj_propagation)
       majPropagation ();

   return getPropagation (arete->getPropag ());
}
// ======================================================== disconnectQuad
Elements* Document::disconnectQuad (Hexa* cell, Quad* element)
{
   if (cell==NULL || element==NULL)
      return NULL;

   update ();
   Elements* crees = cell->disconnectQuad (element);

   if (crees!=NULL)
       majReferences ();

   return crees;
}
// ======================================================== disconnectEdge
Elements* Document::disconnectEdge (Hexa* cell, Edge* element)
{
   if (cell==NULL || element==NULL)
      return NULL;

   update ();
   Elements* crees = cell->disconnectEdge (element);

   if (crees!=NULL)
       majReferences ();
   return crees;
}
// ======================================================== disconnectVertex
Elements* Document::disconnectVertex (Hexa* cell, Vertex* element)
{
   if (cell==NULL || element==NULL)
      return NULL;

   update ();
   Elements* crees = cell->disconnectVertex (element);

   if (crees!=NULL)
       majReferences ();
   return crees;
}
// ======================================================== cut
Elements* Document::cut (Edge* edge, int nbcuts)
{
   Elements* t_hexas = new Elements (this);

   if (edge==NULL || nbcuts<=0)
      return t_hexas;

   Propagation* prop    = findPropagation (edge);
   const Edges& t_edges = prop->getEdges ();

   t_hexas->cutHexas (t_edges, nbcuts);

   majPropagation ();
   return t_hexas;
}
// ======================================================== addGroup
Group* Document::addGroup (cpchar name, EnumGroup kind)
{
   Group* grp = new Group (name, kind);
   doc_group.push_back (grp);
   return grp;
}
// ======================================================== findGroup
Group* Document::findGroup (cpchar name)
{
   int nbre = doc_group.size();

   for (int ng=0 ; ng<nbre ; ng++)
       if (Cestegal (doc_group [ng]->getName(), name))
          return doc_group [ng];

   return NULL;
}
// ======================================================== removeGroup
int Document::removeGroup (Group* grp)
{
   int nbre = doc_group.size();
   for (int ng=0 ; ng<nbre ; ng++)
       {
       if (grp == doc_group [ng])
          {
          doc_group.erase (doc_group.begin() + ng);
          delete grp;
          return HOK;
          }
       }
                      // Pas trouve dans la liste. On detruit quand meme
   delete grp;
   return HERR;
}
// ======================================================== makeCylinder
Elements* Document::makeCylinder (Cylinder* cyl, Vector* base, int nr, int na,
                                                                       int nl)
{
   DumpStart ("makeCylinder", cyl << base << nr << na << nl);

   Elements* grille = new Elements (this);
   grille->makeCylinder (cyl, base, nr, na, nl);

   DumpReturn (grille);
   return grille;
}
// ======================================================== makeCylinders
CrossElements* Document::makeCylinders (Cylinder* cyl1, Cylinder* cyl2)
{
   DumpStart ("makeCylinders", cyl1 << cyl2 );

   CrossElements* grille = new CrossElements (this, GR_BICYL);
   grille->crossCylinders (cyl1, cyl2, true);

   DumpReturn (grille);
   return grille;
}

// ======================================================== makePipe
Elements* Document::makePipe (Pipe* pipe, Vector* bx, int nr, int na, int nl)
{
   DumpStart ("makePipe",
        pipe << bx << nr << na << nl);

   Elements* grille = new Elements (this);
   grille->makePipe (pipe, bx, nr, na, nr);

   DumpReturn (grille);
   return grille;
}
// ======================================================== makePipes
CrossElements* Document::makePipes (Pipe* pipe1, Pipe* pipe2)
{
   DumpStart ("makePipes",
        pipe1 << pipe2);

   CrossElements* grille = new CrossElements (this, GR_BIPIPE);
   grille->crossCylinders (pipe1, pipe2, false);

   DumpReturn (grille);
   return grille;
}
// ======================================================== setName
int Document::setName (cpchar prefix)
{
   if (Cestvide (prefix) || el_name==prefix)
       return HERR;

   string name = prefix;
   if (hex_parent != NULL)
       hex_parent->makeName (prefix, name);

   el_name = name;
   return HOK ;
}
// ======================================================== setLevel

#ifdef  NO_CASCADE
#define _TEST_BIC
#endif

class BiCylinder;

BiCylinder* test_bicylinder (Document* docu, int option);
void        set_debug_asso  (bool boule);

void Document::setLevel (int niv)
{
   if (niv == 747)
      checkAssociations ();
   else if (niv == 748)
      {
      int nbshapes = countShape ();
      for (int nsh=0 ; nsh < nbshapes ; nsh++)
          {
          doc_tab_shape[nsh]->saveBrep();
          }
      }
   else if (niv == 777)
      set_special_option (true);
   else if (niv == 778)
      set_special_option (false);
#ifdef _TEST_BICYL
   else if (niv >=90 && niv <=99)
      test_bicylinder (this, niv-90);
#endif
   else
      {
      doc_db = niv;
      set_debug_asso (niv>0);
      }
}
#if 0
// ======================================================== getAssoVertices
void Document::getAssoVertices (Vertices& tabelt)
{
   tabelt.clear ();
   for (EltBase* elt = doc_first_elt[EL_VERTEX]->next (); elt!=NULL;
                 elt = elt->next())
       if (elt->isHere() && elt->getAssociation()!=NULL)
          tabelt.push_back (static_cast <Vertex*> (elt));
}
// ======================================================== getAssoEdges
void Document::getAssoEdges (Edges& tabelt)
{
   tabelt.clear ();
   for (EltBase* elt = doc_first_elt[EL_EDGE]->next (); elt!=NULL;
                 elt = elt->next())
       if (elt->isHere())
          {
          Edge* edge = static_cast <Edge*> (elt);
          int nbass  = edge->getAssociations().size();
          if (nbass>0)
              tabelt.push_back (edge);
          }
}
#endif
// ======================================================== makeVarName
char* Document::makeVarName (char* name)
{
   strcpy (name, doc_ident);
   return doc_ident ;
}
// ======================================================== isEmpty
bool Document::isEmpty ()
{
   int nombre = countVertex () + countVector ()
                               + countCylinder() + countPipe();
   return nombre <= 0 && countLaw  () <= 1;
}
// ======================================================== getNextName
cpchar Document::getNextName (EnumElt type, string& buff)
{
   char name [8];
   EltBase::makeName (type, doc_nbr_elt [type], name);

   buff = name;
   return buff.c_str();
}

// ======================================================== getNextName
string Document::getNextName (EnumElt type)
{
   char name [8];
   EltBase::makeName (type, doc_nbr_elt [type], name);

   return string(name);
}

// ======================================================== lockDump
void Document::lockDump ()
{
   glob->dump.lock ();
}
// ======================================================== restoreDump
void Document::restoreDump ()
{
   glob->dump.restore (DumpActif);
}
// ======================================================== getGroup
Group* Document::getGroup (int nro)
{
   int size = doc_group.size();
   if (nro>=0 && nro<size)
      return doc_group [nro];
   else
      return NULL;
}
// ======================================================== getVector
Vector* Document::getVector (int nro)
{
   int size = doc_vector.size();
   if (nro>=0 && nro<size)
      return doc_vector [nro];
   else
      return NULL;
}
// ======================================================== getCylinder
Cylinder* Document::getCylinder (int nro)
{
   int size = doc_cylinder.size();
   if (nro>=0 && nro<size)
      return doc_cylinder [nro];
   else
      return NULL;
}
// ======================================================== getPipe
Pipe* Document::getPipe (int nro)
{
   int size = doc_pipe.size();
   if (nro>=0 && nro<size)
      return doc_pipe [nro];
   else
      return NULL;
}

// ======================================================== makeBiCylinder
BiCylinder* Document::makeBiCylinder (Cylinder* cyl1, Cylinder* cyl2)
{
   DumpStart ("makeBiCylinder", cyl1 << cyl2 );

   BiCylinder* grille = new BiCylinder (this);
   grille->crossCylinders (cyl1, cyl2);

   DumpReturn (grille);
   return grille;
}
// ======================================================== makeBiPipe
BiCylinder* Document::makeBiPipe (Pipe* pipe1, Pipe* pipe2)
{
   DumpStart ("makeBiPipe", pipe1 << pipe2);

   BiCylinder* grille = new BiCylinder (this);
   grille->crossCylinders (pipe1, pipe2);

   DumpReturn (grille);
   return grille;
}
END_NAMESPACE_HEXA
