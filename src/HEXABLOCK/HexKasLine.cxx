//
// CC++ : Implementation Cascade des lignes
//
// Copyright (C) 2009-2012  CEA/DEN, EDF R&D
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

#include "HexKasLine.hxx"
#include "HexKasPoint.hxx"
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

#include <GeomAPI_ProjectPointOnCurve.hxx>

BEGIN_NAMESPACE_HEXA

static bool db = false;
static const double TolAsso = 1e-2;    // Tolerance pour les associations

void arrondir (double &val);

// ======================================================= Constructeur
KasLine::KasLine ()
{
   db = on_debug ();
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
   db = on_debug ();
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
   lig_brep  = asso->getBrep  ();
   lig_ident = asso->getIdent ();
   lig_ior   = asso->getIor   ();

   if (fin<0.0)
      {
      lig_debut = asso->getStart();;
      lig_fin   = asso->getEnd();;
      }
   else if (deb >= UnEpsil)
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
      HexDisplay (deb);
      HexDisplay (fin);
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
void KasLine::assoPoint (double abscis, Vertex* node)
{
   GCPnts_AbscissaPoint s1 (*geom_curve, abscis, 
                             geom_curve->FirstParameter());
   double gparam       = s1.Parameter ();
   gp_Pnt pnt_asso = geom_curve->Value (gparam);

   KasPoint gpoint;
   gpoint.definePoint (pnt_asso);
   gpoint.associate   (node);

   if (db)
      {
      double* coord = gpoint.getCoord();
      char    car   = '%';
      if (node->definedBy (coord[dir_x], coord[dir_y], coord[dir_z]))
         car= ' ';
      printf (" ... assoPoint%c: v=%s (%g,%g,%g), p=%g", car,
           node->getName(), node->getX(), node->getY(), node->getZ(), abscis);
 
      printf (" -> (%g,%g,%g)\n", coord[dir_x], coord[dir_y], coord[dir_z]);
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

      cout << " Edge    = " << edge->getName() << 
	              " = [ " << segment[0]->getName() << ", " 
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
      printf (" Asso Line refusee %s -> (%g,%g)\n", edge->getName(),
                                                    lpara1, lpara2);
      }

                               // ---------------Association du vertex 
   double hparam = geom_inverse ? vpara2 : vpara1;
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

             double abscis = geom_total_length*hparam;
             GCPnts_AbscissaPoint s1 (*geom_curve, abscis, 
                                       geom_curve->FirstParameter());
             double gparam       = s1.Parameter ();
             gp_Pnt pnt_asso = geom_curve->Value (gparam);

                                          // .....  Creation d'un vertex Geom
             KasPoint gpoint;
             gpoint.definePoint (pnt_asso);
             gpoint.associate (node);
             if (db)
                {
                double* ass = gpoint.getCoord();
                printf (" Asso Point %s (%g,%g,%g) -> (%g,%g,%g) p=%g s=%g\n", 
                    node->getName(), node->getX(), node->getY(), node->getZ(),
                    ass[dir_x], ass[dir_y], ass[dir_z], hparam, smx);
                }
             }
         else if (db)
             {
             double abscis = geom_total_length*hparam;
             GCPnts_AbscissaPoint s1 (*geom_curve, abscis, 
                                    geom_curve->FirstParameter());
             double gparam   = s1.Parameter ();
             gp_Pnt pnt_asso = geom_curve->Value (gparam);

                                          // .....  Creation d'un vertex Geom
             KasPoint gpoint;
             gpoint.definePoint (pnt_asso);
             double* ass = gpoint.getCoord();
             // gpoint.associate (node);
             printf (" Asso Point %s (%g,%g,%g) -> (%g,%g,%g) p=%g s=%g\n", 
                     node->getName(),
                     node->getX(), node->getY(), node->getZ(),
                     ass[dir_x], ass[dir_y], ass[dir_z], hparam, smx);

             gpoint.definePoint (node);
             ass = gpoint.getCoord();
             printf ("               ignore car deja associe a (%g,%g, %g)\n", 
                     ass[dir_x], ass[dir_y], ass[dir_z]);
             }
          }
      hparam = geom_inverse ? vpara1 : vpara2;
      smx   = sm2;
      }
}
// ========================================================= assoEdge
void KasLine::assoEdge (Edge* edge, double para1, double para2, int vass)
{
   Shape* shape = new Shape (lig_brep);
   shape->setIdent  (lig_ident);
   shape->setIor    (lig_ior);
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
   Handle(Geom_Curve) handle = BRep_Tool::Curve (geom_line, umin, umax);

   GeomAPI_ProjectPointOnCurve projector (gpoint, handle);
   if ( projector.NbPoints() == 0 )
      return -1.0;

   double gparam = projector.LowerDistanceParameter();
   if (gparam <par_mini || gparam>par_maxi)
      {
      // cout << " Rejet : " << gparam << " not in (" << par_mini 
      //                              << ", " << par_maxi << ")" << endl;
      return -1.0;
      }
 
   gp_Pnt rpoint = geom_curve->Value (gparam);
   if (NOT same_coords (gpoint, rpoint))
      {
      //  cout << " Rejet : points differents " << endl;
      return -1.0;
      }


   GeomAdaptor_Curve  adapt_curve (handle);
   double abscis = GCPnts_AbscissaPoint::Length (adapt_curve, umin, gparam);
   double hparam = abscis/geom_total_length;
  
   // gparam = (gparam-par_mini) / (par_maxi-par_mini); 
   return hparam;
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
END_NAMESPACE_HEXA
#endif
