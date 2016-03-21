
// class : Les hexaedres

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

/* -----------------------------------------------------

                // ---- Numerotation des faces (%x)

       6=bed  +----bd-----+ bdf=7
             /|          /|
           be |   B    bf |
           /  |        /  |
    4=bce +----bc-----+...|...bcf=5
          |  de     D |   df
          | E |       | F |             z
         ce   | C     cf  |             ^
  2=ade...|...+----ad-|---+ adf=3       |   y
          |  /        |  /              |  /
          | ae    A   | af              | /
          |/          |/                |/
    0=ace +----ac-----+ acf=1           +----->  x

 * ----------------------------------------------------- */
#ifndef __HEX_HEXA_H_
#define __HEX_HEXA_H_

#include "HexEltBase.hxx"

#include "HexQuad.hxx"
#include "HexEdge.hxx"
#include "HexVertex.hxx"

BEGIN_NAMESPACE_HEXA

class HexaExport Hexa : public EltBase
{
public:
    virtual Quad*   getQuad   (int  nro);
    virtual Edge*   getEdge   (int  nro);
    virtual Vertex* getVertex (int  nro);

    void    setColor  (double valeur);

    Hexa (Quad* qa, Quad* qb, Quad* qc, Quad* qd, Quad* qe, Quad* qf);

#ifndef SWIG
public:

    Hexa (Vertex* v1, Vertex* v2, Vertex* v3, Vertex* v4,
          Vertex* v5, Vertex* v6, Vertex* v7, Vertex* v8);

    virtual int     countQuad   () { return HQ_MAXI; }
    virtual int     countEdge   () { return HE_MAXI; }
    virtual int     countVertex () { return HV_MAXI; }

    Hexa (Hexa* other);
    Quad* Inter   (Hexa* other);
    bool  definedBy  (Vertex* v1, Vertex* v2);
    bool  definedBy  (Quad*   va, Quad*   v2);
    virtual void dump ();
    virtual void dumpPlus ();
    virtual void dumpFull ();

    void razNodes ();
    int  countNodes ();                      // Compte et numerote les noeuds
    void printNodes (pfile vtk, int& count); // Impression format vtk
    void colorNodes (pfile vtk);             // Impression scalaires vtk
    void moveNodes  (Matrix* matrice);       // transfo controlee ds un ensemble
    void transform  (Matrix* matrice);       // transfo indivituelle
    void printHexa  (pfile vtk);             // Impression de la cellule
    void printHexaVtk (pfile vtk);           // Impression de la cellule
    virtual void majReferences();            // M.A.J relation "utilise par"

    bool hasFreeEdges  ();
    void propager (Propagation* prop, int nro);
    void setScalar (double valeur)          { setColor (valeur) ; }

    virtual void saveXml (XmlWriter* xml);

    Elements*  disconnectQuad   (Quad*   face);
    Elements*  disconnectEdge   (Edge*   arete);
    Elements*  disconnectVertex (Vertex* noeud);

    int  disconnectQuad   (Quad*   face,  Elements* dest);
    int  disconnectEdge   (Edge*   arete, Elements* dest);
    int  disconnectVertex (Vertex* noeud, Elements* dest);

    int getBase (Vertex* orig, Edge* normale);

    void replaceQuad   (Quad*   old, Quad* nouveau);
    void replaceEdge   (Edge*   old, Edge* nouveau);
    void replaceVertex (Vertex* old, Vertex* nouveau);
    void removeConnected ();
    void markElements (int marque);
    double* getCenter    (double centre[]);

    virtual void   duplicate ();
    Hexa* getClone ()               {  return h_clone ; }
    Quad*   getOpposedQuad   (Quad* face);
    Vertex* opposedVertex     (Quad* face, Vertex* vertex);
    Edge*   perpendicularEdge (Quad* face, Vertex* vertex);
    Quad*   perpendicularQuad (Quad* face, Edge* edge);
    Edge*   getPerpendicularEdge (Quad* face, Vertex* vertex)
          { return perpendicularEdge (face, vertex); }
    // Edge* getParallelEdge      (Quad* face, Edge*   edge);
    Quad* findQuad             (Edge* e1, Edge* e2);
    Edge* findEdge             (Vertex* v1, Vertex* v2);

    int   findQuad   (Quad*   element);
    int   findEdge   (Edge*   element);
    int   findVertex (Vertex* element);


private:
    friend class Cloner;
    void  OrdonnerAretes ();    // obsolete ?
    void  OrdonnerSommets ();   // obsolete ?

    void  controlerFaces   ();  // Controle primaire des donnees utilisateur
    void  controlerSommets ();  // Idem;

    void  verifierSommets ();   // Controle de coherence des quads
    void  verifierAretes ();    // Idem

    void  controlerArete (int arete, int face1, int face2);
    void  controlerSommet (int node, int arete1, int arete2, int arete3);

    int   findOpposedQuad (Quad* face, Edge* arete);

private:
    Quad*   h_quad   [HQ_MAXI];
    Edge*   h_edge   [HE_MAXI];
    Vertex* h_vertex [HV_MAXI];
    Hexa*   h_clone;
#endif
};
END_NAMESPACE_HEXA
#endif

