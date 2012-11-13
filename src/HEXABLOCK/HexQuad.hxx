
// class : Les Quadrangles

// Copyright (C) 2009-2012  CEA/DEN, EDF R&D
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
#ifndef __QUAD_H
#define __QUAD_H

#include "HexEdge.hxx"

BEGIN_NAMESPACE_HEXA

struct StrOrient
{
   Vertex* v11;     // 1er sommet 1er quad
   Vertex* v12;     // 2eme
   Vertex* v21;     // 1er sommet 2eme quad
   Vertex* v22;     // 2eme
   int dir;         // direction element suivant

StrOrient ( Vertex* n11, Vertex* n12, Vertex* n21, Vertex* n22, int d=0)
    : v11 (n11), v12 (n12), v21 (n21), v22 (n22), dir (d) {}

StrOrient ( StrOrient* s)
    : v11 (s->v11), v12(s->v12), v21(s->v21), v22(s->v22), dir(s->dir){}

};

class Quad : public EltBase
{
public:
    virtual Edge*   getEdge   (int  nro);
    virtual Vertex* getVertex (int  nro);

    virtual int countEdge   () { return QUAD4; } 
    virtual int countVertex () { return QUAD4; } 
 
public:
    Quad (Vertex* va, Vertex* vb, Vertex* vc, Vertex* vd);
    Quad (Edge* ea, Edge* eb, Edge* ec, Edge* ed);
    Quad (Quad* other);

    Hexa* getParent (int nro);

    Edge* findEdge   (Vertex* v1, Vertex*v2);
    Edge* commonEdge (Quad* other);
    int   anaMerge (Vertex* v1, Vertex* v2, Vertex* tv[], Edge* te[]);

    int   ordoVertex (Vertex* v1, Vertex* v2, Vertex* tv[]);
    int   prepaMerge (Vertex* tv1[], Vertex* tv2[], Edge* te1[]);

    int   inter     (Quad* other, int& nro);
    bool  definedBy (Vertex* v1, Vertex* v2);
    bool  definedBy (Edge*   e1,  Edge*   e2);

    int   indexVertex (Vertex* elt);
    int   indexEdge   (Edge*   elt);

    int   accoupler (Quad* other, StrOrient* orient);
    int   coupler (Quad* other, StrOrient* orient, Elements* table);

    Edge*   getOpposEdge   (Edge* arete, int &sens);
    Vertex* getOpposVertex (Vertex* sommet);

    virtual void majReferences();            // M.A.J relation "utilise par"
    virtual void dump ();
    virtual void dumpPlus ();
    virtual void saveXml (XmlWriter* xml);
    void setScalar (double valeur);
    void setColor  (double valeur)          { setScalar (valeur) ; }

    void         replace (Quad* old);
    virtual void replaceEdge   (Edge*   old, Edge*   nouveau);
    virtual void replaceVertex (Vertex* old, Vertex* nouveau);

    virtual void  setAssociation (Shape* forme);
    virtual int   addAssociation (Shape* forme); 
    virtual void  clearAssociation  ()      { tab_assoc.clear() ; }
    virtual bool  isAssociated ()           { return tab_assoc.size() > 0  ; }

    const Shapes& getAssociations ()        { return tab_assoc ; }


    virtual void duplicate ();
    Quad* getClone ()               {  return q_clone ; }

    Edge* getOpposEdge     (Edge* arete);   // Version simplifiee 
    Edge* getPerpendicular (Edge* arete, Vertex* node); 
    int   getOrientation ()            { return q_orientation; }
    int   setOrientation ();
    void  setOrientation (int ori);

private:
    friend class Cloner;
    Quad* getBrother (StrOrient* orient);
    
private:
    Edge*   q_edge   [QUAD4];
    Vertex* q_vertex [QUAD4];
    Quad*   q_clone;
    int     q_orientation; 

    Shapes tab_assoc;
};
// ----------------------------------------------- Inlining
// ========================================================== addAssociation
inline int Quad::addAssociation (Shape* forme)
{
   if (forme != NULL)
       tab_assoc.push_back (forme);
   return HOK;
}
// ======================================================== commonEdge
inline Edge* Quad::commonEdge (Quad* other)
{
   for (int ne1=0 ; ne1<QUAD4 ; ne1++) 
       for (int ne2=0 ; ne2<QUAD4 ; ne2++) 
           if (q_edge [ne1] == other->q_edge [ne2])
              return q_edge [ne1];

   return NULL;
}

// ======================================================== Inter
inline int Quad::inter (Quad* other, int& nother)
{
   for (int ne1=0 ; ne1<QUAD4 ; ne1++) 
       for (int ne2=0 ; ne2<QUAD4 ; ne2++) 
           if (q_edge [ne1] == other->q_edge [ne2])
              {
              nother = ne2;
              return  ne1;
              }

   nother = NOTHING;
   return NOTHING;
}
// ============================================================ definedBy (v)
inline bool Quad::definedBy  (Vertex* v1, Vertex* v2)
{
   for (int n1=0 ; n1< QUAD4 ; n1++)
       if (v1 == q_vertex[n1] && v2 == q_vertex[(n1+2) MODULO QUAD4])
          return true;

   return false;
}
// ============================================================ definedBy (e)
inline bool Quad::definedBy  (Edge* e1, Edge* e2)
{
   for (int n1=0 ; n1< QUAD4 ; n1++)
       if (e1 == q_edge[n1] && e2 == q_edge[(n1+2) MODULO QUAD4])
          return true;

   return false;
}
// =============================================================== findEdge
inline Edge* Quad::findEdge (Vertex* v1, Vertex* v2)
{
   for (int nro=0 ; nro< QUAD4 ; nro++)
       {
       Vertex* va = q_edge[nro]->getVertex(V_AMONT) ;
       Vertex* vb = q_edge[nro]->getVertex(V_AVAL) ;
       if ((v1==va && v2==vb) || (v1==vb && v2==va))
           return q_edge [nro];
       }

   return NULL;
}
// =============================================================== indexVertex
inline int Quad::indexVertex  (Vertex* elt)
{
   for (int n1=0 ; n1< QUAD4 ; n1++)
       if (elt == q_vertex[n1]) 
          return n1;

   return NOTHING;
}
// =============================================================== indexEdge
inline int Quad::indexEdge  (Edge* elt)
{
   for (int n1=0 ; n1< QUAD4 ; n1++)
       if (elt == q_edge[n1]) 
          return n1;

   return NOTHING;
}
// =============================================================== setScalar
inline void Quad::setScalar  (double val)
{
   for (int n1=0 ; n1< QUAD4 ; n1++)
       q_vertex[n1] -> setScalar (val);
}
// =============================================================== duplicate
inline void Quad::duplicate  ()
{
   q_orientation  = Q_UNDEFINED;
   q_clone = new Quad (GetClone (q_edge [E_A]), 
                       GetClone (q_edge [E_B]), 
                       GetClone (q_edge [E_C]), 
                       GetClone (q_edge [E_D]));
   q_clone->tab_assoc = tab_assoc;
}
END_NAMESPACE_HEXA
#endif

