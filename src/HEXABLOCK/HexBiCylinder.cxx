
// C++ : Gestion des cylindres croises

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

#include "HexBiCylinder.hxx"

#include "HexDocument.hxx"
#include "HexVector.hxx"
#include "HexVertex.hxx"
#include "HexHexa.hxx"
#include "HexEdge.hxx"

#include "HexGlobale.hxx"
#include "HexNewShape.hxx"
#include "HexBiCylinderShape.hxx"

///  static const double UnSur2pi = DEMI/M_PI;

BEGIN_NAMESPACE_HEXA

static bool db = on_debug ();
static const double cos45 = cos (M_PI/4);
#define NameOf(x) ((x)?x->getName():"Null")

// ====================================================== Constructeur
BiCylinder::BiCylinder (Document* doc)
          : Elements (doc)
{
   cyl_fill     = false;
   grid_type    = GR_BIPIPE;
   at_right  = at_left = true;

   tab_hexa.push_back   (NULL);
   tab_quad.push_back   (NULL);
   tab_edge.push_back   (NULL);
   tab_vertex.push_back (NULL);
   nbr_vertex = nbr_edges = nbr_quads = nbr_hexas = 1;

   grid_geom = NULL;
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
         printf ("%s = (%g, %g, %g)\n", NameOf (tab_vertex[nro]),
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
         printf ("%s = (%g, %g, %g)\n", NameOf (node), node->getX(),
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
         printf ("%s  ((%s, %s)) = (%s,%s)\n", NameOf(edge),
                 NameOf(edge->getVertex(0)), NameOf(edge->getVertex(1)),
                 NameOf(v1), NameOf(v2));
         if (edge->definedBy (v1,v2))
            return tab_edge [nro];
         else
            printf (" **** Incoherence !!\n");
         }
      }

   if (v1==NULL || v2==NULL)
      return NULL;

   Edge* edge = findEdge (v1, v2);
   if (edge!=NULL)
       return edge;

   edge = newEdge (v1, v2);
   map_edge [key] = nbr_edges;
   tab_edge.push_back (edge);
   nbr_edges ++;

   if (db)
      {
      printf (" tab_edge [%d,%d, %d,%d,%d] = E%d = ", cyl, dir, nx, ny, nz,
                                                      nbr_edges-1);
      if (edge == NULL)
         printf ("NULL\n");
      else
         printf ("%s = (%s, %s)\n", NameOf(edge),
                                    NameOf(edge->getVertex(0)),
                                    NameOf(edge->getVertex(1)));
      }
   return edge;
}
// ====================================================== addQuad
Quad* BiCylinder::addQuad (Edge* e1, Edge* e2, Edge* e3, Edge* e4, int cyl,
                           int dir, int nx, int ny, int nz)
{
   int key = getKey (cyl, dir, nx, ny, nz);
   int nro = map_quad [key];
   if (nro>0)
      {
      if (db)
         {
         Quad* quad = tab_quad [nro];

         printf ("pres_quad [%d,%d, %d,%d,%d] = Q%d = ", cyl, dir, nx, ny, nz,
                                                        nbr_quads);
         printf ("%s  (%s, %s, %s, %s)\n", NameOf(quad),
                 NameOf(e1), NameOf(e2), NameOf(e3), NameOf(e4));
         if (quad->definedBy (e1,e3))
            return tab_quad [nro];
         else
            {
            printf (" **** Incoherence !!\n");
            printf ("%s =  (%s, %s, %s, %s)\n", NameOf(quad),
                 NameOf(quad->getEdge(0)), NameOf(quad->getEdge(1)),
                 NameOf(quad->getEdge(1)), NameOf(quad->getEdge(2)));
            printf ("%s =  (%s, %s, %s, %s)\n", NameOf(quad),
                 NameOf(quad->getVertex(0)), NameOf(quad->getVertex(1)),
                 NameOf(quad->getVertex(1)), NameOf(quad->getVertex(2)));
            }
         }
      }

   Quad* quad = Elements::findQuad (e1, e2);
   if (quad!=NULL) 
      return quad;

   quad = newQuad (e1, e2, e3, e4);
   map_quad [key] = nbr_quads;
   tab_quad.push_back (quad);
   nbr_quads ++;

   if (db)
      {
      printf (" tab_quad [%d,%d, %d,%d,%d] = Q%d = ", cyl, dir, nx, ny, nz,
                                                      nbr_quads-1);
      if (quad == NULL)
         printf ("NULL\n");
      else
         printf ("%s = (%s, %s, %s, %s)\n",  NameOf(quad),
                 NameOf(quad->getEdge(0)), NameOf(quad->getEdge(1)),
                 NameOf(quad->getEdge(2)), NameOf(quad->getEdge(3)));
      }
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
      if (tab_hexa [nro]->definedBy (q1,q2))
         {
         printf (" tab_hexa [%d, %d,%d,%d] = H%d est deja la\n ",
                          cyl, nx, ny, nz, nbr_hexas);
         return tab_hexa [nro];
         }
      printf (" Incoherence : tab_hexa [%d, %d,%d,%d] = H%d = %s\n ",
                    cyl, nx, ny, nz, nbr_hexas, tab_hexa [nro]->getName ());
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
         printf ("%s = (%s, %s, %s, %s, %s, %s)\n",  NameOf(hexa),
                 NameOf(hexa->getQuad(0)), NameOf(hexa->getQuad(1)),
                 NameOf(hexa->getQuad(2)), NameOf(hexa->getQuad(3)),
                 NameOf(hexa->getQuad(4)), NameOf(hexa->getQuad(5)));
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
                       NameOf(tab_vertex[nro]));
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
                       NameOf(elt));
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
                       NameOf(elt));
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
                       dir, nx, ny, nz, nro, it_map->first, NameOf(elt));
          return elt;
          }
       }
   fatal_error ("HexBiCylinder : Quad non trouve");
   return NULL;
}
/*******************************************
// ====================================================== findQuad
Quad* BiCylinder::findQuad (Edge* e1, Edge* e2)
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
                       dir, nx, ny, nz, nro, it_map->first, NameOf(elt));
          return elt;
          }
       }
   fatal_error ("HexBiCylinder : Quad non trouve");
   return NULL;
}
 ********************************************/
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
// ====================================================== createLittlePipe
void BiCylinder::createLittlePipe ()
{
   Real lg  =  cross_hauteur[CylSmall];
   Real h1  =  calc_distance (cross_center, cross_orismall);
   Real h2  =  cross_rayext[CylBig]*cos45;
   Real h3  = (cross_rayext[CylBig] - cross_rayint[CylBig])*cos45;
   Real h5  =  lg-h1;

// double t_haut [NbrVslices] = { -h1, -h2, -h2+h3, h2-h3, h2, lg-h1 };

   if (at_left) 
      {
      addSlice (CylSmall, 0, 0, -h1, cross_rayint [CylSmall]);
      addSlice (CylSmall, 1, 0, -h1, cross_rayext [CylSmall]);
      addSlice (CylSmall, 1, 1, -h2, cross_rayext [CylSmall]);
      addSlice (CylSmall, 0, 2, h3-h2, cross_rayint [CylSmall]);
      }
   else if (cyl_fill) 
      {
      addSlice (CylSmall, 0, 1, -h2, cross_rayint [CylSmall]);

      addSlice (CylSmall, 1, 1, -h2, cross_rayext [CylSmall]);
      addSlice (CylSmall, 0, 2, h3-h2, cross_rayint [CylSmall]);
      }


   addSlice (CylSmall, 2, 1, -h2, cross_rayext [CylBig]);
   addSlice (CylSmall, 1, 2, h3-h2, cross_rayint [CylBig]);
   addSlice (CylSmall, 1, 3, h2-h3, cross_rayint [CylBig]);
   addSlice (CylSmall, 2, 4,  h2, cross_rayext [CylBig]);

   if (at_right) 
      {
      addSlice (CylSmall, 0, 3, h2-h3, cross_rayint [CylSmall]);
      addSlice (CylSmall, 1, 4,  h2, cross_rayext [CylSmall]);
      addSlice (CylSmall, 0, 5,  h5, cross_rayint [CylSmall]);
      addSlice (CylSmall, 1, 5,  h5, cross_rayext [CylSmall]);
      }
   else if (cyl_fill) 
      {
      addSlice (CylSmall, 0, 3, h2-h3, cross_rayint [CylSmall]);
      addSlice (CylSmall, 1, 4,  h2, cross_rayext [CylSmall]);

      addSlice (CylSmall, 0, 4,  h2, cross_rayint [CylSmall]);
      }
                 //--------------------------------- Remplissage
                    //    ka kb kc kd
   if (at_left)
      {
      fillSlice (CylSmall, 0,0, 0,2, 1,1, 1,0);
      fillSlice (CylSmall, 0,2, 1,2, 2,1, 1,1); 
      if (cyl_fill)
         {
         addCube (CylSmall, 0,0,2);
         }
      }
    else if (cyl_fill)
      {
      fillSlice (CylSmall, 0,1, 0,2, 1,2, 2,1);
      addCube (CylSmall, 1,0);
      }
    else 
      addCube ( CylSmall, 1, 2);

   fillSlice     (CylSmall, 1,2, 1,3, 2,4, 2,1, true);
   if (cyl_fill)
      {
      addCube   (CylSmall, 2,0, 3);
      fillSlice (CylSmall, 0,2, 0,3, 1,3, 1,2);
      }

   if (at_right)
      {
      fillSlice (CylSmall, 1,3, 0,3, 1,4, 2,4);
      fillSlice (CylSmall, 0,3, 0,5, 1,5, 1,4);
      if (cyl_fill)
          addCube (CylSmall, 3, 0, 5);
      }
    else if (cyl_fill)
      {
      fillSlice (CylSmall, 0,3, 0,4, 2,4, 1,3);
      addCube   (CylSmall, 3, 0);
      }
    else
      addCube ( CylSmall, 3, 1);
}
// ====================================================== createBigPipe
void BiCylinder::createBigPipe ()
{
   const int cyl = CylBig;
   Real lg   = cross_hauteur[cyl];
   Real rext = cross_rayext [cyl];
   Real rint = cross_rayint [cyl];

   Real h1   = calc_distance (cross_center, cross_oribig);
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
   if (cyl_fill) 
      addCube (CylBig, 0);
   fillSlice (CylBig, 0,0, 0,1, 1,1, 1,0);
   if (cyl_fill) 
      addCube (CylBig, 2);
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
      if (NOT cyl_fill)
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
// ====================================================== addCarre
void BiCylinder::addCarre (int cyl, int nk, double hauteur, double rayon,
                           bool find)
{
   for (int nj=0 ; nj<NbrCotes ; nj++)
       {
       double theta = getAngle (nj);
       double px = rayon*cos(theta);
       double py = rayon*sin(theta);
       if (find)
          findVertex (px, py, hauteur, 0, nj, nk);
       else if (cyl==CylBig)
          addVertex  (px, py, hauteur, CylBig, 0, nj, nk);
       else    // CylSmall
          addVertex (hauteur, px, py, cyl, 0, nj, nk);
       }
}
// ====================================================== adjustLittleSquare
void BiCylinder::adjustLittleSquare (int nk)
{
   double grayon2 = cross_rayext[CylBig] * cross_rayext[CylBig];

   for (int nj=0 ; nj<NbrCotes ; nj++)
       {
       Vertex* node = getVertexIJK (CylSmall, 0, nj, nk);
       double  py   = node->getY();
       double  px   = sqrt (grayon2 - py*py);

       if (node->getX() > 0) node->setX ( px); 
       else                  node->setX (-px); 
       }
}
// ====================================================== addCube
void BiCylinder::addCube (int cyl, int nk0, int nvi0, int k1)
{
/* *****************************************************************
       H=bed  +----bd-----+ bdf=G......0
             /|          /|
           be |   B    bf |
           /  |        /  |
    E=bce +----bc-----+...|...bcf=F
          |  de     D |   df
          | E |       | F |             J
         ce   | C     cf  |             ^
  D=ade...|...+----ad-|---+ adf=C.....3 |   I
          |  /        |  /              |  /
          | ae    A   | af              | /
          |/          |/                |/
    A=ace +----ac-----+ acf=B.....2     +----->  K

   *****************************************************************  */
   enum { nj0, nj1, nj2, nj3, ni0=0 };
   const int nk1  = k1>0 ? k1 : nk0+1;
   const int nvi1 = nvi0==1  ? 2 
                  : nvi0==2  ? 1 : 0;

                           // La face F
   Vertex* vacf = getVertexIJK (cyl, nvi1, nj2, nk1);
   Vertex* vadf = getVertexIJK (cyl, nvi1, nj3, nk1);
   Vertex* vbdf = getVertexIJK (cyl, nvi1, nj0, nk1);
   Vertex* vbcf = getVertexIJK (cyl, nvi1, nj1, nk1);

   Edge* eaf = addEdge (vacf, vadf, cyl, dir_y, ni0, nj2, nk1);
   Edge* edf = addEdge (vadf, vbdf, cyl, dir_y, ni0, nj3, nk1);
   Edge* ebf = addEdge (vbcf, vbdf, cyl, dir_y, ni0, nj0, nk1);
   Edge* ecf = addEdge (vacf, vbcf, cyl, dir_y, ni0, nj1, nk1);

   Quad* qf  = addQuad (eaf, edf, ebf, ecf, cyl, dir_z, ni0 , nj0, nk1);

   Vertex* vace = getVertexIJK (cyl, nvi0, nj2, nk0);
   Vertex* vade = getVertexIJK (cyl, nvi0, nj3, nk0);
   Vertex* vbde = getVertexIJK (cyl, nvi0, nj0, nk0);
   Vertex* vbce = getVertexIJK (cyl, nvi0, nj1, nk0);

   Edge* eac = addEdge (vace, vacf, cyl, dir_z, ni0, nj2, nk0);
   Edge* ead = addEdge (vade, vadf, cyl, dir_z, ni0, nj3, nk0);
   Edge* ebd = addEdge (vbde, vbdf, cyl, dir_z, ni0, nj0, nk0);
   Edge* ebc = addEdge (vbce, vbcf, cyl, dir_z, ni0, nj1, nk0);

   Edge* eae = addEdge (vace, vade, cyl, dir_y, ni0, nj2, nk0);
   Edge* ede = addEdge (vade, vbde, cyl, dir_y, ni0, nj3, nk0);
   Edge* ebe = addEdge (vbce, vbde, cyl, dir_y, ni0, nj0, nk0);
   Edge* ece = addEdge (vace, vbce, cyl, dir_y, ni0, nj1, nk0);

// Quad* qe  = addQuad (eae, ede, ebe, ece, cyl, dir_z, ni0 , nj0, nk0);
   Quad* qe  = addQuad (eae, ece, ebe, ede, cyl, dir_z, ni0 , nj0, nk0);

// Quad* qa  = addQuad (eac, eaf, ead, eae, cyl, dir_y, ni0 , nj2, nk0);
   Quad* qa  = addQuad (eac, eae, ead, eaf, cyl, dir_y, ni0 , nj2, nk0);
// Quad* qd  = addQuad (ead, edf, ebd, ede, cyl, dir_x, ni0 , nj3, nk0);
   Quad* qd  = addQuad (ead, ede, ebd, edf, cyl, dir_x, ni0 , nj3, nk0);
   Quad* qb  = addQuad (ebc, ebf, ebd, ebe, cyl, dir_y, ni0 , nj0, nk0);
   Quad* qc  = addQuad (eac, ecf, ebc, ece, cyl, dir_x, ni0 , nj1, nk0);

   addHexa (qa, qb, qc, qd, qe, qf, cyl, ni0, nj0, nk0);
}
// ====================================================== fillSlice
/* *****************************************************************
       H=bde  +----bd-----+ bdf=G
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

       Edge* eac = addEdge (vace, vacf, cyl, dir_z, nia, nj0, nka);
       Edge* ead = addEdge (vadf, vade, cyl, dir_z, nid, nj0, nkd);
       Edge* eae = addEdge (vade, vace, cyl, dir_x, nia, nj0, nka);
       Edge* eaf = addEdge (vacf, vadf, cyl, dir_x, nib, nj0, nkb);

       Edge* ebc = addEdge (vbce, vbcf, cyl, dir_z, nia, nj1, nka);
       Edge* ebd = addEdge (vbdf, vbde, cyl, dir_z, nid, nj1, nkd);
       Edge* ebe = addEdge (vbde, vbce, cyl, dir_x, nia, nj1, nka);
       Edge* ebf = addEdge (vbcf, vbdf, cyl, dir_x, nib, nj1, nkb);

       Edge* ece = addEdge (vace, vbce, cyl, dir_y, nia, nj0, nka);
       Edge* ecf = addEdge (vacf, vbcf, cyl, dir_y, nib, nj0, nkb);
       Edge* edf = addEdge (vadf, vbdf, cyl, dir_y, nic, nj0, nkc);
       Edge* ede = addEdge (vade, vbde, cyl, dir_y, nid, nj0, nkd);

  //   Quad* qa  = addQuad (eac, eaf, ead, eae, cyl, dir_y, nia+1, nj0, nka);
       Quad* qa  = addQuad (eac, eae, ead, eaf, cyl, dir_y, nia+1, nj0, nka);
       Quad* qb  = addQuad (ebc, ebf, ebd, ebe, cyl, dir_y, nia+1, nj1, nka);

       Quad* qc  = addQuad (eac, ecf, ebc, ece, cyl, dir_x, nia+1, nj0, nka);
 //    Quad* qd  = addQuad (ead, edf, ebd, ede, cyl, dir_x, nid+1, nj0, nkd);
       Quad* qd  = addQuad (ead, ede, ebd, edf, cyl, dir_x, nid+1, nj0, nkd);

  //   Quad* qe  = addQuad (eae, ede, ebe, ece, cyl, dir_z, nia+1, nj0, nka);
       Quad* qe  = addQuad (eae, ece, ebe, ede, cyl, dir_z, nia+1, nj0, nka);
       Quad* qf  = addQuad (eaf, edf, ebf, ecf, cyl, dir_z, nib+1, nj0, nkb);

       addHexa (qa, qb, qc, qd, qe, qf, cyl, nia+1, nj0, nka);
       }
}
// ====================================================== assoCylinders
void BiCylinder::assoCylinders ()
{
   char     name [12];
   sprintf (name, "grid_%02d", el_id);
   grid_geom = el_root->addShape (name, SH_INTER);
   grid_geom -> openShape();

   int s_kmax  = 5;
   int imin    = cyl_fill ? 1 : 0;
   int g_ksize = 4;

   for (int ni=imin ; ni<2 ; ni++)
       {
       assoSlice (CylSmall, ni, 0, cross_dirsmall);
       assoSlice (CylSmall, ni, s_kmax, cross_dirsmall);

       for (int nk=0 ; nk<g_ksize ; nk++)
            assoSlice (CylBig, ni, nk, cross_dirbig);
      }


   if (at_left)
      {
      assoIntersection (1, 1);
      if (NOT cyl_fill)
          assoIntersection (0, 2);
      }

   if (at_right)
      {
      assoIntersection (1, 4);
      if (NOT cyl_fill)
          assoIntersection (0, 3);
      }

   grid_geom -> closeShape();
}
// ====================================================== assoSlice
void BiCylinder::assoSlice (int cyl, int nx, int nzs, double* normal)
{
   Real3  center, pnt0, pnt1, pnt2, vbase;
   int ny0 = 0;
   int ny1 = 1;
   int ny2 = NbrCotes/2;

   Vertex* v0 = getVertexIJK (cyl, nx, ny0 , nzs);
   Vertex* v1 = getVertexIJK (cyl, nx, ny1 , nzs);
   Vertex* v2 = getVertexIJK (cyl, nx, ny2 , nzs);

   if (v0==NULL || v1==NULL || v2==NULL)
      return;

   v0->getPoint (pnt0);
   v1->getPoint (pnt1);
   v2->getPoint (pnt2);

   double rayon = 0;
   for (int nro=0 ; nro<DIM3 ; nro++)
       {
       center[nro] = (pnt0[nro] + pnt2[nro])/2;
       vbase [nro] = (pnt0[nro] - pnt1[nro]);
       rayon += carre (center [nro] - pnt0 [nro]);
       }

   int subid = grid_geom->addCircle (center, sqrt(rayon), normal, vbase);
   for (int ny=0 ; ny<NbrCotes ; ny++)
       {
       int ny1 = (ny+1) MODULO NbrCotes;
       Vertex* va = getVertexIJK (cyl, nx, ny , nzs);
       Vertex* vb = getVertexIJK (cyl, nx, ny1, nzs);

       assoArc (cyl, ny, va, vb, subid);
       }
}
// ===================================================== assoArc
void BiCylinder::assoArc (int cyl, int ny, Vertex* v1, Vertex* v2, int subid)
{
    const double Decal = 1.0 / NbrCotes;
    const double Start = Decal / 2;

    int    ny2    = ny+1;
    double posit  = Start + ny*Decal;
    Edge*  edge   = findEdge (v1, v2);
    if (edge==NULL)
       return;
                                        // Vertex
    grid_geom->addAssociation (v1, subid, posit);

    if (ny2 < NbrCotes)
       {
       grid_geom->addAssociation (edge, subid, posit, posit+Decal);
       }
    else
       {
       grid_geom->addAssociation (edge, subid, posit, 1.0);
       grid_geom->addAssociation (edge, subid, 0,   Start);
       }
}
// ===================================================== assoArc
void BiCylinder::assoArc (int cyl, int nx, int ny, int nz, int subid)
{
    const double Decal = 1.0 / NbrCotes;
    const double Start = Decal / 2;

    int    ny2    = ny+1;
    double posit  = Start + ny*Decal;
    Edge*  edge   = getEdgeJ (cyl, nx, ny, nz);
    if (edge==NULL)
       return;

                                        // Vertex
    Vertex* node = getVertexIJK (cyl, nx, ny,  nz);
    grid_geom->addAssociation (node, subid, posit);

    if (ny2 < NbrCotes)
       {
       grid_geom->addAssociation (edge, subid, posit, posit+Decal);
       // node = getVertexIJK (cyl, nx, ny2, nz);
       // grid_geom->addAssociation (node, subid, angle2*UnSur2pi);
       }
    else
       {
       grid_geom->addAssociation (edge, subid, posit, 1.0);
       grid_geom->addAssociation (edge, subid, 0,   Start);
       }
}
// ===================================================== assoIntersection
int BiCylinder::assoIntersection (int nxs, int nzs)
{
   int ier = HOK;
   Real3  pse, psw, sorig, sbase;
   Real3  pbe, pbw, borig, bbase;
   string brep;
   int ny0 = 0;
   int nyd = NbrCotes/2;
   int MiddleSlice1 = 3;

   int nz = nzs < MiddleSlice1 ? 0 : 5;

   Vertex* vse = getVertexIJK (CylSmall, nxs, ny0 , nz);
   Vertex* vsw = getVertexIJK (CylSmall, nxs, nyd , nz);
   Vertex* vbe = getVertexIJK (CylBig,   nxs, ny0 , 0);
   Vertex* vbw = getVertexIJK (CylBig,   nxs, nyd , 0);

   if (vse==NULL || vsw==NULL || vbe==NULL || vbw==NULL)
      return HERR;

   vse->getPoint (pse);
   vsw->getPoint (psw);
   vbe->getPoint (pbe);
   vbw->getPoint (pbw);

   double srayon = calc_distance (pse, psw)/2;
   double brayon = calc_distance (pbe, pbw)/2;

   calc_milieu  (psw, pse, sorig);
   calc_milieu  (pbw, pbe, borig);
   calc_vecteur (psw, pse, sbase);
   calc_vecteur (pbw, pbe, bbase);

   double shaut = calc_distance (cross_center, sorig);
   double bhaut = calc_distance (cross_center, borig)*2;
   double* orig = nzs < MiddleSlice1 ? sorig : cross_center; // Pb orientation

   BiCylinderShape bicyl_shape (el_root);
   ier = bicyl_shape.defineCyls (borig, cross_dirbig,   bbase, brayon, bhaut,
                                  orig, cross_dirsmall, sbase, srayon, shaut);
   if (ier != HOK)
      return ier;

   for (int ny=0 ; ny<NbrCotes ; ny++)
       {
       Vertex* node = getVertexIJK (CylSmall, nxs, ny, nzs);
       if (node!=NULL)
           node->clearAssociation ();
       // Edge*   edge = getEdgeJ     (CylSmall, nxs, ny, nzs);
       // if (edge!=NULL) edge->clearAssociation ();
       }

   for (int ny=0 ; ny<NbrCotes ; ny++)
       {
       int ny1 = (ny+1) MODULO QUAD4;
       Vertex* node0 = getVertexIJK (CylSmall, nxs, ny, nzs);
       Vertex* node1 = getVertexIJK (CylSmall, nxs, ny1, nzs);
 //    Edge*   edge  = getEdgeJ (CylSmall, nxs, ny, nzs);
       Edge*   edge  = findEdge (node0, node1);
       bicyl_shape.associate (edge);
       }

   return HOK;
}
// ====================================================== makeCylinders
int BiCylinder::makeCylinders (Vertex* ori1, double* vz1, double r1, double h1,
                               Vertex* ori2, double* vz2, double r2, double h2)
{
   grid_type = GR_BICYL;
   int ier = makePipes (ori1,vz1, r1/2, r1,h1, ori2,vz2, r2/2, r2,h2, true);
   return ier;
}
// ====================================================== makePipes
int BiCylinder::makePipes (Vertex* ori1, double* vz1, double rint1,
                           double rext1, double h1, Vertex* ori2, double* vz2,
                           double rint2, double rext2, double h2, bool fill)
{
   cyl_fill = fill;
   cross_hauteur [CylBig]   = h2;
   cross_rayext  [CylBig  ] = rext2;
   cross_rayint  [CylBig  ] = rint2;

   cross_hauteur [CylSmall] = h1;
   cross_rayext  [CylSmall] = rext1;
   cross_rayint  [CylSmall] = rint1;

   ori1->getPoint (cross_orismall);
   ori2->getPoint (cross_oribig  );
   copy_vecteur (vz2, cross_dirbig);
   copy_vecteur (vz1, cross_dirsmall);

//   Intersection des 2 cylindres :
   int ier = checkInter (cross_oribig,   vz2, rext2, h2,
                         cross_orismall, vz1, rext1, h1, 
                         cross_center, at_left, at_right);
   if (ier != HOK)
      return ier;

   createLittlePipe ();
   createBigPipe    ();

   if (at_left) 
      {
      adjustLittleSlice (1, 1);
      adjustLittleSlice (0, 2);
      }
   else if (cyl_fill) 
      {
      adjustLittleSlice (0, 1);
      }

   if (at_right) 
      {
      adjustLittleSlice (0, 3);
      adjustLittleSlice (1, 4);
      }
   else if (cyl_fill) 
      {
      adjustLittleSlice (0, 4);
      }


   transfoVertices (cross_center, cross_dirsmall, cross_dirbig);
   assoCylinders   ();

   //  assoResiduelle ();
   // el_root->reorderQuads ();
   return HOK;
}
END_NAMESPACE_HEXA
