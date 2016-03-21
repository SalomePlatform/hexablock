//
// C++ : Implementation des associations d'edges
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

#include "HexAssoEdge.hxx"
#include "HexXmlWriter.hxx"

#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>

#include <GCPnts_AbscissaPoint.hxx>

BEGIN_NAMESPACE_HEXA

// static bool db = on_debug ();

// ======================================================= Constructeur
AssoEdge::AssoEdge (EdgeShape* shape, double deb, double fin)
{
   arc_line  = shape;
   arc_para1 = std::min (deb, fin);
   arc_para2 = std::max (deb, fin);

   arc_line->getPoint (arc_para1, arc_start);
   arc_line->getPoint (arc_para2, arc_end);

   //  arc_curve   = arc_line->getCurve (); Provisoire en attendant modif plugin
   arc_curve   = arc_line->makeCurve ();
   arc_reverse = false;
}
// ========================================================= onExtremity
int AssoEdge::onExtremity (double* point)
{
   if (EdgeShape::samePoints  (point, arc_start))
      return V_AMONT;
   else if (EdgeShape::samePoints  (point, arc_end))
      return V_AVAL;
   else
      return NOTHING;
}
// ========================================================= onExtremity
int AssoEdge::onExtremity (AssoEdge* other, int& ext_other)
{
   ext_other = NOTHING;
   if (other==NULL)
      return NOTHING;

   int rep = onExtremity (other->arc_start);
   if (rep != NOTHING)
      {
      ext_other = V_AMONT;
      }
   else
      {
      rep = onExtremity (other->arc_end);
      if (rep != NOTHING)
          ext_other = V_AVAL;
      }

   return rep;
}
// ========================================================= length
double AssoEdge::length ()
{
   double longueur = (arc_para2-arc_para1)*arc_line->getLength ();
   return longueur;
}
// ========================================================= getUstart
double AssoEdge::getUstart ()
{
   double ustart = 0;
   BRepAdaptor_Curve* curve = arc_line->getCurve () ;
   double             len   = arc_line->getLength ();

   GCPnts_AbscissaPoint discret_start (*curve, len*arc_para1,
                                        curve->FirstParameter() );
   ustart = discret_start.Parameter();
   return ustart;
}
// ========================================================= saveXml
void AssoEdge::saveXml (XmlWriter* xml)
{
   char     interval [80];
   sprintf (interval, "%g %g", arc_para1, arc_para2);

   arc_line->callXml (xml);
   xml->addAttribute ("interval", interval);
   xml->closeMark ();
}
// ========================================================= setStart
void AssoEdge::setStart (double param)
{
   arc_para1 = param;
   arc_line->getPoint (arc_para1, arc_start);
}
// ========================================================= setEnd
void AssoEdge::setEnd (double param)
{
   arc_para2 = param;
   arc_line->getPoint (arc_para2, arc_end);
}
// -----------------------------------------------------------------------
// ========================================================= dump
void AssoEdge::dump ()
{
   cout << " AssoEddge = " << arc_line->getName()
        << " (" << arc_para1 << ", " << arc_para2 << ")" << endl;

   PutCoord (arc_start);
   PutCoord (arc_end);
}
END_NAMESPACE_HEXA
