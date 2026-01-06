
// class : Gestion des formes associees

// Copyright (C) 2009-2026  CEA, EDF
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

#ifndef __SHAPE_H_
#define __SHAPE_H_

#include "hexa_base.hxx"

BEGIN_NAMESPACE_HEXA

class HexaExport Shape 
{
public :
    Shape (std::string brep);
    std::string getBrep  () { return b_rep; }
    std::string getIdent () { return ident; }
    std::string getIor   () { return ior; }

    double getStart () { return debut;  }
    double getEnd   () { return fin;   }
    void   getBounds (double& pdeb, double& pfin)  { pdeb=debut ; pfin = fin; }

    void  setBrep   (std::string brep)               { b_rep = brep; }
    void  setIdent  (std::string name)               { ident = name; }
    void  setIor    (std::string val)                { ior   = val; }

    void  setBounds (double pdeb, double pfin)  { debut = pdeb ; fin = pfin; }
    void  setStart  (double val)                { debut = val ; }
    void  setEnd    (double val)                { fin   = val ; }

    void  saveXml   (XmlWriter* xml);

private :
    double debut;
    double fin;
    std::string ident;
    std::string ior;

    std::string b_rep;
};
END_NAMESPACE_HEXA
#endif
