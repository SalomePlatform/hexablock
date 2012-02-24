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
#include "HexMatrix.hxx"

#ifndef NO_CASCADE

#include <AIS_Shape.hxx>

#include <Precision.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Transform.hxx>

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
#include <BRepPrimAPI_MakeSphere.hxx>
                                    // Cylindre
#include <GEOMImpl_CylinderDriver.hxx>
#include <GEOMImpl_ICylinder.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepAlgoAPI_Section.hxx>
                                    // Decoupage
#include <TopExp_Explorer.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>

#include <GeomAPI_ProjectPointOnCurve.hxx>


#ifdef WNT
#include <process.h>
#else
#include <unistd.h>
#endif

#include <stdexcept>

#define MESSAGE(m) cout << m << endl
#define Assert(m) if (m!=0) { cout<<" **** Assert "<<m<< endl; exit(101);}



BEGIN_NAMESPACE_HEXA

static bool db = false;

static const double Epsil   = 1e-6;
static const double UnEpsil = 0.999999;
static const double Epsil2  = 1e-12;

static const double TolAsso = 1e-2;    // Tolerance pour les associations

void clear_associations (Edge* edge);

// ---------------------------------------------------------------------
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
   string lig_brep;
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
// ---------------------------------------------------------------------
class KasPoint
{
public :
   KasPoint ();
 
   double* getCoord () { return g_coord;   }
   bool    isOk  ()    { return is_ok;     }
   bool    isBad ()    { return NOT is_ok; }

   void associate   (Vertex* node);
   void razPoint    ();
   int  definePoint (Vertex* node);
   void definePoint (double*   coord);
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
class KasCylinders
{
public :
   KasCylinders () { purge () ; }
  ~KasCylinders () { purge () ; }
 
   int defineCyls (double* borig,  double* bnorm, double* bbase,
                   double  brayon, double  bhaut,
                   double* sorig,  double* snorm, double* sbase,
                   double  srayon, double  shaut);
   int associate  (Edge* edge);

private :
   int  anaVertex (Vertex* node, int* tline, double* tpara);
   void purge ();

private :
   std::vector <KasLine*> inter_line;
   bool no_inter;
};
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------

static KasLine      current_line;
static Shape        current_shape ("");
static KasCylinders current_cyls;

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
// ======================================================= Constructeur
KasPoint::KasPoint ()
{
   razPoint ();
}
// ======================================================= razPoint
void KasPoint::razPoint ()
{
   is_ok = false;
   b_rep = "";
   for (int nc=0; nc <DIM3 ; nc++) g_coord [nc] = 0;
}
// ======================================================= definePoint (vertex)
int KasPoint::definePoint (Vertex* node)
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
   return HOK;
}
// ======================================================= definePoint (xyz)
void KasPoint::definePoint (Real3 coord)
{
   gp_Pnt gpoint (coord[dir_x], coord[dir_y], coord[dir_z]);
   definePoint   (gpoint);
}
// ======================================================= definePoint (g_pnt)
void KasPoint::definePoint (gp_Pnt& gpoint)
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

   return;
   if (db)
      {
      cout << " KasPoint::definePoint :" << endl;
      PutCoord (g_coord);
      }
}
// ======================================================= associate
void KasPoint::associate (Vertex* node)
{
   // if (db) cout << " ++ KasPoint::associate " << endl;
   // if (db) PutName (node);
   if (node==NULL)
      return;

   Shape* vshape = new Shape (b_rep);
   node->setAssociation (vshape);
}
// ---------------------------------------------------------------------
// ======================================================= Constructeur
KasLine::KasLine ()
{
   lig_brep     = "";
   lig_debut    = 0;
   lig_fin      = 1;
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
KasLine::KasLine (Shape* asso, double deb, double fin)
{
   geom_curve  = NULL;

   defineLine (asso, deb, fin);
}
// ========================================================= Destructeur
KasLine::~KasLine ()
{
   delete geom_curve;
}
// ========================================================= defineLine
// === Creation de la quincaillerie associee a une shape 
void KasLine::defineLine (Shape* asso, double deb, double fin)
{
   lig_brep = asso->getBrep ();
   if (fin<0.0)
      {
      lig_debut = asso->debut;
      lig_fin   = asso->fin;
      }
   else if (lig_debut >= UnEpsil)
      {
      lig_debut = fin;
      lig_fin   = deb ;
      }
   else 
      {
      lig_debut = deb;
      lig_fin   = fin;
      }

   geom_inverse = false;
   start_absc   = 0;
   end_absc     = 1;

   istringstream streamBrep (lig_brep);
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
   geom_length = geom_total_length * fabs (lig_fin-lig_debut);

                               // Extremites
   GCPnts_AbscissaPoint s1 (*geom_curve, geom_total_length*lig_debut, 
                             geom_curve->FirstParameter());
   GCPnts_AbscissaPoint s2 (*geom_curve, geom_total_length*lig_fin, 
                             geom_curve->FirstParameter());

   par_mini     = s1.Parameter ();
   par_maxi     = s2.Parameter ();
   start_gpoint = geom_curve->Value (par_mini);
   end_gpoint   = geom_curve->Value (par_maxi);
   majCoord ();

   if (db) 
      {
      Echo (" ____________________________________  KasLine::defineLine");
      HexDisplay (lig_debut);
      HexDisplay (lig_fin);
      HexDisplay (geom_total_length);
      HexDisplay (geom_curve->FirstParameter());
      HexDisplay (s1.Parameter());
      HexDisplay (s2.Parameter());

      PutCoord (start_coord);
      PutCoord (end_coord);
      }
}
// ========================================================= assoPoint
void KasLine::assoPoint (double alpha, Vertex* node)
{
   GCPnts_AbscissaPoint s1 (*geom_curve, alpha, 
                             geom_curve->FirstParameter());
   double u1       = s1.Parameter ();
   gp_Pnt pnt_asso = geom_curve->Value (u1);

   KasPoint gpoint;
   gpoint.definePoint (pnt_asso);
   gpoint.associate   (node);

   if (db)
      {
      double* coord = gpoint.getCoord();
      printf (" ... assoPoint : v=%s (%g,%g,%g), param=%g\n", 
           node->getName(), node->getX(), node->getY(), node->getZ(), alpha);
 
      printf ("                   ----> (%g,%g,%g)\n", coord[dir_x], 
                                         coord[dir_y], coord[dir_z]);
      }
}
// ========================================================= arrondir
void arrondir (double &val)
{
   if (val >= -TolAsso && val <= TolAsso)
      {
      val = 0.0;
      }
   else if (val >= 1.0-TolAsso && val <= 1.0+TolAsso)
      {
      val = 1.0;
      }
}
// ========================================================= associate
void KasLine::associate (Edge* edge, double sm1, double sm2, int vorig)
{
   if (sm2 < start_absc) return;
   if (sm1 > end_absc)   return;

   Vertex* segment[V_TWO] = { edge->getVertex (vorig), 
                              edge->getVertex (1-vorig) };

   double vpara1 = lig_debut + (sm1-start_absc)/geom_total_length;
   double vpara2 = lig_debut + (sm2-start_absc)/geom_total_length;
   if (geom_inverse)
      {
      vpara2 = lig_fin - (sm1-start_absc)/geom_total_length;
      vpara1 = lig_fin - (sm2-start_absc)/geom_total_length;
      }

   double lpara1 = std::max (lig_debut, std::min (lig_fin, vpara1));
   double lpara2 = std::max (lig_debut, std::min (lig_fin, vpara2));

   arrondir (vpara1);    arrondir (vpara2);
   arrondir (lpara1);    arrondir (lpara2);

   if (db)
      {
      cout << " ++ KasLine::associate : rg=" << geom_rang  << "s=" << vorig
           << endl;
      cout << " ligpara = [ " << lig_debut << ", " << lig_fin << " ]" << endl;
      cout << " absc    = [ " << start_absc << ", " << end_absc << " ]\n" ;

      cout << " Edge    = [ " << segment[0]->getName() << ", " 
           << ","             << segment[1]->getName() << endl;
      cout << " smx     = [ " << sm1    << ", " << sm2    << " ]" << endl;
      cout << " vparam  = [ " << vpara1 << ", " << vpara2 << " ]" << endl;
      cout << " lparam  = [ " << lpara1 << ", " << lpara2 << " ]" << endl;
      }

   if (lpara2 >= lpara1 + TolAsso)
      {
      assoEdge (edge, lpara1, lpara2);
      }
   else if (db)
      {
      if (db) printf (" Asso Line refusee %s -> (%g,%g)\n", 
              edge->getName(), lpara1, lpara2);
      }

                               // ---------------Association du vertex 
   double param = geom_inverse ? vpara2 : vpara1;
   double smx   = sm1;
   double absc1 = start_absc - TolAsso*geom_total_length;
   double absc2 = end_absc   + TolAsso*geom_total_length;

   for (int nx=V_AMONT ; nx<=V_AVAL ; nx++)
       {
       if (smx >= absc1 && smx <= absc2)
          {
          Vertex* node = segment [nx];
          if (node->getAssociation()==NULL)
             {
                                          // .....  Coordonnees du point

             double alpha = geom_total_length*param;
             GCPnts_AbscissaPoint s1 (*geom_curve, alpha, 
                                       geom_curve->FirstParameter());
             double u1       = s1.Parameter ();
             gp_Pnt pnt_asso = geom_curve->Value (u1);

                                          // .....  Creation d'un vertex Geom
             KasPoint gpoint;
             gpoint.definePoint (pnt_asso);
             gpoint.associate (node);
             if (db)
                {
                double* ass = gpoint.getCoord();
                printf (" Asso Point %s (%g,%g,%g) -> (%g,%g,%g) p=%g s=%g\n", 
                    node->getName(), node->getX(), node->getY(), node->getZ(),
                    ass[dir_x], ass[dir_y], ass[dir_z], param, smx);
                }
             }
         else if (db)
             {
             double alpha = geom_total_length*param;
             GCPnts_AbscissaPoint s1 (*geom_curve, alpha, 
                                    geom_curve->FirstParameter());
             double u1       = s1.Parameter ();
             gp_Pnt pnt_asso = geom_curve->Value (u1);

                                          // .....  Creation d'un vertex Geom
             KasPoint gpoint;
             gpoint.definePoint (pnt_asso);
             double* ass = gpoint.getCoord();
             // gpoint.associate (node);
             printf (" Asso Point %s (%g,%g,%g) -> (%g,%g,%g) p=%g s=%g\n", 
                     node->getName(),
                     node->getX(), node->getY(), node->getZ(),
                     ass[dir_x], ass[dir_y], ass[dir_z], param, smx);

             gpoint.definePoint (node);
             ass = gpoint.getCoord();
             printf ("               ignore car deja associe a (%g,%g, %g)\n", 
                     ass[dir_x], ass[dir_y], ass[dir_z]);
             }
          }
      param = geom_inverse ? vpara1 : vpara2;
      smx   = sm2;
      }
}
// ========================================================= assoEdge
void KasLine::assoEdge (Edge* edge, double para1, double para2, int vass)
{
   Shape* shape = new Shape (lig_brep);
   shape->setBounds (para1, para2);
   edge ->addAssociation (shape);
   if (db) printf (" ... Asso Edge %s -> (%g,%g)\n", 
           edge->getName(), para1, para2);

   double lg = geom_total_length;
   switch (vass) 
      {
      case V_AMONT :
           assoPoint (para1*lg, edge->getVertex (V_AMONT));
           break;
      case V_AVAL  :
           assoPoint (para2*lg, edge->getVertex (V_AVAL));
           break;
      case V_TWO   :
           assoPoint (para1*lg, edge->getVertex (V_AMONT));
           assoPoint (para2*lg, edge->getVertex (V_AVAL ));
           break;
      default :;
      }
}
// ========================================================= majCoord
void KasLine::majCoord ()
{
   start_coord [dir_x] = start_gpoint.X();
   start_coord [dir_y] = start_gpoint.Y();
   start_coord [dir_z] = start_gpoint.Z();

   end_coord   [dir_x] = end_gpoint.X();
   end_coord   [dir_y] = end_gpoint.Y();
   end_coord   [dir_z] = end_gpoint.Z();
}
// ========================================================= setBounds
void KasLine::setBounds (double deb, double fin)
{
   lig_debut = deb;
   lig_fin   = fin;
}
// ========================================================= inverser
void KasLine::inverser ()
{
   gp_Pnt  foo  = start_gpoint;
   start_gpoint = end_gpoint;
   end_gpoint   = foo;

              // Prendre le complementaire de l'intercvale initial
   if (lig_debut > Epsil && lig_debut < UnEpsil)
      {
      lig_fin  = lig_debut;
      lig_debut = 0;
      cout << " ... inverser : fin = debut = " << lig_fin << endl;
      }
   else if (lig_fin > Epsil && lig_fin < UnEpsil)
      {
      lig_debut = lig_fin;
      lig_fin   = 1;
      cout << " ... inverser : debut = fin = " << lig_debut  << endl;
      }

   geom_inverse = NOT geom_inverse;
   majCoord ();
}
// ========================================================= setRank
void KasLine::setRank (int nro, int sens, double& abscisse)
{
   if (sens==V_AVAL)
      inverser ();

   geom_rang  = nro;
   start_absc = abscisse;
   abscisse   = end_absc = start_absc + (lig_fin-lig_debut) * geom_total_length;

   if (db)
      {
      cout << "KasLine::setRank : nro = " << nro << " sens="  << sens 
           <<  " = (" << start_absc << ", " << end_absc << ")" << endl;
      }
}
// ====================================================== carre
inline double carre (double val)
{
   return val*val;
}
//
// ====================================================== same_coords
inline bool same_coords (gp_Pnt& pa, gp_Pnt& pb)
{
   double d2 = carre (pb.X()-pa.X()) + carre (pb.Y()-pa.Y()) 
                                     + carre (pb.Z()-pa.Z()) ;
   return d2 < Epsil2;
}
// ====================================================== same_coords
inline bool same_coords (double* pa, double* pb)
{

   double d2 = carre (pb[dir_x]-pa[dir_x]) + carre (pb[dir_y]-pa[dir_y]) 
             + carre (pb[dir_z]-pa[dir_z]); 
   return d2 < Epsil2;
}
// ========================================================= findBound
int KasLine::findBound (double* coord)
{
   if (same_coords (coord, start_coord) )
      return V_AMONT;

   if (same_coords (coord, end_coord) )
      return V_AVAL;

   return NOTHING;
}
// ========================================================= findParam
double KasLine::findParam (double* coord)
{
   double umin = 0, umax = 0;
   gp_Pnt gpoint (coord[dir_x], coord[dir_y], coord[dir_z]);
   Handle(Geom_Curve) curve = BRep_Tool::Curve (geom_line, umin, umax);

   GeomAPI_ProjectPointOnCurve projector (gpoint, curve);
   if ( projector.NbPoints() == 0 )
      return -1.0;

   double param = projector.LowerDistanceParameter();
   if (param <par_mini || param>par_maxi)
      {
      // cout << " Rejet : " << param << " not in (" << par_mini 
      //                              << ", " << par_maxi << ")" << endl;
      return -1.0;
      }
 
   gp_Pnt rpoint = geom_curve->Value (param);
   if (NOT same_coords (gpoint, rpoint))
      {
      //  cout << " Rejet : points differents " << endl;
      return -1.0;
      }

   param = (param-par_mini) / (par_maxi-par_mini); 
   return param;
}
// ========================================================= cutAssociation
void Elements::cutAssociation (Shapes& tshapes, Edges& tedges, bool exist)
{
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
// ====================================================== geom_make_brep 
void geom_make_brep (TopoDS_Shape& shape, string& brep)
{
   ostringstream  stream_shape;
   BRepTools::Write (shape, stream_shape);
   brep = stream_shape.str();
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
// ====================================================== save_brep
void save_brep (cpchar nom, string brep)
{
    static int compteur = 0;
    char buff[8];

    compteur ++;
    sprintf (buff ,"%d", compteur);
    string name (nom);
    name += buff;
    name += ".brep";
    FILE*    fic = fopen (name.c_str(), "w");
    fprintf (fic, "%s\n", brep.c_str());
    fclose  (fic);
}
// ====================================================== purge
void KasCylinders::purge ()
{
    no_inter = true;
    inter_line.clear ();
}
// ====================================================== defineCyls 
int KasCylinders::defineCyls (double* borig,  double* bnorm, double* bbase,
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
   while (explo.More())
         {
         TopoDS_Shape ligne = explo.Current();
         // TopoDS_Edge ligne = explo.Current();
         explo.Next ();

         cout << "____________________________________ Ligne suivante" << endl;
         geom_make_brep (ligne, crep);
         save_brep ("inter", crep);
         shape.setBrep (crep);

         KasLine* geom_line = new KasLine (&shape);
         inter_line.push_back (geom_line);
         nroline ++;
         }
   return HOK;
}
// ====================================================== anaVertex
// === Trouve le(s) ligne(s) contenant ce vertex et le(s) parametre(s)
int KasCylinders::anaVertex (Vertex* node, int* tline, double* tpara)
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
int KasCylinders::associate (Edge* edge)
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
}
// ====================================================== geom_create_cylcyl
int geom_create_cylcyl (double* borig, double* bnorm, double* bbase,
                        double  bray,  double  bhaut,
                        double* sorig, double* snorm, double* sbase,
                        double  sray,  double  shaut)
{
   int ier = current_cyls.defineCyls (borig, bnorm, bbase, bray,  bhaut,
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

   int ier = current_cyls.associate (edge);
   return ier;
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
   printf (" _____________________ dump_edge :\n");
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
          printf (" Debut = %g = (%g, %g, %g)\n", shape->debut, 
                                                  deb[0], deb[1], deb[2]);
          printf (" Fin   = %g = (%g, %g, %g)\n", shape->fin, 
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
// ====================================================== associateShapes
int associateShapes (Edges& mline, int msens[], Shape* gstart, Shapes& gline, 
                    double pstart, double pend, bool closed, bool inv)
{
   cout << "____________________________________________"
        << " associateShapes" << endl;

   int nbshapes = gline.size ();
   int nblines  = nbshapes + 1; 

   vector <KasLine*> buff_line (nblines); // car nblines != 0 
   vector <KasLine*> geom_line (nblines);

                                    // -------- Bufferisation des shapes
   for (int ns=0 ; ns<nbshapes ; ns++)
       buff_line [ns] = new KasLine (gline [ns], 0.0, 1.0);

                                    // -------- Premiere ligne
   int    sdepart  = NOTHING;
   double*  extrem    = NULL;
   double abscisse = 0;
   double pfin0    = 1;
   if (closed)
      {
      if (pstart > UnEpsil)
         { 
         inv    = true;
         pstart = 0;
         }
      else if (pstart < Epsil)
         { 
         inv    = false;
         }
      }
   else if (nbshapes==1)
      pfin0 = pend;

   KasLine* prems = geom_line[0]  = new KasLine (gstart, pstart, pfin0);

                          // Ligne fermee : sens impose par l'utilisateur
   if (closed)
      {
      sdepart = inv ? V_AVAL : V_AMONT;
      }
                          // PolyLigne ouverte : trouver le sens
   else if (nbshapes>0)
      {
      sdepart  = NOTHING;
      extrem = prems -> getEnd();
      for (int ns=V_AMONT ; ns<=V_AVAL && sdepart==NOTHING ; ns++)
          {
          for (int nb=0 ; nb<nbshapes && sdepart==NOTHING ; nb++)
              {
              if (buff_line[nb]->findBound(extrem) != NOTHING)
                 sdepart = ns;
              }
          extrem = prems -> getStart();
          }

      if (sdepart==NOTHING)
         {
         printf (" ***************** Erreur dans KasLine\n");
         printf (" ***************** La ligne ouverte est interrompue\n");
         return HERR;
         }
      }
                          // Une seule ligne ouverte : depend de pstart et pend
   else if (pstart < pend)
      {
      sdepart = V_AMONT;
      pfin0   = pend;
      }
   else 
      {
      sdepart = V_AVAL;
      pfin0   = pend;
      }

   prems -> setRank (0, sdepart, abscisse);
   extrem = prems -> getEnd();
                                    // -------- Rangement des autres lignes
   for (int nl=1 ; nl<nblines ; nl++)
       {
       bool more = true;
       for (int nb=0 ; more && nb<nbshapes ; nb++)
           {
           KasLine*  ligne = buff_line[nb];
           int sens = ligne==NULL ? NOTHING : ligne->findBound (extrem);
           if (sens != NOTHING)
              {
                                 // Derniere ligne si ouverte
              if (NOT closed && nl==nblines-1)
                 {
                 ligne -> setBounds (0, pend);
                 }
              ligne -> setRank (nl, sens, abscisse);
              buff_line [nb] = NULL;
              geom_line [nl] = ligne;
              extrem = ligne->getEnd();
              more   = false;
              }
           }
       if (more)
          {
          printf (" ***************** Erreur dans KasLine\n");
          return HERR;
          }
       }
   if (closed && pstart > Epsil)
      {
      KasLine* ligne = new KasLine (gstart, 0, pstart);
      ligne->setRank   (nblines, sdepart, abscisse);
      geom_line.push_back (ligne);
      }

   int ntlines = geom_line.size();
   int nbedges = mline.size ();

   if (db) 
      cout << "=============================================================="
           << endl;
                                    // -------- Menage
   for (int ned=0 ; ned<nbedges ; ned++)
       clear_associations (mline[ned]);
                                    // -------- Associations
   double delta = abscisse / nbedges;
   for (int ned=0 ; ned<nbedges ; ned++)
       {
       double sm1 = ned * delta;
       double sm2 = sm1 + delta;
       for (int ns = 0 ; ns<ntlines ; ns++)
           geom_line[ns]->associate (mline[ned], sm1, sm2, msens[ned]);
       }
                                    // -------- Liberation
   for (int nl=1 ; nl<ntlines ; nl++)
       delete geom_line [nl];

   return HOK;
}
// ====================================================== set_debug_asso
void set_debug_asso (bool boule)
{
   db = boule;
}
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
// ====================================================== geom_dump_asso
void geom_dump_asso (Edge* edge)
{
}
// ====================================================== geom_asso_point
void geom_asso_point (double angle, Vertex* node)
{
}
// ====================================================== geom_asso_point
void geom_asso_point (Vertex* node)
{
}
// ====================================================== translate_brep
void translate_brep (string& orig, double dir[], string& result)
{
   result = orig;
}
// ====================================================== transfo_brep
void transfo_brep (string& brep, Matrix* matrice, string& trep)
{
   trep = brep;
}
// ====================================================== dump_edge
void dump_edge (Edge* edge)
{
}
// ====================================================== associateShapes
int associateShapes (Edges& mline, int msens[], Shape* gstart, Shapes& gline,
                     double pstart, double pend, bool closed, bool inv)
{
   return HOK;
}
// ====================================================== set_debug_asso
void set_debug_asso (bool boule)
{
}
// ====================================================== geom_create_cylcyl
int geom_create_cylcyl (double* borig* double* bnorm, double* bbase,
                         double  bray,  double  bhaut,
                         double* sorig, double* snorm, double* sbase,
                         double  sray,  double  shaut, string& brep)
{
}
END_NAMESPACE_HEXA
#endif
