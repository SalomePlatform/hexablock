//
// CC++ : interface modele/cascade
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
#include "HexDocument.hxx"

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
#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexQuad.hxx"
#include "HexHexa.hxx"
#include "HexPropagation.hxx"
#include "HexShape.hxx"
#include "HexDiagnostics.hxx"

#include <list>
#include <map>

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
//  static double HEXA_QUAD_WAY = PI/4.; //3.*PI/8.;
    double fin;


BEGIN_NAMESPACE_HEXA

static bool db = false;
static int nro_xmgr = 0;

typedef vector<double> Dtable;
double edge_length (const TopoDS_Edge& edge );

// ========================================================= incrementer_xmgr
inline void incrementer_xmgr (int nro=-1)
{
   if (nro>=0) 
      nro_xmgr = 0;
   else
      nro_xmgr ++;
}
// ============================================================== save_xmgr
inline void save_xmgr (Dtable& tx, Dtable& ty, Dtable& tz, cpchar radical)
{
   char cnro [16];
   sprintf (cnro, "_%02d.xmgr", nro_xmgr);

   std::string nom = radical;
   nom += cnro;

   FILE* fic = fopen (nom.c_str (), "w");
   int nbre = tx.size();
   for (int np=0 ; np<nbre ; np++)
       fprintf (fic, "%g %g %g\n", tx[np],  ty[np],  tz[np]);

   fclose (fic);
}

// ============================================================== distance
inline double distance (double vx, double vy, double vz)
{
   double dist = sqrt (vx*vx + vy*vy + vz*vz);
   return dist;
}

// ============================================================== string2shape
TopoDS_Shape string2shape (const string& brep)
{
   TopoDS_Shape shape;
   istringstream streamBrep(brep);
   BRep_Builder aBuilder;
   BRepTools::Read(shape, streamBrep, aBuilder);

   return shape;
}
// ============================================================== asso2edge
// === Conversion d'une forme associee en une ligne de Geom
TopoDS_Edge asso2edge (Shape* asso)
{
   string brep = asso->getBrep();

   istringstream streamBrep (brep);
   BRep_Builder  aBuilder;
   TopoDS_Shape  shape;

   BRepTools::Read(shape, streamBrep, aBuilder);
   return TopoDS::Edge(shape);
}
// ============================================================ asso2vertex
void asso2vertex (Vertex* node, gp_Pnt& pnt_asso)
{
   BRepBuilderAPI_MakeVertex mkVertex (pnt_asso);
   ostringstream             streamShape;

   TopoDS_Shape  aShape = mkVertex.Shape();
   BRepTools::Write(aShape, streamShape);

   string        brep   = streamShape.str();
   Hex::Shape*   hshape = new Shape (brep); 

   node->setAssociation (hshape);
   if (NOT db) return;

   char cc1[12];

   printf (" asso2vertex %s -> ", node->getName(cc1));
   printf (" =(%g,%g,%g)\n", pnt_asso.X(), pnt_asso.Y(), pnt_asso.Z());
}

// ============================================================ asso2edge
void asso2edge (Edge* edge, Shape* assold, double deb, double fin)
{
   string brep   = assold->getBrep();
   Shape* assnew = new Shape (brep);

   assnew->debut = std::min (deb, fin);
   assnew->fin   = std::max (deb, fin);
   assnew->ident = assold->ident;
   edge->addAssociation (assnew);

   if (NOT db) return;

   char cc1[12], cc2 [12];
   Vertex* v1 = edge->getVertex (V_AMONT);
   Vertex* v2 = edge->getVertex (V_AVAL );

   printf (" Asso edge %s", edge->getName(cc1));
   printf (" =(%s,%s)", v1->getName(cc1), v2->getName(cc2));
   printf (" : deb=%g, fin=%g\n", assnew->debut, assnew->fin);

   TopoDS_Shape  theShape = string2shape( brep );
   TopoDS_Edge   theEdge  = TopoDS::Edge( theShape );
   double        theCurve_length = edge_length( theEdge );


   BRepAdaptor_Curve* theCurve         = NULL;
   gp_Pnt  theCurve_start, theCurve_end;
   theCurve = new BRepAdaptor_Curve( theEdge );

   GCPnts_AbscissaPoint discret_start (*theCurve, 
                                       theCurve_length * assnew->debut, 
                                       theCurve->FirstParameter() );
  GCPnts_AbscissaPoint discret_end   (*theCurve, 
                                       theCurve_length * assnew->fin, 
                                       theCurve->FirstParameter() );

   double u_start = discret_start.Parameter();
   double u_end   = discret_end.Parameter();

   theCurve_start  = theCurve->Value( u_start);
   theCurve_end    = theCurve->Value( u_end );
   printf ("   Start= (%g, %g, %g)\n", theCurve_start.X(), 
                                       theCurve_start.Y(), theCurve_start.Z());
   printf ("   End  = (%g, %g, %g)\n", theCurve_end.X(), 
                                       theCurve_end.Y(), theCurve_end.Z());
   delete theCurve;
}
// ============================================================== edge_length
double edge_length (const TopoDS_Edge& edge )
{
  if (BRep_Tool::Degenerated(edge))
    return 0;

  double umin = 0, umax = 0;
  TopLoc_Location    loc;
  Handle(Geom_Curve) curv = BRep_Tool::Curve (edge, loc, umin, umax);
  GeomAdaptor_Curve AdaptCurve(curv);

  double lg = GCPnts_AbscissaPoint::Length(AdaptCurve, umin, umax);
  return lg;
}
// ====================================================== carre
inline double carre (double val)
{
   return val*val;
}
// ============================================================== edge_length
double edge_length (double x1, double y1, double z1, 
                    double x2, double y2, double z2)
{
  double lg = carre (x2-x1) + carre (y2-y1) + carre (z2-z1);
  return sqrt (lg);
}
// ============================================================== edge_length
double edge_length (Edge* ed)
{
  Vertex* v1 = ed->getVertex (V_AMONT);
  Vertex* v2 = ed->getVertex (V_AVAL );

  double longueur = edge_length (v1->getX(), v1->getY(), v1->getZ(), 
                                 v2->getX(), v2->getY(), v2->getZ());
  return longueur;
}
// ====================================================== meme_points
inline bool meme_points (gp_Pnt& pnt1, gp_Pnt& pnt2)
{
   const double Epsil2 = 1e-10;

   double d2 = carre (pnt2.X()-pnt1.X()) + carre (pnt2.Y()-pnt1.Y()) 
                                         + carre (pnt2.Z()-pnt1.Z());
//    std::cout << "pnt1 : " << pnt1.X() << " " << pnt1.Y() << " " << pnt1.Z() << std::endl;
//    std::cout << "pnt2 : " << pnt2.X() << " " << pnt2.Y() << " " << pnt2.Z() << std::endl;   
//    HexDisplay(d2);
   return d2 < Epsil2;
}

// ====================================================== vertexInLine
int vertexInLine (int nedge, int nblines, vector<int> &torig,
                             vector <gp_Pnt>&  textrem)
{
   int sens  = 1 - torig [nedge];
   int kpnt  = 2*nedge;
   if (sens==V_AVAL) kpnt = kpnt+1;

   for (int nro = 0 ; nro<nblines ; nro++)
       {

//           std::cout << "nro = " << nro << " : " << std::endl;
       if (torig [nro] == NOTHING)
          {
//              std::cout << " NOTHING : kpnt = " << kpnt << std::endl ;
          if (meme_points (textrem [kpnt], textrem [2*nro]))
             {
//                 std::cout << kpnt << " " << 2*nro << std::endl;
             torig [nro] = V_AMONT;
             return nro;
             }
          else if (meme_points (textrem [kpnt], textrem [2*nro+1]))
             {
//                 std::cout << kpnt << " " << 2*nro+1 << std::endl;                 
             torig [nro] = V_AVAL;
             return nro;
             }
          }
       }
 
   return NOTHING;
}
//
// ====================================================== associateCascade
int Document::associateCascade (Edges& mline, int msens[], Shape* gstart, 
                                Shapes& gline, double pstart, double pend, 
                                bool closed)
{
   cout << "____________________________________________"
        << " Document::associateCascade" << endl;

   int nbedges  = mline.size ();
   int nbshapes = gline.size ();
   int nblines  = nbshapes + 1; 

   Shapes                      tabg_shape  (nblines);
   vector <TopoDS_Edge>        tabg_line   (nblines);
   vector <int>                tabg_orig   (nblines, NOTHING);
   vector <double>             tabg_deb    (nblines, 0.0);
   vector <double>             tabg_fin    (nblines, 1.0);
   vector <double>             tabg_length (nblines);
   vector <BRepAdaptor_Curve*> tabg_curve  (nblines);
   vector <gp_Pnt>             tabg_point  (2*nblines);

   vector <int>         shape_posit (nblines);
   //  vector <double>      shape_absc  (nblines);

                                    // -------- Memorisation des shapes
   tabg_shape [0] = gstart;
   for (int ns=0 ; ns<nbshapes ; ns++)
       tabg_shape [ns+1] = gline [ns];

                                    // -------- Creation des TopoDS_edges
   double lg_lines  = 0;
   for (int ns=0 ; ns<nblines ; ns++)
       {
       tabg_line [ns] = asso2edge (tabg_shape [ns]);
       BRepAdaptor_Curve* curve = new BRepAdaptor_Curve (tabg_line [ns]);
       double lg  = edge_length(tabg_line[ns]);

       // correction Alain le 07/07/2011 :
//        double deb = ns==0        ? pstart*lg : 0.0;
//        double fin = ns==nbshapes && NOT closed ? pend*lg   : lg;
       double deb = 0;
       double fin = lg;
       if (ns==0)
       {
           deb = pstart*lg;
           fin = lg - deb;
       }
       else if (ns==nbshapes && NOT closed)
       {
           fin = pend*lg;
           deb = lg - fin;
       }
       
       GCPnts_AbscissaPoint s1 (*curve, deb, curve->FirstParameter());
       GCPnts_AbscissaPoint s2 (*curve, fin, curve->FirstParameter());

       double u1 = s1.Parameter ();
       double u2 = s2.Parameter ();

       tabg_deb    [ns] = deb;
       tabg_fin    [ns] = fin;
       tabg_curve  [ns] = curve;
       tabg_length [ns] = lg - deb;
       tabg_point  [2*ns]   = curve->Value (u1);
       tabg_point  [2*ns+1] = curve->Value (u2);
       lg_lines += tabg_length [ns];

       if (db) 
          {
          double lg2 = edge_length(tabg_point[2*ns].X(), tabg_point[2*ns].Y(),
                                tabg_point[2*ns].Z(), tabg_point[2*ns+1].X(), 
                                tabg_point[2*ns+1].Y(),tabg_point[2*ns+1].Z());

          printf (" %d : (%g,%g,%g), lg=%g, deb=%g, fin=%g, lg2=%g\n", ns, 
               tabg_point[2*ns].X(), tabg_point[2*ns].Y(),
               tabg_point[2*ns].Z(), tabg_length [ns], deb, fin, lg2);
          printf ("     (%g,%g,%g)\n",  tabg_point[2*ns+1].X(),
               tabg_point[2*ns+1].Y(), tabg_point[2*ns+1].Z());
          }
       }
                                    //   -------- Tri des TopoDS_edges
                                    //   -------- memorisation des longueurs
   Dtable tgx, tgy, tgz;
   Dtable tmx, tmy, tmz;
   Dtable tax, tay, taz;
 
   double abscisse = tabg_length [0]; 
   shape_posit [0] = 0;

   tabg_orig   [0] = V_AMONT;
   
   int nedge = 0;
   for (int ns=1 ; ns<nblines ; ns++)
       {
       int nro = vertexInLine (nedge, nblines, tabg_orig, tabg_point);

       // JPL le 12/07/2011 : si c'est la premiere ligne de la
       // geometrie, on teste les 2 points :
       // => dans ce cas, quel interet de mettre pstart en argument ?
       // REM : il faudrait faire la meme chose pour la derniere ligne ?
       
       if (nro == NOTHING && not closed && nedge == 0)
          {
          tabg_orig[0] = V_AVAL;
          if (db) printf (" . :  Origine = V_AVAL\n");
          nro = vertexInLine (nedge, nblines, tabg_orig, tabg_point);
          }
       if (nro == NOTHING)
          {
          char cnum1 [8], cnum2[8];
          sprintf (cnum1, "%d", nedge);
          sprintf (cnum2, "%d", 1-tabg_orig[nedge]);
          putError (W_ASSO_LINE2, cnum1, cnum2);
          return HERR;
          }

       nedge      = nro;
       abscisse += tabg_length [nro];
       shape_posit [ns] = nro;
       }

   if (db)
      {
      printf (" --------------- Ordonnancement des lignes\n");

      for (int ns=0 ; ns<nblines ; ns++)
          {
          int nro = shape_posit [ns];
          printf (" %d : tabg_line[%d](%d), orig=(%g,%g,%g), lg=%g, s=%g\n", 
                      ns, nro, tabg_orig[nro], 
                      tabg_point [2*nro + tabg_orig[nro]].X(), 
                      tabg_point [2*nro + tabg_orig[nro]].Y(), 
                      tabg_point [2*nro + tabg_orig[nro]].Z(), 
                      tabg_length [nro]);

          tgx.push_back (tabg_point [2*nro + tabg_orig[nro]].X());
          tgy.push_back (tabg_point [2*nro + tabg_orig[nro]].Y());
          tgz.push_back (tabg_point [2*nro + tabg_orig[nro]].Z());
          }
       }

   incrementer_xmgr  ();
   if (db) save_xmgr (tgx, tgy, tgz, "pts_geom");

   vector <double> mod_absc (nbedges);
   double lg_edges = 0;

   for (int ned=0 ; ned<nbedges ; ned++)
       {
       lg_edges += edge_length (mline[ned]);
       mod_absc [ned] = lg_edges;
       }

   if (db) 
      printf (" --------------- Ordonnancement des edges\n");
   char cc1[8], cc2[8];
   for (int ned=0 ; ned<nbedges ; ned++)
       {
       mod_absc [ned] /= lg_edges;
       if (db) 
       {
          Vertex* v1 = mline[ned]->getVertex(msens[ned]);
          Vertex* v2 = mline[ned]->getVertex(1-msens[ned]);
          printf (" %d : (%g %g %g) = (%s.%s)[%d] s=%g\n", ned, 
                   v1->getX(), v1->getY(), v1->getZ(), 
                v1->getName(cc1), v2->getName(cc2), msens[ned], mod_absc[ned]);

          tmx.push_back (v1->getX());
          tmy.push_back (v1->getY());
          tmz.push_back (v1->getZ());
          }
       }

   if (db) save_xmgr (tmx, tmy, tmz, "pts_model");

   Vertex* sommet = mline[0] ->getVertex (msens[0]); 
   int npoint = tabg_orig  [0];

   double ppx = sommet->getX ();
   double ppy = sommet->getY ();
   double ppz = sommet->getZ ();

   asso2vertex (sommet, tabg_point[npoint]);

   double sm1=0, sm2 = 0;

   if (db) 
      {
      HexDisplay (nbedges );
      HexDisplay (nblines );

      printf (" --------------- Associations \n");
      printf ("  ... asso point m=%d,g=%d %s : (%g %g %g) -> (%g %g %g)\n", 
              msens[0], tabg_orig[0], sommet->getName(cc1), ppx, ppy, ppz,
              tabg_point[npoint].X(), tabg_point[npoint].Y(),
              tabg_point[npoint].Z());
      }

   tax.push_back (tabg_point[npoint].X());
   tay.push_back (tabg_point[npoint].Y());
   taz.push_back (tabg_point[npoint].Z());

   int nbm = 1;
   for (int ned=0 ; ned<nbedges ; ned++)
       {
       bool point_libre = true;
       Edge*    edge  = mline[ned];
       Vertex* orig = edge->getVertex (msens[ned]); 
       sommet = edge->getVertex (1-msens[ned]); 
       if (closed && ned==nbedges-1) 
          point_libre = false;

       edge->clearAssociation ();
       sm1 = sm2;
       // sm2 = mod_absc [ned];
       sm2 = (ned+1.0) /nbedges;
       double sg1=0, sg2 = 0;
       if (db) 
          printf (" edge=%d =[%s,%s] in [%g, %g]\n", ned, orig->getName(cc1),  
                   sommet->getName(cc2), sm1, sm2);
        
       for (int ns=0 ; ns<nblines ; ns++)
           {
           int nro  = shape_posit [ns];
           sg1 = sg2;
           // sg2 = shape_absc [nro]; 
           sg2 = (ns+1.0) / (nblines);
           if (sg1 < sm2 && sg2 > sm1)
              {
              double dg = (sg2-sg1);
              double paradeb = sg1 < sm1 ? (sm1-sg1)/dg : 0.0;
              double parafin = sg2 > sm2 ? (sm2-sg1)/dg : 1.0;

              if (tabg_orig[nro] == V_AVAL)
                 {
                 Echo ( "On inverse paradeb et parafin");
                 paradeb = 1 - paradeb;
                 parafin = 1 - parafin;
                 }

              asso2edge (edge, tabg_shape[nro], paradeb, parafin);

              if (db) 
                 printf ("  ... asso ligne=%d in [%g, %g]\n", nro, sg1, sg2);

              if (point_libre && sm2 >= sg1 && sm2 <= sg2+HEXA_EPSILON)
                 {
                 point_libre  = false;
                 BRepAdaptor_Curve* curve = tabg_curve [nro];

                 double alpha = tabg_deb [nro] + parafin*tabg_length [nro];
                 GCPnts_AbscissaPoint s1 (*curve,alpha, curve->FirstParameter());
                 double u1 = s1.Parameter ();

                 gp_Pnt pnt_asso = curve->Value (u1);
                 ppx = sommet->getX ();
                 ppy = sommet->getY ();
                 ppz = sommet->getZ ();

                 asso2vertex (sommet, pnt_asso);

                 tax.push_back (pnt_asso.X());
                 tay.push_back (pnt_asso.Y());
                 taz.push_back (pnt_asso.Z());

                 nbm ++;
                 if (db)
                    {
                    printf ("  ... asso point m=%d,g=%d %s k=%g :", 
                         ned, nro, sommet->getName(cc1), parafin);
                    printf (" sm2=%g in (%g,%g)\n", sm2, sg1, sg2);
                    printf ("  ... asso point: orig=%d,deb=%g,lg=%g,alpha=%g\n",
                      tabg_orig[nro], tabg_deb [nro], tabg_length [nro], alpha);
                    printf ("  ... asso point : (%g %g %g) -> (%g %g %g)\n", 
                            sommet->getX(),sommet->getY(),sommet->getZ(),
                            pnt_asso.X(), pnt_asso.Y(), pnt_asso.Z());
                    }
                 }
              }
           }
       }

   if (db) save_xmgr (tax, tay, taz, "pts_asso");

   cout << "______________________________________________________"
        << " End associateCascade" << endl;
   return HOK;
}
END_NAMESPACE_HEXA
      
#else    // #ifndef NO_CASCADE

BEGIN_NAMESPACE_HEXA
int Document::associateCascade (Edges& mline, int msens[], Shape* gstart, 
                                Shapes& gline, double pstart, double pend, 
                                bool closed)
{
   return HOK;
}
END_NAMESPACE_HEXA
#endif
