
// class : Analyse des quadrangles avant construction

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
#ifndef __ANA_QUADS_H
#define __ANA_QUADS_H

#include "HexQuad.hxx"
// #include "HexEdge.hxx"

BEGIN_NAMESPACE_HEXA

class HexaExport AnaQuads
{
public:
    AnaQuads (Quad* q1, Quad* q2, Quad* q3=NULL, Quad* q4=NULL, Quad* q5=NULL);

public:
    enum {MaxQuads=5};
    int    status;
    int    nbr_quads;
    int    nbr_aretes;
    Quad*  tab_quads  [MaxQuads];
    int    inter_nbre [MaxQuads];
    int    inter_edge [MaxQuads][MaxQuads]; // L'arete commune des 2 quads
    int    inter_quad [MaxQuads][QUAD4];    // Le quad touchant la ieme arete
};
// ----------------------------------------------- Inlining
// ========================================================== Constructeur
inline AnaQuads::AnaQuads (Quad* q0, Quad* q1, Quad* q2, Quad* q3, Quad* q4)
{
   nbr_quads  = 2;
   nbr_aretes = 0;
   status     = HOK;

   for (int nquad=0 ; nquad < MaxQuads ; nquad++)
       {
       tab_quads  [nquad] = NULL;
       inter_nbre [nquad] = 0;
       for (int nro=0 ; nro < QUAD4 ; nro++)
           inter_quad [nquad][nro] = NOTHING;
       for (int nro=0 ; nro < MaxQuads ; nro++)
           inter_edge [nquad][nro] = NOTHING;
       }

   tab_quads [0] = q0;
   tab_quads [1] = q1;
   tab_quads [2] = q2;
   tab_quads [3] = q3;
   tab_quads [4] = q4;

   if      (q4 != NULL) nbr_quads = 5;
   else if (q3 != NULL) nbr_quads = 4;
   else if (q2 != NULL) nbr_quads = 3;
   else                 nbr_quads = 2;

   for (int nquad1=0 ; nquad1 < nbr_quads ; nquad1++)
       {
       if (tab_quads[nquad1]==NULL || tab_quads[nquad1]->isDeleted())
          {
          status = HERR;
          return;
          }
       for (int nquad2=0 ; nquad2 < nquad1 ; nquad2++)
           {
           if (tab_quads[nquad1]==tab_quads[nquad2]) 
              {
              status = HERR;
              return;
              }
           int nedge2 = 0;
           int nedge1 = tab_quads[nquad1]->inter (tab_quads[nquad2], nedge2) ;
           if (nedge1 != NOTHING)
              {
              inter_edge [nquad1] [nquad2] = nedge1;
              inter_quad [nquad1] [nedge1] = nquad2;

              inter_edge [nquad2] [nquad1] = nedge2;
              inter_quad [nquad2] [nedge2] = nquad1;

              inter_nbre [nquad1]++;
              inter_nbre [nquad2]++;
              nbr_aretes++;
              }
           }
       }
}
// ============================================================== distance
inline double distance (Vertex* v1, Vertex* v2)
{
   double vx   = v1->getX () - v2->getX ();
   double vy   = v1->getY () - v2->getY ();
   double vz   = v1->getZ () - v2->getZ ();
   double dist = sqrt (vx*vx + vy*vy + vz*vz);
   return dist;
}
END_NAMESPACE_HEXA
#endif
