
// C++ : Table d'hexaedres

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

#include "HexElements.hxx"

#include "HexDocument.hxx"
#include "HexVector.hxx"
#include "HexVertex.hxx"
#include "HexHexa.hxx"
#include "HexEdge.hxx"

#include "HexGlobale.hxx"
#include "HexCylinder.hxx"
#include "HexOldShape.hxx"

#include <map>

BEGIN_NAMESPACE_HEXA

void geom_dump_asso     (Edge* edge);
void geom_create_circle (double* milieu, double rayon, double* normale,
                         double* base, string& brep);

// ====================================================== getHexaIJK
Hexa* Elements::getHexaIJK (int nx, int ny, int nz)
{
   if (isBad())
      return NULL;
   if (nx<0 || nx>=size_hx ||  ny<0 || ny>=size_hy || nz<0 || nz>=size_hz)
      return NULL;
   else if (grid_nocart)
      return NULL;

   int nro = nx + size_hx*ny + size_hx*size_hy*nz;

   DumpStart  ("getHexaIJK", nx << ny << nz);
   DumpReturn (tab_hexa [nro]);
   return      tab_hexa [nro];
}
// ====================================================== getQuadIJ
Quad* Elements::getQuadIJ (int nx, int ny, int nz)
{
   if (isBad())
      return NULL;
   if (nx<0 || nx>=size_qx ||  ny<0 || ny>=size_qy || nz<0 || nz>=size_qz)
      return NULL;
   else if (grid_nocart)
      return NULL;

   int nro = nx + size_qx*ny + size_qx*size_qy*nz
                + size_qx*size_qy*size_qz*dir_z;

   DumpStart  ("getQuadIJ", nx << ny << nz);
   DumpReturn (tab_quad [nro]);
   return tab_quad [nro];
}
// ====================================================== getQuadJK
Quad* Elements::getQuadJK (int nx, int ny, int nz)
{
   if (isBad())
      return NULL;
   if (nx<0 || nx>=size_qx ||  ny<0 || ny>=size_qy || nz<0 || nz>=size_qz)
      return NULL;
   else if (grid_nocart)
      return NULL;

   int nro = nx + size_qx*ny + size_qx*size_qy*nz; // + dir_x*...

   DumpStart  ("getQuadJK", nx << ny << nz);
   DumpReturn (tab_quad [nro]);
   return tab_quad [nro];
}
// ====================================================== getQuadIK
Quad* Elements::getQuadIK (int nx, int ny, int nz)
{
   if (isBad())
      return NULL;
   if (nx<0 || nx>=size_qx ||  ny<0 || ny>=size_qy || nz<0 || nz>=size_qz)
      return NULL;
   else if (grid_nocart)
      return NULL;

   int nro = nx + size_qx*ny + size_qx*size_qy*nz + size_qx*size_qy*size_qz;

   DumpStart  ("getQuadIK", nx << ny << nz);
   DumpReturn (tab_quad [nro]);
   return tab_quad [nro];
}
// ====================================================== getEdgeI
Edge* Elements::getEdgeI (int nx, int ny, int nz)
{
   if (isBad())
      return NULL;
   if (nx<0 || nx>=size_ex ||  ny<0 || ny>=size_ey || nz<0 || nz>=size_ez)
      return NULL;
   else if (grid_nocart)
      return NULL;

   int nro = nx + size_ex*ny + size_ex*size_ey*nz;

   DumpStart  ("getEdgeI", nx << ny << nz);
   DumpReturn (tab_edge [nro]);
   return tab_edge [nro];
}
// ====================================================== getEdgeJ
Edge* Elements::getEdgeJ (int nx, int ny, int nz)
{
   if (isBad())
      return NULL;
   if (nx<0 || nx>=size_ex ||  ny<0 || ny>=size_ey || nz<0 || nz>=size_ez)
      return NULL;
   else if (grid_nocart)
      return NULL;

   int nro = nx + size_ex*ny + size_ex*size_ey*nz + size_ex*size_ey*size_ez;

   DumpStart  ("getEdgeJ", nx << ny << nz);
   DumpReturn (tab_edge [nro]);
   return tab_edge [nro];
}
// ====================================================== getEdgeK
Edge* Elements::getEdgeK (int nx, int ny, int nz)
{
   if (isBad())
      return NULL;
   if (nx<0 || nx>=size_ex ||  ny<0 || ny>=size_ey || nz<0 || nz>=size_ez)
      return NULL;
   else if (grid_nocart)
      return NULL;

   int nro = nx + size_ex*ny + size_ex*size_ey*nz
                + size_ex*size_ey*size_ez*dir_z;

   DumpStart  ("getEdgeK", nx << ny << nz);
   DumpReturn (tab_edge [nro]);
   return tab_edge [nro];
}
// ====================================================== getVertexIJK
Vertex* Elements::getVertexIJK (int nx, int ny, int nz)
{
   if (isBad())
      return NULL;
   if (nx<0 || nx>=size_vx ||  ny<0 || ny>=size_vy || nz<0 || nz>=size_vz)
      return NULL;
   else if (grid_nocart)
      return NULL;

   int nro = nx + size_vx*ny + size_vx*size_vy*nz;

   DumpStart  ("getVertexIJK", nx << ny << nz);
   DumpReturn (tab_vertex [nro]);
   return tab_vertex [nro];
}
// ====================================================== setVertex
void Elements::setVertex (Vertex* elt, int nx, int ny, int nz)
{
   if (   nx < 0 || nx >= size_vx || ny < 0 || ny >= size_vy
       || nz < 0 || nz >= size_vz) return;

   int nro = nx + size_vx*ny + size_vx*size_vy*nz;
   tab_vertex [nro] = elt;
}
// ====================================================== setVertex (2)
void Elements::setVertex (int nx, int ny, int nz, double px, double py,
                                                  double pz)
{
   if (   nx < 0 || nx >= size_vx || ny < 0 || ny >= size_vy
       || nz < 0 || nz >= size_vz) return;

   Vertex*    node = el_root->addVertex (px, py, pz);
   setVertex (node, nx, ny, nz);
}
// ====================================================== setEdge
void Elements::setEdge (Edge* elt, EnumCoord dir, int nx, int ny, int nz)
{
   if (nx<0 || nx>=size_ex ||  ny<0 || ny>=size_ey || nz<0 || nz>=size_ez
            || dir < dir_x || dir > dir_z )
      return;

   int nro = nx + size_ex*ny + size_ex*size_ey*nz + size_ex*size_ey*size_ez*dir;
   tab_edge [nro] = elt;
}
// ====================================================== setQuad
void Elements::setQuad (Quad* elt, EnumCoord dir, int nx, int ny, int nz)
{
   if (nx<0 || nx>=size_ex ||  ny<0 || ny>=size_ey || nz<0 || nz>=size_ez
            || dir < dir_x || dir > dir_z )
      return;

   int nro = nx + size_ex*ny + size_ex*size_ey*nz + size_ex*size_ey*size_ez*dir;
   tab_quad [nro] = elt;
}
// ====================================================== setHexa
void Elements::setHexa (Hexa* elt, int nx, int ny, int nz)
{
   if (   nx < 0 || nx >= size_hx || ny < 0 || ny >= size_hy
       || nz < 0 || nz >= size_hz) return;

   int nro = nx + size_hx*ny + size_hx*size_hy*nz;
   tab_hexa [nro] = elt;
}
// ====================================================== remove
void Elements::remove ()
{
   int nbre=tab_hexa.size ();
   nbre = nbr_hexas;
   for (int nh=0 ; nh<nbre ; nh++)
       if (tab_hexa[nh] != NULL)
           tab_hexa[nh]->remove();
}
// ====================================================== makeCylinder
int Elements::makeCylinder (Cylinder* cyl, Vector* vx, int nr, int na, int nl)
{
   if (BadElement (cyl) || BadElement (vx) ||  nr<=0 || na <=3 || nl <=0
                        || vx->getNorm () <= Epsil)
      {
      setError ();
      return HERR;
      }

   Vertex* orig = cyl->getBase ();
   Vector* dir  = cyl->getDirection ();
   double  ray  = cyl->getRadius ();
   double  haut = cyl->getHeight ();

   resize (GR_CYLINDRIC, nr, na, nl);
   cyl_closed = true;
   makeCylindricalNodes (orig, vx, dir, ray/(nr+1), 360, haut/nl,
                         nr, na, nl, true);
   fillGrid ();
   assoCylinder (orig, dir, 360);
   return HOK;
}
// ====================================================== makePipe
int Elements::makePipe (Cylinder* cyl, Vector* vx, int nr, int na, int nl)
{
   if (BadElement (cyl) || BadElement (vx) ||  nr<=0 || na <=3 || nl <=0
                        || vx->getNorm () <= Epsil)
      {
      setError ();
      return HERR;
      }

   Vertex* orig = cyl->getBase ();
   Vector* dir  = cyl->getDirection ();
   double  ray  = cyl->getRadius ();
   double  haut = cyl->getHeight ();

   resize (GR_CYLINDRIC, nr, na, nl);
   cyl_closed = true;
   makeCylindricalNodes (orig, vx, dir, ray, 360, haut, nr, na, nl, false);
   fillGrid ();
   assoCylinder (orig, dir, 360);
   return HOK;
}
//
// ---------------------------------------- prism Quads
//
// ====================================================== prismQuads
int Elements::prismQuads (Quads& tstart, Vector* dir, int nbiter)
{
   if (BadElement (dir) || dir->getNorm () <= Epsil || nbiter <= 0)
      {
      setError ();
      return HERR;
      }

   el_root->markAll (NO_USED);
   int nbcells   = tstart.size ();
   nbr_vertex    = 0;
   nbr_edges     = 0;

   nbr_hexas = nbcells*nbiter;

   tab_hexa.resize (nbr_hexas);
   tab_quad.clear ();          // verticaux
   ker_hquad.clear ();         // Horizontaux
   tab_edge.clear ();
   tab_pilier.clear ();
   tab_vertex.clear ();

   revo_lution = false;
   prism_vec   = false;
   gen_matrix.defTranslation (dir);

   for (int nro=0 ; nro<nbcells ; nro++)
       {
       prismHexas (nro, tstart[nro], nbiter);
       }

   endPrism ();
   return HOK;
}
// ====================================================== prismQuadsVec
int Elements::prismQuadsVec (Quads& tstart, Vector* dir, RealVector& tlen,
                             int mode)
{
   int nbiter = tlen.size();
   if (BadElement (dir) || dir->getNorm () <= Epsil || nbiter <= 0)
      {
      setError ();
      return HERR;
      }

   el_root->markAll (NO_USED);
   int nbcells   = tstart.size ();
   nbr_vertex    = 0;
   nbr_edges     = 0;

   nbr_hexas = nbcells*nbiter;

   tab_hexa.resize (nbr_hexas);
   tab_quad.clear ();          // verticaux
   ker_hquad.clear ();         // Horizontaux
   tab_edge.clear ();
   tab_pilier.clear ();
   tab_vertex.clear ();

   revo_lution = false;
   prism_vec   = true;
   dir->getCoord  (prism_dir);
   normer_vecteur (prism_dir);
   gen_values = tlen;

   for (int nro=0 ; nro<nbcells ; nro++)
       {
       prismHexas (nro, tstart[nro], nbiter);
       }

   endPrism ();
   return HOK;
}
// ======================================================== revolutionQuads
int Elements::revolutionQuads (Quads& start, Vertex* center, Vector* axis,
                               RealVector &angles)
{
   int nbiter  = angles.size();
   int nbcells = start.size ();
   if (BadElement (center)  || BadElement(axis) || nbiter==0 || nbcells==0
                            || axis->getNorm () <= Epsil)
      {
      setError ();
      return HERR;
      }

   el_root->markAll (NO_USED);
   nbr_vertex    = 0;
   nbr_edges     = 0;

   nbr_hexas   = nbcells*nbiter;

   tab_hexa.resize (nbr_hexas);
   tab_quad.clear ();          // verticaux
   ker_hquad.clear ();         // Horizontaux
   tab_edge.clear ();
   tab_pilier.clear ();
   tab_vertex.clear ();

   revo_lution  = true;
   prism_vec    = false;
   revo_axis    = axis;
   revo_center  = center;
   gen_values = angles;

   for (int nro=0 ; nro<nbcells ; nro++)
       {
       prismHexas (nro, start[nro], nbiter);
       }

   endPrism ();
   return HOK;
}
// ====================================================== prismHexas
int  Elements::prismHexas (int nro, Quad* qbase, int hauteur)
{
   int ind_node [QUAD4];
   string c_rep;

           // ----------------------------- Vertex + aretes verticales
   for (int ns=0 ; ns<QUAD4 ; ns++)
       {
       Vertex* vbase = qbase ->getVertex (ns);
       int     indx  = vbase->getMark ();
       if (indx<0)
          {
          indx = nbr_vertex++;
          vbase->setMark (indx);
          Vertex* nd0 = vbase;
          Vertex* nd1 = NULL;
          double beta = 0;
          if (revo_lution)
             {
             Real3 centre, vk, point, om;
             revo_center->getPoint (centre);
             vbase      ->getPoint (point);
             revo_axis  ->getCoord (vk);
             normer_vecteur (vk);

             calc_vecteur   (centre, point, om);
             double oh     = prod_scalaire (om, vk);
             double rayon  = 0;
             Real3  ph, hm;
             for (int dd=dir_x; dd<=dir_z ; dd++)
                 {
                 ph [dd] = centre [dd] + oh*vk[dd];
                 hm [dd] = point  [dd] - ph[dd];
                 rayon  += hm[dd] * hm[dd];
                 }
             rayon = sqrt (rayon);
/********************************
             PutCoord (centre);
             PutCoord (point);
             PutData  (oh);
             PutCoord (ph);
             PutData  (rayon);
             PutCoord (vk);
             PutCoord (hm);
********************************/
             geom_create_circle (ph, rayon, vk, hm, c_rep);
             }

          for (int nh=0 ; nh<hauteur ; nh++)
              {
              nd1 = el_root->addVertex (nd0->getX(), nd0->getY(), nd0->getZ());
              updateMatrix (nh);
              gen_matrix.perform   (nd1);
              tab_vertex.push_back (nd1);
              Edge* pilier = newEdge (nd0, nd1);
              tab_pilier.push_back (pilier);
              if (revo_lution)
                 {
                 double alpha = beta;
                 beta = alpha + gen_values[nh];
                 Shape* shape = new Shape (c_rep);
                 shape->setBounds (alpha/360, beta/360);
                 pilier->addAssociation (shape);
                 //      geom_dump_asso (pilier);
                 }
              nd0 = nd1;
              }
          }
       ind_node [ns] = indx;
       }
           // ----------------------------- Aretes horizontales
           // ----------------------------- + face verticales
   int ind_poutre [QUAD4];
   for (int ns=0 ; ns<QUAD4 ; ns++)
       {
       Edge* ebase = qbase->getEdge (ns);
       int   indx  = ebase->getMark ();
       if (indx<0)
          {
          indx = nbr_edges ++;
          ebase->setMark (indx);
          int   nd1 = ind_node [ns];
          int   nd2 = ind_node [(ns+1) MODULO QUAD4];
          Edge* ed0 = ebase;
          Edge *ed1, *ed2, *ed3;
          for (int nh=0 ; nh<hauteur ; nh++)
              {
              ed2 = ed0;
              ed0 = newEdge (tab_vertex [nd1*hauteur + nh],
                             tab_vertex [nd2*hauteur + nh]);
              ed1 = tab_pilier [nd1*hauteur + nh];
              ed3 = tab_pilier [nd2*hauteur + nh];

              Quad* mur = newQuad (ed0, ed1, ed2, ed3);
              tab_edge.push_back (ed0);
              tab_quad.push_back (mur);
              prismAssociation (ed2, ed0, nh, ed1);
              }
          }
       ind_poutre [ns] = indx;
       }
           // ----------------------------- Faces horizontales
           // ----------------------------- + Hexas
   Quad* qa = qbase;
   Quad *qb, *qc, *qd, *qe, *qf;
   int nv0 = hauteur*ind_poutre [0];
   int nv1 = hauteur*ind_poutre [1];
   int nv2 = hauteur*ind_poutre [2];
   int nv3 = hauteur*ind_poutre [3];
   for (int nh=0 ; nh<hauteur ; nh++)
       {
       qb = newQuad (tab_edge [nh+nv0], tab_edge [nh+nv1],
                     tab_edge [nh+nv2], tab_edge [nh+nv3]);
       qc = tab_quad [nh + nv0];
       qd = tab_quad [nh + nv2];
       qe = tab_quad [nh + nv1];
       qf = tab_quad [nh + nv3];

// *** tab_hexa [nh*hauteur + nro] = newHexa (qa, qb, qc, qd, qe, qf); Abu
       tab_hexa [nro*hauteur + nh] = newHexa (qa, qb, qc, qd, qe, qf);
       ker_hquad.push_back (qb);
       qa = qb;
       }
   return HOK;
}
// ====================================================== updateMatrix
void Elements::updateMatrix (int hauteur)
{
   if (revo_lution)
      {
      gen_matrix.defRotation (revo_center, revo_axis, gen_values[hauteur]);
      }
   else if (prism_vec)
      {
      double h0 = hauteur>0 ?  gen_values[hauteur-1] : 0;
      double dh = gen_values[hauteur] - h0;
      Real3 decal;
      for (int nc=dir_x ; nc<=dir_z ; nc++)
          decal [nc] = prism_dir [nc]*dh;
      gen_matrix.defTranslation (decal);
      }
}
// ====================================================== endPrism
void Elements::endPrism ()
{
   int nbelts = ker_hquad.size();
   for (int nro=0 ; nro<nbelts ; nro++)
       tab_quad.push_back (ker_hquad[nro]);

   nbelts = tab_pilier.size();
   for (int nro=0 ; nro<nbelts ; nro++)
       tab_edge.push_back (tab_pilier[nro]);


   nbr_hexas  = tab_hexa.size ();
   nbr_edges  = tab_edge.size ();
   nbr_quads  = tab_quad.size ();
   nbr_vertex = tab_vertex.size ();
}
END_NAMESPACE_HEXA
