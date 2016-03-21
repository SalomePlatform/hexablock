
// Class : Gestion des tutaux en T (like castles in scotland)

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
#ifndef __BI_CYLINDER_H_
#define __BI_CYLINDER_H_

#include "HexElements.hxx"
#include <map>

BEGIN_NAMESPACE_HEXA

class HexaExport BiCylinder : public Elements
{
public:
   BiCylinder (Document* doc);

   Hexa*   getHexaIJK   (int part, int nx, int ny, int nz);
   Quad*   getQuadIJ    (int part, int nx, int ny, int nz);
   Quad*   getQuadJK    (int part, int nx, int ny, int nz);
   Quad*   getQuadIK    (int part, int nx, int ny, int nz);
   Edge*   getEdgeI     (int part, int nx, int ny, int nz);
   Edge*   getEdgeJ     (int part, int nx, int ny, int nz);
   Edge*   getEdgeK     (int part, int nx, int ny, int nz);
   Vertex* getVertexIJK (int part, int nx, int ny, int nz);

#ifndef SWIG
public :
   virtual ~BiCylinder () {}
                     // Tutaux en T (like castles in scotland)
   int makeCylinders (Vertex* ori1, double* vz1, double ray1, double h1, 
                      Vertex* ori2, double* vz2, double ray2, double h2);

   int makePipes (Vertex* ori1, double* z1, double ri1, double rx1, double h1, 
                  Vertex* ori2, double* z2, double ri2, double rx2, double h2, 
                  bool fill=false);

   virtual int countHexa   () { return nbr_hexas ; }
   virtual int countQuad   () { return nbr_quads ; }
   virtual int countEdge   () { return nbr_edges ; }
   virtual int countVertex () { return nbr_vertex ; }

   // int  crossCylinders (Cylinder* cyl1, Cylinder* cyl2); // perime

private :

   enum { NbrSlices=5, NbrVslices=NbrSlices+1, NbrCotes=4 };

   void createLittlePipe ();
   void createBigPipe    ();
   void adjustLittleSlice  (int nx, int nzs);
   void adjustLittleSquare (int nk);

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

   Edge* findEdge (Vertex* v1, Vertex* v2);    // A replacer dans HexElements

   void assoCylinders ();
   void assoSlice    (int cyl, int nx, int nzs, double* norm);
   void assoArc (int cyl, int nx, int ny, int nz, int subid);
   void assoArc (int cyl, int ny, Vertex* v1, Vertex* v2, int subid);
   int  assoIntersection (int nx, int nzlice);


   void addSlice  (int cyl, int i, int k, double h, double r, bool find=false);
   void addCarre  (int cyl, int k, double h, double r, bool find=false);
   void addCube   (int cyl, int k, int i=0, int k1=-1);
   void fillSlice (int cyl, int nia, int nka, int nib, int nkb, int nic,
                            int nkc, int nid, int nkd, bool med=false);

   double getAngle (int nj)             { return  (2*((nj)%4)+1)*M_PI/4; }
   int getKey (int cyl, int ni, int nj, int nk)
              { return 1000*(cyl+1) + 100*ni + 10*nj + nk; }
   int getKey (int cyl, int dd, int ni, int nj, int nk)
              { return 10000*(cyl+1) + 1000*dd + 100*ni + 10*nj + nk; }
private :
    Real3     cross_center;
    Real3     cross_oribig,   cross_dirbig;
    Real3     cross_orismall, cross_dirsmall;

    map <int,int> map_vertex, map_edge, map_quad, map_hexa;
    map <int,int> :: iterator it_map;

    int    nbr_hexas1, nbr_quads1, nbr_edges1, nbr_vertex1;
    double cross_rayext  [V_TWO];
    double cross_rayint  [V_TWO];
    double cross_hauteur [V_TWO];

    bool   at_right, at_left;

    NewShape* grid_geom;
#endif
};
END_NAMESPACE_HEXA
#endif
