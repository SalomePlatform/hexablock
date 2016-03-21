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

// C++ : Table des noeuds

#include "HexElements.hxx"

#include "HexDocument.hxx"
#include "HexVector.hxx"
#include "HexVertex.hxx"
#include "HexHexa.hxx"
#include "HexEdge.hxx"

#include "HexGlobale.hxx"

#include <cmath>

BEGIN_NAMESPACE_HEXA
// ====================================================== fillGrid
int Elements::fillGrid ()
{
   if (cyl_closed)
      for (int nx=0 ; nx<size_vx ; nx++)
           for (int nz=0 ; nz<size_vz ; nz++)
               setVertex (getVertexIJK (nx, 0, nz), nx, size_vy-1, nz);

   for (int nz=0 ; nz<size_vz ; nz++)
       for (int ny=0 ; ny<size_vy ; ny++)
           for (int nx=0 ; nx<size_vx ; nx++)
               {
               Vertex* v0 = getVertexIJK (nx, ny, nz  );
               Vertex* vx = getVertexIJK (nx+1, ny, nz);
               Vertex* vy = getVertexIJK (nx, ny+1, nz);
               Vertex* vz = getVertexIJK (nx, ny, nz+1);

               Edge* e1 = NULL;
               Edge* e2 = newEdge (v0, vy);
               Edge* e3 = NULL;

               if (cyl_closed && ny==size_vy-1)
                  {
                  e1 = getEdgeI (nx, 0, nz);
                  e3 = getEdgeK (nx, 0, nz);
                  }
               else
                  {
                  e3 = newEdge (v0, vz);
                  e1 = newEdge (v0, vx);
                  }

               setEdge (e1, dir_x, nx, ny, nz);
               setEdge (e2, dir_y, nx, ny, nz);
               setEdge (e3, dir_z, nx, ny, nz);
               }

   for (int nz=0 ; nz<size_vz ; nz++)
       for (int ny=0 ; ny<size_vy ; ny++)
           for (int nx=0 ; nx<size_vx ; nx++)
               {
               Edge* eae = getEdgeI (nx, ny,   nz);
               Edge* ebe = getEdgeI (nx, ny,   nz+1);
               Edge* eaf = getEdgeI (nx, ny+1, nz);

               Edge* eac = getEdgeJ (nx,   ny, nz);
               Edge* ead = getEdgeJ (nx+1, ny, nz);
               Edge* ebc = getEdgeJ (nx,   ny, nz+1);

               Edge* ece = getEdgeK (nx,   ny,   nz);
               Edge* ede = getEdgeK (nx+1, ny,   nz);
               Edge* ecf = getEdgeK (nx,   ny+1, nz);
               
               Quad* q1 = newQuad (eac, eaf, ead, eae);
               Quad* q2 = newQuad (eac, ecf, ebc, ece);
               Quad* q3 = NULL;
               Quad* q30 = getQuadIK (nx, 0 ,nz);

               if (cyl_closed && ny==size_vy-1 && q30!= NULL)
                  {
                  q3 = q30;
                  // Display(q3);
                  }
               else
                  {
                  q3 = newQuad (eae, ede, ebe, ece);
                  }

               setQuad (q1, dir_z, nx,ny,nz);
               setQuad (q2, dir_x, nx,ny,nz);
               setQuad (q3, dir_y, nx,ny,nz);
               }

   for (int nz=0 ; nz<size_hz ; nz++)
       for (int ny=0 ; ny<size_hy ; ny++)
           for (int nx=0 ; nx<size_hx ; nx++)
               {
               Quad* qa = getQuadIJ (nx, ny, nz);
               Quad* qb = getQuadIJ (nx, ny, nz+1);

               Quad* qc = getQuadIK (nx, ny,   nz);
               Quad* qd = getQuadIK (nx, ny+1, nz);

               Quad* qe = getQuadJK (nx,   ny, nz);
               Quad* qf = getQuadJK (nx+1, ny, nz);

               setHexa (newHexa (qa, qb, qc, qd, qe, qf), nx, ny, nz);
               }

   switch (cyl_dispo)
      {
      case CYL_CLOSED :
      case CYL_PEER   :  fillCenter ();
           break ; 
      case CYL_CL4 :     fillCenter4 ();
           break ; 
      case CYL_CL6 :     fillCenter6 ();
           break ; 
      case CYL_ODD :     fillCenterOdd ();
           break ; 
      case CYL_NOFILL  :
      default : ;
      }
   return HOK;
}
// ====================================================== fillCenter
// === Remplissage radial
#define IndElt(nc,nz)   (nbsecteurs*(nz) + nc)
#define IndRedge(nc,nz) (nbrayons  *(nz) + nc)
#define IndVquad(nc,nz) (nbrayons  *(nz-1) + nc)
void Elements::fillCenter ()
{
   int nx0 = 0;
   int nbsecteurs = size_hy / 2;
   int nbrayons   = cyl_closed ? nbsecteurs : nbsecteurs + 1;

   size_hplus  = nbsecteurs * size_hz;
   size_qhplus = nbsecteurs * size_vz;
   size_qvplus = nbrayons   * size_hz;
   size_ehplus = nbrayons   * size_vz;
   size_evplus = size_hz;

   ker_hexa .resize (size_hplus,  NULL);
   ker_hquad.resize (size_qhplus, NULL);
   ker_vquad.resize (size_qvplus, NULL);
   ker_hedge.resize (size_ehplus, NULL);
   ker_vedge.resize (size_evplus, NULL);

   Vertex* pcenter = NULL;

   for (int nz=0 ; nz<size_vz ; nz++)
       {
                                 //   Vertex central
       Vertex* center = getVertex (ker_vertex+nz);
                                 //   Edges horizontaux radiaux
       for (int nc=0 ; nc<nbrayons ; nc++)
           {
           Vertex* vv = getVertexIJK (nx0, 2*nc, nz);
           Edge*   edge = newEdge (center, vv);
           ker_hedge [IndRedge(nc,nz)] = edge;
           }
                                 //   Quads horizontaux
       for (int nc=0; nc<nbsecteurs ; nc++)
           {
           int nc1  = (nc + 1) MODULO nbrayons;
           Edge* e1 = ker_hedge [IndRedge (nc, nz)];
           Edge* e2 = getEdgeJ (nx0, 2*nc,  nz);
           Edge* e3 = getEdgeJ (nx0, 2*nc+1,nz);
           Edge* e4 = ker_hedge [IndRedge (nc1, nz)];

           ker_hquad [IndElt (nc,nz)] = newQuad (e1, e2, e3, e4);
           if (debug()) 
              {
	      printf ("hquad (%d,%d) = ", nc, nz);
              ker_hquad [IndElt (nc,nz)]->dumpPlus();
              }
           }

       if (nz>0)
          {
                                 //   Edges verticaux + cloisons interieures
          Edge* pilier = ker_vedge [nz-1] = newEdge (pcenter, center);

          for (int nc=0 ; nc<nbrayons ; nc++)
              {
              Edge* e1 = ker_hedge [IndRedge (nc, nz)];
              Edge* e2 = getEdgeK (nx0, 2*nc,  nz-1);
              Edge* e3 = ker_hedge [IndRedge (nc, nz-1)];
              ker_vquad [IndVquad (nc,nz)] = newQuad (e1, e2, e3, pilier);
              if (debug()) 
                 {
	         printf ("vquad (%d,%d) = ", nc, nz);
                 ker_vquad [IndVquad (nc,nz)]->dumpPlus();
                 }
              }
                                 //   Hexas
          for (int nc=0 ; nc < nbsecteurs ; nc++)
              {
              int nc1  = nc + 1;
              if (cyl_closed) 
                  nc1  = nc1 MODULO nbsecteurs;
              Quad* qa = ker_hquad [IndElt (nc, nz-1)];
              Quad* qb = ker_hquad [IndElt (nc, nz)];

              Quad* qc = ker_vquad [IndVquad (nc, nz)];
              Quad* qd = getQuadJK (nx0, 2*nc+1, nz-1);

              Quad* qe = getQuadJK (nx0, 2*nc,   nz-1);
              Quad* qf = ker_vquad [IndVquad (nc1,  nz)];

              if (debug()) 
                 {
                 printf (" --------------- Hexa : nc=%d, nz=%d\n", nc, nz);
	         HexDump (qa);
	         HexDump (qb);
	         HexDump (qc);
	         HexDump (qd);
	         HexDump (qe);
	         HexDump (qf);
                 }
              Hexa* cell = newHexa (qa, qb, qc, qd, qe, qf);
              tab_hexa.push_back (cell);
              ker_hexa [IndElt (nc,  nz-1)] = cell;
              }
          }
       pcenter = center;
       }
}
// ====================================================== fillCenter4
// === Remplissage radial
void Elements::fillCenter4 ()
{
   int   nx0  = 0;
   Quad* sol = NULL;

   for (int nz=0 ; nz<size_vz ; nz++)
       {
                                 //   Quad horizontal

       Quad* plafond = newQuad (getEdgeJ (nx0, 0,  nz), getEdgeJ (nx0, 1, nz), 
                                getEdgeJ (nx0, 2,  nz), getEdgeJ (nx0, 3, nz));

       if (nz>0)
          {
          Quad* qc = getQuadJK (nx0, 0, nz-1);
          Quad* qd = getQuadJK (nx0, 2, nz-1);
          Quad* qe = getQuadJK (nx0, 1, nz-1);
          Quad* qf = getQuadJK (nx0, 3, nz-1);

          if (debug()) 
             {
             printf (" --------------- Hexa grille4 : nz=%d\n", nz);
	     HexDump (plafond);
	     HexDump (sol);
	     HexDump (qc);
	     HexDump (qd);
	     HexDump (qe);
	     HexDump (qf);
             }
          Hexa* cell = newHexa (plafond, sol, qc, qd, qe, qf);
          tab_hexa.push_back (cell);
          }
       sol = plafond;
       }
}
// ====================================================== fillCenter6
void Elements::fillCenter6 ()
{
   int nx0 = 0;
   int nydemi = size_hy / 2;

   Edge* s_barre = NULL;
   Quad* sr_quad = NULL;
   Quad* sl_quad = NULL;

   for (int nz=0 ; nz<size_vz ; nz++)
       {
                                 //   Edges horizontal radial
       Edge* p_barre = newEdge (getVertexIJK (nx0, 0,      nz),
                                getVertexIJK (nx0, nydemi, nz));
                                 //   Quads horizontaux
       Edge* e0 = getEdgeJ (nx0, 0,  nz);
       Edge* e1 = getEdgeJ (nx0, 1,  nz);
       Edge* e2 = getEdgeJ (nx0, 2,  nz);
       Edge* e3 = getEdgeJ (nx0, 3,  nz);
       Edge* e4 = getEdgeJ (nx0, 4,  nz);
       Edge* e5 = getEdgeJ (nx0, 5,  nz);

       Quad* pl_quad = newQuad (e0, e1, e2, p_barre);
       Quad* pr_quad = newQuad (e3, e4, e5, p_barre);

       if (nz>0)
          {
                                 //   Cloison interieure
          Quad* cloison = newQuad (p_barre, getEdgeK (nx0, 0,       nz-1), 
                                   s_barre, getEdgeK (nx0, nydemi,  nz-1));
                                 //   Hexas
          Quad* q0 = getQuadJK (nx0, 0, nz-1);
          Quad* q1 = getQuadJK (nx0, 1, nz-1);
          Quad* q2 = getQuadJK (nx0, 2, nz-1);
          Quad* q3 = getQuadJK (nx0, 3, nz-1);
          Quad* q4 = getQuadJK (nx0, 4, nz-1);
          Quad* q5 = getQuadJK (nx0, 5, nz-1);

          Hexa* left  = newHexa (sl_quad, pl_quad, q0, q2,  q1, cloison);
          Hexa* right = newHexa (sr_quad, pr_quad, q3, q5,  q4, cloison);
          tab_hexa.push_back (left);
          tab_hexa.push_back (right);
          }
       s_barre = p_barre;
       sr_quad = pr_quad;
       sl_quad = pl_quad;
       }
}
// ====================================================== fillCenterOdd
#undef  IndElt
#define IndElt(nc,nz) (nbsecteurs*(nz) + nc)
void Elements::fillCenterOdd ()
{
   int nx0 = 0;
   int nbsecteurs = size_hy / 2;

   vector <Edge*> ker_hedge (nbsecteurs*size_vz);
   vector <Quad*> ker_hquad (nbsecteurs*size_vz);
   vector <Quad*> ker_vquad (nbsecteurs*size_vz);

   for (int nz=0 ; nz<size_vz ; nz++)
       {
                                 //   Edges horizontaux radiaux
       for (int nc=0 ; nc<nbsecteurs ; nc++)
           {
           int nc1 = size_hy - nc;
           Edge* edge = newEdge (getVertexIJK (nx0, nc,  nz), 
                                 getVertexIJK (nx0, nc1, nz));
           ker_hedge [IndElt(nc,nz)] = edge;
           }
                                 //   Quads horizontaux
       for (int nc=0; nc<nbsecteurs ; nc++)
           {
           Edge* e1 = ker_hedge [IndElt (nc, nz)];
           Edge* e2 = getEdgeJ (nx0, nc,           nz);
           Edge* e4 = getEdgeJ (nx0, size_hy-nc-1, nz);

           Edge* e3 = nc<nbsecteurs-1 ? ker_hedge [IndElt (nc+1, nz)]
                                      : getEdgeJ (nx0, nbsecteurs, nz);

           ker_hquad [IndElt (nc,nz)] = newQuad (e1, e2, e3, e4);
           if (debug()) 
              {
	      printf ("hquad (%d,%d) = ", nc, nz);
              ker_hquad [IndElt (nc,nz)]->dumpPlus();
              }
           }

       if (nz>0)
          {
                                 //   Edges verticaux + cloisons interieures
          for (int nc=0 ; nc<nbsecteurs ; nc++)
              {
              int nc1 = size_hy - nc;
              Edge* e1 = ker_hedge [IndElt (nc, nz)];
              Edge* e3 = ker_hedge [IndElt (nc, nz-1)];
              Edge* e2 = getEdgeK (nx0, nc,   nz-1);
              Edge* e4 = getEdgeK (nx0, nc1,  nz-1);
              ker_vquad [IndElt (nc,nz)] = newQuad (e1, e2, e3, e4);
              if (debug()) 
                 {
	         printf ("vquad (%d,%d) = ", nc, nz);
                 ker_vquad [IndElt (nc,nz)]->dumpPlus();
                 }
              }
                                 //   Hexas
          for (int nc=0 ; nc < nbsecteurs ; nc++)
              {
              int nc1 = size_hy - nc-1;
              Quad* qa = ker_hquad [IndElt (nc, nz-1)];
              Quad* qb = ker_hquad [IndElt (nc, nz)];

              Quad* qc = getQuadJK (nx0, nc,  nz-1);
              Quad* qd = getQuadJK (nx0, nc1, nz-1);

              Quad* qe = ker_vquad [IndElt (nc, nz)];
              Quad* qf = nc<nbsecteurs-1 ? ker_vquad [IndElt (nc+1,  nz)]
                                         : getQuadJK (nx0, nbsecteurs, nz-1);
              if (debug()) 
                 {
                 printf (" --------------- Hexa : nc=%d, nz=%d\n", nc, nz);
	         HexDump (qa);
	         HexDump (qb);
	         HexDump (qc);
	         HexDump (qd);
	         HexDump (qe);
	         HexDump (qf);
                 }
              Hexa* cell = newHexa (qa, qb, qc, qd, qe, qf);
              tab_hexa.push_back (cell);
              }
          }
       }
}
// --------------------------------------------------------------------------
// ----------------------------------------- Evols Hexa 3
// --------------------------------------------------------------------------

// ====================================================== makeBasicCylinder
// ==== Version avec vecteurs
int Elements::makeBasicCylinder (RealVector& tdr, RealVector& tda, 
                                 RealVector& tdh, bool fill)
{
   int na    = tda.size()-1;
   cyl_dispo = CYL_NOFILL;
   if (fill && na > 3)
      {
      if (cyl_closed)
         {
         if (na==4)
            cyl_dispo = CYL_CL4;
	 else if (na==6)
            cyl_dispo = CYL_CL6;
	 else if (na MODULO 2 == 0)
            cyl_dispo = CYL_CLOSED;
         }
      else if ((na MODULO 2)==0)
         cyl_dispo = CYL_PEER;
      else 
         cyl_dispo = CYL_ODD;
      }

   cyl_fill = cyl_dispo != CYL_NOFILL;

   double alpha  = 0;
   int    nb_secteurs = cyl_closed ? size_vy-1 : size_vy;

   for (int ny=0 ; ny<nb_secteurs ; ny++)
       {
       alpha  = tda[ny];
       double theta     = M_PI*alpha/180;
       double cos_theta = cos (theta);
       double sin_theta = sin (theta);

       for (int nx=0 ; nx<size_vx ; nx++)
           {
           double rayon = tdr [nx];
           double px = rayon*cos_theta;
           double py = rayon*sin_theta;

           for (int nz=0 ; nz<size_vz ; nz++)
               {
               double pz = tdh [nz];
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
                      // Les vertex centraux
   if (cyl_fill) 
      {
      ker_vertex = nbr_vertex;
      for (int nz=0 ; nz<size_vz ; nz++)
          {
          double pz = tdh [nz];
          Vertex* node = el_root->addVertex (0, 0, pz);
          tab_vertex.push_back (node);
          nbr_vertex ++;
          }
      }

   return HOK;
}
END_NAMESPACE_HEXA

