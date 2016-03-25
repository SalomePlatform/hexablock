
// C++ : Grilles hexa 6

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

#include "HexElements.hxx"
#include "HexGlobale.hxx"
#include "HexDocument.hxx"
#include "HexVector.hxx"
#include "HexVertex.hxx"
#include "HexHexa.hxx"
#include "HexEdge.hxx"
#include "HexPropagation.hxx"

#include <cmath>
#include <map>

BEGIN_NAMESPACE_HEXA

static bool db=false;
double calcul_phimax (double radhole, double radext, bool sphere);

// ====================================================== makeCartesian
int Elements::makeCartesian (Vertex* orig, double* ux, double* uy, double* uz,
                             RealVector& tx, RealVector& ty, RealVector& tz)
{
   resize (GR_CARTESIAN, tx.size(), ty.size(), tz.size());

   double dx, dy, dz;
   double kx, ky, kz;
   double px0 = orig->getX ();
   double py0 = orig->getY ();
   double pz0 = orig->getZ ();

   setVertex (orig, 0, 0, 0);
   for (int nz=0 ; nz<size_vz ; nz++)
       {
       kz = nz==0 ? 0 : tz [nz-1]; 
       for (int ny=0 ; ny<size_vy ; ny++)
           {
           ky = ny==0 ? 0 : ty [ny-1]; 
           for (int nx=0 ; nx<size_vx ; nx++)
               {
               kx = nx==0 ? 0 : tx [nx-1]; 
               dx = px0 + kx*ux[dir_x] + ky*uy[dir_x] + kz*uz[dir_x];
               dy = py0 + kx*ux[dir_y] + ky*uy[dir_y] + kz*uz[dir_y];
               dz = pz0 + kx*ux[dir_z] + ky*uy[dir_z] + kz*uz[dir_z];

               Vertex* node = orig;
               if (nx+ny+nz !=0)
                  node = el_root->addVertex (dx, dy, dz);
               setVertex (node, nx, ny, nz);
               }
           }
       }
   fillGrid ();
   return HOK;
}
// ====================================================== makeCylinder
int Elements::makeCylinder (Vertex* vorig, double* base, double* haut,
                         RealVector& tray, RealVector& tang, RealVector& thaut,
                            bool fill)
{
   Real3 orig = { 0, 0, 0 };
   if (vorig!=NULL)
       vorig->getPoint (orig);

   int nr = tray .size() - 1;
   int na = tang .size() - 1;
   int nl = thaut.size() - 1;
   double angle = tang[na];

   resize (GR_CYLINDRIC, nr, na, nl);
   cyl_closed = angle >= 359.9;

   val_absolues = true;
   int ier = makeBasicCylinder (tray, tang, thaut, fill);
   if (ier!=HOK) 
       return ier;

   transfoVertices  (orig,  base, haut);

   fillGrid ();
   assoCylinders (orig, haut, angle, tang);
   return HOK;
}
// ====================================================== transfoVertices
void Elements::transfoVertices (double* omega, double* base, double* haut)
{
   Matrix matrice;
   Real3  iprim, jprim;

   prod_vectoriel (haut,  base, jprim);
   prod_vectoriel (jprim, haut, iprim);

   matrice.define (omega, iprim, jprim, haut);
   int nbre = tab_vertex.size ();
   for (int nro=0 ; nro<nbre ; nro++)
       {
       if (tab_vertex[nro] != NULL)
           tab_vertex[nro]->setMark (NO_USED);
       }

   for (int nro=0 ; nro<nbre ; nro++)
       {
       Vertex* node =  tab_vertex[nro];
       if (node != NULL && node->getMark() == NO_USED)
          {
          matrice.perform (node);
          node->setMark (IS_USED);
          }
       }
}
// ====================================================== makeSpherical
int Elements::makeSpherical (double* center, double* base, double* haut, 
                             RealVector& trayon, int critere)
{
   Real3  iprim, jprim;
   prod_vectoriel (haut,  base, jprim);
   prod_vectoriel (jprim, haut, iprim);

   int nbre = trayon.size();
   resize (GR_SPHERIC, nbre);

   Vertex* i_node [HV_MAXI];    // Les noeuds de l'hexa englobant
   Edge*   i_edge [HE_MAXI];    // Les noeuds de l'hexa englobant
   Quad*   i_quad [HQ_MAXI];    // Les noeuds de l'hexa englobant

   double rayon = trayon [0];
   for (int nro=0 ; nro<HV_MAXI; nro++)
       {
       double dx = glob->CoordVertex (nro, dir_x) * rayon;
       double dy = glob->CoordVertex (nro, dir_y) * rayon;
       double dz = glob->CoordVertex (nro, dir_z) * rayon;

       double px = center[0] + dx*iprim[0] + dy*jprim[0] + dz*haut[0] ;
       double py = center[1] + dx*iprim[1] + dy*jprim[1] + dz*haut[1] ;
       double pz = center[2] + dx*iprim[2] + dy*jprim[2] + dz*haut[2] ;

       i_node [nro] = el_root->addVertex (px, py, pz);
       }

   for (int nro=0 ; nro<HE_MAXI; nro++)
       {
       int v1 = glob->EdgeVertex (nro, V_AMONT);
       int v2 = glob->EdgeVertex (nro, V_AVAL);
       i_edge[nro] = newEdge (i_node[v1], i_node[v2]);

       if (db)
          {
          char nm0[8], nm1 [8], nm2 [8];
          printf (" %2d : %s = %s = [%s, %s] = [%d,%d] = [%s,%s]\n", nro,
                 glob->namofHexaEdge(nro), i_edge[nro]->getName(nm0),
                 glob->namofHexaVertex(v1), glob->namofHexaVertex(v2), v1, v2,
                 i_node[v1]->getName(nm1), i_node[v2]->getName(nm2));
          }
       }

   for (int nro=0 ; nro<HQ_MAXI; nro++)
       i_quad[nro] = newQuad (i_edge[glob->QuadEdge (nro, E_A)],
                              i_edge[glob->QuadEdge (nro, E_B)],
                              i_edge[glob->QuadEdge (nro, E_C)],
                              i_edge[glob->QuadEdge (nro, E_D)]);

   tab_hexa.push_back (newHexa (i_quad[Q_A], i_quad[Q_B], i_quad[Q_C],
                                i_quad[Q_D], i_quad[Q_E], i_quad[Q_F]));

   for (int niv=1; niv<gr_rayon ; niv++)
       {
       double rayon0  = rayon;
       rayon  = trayon [niv];
       addStrate (i_quad, i_edge, i_node, center, rayon/rayon0);
       }

   nbr_hexas = tab_hexa.size();
   assoSphere (center, i_edge, i_quad);
   return HOK;
}
// ====================================================== addStrate
int Elements::addStrate (Quad* i_quad[], Edge* i_edge[], Vertex* i_node[],
                         Real* center,  double lambda)
{
   Vertex* e_node [HV_MAXI];    // Les noeuds de l'hexa englobant
   Edge*   e_edge [HE_MAXI];    // Les noeuds de l'hexa englobant
   Quad*   e_quad [HQ_MAXI];    // Les noeuds de l'hexa englobant

   Edge*   d_edge [HV_MAXI];    // Les aretes diagonales (1 par sommet)
   Quad*   d_quad [HE_MAXI];    // Les faces  diagonales (1 par arete)

                                          // Les sommets
                                          //  + les aretes diagonales
   double px0 = center [dir_x];
   double py0 = center [dir_y];
   double pz0 = center [dir_z];

   for (int nv=0 ; nv<HV_MAXI ; nv++)
       {
       e_node[nv] = el_root->addVertex (px0+lambda*(i_node[nv]->getX()-px0),
                                        py0+lambda*(i_node[nv]->getY()-py0),
                                        pz0+lambda*(i_node[nv]->getZ()-pz0));

       d_edge[nv] = newEdge (i_node[nv], e_node[nv]);
       }
                                          // Les aretes exterieures
                                          //  + les faces diagonales
   for (int nro=0 ; nro<HE_MAXI ; nro++)
       {
       int nv0  = glob->EdgeVertex (nro, V_AMONT);
       int nv1  = glob->EdgeVertex (nro, V_AVAL );
       e_edge[nro] = newEdge (e_node [nv0], e_node [nv1]);
       d_quad[nro] = newQuad (i_edge [nro], d_edge [nv0],
                              e_edge [nro], d_edge [nv1]);
       }
                                          // Les faces exterieures
                                          //  + les hexas
   Hexa* strate = NULL;
   for (int nro=0 ; nro<HQ_MAXI ; nro++)
       {
       int ne0 = glob->QuadEdge (nro, E_A);
       int ne1 = glob->QuadEdge (nro, E_B);
       int ne2 = glob->QuadEdge (nro, E_C);
       int ne3 = glob->QuadEdge (nro, E_D);

       e_quad[nro] = newQuad (e_edge[ne0], e_edge[ne1],
                              e_edge[ne2], e_edge[ne3]);
       strate  = newHexa (i_quad[nro], e_quad[nro], d_quad[ne0],
                          d_quad[ne2], d_quad[ne1], d_quad[ne3]);
       tab_hexa.push_back (strate);
       }

   for (int nv=0 ; nv<HV_MAXI ; nv++) i_node  [nv] = e_node [nv];
   for (int ns=0 ; ns<HE_MAXI ; ns++) i_edge  [ns] = e_edge [ns];
   for (int nq=0 ; nq<HQ_MAXI ; nq++) i_quad  [nq] = e_quad [nq];
   return HOK;
}
// ====================================================== makeRind
int Elements::makeRind (EnumGrid type, double* center, double* vx, double* vz, 
                        RealVector& tray, RealVector& tang, RealVector& tphi)
{
   int na = tang.size() - 1;
   int nr = tray.size() - 1;
   int nh = tphi.size() - 1;
   int rad0 = 0;
   int nint = 0;

   bool sphere = true;
   if (type==GR_PART_RIND || type==GR_RIND)
      {
      sphere = false;
      nr   --;
      rad0 = 1;
      nint = 1;
      }

   resize (type, nr, na, nh);

   cyl_radhole = tray[0];
   cyl_radint  = tray[nint];
   cyl_radext  = tray[rad0+nr];
   cyl_closed  = type==GR_HEMISPHERIC || type==GR_RIND;
   cyl_phimax  = calcul_phimax (cyl_radhole, cyl_radext, sphere);
   PutData (cyl_phimax);
   int nb_secteurs = cyl_closed ? size_vy-1 : size_vy;

   for (int ny=0 ; ny<nb_secteurs ; ny++)
       {
       double theta = tang[ny];
       for (int nx=0 ; nx<size_vx ; nx++)
           {
           double rayon = tray[rad0+nx];
           if (sphere && ny==0 && nx==0)
               printf (" rayon = %g, rad_int=%g\n", rayon, cyl_radint);

           for (int nz=0 ; nz<size_vz ; nz++)
               {
               double px, py, pz;
               double phi = tphi [nz];
               makeRindPoint (rayon, theta, phi, px, py, pz);
               Vertex* node = el_root->addVertex (px, py, pz);
               setVertex (node, nx, ny, nz);
               }
           }
       }

   if (cyl_closed)
      {
      for (int nx=0 ; nx<size_vx ; nx++)
          for (int nz=0 ; nz<size_vz ; nz++)
              {
              Vertex* node = getVertexIJK (nx, 0, nz);
              setVertex (node, nx, size_vy-1, nz);
              }
      }

   transfoVertices (center, vx, vz);
   fillGrid ();
   double angle = tang[na];
   assoCylinders (center, vz, angle, tang);
   return HOK;
}
// ====================================================== makeRindPoint
int Elements::makeRindPoint (double ray, double theta, double phi, 
                             double& px, double& py, double& pz)
{
   bool   rind   = (grid_type == GR_RIND || grid_type == GR_PART_RIND);
   double rphi   = deg2radians (phi);
   rphi = std::min (cyl_phimax, std::max(-cyl_phimax, rphi));
   double rtheta = deg2radians (theta);
   double sinphi = sin (rphi);
   double cosphi = cos (rphi);

   double rayon = 0;
   if (rind)
      {
      // rayon = cyl_radint + nr*(cyl_radext-cyl_radint)/size_hx;
      // pz    = rayon*sinphi;
      // rayon = rayon*cosphi;
      pz    = ray*sinphi;
      rayon = ray*cosphi;
      }
   else
      {
      // rayon = cyl_radhole + nr*(cyl_radext*cosphi - cyl_radhole)/size_hx;
      pz    = cyl_radext*sinphi;
      rayon = cyl_radhole + (ray-cyl_radhole)*(cyl_radext*cosphi-cyl_radhole)
                          / (cyl_radext-cyl_radhole);
      }

   rayon = std::max (cyl_radhole, rayon);
   px = rayon * cos (rtheta);
   py = rayon * sin (rtheta);
   return HOK;
}
// ====================================================== extrudeQuads
int Elements::extrudeQuads (Quads& tstart, double* axe, RealVector& tabval, 
                            bool revol, Vertex* center)
{
   under_v6    = false;
   int nbiter  = tabval.size ();
   int nbcells = tstart.size ();
   
   el_root->markAll (NO_USED);
   nbr_vertex    = 0;
   nbr_edges     = 0;

   nbr_hexas   = nbcells*nbiter;

   tab_hexa.resize (nbr_hexas, NULL);
   tab_quad.clear ();          // verticaux
   ker_hquad.clear ();         // Horizontaux
   tab_edge.clear ();
   tab_pilier.clear ();
   tab_vertex.clear ();
   copy_vecteur (axe, prism_dir);

   revo_lution  = revol;
   prism_vec    = NOT revol;
   copy_vecteur (axe, revo_axe);
   revo_center  = center;
   gen_values   = tabval;
   cum_values   = gen_values;

   for (int nro=1 ; nro<nbiter ; nro++)
       gen_values [nro] = cum_values [nro] - cum_values [nro-1];

   grid_geom   = NULL;
   getShape ();

   for (int nro=0 ; nro<nbcells ; nro++)
       {
       prismHexas (nro, tstart[nro], nbiter);
       }

   endPrism ();
   return HOK;
}
// ============================================================ joinQuads
int Elements::joinQuads (Quads& tstart, Quad* cible, Vertex* va1, Vertex* vb1,
                         Vertex* va2, Vertex* vb2, RealVector& tlen)
{
   under_v6    = false;
   int nbiter  = tlen  .size();
   int nbquads = tstart.size();
   resize (GR_JOINT, nbquads, nbiter);

   el_root->markAll (IS_NONE);
   db = on_debug();

   gr_hauteur = nbiter;
   nbr_orig   = nbquads;
   tab_orig   = tstart;
   cum_values = tlen;

   for (int nq=0 ; nq<nbquads ; nq++)
         tab_orig [nq]->setMark (nq);

   StrOrient orient (va1, va2, vb1, vb2);
   int ier =  this   ->coupler (0, cible, &orient);
   if (ier!=HOK)
      {
      setError ();
      return HERR;
      }
   ier = tstart[0]->coupler (cible, &orient, this);
   return ier;
}
// ============================================================ cutEdge
int Elements::cutEdge (Edge* edge, RealVector& tlen)
{
   under_v6    = false;
   gen_values  = tlen;
   cum_values  = gen_values;

   Propagation* prop    = el_root->findPropagation (edge);
   const Edges& t_edges = prop->getEdges ();

   int nbcuts = tlen.size ();
   cutHexas (t_edges, nbcuts);

   // el_root->majPropagation ();
   return HOK;
}
// ============================================================ nearestVertex
Vertex* Elements::nearestVertex (Vertex* other)
{
   if (BadElement (other))
      return NULL;
 
   Vertex* vsol = NULL;
   double  dmin = 1e+77;
   int nbre = countVertex ();
   for (int nro=0 ; nro<nbre ; nro++)
       {
       Vertex* vert = getVertex (nro);
       double  dist = other->dist2 (vert);
       if (dist < dmin)
          {
          dmin = dist;
          vsol = vert; 
          }
       }
   return vsol;
}
END_NAMESPACE_HEXA
