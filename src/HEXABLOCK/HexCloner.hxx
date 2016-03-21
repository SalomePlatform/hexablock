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

// class : Le Reproducteur de blocks

#ifndef __CLONER_H
#define __CLONER_H

#include "hexa_base.hxx"

#include <map>

BEGIN_NAMESPACE_HEXA

class HexaExport Cloner 
{
public:
    Cloner (Matrix* mat);

    void erase ();

    Hexa*     clonerHexa     (Hexa*     orig);
    Quad*     clonerQuad     (Quad*     orig);
    Edge*     clonerEdge     (Edge*     orig);
    Vertex*   clonerVertex   (Vertex*   orig);
    Elements* clonerElements (Elements* orig);

private:
    Matrix* matrice;
    std::map <Hexa*,   Hexa*>   clone_hexa;
    std::map <Quad*,   Quad*>   clone_quad;
    std::map <Edge*,   Edge*>   clone_edge;
    std::map <Vertex*, Vertex*> clone_vertex;
};
END_NAMESPACE_HEXA
#endif
