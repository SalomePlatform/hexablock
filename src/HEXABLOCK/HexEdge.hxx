
// class : Gestion des aretes

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
#ifndef __EDGE_H
#define __EDGE_H

#include "HexVertex.hxx"

BEGIN_NAMESPACE_HEXA

class HexaExport Edge : public EltBase
{
public:
    virtual Vertex* getVertex (int  nro);

    int addAssociation  (NewShape* geom, int subid, double deb, double fin);
    int setAssociation  (NewShape* geom, int subid);
    int findAssociation (NewShape* geom);
    virtual void clearAssociation ();
    void setColor  (double valeur);
    bool    getWay ()              { return e_way ; }
    double  getLength ();

    Edge (Vertex* va, Vertex* vb);

#ifndef SWIG
public:
    virtual int  countVertex () { return V_TWO; }
    virtual void saveXml (XmlWriter* xml);
    virtual void replaceVertex (Vertex* old, Vertex* nouveau);

    Edge (Edge* other);
    int  anaMerge  (Vertex* orig, Vertex* couple[]);
    bool definedBy (Vertex* v1,   Vertex* v2);

    void  propager (Propagation* prop, int nro, int sens=1);

    void  setPropag (int nro, bool sens);
    int   getPropag ()                   { return e_propag ; }

    Quad* getParent (int nro);
    virtual void dump ();
    void         dumpPlus ();
    void         dumpAsso ();

    virtual void majReferences();

    int  inter (Edge* other, int& nother);
    int  inter (Edge* other);
    int  index (Vertex* node);
    Vertex* commonVertex  (Edge* other);
    Vertex* opposedVertex (Vertex* sommet);
    double* commonPoint   (Edge* other, double point[]);

    Vertex* getAmont ()       { return e_vertex [NOT e_way] ; }
    Vertex* getAval  ()       { return e_vertex [e_way] ; }

    Law* getLaw ()            { return e_law ; }
    void setLaw (Law* law)    { e_law =  law ; }

    void setScalar (double valeur)          { setColor (valeur) ; }

    virtual void duplicate ();
    Edge*   getClone  ()                    { return e_clone ;   }
    double* getVector (double vecteur[]);
    string  makeDefinition ();

    int addAssociation (EdgeShape* gline, double deb, double fin);
    int checkAssociation ();
    int countAssociation () { return tab_assoc.size () ; }
    AssoEdge* getAssociation (int nro);
    double    getAssoLen ();

private:
    friend class Cloner;
    Vertex*   e_vertex [V_TWO];
    Edge*     e_clone;
    int       e_propag;
    bool      e_way;     // Sens de propagation
    Law*      e_law;     // Le soleil brille, brille, brille
    AssoEdges tab_assoc;
#endif
};
END_NAMESPACE_HEXA
#endif

