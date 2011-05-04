//
// CC++ : interface modele/cascade
//

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

// SMESH includes
//  #include "SMDS_MeshNode.hxx"
//  #include "SMDS_MeshVolume.hxx"
//  #include "SMESH_Gen.hxx"
//  #include "SMESH_MesherHelper.hxx"
//  #include "SMESHDS_Group.hxx"

// HEXABLOCK includes
#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexQuad.hxx"
#include "HexHexa.hxx"
#include "HexPropagation.hxx"
#include "HexShape.hxx"
#include "HexGroups.hxx"
#include "HexDiagnostics.hxx"

//  #include "Basics_Utils.hxx"
//  #include "utilities.h"

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
static int MYDEBUG = 1;
#else
static int MYDEBUG = 1;
#endif
#define MESSAGE(m) cout << m << endl
#define Assert(m) if (m!=0) { cout<<" **** Assert "<<m<< endl; exit(101);}

static double HEXA_EPSILON  = 1E-6; //1E-3; 
//  static double HEXA_QUAD_WAY = PI/4.; //3.*PI/8.;

BEGIN_NAMESPACE_HEXA

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
TopoDS_Edge asso2edge (Shape* asso)
{
   string brep = asso->getBrep();

   istringstream streamBrep(brep);
   BRep_Builder  aBuilder;
   TopoDS_Shape  shape;

   BRepTools::Read(shape, streamBrep, aBuilder);
   return TopoDS::Edge(shape);
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
// ====================================================== meme_points
inline bool meme_points (gp_Pnt& pnt1, gp_Pnt& pnt2)
{
   const double Epsil2 = 1e-10;

   double d2 = carre (pnt2.X()-pnt1.X()) + carre (pnt2.Y()-pnt1.Y()) 
                                         + carre (pnt2.Z()-pnt1.Z());
   return d2 < Epsil2;
}

// ====================================================== vertexInLine
int vertexInLine (int nedge, int nbrseg, vector<int> &torig,
                             vector <gp_Pnt>&  textrem)
{
   int sens  = 1 - torig [nedge];
   int kpnt  = 2*nedge;
   if (sens==V_AVAL) kpnt = kpnt+1;

   for (int nro = 0 ; nro<nbrseg ; nro++)
       {
       if (torig [nro] == NOTHING)
          {
          if (meme_points (textrem [kpnt], textrem [2*nro]))
             {
             torig [nro] = V_AMONT;
             return nro;
             }
          else if (meme_points (textrem [kpnt], textrem [2*nro+1]))
             {
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
   cout << "______________________________________________________"
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
   vector <double>      shape_absc  (nblines);

   vector <TopoDS_Edge> tab_line;
   vector <int>         tab_orig, tmp_orig (nblines, NOTHING);
   vector <double>      tab_absc;
   vector <BRepAdaptor_Curve*> tab_curve, tmp_curve;

   HexDisplay (nbedges );
   HexDisplay (nbshapes);
   HexDisplay (pstart);
   HexDisplay (pend);
                                    // -------- Creation des TopoDS_edges
   tabg_shape [0] = gstart;
   for (int ns=0 ; ns<nbshapes ; ns++)
       tabg_shape [ns+1] = gline [ns];

                                    // -------- Creation des TopoDS_edges
   for (int ns=0 ; ns<nblines ; ns++)
       {
       tabg_line [ns] = asso2edge (tabg_shape [ns]);
       BRepAdaptor_Curve* curve = new BRepAdaptor_Curve (tabg_line [ns]);
       double lg = edge_length(tabg_line[ns]);

       double deb = ns==0        ? pstart*lg : 0.0;
       double fin = ns==nbshapes && NOT closed ? pend*lg   : lg;
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
       printf (" %d : (%g,%g,%g), lg=%g, deb=%g, fin=%g\n", ns, 
               tabg_point[2*ns].X(), tabg_point[2*ns].Y(),
               tabg_point[2*ns].Z(), tabg_length [ns], deb, fin);
       printf ("     (%g,%g,%g)\n",  tabg_point[2*ns+1].X(),
               tabg_point[2*ns+1].Y(), tabg_point[2*ns+1].Z());
       }

   int nbr=tabg_point.size();
   for (int n1=0 ; n1<nbr ; n1++)
       for (int n2=n1+1 ; n2<nbr ; n2++)
           if (meme_points (tabg_point [n1], tabg_point [n2]) )
              printf ("tabg_point [%d] == tabg_point [%d] == (%g,%g,%g)\n", 
                      n1, n2, tabg_point [n1].X(),  tabg_point [n1].Y(), 
                              tabg_point [n1].Z());
                                    //   -------- Tri des TopoDS_edges
                                    //   -------- memorisation des longueurs
   shape_absc  [0] = 0;
   shape_posit [0] = 0;
   tabg_orig   [0] = V_AMONT;

   double lg_totale  = tabg_length [0];
   int    nedge      = 0;

   printf (" %d : tabg_line[%d](%d), s=%g\n", nedge, nedge,tabg_orig[nedge], 
                                              lg_totale);
   for (int ns=1 ; ns<nblines ; ns++)
       {
       int nro = vertexInLine (nedge, nblines, tabg_orig, tabg_point);

       if (nro == NOTHING)
          {
          char cnum1 [8], cnum2[8];
          sprintf (cnum1, "%d", nedge);
          sprintf (cnum2, "%d", 1-tabg_orig[nedge]);
          putError (W_ASSO_LINE2, cnum1, cnum2);
          return HERR;
          }

       nedge      = nro;
       lg_totale += tabg_length [nro];
       shape_absc  [ns] = lg_totale;
       shape_posit [ns] = nro;

       printf (" %d : tabg_line[%d](%d), s=%g\n", ns, nro, tabg_orig[nro], 
                                                  lg_totale);
       }

   for (int ns=0 ; ns<nblines ; ns++)
       shape_absc [ns] /= lg_totale;

   vector <double> mod_absc (nbedges);
   lg_totale = 0;

   for (int ned=0 ; ned<nbedges ; ned++)
       {
       Vertex* v1 = mline[ned]->getVertex(V_AMONT);
       Vertex* v2 = mline[ned]->getVertex(V_AVAL );

       double longueur = edge_length (v1->getX(), v1->getY(), v1->getZ(), 
                                      v2->getX(), v2->getY(), v2->getZ());

       char s1[8], s2[8];
       printf ("(%s.%s)[%d]\n", v1->getName(s1), v2->getName(s2), msens[ned]);
       lg_totale += longueur;
       mod_absc [ned] = lg_totale;
       }

   for (int ned=0 ; ned<nbedges ; ned++)
       {
       mod_absc [ned] /= lg_totale;
       }

   Vertex* sommet = mline[0] ->getVertex (msens[0]); 
   sommet->setAssociation (NULL);
   sommet->setX (tabg_point[0].X());
   sommet->setY (tabg_point[0].Y());
   sommet->setZ (tabg_point[0].Z());

   int ns = 0;
   double sm1=0, sm2 = 0;
   for (int ned=0 ; ned<nbedges ; ned++)
       {
       Edge*    edge  = mline[ned];
       sommet = edge->getVertex (1-msens[ned]); 
       if (closed && ned==nbedges-1) 
          sommet = NULL;
       edge ->clearAssociations ();
       sm1 = sm2; 
       sm2 = mod_absc [ned];
       double sg1=0, sg2 = 0;
       for (int ns=0 ; ns<nblines ; ns++)
           {
           sg1 = sg2;
           sg2 = shape_absc [ns]; 
           if ( (sg1 >= sm1 && sg1 < sm2) || (sg2 >= sm1 && sg2 < sm2))
               edge ->addAssociation (tabg_shape[ns]);

	   if (sommet != NULL && sm2 >= sg1 && sm2 < sg2)
              {
              double alpha = (sm2-sg1) / (sg2-sg1);
              int    nro   = shape_posit [ns];
              BRepAdaptor_Curve* curve = tabg_curve [nro];

              if (tabg_orig[nro] == V_AMONT)
                 alpha = tabg_deb [nro] + alpha*tabg_length [nro];
              else
                 alpha = tabg_fin [nro] + (1-alpha)*tabg_length [nro];

              GCPnts_AbscissaPoint s1 (*curve, alpha, curve->FirstParameter());
              double u1 = s1.Parameter ();
              gp_Pnt pnt_asso = curve->Value (u1);
              sommet->setAssociation (NULL);
              sommet->setX (pnt_asso.X());
              sommet->setY (pnt_asso.Y());
              sommet->setZ (pnt_asso.Z());
              }
           }
       }

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
