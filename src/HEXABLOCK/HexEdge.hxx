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

// class : Gestion des aretes

#ifndef __EDGE_H
#define __EDGE_H

#include "HexVertex.hxx"

BEGIN_NAMESPACE_HEXA

class Edge : public EltBase 
{
public:
    virtual int     countVertex () { return V_TWO; } 
    virtual Vertex* getVertex (int  nro);
    bool    getWay ()              { return e_way ; }

public:
    Edge (Vertex* va, Vertex* vb);
    Edge (Edge* other);
    int  anaMerge  (Vertex* orig, Vertex* couple[]);
    bool definedBy (Vertex* v1,   Vertex* v2);

    void  propager (Propagation* prop, int nro, int sens=1);

    void  setPropag (int nro, bool sens) { e_propag = nro; e_way=sens; }
    int   getPropag ()                   { return e_propag ; }

    Quad* getParent (int nro); 
    virtual void dump ();
    void         dumpPlus ();
    virtual void saveXml (XmlWriter* xml);
    virtual void majReferences();

    int  inter (Edge* other, int& na);
    int  inter (Edge* other);
    int  index (Vertex* node);
    Vertex* commonVertex (Edge* other);

    Vertex* getAmont ()       { return e_vertex [NOT e_way] ; } 
    Vertex* getAval  ()       { return e_vertex [e_way] ; } 

    Law* getLaw ()            { return e_law ; } 
    void setLaw (Law* law)    { e_law =  law ; } 

    void setScalar (double valeur);
    virtual void replaceVertex (Vertex* old, Vertex* nouveau);

    int   addAssociation (Shape* forme); 
    const Shapes & getAssociations ()       { return tab_assoc ;  }
    void  clearAssociations ()              { tab_assoc.clear() ; }

private:
    friend class Cloner;
    Vertex* e_vertex [V_TWO];
    int     e_propag;
    bool    e_way;     // Sens de propagation
    Law*    e_law;     // Le soleil brille, brille, brille

    Shapes tab_assoc;
};

// ----------------------------------------------- Inlining
// ========================================================== addAssociation
inline int Edge::addAssociation (Shape* forme)
{
   tab_assoc.push_back (forme);
   return HOK;
}
// =============================================================== getVertex
inline Vertex* Edge::getVertex(int nro) 
{
   Vertex* elt = NULL;
   if (nro >=0 && nro < V_TWO  && el_status == HOK
               && e_vertex [nro]->isValid())
      elt = e_vertex [nro]; 

   return elt;
}
// =============================================================== index
inline int Edge::index (Vertex* node)
{
   return  node == NULL ? NOTHING 
         : node == e_vertex[V_AMONT] ? V_AMONT
         : node == e_vertex[V_AVAL ] ? V_AVAL : NOTHING; 
}
// ============================================================= commonVertex
inline Vertex* Edge::commonVertex (Edge* other)
{
   int n1;
   int nro = inter (other, n1);
   return nro<0 ? NULL : e_vertex[nro];
}
// =============================================================== inter
inline int Edge::inter (Edge* other)
{
   int nro;
   return inter (other, nro);
}
// =============================================================== inter
inline int Edge::inter (Edge* other, int& nro)
{
   for (int ni=0 ; ni<V_TWO ; ni++) 
        for (int nj=0 ; nj<V_TWO ; nj++) 
            if (e_vertex[ni] == other->e_vertex[nj])
               {
               nro =  nj;
               return ni;
               }
 
   nro =  NOTHING;
   return NOTHING;
}
// =============================================================== definedBy
inline bool Edge::definedBy  (Vertex* v1, Vertex* v2)
{
   bool   rep =    (v1 == e_vertex[V_AMONT] && v2 == e_vertex[V_AVAL ])
                || (v1 == e_vertex[V_AVAL ] && v2 == e_vertex[V_AMONT]);
   return rep;
}
// =============================================================== setScalar
inline void Edge::setScalar  (double val)
{
   e_vertex [V_AMONT]->setScalar (val);
   e_vertex [V_AVAL ]->setScalar (val);
}
END_NAMESPACE_HEXA
#endif

