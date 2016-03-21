
// class : Au nom de la loi

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
#ifndef __LAW_H_
#define __LAW_H_

#include "HexEltBase.hxx"
#include "HexXmlWriter.hxx"

BEGIN_NAMESPACE_HEXA

class HexaExport Law : public EltBase  
{
public:
   const char* getName ()          { return law_name.c_str() ; }
   char* getNextName  (pchar buffer);
   int     getNodes ()             { return nbr_nodes ; }
   double  getCoefficient ()       { return law_coefficient ; }
   KindLaw getKind ()              { return law_type ;  }

   int     setNodes (int  nbre);
   int     setCoefficient (double coeff);
   void    setKind (KindLaw type)  { law_type = type ;  }
   void    setKind (cpchar  type);
   int     setName (cpchar  nom);

    Law (Document* dad, cpchar name, int nbnodes);
    Law (Law* other);
    void saveXml (XmlWriter* xml);

private:
    int law_id;
    static int last_law_id;
    std::string law_name;
    int         nbr_nodes;
    KindLaw     law_type;
    double      law_coefficient;
};

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
inline int Law::setName (cpchar name)
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
// ================================================== setKind
inline void Law::setKind (cpchar type)
{
#define VerifKind(t,k) if (strcmp(t,#k)==0) law_type = k
   VerifKind (type, Uniform);
   VerifKind (type, Arithmetic);
   VerifKind (type, Geometric);
}

END_NAMESPACE_HEXA
#endif
