
// C++ : Grilles

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
#include "HexDocument.hxx"
#include "HexVector.hxx"
#include "HexVertex.hxx"
#include "HexHexa.hxx"
#include "HexEdge.hxx"
#include "HexGlobale.hxx"

#include <cmath>
#include <map>

BEGIN_NAMESPACE_HEXA

static bool db=on_debug();

// ====================================================== Constructeur
Elements::Elements (Document* doc) : EltBase (doc, EL_GRID)
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
   val_absolues = false;       // Version Hexa6
   cyl_phimax   = M_PI/2;
   under_v6     = true;
}
// ====================================================== Constructeur
Elements::Elements (Document* doc, int nx, int ny, int nz)
        : EltBase (doc, EL_GRID)
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
   grid_geom  = NULL;
   cyl_phimax   = M_PI/2;
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
   cyl_phimax = orig->cyl_phimax;
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

   tab_hexa  .resize (nbr_hexas,  NULL);
   tab_quad  .resize (nbr_quads,  NULL);
   tab_edge  .resize (nbr_edges,  NULL);
   tab_vertex.resize (nbr_vertex, NULL);

   ker_vertex = nbr_vertex;
}
// ====================================================== saveVtk
int Elements::saveVtk (cpchar nomfic)
{
   DumpStart ("saveVtk", nomfic);

   pfile    vtk = fopen (nomfic, "w");
   if (vtk==NULL)
      {
      DumpReturn (HERR);
      return HERR;
      }
                                           // -- 1) Raz numerotation precedente
   for (int nro=0 ; nro<nbr_hexas ; nro++)
       {
       Hexa* cell = tab_hexa[nro];
       if (cell!=NULL && cell->isHere())
           cell->razNodes ();
       }
   for (int nro=0 ; nro<size_hplus ; nro++)
       {
       Hexa* cell = ker_hexa[nro];
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

   for (int nro=0 ; nro<size_hplus ; nro++)
       {
       Hexa* cell = ker_hexa[nro];
       if (cell!=NULL && cell->isHere())
          {
          nbcells ++;
          nbnodes += cell->countNodes ();
          }
       }

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
   for (int nro=0 ; nro<size_hplus ; nro++)
       {
       Hexa* cell = ker_hexa[nro];
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
   for (int nro=0 ; nro<size_hplus ; nro++)
       {
       Hexa* cell = ker_hexa[nro];
       if (cell!=NULL && cell->isHere())
          cell->printHexa (vtk);
       }

   fprintf (vtk, "CELL_TYPES %d\n", nbcells);
   for (int nro=0 ; nro<nbcells ; nro++)
       fprintf (vtk, "%d\n", HE_MAXI);

   fclose (vtk);
   DumpReturn (HOK);
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

          double dxt = nd2->getX() - px0;
          double dyt = nd2->getY() - py0;
          double dzt = nd2->getZ() - pz0;

          nd1->setMark (indVertex (nquad, ns, 0));

          Vertex* nd = nd1;
          for (int nh=0 ; nh<gr_hauteur ; nh++)
              {
              double coeff = under_v6 ? ((double)(nh+1))/gr_hauteur : cum_values [nh];
              Vertex* ndp = nd;
              if (nh == gr_hauteur-1)
                 nd = nd2 ;
              else
                 nd = el_root->addVertex (px0 + coeff*dxt, py0 + coeff*dyt,
                                          pz0 + coeff*dzt);
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
   tab_quad.resize   (nbr_quads,  NULL);
   tab_edge.resize   (nbr_edges,  NULL);
   tab_hexa.resize   (nbr_hexas,  NULL);
   tab_vertex.resize (nbr_vertex, NULL);
   vector <Edge*>    tab_pilier (nbpiliers);

   int nbinter = nbcuts + 1;
   for (int ned=0; ned<nbnodes ; ned++)
       {
       Edges  ass_edges;
       t_edges [ned]->remove ();
       Vertex* ndamont = v_amont [ned];
       Vertex* ndaval  = v_aval  [ned];

       double lgx = ndaval->getX() - ndamont->getX();
       double lgy = ndaval->getY() - ndamont->getY();
       double lgz = ndaval->getZ() - ndamont->getZ();

       Vertex* nd0 = tab_vertex [ned] = ndamont;
       for (int nc=0; nc<nbcuts ; nc++)
           {
           int nc1 = nc+1;
           double coeff = under_v6 ? ((double)(nc+1))/nbinter : cum_values[nc]; 
           Vertex* nd1 = el_root->addVertex (ndamont->getX() + coeff*lgx,
                                             ndamont->getY() + coeff*lgy,
                                             ndamont->getZ() + coeff*lgz);
           tab_vertex [nc1*nbnodes + ned] = nd1;
           tab_pilier [nc *nbnodes + ned] = newEdge (nd0, nd1);
           ass_edges.push_back (tab_pilier [nc *nbnodes + ned]);
           nd0 =  nd1;
           }
       tab_vertex [nbinter*nbnodes + ned] = ndaval;
       tab_pilier [nbcuts *nbnodes + ned] = newEdge (nd0, ndaval);
       ass_edges.push_back (tab_pilier[nbcuts *nbnodes + ned]);
       ndamont->setMark (ned);
       if (t_edges[ned]->isAssociated())
           cutAssociation (t_edges[ned], ass_edges);
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
// ====================================================== clear_associations
void clear_associations (std::vector<Quad*>& table)
{
   int nbelts = table.size();
   for (int nro=0 ; nro<nbelts ; nro++)
       {
       Quad* elt = table [nro];
       if (elt != NULL && elt->isValid())
           elt->clearAssociation ();
       }
}
// ====================================================== clear_associations
void clear_associations (std::vector<Edge*>& table)
{
   int nbelts = table.size();
   for (int nro=0 ; nro<nbelts ; nro++)
       {
       Edge* elt = table [nro];
       if (elt != NULL && elt->isValid())
           elt->clearAssociation ();
       }
}
// ====================================================== clear_associations
void clear_associations (std::vector<Vertex*>& table)
{
   int nbelts = table.size();
   for (int nro=0 ; nro<nbelts ; nro++)
       {
       Vertex* elt = table [nro];
       if (elt != NULL && elt->isValid())
           elt->clearAssociation ();
       }
}
// ====================================================== clearAssociation
void Elements::clearAssociation  ()
{
   clear_associations (tab_quad);
   clear_associations (tab_edge);
   clear_associations (tab_vertex);


   clear_associations (ker_hquad);
   clear_associations (ker_vquad);
   clear_associations (ker_hedge);
   clear_associations (ker_vedge);
}
// ============================================================ findVertex
int Elements::findVertex (double vx, double vy, double vz)
{
   double tol = el_root->getTolerance ();
   double xmin = vx - tol;
   double xmax = vx + tol;
   double ymin = vy - tol;
   double ymax = vy + tol;
   double zmin = vz - tol;
   double zmax = vz + tol;

   int nbre = tab_vertex.size();
   for (int nro=0 ; nro<nbre ; nro++)
       {
       Vertex* node = tab_vertex [nro];
       if (node != NULL && node->isHere ()
                        && node->isin   (xmin, xmax, ymin, ymax, zmin, zmax))
              return nro;
       }
   return NOTHING;
}
// ============================================================ findQuad
Quad* Elements::findQuad (Edge* e1, Edge* e2)
{
   int nbre = tab_quad.size();
   for (int nro=0 ; nro<nbre ; nro++)
       {
       Quad* quad = tab_quad [nro];
       if (quad != NULL && quad->isHere ()
                        && quad->definedBy (e1, e2))
              return quad;
       }
   return NULL;
}

END_NAMESPACE_HEXA
