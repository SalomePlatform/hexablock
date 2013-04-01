
// C++ : Gestion des cylindres croises

// Copyright (C) 2009-2013  CEA/DEN, EDF R&D
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

#include "HexBiCylinder.hxx"

#include "HexDocument.hxx"
#include "HexVector.hxx"
#include "HexVertex.hxx"
#include "HexHexa.hxx"
#include "HexEdge.hxx"

#include "HexGlobale.hxx"
#include "HexCylinder.hxx"

static bool   db  = false;

BEGIN_NAMESPACE_HEXA

static const double cos45 = cos (M_PI/4);


void geom_define_line (string& brep);
void geom_asso_point  (double angle, Vertex* node);

void geom_create_circle (double* milieu, double rayon, double* normale,
                         double* base, string& brep);
int  geom_create_cylcyl (double* borig, double* bnorm, double* bbase,
                         double  bray,  double  bhaut,
                         double* sorig, double* snorm, double* sbase,
                         double  sray,  double  shaut);
int  geom_asso_cylcyl   (Edge* edge);

// ====================================================== Constructeur
BiCylinder::BiCylinder (Document* doc)
          : Elements (doc)
{
   cyl_fill     = false;
   cross_cyl1   = NULL;
   cross_cyl1   = NULL;
   cross_cyl2   = NULL;
   grid_type    = GR_BICYL;
   at_right  = at_left = true;

   tab_hexa.push_back   (NULL);
   tab_quad.push_back   (NULL);
   tab_edge.push_back   (NULL);
   tab_vertex.push_back (NULL);
   nbr_vertex = nbr_edges = nbr_quads = nbr_hexas = 1;
}
// ====================================================== getHexaIJK
Hexa* BiCylinder::getHexaIJK (int cyl, int nx, int ny, int nz)
{
   int key = getKey (cyl, nx, ny, nz);
   int nro = map_hexa [key];
   return tab_hexa [nro];
}
// ====================================================== getQuadIJ
Quad* BiCylinder::getQuadIJ (int cyl, int nx, int ny, int nz)
{
   int key = getKey (cyl, dir_z, nx, ny, nz);
   int nro = map_quad [key];
   return tab_quad [nro];
}
// ====================================================== getQuadJK
Quad* BiCylinder::getQuadJK (int cyl, int nx, int ny, int nz)
{
   int key = getKey (cyl, dir_x, nx, ny, nz);
   int nro = map_quad [key];
   return tab_quad [nro];
}
// ====================================================== getQuadIK
Quad* BiCylinder::getQuadIK (int cyl, int nx, int ny, int nz)
{
   int key = getKey (cyl, dir_y, nx, ny, nz);
   int nro = map_quad [key];
   return tab_quad [nro];
}
// ====================================================== getEdgeI
Edge* BiCylinder::getEdgeI (int cyl, int nx, int ny, int nz)
{
   int key = getKey (cyl, dir_x, nx, ny, nz);
   int nro = map_edge [key];
   return tab_edge [nro];
}
// ====================================================== getEdgeJ
Edge* BiCylinder::getEdgeJ (int cyl, int nx, int ny, int nz)
{
   int key = getKey (cyl, dir_y, nx, ny, nz);
   int nro = map_edge [key];
   return tab_edge [nro];
}
// ====================================================== getEdgeK
Edge* BiCylinder::getEdgeK (int cyl, int nx, int ny, int nz)
{
   int key = getKey (cyl, dir_z, nx, ny, nz);
   int nro = map_edge [key];
   return tab_edge [nro];
}
// ====================================================== getVertexIJK
Vertex* BiCylinder::getVertexIJK (int cyl, int nx, int ny, int nz)
{
   int key = getKey (cyl, nx, ny, nz);
   int nro = map_vertex [key];
   if (db)
      {
      printf ("getVertexIJK (%d, %d,%d,%d) = V%d = ", cyl, nx, ny, nz, nro);
      if (tab_vertex[nro] == NULL)
         printf ("NULL\n");
      else
         printf ("%s = (%g, %g, %g)\n", tab_vertex[nro]->getName(),
                 tab_vertex[nro]->getX(), tab_vertex[nro]->getY(),
                 tab_vertex[nro]->getZ());
      }
   return tab_vertex [nro];
}
// ------------------------------------------------------------------------
// ====================================================== addVertex
Vertex* BiCylinder::addVertex (double px, double py, double pz, int cyl,
                               int nx, int ny, int nz)
{
   Vertex* node = el_root->addVertex (px, py, pz);
   int     key  = getKey (cyl, nx, ny, nz);
   tab_vertex.push_back (node);
   map_vertex [key] = nbr_vertex;

   if (db)
      {
      printf (" tab_vertex [%d, %d,%d,%d] = V%d = ", cyl, nx, ny, nz,
                                                     nbr_vertex);
      if (node == NULL)
         printf ("NULL\n");
      else
         printf ("%s = (%g, %g, %g)\n", node->getName(), node->getX(),
                                        node->getY(),    node->getZ());
      }
   nbr_vertex ++;
   return  node;
}
// ====================================================== addEdge
Edge* BiCylinder::addEdge (Vertex* v1, Vertex* v2, int cyl, int dir, int nx,
                           int ny, int nz)
{
   int key = getKey  (cyl, dir, nx, ny, nz);
   int nro = map_edge [key];
   if (nro>0)
      {
      if (db)
         {
         Edge* edge = tab_edge [nro];

         printf ("pres_edge [%d,%d, %d,%d,%d] = E%d = ", cyl, dir, nx, ny, nz,
                                                        nbr_edges);
         printf ("%s  ((%s, %s)) = (%s,%s)\n", edge->getName(),
                 edge->getVertex(0)->getName(), edge->getVertex(1)->getName(),
                 v1->getName(), v2->getName());
         if (NOT edge->definedBy (v1,v2))
            printf (" **** Incoherence !!\n");
         }
      return tab_edge [nro];
      }

   if (v1==NULL || v2==NULL)
      return NULL;

   Edge* edge = findEdge (v1, v2);
   if (edge==NULL)
       edge = newEdge (v1, v2);

   map_edge [key] = nbr_edges;
   tab_edge.push_back (edge);

   if (db)
      {
      printf (" tab_edge [%d,%d, %d,%d,%d] = E%d = ", cyl, dir, nx, ny, nz,
                                                     nbr_edges);
      if (edge == NULL)
         printf ("NULL\n");
      else
         printf ("%s = (%s, %s)\n", edge->getName(),
                                    edge->getVertex(0)->getName(),
                                    edge->getVertex(1)->getName());
      }
   nbr_edges ++;
   return edge;
}
// ====================================================== addQuad
Quad* BiCylinder::addQuad (Edge* e1, Edge* e2, Edge* e3, Edge* e4, int cyl,
                           int dir, int nx, int ny, int nz)
{
   int key = getKey (cyl, dir, nx, ny, nz);
   int nro = map_quad [key];
   if (nro>0)
      return tab_quad [nro];

   Quad* quad = newQuad (e1, e2, e3, e4);
   map_quad [key] = nbr_quads;
   tab_quad.push_back (quad);

   if (db)
      {
      printf (" tab_quad [%d,%d, %d,%d,%d] = Q%d = ", cyl, dir, nx, ny, nz,
                                                     nbr_quads);
      if (quad == NULL)
         printf ("NULL\n");
      else
         printf ("%s = (%s, %s, %s, %s)\n",  quad->getName(),
                 quad->getEdge(0)->getName(), quad->getEdge(1)->getName(),
                 quad->getEdge(2)->getName(), quad->getEdge(3)->getName());
      }
   nbr_quads ++;
   return quad;
}
// ====================================================== addHexa
Hexa* BiCylinder::addHexa (Quad* q1, Quad* q2, Quad* q3, Quad* q4, Quad* q5,
                           Quad* q6, int cyl, int nx, int ny, int nz)
{
   int key = getKey (cyl, nx, ny, nz);
   int nro = map_hexa [key];
   if (nro>0)
      {
      printf (" tab_hexa [%d, %d,%d,%d] = H%d est deja la\n ",
                          cyl, nx, ny, nz, nbr_hexas);
      return tab_hexa [nro];
      }

   Hexa* hexa = newHexa (q1, q2, q3, q4, q5, q6);
   map_hexa [key] = nbr_hexas;
   tab_hexa.push_back (hexa);

   if (db)
      {
      printf (" tab_hexa [%d, %d,%d,%d] = H%d = ", cyl, nx, ny, nz, nbr_hexas);
      if (hexa == NULL)
         printf ("NULL\n");
      else
         printf ("%s = (%s, %s, %s, %s, %s, %s)\n",  hexa->getName(),
                 hexa->getQuad(0)->getName(), hexa->getQuad(1)->getName(),
                 hexa->getQuad(2)->getName(), hexa->getQuad(3)->getName(),
                 hexa->getQuad(4)->getName(), hexa->getQuad(5)->getName());
      }
   nbr_hexas ++;
   return   hexa;
}
// ------------------------------------------------------------------------
// ====================================================== findVertex
Vertex* BiCylinder::findVertex (double px, double py, double pz,
                               int nx, int ny, int nz)
{
   for (it_map=map_vertex.begin() ; it_map!=map_vertex.end() ; ++it_map)
       {
       int     nro = it_map->second;
       Vertex* elt = tab_vertex[nro];
       if (elt != NULL && elt->definedBy (px, py, pz))
          {
          int key = getKey (CylBig, nx, ny, nz);
          map_vertex [key] = nro;
          if (db)
             printf ("findVertex [Big,%d,%d,%d] = V%d = '%d' = %s\n",
                       nx, ny, nz, nro, it_map->first,
                       tab_vertex[nro]->getName());
          return elt;
          }
       }
   printf ("**** Recherche du vertex (%g, %g, %g)\n", px, py, pz);
   fatal_error ("HexBiCylinder : Vertex non trouve");
   return NULL;
}
// ====================================================== findEdge
Edge* BiCylinder::findEdge (Vertex* v1, Vertex* v2)
{
   int nbedges = tab_edge.size();
   for (int nro = 0 ; nro<nbedges ; nro++)
       {
       Edge* elt = tab_edge[nro];
       if (elt != NULL && elt->definedBy (v1, v2))
          return elt;
       }
   return NULL;
}
// ====================================================== findEdge
Edge* BiCylinder::findEdge (Vertex* v1, Vertex* v2, int dir, int nx,
                           int ny, int nz)
{
   for (it_map=map_edge.begin() ; it_map!=map_edge.end() ; ++it_map)
       {
       int   nro = it_map->second;
       Edge* elt = tab_edge[nro];
       if (elt != NULL && elt->definedBy (v1, v2))
          {
          int key = getKey (CylBig, dir, nx, ny, nz);
          map_edge [key] = nro;
          if (db)
             printf ("findEdge [%d, %d,%d,%d] = E%d = '%d' = %s\n",
                       dir, nx, ny, nz, nro, it_map->first,
                       elt->getName());
          return elt;
          }
       }
   fatal_error ("HexBiCylinder : Edge non trouve");
   return NULL;
}
// ====================================================== findQuad
Quad* BiCylinder::findQuad (Vertex* v1, Vertex* v2,
                           int dir, int nx, int ny, int nz)
{
   for (it_map=map_quad.begin() ; it_map!=map_quad.end() ; ++it_map)
       {
       int   nro = it_map->second;
       Quad* elt = tab_quad[nro];
       if (elt != NULL && elt->definedBy (v1, v2))
          {
          int key = getKey (CylBig, dir, nx, ny, nz);
          map_quad [key] = nro;
          if (db)
             printf ("findQuad [%d, %d,%d,%d] = E%d = '%d' = %s\n",
                       dir, nx, ny, nz, nro, it_map->first,
                       elt->getName());
          return elt;
          }
       }
   fatal_error ("HexBiCylinder : Quad non trouve");
   return NULL;
}
// ====================================================== findQuad
Quad* BiCylinder::findQuad (Edge* e1, Edge* e2, int dir, int nx, int ny, int nz)
{
   for (it_map=map_quad.begin() ; it_map!=map_quad.end() ; ++it_map)
       {
       int   nro = it_map->second;
       Quad* elt = tab_quad[nro];
       if (elt != NULL && elt->definedBy (e1, e2))
          {
          int key = getKey (CylBig, dir, nx, ny, nz);
          map_quad [key] = nro;
          if (db)
             printf ("findQuad [%d, %d,%d,%d] = E%d = '%d' = %s\n",
                       dir, nx, ny, nz, nro, it_map->first, elt->getName());
          return elt;
          }
       }
   fatal_error ("HexBiCylinder : Quad non trouve");
   return NULL;
}
// ====================================================== findHexa
Hexa* BiCylinder::findHexa (Quad* q1, Quad* q2, int nx, int ny, int nz)
{
   for (it_map=map_hexa.begin() ; it_map!=map_hexa.end() ; ++it_map)
       {
       int   nro = it_map->second;
       Hexa* elt = tab_hexa[nro];
       if (elt != NULL && elt->definedBy (q1, q2))
          {
          int key = getKey (CylBig, nx, ny, nz);
          map_hexa [key] = nro;
          if (db)
             printf ("findHexa [%d,%d,%d] = H%d = '%d'\n",
                       nx, ny, nz, nro, it_map->first);
          return elt;
          }
       }
   fatal_error ("HexBiCylinder : Hexa non trouve");
   return NULL;
}
// ====================================================== crossCylinders
int BiCylinder::crossCylinders (Cylinder* lun, Cylinder* lautre)
{
   if (lun->getRadius() < lautre->getRadius())
      {
      cross_cyl1 = lun;
      cross_cyl2 = lautre;
      }
   else
      {
      cross_cyl1 = lautre;
      cross_cyl2 = lun;
      }

   int ier = cross_cyl2->interCylinder (cross_cyl1, at_left, at_right,
                                        cross_center);
   if (ier != HOK)
      return ier;

   cyl_fill = false;
   cross_rayext  [CylSmall] = cross_cyl1->getRadius ();
   cross_rayext  [CylBig]   = cross_cyl2->getRadius ();
   cross_rayint  [CylSmall] = cross_rayext  [CylSmall] / 2;
   cross_rayint  [CylBig  ] = cross_rayext  [CylBig  ] / 2;
   cross_hauteur [CylSmall] = cross_cyl1->getHeight ();
   cross_hauteur [CylBig]   = cross_cyl2->getHeight ();

   createLittleCyl ();
   createBigCyl    ();
   adjustLittleSlice (1, 1);
   adjustLittleSlice (0, 2);
   adjustLittleSlice (0, 3);
   adjustLittleSlice (1, 4);

   Vector* iprim = new Vector (cross_cyl1->getDirection());
   Vector* kprim = new Vector (cross_cyl2->getDirection());
   Vector* jprim = new Vector (kprim);

   iprim->renormer ();
   kprim->renormer ();
   jprim->vectoriel (kprim, iprim);

   // transfoVertices (cross_center, iprim, jprim, kprim);

   // Real3 snorm, bnorm;
   // iprim->getCoord (snorm);
   // kprim->getCoord (bnorm);

   Real3 bnorm  = {0, 0, 1};
   Real3 snorm  = {1, 0, 0};
   assoCylinders (snorm, bnorm);

/*********************************************
   if (at_left)
      {
      assoIntersection (NxExt, 1, snorm, bnorm);
      if (grid_type == GR_BIPIPE)
         {
         assoIntersection (NxInt, 2, snorm, bnorm);
         }
      }

   if (at_right)
      {
      assoIntersection (NxExt, NbrSlices1-1, snorm, bnorm);
      if (grid_type == GR_BIPIPE)
         {
         assoIntersection (NxInt, NbrSlices1-2, snorm, bnorm);
         }
      }

  ******************************************* */
   assoResiduelle ();
   return HOK;
}
// ====================================================== createLittleCyl
void BiCylinder::createLittleCyl ()
{
   Real3   base;
   Vertex* vbase = cross_cyl1->getBase();

   Real lg   =  cross_hauteur[CylSmall];
   Real h1   =  calc_distance (cross_center, vbase->getPoint (base));
   Real h2   =  cross_rayext[CylBig]*cos45;
   Real h3   = (cross_rayext[CylBig] - cross_rayint[CylBig])*cos45;

   double t_haut [NbrVslices] = { -h1, -h2, -h2+h3, h2-h3, h2, lg-h1 };

   for (int nk=0; nk<NbrVslices ; nk++)
       {
       switch (nk)
          {
          case 0 : case 5 :
              addSlice (CylSmall, 0, nk, t_haut[nk], cross_rayint [CylSmall]);
              addSlice (CylSmall, 1, nk, t_haut[nk], cross_rayext [CylSmall]);
              break;
          case 1 : case 4 :
              addSlice (CylSmall, 1, nk, t_haut[nk], cross_rayext [CylSmall]);
              addSlice (CylSmall, 2, nk, t_haut[nk], cross_rayext [CylBig]);
              break;
          case 2 : case 3 :
              addSlice (CylSmall, 0, nk, t_haut[nk], cross_rayint [CylSmall]);
              addSlice (CylSmall, 1, nk, t_haut[nk], cross_rayint [CylBig]);
              break;
          }
       }
                    //    ka kb kc kd
   fillSlice (CylSmall, 0,0, 0,2, 1,1, 1,0);
   fillSlice (CylSmall, 0,2, 1,2, 2,1, 1,1);  // OK
   // fillSlice (CylSmall, 1,1, 0,2, 1,2, 2,1);     // Test
   fillSlice (CylSmall, 1,2, 1,3, 2,4, 2,1, true);
   fillSlice (CylSmall, 1,3, 0,3, 1,4, 2,4);
   fillSlice (CylSmall, 0,3, 0,5, 1,5, 1,4);
   // fillSmallCyl ();
}
// ====================================================== createBigCyl
void BiCylinder::createBigCyl ()
{
   const int cyl = CylBig;
   Real3   base;
   Vertex* vbase = cross_cyl2->getBase();
   Real lg   = cross_hauteur[cyl];
   Real rext = cross_rayext [cyl];
   Real rint = cross_rayint [cyl];
   Real h1   = calc_distance (cross_center, vbase->getPoint (base));
   Real h2   = rext * cos45;
   Real h3   = lg - h1;
   Real dh   = (rext - rint)*cos45;

   addSlice (CylBig, 0, 0, -h1,    rint);
   addSlice (CylBig, 1, 0, -h1,    rext);
   addSlice (CylBig, 0, 1, -h2+dh, rint, true);
   addSlice (CylBig, 1, 1, -h2,    rext, true);
   addSlice (CylBig, 0, 2,  h2-dh, rint, true);
   addSlice (CylBig, 1, 2,  h2,    rext, true);
   addSlice (CylBig, 0, 3,  h3,    rint);
   addSlice (CylBig, 1, 3,  h3,    rext);

                  //   A    B    C    D
   fillSlice (CylBig, 0,0, 0,1, 1,1, 1,0);
   fillSlice (CylBig, 0,2, 0,3, 1,3, 1,2);
}
// ====================================================== adjustLittleSlice
void BiCylinder::adjustLittleSlice (int ni, int nk)
{
   Vertex* node = getVertexIJK (CylSmall, ni, 0, nk);
   if (node==NULL)
      return;

   double grayon2 = cross_rayext[CylBig] * cross_rayext[CylBig];
   double prayon  = cross_rayext[CylSmall];
   if (ni==0)
      {
      grayon2 = cross_rayint[CylBig] * cross_rayint[CylBig];
      prayon  = cross_rayint[CylSmall];
      }

   for (int nj=0;  nj<NbrCotes ; nj++)
       {
       node  = getVertexIJK (CylSmall, ni, nj, nk);
       double angle = getAngle (nj);
       double py = prayon * cos (angle);
       double pz = prayon * sin (angle);
       double px = sqrt (grayon2-py*py);

       double qx = node->getX();
       if (qx>=0) node->setX ( px);
       else       node->setX (-px);
       node->setY (py);
       node->setZ (pz);
       }
}
// ====================================================== addSlice
void BiCylinder::addSlice (int cyl, int ni, int nk, double hauteur,
                           double rayon, bool find)
{
   for (int nj=0 ; nj<NbrCotes ; nj++)
       {
       double theta = getAngle (nj);
       double px = rayon*cos(theta);
       double py = rayon*sin(theta);
                  // Find = forcement le gros cylindre
       if (find)
          findVertex (px, py, hauteur, ni, nj, nk);
       else if (cyl==CylBig)
          addVertex  (px, py, hauteur, CylBig, ni, nj, nk);
       else    // CylSmall
          addVertex (hauteur, px, py, CylSmall, ni, nj, nk);
       }
}
// ====================================================== fillSlice
/* *****************************************************************
       H=bed  +----bd-----+ bdf=G
             /|          /|
           be |   B    bf |
           /  |        /  |
    E=bce +----bc-----+...|...bcf=F
          |  de     D |   df
          | E |       | F |             J
         ce   | C     cf  |             ^
  D=ade...|...+----ad-|---+ adf=C       |   I
          |  /        |  /              |  /
          | ae    A   | af              | /
          |/          |/                |/
    A=ace +----ac-----+ acf=B           +----->  K

   ***************************************************************** */
void BiCylinder::fillSlice (int cyl, int nia, int nka, int nib, int nkb,
                                     int nic, int nkc, int nid, int nkd,
                                     bool meddle)
{
   for (int nj0=0; nj0<NbrCotes ; nj0++)
       {
       if (meddle) nj0++;
       int nj1 = nj0+1;
       if (nj1>=NbrCotes) nj1=0;
       Vertex* vace = getVertexIJK (cyl, nia, nj0, nka);
       Vertex* vacf = getVertexIJK (cyl, nib, nj0, nkb);
       Vertex* vadf = getVertexIJK (cyl, nic, nj0, nkc);
       Vertex* vade = getVertexIJK (cyl, nid, nj0, nkd);

       Vertex* vbce = getVertexIJK (cyl, nia, nj1, nka);
       Vertex* vbcf = getVertexIJK (cyl, nib, nj1, nkb);
       Vertex* vbdf = getVertexIJK (cyl, nic, nj1, nkc);
       Vertex* vbde = getVertexIJK (cyl, nid, nj1, nkd);

/* *******************
       PutName (vace);
       PutName (vacf);
       PutName (vadf);
       PutName (vade);
       PutName (vbce);
       PutName (vbcf);
       PutName (vbdf);
       PutName (vbde);
   ******************* */

       Edge* eac = addEdge (vace, vacf, cyl, dir_z, nia, nj0, nka);
       Edge* ead = addEdge (vade, vadf, cyl, dir_z, nid, nj0, nkd);
       Edge* eae = addEdge (vace, vade, cyl, dir_x, nia, nj0, nka);
       Edge* eaf = addEdge (vacf, vadf, cyl, dir_x, nib, nj0, nkb);

       Edge* ebc = addEdge (vbce, vbcf, cyl, dir_z, nia, nj1, nka);
       Edge* ebd = addEdge (vbde, vbdf, cyl, dir_z, nid, nj1, nkd);
       Edge* ebe = addEdge (vbce, vbde, cyl, dir_x, nia, nj1, nka);
       Edge* ebf = addEdge (vbcf, vbdf, cyl, dir_x, nib, nj1, nkb);

       Edge* ece = addEdge (vace, vbce, cyl, dir_y, nia, nj0, nka);
       Edge* ecf = addEdge (vacf, vbcf, cyl, dir_y, nib, nj0, nkb);
       Edge* edf = addEdge (vadf, vbdf, cyl, dir_y, nic, nj0, nkc);
       Edge* ede = addEdge (vade, vbde, cyl, dir_y, nid, nj0, nkd);

       Quad* qa  = addQuad (eac, eaf, ead, eae, cyl, dir_y, nia , nj0, nka);
       Quad* qb  = addQuad (ebc, ebf, ebd, ebe, cyl, dir_y, nia , nj1, nka);
       Quad* qc  = addQuad (eac, ecf, ebc, ece, cyl, dir_x, nia , nj0, nka);
       Quad* qd  = addQuad (ead, edf, ebd, ede, cyl, dir_x, nid , nj0, nkd);
       Quad* qe  = addQuad (eae, ede, ebe, ece, cyl, dir_z, nia , nj0, nka);
       Quad* qf  = addQuad (eaf, edf, ebf, ecf, cyl, dir_z, nib , nj0, nkb);

       addHexa (qa, qb, qc, qd, qe, qf, cyl, nia, nj0, nka);
       }
}
// ====================================================== assoCylinders
void BiCylinder::assoCylinders (double* snormal, double* gnormal)
{
   assoSlice (CylSmall, 0, 0, snormal);
   assoSlice (CylSmall, 1, 0, snormal);
   assoSlice (CylSmall, 0, 5, snormal);
   assoSlice (CylSmall, 1, 5, snormal);

   for (int nk=0 ; nk<4 ; nk++)
       for (int ni=0 ; ni<2 ; ni++)
            assoSlice (CylBig, ni, nk, gnormal);

   assoIntersection (1, 1, snormal, gnormal);
   assoIntersection (0, 2, snormal, gnormal);
   assoIntersection (0, 3, snormal, gnormal);
   assoIntersection (1, 4, snormal, gnormal);

}
// ====================================================== assoSlice
void BiCylinder::assoSlice (int cyl, int nx, int nzs, double* normal)
{
   Real3  center, pnt1, pnt2, vbase;
   string brep;
   int ny0 = 0;
   int nyd = NbrCotes/2;

   Vertex* v0 = getVertexIJK (cyl, nx, ny0 , nzs);
   Vertex* vd = getVertexIJK (cyl, nx, nyd , nzs);

   if (vd==NULL || v0==NULL)
      return;

   v0->getPoint (pnt1);
   vd->getPoint (pnt2);

   double rayon = 0;
   for (int nro=0 ; nro<DIM3 ; nro++)
       {
       center[nro] = (pnt1[nro] + pnt2[nro])/2;
       vbase [nro] = (pnt1[nro] - center[nro]);
       rayon += vbase [nro]*vbase[nro];
       }
   rayon = sqrt (rayon);

   PutCoord (pnt1);
   PutCoord (pnt2);
   PutCoord (vbase);
   PutCoord (center);
   PutData  (rayon);

   geom_create_circle (center, rayon, normal, vbase, brep);
   geom_define_line   (brep);   // pour geom_asso_point

   for (int ny=0 ; ny<NbrCotes ; ny++)
       {
       assoArc (cyl, nx, ny, nzs, brep, rayon);
       }
}
// ===================================================== assoArc
void BiCylinder::assoArc (int cyl, int nx, int ny, int nz, string& brep,
                             double rayon)
{
    static const double Alpha = 1.0 / NbrCotes;
    static const double Theta = 2*M_PI/ NbrCotes;
    int ny1 = (ny+1) MODULO NbrCotes;

    Edge*   edge  = getEdgeJ  (cyl, nx, ny, nz);
    Vertex* node0 = getVertexIJK (cyl, nx, ny, nz);
    Vertex* node1 = getVertexIJK (cyl, nx, ny1, nz);

    if (db)
       printf ("AssoArc : Edge = %s  = (%s,%s) -> (%s,%s)\n", edge->getName(),
               edge->getVertex(0)->getName(), edge->getVertex(1)->getName(),
               node0->getName(), node1->getName());

    // Shape*  shape = new Shape (brep);

    // shape->setBounds (ny*Alpha, (ny+1)*Alpha);
    // edge ->addAssociation (shape);

    geom_asso_point ( ny   *Theta*rayon, node0);
    geom_asso_point ((ny+1)*Theta*rayon, node1);
}
// ===================================================== assoIntersection
int BiCylinder::assoIntersection (int nxs, int nzs, double* snorm,
                                                    double* bnorm)
{
   Real3  X_center = {0, 0, 0};  // provisoire, a la place de cross_center
   Real3  pse, psw, sorig, sbase;
   Real3  pbe, pbw, borig, bbase;
   string brep;
   int ny0 = 0;
   int nyd = NbrCotes/2;
   int MiddleSlice1 = 3;

   int nz = nzs < MiddleSlice1 ? 0 : 5;

   getVertexIJK (CylSmall, nxs, ny0 , nz)->getPoint (pse);
   getVertexIJK (CylSmall, nxs, nyd , nz)->getPoint (psw);
   getVertexIJK (CylBig,   nxs, ny0 , 0) ->getPoint (pbe);
   getVertexIJK (CylBig,   nxs, nyd , 0) ->getPoint (pbw);

   double srayon = calc_distance (pse, psw)/2;
   double brayon = calc_distance (pbe, pbw)/2;

   calc_milieu  (psw, pse, sorig);
   calc_milieu  (pbw, pbe, borig);
   calc_vecteur (psw, pse, sbase);
   calc_vecteur (pbw, pbe, bbase);

   double shaut = calc_distance (X_center, sorig);
   double bhaut = calc_distance (X_center, borig)*2;
   double* orig = nzs < MiddleSlice1 ? sorig : X_center; // Pb orientation

   if (db)
      {
      PutCoord  (borig);
      PutCoord  (sorig);
      PutCoord  (orig);
      PutData (nz);
      PutData  (srayon);
      PutData  (brayon);
      }

   int ier = geom_create_cylcyl (borig, bnorm, bbase, brayon, bhaut,
                                  orig, snorm, sbase, srayon, shaut);
   if (ier != HOK)
      return ier;

   for (int ny=0 ; ny<NbrCotes ; ny++)
       {
       Vertex* node = getVertexIJK (CylSmall, nxs, ny, nzs);
       if (node!=NULL)
           node->clearAssociation ();
       }

   for (int ny=0 ; ny<NbrCotes ; ny++)
       {
       Edge* edge = getEdgeJ (CylSmall, nxs, ny, nzs);
       geom_asso_cylcyl (edge);
       }

   return HOK;
}
// ======================================================== test_bicylinder
BiCylinder* test_bicylinder (Document* docu, int option)
{
   Vertex* ori1 = docu->addVertex ( 0,0,0);
   Vertex* ori2 = docu->addVertex (-5,0,5);
   Vector* vz   = docu->addVector ( 0,0,1);
   Vector* vx   = docu->addVector ( 1,0,0);

   double r1 = 2*sqrt (2.0);
   double r2 = 3*sqrt (2.0);
   double l2 = 10;
   double l1 = 10;

   Cylinder* cyl1 = docu->addCylinder (ori1, vz, r1, l1);
   Cylinder* cyl2 = docu->addCylinder (ori2, vx, r2, l2);

   BiCylinder* grid = new BiCylinder (docu);
   grid->crossCylinders (cyl1, cyl2);
   return grid;
}
END_NAMESPACE_HEXA
