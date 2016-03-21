
// class : Gestion des formes associees

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

#ifndef __SHAPE_H_
#define __SHAPE_H_

#include "hexa_base.hxx"

BEGIN_NAMESPACE_HEXA

class HexaExport Shape 
{
public :
    Shape (string brep);
    string getBrep  () { return b_rep; }
    string getIdent () { return ident; }
    string getIor   () { return ior; }

    double getStart () { return debut;  }
    double getEnd   () { return fin;   }
    void   getBounds (double& pdeb, double& pfin)  { pdeb=debut ; pfin = fin; }

    void  setBrep   (string brep)               { b_rep = brep; }
    void  setIdent  (string name)               { ident = name; }
    void  setIor    (string val)                { ior   = val; }

    void  setBounds (double pdeb, double pfin)  { debut = pdeb ; fin = pfin; }
    void  setStart  (double val)                { debut = val ; }
    void  setEnd    (double val)                { fin   = val ; }

    void  saveXml   (XmlWriter* xml);

private :
    double debut;
    double fin;
    string ident;
    string ior;

    std::string b_rep;
};
END_NAMESPACE_HEXA
#endif
