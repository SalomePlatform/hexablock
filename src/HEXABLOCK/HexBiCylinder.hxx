
// Class : Gestion des tutaux en T (like castles in scotland)

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
//
#ifndef __BI_CYLINDER_H_
#define __BI_CYLINDER_H_

#include "HexElements.hxx"
#include <map>

BEGIN_NAMESPACE_HEXA

class BiCylinder : public Elements 
{
public:
   virtual int countHexa   () { return nbr_hexas ; }
   virtual int countQuad   () { return nbr_quads ; }
   virtual int countEdge   () { return nbr_edges ; }
   virtual int countVertex () { return nbr_vertex ; }

public:
   BiCylinder (Document* doc);

   virtual ~BiCylinder () {}

   Hexa*   getHexaIJK   (int part, int nx, int ny, int nz);
   Quad*   getQuadIJ    (int part, int nx, int ny, int nz);
   Quad*   getQuadJK    (int part, int nx, int ny, int nz);
   Quad*   getQuadIK    (int part, int nx, int ny, int nz);
   Edge*   getEdgeI     (int part, int nx, int ny, int nz);
   Edge*   getEdgeJ     (int part, int nx, int ny, int nz);
   Edge*   getEdgeK     (int part, int nx, int ny, int nz);
   Vertex* getVertexIJK (int part, int nx, int ny, int nz);

                     // Tutaux en T (like castles in scotland)
   int  crossCylinders (Cylinder* cyl1, Cylinder* cyl2);

private :
 
   enum { NbrSlices=5, NbrVslices=NbrSlices+1, NbrCotes=4 };

   void createLittleCyl ();
   void createBigCyl    ();
   void adjustLittleSlice (int nx, int nzs);

   Vertex* addVertex (double px, double py, double pz, 
                      int cyl, int ni, int nj, int nk);
   Edge* addEdge (Vertex* v1, Vertex* v2, 
                  int cyl, int dir, int nx, int ny, int nz);
   Quad* addQuad (Edge* e1, Edge* e2, Edge* e3, Edge* e4, 
                  int cyl, int dir, int nx, int ny, int nz);
   Hexa* addHexa (Quad* qa, Quad* qb, Quad* qc, Quad* qd, Quad* qe, Quad* qf,
                  int cyl, int nx, int ny, int nz);

   Vertex* findVertex (double px, double py, double pz, int ni, int nj, int nk);
   Edge*   findEdge (Vertex* v1, Vertex* v2, int dir, int nx, int ny, int nz);
   Quad*   findQuad (Vertex* v1, Vertex* v2, int dir, int nx, int ny, int nz);
   Quad*   findQuad (Edge* e1, Edge* e2, int dir, int nx, int ny, int nz);
   Hexa*   findHexa (Quad* qa, Quad* qb, int nx, int ny, int nz);

   Edge* findEdge (Vertex* v1, Vertex* v2);
   Quad* findQuad (Edge*   e1, Edge*   e2);

   void assoCylinder  (int cyl, double* normal);
   void assoCylinders (double* snormal, double* gnormal);
   void assoSlice    (int cyl, int nx, int nzs, double* norm);
   void assoArc (int cyl, int nx, int ny, int nz, string& brep, double ray);
   int  assoIntersection (int nx, int nzlice, double* snorm, double* bnorm);


   void addSlice  (int cyl, int i, int k, double h, double r, bool find=false);
   void fillSlice (int cyl, int nia, int nka, int nib, int nkb, int nic, 
                            int nkc, int nid, int nkd, bool med=false);
   void fillSmallCyl ();

   double getAngle (int nj)             { return  (2*((nj)%4)+1)*M_PI/4; }
   int getKey (int cyl, int ni, int nj, int nk) 
              { return 1000*(cyl+1) + 100*ni + 10*nj + nk; }
   int getKey (int cyl, int dd, int ni, int nj, int nk) 
              { return 10000*(cyl+1) + 1000*dd + 100*ni + 10*nj + nk; }
private :
    Cylinder* cross_cyl1;    // Petit
    Cylinder* cross_cyl2;    // gros
    Real3     cross_center;
   
    map <int,int> map_vertex, map_edge, map_quad, map_hexa;
    map <int,int> :: iterator it_map;

    int    nbr_hexas1, nbr_quads1, nbr_edges1, nbr_vertex1;
    double cross_rayext  [V_TWO];
    double cross_rayint  [V_TWO];
    double cross_hauteur [V_TWO];

    bool   at_right, at_left;
};
END_NAMESPACE_HEXA
#endif
