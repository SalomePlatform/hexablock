
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
#ifndef __CROSS_ELEMENTS_H
#define __CROSS_ELEMENTS_H

#include "HexElements.hxx"

BEGIN_NAMESPACE_HEXA

enum {CylSmall=0, CylBig=1, NbrSlices1=6, NbrSlices2=4, NxInt=1, NxExt=2 };
enum {SizeRay=3, BiCyl=2};
enum {NbrVSlices1=NbrSlices1+1, NbrVSlices2=NbrSlices2+1};
enum {Cyl1=CylSmall, Cyl2=CylBig };

class CrossElements : public Elements 
{
public:
   virtual int countHexa   () { return nbr_hexas ; }
   virtual int countQuad   () { return nbr_quads ; }
   virtual int countEdge   () { return nbr_edges ; }
   virtual int countVertex () { return nbr_vertex ; }

public:
   CrossElements (Document* doc, EnumGrid type=GR_BICYL);

   virtual ~CrossElements () {}

   Hexa*   getHexaIJK   (int part, int nx, int ny, int nz);
   Quad*   getQuadIJ    (int part, int nx, int ny, int nz);
   Quad*   getQuadJK    (int part, int nx, int ny, int nz);
   Quad*   getQuadIK    (int part, int nx, int ny, int nz);
   Edge*   getEdgeI     (int part, int nx, int ny, int nz);
   Edge*   getEdgeJ     (int part, int nx, int ny, int nz);
   Edge*   getEdgeK     (int part, int nx, int ny, int nz);
   Vertex* getVertexIJK (int part, int nx, int ny, int nz);

                     // Tutaux en T (like castles in scotland)
   int  crossCylinders (Cylinder* cyl1, Cylinder* cyl2, bool fill=true);
   void dump ();
   void dumpVertex ();
   void dumpHexas  ();

private :
 
   void resize ();
   void fillGrid   (int cyl, int deb=-1, int fin=-1);
   void fillCenter (int cyl, int deb, int fin);
   void createLittleCyl ();
   void createBigCyl    ();

   void copyVertex (int i1, int j1, int k1, int i2, int j2, int k2);
   void copyEdge (int d1, int i, int j, int k, int d2, int i2, int j2, int k2);
   void copyQuad (int d1, int i, int j, int k, int d2, int i2, int j2, int k2);

   void setVertex (int cyl, int nx, int ny, int nz, double px, double py, 
                                                               double pz);

   void setVertex (Vertex* vv, int cyl, int nx, int ny, int nz);
   void setEdge   (Edge* edge, int cyl, int dir, int nx, int ny, int nz);
   void setQuad   (Quad* quad, int cyl, int dir, int nx, int ny, int nz);
   void setHexa   (Hexa* cell, int cyl, int nx, int ny, int nz);

   Edge* addEdge   (Vertex* v1, Vertex* v2, int cyl, int dir, int nx, 
                                                    int ny, int nz);
   Quad* addQuad   (Edge* e1, Edge* e2, Edge* e3, Edge* e4, int cyl, int dir, 
                   int nx, int ny, int nz);
   Hexa* addHexa   (Quad* qa, Quad* qb, Quad* qc, Quad* qd, Quad* qe, Quad* qf,
                   int cyl, int nx, int ny, int nz);

   int  indVertex (int cyl, int nx, int ny, int nz);
   int  indHexa   (int cyl, int nx, int ny, int nz);
   int  indQuad   (int cyl, int dir, int nx, int ny, int nz);
   int  indEdge   (int cyl, int dir, int nx, int ny, int nz);

   Hexa* findHexa1 (Quad*   q1, Quad*   q2);
   Edge* findEdge1 (Vertex* v1, Vertex* v2);
   Quad* findQuad1 (Edge*   e1, Edge*   e2);

   void assoCylinder  (int cyl, double* normal);
   void assoSlice     (int cyl, double* base, double* norm, int nx, int zlice);
   void assoArc    (int cyl, int nx, int ny, int nz, string& brep, double ray);
   void assoBigMiddle (double* base, double* normal, int nzlice);


   double getAngle  (int cyl, int ny);
   void   addSlice  (int cyl, int ni, int nk, double px, double rayon=-1);
   void   addVertex (int cyl, int ni, int nj, int nk, double px, double rayon);
   void   majIntersection ();
   void   adjustLittleSlice (int nx, int nk, int nxbig);

private :
    enum {NbrIntCotes = 4 };
    enum {size_hiy = NbrIntCotes, size_h1z = 6,    size_h2z=4};
    enum {size_v1z = size_h1z+1,  size_v2z = size_h2z+1};

    bool      is_filled;
    Cylinder* cross_cyl1;
    Cylinder* cross_cyl2;
    Vertex*   cross_center;

    int  size_vz[BiCyl], size_hz[BiCyl];
    int  nbr_hexas1, nbr_quads1, nbr_edges1, nbr_vertex1;

    double angle_inter [BiCyl];
    double cross_rayon [BiCyl][SizeRay];
    double big_hauteur [size_v2z];

    bool      at_right, at_left;
};
END_NAMESPACE_HEXA
#endif
