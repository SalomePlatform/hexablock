//
// CC++ : Interface Cascade de la classe Elements
//
// Copyright (C) 2009-2013  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include "HexElements.hxx"
#include "HexEdge.hxx"
#include "HexDocument.hxx"

#include "HexKasBiCylinder.hxx"
#include "HexKasPoint.hxx"
#include "HexKasLine.hxx"

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
#include "HexOldShape.hxx"
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

BEGIN_NAMESPACE_HEXA

static bool db = false;

void clear_associations (Edge* edge);

// ---------------------------------------------------------------------

static KasLine      current_line;
static Shape        current_shape ("");

// ========================================================= cutAssociation
void Elements::cutAssociation (Shapes& tshapes, Edges& tedges, bool exist)
{
   db = on_debug ();
   char foo[18];
   int nbedges  = tedges.size();
   int nbshapes = tshapes.size ();
   if (nbshapes==0)
      return;

   std::vector <KasLine>  tab_gline  (nbshapes);

   Vertex* prems = tedges [0]         -> getVertex (V_AMONT);
   Vertex* derns = tedges [nbedges-1] -> getVertex (V_AVAL);

   KasPoint pnt_first, pnt_last;
   pnt_first.definePoint (prems);
   pnt_last .definePoint (derns);

   if (exist)
      {
      if (pnt_first.isBad())
         {
         el_root->putError (W_ASSO_CUT1, prems->getName (foo));
         return;
         }
      else if (pnt_last.isBad ())
         {
         el_root->putError (W_ASSO_CUT2, derns->getName (foo));
         return;
         }
      }
                            // ----------- Define + longueur totale
   double  longueur = 0;
   for (int ns = 0 ; ns<nbshapes ; ns++)
       {
       tab_gline[ns].defineLine (tshapes[ns]);
       longueur += tab_gline[ns].getLength ();
       }

                            // ----------- Ordonnancement des shapes
   double  abscisse = 0;
   int     nslast   = 0;
   double* coord    = pnt_first.getCoord ();
   if (nbshapes==1)
      {
      tab_gline [0].setRank (0, 0, abscisse);
      }
   else
      {

      for (int rg = 0 ; rg<nbshapes ; rg++)
          {
          bool more = true;
          for (int ns = 0 ; ns<nbshapes && more ; ns++)
              {
              if (tab_gline[ns].getRank()==NOTHING)
                 {
                 int pos = tab_gline[ns].findBound (coord);
                 if (pos!=NOTHING)
                    {
                    more   = false;
                    nslast = ns;
                    tab_gline [ns].setRank (rg, pos, abscisse);
                    coord = tab_gline [ns].getEnd ();
                    }
                 }
              }
                          // Pas trouve
          if (more)
             {
             el_root->putError (W_ASSO_CUT3, derns->getName (foo));
             return;
             }
          }
      }
                            // ----------- Dernier
   if (exist)
      {
      coord = pnt_last.getCoord ();
      int pos = tab_gline[nslast].findBound (coord);
      if (pos != V_AVAL)
         {
         el_root->putError (W_ASSO_CUT4, derns->getName (foo));
         return;
         }
      }
                            // ----------- Associations
   double delta = longueur / nbedges;
   for (int ned = 0 ; ned<nbedges ; ned++)
       {
       if (db) cout << " ++ Association Edge nro "
                    << ned << "/" <<nbedges << endl;
       Edge*  edge = tedges[ned];
       double  sm1 = ned*delta;
       double  sm2 = sm1 + delta;
       for (int ns = 0 ; ns<nbshapes ; ns++)
           {
           tab_gline[ns].associate (edge, sm1, sm2);
           }

       }
   if (db) cout << " +++ End of Elements::cutAssociation" << endl;
}
// ====================================================== geom_define_line
void geom_define_line (string& brep)
{
   current_shape.setBrep   (brep);
   current_shape.setBounds (0, 1);
   current_line.defineLine (&current_shape);
}
// ====================================================== geom_asso_point
void geom_asso_point (double angle, Vertex* node)
{
   if (node!=NULL && node->getAssociation() == NULL)
       current_line.assoPoint (angle, node);
}
// ====================================================== geom_asso_point
void geom_asso_point (Vertex* node)
{
   if (node==NULL || node->getAssociation() != NULL)
      return;

   Real3 koord = { node->getX(), node->getY(), node->getZ() };
   KasPoint  asso_point ;
   asso_point.definePoint (koord);
   asso_point.associate   (node);
}
// ====================================================== geom_create_circle
void geom_create_circle (double* milieu, double rayon, double* normale,
                         double* base, string& brep)
{
   db = on_debug ();
   if (db) printf ("geom_create_circle c=(%g,%g,%g), r=%g\n",
                    milieu[0], milieu[1], milieu[2], rayon);
   if (db) printf ("    -------- base=(%g,%g,%g)\n", base[0], base[1], base[2]);
   if (db) printf ("    -------- norm=(%g,%g,%g)\n", normale[0], normale[1],
                                                  normale[2]);

   gp_Pnt gp_center (milieu [dir_x], milieu [dir_y], milieu [dir_z]);
   gp_Vec gp_vx     (base   [dir_x], base   [dir_y], base   [dir_z]);
   gp_Vec gp_norm   (normale[dir_x], normale[dir_y], normale[dir_z]);

   gp_Ax2  gp_axes (gp_center, gp_norm, gp_vx);
   gp_Circ gp_circ (gp_axes,   rayon);

   TopoDS_Edge    geom_circ = BRepBuilderAPI_MakeEdge(gp_circ).Edge();
   ostringstream  stream_shape;
   BRepTools::Write(geom_circ, stream_shape);
   brep = stream_shape.str();

   // geom_make_brep (geom_circ, brep);
   if (NOT db)
      return;
                             // Impressions de mise au point
   double umin = 0, umax = 0;
   TopLoc_Location    loc;
   Handle(Geom_Curve) handle = BRep_Tool::Curve (geom_circ, loc, umin, umax);
   GeomAdaptor_Curve  AdaptCurve (handle);
   double length = GCPnts_AbscissaPoint::Length(AdaptCurve, umin, umax);

   BRepAdaptor_Curve geom_curve (geom_circ);

   for (int pk=0; pk<=4; pk++)
       {
       GCPnts_AbscissaPoint s1 (geom_curve, pk*length/4,
                                geom_curve.FirstParameter());
       double u1    = s1.Parameter ();
       gp_Pnt point = geom_curve.Value (u1);
       if (db)
          printf ( " ..... pnt%d = (%g, %g, %g)\n", pk, point.X(),
                                            point.Y(),  point.Z());
       }
}
// ====================================================== geom_create_sphere
void geom_create_sphere (double* milieu, double radius, string& brep)
{
   gp_Pnt gp_center (milieu [dir_x], milieu [dir_y], milieu [dir_z]);
   gp_Ax2 gp_axis = gp_Ax2 (gp_center, gp_Dir(0,0,1), gp_Dir(1,0,0));

   BRepPrimAPI_MakeSphere make_sphere (gp_axis, radius);

   make_sphere.Build();

   ostringstream     stream_shape;
   TopoDS_Shape      geom_sphere = make_sphere.Face();
   BRepTools::Write (geom_sphere, stream_shape);
   brep = stream_shape.str();
}
// ====================================================== geom_dump_asso
void geom_dump_asso (Edge* edge)
{
   printf (" %s dump_edge :\n",
           "_______________________________________________________________");
   if (edge==NULL || NOT edge->isHere ())
      {
      printf ("*** deleted ***)\n");
      return;
      }

   bool db0 = db;
   db  = false;
   edge->printName(" = (");
   edge->getVertex (V_AMONT)-> printName (", ");
   edge->getVertex (V_AVAL) -> printName (")\n");

   KasPoint asso_point;
   for (int nro=0 ; nro<V_TWO ; nro++)
       {
       Vertex* vertex = edge->getVertex (nro);
       vertex->printName ("");
       printf (" = (%g, %g, %g)", vertex->getX(),  vertex->getY(),
                                  vertex->getZ());

       int ier = asso_point.definePoint (vertex);
       if (ier==HOK)
          {
          double* coord = asso_point.getCoord();
          printf (",  pnt_asso  = (%g, %g, %g)", coord[dir_x],  coord[dir_y],
                                     coord[dir_z]);
          }
       printf ("\n");
       }

   KasLine asso_line;
   const Shapes&  tshapes = edge->getAssociations ();
   for (int nro=0 ; nro<tshapes.size() ; nro++)
       {
       printf ( " ------------------- Edge.tab_shape[%d] :\n", nro);
       Shape* shape = tshapes[nro];
       if (shape == NULL)
          {
          printf ( "                =  NULL\n");
          }
       else
          {
          asso_line.defineLine (shape);
          double* deb = asso_line.getStart  ();
          double* fin = asso_line.getEnd    ();
          double  lg  = asso_line.getLength ();
          printf (" Longueur = %g\n", lg);
          printf (" Debut = %g = (%g, %g, %g)\n", shape->getStart(),
                                                  deb[0], deb[1], deb[2]);
          printf (" Fin   = %g = (%g, %g, %g)\n", shape->getEnd(),
                                                  fin[0], fin[1], fin[2]);
          }
       }
   db = db0;
}
// ====================================================== translate_brep
void translate_brep (string& brep, double dir[], string& trep)
{
   gp_Trsf       transfo;
   BRep_Builder  builder;
   TopoDS_Shape  orig;

   gp_Vec      vecteur       (dir [dir_x], dir [dir_y], dir [dir_z]);
   transfo.SetTranslation    (vecteur);
   istringstream stream_brep (brep);
   BRepTools::Read           (orig, stream_brep, builder);

   TopLoc_Location  loc_orig   = orig.Location();
   gp_Trsf          trans_orig = loc_orig.Transformation();
   TopLoc_Location  loc_result (transfo * trans_orig);
   TopoDS_Shape     result = orig.Located (loc_result);

   ostringstream stream_shape;
   BRepTools::Write (result, stream_shape);
   trep = stream_shape.str();
}
// ====================================================== transfo_brep
void transfo_brep (string& brep, Matrix* matrice, string& trep)
{
   BRep_Builder  builder;
   TopoDS_Shape  shape_orig;
   gp_Trsf       transfo;

   double             a11,a12,a13,a14, a21,a22,a23,a24, a31,a32,a33,a34;
   matrice->getCoeff (a11,a12,a13,a14, a21,a22,a23,a24, a31,a32,a33,a34);
   transfo.SetValues (a11,a12,a13,a14, a21,a22,a23,a24, a31,a32,a33,a34,
                      Epsil2, Epsil2);

   istringstream stream_brep (brep);
   BRepTools::Read           (shape_orig, stream_brep, builder);

   BRepBuilderAPI_Transform brep_transfo (shape_orig, transfo, Standard_True);
   TopoDS_Shape result = brep_transfo.Shape();

   ostringstream stream_shape;
   BRepTools::Write (result, stream_shape);
   trep = stream_shape.str();
}
// ====================================================== clear_associations
void clear_associations (Edge* edge)
{
   edge->clearAssociation();
   edge->getVertex(V_AMONT)->clearAssociation();
   edge->getVertex(V_AVAL )->clearAssociation();
}
// ====================================================== set_debug_asso
void set_debug_asso (bool boule)
{
   db = boule;
   if (db)
       printf (" ... Traces actives dans  HexAssoElements_asso.cxx\n");
}
// ====================================================== clean_brep
void clean_brep (string& brep)
{
   TopoDS_Shape  shape;
   BRep_Builder  builder;
   istringstream stream_brep (brep);

   BRepTools::Read  (shape, stream_brep, builder);
   BRepTools::Clean (shape);

   ostringstream    stream_shape;
   BRepTools::Write (shape, stream_shape);
   brep = stream_shape.str();
}
END_NAMESPACE_HEXA
