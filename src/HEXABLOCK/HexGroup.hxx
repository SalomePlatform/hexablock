
// Class : Implementation des groupes

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

#ifndef __GROUP_H
#define __GROUP_H

#include "HexEltBase.hxx"

BEGIN_NAMESPACE_HEXA

class HexaExport Group : public EltBase 
{
public :
   int       addElement    (EltBase* elt);
   int       countElement  ()              { return grp_table.size () ; }
   EnumGroup getKind ()                    { return grp_kind ; }
   int       removeElement (int nro);
   void      clearElement  ()              { grp_table.clear () ; }
   int       removeElement (EltBase* elt);
   EltBase*  getElement    (int nro)       { return grp_table [nro] ; }

   Group (Document* dad, cpchar nom, EnumGroup grp);
#ifndef SWIG
public :
   int       findElement   (EltBase* elt);
  ~Group ()                   {}
   void      saveXml  (XmlWriter* xml);
   EnumElt   getTypeElt ()                 { return grp_typelt ; }

   static EnumGroup getKind (cpchar kind);
   static EnumGroup getKind (const string& k) { return getKind (k.c_str());}

private :
   int grp_id;
   EnumGroup   grp_kind;
   EnumElt     grp_typelt;
   std::vector <EltBase*> grp_table;
#endif
};

END_NAMESPACE_HEXA
#endif
