//
// CC++ : Interface Cascade de la ligne
//
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
//--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8

#ifndef __KAS_LINE_HXX_
#define __KAS_LINE_HXX_

#include "HexGlobale.hxx"

#ifndef NO_CASCADE

#include <gp_Pnt.hxx>
#include <BRepAdaptor_Curve.hxx>

BEGIN_NAMESPACE_HEXA

class KasPoint;

class KasLine
{
public :
   KasLine ();
   KasLine (Shape* shape, double deb=-1.0, double fin=-1.0);
  ~KasLine ();
 
   int  findBound (double* point);
   // void setAbscissa (double total, double& abscisse);
   void inverser ();
   void setBounds (double deb, double fin);

   double* getStart  ()       { return start_coord ; }
   double* getEnd    ()       { return end_coord   ; }
   double  getLength ()       { return geom_length ; }
   double  getRank   ()       { return geom_rang ; }

   void defineLine (Shape* asso, double deb=-1.0, double fin=-1.0);
   void setRank    (int rang, int sens, double& abscisse);
   void associate  (Edge* edge, double sm1, double sm2, int orig=V_AMONT);
   void assoEdge   (Edge* edge, double pm1, double pm2, int vass=NOTHING);
   void assoPoint  (double alpha, Vertex* node);

   double findParam  (double* point);

private :
   void majCoord ();

private :
   string lig_brep,  lig_ident, lig_ior;
   double lig_debut, lig_fin;
   double par_mini,  par_maxi;

   bool   geom_inverse; 
   int    geom_rang;
   Real3  start_coord,  end_coord;
   double start_absc, end_absc;        // Abscisse globale
   double geom_length;
   double geom_total_length;

   gp_Pnt             start_gpoint, end_gpoint;
   TopoDS_Edge        geom_line;
   BRepAdaptor_Curve* geom_curve;
};
END_NAMESPACE_HEXA
#endif
#endif
