//
// CC++ : Interface Cascade de la classe Elements
//
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

#include "HexBiCylinderShape.hxx"

#include "HexDocument.hxx"
#include "HexNewShape.hxx"
#include "HexEdgeShape.hxx"

#include "HexVertex.hxx"
#include "HexEdge.hxx"

                                    // Cylindre
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepAlgoAPI_Section.hxx>

BEGIN_NAMESPACE_HEXA

static bool db = false;
int BiCylinderShape::nbr_intersections = 0;

// ====================================================== Constructeur
BiCylinderShape::BiCylinderShape (Document* doc)
{
   bi_docu  = doc;
   bi_shape = NULL;
   no_inter = true;
}
// ====================================================== defineCyls
int BiCylinderShape::defineCyls (double* borig,  double* bnorm, double* bbase,
                              double  brayon, double  bhaut,
                              double* sorig,  double* snorm, double* sbase,
                              double  srayon, double  shaut)
{
                    // --------------------------- Preparation
   gp_Pnt gpb_orig (borig [dir_x], borig [dir_y], borig [dir_z]);
   gp_Vec gpb_norm (bnorm [dir_x], bnorm [dir_y], bnorm [dir_z]);
   gp_Vec gpb_vx   (bbase [dir_x], bbase [dir_y], bbase [dir_z]);
   gp_Ax2 gpb_axes (gpb_orig, gpb_norm, gpb_vx);
// gp_Ax2  gp_axes (gp_center, gp_norm, gp_vx);

   BRepPrimAPI_MakeCylinder make_bcyl (gpb_axes, brayon, bhaut);
   make_bcyl.Build();

   gp_Pnt gps_orig (sorig [dir_x], sorig [dir_y], sorig [dir_z]);
   gp_Vec gps_vx   (sbase [dir_x], sbase [dir_y], sbase [dir_z]);
   gp_Vec gps_norm (snorm [dir_x], snorm [dir_y], snorm [dir_z]);
   gp_Ax2 gps_axes (gps_orig, gps_norm, gpb_vx);

   BRepPrimAPI_MakeCylinder make_scyl (gps_axes, srayon, shaut);
   make_scyl.Build();

   if (NOT make_bcyl.IsDone())
      {
      printf ("defineCyls : Can' build big cylinder\n");
      return HERR;
      }
   else if (NOT make_scyl.IsDone())
      {
      printf ("defineCyls : Can' build small cylinder\n");
      return HERR;
      }
                    // --------------------------- Intersection

   TopoDS_Shape cyl_big   = make_bcyl.Shape();
   TopoDS_Shape cyl_small = make_scyl.Shape();

   BRepAlgoAPI_Section make_inter (cyl_big, cyl_small, false);
   make_inter.Approximation (true);
   make_inter.Build();

   if (NOT make_inter.IsDone())
      {
      printf ("defineCyls : No intersection\n");
      return HERR;
      }

   TopoDS_Shape cyl_inter = make_inter.Shape();
   // geom_make_brep (cyl_inter, brep);

                    // --------------------------- Recuperation
   char name [16];
   nbr_intersections ++;
   sprintf (name, "inter_cyl%d", nbr_intersections);
   bi_shape = bi_docu->addShape (name, SH_INTER);
   bi_shape->setShape (cyl_inter, SH_INTER);
   bi_shape->saveBrep ();        // PROVISOIRE

   return HOK;
}
// ====================================================== anaVertex
// === Trouve le(s) ligne(s) contenant ce vertex et le(s) parametre(s)
int BiCylinderShape::anaVertex (Vertex* node, int* tline, double* tpara)
{
   Real3  point;
   node->getPoint (point);
   int nbsol   = 0;
   int nblines = bi_shape->countEdge ();

   for (int nl=0 ; nl<nblines ; nl++)
       {
       EdgeShape* line  = bi_shape->getEdgeShape (nl);
       double     param = line->getParam (point);
       if (db) cout << " ... getParam " << node->getName()
                    << ", point=(" << point[0] << ", " << point[1]
                    << ", " << point[2]
                    << "), nl=" << nl << ", param=" << param << endl;
       if (param>=0)
          {
          if (nbsol>=2)
             return nbsol;

          tline [nbsol] = nl;
          tpara [nbsol] = param;
          nbsol ++;
          }
       }

   if (nbsol==1)
      {
      if (tpara[0]<=Epsil)
         {
         nbsol ++;
         tpara[1] = 1.0;
         tline[1] = tline[0]-1;
         if (tline[1] <0) tline[1] = nblines-1;
         }
      else if (tpara[0]>=UnEpsil)
         {
         nbsol ++;
         tpara[1] = 0;
         tline[1] = tline[0]+1;
         if (tline[1] >= nblines) tline[1] = 0;
         }
      }

   return nbsol;
}
// ====================================================== associate
// ==== On suppose une orientation correcte
int BiCylinderShape::associate (Edge* edge)
{

   double tparam1 [V_TWO], tparam2 [V_TWO];
   int    tline1  [V_TWO], tline2  [V_TWO];
   int sol1 = anaVertex (edge->getVertex(V_AMONT), tline1, tparam1);
   int sol2 = anaVertex (edge->getVertex(V_AVAL),  tline2, tparam2);

   if (sol1==0 || sol2==0)
      return HERR;

   edge->clearAssociation ();
                                // Ligne commune ?
   for (int ns1=0 ; ns1<sol1 ; ns1++)
       {
       int nlig  = tline1[ns1];
       for (int ns2=0 ; ns2<sol2 ; ns2++)
           {
           if (tline2[ns2] == nlig)
              {
              EdgeShape* line = bi_shape->getEdgeShape (nlig);
              double   param1 = tparam1 [ns1];
              double   param2 = tparam2 [ns2];
              if (param1 >= 1.0-Epsil && param1 <= 1.0+Epsil)
                  param1  = 0.0;
              if (param2 >= -Epsil && param2 <= Epsil)
                  param2  = 1.0;
              if (param1 < param2)
                  {
                  associate (edge, line, param1, param2, V_TWO);
                  }
              else
                     // Le debut de la ligne (fermee) est entre les 2 vertex)
                  {
                  associate (edge, line, param1, 1, V_AMONT);
                  associate (edge, line, 0, param2, V_AVAL);
                  }
              return HOK;
              }
           }
       }


   EdgeShape* line1 = bi_shape->getEdgeShape (tline1[0]);
   EdgeShape* line2 = bi_shape->getEdgeShape (tline2[0]);

   associate (edge, line1, tparam1[0], 1, V_AMONT);
   associate (edge, line2, 0, tparam2[0], V_AVAL);
   return HOK;
}
// ====================================================== associate
// ==== On suppose une orientation correcte
int BiCylinderShape::associate (Edge* edge, EdgeShape* line, 
                                double para1, double para2, int extrem)
{
   edge->addAssociation (line, para1, para2);

   return HOK;                    // PROVISOIRE
   Real3   point;
   Vertex* vertex;

   if (extrem != V_AVAL)
      {
      line->getPoint   (para1, point);
      vertex =  edge->getVertex (V_AMONT);
      vertex->setAssociation (point);
      }

   if (extrem != V_AMONT)
      {
      line->getPoint   (para2, point);
      vertex =  edge->getVertex (V_AVAL);
      vertex->setAssociation (point);
      }
}
END_NAMESPACE_HEXA
