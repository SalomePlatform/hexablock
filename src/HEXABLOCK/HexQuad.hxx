
// class : Les Quadrangles

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
#ifndef __QUAD_H
#define __QUAD_H

#include "HexEdge.hxx"

BEGIN_NAMESPACE_HEXA

#ifndef SWIG
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
#endif

class HexaExport Quad : public EltBase
{
public:
    virtual Edge*   getEdge   (int  nro);
    virtual Vertex* getVertex (int  nro);

    int   addAssociation (NewShape*  forme, int subid);
    virtual void  clearAssociation  ();

    void setColor  (double valeur);
    Vertex* nearestVertex (Vertex* other);
    double* getCenter     (double* center);

    Quad (Vertex* va, Vertex* vb, Vertex* vc, Vertex* vd);

#ifndef SWIG
public:
    Quad (Edge* ea, Edge* eb, Edge* ec, Edge* ed);
    Quad (Quad* other);

    virtual int countEdge   () { return QUAD4; }
    virtual int countVertex () { return QUAD4; }

    Hexa* getParent (int nro);

    Edge* findEdge   (Vertex* v1, Vertex*v2);
    Edge* commonEdge (Quad* other);
    int   anaMerge (Vertex* v1, Vertex* v2, Vertex* tv[], Edge* te[]);

    int   ordoVertex (Vertex* v1, Vertex* v2, Vertex* tv[]);
    int   ordonner   (Vertex* v1, Vertex* v2, Vertex* tv[], Edge* ted[]);

    int   inter     (Quad* other, int& nro);
    Edge* inter     (Quad* other);
    bool  definedBy (Vertex* v1, Vertex* v2);
    bool  definedBy (Edge*   e1,  Edge*   e2);

    int   indexVertex (Vertex* elt);
    int   indexEdge   (Edge*   elt);

    //int   accoupler (Quad* other, StrOrient* orient);
    int   coupler (Quad* other, StrOrient* orient, Elements* table);

    Edge*   getOpposEdge   (Edge* arete, int &sens);
    Vertex* getOpposVertex (Vertex* sommet);
    Hexa*   opposedHexa (Hexa* hexa);

    virtual void majReferences();            // M.A.J relation "utilise par"
    virtual void dump ();
    virtual void dumpPlus ();
    virtual void saveXml (XmlWriter* xml);
    void setScalar (double valeur)              { setColor (valeur); }

    void         replace (Quad* old);
    virtual void replaceEdge   (Edge*   old, Edge*   nouveau);
    virtual void replaceVertex (Vertex* old, Vertex* nouveau);

    virtual void  setAssociation (Shape* forme) {}              // PERIME
    virtual int   addAssociation (Shape* forme) {return HOK ; } // PERIME


    // const Shapes& getAssociations ()        { return tab_shapes ; }


    virtual void duplicate ();
    Quad* getClone ()               {  return q_clone ; }

    Edge* getOpposEdge     (Edge* arete);   // Version simplifiee
    Edge* getPerpendicular (Edge* arete, Vertex* node);
    int   getOrientation ()            { return q_orientation; }
    int   setOrientation ();
    void  setOrientation (int ori);
    void  reorienter ();
                                                      // Hexa5
    int   addAssociation (FaceShape* forme);
    int   countAssociation ()                     { return tab_assoc.size () ; }
    FaceShape* getAssociation (int nro);

    double  dist2         (double* point);

private:
    friend class Cloner;
    Quad* getBrother (StrOrient* orient);

private:
    Edge*   q_edge   [QUAD4];
    Vertex* q_vertex [QUAD4];
    Quad*   q_clone;
    int     q_orientation;

    FaceShapes tab_assoc;
#endif
};
END_NAMESPACE_HEXA
#endif

