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
#include "HexElements.hxx"

#ifndef NO_CASCADE
// CasCade includes
#include <AIS_Shape.hxx>

#include <Precision.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <TopoDS_Wire.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <IntCurvesFace_ShapeIntersector.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>

// HEXABLOCK includes
#include "HexDocument.hxx"
#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexQuad.hxx"
#include "HexHexa.hxx"
#include "HexPropagation.hxx"
#include "HexShape.hxx"
#include "HexDiagnostics.hxx"

                                    // Cercles 
#include <GEOMImpl_CircleDriver.hxx>
#include <GEOMImpl_ICircle.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Tool.hxx>

#include <GC_MakeCircle.hxx>
#include <Geom_Circle.hxx>

#include <Standard_ConstructionError.hxx>
#include <gp_Circ.hxx>

                                    // Sphere
//  #include <gp_Ax2.hxx>
//  #include <gp_Pnt.hxx>
//  #include <gp_Dir.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
// #include <TopoDS_Shape.hxx>

#ifdef WNT
#include <process.h>
#else
#include <unistd.h>
#endif

#include <stdexcept>

#ifndef EXCEPTION
#define EXCEPTION(TYPE, MSG) {\
  std::ostringstream aStream;\
  aStream<<__FILE__<<"["<<__LINE__<<"]::"<<MSG;\
  throw TYPE(aStream.str());\
}
#endif

#ifdef _DEBUG_
// static int MYDEBUG = 1;
#else
// static int MYDEBUG = 0;
#endif
#define MESSAGE(m) cout << m << endl
#define Assert(m) if (m!=0) { cout<<" **** Assert "<<m<< endl; exit(101);}

static double HEXA_EPSILON  = 1E-6; //1E-3; 
#define PutCoord(x) cout << " ... " #x " = (" << x[0] << ", " \
                         << x[1] << ", "   << x[2] << ") "   << endl


BEGIN_NAMESPACE_HEXA

static bool db = false;
// ---------------------------------------------------------------------
class GeomLine
{
public :
   GeomLine ();
   GeomLine (Shape* shape);
  ~GeomLine ();
 
   int  findBound (Real3 point);
   void setAbscissa (double total, double& abscisse);
   void inverser ();

   double* getStart  ()       { return start_coord ; }
   double* getEnd    ()       { return end_coord   ; }
   double  getLength ()       { return geom_length ; }
   double  getRank   ()       { return geom_rang ; }

   void defineLine (Shape* asso);
   void setRank   (int rang, int sens, double& abscisse);
   void associate (Edge* edge, double sm1, double sm2);
   void assoPoint (double alpha, Vertex* node);

private :
   void majCoord ();

private :
   Shape* bloc_shape; 
   bool   geom_inverse; 
   int    geom_rang;
   Real3  start_coord,  end_coord;
   double start_absc, end_absc;
   double geom_length;
   double geom_total_length;

   gp_Pnt             start_gpoint, end_gpoint;
   TopoDS_Edge        geom_line;
   BRepAdaptor_Curve* geom_curve;
};
// ---------------------------------------------------------------------
class GeomPoint
{
public :
   GeomPoint ();
 
   double* getCoord () { return g_coord;   }
   bool    isOk  ()    { return is_ok;     }
   bool    isBad ()    { return NOT is_ok; }

   void associate   (Vertex* node);
   void razPoint    ();
   int  definePoint (Vertex* node);
   void definePoint (Real3   coord);
   void definePoint (gp_Pnt& gpoint);

   TopoDS_Vertex& getVertex ()              { return g_vertex;  }

private :
   string b_rep; 
   Real3  g_coord;
   bool   is_ok;

   TopoDS_Vertex g_vertex;
   gp_Pnt        g_point;
};
// ---------------------------------------------------------------------
// ======================================================= Constructeur
GeomPoint::GeomPoint ()
{
   razPoint ();
}
// ======================================================= razPoint
void GeomPoint::razPoint ()
{
   is_ok = false;
   b_rep = "";
   for (int nc=0; nc <DIM3 ; nc++) g_coord [nc] = 0;
}
// ======================================================= definePoint (vertex)
int GeomPoint::definePoint (Vertex* node)
{
   razPoint ();

   if (node==NULL)
      return HERR;

   Shape* asso = node->getAssociation();
   if (asso==NULL)
      return HERR;
 
   b_rep = asso->getBrep ();

   TopoDS_Shape topo;
   istringstream streamBrep(b_rep);
   BRep_Builder aBuilder;
   BRepTools::Read(topo, streamBrep, aBuilder);

   if (topo.ShapeType() != TopAbs_VERTEX)
      return HERR;

   is_ok    = true;
   g_vertex = TopoDS::Vertex (topo);
   g_point  = BRep_Tool::Pnt( g_vertex );

   g_coord [dir_x] = g_point.X(); 
   g_coord [dir_y] = g_point.Y(); 
   g_coord [dir_z] = g_point.Z(); 
   if (db) PutCoord (g_coord);

   return HOK;
}
// ======================================================= definePoint (xyz)
void GeomPoint::definePoint (Real3 coord)
{
   gp_Pnt gpoint (coord[dir_x], coord[dir_y], coord[dir_z]);
   definePoint   (gpoint);
}
// ======================================================= definePoint (g_pnt)
void GeomPoint::definePoint (gp_Pnt& gpoint)
{
   is_ok   = true;
   g_point = gpoint;

   ostringstream   stream_shape;

   BRepBuilderAPI_MakeVertex mkVertex (g_point);
   TopoDS_Shape              aShape = mkVertex.Shape();
   BRepTools::Write(aShape, stream_shape);

   g_vertex = TopoDS::Vertex( aShape );
   b_rep    = stream_shape.str();

   g_coord [dir_x] = g_point.X();
   g_coord [dir_y] = g_point.Y();
   g_coord [dir_z] = g_point.Z();

   if (db) PutCoord (g_coord);
}
// ======================================================= associate
void GeomPoint::associate (Vertex* node)
{
   if (db) cout << " ++ GeomPoint::associate " << endl;
   if (db) PutName (node);
   if (node==NULL)
      return;

   Shape* vshape = new Shape (b_rep); 
   node->setAssociation (vshape);
}
// ---------------------------------------------------------------------
// ======================================================= Constructeur
GeomLine::GeomLine ()
{
   bloc_shape   = NULL;
   geom_inverse = false;
   start_absc   = 0;
   end_absc     = 1;
   geom_rang    = NOTHING;

   // geom_line   = TopoDS::Edge(shape);
   geom_curve  = NULL;
   geom_length = 0;
   geom_total_length = 0;
   for (int nc=0; nc <DIM3 ; nc++) start_coord [nc] = end_coord [nc] = 0;
}
// ========================================================= Constructeur bis
GeomLine::GeomLine (Shape* asso)
{
   defineLine (asso);
}
// ========================================================= Destructeur
GeomLine::~GeomLine ()
{
   delete geom_curve;
}
// ========================================================= defineLine
// === Creation de la quincaillerie associee a une shape 
void GeomLine::defineLine (Shape* asso)
{
   bloc_shape   = asso;
   geom_inverse = false;
   start_absc   = 0;
   end_absc     = 1;

   istringstream streamBrep (bloc_shape->getBrep());
   BRep_Builder  aBuilder;
   TopoDS_Shape  topo;

   BRepTools::Read(topo, streamBrep, aBuilder);
   geom_line  = TopoDS::Edge(topo);
   delete geom_curve;
   geom_curve = new BRepAdaptor_Curve (geom_line);
                               // Longueur de la ligne
   double umin = 0, umax = 0;
   TopLoc_Location    loc;
   Handle(Geom_Curve) handle = BRep_Tool::Curve (geom_line, loc, umin, umax);
   GeomAdaptor_Curve  AdaptCurve (handle);
   geom_total_length = GCPnts_AbscissaPoint::Length(AdaptCurve, umin, umax);
   geom_length = geom_total_length * fabs (asso->fin-asso->debut);

                               // Extremites
   GCPnts_AbscissaPoint s1 (*geom_curve, geom_total_length*asso->debut, 
                             geom_curve->FirstParameter());
   GCPnts_AbscissaPoint s2 (*geom_curve, geom_total_length*asso->fin, 
                             geom_curve->FirstParameter());

   double u1    = s1.Parameter ();
   double u2    = s2.Parameter ();
   start_gpoint =  geom_curve->Value (u1);
   end_gpoint   =  geom_curve->Value (u2);
   majCoord ();

   if (db) 
      {
      HexDisplay (asso->debut);
      HexDisplay (asso->fin);
      HexDisplay (geom_total_length);
      HexDisplay (geom_curve->FirstParameter());
      HexDisplay (s1.Parameter());
      HexDisplay (s2.Parameter());

      PutCoord (start_coord);
      PutCoord (end_coord);
      }
}
// ========================================================= assoPoint
void GeomLine::assoPoint (double alpha, Vertex* node)
{
   GCPnts_AbscissaPoint s1 (*geom_curve, alpha, 
                             geom_curve->FirstParameter());
   double u1       = s1.Parameter ();
   gp_Pnt pnt_asso = geom_curve->Value (u1);

   GeomPoint gpoint;
   gpoint.definePoint (pnt_asso);
   gpoint.associate   (node);

   double* coord = gpoint.getCoord();
   printf (" ... assoPoint : angle=%g, v=%s (%g,%g,%g) -> (%g, %g, %g)\n", 
           alpha, node->getName(), node->getX(), node->getY(), node->getZ(), 
           coord[dir_x], coord[dir_y], coord[dir_z]);
}
// ========================================================= associate
void GeomLine::associate (Edge* edge, double sm1, double sm2)
{
   if (db) cout << " ++ GeomLine::associate " << sm1 << ", " << sm2 << endl;
   if (db) PutName (edge);
   if (sm2 < start_absc) return;
   if (sm1 > end_absc)   return;

   double dg    = (end_absc-start_absc);
   double para1 = start_absc < sm1 ? (sm1-start_absc)/dg : 0.0;
   double para2 = end_absc   > sm2 ? (sm2-start_absc)/dg : 1.0;

   if (geom_inverse)
      {
      double para11 = para1;
      para1 = 1 - para2;
      para2 = 1 - para11;
      }

   Shape* shape = new Shape (bloc_shape->getBrep());
   double orig = bloc_shape->debut;
   double dp   = bloc_shape->fin - orig;
   shape->setBounds (orig + dp*para1, orig + dp*para2);
   edge ->addAssociation (shape);

                               // ---------------Association du vertex 
   if (sm2 < start_absc || sm2 > end_absc+HEXA_EPSILON)
      return;

   double alpha = geom_total_length*shape->debut;
   for (int nx=V_AMONT ; nx<=V_AVAL ; nx++)
       {
       Vertex* node = edge->getVertex (nx);
       if (node->getAssociation()==NULL)
          {
                                          // .....  Coordonnees du point

          GCPnts_AbscissaPoint s1 (*geom_curve, alpha, 
                                    geom_curve->FirstParameter());
          double u1       = s1.Parameter ();
          gp_Pnt pnt_asso = geom_curve->Value (u1);

                                          // .....  Creation d'un vertex Geom
          GeomPoint gpoint;
          gpoint.definePoint (pnt_asso);
          gpoint.associate (node);
          Real* ass = gpoint.getCoord();
          if (db) printf (" Asso point %s (%g,%g,%g) -> (%g,%g, %g)\n", 
                    node->getName(), node->getX(), node->getY(), node->getZ(),
                    ass[dir_x], ass[dir_y], ass[dir_z]);
          }
      else if (db)
          {
          GCPnts_AbscissaPoint s1 (*geom_curve, alpha, 
                                    geom_curve->FirstParameter());
          double u1       = s1.Parameter ();
          gp_Pnt pnt_asso = geom_curve->Value (u1);

                                          // .....  Creation d'un vertex Geom
          GeomPoint gpoint;
          gpoint.definePoint (pnt_asso);
          Real* ass = gpoint.getCoord();
          // gpoint.associate (node);
          printf (" Asso point %s (%g,%g,%g) -> (%g,%g, %g)\n", node->getName(),
                  node->getX(), node->getY(), node->getZ(),
                  ass[dir_x], ass[dir_y], ass[dir_z]);

          gpoint.definePoint (node);
          ass = gpoint.getCoord();
          printf ("   deja associe a (%g,%g, %g)\n", 
                  ass[dir_x], ass[dir_y], ass[dir_z]);
          HexDisplay (sm1);
          HexDisplay (sm2);
          HexDisplay (bloc_shape->debut);
          HexDisplay (bloc_shape->fin);
          HexDisplay (shape->debut);
          HexDisplay (shape->fin);
          }
      alpha = geom_total_length*shape->fin; // Pour le point suivant
      }
}
// ========================================================= majCoord
void GeomLine::majCoord ()
{
   start_coord [dir_x] = start_gpoint.X();
   start_coord [dir_y] = start_gpoint.Y();
   start_coord [dir_z] = start_gpoint.Z();

   end_coord   [dir_x] = end_gpoint.X();
   end_coord   [dir_y] = end_gpoint.Y();
   end_coord   [dir_z] = end_gpoint.Z();
}
// ========================================================= inverser
void GeomLine::inverser ()
{
   gp_Pnt  foo  = start_gpoint;
   start_gpoint = end_gpoint;
   end_gpoint   = foo;
   geom_inverse = NOT geom_inverse;
   majCoord ();
}
// ========================================================= setRank
void GeomLine::setRank (int nro, int sens, double& abscisse)
{
   geom_rang  = nro;
   start_absc = abscisse;
   abscisse   = end_absc = start_absc + geom_length;
   if (sens==V_AVAL) inverser ();
}
// ====================================================== carre
inline double carre (double val)
{
   return val*val;
}
//
// ====================================================== same_coords
inline bool same_coords (double* pa, double* pb)
{
   const double Epsil2 = 1e-10;

   double d2 = carre (pb[dir_x]-pa[dir_x]) + carre (pb[dir_y]-pa[dir_y]) 
             + carre (pb[dir_z]-pa[dir_z]); 
/********************************************
   printf ( " ... same_coords :d2 ((%g,%g,%g), (%g,%g,%g)) = %g\n", 
           pa[dir_x], pa[dir_y], pa[dir_z], pb[dir_x],
           pb[dir_y], pb[dir_z],  d2);
**************************************/
   return d2 < Epsil2;
}
// ========================================================= findBound
int GeomLine::findBound (Real3 coord)
{
   if (same_coords (coord, start_coord) )
      return V_AMONT;

   if (same_coords (coord, end_coord) )
      return V_AVAL;

   return NOTHING;
}
// ========================================================= cutAssociation
void Elements::cutAssociation (Shapes& tshapes, Edges& tedges, bool exist)
{
   char foo[18];
   int nbedges  = tedges.size();
   int nbshapes = tshapes.size ();
   if (nbshapes==0) 
      return;

   std::vector <GeomLine>  tab_gline  (nbshapes);
   
   Vertex* prems = tedges [0]         -> getVertex (V_AMONT);
   Vertex* derns = tedges [nbedges-1] -> getVertex (V_AVAL);

   GeomPoint pnt_first, pnt_last;
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
   abscisse     = 0;
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

       abscisse += delta;
       }
   if (db) cout << " +++ End of Elements::cutAssociation" << endl;
}
static GeomLine current_line;
static Shape    current_shape ("");
// ====================================================== geom_define_line
void geom_define_line (string& brep)
{
   current_shape.setBrep (brep);
   current_shape.setBounds (0,1);
   current_line.defineLine (&current_shape);
}
// ====================================================== geom_asso_point
void geom_asso_point (double angle, Vertex* node)
{
   current_line.assoPoint (angle, node);
}
// ====================================================== geom_create_circle 
void geom_create_circle (double* milieu, double rayon, double* normale, 
                         double* base, string& brep)
{
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
//
END_NAMESPACE_HEXA
      
// ------------------------------------------------------------------------
#else    // #ifndef NO_CASCADE
// ------------------------------------------------------------------------

BEGIN_NAMESPACE_HEXA

// ====================================================== geom_create_circle 
void geom_create_circle (double* milieu, double rayon, double* normale, 
                         double* base, string& brep)
{
   char buffer [80];
   sprintf (buffer, "(Cercle c=(%g,%g,%g), r=%g", 
                    milieu[0], milieu[1], milieu[2], rayon);
   brep = buffer;
}
// ====================================================== geom_create_sphere 
void geom_create_sphere (double* milieu, double radius, string& brep)
{
}
//
// ========================================================= cutAssociation
void Elements::cutAssociation (Shapes& tshapes, Edges& tedges, bool exist)
{
}
// ====================================================== geom_define_line
void geom_define_line (string& brep)
{
}
// ========================================================= geom_asso_point
void geom_asso_point (double angle, Vertex* node)
{
}
END_NAMESPACE_HEXA
#endif
