
// C++ : Classe Document : Methodes internes 2011

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

#include <cmath>
#include <map>

#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexQuad.hxx"
#include "HexHexa.hxx"

#include "HexLaw.hxx"

#include "HexAnaQuads.hxx"
#include "HexElements.hxx"
#include "HexCramer.hxx"
#include "HexGlobale.hxx"

BEGIN_NAMESPACE_HEXA

#define PermuterEdges(e1,e2) permuter_edges (e1, e2, #e1, #e2)
void    permuter_edges  (Edge* &e1, Edge* &e2, cpchar n1=NULL, cpchar n2=NULL);
double* prod_vectoriel (Edge* e1, Edge* e2, double result[]);

static bool db = false;

// ======================================================== copyDocument
Document* Document::copyDocument ()
{
   string nom = "CopyOf_";
   nom += el_name;
 
   Document* clone = new Document (nom.c_str());

   for (EltBase* elt = doc_first_elt[EL_VERTEX]->next (); elt!=NULL;
                 elt = elt->next())
       {
       if (elt !=NULL && elt->isHere())
          {
          Vertex* node = static_cast <Vertex*> (elt);
          node->duplicate (clone);
          }
       }

   for (int type=EL_EDGE ; type <= EL_HEXA ; type++)
       {
       for (EltBase* elt = doc_first_elt[type]->next (); elt!=NULL;
                     elt = elt->next())
           {
           if (elt !=NULL && elt->isHere())
              elt->duplicate ();
           }
       }

   for (int nro=0 ; nro<nbr_laws ; nro++)
       {
       Law* law = new Law (doc_laws [nro]);
       clone->doc_laws.push_back (law);
       }

   return clone;
}
// ---------------------------------------------------------------
// ============================================================== addHexa2quads
Hexa* Document::addHexa2Quads (Quad* q1, Quad* q2)
{
   DumpStart ("addHexa2Quads", q1 << q2);
   AnaQuads ana_quads (q1, q2);

   Hexa* hexa = NULL;
   if (ana_quads.status != HOK)
      hexa = NULL;

   else if (ana_quads.nbr_aretes == 0)
      hexa = addHexaQuadsAB (ana_quads);

   else if (ana_quads.nbr_aretes == 1)
      hexa = addHexaQuadsAC (ana_quads);

   DumpReturn (hexa);
   return hexa;
}
// ============================================================= addHexa3quads
Hexa* Document::addHexa3Quads (Quad* q1, Quad* q2, Quad* q3)
{
   DumpStart ("addHexa3Quads", q1 << q2 << q3);
   AnaQuads ana_quads (q1, q2, q3);

   Hexa* hexa = NULL;
   if (ana_quads.status != HOK)
      hexa = NULL;

   else if (ana_quads.nbr_aretes == 2)
      hexa = addHexaQuadsACD (ana_quads);

   else if (ana_quads.nbr_aretes == 3)
      hexa = addHexaQuadsACE (ana_quads);

   DumpReturn (hexa);
   return hexa;
}
// ============================================================= addHexa4quads
Hexa* Document::addHexa4Quads (Quad* q1, Quad* q2, Quad* q3, Quad* q4)
{
   DumpStart ("addHexa4Quads", q1 << q2 << q3 << q4);
   AnaQuads ana_quads (q1, q2, q3, q4);

   Hexa* hexa = NULL;
   if (ana_quads.status != HOK)
      hexa = NULL;

   else if (ana_quads.nbr_aretes == 4)
      hexa = addHexaQuadsABCD (ana_quads);

   else if (ana_quads.nbr_aretes == 5)
      hexa = addHexaQuadsACDE (ana_quads);

   DumpReturn (hexa);
   return hexa;
}
// ============================================================== addHexa5quads
Hexa* Document::addHexa5Quads (Quad* q1, Quad* q2, Quad* q3, Quad* q4, Quad* q5)
{
   DumpStart ("addHexa5Quads", q1 << q2 << q3 << q4 << q5);
   AnaQuads ana_quads (q1, q2, q3, q4, q5);
   if (ana_quads.status != HOK)
      return NULL;
   else if (ana_quads.nbr_aretes != 8)
      return NULL;
        
   int qbase = NOTHING;
   for (int nquad=0 ; nquad < ana_quads.nbr_quads ; nquad++)
       if (ana_quads.inter_nbre [nquad] == 4) 
          qbase = nquad;

   if (qbase == NOTHING)
      return NULL;

   Edge* tedge [QUAD4];
   Quad* tquad [QUAD4];
   for (int nedge=0 ; nedge < QUAD4 ; nedge++)
       {
       int nq    = ana_quads.inter_quad [qbase] [nedge];
       int ned1  = ana_quads.inter_edge [nq]    [qbase];
       int ned2  = (ned1 + 2) MODULO QUAD4;
       Quad* mur = ana_quads.tab_quads[nq];
       tedge [nedge] = mur->getEdge (ned2);
       tquad [nedge] = mur;
       }

   Quad*  q_a  = ana_quads.tab_quads[qbase];
   Quad*  q_b  = new Quad (tedge[0], tedge[1], tedge[2], tedge[3]);
   Hexa*  hexa = new Hexa (q_a, q_b, tquad[0], tquad[2], tquad[1], tquad[3]);

   DumpReturn (hexa);
   return hexa;
}
// ---------------------------------------------------------------
// ========================================================== addHexaquadsAB
Hexa* Document::addHexaQuadsAB (AnaQuads& strquads)
{
   Quad* q_a = strquads.tab_quads[0];
   Quad* q_b = strquads.tab_quads[1];

   double dmin  = 0;
   int    sens  = 1;
   int    decal = 0;
   for (int is = 0 ; is<2 ; is++)
       {
       int ns = 1-2*is;
       for (int ndec = 0 ; ndec<QUAD4 ; ndec++)
           {
           double dist = 0;
           for (int na = 0 ; na<QUAD4 ; na++)
               {
               int nb = (ndec + QUAD4 + ns*na) MODULO QUAD4; 
               dist += distance (q_a->getVertex (na),
                                 q_b->getVertex (nb));
               }
           if (ndec==0 && is==0)
              {
              decal = ndec;
              dmin  = dist;
              sens  = ns;
              }
           else if (dist<dmin) 
              {
              dmin  = dist;
              decal = ndec;
              sens  = ns;
              }
           }
       }

   Edge* tedge [QUAD4];
   Quad* tquad [QUAD4];
   for (int na = 0 ; na<QUAD4 ; na++)
       {
       int nb = (decal + QUAD4 + sens*na) MODULO QUAD4; 
       tedge [na] = new Edge (q_a->getVertex (na), q_b->getVertex (nb));
       }

   for (int nal = 0 ; nal<QUAD4 ; nal++)
       {
       int nar = (nal+1) MODULO QUAD4;
       Edge* e_left  = tedge [nal];
       Edge* e_right = tedge [nar];
       Edge* e_ax  = q_a->findEdge (e_left ->getVertex (V_AMONT), 
                                    e_right->getVertex (V_AMONT));
       Edge* e_bx  = q_b->findEdge (e_left ->getVertex (V_AVAL), 
                                    e_right->getVertex (V_AVAL));
       tquad [nal] = new Quad (e_ax, tedge [nal], e_bx, tedge [nar]);
       }

   Hexa* hexa = new Hexa (q_a, q_b, tquad[0], tquad[2], tquad[1], tquad[3]);
   return hexa;
}
// ========================================================== addHexaquadsAC
Hexa* Document::addHexaQuadsAC (AnaQuads& strquads)
{
   Quad* q_a = strquads.tab_quads[0];
   Quad* q_c = strquads.tab_quads[1];

   Vertex* tv_bdx [V_TWO];   // x = e ou f
   Edge*   te_dX  [V_TWO];
   Edge*   te_bX  [V_TWO];
   Quad*   tq_ef  [V_TWO];

   int neda0  = strquads.inter_edge [0] [1];

   Edge* e_ac = q_a->getEdge (neda0);

   for (int ns=V_AMONT; ns<=V_AVAL ; ns++)
       {
       Vertex* vx1 = e_ac->getVertex (ns);
       Vertex* vx2 = e_ac->getVertex (1-ns);

       int nda2 = q_a->indexVertex (vx2); 
       nda2 = (nda2 +2) MODULO QUAD4;

       int ndc2 = q_c->indexVertex (vx2); 
       ndc2 = (ndc2 +2) MODULO QUAD4;

       Vertex* vxa = q_a->getVertex (nda2);
       Vertex* vxc = q_c->getVertex (ndc2);

       double dx  = (vxa->getX() - vx1->getX()) + (vxc->getX() - vx1->getX());
       double dy  = (vxa->getY() - vx1->getY()) + (vxc->getY() - vx1->getY());
       double dz  = (vxa->getZ() - vx1->getZ()) + (vxc->getZ() - vx1->getZ());
       tv_bdx [ns] = new Vertex (this, vx1->getX()+dx, vx1->getY()+dy,  
                                                      vx1->getZ()+dz);
       Edge* edga = q_a->findEdge (vx1, vxa);
       Edge* edgc = q_c->findEdge (vx1, vxc);

       te_dX [ns] = new Edge (vxa, tv_bdx[ns]);
       te_bX [ns] = new Edge (vxc, tv_bdx[ns]);
       tq_ef [ns] = new Quad (edga, te_dX[ns], te_bX[ns], edgc); 
       }

   int ff = 0;
   Edge* e_bd = new Edge (tv_bdx[V_AMONT], tv_bdx[V_AVAL]);
   Edge* e_ad = q_a->getOpposEdge (e_ac, ff); 
   Edge* e_bc = q_c->getOpposEdge (e_ac, ff); 
   
   Quad* q_d = new Quad (e_bd, te_dX[V_AMONT], e_ad, te_dX[V_AVAL]);
   Quad* q_b = new Quad (e_bd, te_bX[V_AMONT], e_bc, te_bX[V_AVAL]);

   Hexa*  hexa  = new Hexa (q_a, q_b, q_c, q_d, tq_ef[V_AMONT], tq_ef[V_AVAL]);
   return hexa;
}
// ========================================================= addHexaquadsACE
// ==== Construction d'un hexaedre a partir d'un triedre
/*
       6=bed  +----bd-----+ bdf=7
             /|          /|
           be |   B    bf |
           /  |        /  |
    4=bce +----bc-----+...|...bcf=5
          |  de     D |   df
          | E |       | F |             z
         ce   | C     cf  |             ^
  2=ade...|...+----ad-|---+ adf=3       |   y
          |  /        |  /              |  /
          | ae    A   | af              | /
          |/          |/                |/
    0=ace +----ac-----+ acf=1           +----->  x
                
On connait les faces A, C, E
Il faut determiner le point bdf, intersection des faces B, D, F

bdf in B : Scalaire ((bdf-bce), Vectoriel (be, bc)) = 0
bdf in D : Scalaire ((bdf-ade), Vectoriel (ad, de)) = 0
bdf in F : Scalaire ((bdf-acf), Vectoriel (af, cf)) = 0

Soit 3 inconnues (Xbdf, Ybdf, Zbdf) et 3 equations
Un merci a Francois Mougery qui m'a rappele quelques notions de geometrie 3D

Le systeme s'ecrit :

  Scalaire ((M-bce), norm_b) = 0
  Scalaire ((M-ade), morm_d) = 0
  Scalaire ((M-acf), morm_f) = 0

<=>

  Scalaire (M, norm_b) = Scalaire (bce, norm_b)
  scalaire (M, norm_d) = Scalaire (ade, norm_b)
  scalaire (M, norm_f) = Scalaire (acf, norm_b)

<=>
   norme_b.x*X + norme_b.y*Y + norme_b.z*Z = K1
   norme_d.x*X + norme_d.y*Y + norme_d.z*Z = K2
   norme_f.x*X + norme_f.y*Y + norme_f.z*Z = K3

 * ----------------------------------------------------- */
Hexa* Document::addHexaQuadsACE (AnaQuads& strquads)
{
   Real3 v_bce,  v_ade,  v_acf, v_bdf;    // Sommets
   Real3 norm_b, norm_d, norm_f;   // Normales aux faces
   int   ff = 0;

   Quad* q_a = strquads.tab_quads[0];
   Quad* q_c = strquads.tab_quads[1];
   Quad* q_e = strquads.tab_quads[2];

   Edge* e_ac = q_a->commonEdge (q_c);
   Edge* e_ae = q_a->commonEdge (q_e);
   Edge* e_ce = q_c->commonEdge (q_e);

   Edge* e_ad = q_a->getOpposEdge (e_ac, ff);
   Edge* e_af = q_a->getOpposEdge (e_ae, ff);
   Edge* e_cf = q_c->getOpposEdge (e_ce, ff);

   Edge* e_bc = q_c->getOpposEdge (e_ac, ff);
   Edge* e_be = q_e->getOpposEdge (e_ae, ff);
   Edge* e_de = q_e->getOpposEdge (e_ce, ff);

   e_ce->commonPoint (e_bc, v_bce);
   e_ae->commonPoint (e_ad, v_ade);
   e_ac->commonPoint (e_af, v_acf);

   prod_vectoriel (e_be, e_bc, norm_b);  // Calcul normale a la face B
   prod_vectoriel (e_ad, e_de, norm_d);  // Calcul normale a la face D
   prod_vectoriel (e_af, e_cf, norm_f);  // Calcul normale a la face F

   Real3 membre2 = { prod_scalaire (v_bce, norm_b), 
                     prod_scalaire (v_ade, norm_d), 
                     prod_scalaire (v_acf, norm_f) };

   double matrix [] = { norm_b[dir_x],  norm_b[dir_y],  norm_b[dir_z],  
                        norm_d[dir_x],  norm_d[dir_y],  norm_d[dir_z],  
                        norm_f[dir_x],  norm_f[dir_y],  norm_f[dir_z] };
   Cramer systeme (DIM3);
   int ier = systeme.resoudre (matrix, membre2, v_bdf);
   if (ier != HOK)
      {
      printf (" addHexaQuadsACE : systeme impossible\n");
      return NULL;
      }

   Vertex* s_bdf = new Vertex (this, v_bdf[dir_x], v_bdf[dir_y], v_bdf[dir_z]);
   Vertex* s_bde = e_be -> commonVertex (e_de);
   Vertex* s_bcf = e_bc -> commonVertex (e_cf);
   Vertex* s_adf = e_af -> commonVertex (e_ad);
   
   Edge* e_bd = new Edge (s_bdf, s_bde);
   Edge* e_bf = new Edge (s_bdf, s_bcf);
   Edge* e_df = new Edge (s_bdf, s_adf);

   Quad* q_b = new Quad (e_bc, e_be, e_bd, e_bf);
   Quad* q_d = new Quad (e_de, e_ad, e_df, e_bd);
   Quad* q_f = new Quad (e_af, e_cf, e_bf, e_df);

   Hexa*  hexa = new Hexa (q_a, q_b, q_c, q_d, q_e, q_f);
   return hexa;
}
// ========================================================= addHexaquadsACD
// ==== Construction d'un hexaedre a partir d'un U
Hexa* Document::addHexaQuadsACD (AnaQuads& strquads)
{
   int pos_a = NOTHING;
   for (int np=0 ; np<3 && pos_a==NOTHING ; np++)
       if (strquads.inter_nbre[np]==2) 
          pos_a = np;

   if (pos_a==NOTHING) 
      return NULL;

   int pos_c = (pos_a+1) MODULO 3;
   int pos_d = (pos_a+2) MODULO 3;
   Quad* q_a = strquads.tab_quads[pos_a];
   Quad* q_c = strquads.tab_quads[pos_c];
   Quad* q_d = strquads.tab_quads[pos_d];

   int   na_ac = strquads.inter_edge[pos_a][pos_c]; // Nro dans  q_a de e_ac
   int   nc_ac = strquads.inter_edge[pos_c][pos_a]; // Nro dans  q_c de e_ac
   int   nd_ad = strquads.inter_edge[pos_d][pos_a]; // Nro dans  q_d de e_ad

   Edge* e_ae  = q_a->getEdge ((na_ac + 1) MODULO QUAD4); // Arbitraire
   Edge* e_af  = q_a->getEdge ((na_ac + 3) MODULO QUAD4); // Arbitraire

   Edge* e_bc  = q_c->getEdge ((nc_ac + 2) MODULO QUAD4); 
   Edge* e_bd  = q_d->getEdge ((nd_ad + 2) MODULO QUAD4);

   Edge* e_ce  = q_c->getEdge ((nc_ac + 1) MODULO QUAD4);
   Edge* e_cf  = q_c->getEdge ((nc_ac + 3) MODULO QUAD4);

   Edge* e_de  = q_d->getEdge ((nd_ad + 1) MODULO QUAD4);
   Edge* e_df  = q_d->getEdge ((nd_ad + 3) MODULO QUAD4);

   Vertex* v_ace = e_ae->commonVertex (e_ce);
   Vertex* v_ade = e_ae->commonVertex (e_de);
   if (v_ace==NULL)
      {
      permuter_edges (e_ce, e_cf);
      v_ace = e_ae->commonVertex (e_ce);
      }

   if (v_ade==NULL)
      {
      permuter_edges (e_de, e_df);
      v_ade = e_ae->commonVertex (e_de);
      }

   Vertex* v_acf = e_af->commonVertex (e_cf);
   Vertex* v_adf = e_af->commonVertex (e_df);

   Vertex* v_bce = e_ce->opposedVertex (v_ace);
   Vertex* v_bde = e_de->opposedVertex (v_ade);
   Vertex* v_bcf = e_cf->opposedVertex (v_acf);
   Vertex* v_bdf = e_df->opposedVertex (v_adf);

   Edge*  e_be = new Edge (v_bce, v_bde);
   Edge*  e_bf = new Edge (v_bcf, v_bdf);

   Quad* q_b = new Quad  (e_be, e_bc, e_bf, e_bd);
   Quad* q_e = new Quad  (e_ae, e_ce, e_be, e_de);
   Quad* q_f = new Quad  (e_af, e_cf, e_bf, e_df);

   Hexa*  hexa = new Hexa (q_a, q_b, q_c, q_d, q_e, q_f);
   return hexa;
}
// ========================================================= addHexaquadsABCD
Hexa* Document::addHexaQuadsABCD (AnaQuads& strquads)
{
   int pos_a = 0;
   int pos_b = NOTHING;
   int pos_c = NOTHING;
   int pos_d = NOTHING;

   for (int np=1 ; np<4 ; np++)
       {
       if (strquads.inter_edge [pos_a] [np] == NOTHING )
          pos_b = np; 
       else if (pos_c==NOTHING)
          pos_c = np; 
       else 
          pos_d = np; 
       }
     
   if (pos_b==NOTHING || pos_c==NOTHING || pos_d==NOTHING) 
      return NULL;

   Quad* q_a = strquads.tab_quads [pos_a];
   Quad* q_b = strquads.tab_quads [pos_b];
   Quad* q_c = strquads.tab_quads [pos_c];
   Quad* q_d = strquads.tab_quads [pos_d];
   
   int   na_ac = strquads.inter_edge[pos_a][pos_c]; // Nro dans  q_a de e_ac
   int   nc_ac = strquads.inter_edge[pos_c][pos_a]; // Nro dans  q_c de e_ac
   int   nd_ad = strquads.inter_edge[pos_d][pos_a]; // Nro dans  q_d de e_ad
   int   nb_bc = strquads.inter_edge[pos_b][pos_c]; // Nro dans  q_b de e_bc

   Edge* e_ae  = q_a->getEdge ((na_ac + 1) MODULO QUAD4);
   Edge* e_af  = q_a->getEdge ((na_ac + 3) MODULO QUAD4);

   Edge* e_ce  = q_c->getEdge ((nc_ac + 1) MODULO QUAD4);
   Edge* e_cf  = q_c->getEdge ((nc_ac + 3) MODULO QUAD4);

   Edge* e_de  = q_d->getEdge ((nd_ad + 1) MODULO QUAD4);
   Edge* e_df  = q_d->getEdge ((nd_ad + 3) MODULO QUAD4);

   Edge*  e_be = q_b->getEdge ((nb_bc + 1) MODULO QUAD4); 
   Edge*  e_bf = q_b->getEdge ((nb_bc + 3) MODULO QUAD4); 

   if (db)
      {
      HexDump (q_a);
      HexDump (q_b);
      HexDump (q_c);
      HexDump (q_d);

      HexDump (e_ae);
      HexDump (e_af);
      HexDump (e_ce);
      HexDump (e_cf);
      HexDump (e_de);
      HexDump (e_df);
      HexDump (e_be);
      HexDump (e_bf);
      }

   if (e_ae->commonVertex (e_ce) == NULL)
      PermuterEdges (e_ce, e_cf);

   if (e_ae->commonVertex (e_de) == NULL)
      PermuterEdges (e_de, e_df);

   if (e_ce->commonVertex (e_be) == NULL)
      PermuterEdges (e_be, e_bf);

   Quad* q_e = new Quad  (e_ae, e_ce, e_be, e_de);
   Quad* q_f = new Quad  (e_af, e_cf, e_bf, e_df);

   Hexa*  hexa = new Hexa (q_a, q_b, q_c, q_d, q_e, q_f);
   return hexa;
}
// ========================================================= addHexaquadsACDE
Hexa* Document::addHexaQuadsACDE (AnaQuads& strquads)
{
   int pos_a = NOTHING;
   int pos_c = NOTHING;
   int pos_d = NOTHING;
   int pos_e = NOTHING;

   for (int np=0 ; np<4  ; np++)
       if (strquads.inter_nbre[np]==3) 
          {
          if (pos_a == NOTHING) pos_a = np;
             else               pos_e = np;
          }
       else if (strquads.inter_nbre[np]==2) 
          {
          if (pos_c == NOTHING) pos_c = np;
             else               pos_d = np;
          }

   if (pos_a==NOTHING || pos_c==NOTHING  || pos_d==NOTHING || pos_e==NOTHING)
      return NULL;

   Quad* q_a = strquads.tab_quads[pos_a];
   Quad* q_c = strquads.tab_quads[pos_c];
   Quad* q_d = strquads.tab_quads[pos_d];
   Quad* q_e = strquads.tab_quads[pos_e];

   int   na_ac = strquads.inter_edge[pos_a][pos_c]; // Nro dans  q_a de e_ac

   int   nc_ac = strquads.inter_edge[pos_c][pos_a]; // Nro dans  q_c de e_ac
   int   nc_ce = strquads.inter_edge[pos_c][pos_e]; // Nro dans  q_c de e_ce

   int   nd_ad = strquads.inter_edge[pos_d][pos_a]; // Nro dans  q_d de e_ad
   int   nd_de = strquads.inter_edge[pos_d][pos_e]; // Nro dans  q_d de e_de

   int   ne_ae = strquads.inter_edge[pos_e][pos_a]; // Nro dans  q_e de e_ae

   Edge* e_af  = q_a->getEdge ((na_ac + 3) MODULO QUAD4);
   Edge* e_bc  = q_c->getEdge ((nc_ac + 2) MODULO QUAD4);
   Edge* e_cf  = q_c->getEdge ((nc_ce + 2) MODULO QUAD4);
   Edge* e_bd  = q_d->getEdge ((nd_ad + 2) MODULO QUAD4);
   Edge* e_df  = q_d->getEdge ((nd_de + 2) MODULO QUAD4);
   Edge* e_be  = q_e->getEdge ((ne_ae + 2) MODULO QUAD4);

   Vertex* v_bcf = e_cf->opposedVertex (e_cf->commonVertex (e_af));
   Vertex* v_bdf = e_df->opposedVertex (e_df->commonVertex (e_af));

   Edge*   e_bf = new Edge (v_bcf, v_bdf);
   Quad*   q_b  = new Quad (e_be, e_bc, e_bf, e_bd);
   Quad*   q_f  = new Quad (e_af, e_cf, e_bf, e_df);

   Hexa*  hexa = new Hexa (q_a, q_b, q_c, q_d, q_e, q_f);
   return hexa;
}
// ========================================================= replaceHexa
Elements* Document::replaceHexa (Quads pattern, Vertex* p1, Vertex* c1, 
                             Vertex* p2, Vertex* c2, Vertex* p3, Vertex* c3)
{
   DumpStart ("replaceHexa", pattern << p1 << c1 << p2 << c2 << p3 << c3);

   Elements* t_hexas = new Elements (this);
   Mess << " **** This syntax is deprecated" ;
   t_hexas->setError (HERR);

   DumpReturn (t_hexas);
   return      t_hexas;
}
// ========================================================= replace
Elements* Document::replace (Quads motif, Quads cible, Vertex* p1, Vertex* c1,                             Vertex* p2, Vertex* c2)
{
   DumpStart ("replace", motif << cible << p1 << c1 << p2 << c2);

   Elements* t_hexas = new Elements (this);
   int ier = t_hexas->replaceHexas (motif, cible, p1, c1, p2, c2);

   if (ier!=HOK)
      {
      Mess << " **** Error in Document::replace" ;
      t_hexas->setError (HERR);
      }

   DumpReturn (t_hexas);
   return t_hexas;
}
// ========================================================= print_replace
void print_replace (Edge* zig, Edge*  zag)
{
   cout << zig->getName() << " = (" << zig->getVertex(0)->getName() 
        << ", " << zig->getVertex(1)->getName() << ") est clone en ";

   cout << zag->getName() << " = (" << zag->getVertex(0)->getName() 
        << ", " << zag->getVertex(1)->getName() << ")" << endl;
}
// ========================================================= only_in_hexas
bool only_in_hexas (Hexas& thexas, Quad* quad)
{
   int nbhexas = thexas.size();
   int nbp     = quad->getNbrParents();
   for (int nh=0 ; nh <nbp ; nh++)
       {
       bool pasla = true;
       Hexa* hexa = quad->getParent (nh); 
       for (int nc=0 ; pasla && nc < nbhexas ; nc++)
           pasla = hexa != thexas [nc];
       if (pasla) 
           return false;
       }
   return true;
}
// ========================================================= only_in_hexas
bool only_in_hexas (Hexas& thexas, Edge*  edge)
{
   return false;
   int nbp = edge->getNbrParents();
   for (int nq=0 ; nq <nbp ; nq++)
       {
       Quad* quad = edge->getParent   (nq); 
       if (NOT only_in_hexas (thexas, quad))
          {
          cout << " ... inMoreHexas " << edge->makeDefinition() << endl; 
          return false;
          }
       }
   cout << " ... only_in_hexas " << edge->makeDefinition() << endl; 
   return true;
}
// ========================================================= replace_vertex
void replace_vertex (Hexas& thexas, Vertex* node,  Vertex* par)
{
   int nbh = thexas.size();
   for (int nh=0 ; nh <nbh ; nh++)
       thexas[nh]->replaceVertex (node, par);
}
// ========================================================= disconnectEdges
Elements* Document::disconnectEdges (Hexas thexas, Edges  tedges)
{
   DumpStart ("disconnectEdges",  thexas << tedges);
   
   if (db)
      cout << " +++ Disconnect Edges" << endl;

   Elements* grid  = new Elements (this);

   grid->checkDisco (thexas, tedges);

   int nbedges = tedges.size();
   int nbhexas = thexas.size();

   if (nbhexas != nbedges) 
      {
      cout << " **** Error in Document::disconnectEdges\n" << endl;
      cout << " **** Number of Edges and number of Hexas are different\n" 
           << endl;
      return NULL;
      }
   else if (nbhexas==1)
      {
      update ();
      thexas[0]->disconnectEdge (tedges[0], grid);
      return    grid;
      }

   for (int nro=0 ; nro<nbedges ; nro++)
       {
       if (BadElement (tedges[nro]))
          {
          cout << " **** Eddge number " << nro+1 << " is incorrect"
               << endl;
          return NULL;
          }
       if (BadElement (thexas[nro]))
          {
          cout << " **** Hexa number " << nro+1 << " is incorrect"
               << endl;
          return NULL;
          }
       if (db)
          cout << nro+1 << " hexa = " << thexas[nro]->getName () 
                        << ", edge = " << tedges[nro]->getName () 
                        << " = (" << tedges[nro]->getVertex(0)->getName () 
                        << ", "   << tedges[nro]->getVertex(1)->getName () 
                        << ")" << endl;
       }

   for (int nro=0 ; nro<nbhexas ; nro++)
       {
       int ned = thexas[nro]->findEdge (tedges[nro]);
       if (ned==NOTHING)
          {
          cout << " **** Edge number " << nro+1 
               << " doesnt belong to correspondant hexa" << endl;
          return NULL;
          }
       }

   vector <Vertex*> tvertex (nbedges+1);

   for (int nro=1 ; nro<nbedges ; nro++)
       {
       tvertex[nro] = tedges[nro]->commonVertex (tedges[nro-1]);
       if (tvertex[nro]==NULL)
          {
          cout << " **** Edge number " << nro 
               << " doesnt intesect next edge" << endl;
          return NULL;
          }
       }

   int nv0 = tedges[0]        ->inter (tedges[1]);
   int nvn = tedges[nbedges-1]->inter (tedges[nbedges-2]);
   tvertex [0]       = tedges[0]        ->getVertex (1-nv0); 
   tvertex [nbedges] = tedges[nbedges-1]->getVertex (1-nvn); 

   for (int nro=0 ; nro<nbhexas ; nro++)
       {
       int ned = thexas[nro]->findEdge (tedges[nro]);
       if (ned==NOTHING)
          {
          cout << " **** Edge number " << nro+1 
               << " doesnt belong to correspondant hexa" << endl;
          return NULL;
          }
       }
                      // Fin des controles, on peut y aller ...

   map <Edge*, int> state_edge;
   map <Quad*, int> state_quad;
   enum { UNDEFINED, REPLACED, AS_IS };

   map <Vertex*, Vertex*> new_vertex;
   map <Edge*,   Edge*>   new_edge;
   map <Quad*,   Quad*>   new_quad;

   map <Vertex*, Vertex*> :: iterator it_vertex;
   map <Edge*,   Edge*>   :: iterator it_edge;
   map <Quad*,   Quad*>   :: iterator it_quad;

#define VertexIsNew(v) (it_vertex=new_vertex.find(v))!=new_vertex.end()

   Vertex*   node1     = NULL;

   for (int nro=0 ; nro<=nbedges ; nro++)
       {
       Vertex* node0 = node1;
       node1 = new Vertex (tvertex[nro]);
       grid->addVertex  (node1);
       new_vertex [tvertex[nro]] = node1;
       if (db)
          {
          cout << nro << " : "         << tvertex[nro]->getName() 
               << " est clone en " << node1->getName() << endl;
          }

       if (nro>0)
          {
          Edge* edge = new Edge (node0, node1);
          grid->addEdge  (edge);
          new_edge   [tedges[nro-1]] = edge;
          state_edge [tedges[nro-1]] = REPLACED;
          if (db)
             print_replace (tedges[nro-1], edge);
          }
       }

   if (db)
      cout << "_____________________________ Autres substitutions" << endl;

   // Un edge non remplace, qui contient un vertex remplace
   //         commun a plus de 2 faces (donc appartenant a un autre hexa)
   //         doit etre duplique

   for (int nro=0 ; nro<nbhexas ; nro++)
       {
       Hexa* hexa = thexas [nro]; 
       for (int nro=0 ; nro<HE_MAXI ; nro++)
           {
           Edge* edge = hexa->getEdge (nro);
           if (state_edge[edge]==UNDEFINED)
              {
              Vertex* v1 = edge->getVertex (V_AMONT);
              Vertex* v2 = edge->getVertex (V_AVAL);
              int etat = REPLACED;
              if (VertexIsNew (v1))
                 {
                 if (only_in_hexas (thexas, edge))
                    {
                    replace_vertex (thexas, v1, new_vertex[v1]);
                    etat = AS_IS;
                    }
                 else
                    v1 = new_vertex [v1];
                 }
              else if (VertexIsNew (v2))
                 {
                 if (only_in_hexas (thexas, edge))
                    {
                    replace_vertex (thexas, v2, new_vertex[v2]);
                    etat = AS_IS;
                    }
                 else
                    v2 = new_vertex [v2];
                 }
              else 
                 etat = AS_IS;

              if (etat==REPLACED)
                 {
                 Edge* arete = new Edge (v1, v2);
                 new_edge   [edge] = arete;
                 grid->addEdge  (arete);
                 if (db)
                    print_replace (edge, arete);
                 }
              state_edge [edge] = etat;
              }
           }
       }

   // Un quad non remplace, qui contient un edge remplace 
   //         commun a plus de 2 Hexas
   //         doit etre duplique

   for (int nro=0 ; nro<nbhexas ; nro++)
       {
       Hexa* hexa = thexas [nro]; 
       for (int nro=0 ; nro<HQ_MAXI ; nro++)
           {
           Quad* quad = hexa->getQuad (nro);
           if (state_quad[quad]==UNDEFINED)
              {
              Edge* ted [QUAD4];
              int etat = AS_IS;
              for (int ned=0 ; ned < QUAD4 ; ned++)
                  {
                  Edge* edge = quad->getEdge (ned);
                  if (state_edge [edge]==AS_IS)
                      ted[ned] = edge;
                  else 
                      {
                      ted[ned] = new_edge[edge];
                      etat = REPLACED;
                      }
                  }
              if (etat==REPLACED)
                 {
                 Quad* face = new Quad (ted[0], ted[1], ted[2], ted[3]);
                 new_quad   [quad] = face;
                 grid->addQuad  (face);
                 }
              state_quad [quad] = etat;
              }
           }
       }

   for (int nro=0 ; nro<nbhexas ; nro++)
       {
       Hexa* hexa = thexas [nro]; 
       for (it_quad=new_quad.begin(); it_quad != new_quad.end() ; ++it_quad)
           {
           Quad* pile = it_quad->first;
           Quad* face = it_quad->second;
           hexa->replaceQuad (pile, face);
           }

       for (it_edge=new_edge.begin(); it_edge != new_edge.end() ; ++it_edge)
           {
           Edge* zig = it_edge->first;
           Edge* zag = it_edge->second;
           hexa->replaceEdge (zig, zag);
           }

       for (it_vertex=new_vertex.begin(); 
            it_vertex != new_vertex.end() ; ++it_vertex)
           {
           Vertex* flip = it_vertex->first;
           Vertex* flop = it_vertex->second;
           hexa->replaceVertex (flip, flop);
           }
       }
         
   Real3  center0, center1; 
   Matrix matrix;
   Hexa*  hexa0 = NULL;
   Hexa*  hexa1 = NULL;
   for (int nro=0 ; nro<=nbhexas ; nro++)
       {
       hexa0 = hexa1;
       if (nro==0)
          {
          hexa1 = thexas [nro]; 
          hexa1->getCenter (center1);
          }
       else if (nro==nbhexas)
          {
          hexa1->getCenter (center1);
          }
       else 
          {
          hexa1 = thexas [nro]; 
          hexa0->getCenter (center0);
          hexa1->getCenter (center1);
          for (int nc=0 ; nc<DIM3 ; nc++)
              center1[nc] = (center0[nc] + center1[nc])/2;
          }

       Vertex* node  = grid->getVertex (nro);
       matrix.defScale (center1, 0.55);
       matrix.perform  (node);
       }

   DumpReturn (grid);
   return      grid;
}
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
// ========================================================= prod_vectoriel
double*  prod_vectoriel (Edge* e1, Edge* e2, double prod[])
{
   prod [dir_x] = prod [dir_y] = prod [dir_z] = 0;
   if (e1==NULL || e2==NULL) 
      return prod;

   Real3 v1, v2;
   e1->getVector (v1);
   e2->getVector (v2);

   prod [dir_x] = v1[dir_y] * v2[dir_z] - v2[dir_y] * v1[dir_z];
   prod [dir_y] = v1[dir_z] * v2[dir_x] - v2[dir_z] * v1[dir_x];
   prod [dir_z] = v1[dir_x] * v2[dir_y] - v2[dir_x] * v1[dir_y];

   return prod;
}
// ========================================================= permuter_edges
void permuter_edges (Edge* &e1, Edge* &e2, cpchar nm1, cpchar nm2)
{
   if (db && nm1!=NULL)
      {
      printf (" ... permuter_edges %s = %s et %s = %s\n", 
                    nm1, e1->getName(), nm2, e2->getName() );
      }

   Edge* foo = e1;
   e1  = e2;
   e2  = foo;
}
END_NAMESPACE_HEXA
