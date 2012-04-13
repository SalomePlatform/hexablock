//
// CC++ : Representation Cascade d'un bi-cylindre
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

#ifndef __KAS_BI_CYLINDER_HXX_
#define __KAS_BI_CYLINDER_HXX_

#include "HexGlobale.hxx"

#ifndef NO_CASCADE

BEGIN_NAMESPACE_HEXA

class KasLine;

class KasBiCylinder
{
public :
   KasBiCylinder () { purge () ; }
  ~KasBiCylinder () { purge () ; }
 
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
END_NAMESPACE_HEXA
#endif
#endif
