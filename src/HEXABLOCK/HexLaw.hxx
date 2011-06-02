// Copyright (C) 2009-2011  CEA/DEN, EDF R&D
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


// class : Au nom de la loi

#ifndef __LAW_H
#define __LAW_H

#include "HexXmlWriter.hxx"

BEGIN_NAMESPACE_HEXA

class Law 
{
public:
   const char* getName ()          { return law_name.c_str() ; }
   int     getNodes ()             { return nbr_nodes ; }
   double  getCoefficient ()       { return law_coefficient ; }
   KindLaw getKind ()              { return law_type ;  }

   int     setNodes (int  nbre);
   int     setCoefficient (double coeff);
   void    setKind (KindLaw type)  { law_type = type ;  }
   int     setName (const char* nom);

public:
    Law (cpchar name, int nbnodes);
    void saveXml (XmlWriter* xml);

private:
    std::string law_name;
    int         nbr_nodes;
    KindLaw     law_type;
    double      law_coefficient;
};
// ================================================== Constructeur
inline Law::Law (cpchar name, int nbnodes)
{
   nbr_nodes = nbnodes;
   law_name  = name;
   law_type  = Uniform;
   law_coefficient = 0.0;
}
// ================================================== setNodes
inline int Law::setNodes (int  nbre)
{
   nbr_nodes = nbre;
   return HOK;
}
// ================================================== setCoefficient
inline int Law::setCoefficient (double coeff)
{
   law_coefficient = coeff;
   return HOK;
}
// ================================================== setName
inline int Law::setName (const char* name)
{
   law_name = name;
   return HOK;
}
// ================================================== saveXml
inline void Law::saveXml (XmlWriter* xml)
{
   cpchar kind_law [] = { "Uniform", "Arithmetic", "Geometric" };

   xml->openMark     ("Law");
   xml->addAttribute ("id",          getName ());
   xml->addAttribute ("kind",    kind_law [law_type]);
   xml->addAttribute ("nodes",   nbr_nodes);
   xml->addAttribute ("coeff", law_coefficient);
   xml->closeMark ();
}
END_NAMESPACE_HEXA
#endif
