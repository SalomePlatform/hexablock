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
    void         dumpAsso ();
    virtual void saveXml (XmlWriter* xml);
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

    void setScalar (double valeur);
    void setColor  (double valeur)          { setScalar (valeur) ; }
    virtual void replaceVertex (Vertex* old, Vertex* nouveau);

    virtual void  clearAssociation  ()      { tab_assoc.clear() ; }
    int   addAssociation (Shape* forme); 
    const Shapes & getAssociations ()       { return tab_assoc ;  }

    virtual void duplicate ();
    Edge*   getClone ()                     {  return e_clone ;   }
    double* getVector (double vecteur[]);

private:
    friend class Cloner;
    Vertex* e_vertex [V_TWO];
    Edge*   e_clone;
    int     e_propag;
    bool    e_way;     // Sens de propagation
    Law*    e_law;     // Le soleil brille, brille, brille

    Shapes tab_assoc;
};

// ----------------------------------------------- Inlining
// ========================================================== addAssociation
inline int Edge::addAssociation (Shape* forme)
{
   if (forme != NULL)
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
// ============================================================= opposedVertex
inline Vertex* Edge::opposedVertex (Vertex* sommet)
{
   int nro = index (sommet);
   return nro<0 ? NULL : e_vertex[1-nro];
}
// ============================================================= commonVertex
inline Vertex* Edge::commonVertex (Edge* other)
{
   int nro = inter (other);
   return nro<0 ? NULL : e_vertex[nro];
}
// ============================================================= commonPoint
inline double* Edge::commonPoint (Edge* other, double point[])
{
   Vertex* commun = commonVertex (other);
   if (commun==NULL)
      {
      point[dir_x] = point[dir_y] = point[dir_z] =  0;
      return NULL;
      }

   commun->getPoint (point);
   return point;
}
// =============================================================== inter
inline int Edge::inter (Edge* other)
{
   int nro;
   return inter (other, nro);
}
// =============================================================== inter
inline int Edge::inter (Edge* other, int& nother)
{
   for (int ni=0 ; ni<V_TWO ; ni++) 
        for (int nj=0 ; nj<V_TWO ; nj++) 
            if (e_vertex[ni] == other->e_vertex[nj])
               {
               nother =  nj;
               return ni;
               }
 
   nother = NOTHING;
   return   NOTHING;
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
// =============================================================== duplicate
inline void Edge::duplicate  ()
{
   e_clone = new Edge (GetClone (e_vertex [V_AMONT]), 
                       GetClone (e_vertex [V_AVAL ]));

   e_clone->tab_assoc = tab_assoc;
}
// =============================================================== getVector
inline double* Edge::getVector (double vecteur[])
{

   if (e_vertex[V_AMONT]==NULL ||  e_vertex[V_AVAL]==NULL)
      {
      vecteur [dir_x] = vecteur [dir_y] = vecteur [dir_z] = 0;
      return NULL;
      }

   vecteur[dir_x] = e_vertex[V_AVAL]->getX() - e_vertex[V_AMONT]->getX();
   vecteur[dir_y] = e_vertex[V_AVAL]->getY() - e_vertex[V_AMONT]->getY();
   vecteur[dir_z] = e_vertex[V_AVAL]->getZ() - e_vertex[V_AMONT]->getZ();

   return vecteur;
}
END_NAMESPACE_HEXA
#endif

