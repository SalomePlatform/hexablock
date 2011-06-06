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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// Class : Gestion des tableaux d'hexaedres

#ifndef __ELEMENTS_H
#define __ELEMENTS_H

#include "HexEltBase.hxx"
#include "HexHexa.hxx"

BEGIN_NAMESPACE_HEXA

class Elements : public EltBase 
{
public:
   virtual Hexa*   getHexa   (int nro);
   virtual Quad*   getQuad   (int nro);
   virtual Edge*   getEdge   (int nro);
   virtual Vertex* getVertex (int nro);

   virtual int countHexa   () { return nbr_hexas ; }
   virtual int countQuad   () { return nbr_quads ; }
   virtual int countEdge   () { return nbr_edges ; }
   virtual int countVertex () { return nbr_vertex ; }

   int findHexa   (Hexa*   elt);
   int findQuad   (Quad*   elt);
   int findEdge   (Edge*   elt);
   int findVertex (Vertex* elt);

public:
   Elements (Document* doc);
   Elements (Document* doc, int nx, int ny, int nz);
   Elements (Elements* orig);

   virtual ~Elements () {}
   virtual void remove ();

   Hexa*   getStrate (int couche, EnumHQuad face);

   Hexa*   getHexaIJK   (int nx, int ny, int nz);
   Quad*   getQuadIJ    (int nx, int ny, int nz);
   Quad*   getQuadJK    (int nx, int ny, int nz);
   Quad*   getQuadIK    (int nx, int ny, int nz);
   Edge*   getEdgeI     (int nx, int ny, int nz);
   Edge*   getEdgeJ     (int nx, int ny, int nz);
   Edge*   getEdgeK     (int nx, int ny, int nz);
   Vertex* getVertexIJK (int nx, int ny, int nz);

   void   addHexa (Hexa* element);
   void   addQuad (Quad* element);
   void   addEdge (Edge* element);
   void   addVertex (Vertex* element);
   int    transform (Matrix* matrice);

   int saveVtk  (cpchar nomfic); 
   int saveVtk  (cpchar nomfic, int& nro); 

   int makeCartesianGrid (Vertex* orig, Vector* v1, Vector* v2, Vector* v3, 
                      int px, int py, int pz, int mx=0, int my=0, int mz=0);

   int makeCylinder (Cylinder* cyl, Vector* base, int nr, int na, int nl);
   int makePipe     (Cylinder* cyl, Vector* base, int nr, int na, int nl);

   int makeCylindricalGrid (Vertex* c, Vector* b, Vector* h, 
         double dr, double da, double dl, int nr, int na, int nl, bool fill);

   int makeSphericalGrid (Vertex* v, Vector* dv, int nb, double k=1);

   int  joinQuads (Quads& q0, int nb, Vertex* v1, Vertex* v2, Vertex* v3, 
                   Vertex* v4, Quad* dest);
   void coupler  (int nro, Quad* other, StrOrient* orient);

   int  prismQuads (Quads& start, Vector* dv, int nb);
   int  cutHexas   (const Edges& edges, int nbcuts);
   int  cutHexas0  (const Edges& edges, int nbcuts);

   void setVertex (Vertex* node, int nx, int ny, int nz);

   void transfoVertices (Vertex* orig, Vector* vi, Vector* vj, Vector* vk);

   void setVertex (Vertex* node, int nro);
   void setEdge   (Edge*   edge, int nro);
   void setQuad   (Quad*   quad, int nro);
   void setHexa   (Hexa*   cell, int nro);

   int nroVertex (int nsommet,  int nquad, int nh);

protected :
 
   int   fillGrid ();

   void  fillCenter    ();
   void  fillCenter4   ();
   void  fillCenter6   ();
   void  fillCenterOdd ();

   int  pushHexas (int nro, Quad* sol, double dx, double dy, double dz, 
                   int hauteur, Quad* toit=NULL, int* decal=NULL);

   void setVertex (int nx, int ny, int nz, double px, double py, double pz);
   void setEdge   (Edge*   edge, EnumCoord dir, int nx, int ny, int nz);
   void setQuad   (Quad*   quad, EnumCoord dir, int nx, int ny, int nz);
   void setHexa   (Hexa*   cell, int nx, int ny, int nz);

   Edge* newEdge (Vertex* v1, Vertex* v2);
   Quad* newQuad (Edge* e1, Edge* e2, Edge* e3, Edge* e4);
   Hexa* newHexa (Quad* e1, Quad* e2, Quad* e3, Quad* e4, Quad* e5, Quad* e6);

   void resize (EnumGrid type, int nx, int ny=0, int nz=0);

   int makeCartesianNodes (Vertex* orig, Vector* v1, Vector* v2, Vector* v3, 
                      int px, int py, int pz, int mx=0, int my=0, int mz=0);

   int makeCylindricalNodes (Vertex* c, Vector* b, Vector* h, 
         double dr, double da, double dl, int nr, int na, int nl, bool fill);

   int makeBasicCylinder (double dr, double da, double dl, int nr, int na, 
                                                           int nl, bool fill);

   int addStrate (Quad* i_quad[], Edge* i_edge[], Vertex* i_node[], 
                  Vertex* center,  double lambda);

   int indVertex (int nsommet,  int nquad, int nh);
   int indVertex (int ref_vert, int nh);
   int nroEdgeH  (int nvertex);
   int nroEdgeH  (int nsommet,  int nquad, int nh);
   int nroHexa   (int nquad, int nh);

   void copyVertex (Elements* other, int px, int py, int pz,  
                                     int nx, int ny, int nz);
   void completerGrille  (double hauteur);

protected :
    Globale* glob;
    EnumGrid grid_type;

    std::vector<Hexa*>   tab_hexa;
    std::vector<Quad*>   tab_quad;
    std::vector<Edge*>   tab_edge;
    std::vector<Edge*>   tab_pilier;
    std::vector<Vertex*> tab_vertex;

    std::vector<Quad*>   tab_orig;

    int size_hx, size_hy, size_hz, size_hplus;
    int size_qx, size_qy, size_qz, size_qplus;
    int size_ex, size_ey, size_ez, size_eplus;
    int size_vx, size_vy, size_vz, size_vplus;

    int nbr_hexas, nbr_quads, nbr_edges, nbr_vertex;
    int nbr_orig, nbr_piliers, ker_vertex;

    int   gr_rayon;              // Spheric
    int   gr_hauteur;            // Joint
    int   nbr_secteurs;          // Cyl

    bool     cyl_closed;         // Cyl
    bool     cyl_fill;           // Cyl
    EnumCyl  cyl_dispo;          // Cyl
};
// =================================================== getStrate
inline Hexa* Elements::getStrate (int couche, EnumHQuad nroface)
{ 
   Hexa* cell = NULL;
   int   nro  = couche <= 0 ? 0 : (couche-1)*HQ_MAXI + nroface + 1;

   if (nbr_hexas==0 || nro >= nbr_hexas)
      cell = NULL;
   else
      cell = tab_hexa [nro]; 

   return cell; 
}
// ============================================================  setHexa
inline void Elements::setHexa (Hexa* elt, int nro) 
{
   if (nro >=0 && nro < nbr_hexas)
       tab_hexa [nro] = elt;
}
// ============================================================  setQuad
inline void Elements::setQuad (Quad* elt, int nro) 
{
   if (nro >=0 && nro < nbr_quads)
       tab_quad [nro] = elt;
}
// ============================================================  setEdge
inline void Elements::setEdge (Edge* elt, int nro)
{
   if (nro >=0 && nro < nbr_edges)
       tab_edge [nro] = elt;
}
// ============================================================  setVertex
inline void Elements::setVertex (Vertex* elt, int nro)
{
   if (nro >=0 && nro < nbr_vertex)
       tab_vertex [nro] = elt;
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// ============================================================  getHexa
inline Hexa* Elements::getHexa (int nro) 
{
   Hexa* elt = NULL;
   int  nombre=tab_hexa.size();
   // if (nro >=0 && nro < nbr_hexas && el_status == HOK Abu 2010/05/06
   if (nro >=0 && nro < nombre && el_status == HOK 
               && tab_hexa [nro] != NULL && tab_hexa [nro]->isValid())
      elt = tab_hexa [nro]; 

   return elt;
}
// ============================================================  getQuad
inline Quad* Elements::getQuad (int nro) 
{
   Quad* elt = NULL;
   if (nro >=0 && nro < nbr_quads && el_status == HOK 
               && tab_quad [nro] != NULL && tab_quad [nro]->isValid())
      elt = tab_quad [nro]; 

   return elt;
}
// ============================================================  getEdge
inline Edge* Elements::getEdge (int nro)
{
   Edge* elt = NULL;
   if (nro >=0 && nro < nbr_edges && el_status == HOK 
               && tab_edge [nro] != NULL && tab_edge [nro]->isValid())
      elt = tab_edge [nro]; 

   return elt;
}
// ============================================================  getVertex
inline Vertex* Elements::getVertex (int nro)
{
   Vertex* elt = NULL;
   if (nro >=0 && nro <  nbr_vertex && el_status == HOK 
               && tab_vertex [nro] != NULL && tab_vertex [nro]->isValid())
      elt = tab_vertex [nro]; 

   return elt;
}
// ============================================================  indVertex
inline int Elements::indVertex (int nquad, int nsommet, int nh)
{
   int nro = nsommet  + QUAD4*nquad + nbr_orig*QUAD4*(nh+1);
   return nro;
}
// ============================================================  nroVertex
inline int Elements::nroVertex (int nquad, int nsommet, int nh)
{
   int nro = nsommet  + QUAD4*nquad + nbr_orig*QUAD4*nh;
   return nro;
}
// ============================================================  indVertex
inline int Elements::indVertex (int ref_edge, int nh)
{
   int    nro = ref_edge + nbr_orig*QUAD4*nh;
   return nro;
}
// ============================================================  nroEdgeH
inline int Elements::nroEdgeH (int nvertex)
{
   return QUAD4*nbr_orig*gr_hauteur + nvertex;
}
// ============================================================  nroEdgeH
inline int Elements::nroEdgeH (int nquad, int nsommet, int nh)
{
   return QUAD4*nbr_orig*gr_hauteur + indVertex (nquad, nsommet, nh);
}
// ============================================================  nroHexa
inline int Elements::nroHexa (int nquad, int nh)
{
   int nro = gr_hauteur*nquad + nh;
   return nro;
}

// ============================================================  addHexa
inline void Elements::addHexa (Hexa* element)
{
   tab_hexa.push_back (element);
   nbr_hexas ++;
}
// ============================================================  addQuad
inline void Elements::addQuad (Quad* element)
{
   tab_quad.push_back (element);
   nbr_quads ++;
}
// ============================================================  addEdge
inline void Elements::addEdge (Edge* element)
{
   tab_edge.push_back (element);
   nbr_edges ++;
}
// ============================================================  addVertex
inline void Elements::addVertex (Vertex* element)
{
   tab_vertex.push_back (element);
   nbr_vertex ++;
}
// ============================================================  findHexa
inline int Elements::findHexa (Hexa* element)
{
   int nbre = tab_hexa.size();
   for (int nro=0 ; nro<nbre ; nro++)
       if (tab_hexa [nro] == element)
          return nro;
   return NOTHING;
}
// ============================================================  findQuad
inline int Elements::findQuad (Quad* element)
{
   int nbre = tab_quad.size();
   for (int nro=0 ; nro<nbre ; nro++)
       if (tab_quad [nro] == element)
          return nro;
   return NOTHING;
}
// ============================================================  findEdge
inline int Elements::findEdge (Edge* element)
{
   int nbre = tab_edge.size();
   for (int nro=0 ; nro<nbre ; nro++)
       if (tab_edge [nro] == element)
          return nro;
   return NOTHING;
}
// ============================================================  findVertex
inline int Elements::findVertex (Vertex* element)
{
   int nbre = tab_vertex.size();
   for (int nro=0 ; nro<nbre ; nro++)
       if (tab_vertex [nro] == element)
          return nro;
   return NOTHING;
}
// ========================================================= saveVtk (avec nro)
inline int Elements::saveVtk  (cpchar radical, int &nro)
{
   char num[8];
   sprintf (num, "%d", nro);
   nro ++;

   string filename = radical;
   filename += num;
   filename += ".vtk";
   int ier = saveVtk (filename.c_str());
   return ier;
}

END_NAMESPACE_HEXA
#endif
