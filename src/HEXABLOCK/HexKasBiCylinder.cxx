//
// CC++ : Interface Cascade de la classe Elements
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

#include "HexKasBiCylinder.hxx"
#include "HexKasLine.hxx"
#include "HexKas_functions.hxx"

#ifndef NO_CASCADE

#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>

#include <GCPnts_AbscissaPoint.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Vertex.hxx>

#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>

// HEXABLOCK includes
#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexShape.hxx"
#include "HexDiagnostics.hxx"
                                    // Cercles 
#include <GEOMImpl_CircleDriver.hxx>
#include <GEOMImpl_ICircle.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Tool.hxx>

#include <GC_MakeCircle.hxx>
#include <Geom_Circle.hxx>

#include <gp_Circ.hxx>
                                    // Sphere
#include <BRepPrimAPI_MakeSphere.hxx>
                                    // Cylindre
#include <GEOMImpl_CylinderDriver.hxx>
#include <GEOMImpl_ICylinder.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepAlgoAPI_Section.hxx>
                                    // Decoupage
#include <TopExp_Explorer.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>

BEGIN_NAMESPACE_HEXA

static bool db = true;

static KasBiCylinder current_bicyl;

// ====================================================== purge
void KasBiCylinder::purge ()
{
    no_inter = true;
    inter_line.clear ();
}
// ====================================================== defineCyls 
int KasBiCylinder::defineCyls (double* borig,  double* bnorm, double* bbase,
                              double  brayon, double  bhaut,
                              double* sorig,  double* snorm, double* sbase,
                              double  srayon, double  shaut)
{
   purge ();
   if (db)
      {
      printf (" ====================== defineCyls\n");
      printf ("    ----- borig=(%g,%g,%g)\n", borig[0], borig[1], borig[2]);
      printf ("    ----- bbase=(%g,%g,%g)\n", bbase[0], bbase[1], bbase[2]);
      printf ("    ----- bnorm=(%g,%g,%g)\n", bnorm[0], bnorm[1], bnorm[2]);
      printf ("    ----- brayon=%g, bhaut=%g\n", brayon, bhaut);
      printf ("\n");
      printf ("    ----- sorig=(%g,%g,%g)\n", sorig[0], sorig[1], sorig[2]);
      printf ("    ----- sbase=(%g,%g,%g)\n", sbase[0], sbase[1], sbase[2]);
      printf ("    ----- snorm=(%g,%g,%g)\n", snorm[0], snorm[1], snorm[2]);
      printf ("    ----- srayon=%g, shaut=%g\n", srayon, shaut);
      }

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

   TopoDS_Shape    cyl_inter = make_inter.Shape();
   // geom_make_brep (cyl_inter, brep);

                    // --------------------------- Recuperation
   string crep;
   Shape shape (crep);

   TopExp_Explorer explo (cyl_inter, TopAbs_EDGE);
   int nroline = 0;
   static int nbrep = 0;
   while (explo.More())
         {
         TopoDS_Shape ligne = explo.Current();
         // TopoDS_Edge ligne = explo.Current();
         explo.Next ();

         cout << "____________________________________ Ligne suivante" << endl;
         geom_make_brep (ligne, crep);
         if (db) 
            save_brep ("inter", crep, nbrep);
         shape.setBrep (crep);

         KasLine* geom_line = new KasLine (&shape);
         inter_line.push_back (geom_line);
         nroline ++;
         }
   return HOK;
}
// ====================================================== anaVertex
// === Trouve le(s) ligne(s) contenant ce vertex et le(s) parametre(s)
int KasBiCylinder::anaVertex (Vertex* node, int* tline, double* tpara)
{
   Real3  point; 
   node->getPoint (point);
   int nbsol = 0;
   int nblines = inter_line.size();

   for (int nl=0 ; nl<nblines ; nl++)
       {
       double param = inter_line[nl]->findParam (point);
       if (param>=0)
          {
          if (db) 
              cout << " ... findParam " << node->getName() 
                   << ", point=(" << point[0] << ", " << point[1] 
                                 << ", " << point[2] 
                   << "), nl=" << nl << ", param=" << param << endl;
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
int KasBiCylinder::associate (Edge* edge)
{
   edge->clearAssociation ();

   double tparam1 [V_TWO], tparam2 [V_TWO];
   int    tline1  [V_TWO], tline2  [V_TWO];
   int sol1 = anaVertex (edge->getVertex(V_AMONT), tline1, tparam1);
   int sol2 = anaVertex (edge->getVertex(V_AVAL),  tline2, tparam2);

   if (sol1==0 || sol2==0)
      return HERR;
                                // Ligne commune ? 
   for (int ns1=0 ; ns1<sol1 ; ns1++)
       {
       int nlig = tline1[ns1];
       for (int ns2=0 ; ns2<sol2 ; ns2++)
           {
           if (tline2[ns2] == nlig)
              {
              inter_line[nlig]->assoEdge (edge, tparam1[ns1], tparam2[ns2], 2);
              return HOK;
              }  
           }
       }
   
    inter_line[tline1[0]]->assoEdge (edge, tparam1[0],    1.0,  V_AMONT);
    inter_line[tline2[0]]->assoEdge (edge, 0,      tparam2[0], V_AVAL);
    return HOK;
}
// ====================================================== geom_create_cylcyl
int geom_create_cylcyl (double* borig, double* bnorm, double* bbase,
                        double  bray,  double  bhaut,
                        double* sorig, double* snorm, double* sbase,
                        double  sray,  double  shaut)
{
   int ier = current_bicyl.defineCyls (borig, bnorm, bbase, bray,  bhaut,
                                       sorig, snorm, sbase, sray,  shaut);
   return ier;
}
// ====================================================== geom_asso_cylcyl
int geom_asso_cylcyl (Edge* edge)
{
   cout << " ___________________________________ geom_asso_cylcyl " 
        << edge->getName () << " = (" << edge->getVertex(0)->getName () 
                            << ","  << edge->getVertex(1)->getName () 
        << ")" << endl;

   int ier = current_bicyl.associate (edge);
   return ier;
}
END_NAMESPACE_HEXA
#endif
