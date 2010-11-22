
// class : Les Quadrangles

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

    int   getOrientation (Vertex* v1, Vertex* v2, int &n1, int& n2);
    int   accoupler (Quad* other, StrOrient* orient);
    int   coupler (Quad* other, StrOrient* orient, Elements* table);

    Edge* getOpposEdge (Edge* arete, int &sens);

    virtual void majReferences();            // M.A.J relation "utilise par"
    virtual void dump ();
    virtual void dumpPlus ();
    virtual void saveXml (XmlWriter& xml);
    void setScalar (double valeur);

    virtual void replaceEdge   (Edge*   old, Edge*   nouveau);
    virtual void replaceVertex (Vertex* old, Vertex* nouveau);

    int   addAssociation (Shape* forme); 
    const std::vector <Shape*> & getAssociations () { return tab_assoc ; }

private:
    friend class Cloner;
    Quad* getBrother (StrOrient* orient);
    
private:
    Edge*   q_edge   [QUAD4];
    Vertex* q_vertex [QUAD4];

    std::vector <Shape*> tab_assoc;
};
// ----------------------------------------------- Inlining
// ========================================================== addAssociation
inline int Quad::addAssociation (Shape* forme)
{
   tab_assoc.push_back (forme);
   return HOK;
}
// ============================================================  getEdge
inline Edge* Quad::getEdge (int nro) 
{
   Edge* elt = NULL;
   if (nro >=0 && nro < QUAD4 && el_status == HOK && q_edge [nro]->isValid())
      elt = q_edge [nro]; 

   return elt;
}
// ============================================================  getVertex
inline Vertex* Quad::getVertex (int nro) 
{
   Vertex* elt = NULL;
   if (nro >=0 && nro < QUAD4 && el_status == HOK && q_vertex [nro]->isValid())
      elt = q_vertex [nro]; 

   return elt;
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
inline int Quad::inter (Quad* other, int& nro)
{
   for (int ne1=0 ; ne1<QUAD4 ; ne1++) 
       for (int ne2=0 ; ne2<QUAD4 ; ne2++) 
           if (q_edge [ne1] == other->q_edge [ne2])
              {
              nro  = ne2;
              return ne1;
              }

   nro  = NOTHING;
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

END_NAMESPACE_HEXA
#endif
