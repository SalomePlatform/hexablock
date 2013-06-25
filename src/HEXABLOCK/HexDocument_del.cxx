
// C++ : Fonctions perimees de HexDocument

//  Copyright (C) 2009-2013  CEA/DEN, EDF R&D
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
Elements* Document::prismQuads (Quads tstart, Vector* dir, int nb)
{
   DumpStart ("prismQuads", tstart << dir << nb);

   Elements*  prisme = new Elements (this);

   prisme->prismQuads (tstart, dir, nb);

   DumpReturn (prisme);
   return prisme;
}
// ======================================================== prismQuadsVec
Elements* Document::prismQuadsVec (Quads tstart, Vector* dir, RealVector tlen,
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
Elements* Document::joinQuads (Quads tstart, Quad* dest, Vertex* v1,
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
// ======================================================== revolutionQuads
Elements* Document::revolutionQuadsVec (Quads start, Vertex* orig, Vector* axis,
                                        RealVector angles)
{
   if (orig==NULL)       return NULL;
   if (axis==NULL)       return NULL;
   if (angles.size()==0) return NULL;
   if (start .size()==0) return NULL;

   Elements*  prisme = new Elements (this);
   prisme->revolutionQuads (start, orig, axis, angles);
   return prisme;
}
// ======================================================== makeCylindricals
Elements* Document::makeCylindricals (Vertex* c, Vector* b, Vector* h, 
                 RealVector tdr, RealVector tda, RealVector tdl, bool fill)
{
   Elements* grille = new Elements (this);
   grille->makeCylindricalGrid (c, b, h, tdr, tda, tdl, fill);
   return grille;
}
END_NAMESPACE_HEXA
