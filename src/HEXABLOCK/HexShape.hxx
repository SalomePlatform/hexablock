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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// Class : Implementation provisoire et symbolique de la classe "shape"
//

// #ifdef  _NO_CASCADE 
#ifndef __SHAPE_H_
#define __SHAPE_H_

#include "hexa_base.hxx"

BEGIN_NAMESPACE_HEXA

class Shape 
{
public :
    Shape (string brep);
    string getBrep ()  { return b_rep; }

    void  setBrep   (string brep)         { b_rep = brep; }
    void  setBounds (int pdeb, int pfin)  { debut = pdeb ; fin = pfin; }
    void  setStart  (int val)             { debut = val ; }
    void  setEnd    (int val)             { fin   = val ; }

public :
    double debut;
    double fin;
    string ident;

private :
    std::string b_rep;
};
END_NAMESPACE_HEXA
#endif
// #endif
