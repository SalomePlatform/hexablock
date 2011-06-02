// Copyright (C) 2009-2011  CEA/DEN, EDF R&D
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


// class : Les hexaedres
/* -----------------------------------------------------

                // ---- Numerotation des faces (%x) 

                   
              +----bf-----+
             /|          /|
           bc |   B    bd |
           /  |        /  |
          +----be-----+   |   
          |  cf     F |   df
          | C |       | D |             z
         ce   | E     de  |             ^
          |   +----af-|---+             |   y
          |  /        |  /              |  /
          | ac    A   | ad              | /
          |/          |/                |/
          +----ae-----+                 +----->  x
                

 * ----------------------------------------------------- */
#ifndef __HEX_HEXA_H_
#define __HEX_HEXA_H_

#include "HexEltBase.hxx"

#include "HexQuad.hxx"
#include "HexEdge.hxx"
#include "HexVertex.hxx"

BEGIN_NAMESPACE_HEXA

class Hexa : public EltBase 
{
public:
    virtual Quad*   getQuad   (int  nro);
    virtual Edge*   getEdge   (int  nro);
    virtual Vertex* getVertex (int  nro);

    virtual int     countQuad   () { return HQ_MAXI; } 
    virtual int     countEdge   () { return HE_MAXI; } 
    virtual int     countVertex () { return HV_MAXI; } 

public:
    Hexa (Vertex* v1, Vertex* v2, Vertex* v3, Vertex* v4, 
          Vertex* v5, Vertex* v6, Vertex* v7, Vertex* v8);
    Hexa (Quad* qa, Quad* qb, Quad* qc, Quad* qd, Quad* qe, Quad* qf);
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
    virtual void majReferences();            // M.A.J relation "utilise par"

    bool hasFreeEdges  ();
    void propager (Propagation* prop, int nro);
    void setScalar (double valeur);

    virtual void saveXml (XmlWriter* xml);

    Elements*  disconnectQuad   (Quad*   face);
    Elements*  disconnectEdge   (Edge*   arete);
    Elements*  disconnectVertex (Vertex* noeud);

    int getBase (Vertex* orig, Edge* normale);

    void replaceQuad   (Quad*   old, Quad* nouveau);
    void replaceEdge   (Edge*   old, Edge* nouveau);
    void replaceVertex (Vertex* old, Vertex* nouveau);
    void removeConnected ();
    void markElements (int marque);

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

    int   findQuad   (Quad*   element);
    int   findEdge   (Edge*   element);
    int   findVertex (Vertex* element);

    int   findOpposedQuad  (Quad* face, Edge* arete);

private:
    Quad*   h_quad   [HQ_MAXI];
    Edge*   h_edge   [HE_MAXI];
    Vertex* h_vertex [HV_MAXI];
};
// ------------------------------------------------------------  inlining
// ============================================================  getQuad
inline Quad* Hexa::getQuad (int nro) 
{
   Quad* elt = NULL;
   if (nro >=0 && nro < HQ_MAXI && el_status == HOK && h_quad [nro]->isValid())
      elt = h_quad [nro]; 

   return elt;
}
// ============================================================  getEdge
inline Edge* Hexa::getEdge (int nro)
{
   Edge* elt = NULL;
   if (nro >=0 && nro < HE_MAXI && el_status == HOK && h_edge [nro]->isValid())
      elt = h_edge [nro]; 

   return elt;
}
// ============================================================  getVertex
inline Vertex* Hexa::getVertex (int nro)
{
   Vertex* elt = NULL;
   if (nro >=0 && nro <  HV_MAXI && el_status == HOK && h_vertex [nro]->isValid())
      elt = h_vertex [nro]; 

   return elt;
}
// =============================================================== definedBy
inline bool Hexa::definedBy  (Vertex* v1, Vertex* v2)
{
   for (int n1=0 ; n1< HV_MAXI ; n1++)
       {
//              (   Diagonale        )  Dessus
       int n2 = (n1 + 2) MODULO HV_MAXI + HV_MAXI;
       if (   (v1 == h_vertex[n1] && v2 == h_vertex[n2])
           || (v1 == h_vertex[n2] && v2 == h_vertex[n1])) return true;
       }
   return false;
}
// =============================================================== definedBy
inline bool Hexa::definedBy  (Quad* qa, Quad* qb)
{
   for (int nc=0 ; nc< 3 ; nc++)
       {
       if (   (qa == h_quad[2*nc]   && qb == h_quad[2*nc+1])
           || (qa == h_quad[2*nc+1] && qb == h_quad[2*nc])) return true;
       }
   return false;
}
// =============================================================== setScalar
inline void Hexa::setScalar  (double val)
{
   for (int nc=0 ; nc< HV_MAXI ; nc++)
       h_vertex[nc] -> setScalar (val);
}
// ============================================================== markElements
inline void Hexa::markElements  (int marque)
{
   for (int nc=0 ; nc< HQ_MAXI ; nc++) h_quad  [nc] -> setMark (marque);
   for (int nc=0 ; nc< HE_MAXI ; nc++) h_edge  [nc] -> setMark (marque);
   for (int nc=0 ; nc< HV_MAXI ; nc++) h_vertex[nc] -> setMark (marque);
}

END_NAMESPACE_HEXA
#endif
