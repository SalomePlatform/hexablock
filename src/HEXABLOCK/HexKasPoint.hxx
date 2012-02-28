//
// CC++ : Representation Cascade d'un point
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

#ifndef __KAS_POINT_HXX_
#define __KAS_POINT_HXX_

#include "HexGlobale.hxx"

#ifndef NO_CASCADE

#include <gp_Pnt.hxx>
#include <TopoDS_Vertex.hxx>

BEGIN_NAMESPACE_HEXA
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
END_NAMESPACE_HEXA
#endif
#endif
