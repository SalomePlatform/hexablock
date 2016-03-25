
// C++ : Quads Pattern (for replaceHexas)

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

#ifndef __HEX_QPATTERN_H_

#include "HexGlobale.hxx"

#include <cmath>
#include <map>

BEGIN_NAMESPACE_HEXA

// --------------------------------------------------------
class HexaExport QpatQuad
{
public :
   QpatQuad (Quad* refer=NULL);
   void operator= (const QpatQuad& other);
public :
   int   q_edge [QUAD4];
   Quad* refer;
   Quad* cible;
   Quad* oppos;
}; 
// --------------------------------------------------------
class HexaExport QpatEdge
{
public :
   QpatEdge (Edge* refer=NULL);
   void operator= (const QpatEdge& other);

public :
   int   v_amont, v_aval;
   Edge* refer;
   Edge* cible;
   Edge* oppos;
}; 
// --------------------------------------------------------
class HexaExport QpatVertex
{
public :
   QpatVertex (Vertex* refer=NULL);
   void operator= (const QpatVertex& other);

public :
   double  v_x, v_y;
   Vertex* refer;
   Vertex* cible;
   Vertex* oppos;
   bool    on_contour;
   int     quad_cible;
}; 

// --------------------------------------------------------
class HexaExport Qpattern 
{
friend class Elements;
public  :
    Qpattern();
    void setPattern (Vertices& contour, Quads& pattern);
    void setTarget  (Vertices& contour, Quads& target);
    void stepDown ();
    void extrude  ();
    void getSize  (int& nbq, int& nbed, int& nbv, int& prof);

    void anaTarget  (double tmin[], double tmax[], double centre[]);
    Vertex* getTargetVertex (int nro)
           { return old_contour [nro] ; }

private :
    int  setOrigin (double* v1, double* v2, double* v3);
    int  addQuad   (Quad* quad);
    int  addEdge   (Edge*   edge);
    int  addVertex (Vertex* vertex, Vertex* ref=NULL);
    void projeter  (Vertex* vertex, double& ux, double& uy);

private :
   vector <QpatVertex> pat_vertex;
   vector <QpatEdge  > pat_edge;
   vector <QpatQuad  > pat_quad;

   Quads    old_top, old_bottom;
   Hexas    old_hexa;
   Vertices old_contour;

   int      nbr_vertex, nbr_edges, nbr_quads;
   int      cont_nbnodes;
   Real3    base_i, base_j, origine;
   double   ux_mini, uy_mini, ux_delta, uy_delta;

   int pos_vertex4;
   int pos_edge3, pos_edge4;
   int nbr_layers;
};
END_NAMESPACE_HEXA
#endif
