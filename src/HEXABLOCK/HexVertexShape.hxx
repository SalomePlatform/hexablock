
// class : Gestion des Sous-shapes categorie Vertex

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

#ifndef __NODE_SHAPE_H_
#define __NODE_SHAPE_H_

#include "HexSubShape.hxx"

BEGIN_NAMESPACE_HEXA

class HexaExport VertexShape : public SubShape
{
public :
    VertexShape (NewShape* dad, int id);
    VertexShape (NewShape* dad, int id, double* point);

    void    addAssociation   (Vertex* elt);
    int     countAssociation ()                { return tab_assoc.size(); }
    Vertex* getAssociation   (int nro);

    void getCoords (double& px, double& py, double& pz);
    void saveXml (XmlWriter* xml);

    // virtual const TopoDS_Shape& getShape(); // #mbarry
    bool definedBy (double point[]);

private :
    void updateCoords ();

private :
    Real3  ss_coord;   // Pour les vertex
    bool   maj_coords;
    bool   free_point;
    vector <Vertex*> tab_assoc;
};
END_NAMESPACE_HEXA
#endif
