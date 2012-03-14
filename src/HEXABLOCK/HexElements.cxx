
// C++ : Grilles

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
//  See http://www.salome-platform.org/ 
//  or email : webmaster.salome@opencascade.com

#include "HexElements.hxx"
#include "HexDocument.hxx"
#include "HexVector.hxx"
#include "HexVertex.hxx"
#include "HexHexa.hxx"
#include "HexEdge.hxx"
#include "HexGlobale.hxx"

#include <cmath>
#include <map>

static bool db=false;

BEGIN_NAMESPACE_HEXA

// ====================================================== Constructeur
Elements::Elements (Document* doc) : EltBase (doc)
{
   glob  = Globale::getInstance ();

   grid_type  = GR_NONE;
   size_qx = size_ex = size_vx = size_hx = 0;
   size_qy = size_ey = size_vy = size_hy = 0;
   size_qz = size_ez = size_vz = size_hz = 0;
   size_qvplus = size_qhplus = size_ehplus = size_evplus = size_hplus = 0;

   nbr_hexas  = nbr_quads  = nbr_edges  = nbr_vertex = 0;
   ker_vertex = 0;
   cyl_closed = false;
   cyl_fill   = false;
   grid_nocart = true;
   cyl_dispo  = CYL_NOFILL;
   revo_lution = false;
   prism_vec   = false;
}
// ====================================================== Constructeur
Elements::Elements (Document* doc, int nx, int ny, int nz) : EltBase (doc)
{
   glob  = Globale::getInstance ();

   grid_type  = GR_NONE;
   size_qx = size_ex = size_vx = size_hx = 0;
   size_qy = size_ey = size_vy = size_hy = 0;
   size_qz = size_ez = size_vz = size_hz = 0;
   size_qvplus = size_qhplus = size_ehplus = size_evplus = size_hplus = 0;

   nbr_hexas  = nbr_quads  = nbr_edges  = nbr_vertex = 0;
   cyl_closed = true;
   cyl_fill   = false;
   cyl_dispo  = CYL_NOFILL;

   resize (GR_CYLINDRIC, nx, ny, nz);
   cyl_closed = true;
}
// ====================================================== Constructeur (clonage)
Elements::Elements (Elements* orig) : EltBase (orig->el_root)
{
   glob = Globale::getInstance ();

   grid_type  = orig->grid_type;
   cyl_closed = orig->cyl_closed;
   cyl_fill   = orig->cyl_fill;
   cyl_dispo  = orig->cyl_dispo;

   size_qx = size_ex = size_vx = size_hx = 0;
   size_qy = size_ey = size_vy = size_hy = 0;
   size_qz = size_ez = size_vz = size_hz = 0;
   size_qvplus = size_qhplus = size_ehplus = size_evplus = size_hplus = 0;
   nbr_hexas  = nbr_quads  = nbr_edges  = nbr_vertex = 0;

   resize (orig->grid_type, orig->size_hx, orig->size_hy, orig->size_hz);
   cyl_closed = orig->cyl_closed;
}
// ====================================================== resize
void Elements::resize (EnumGrid type, int nx, int ny, int nz, int nplus)
{
   grid_type   = type;
   grid_nocart = true;

   switch (grid_type)
      {
      case GR_CARTESIAN :
      case GR_CYLINDRIC :
      default :
           grid_nocart = false;
           size_hx = std::max (nx, 1);
           size_hy = std::max (ny, 1);
           size_hz = std::max (nz, 1);

           size_qx = size_ex = size_vx = size_hx + 1;
           size_qy = size_ey = size_vy = size_hy + 1;
           size_qz = size_ez = size_vz = size_hz + 1;

           nbr_hexas  = size_hx * size_hy * size_hz;
           nbr_quads  = size_qx * size_qy * size_qz * DIM3;
           nbr_edges  = size_ex * size_ey * size_ez * DIM3;
           nbr_vertex = size_vx * size_vy * size_vz;
           break;

      case GR_SPHERIC :
           size_qx = size_ex = size_vx = size_hx = 0;
           size_qy = size_ey = size_vy = size_hy = 0;
           size_qz = size_ez = size_vz = size_hz = 0;
           nbr_quads  = nbr_edges  = nbr_vertex = 0;
           gr_rayon = std::max (nx, 1);

           nbr_hexas = 1 +  gr_rayon*HQ_MAXI;
           tab_hexa.clear ();
           ker_vertex = nbr_vertex;
           return;

      case GR_JOINT :
           nbr_orig = std::max (nx, 1); 
           gr_hauteur  = std::max (ny, 1);
           size_hx  = nbr_orig;
           size_hy  = 1;
           size_hz  = gr_hauteur;

           nbr_hexas  = nbr_orig * gr_hauteur;
           nbr_vertex = nbr_hexas * QUAD4;
           nbr_vertex = nbr_orig * (gr_hauteur+1)*QUAD4;
           nbr_quads  = nbr_vertex;
           nbr_edges  = 2*nbr_vertex;
           break;

      case GR_REPLACE :
           nbr_orig    = std::max (nx, 1);    // nb quads du pattern
           gr_hauteur  = ny + 1;              // Hauteur des hexas 
           pat_nbvertex  = std::max (nz, 1);    // nb vertex du pattern
           pat_nbedges   = std::max (nplus, 1); // nb edges du pattern
           size_hx  = nbr_orig;
           size_hy  = 1;
           size_hz  = gr_hauteur;

           nbr_hexas  = nbr_orig  * gr_hauteur;
           nbr_vertex = pat_nbvertex * (gr_hauteur+1);
           nbr_edges  = pat_nbedges * (gr_hauteur+1) + pat_nbvertex*gr_hauteur;
           nbr_quads  = nbr_orig    * (gr_hauteur+1) + pat_nbedges *gr_hauteur;
           break;

      case GR_BICYL :
           cyl_closed = true;
           size_hx = 1;
           size_hy = 8;
           size_hz = 4;

           size_qx = size_ex = size_vx = size_hx + 1;
           size_qy = size_ey = size_vy = size_hy + 1;
           size_qz = size_ez = size_vz = size_hz + 1;

           nbr_hexas  = size_hx * size_hy * size_hz;
           nbr_quads  = size_qx * size_qy * size_qz * DIM3;
           nbr_edges  = size_ex * size_ey * size_ez * DIM3;
           nbr_vertex = size_vx * size_vy * size_vz;
           break;
      }

   tab_hexa  .resize (nbr_hexas);
   tab_quad  .resize (nbr_quads);
   tab_edge  .resize (nbr_edges);
   tab_vertex.resize (nbr_vertex);

   ker_vertex = nbr_vertex;

   for (int nc=0 ; nc< nbr_hexas  ; nc++) tab_hexa   [nc] = NULL;
   for (int nc=0 ; nc< nbr_quads  ; nc++) tab_quad   [nc] = NULL;
   for (int nc=0 ; nc< nbr_edges  ; nc++) tab_edge   [nc] = NULL;
   for (int nc=0 ; nc< nbr_vertex ; nc++) tab_vertex [nc] = NULL;
}

// ====================================================== makeCartesianGrid
int Elements::makeCartesianGrid (Vertex* orig, Vector* v1, Vector* v2, 
                   Vector* v3, int px, int py, int pz, int mx, int my, int mz)
{
   resize (GR_CARTESIAN, px+mx, py+my, pz+mz);

   makeCartesianNodes (orig, v1, v2, v3, px, py, pz, mx, my, mz);

   fillGrid ();
   return HOK;
}
// ====================================================== makeCylindricalGrid
int Elements::makeCylindricalGrid (Vertex* c, Vector* b, Vector* h, 
         double dr, double da, double dl, int nr, int na, int nl, bool fill)
{
   resize (GR_CYLINDRIC, nr, na, nl);
   cyl_closed = da >= 360.0;
   makeCylindricalNodes (c, b, h, dr, da, dl, nr, na, nl, fill);
   fillGrid ();
   assoCylinder (c, h, da);
   return HOK;
}
// ====================================================== makeSphericalGrid
int Elements::makeSphericalGrid (Vertex* c, Vector* dv, int nb, double  k)
{
   resize (GR_SPHERIC, nb);

   if (nb<0) 
      return HERR;
   else if (dv->getDx()<=ZEROR || dv->getDy()<=ZEROR || dv->getDz()<=ZEROR)
      return HERR;

   Vertex* i_node [HV_MAXI];    // Les noeuds de l'hexa englobant
   Edge*   i_edge [HE_MAXI];    // Les noeuds de l'hexa englobant
   Quad*   i_quad [HQ_MAXI];    // Les noeuds de l'hexa englobant

   for (int nro=0 ; nro<HV_MAXI; nro++)
       {
       double dx = glob->CoordVertex (nro, dir_x) * dv->getDx();
       double dy = glob->CoordVertex (nro, dir_y) * dv->getDy();
       double dz = glob->CoordVertex (nro, dir_z) * dv->getDz();

       i_node [nro] = el_root->addVertex (c->getX ()+dx, c->getY ()+dy, 
                                          c->getZ ()+dz);
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
   double lambda  = 1;
   double dcell   = 1;
   for (int niv=0; niv<gr_rayon ; niv++)
       {
       double lambda0 = lambda;
       dcell  *= k;
       lambda += dcell;
       addStrate (i_quad, i_edge, i_node, c,  lambda/lambda0);
       }
       
   return HOK;
}
// ====================================================== addStrate
int Elements::addStrate (Quad* i_quad[], Edge* i_edge[], Vertex* i_node[], 
                        Vertex* center,  double lambda)
{
   Vertex* e_node [HV_MAXI];    // Les noeuds de l'hexa englobant
   Edge*   e_edge [HE_MAXI];    // Les noeuds de l'hexa englobant
   Quad*   e_quad [HQ_MAXI];    // Les noeuds de l'hexa englobant

   Edge*   d_edge [HV_MAXI];    // Les aretes diagonales (1 par sommet)
   Quad*   d_quad [HE_MAXI];    // Les faces  diagonales (1 par arete)

                                          // Les sommets
                                          //  + les aretes diagonales
   for (int nv=0 ; nv<HV_MAXI ; nv++)
       {
       double px0 = center->getX ();
       double py0 = center->getY ();
       double pz0 = center->getZ ();
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
// ====================================================== saveVtk
int Elements::saveVtk (cpchar nomfic)
{
                                           // -- 1) Raz numerotation precedente
   for (int nro=0 ; nro<nbr_hexas ; nro++)
       {
       Hexa* cell = tab_hexa[nro];
       if (cell!=NULL && cell->isHere())
           cell->razNodes ();
       }

   int nbnodes = 0;
   int nbcells = 0;
                                           // -- 2) Comptage
   for (int nro=0 ; nro<nbr_hexas ; nro++)
       {
       Hexa* cell = tab_hexa[nro];
       if (cell!=NULL && cell->isHere())
          {
          nbcells ++;
          nbnodes += cell->countNodes ();
          }
       }

   pfile    vtk = fopen (nomfic, "w");
   fprintf (vtk, "# vtk DataFile Version 3.1\n");
   fprintf (vtk, "%s \n", nomfic);
   fprintf (vtk, "ASCII\n");
   fprintf (vtk, "DATASET UNSTRUCTURED_GRID\n");
   fprintf (vtk, "POINTS %d float\n", nbnodes);

                                           // -- 2) Les noeuds
   int nronode = 0;
   for (int nro=0 ; nro<nbr_hexas ; nro++)
       {
       Hexa* cell = tab_hexa[nro];
       if (cell!=NULL && cell->isHere())
          cell->printNodes (vtk, nronode);
       }
                                           // -- 2) Les hexas

   fprintf (vtk, "CELLS %d %d\n", nbcells, nbcells*(HV_MAXI+1));

   for (int nro=0 ; nro<nbr_hexas ; nro++)
       {
       Hexa* cell = tab_hexa[nro];
       if (cell!=NULL && cell->isHere())
          cell->printHexa (vtk);
       }

   fprintf (vtk, "CELL_TYPES %d\n", nbcells);
   for (int nro=0 ; nro<nbcells ; nro++)
       fprintf (vtk, "%d\n", HE_MAXI);

   fclose (vtk);
   return HOK;
}
// ============ = = = = = = = = = Vertices ..........
// ====================================================== newEdge
Edge* Elements::newEdge (Vertex* v1, Vertex* v2)
{
   if (v1==NULL || v2==NULL) 
      return NULL;

   Edge* elt = new Edge (v1, v2);
   return elt;
}
// ====================================================== newQuad
Quad* Elements::newQuad (Edge* e1, Edge* e2, Edge* e3, Edge* e4)
{
   if (e1==NULL || e2==NULL || e3==NULL|| e4==NULL)
      return NULL;

   Quad* elt = new Quad (e1, e2, e3, e4);
   return elt;
}
// ====================================================== newHexa
Hexa* Elements::newHexa (Quad* qa, Quad* qb, Quad* qc, Quad* qd, 
                                             Quad* qe, Quad* qf)
{
   if (qa==NULL || qb==NULL || qc==NULL|| qd==NULL || qe==NULL|| qf==NULL)
      return NULL;

   Hexa*  elt = new Hexa (qa, qb, qc, qd, qe, qf);
   return elt;
}
// ====================================================== joinQuads
int Elements::joinQuads (Quads& orig, int nb, Vertex* v1, Vertex* v2, 
                  Vertex* v3, Vertex* v4, Quad* cible)
{
   resize (GR_JOINT, orig.size(), nb);

   el_root->markAll (IS_NONE);
   db = on_debug();
   // db = el_root->debug ();
   
   gr_hauteur  = nb;
   nbr_orig = orig.size();

   for (int nro=0 ; nro<nbr_orig ; nro++)
       {
       Quad* face = orig[nro];
       if (face==NULL)
          {
          printf ("\n");
          printf (" *** joinQuads : donnees incorrectes\n");
          printf (" *** le %deme quadrangle de depart est NULL\n", nro);
          return HERR;
          }
       else if (face->getNbrParents()>1)
          {
          printf ("\n");
          printf (" *** joinQuads : donnees incorrectes\n");
          printf (" *** le %deme quadrangle de depart n'est pas une " 
                  "face externe\n", nro);
          face->dump ();        
          return HERR;
          }
       orig [nro]->setMark (nro);
       tab_orig.push_back (orig[nro]);
       }

   Edge* e_orig = tab_orig[0] -> findEdge (v1, v3);
   Edge* e_dest = cible       -> findEdge (v2, v4);

   if (e_orig==NULL)
      {
      printf ("\n");
      printf (" *** joinQuads : donnees incorrectes\n");
      printf (" *** Les vertex v1 et v3 passes en argument ne definissent\n");
      printf (" *** pas une arete du 1er quadrangle de depart\n");
      printf ("\n");
      HexDump (v1);
      HexDump (v3);
      HexDump (orig[0]);
      }

   if (e_dest==NULL)
      {
      printf ("\n");
      printf (" *** joinQuads : donnees incorrectes\n");
      printf (" *** Les vertex v2 et v4 passes en argument ne definissent\n");
      printf (" *** pas une arete du quadrangle cible\n");
      printf ("\n");
      HexDump (v2);
      HexDump (v4);
      HexDump (cible);
      }

   if (e_orig==NULL || e_dest==NULL)
      return HERR;

   StrOrient orient (v1, v3, v2, v4);
   int ier =  this   ->coupler (0, cible, &orient);
   if (ier!=HOK)
       return HERR;
   ier = orig[0]->coupler (cible, &orient, this);
   return ier;
}
// ======================================================== coupler
int Elements::coupler (int nquad, Quad* dest, StrOrient* orient)
{
   Quad* orig = tab_orig [nquad];

   setQuad (orig, dir_z, 0, 0, 0);
   setQuad (dest, dir_z, nquad, 0, 0);

   int n11 = orig->indexVertex (orient->v11);
   int n12 = orig->indexVertex (orient->v12);
   int n21 = dest->indexVertex (orient->v21);
   int n22 = dest->indexVertex (orient->v22);
   
               // ---------------- Les 4 sommets initiaux
   Vertex* vorig[QUAD4] = { orient->v11, orient->v12, 
                            orig->getVertex((n11+2) MODULO QUAD4),
                            orig->getVertex((n12+2) MODULO QUAD4) };

   Vertex* vdest[QUAD4] = { orient->v21, orient->v22, 
                            dest->getVertex((n21+2) MODULO QUAD4),
                            dest->getVertex((n22+2) MODULO QUAD4) };
   if (db)
      {
      printf ("Quad nro %d : ", nquad);
      orig->printName (" est couple avec "); 
      dest->printName ("\n"); 
      printf ("Orientation : (");
      for (int ii=0 ; ii<QUAD4 ; ii++) printf("%s ", vorig[ii]->getName());
      printf (")\n");
      printf ("           -> (");
      for (int ii=0 ; ii<QUAD4 ; ii++) printf("%s ", vdest[ii]->getName());
      printf (")\n");
      }

               // ---------------- Les sommets + les aretes verticales
   for (int ns=0 ; ns< QUAD4 ; ns++)
       {
       Vertex* nd1 = vorig[ns];
       Vertex* nd2 = vdest[ns];
       int nref0   = nd1->getMark();
       tab_vertex [nroVertex (nquad,ns,0)] = nd1;

       if (nref0==IS_NONE)
          {
          double px0 = nd1->getX();
          double py0 = nd1->getY();
          double pz0 = nd1->getZ();

          double dx = (nd2->getX() - nd1->getX()) / gr_hauteur;
          double dy = (nd2->getY() - nd1->getY()) / gr_hauteur;
          double dz = (nd2->getZ() - nd1->getZ()) / gr_hauteur;

          nd1->setMark (indVertex (nquad, ns, 0));

          Vertex* nd = nd1;
          for (int nh=0 ; nh<gr_hauteur ; nh++)
              {
              int nh1 = nh + 1;
              Vertex* ndp = nd;
              if (nh == gr_hauteur-1)
                 nd = nd2 ;
              else 
                 nd = el_root->addVertex (px0 + nh1*dx, py0 + nh1*dy,  
                                          pz0 + nh1*dz);
              int nv  = indVertex (nquad, ns, nh);
              tab_vertex [nv] = nd;
              tab_edge   [nv] = el_root->addEdge (ndp, nd);
              if (db)
                 printf (" Edge vertical nro %d = %s = (%s, %s)\n", nv, 
                         tab_edge[nv]->getName(), 
                         ndp->getName(), nd->getName());
              }
          }
       else
          {
          for (int nh=0 ; nh<gr_hauteur ; nh++)
              {
              int nv  = indVertex (nquad, ns, nh);
              int nv0 = indVertex (nref0, nh);
              tab_vertex [nv] = tab_vertex [nv0];
              tab_edge   [nv] = tab_edge   [nv0];
              }
          }
       }
               // ---------------- Les quads verticaux + aretes horiz
   for (int ns=0 ; ns< QUAD4 ; ns++)
       {
       int next = (ns+1) MODULO QUAD4;
       Edge*   arete = orig->findEdge (vorig[ns], vorig[next]);
       int     nref0 = arete->getMark();
       int     nref  = indVertex (nquad, ns, 0);
                                  // Construction des faces & aretes H
       if (nref0==IS_NONE)
          {
          arete->setMark (nref);
          Edge* ea = arete;
          Edge *eb, *ec, *ed;
          int  nva, nvb, nha;

          for (int nh=0 ; nh< gr_hauteur ; nh++)
              {
              nva = indVertex (nquad, ns,   nh);
              nvb = indVertex (nquad, next, nh);
              nha = nroEdgeH  (nva);

              ed  = tab_edge [nva];
              ec  = ea;
              eb  = tab_edge [nvb];
              if (nh==gr_hauteur-1)
                 ea = dest->findEdge (vdest[ns], vdest[next]);
              else 
                 ea = el_root->addEdge (tab_vertex [nva], tab_vertex [nvb]);

              propagateAssociation (ec, ea, eb);
              tab_quad [nva] = newQuad (ea, eb, ec, ed);
              if (BadElement (tab_quad [nva]))
                  return HERR;
              tab_edge [nha] = ea;
              }
          }
                                  // L'arete a deja ete traitee
       else
          {
          for (int nh=0 ; nh<gr_hauteur ; nh++)
              {
              int nva = indVertex (nquad, ns, nh);
              int nha = nroEdgeH  (nva);

              int nvb = indVertex (nref0, nh);
              int nhb = nroEdgeH  (nvb);

              tab_quad [nva] = tab_quad [nvb];
              tab_edge [nha] = tab_edge [nhb];
              }
          }
       }
               // -------------------------------- Les Hexas
   Quad *fb = orig;
   Quad *fa, *fc, *fd, *fe, *ff;
   for (int nh=0 ; nh< gr_hauteur ; nh++)
       {
       fa = fb;
       if (nh == gr_hauteur-1)
          fb = dest;
       else
          fb = newQuad (tab_edge [nroEdgeH (nquad, E_A, nh)], 
                        tab_edge [nroEdgeH (nquad, E_B, nh)],
                        tab_edge [nroEdgeH (nquad, E_C, nh)],
                        tab_edge [nroEdgeH (nquad, E_D, nh)]);

       fc = tab_quad [indVertex (nquad, E_A, nh)];
       fd = tab_quad [indVertex (nquad, E_C, nh)];
       fe = tab_quad [indVertex (nquad, E_B, nh)];
       ff = tab_quad [indVertex (nquad, E_D, nh)];

       tab_hexa [nroHexa(nquad,nh)] = newHexa (fa,fb,fc,fd,fe,ff);
       if (BadElement (tab_hexa [nroHexa(nquad,nh)]))
           return HERR;
       }
   return HOK;
}
// ====================================================== makeCartesianNodes
int Elements::makeCartesianNodes (Vertex* orig, Vector* v1, Vector* v2, 
                   Vector* v3, int px, int py, int pz, int mx, int my, int mz)
{
   double dx  = v1->getDx() + v2->getDx() + v3->getDx();
   double dy  = v1->getDy() + v2->getDy() + v3->getDy();
   double dz  = v1->getDz() + v2->getDz() + v3->getDz();

   double px0 = orig->getX () - mx * dx;
   double py0 = orig->getY () - my * dy;
   double pz0 = orig->getZ () - mz * dz;

   int nbre= 0;

   for (int nz=0 ; nz<size_vz ; nz++)
       for (int ny=0 ; ny<size_vy ; ny++)
           for (int nx=0 ; nx<size_vx ; nx++)
               {
               Vertex* node = orig;
               if (nx!=mx || ny!=my || nz!=mz)
                  node = el_root->addVertex (px0 + nx*dx,  py0 + ny*dy,  
                                                          pz0 + nz*dz);
               setVertex (node, nx, ny, nz);
               nbre++;
               }
   return HOK;
}
// ====================================================== makeCylindricalNodes
int Elements::makeCylindricalNodes (Vertex* orig, Vector* base, Vector* haut, 
            double dr, double da, double dl, int nr, int na, int nl, bool fill)
{
   int ier = makeBasicCylinder (dr, da, dl, nr, na, nl, fill);
   if (ier!=HOK) 
       return ier;

   transfoVertices  (orig,  base, haut);
   return HOK;
}
// ====================================================== transfoVertices
void Elements::transfoVertices (Vertex* orig, Vector* base, Vector* haut)
{
   Vector* iprim = new Vector (base);
   Vector* jprim = new Vector (base);
   Vector* kprim = new Vector (haut);

   int ier = kprim->renormer ();
   if (ier!=HOK) 
       return;

   jprim->vectoriel (kprim, base);
   ier = jprim->renormer ();
   if (ier!=HOK) 
       return;

   iprim->vectoriel (jprim, kprim);
   transfoVertices  (orig,  iprim, jprim, kprim);
}
// ====================================================== transfoVertices
void Elements::transfoVertices (Vertex* orig, Vector* iprim, Vector* jprim, 
                                Vector* kprim)
{
   double matrice[DIM3][DIM3]={{iprim->getDx(),jprim->getDx(),kprim->getDx()},
                               {iprim->getDy(),jprim->getDy(),kprim->getDy()},
                               {iprim->getDz(),jprim->getDz(),kprim->getDz()}};

   double matkx = orig->getX();
   double matky = orig->getY();
   double matkz = orig->getZ();

   int nbre = tab_vertex.size ();
   for (int nro=0 ; nro<nbre ; nro++)
       {
       if (tab_vertex[nro] != NULL) 
           tab_vertex[nro]->setMark (NO_USED);
       }

   for (int nro=0 ; nro<nbre ; nro++)
       {
       Vertex* node =  tab_vertex[nro];
       if (node != NULL  && node->getMark() == NO_USED)
          {
          double point [DIM3] = {node->getX(), node->getY(), node->getZ()};
          double result[DIM3] = {matkx, matky, matkz};

          for (int ni=0 ; ni<DIM3; ni++)
              for (int nj=0 ; nj<DIM3; nj++)
                  result [ni] += matrice[ni][nj] * point[nj];
               
          node->setCoord (result[dir_x], result[dir_y], result[dir_z]);
          node->setMark (IS_USED);
          }
       }
}
// ====================================================== transform
int Elements::transform (Matrix* matrice)
{
                                           // -- 1) Raz Marques
   for (int nro=0 ; nro<nbr_hexas ; nro++)
       {
       Hexa* cell = tab_hexa[nro];
       if (cell!=NULL && cell->isHere())
           cell->razNodes ();
       }
                                           // -- 2) Move Nodes
   for (int nro=0 ; nro<nbr_hexas ; nro++)
       {
       Hexa* cell = tab_hexa[nro];
       if (cell!=NULL && cell->isHere())
           cell->moveNodes (matrice);
       }

   if (nbr_hexas!=0 )
      return HOK;
                    // -- Cas pathologique : il n'y avait pas d'hexas
                    // -- On se rabat sur les sommets

   for (int nro=0 ; nro<nbr_vertex ; nro++)
       {
       Vertex* node = tab_vertex[nro];
       if (node!=NULL && node->isHere())
           matrice->perform (node);
       }

   return HOK;
}
// ====================================================== cutHexas
int Elements::cutHexas  (const Edges& t_edges, int nbcuts)
{
                                       // 1) marquage des hexas
   el_root->markAll (NO_USED);
                                       // 2) Memo noeuds
   vector <Quad*> q_amont;
   vector <Quad*> q_aval;
   map    <Vertex*, Vertex*> vis_a_vis;

   int nbnodes = t_edges.size();
   vector <Vertex*> v_amont (nbnodes);
   vector <Vertex*> v_aval  (nbnodes);

   int nbfaces = 0;
   for (int nro=0; nro<nbnodes ; nro++)
       {
       Edge* arete   = t_edges [nro];
       v_amont [nro] = arete->getAmont ();
       v_aval  [nro] = arete->getAval  ();
       if (db) 
          {
          printf (" %3d : Edge = (", nro);
          v_amont[nro]->printName (", ");
          v_aval [nro]->printName (")\n");
          }

       vis_a_vis [v_amont[nro]] = v_aval[nro];
       vis_a_vis [v_aval[nro]] = v_amont[nro];
       int nbcells   = arete->getNbrParents ();

       for (int nq=0 ; nq<nbcells ; nq++)
           {
           Quad* quad = arete->getParent (nq);
           if (quad->getMark () != IS_USED)
              {
              quad->setMark (IS_USED);
              int nbcubes = quad->getNbrParents ();
              for (int nh=0 ; nh<nbcubes ; nh++)
                  {
                  Hexa* hexa = quad->getParent (nh);
                  if (hexa->getMark () != IS_USED)  
                     {
                     hexa->setMark (IS_USED);
                     int namont = hexa->getBase (v_amont[nro], arete);
                     int naval  = glob->getOpposedQuad (namont);
                     q_amont.push_back (hexa->getQuad  (namont));
                     q_aval .push_back (hexa->getQuad  (naval ));
 
                     if (db)
                        {
                        printf (" %3d : Quad = ", nbfaces);
                        hexa->printName (", ");
                        printf (" Faces = (");
                        hexa->getQuad (namont)->printName (", ");
                        hexa->getQuad (naval )->printName (")\n");
                        nbfaces ++;
                        }
                     }
                  }
              }
           }
       }
                   // ------------------- Dimensionnement
   int nbcells   = q_amont.size ();
   nbr_vertex    = nbnodes*(nbcuts+2);
   int nbpiliers = nbnodes*(nbcuts+1);            // aretes verticales
   int nbpoutres = nbcells*(nbcuts+2)*QUAD4;      // aretes horizontales
   nbr_edges     = nbpoutres; 
   nbr_quads     = nbcells*(nbcuts+1)*QUAD4;      // faces Verticales
   nbr_hexas     = nbcells*(nbcuts+1);

                   // ------------------- Les noeuds et les aretes verticales
   tab_quad.resize   (nbr_quads);
   tab_edge.resize   (nbr_edges);
   tab_hexa.resize   (nbr_hexas);
   tab_vertex.resize (nbr_vertex);
   vector <Edge*>    tab_pilier (nbpiliers);

   int nbinter = nbcuts + 1;
   for (int ned=0; ned<nbnodes ; ned++)
       {
       Shapes ass_shapes = t_edges[ned] -> getAssociations ();
       Edges  ass_edges;
       t_edges [ned]->remove ();
       Vertex* ndamont = v_amont [ned];
       Vertex* ndaval  = v_aval  [ned];

       double dx = (ndaval->getX() - ndamont->getX()) / nbinter;
       double dy = (ndaval->getY() - ndamont->getY()) / nbinter;
       double dz = (ndaval->getZ() - ndamont->getZ()) / nbinter;

       Vertex* nd0 = tab_vertex [ned] = ndamont;
       for (int nc=0; nc<nbcuts ; nc++)
           {
           int nc1 = nc+1;
           Vertex* nd1 = el_root->addVertex (ndamont->getX() + nc1*dx, 
                                             ndamont->getY() + nc1*dy, 
                                             ndamont->getZ() + nc1*dz);
           tab_vertex [nc1*nbnodes + ned] = nd1;
           tab_pilier [nc *nbnodes + ned] = newEdge (nd0, nd1);
           ass_edges.push_back (tab_pilier [nc *nbnodes + ned]);
           nd0 =  nd1;
           }
       tab_vertex [nbinter*nbnodes + ned] = ndaval;
       tab_pilier [nbcuts *nbnodes + ned] = newEdge (nd0, ndaval);
       ass_edges.push_back (tab_pilier[nbcuts *nbnodes + ned]);
       ndamont->setMark (ned);
       cutAssociation (ass_shapes, ass_edges);
       }
                   // ------------------- Les aretes horizontales
                   // ------------------- Les faces verticales
   HexDisplay (nbcells);
   int sizelig = nbcells*QUAD4;
   for (int nro=0; nro<nbcells ; nro++)
       {
       Quad* sol  = q_amont [nro];
       Quad* toit = q_aval  [nro];
       for (int ns=0; ns<QUAD4 ; ns++)
           {
           Edge* plinthe = sol->getEdge (ns);
           int   nmur  = nro*QUAD4 + ns;
           int   nmur0 = plinthe->getMark();
           if (nmur0 >= 0)
              {
              for (int nc=0 ; nc<nbinter ; nc++)
                  {
                  tab_edge [nc*sizelig + nmur] = tab_edge [nc*sizelig + nmur0];
                  tab_quad [nc*sizelig + nmur] = tab_quad [nc *sizelig + nmur0];
                  if (db) 
                     {
                     printf (" %2d : %d quad_vertical [%02d] =", nro, ns, 
                                                        nc*sizelig + nmur);
                     printf (" quad_vertical [%02d]\n",  nc*sizelig + nmur0);
                     }
                  }
              tab_edge [nbinter*sizelig+nmur] = tab_edge[nbinter*sizelig+nmur0];
              }
           else
              {
              plinthe->setMark (nmur);
              Vertex* vs1 = sol->getVertex (ns);
              Vertex* vs2 = sol->getVertex ((ns+1) MODULO QUAD4);
              int     nd1 = vs1->getMark ();
              int     nd2 = vs2->getMark ();
              Edge*   ed0 = tab_edge [nmur] = plinthe;
              Edge*   ed2 = NULL;
              Vertex* v1  = NULL;
              Vertex* v2  = NULL;
              for (int nc=0 ; nc<nbinter ; nc++)
                  {
                  int   nc1 = nc + 1;
                  if (nc<nbcuts)
                     {
                     v1 = tab_vertex [nc1*nbnodes + nd1];
                     v2 = tab_vertex [nc1*nbnodes + nd2];
                     ed2 = newEdge (v1, v2);
                     }
                  else
                     {
                     v1 = vis_a_vis [vs1];
                     v2 = vis_a_vis [vs2];
                     ed2 = toit->findEdge (v1, v2); 
                     }

                  tab_edge [nc1*sizelig + nmur] = ed2;
                  tab_quad [nc *sizelig + nmur] = newQuad (ed0,
                            tab_pilier [nc*nbnodes + nd1], ed2,
                            tab_pilier [nc*nbnodes + nd2]);
                  ed0 = ed2;
                  if (db) 
                     {
                     printf (" %2d : %d quad_vertical [%02d] = ", nro, ns, 
                                                         nc*sizelig + nmur);
                     PrintName (tab_quad [nc *sizelig + nmur]);
                     printf ("\n");
                     }
                  }
              }
           }
       }
                   // ------------------- Les faces horizontales
                   // ------------------- Les hexas
   // Rappel : sizelig = nbcells*QUAD4 
   for (int nro=0; nro<nbcells ; nro++)
       {
       Quad* qa = q_amont [nro];
       for (int nc=0 ; nc<nbinter ; nc++)
           {
           int  quadv =  nc*nbcells*QUAD4 + nro*QUAD4;
           Quad* qb = q_aval[nro];
           if (nc<nbcuts)
              {
              int  edh   = (nc+1)*nbcells*QUAD4 + nro*QUAD4;
              qb = newQuad (tab_edge[edh],   tab_edge[edh+1], 
                            tab_edge[edh+2], tab_edge[edh+3]);
              if (BadElement(qb))
                  return HERR;
              }
           Quad* qc = tab_quad [quadv];
           Quad* qd = tab_quad [quadv + 2];
           Quad* qe = tab_quad [quadv + 1];
           Quad* qf = tab_quad [quadv + 3];
           Hexa* hexa = newHexa (qa, qb, qc, qd, qe, qf);
           if (BadElement(hexa))
               return HERR;
           tab_hexa [nc*nbcells + nro] = hexa;
           qa = qb;
           }
       }
   return HOK;
}
END_NAMESPACE_HEXA
