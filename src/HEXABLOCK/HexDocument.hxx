
// class : Document

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
//  See http://www.salome-platform.org/ 
//  or email : webmaster.salome@opencascade.com

#ifndef __DOCUMENT_H_
#define __DOCUMENT_H_

#include "hexa_base.hxx"

BEGIN_NAMESPACE_HEXA

class Document
{
                                   // Fonctions utilisateur
public :
                                   // Fonctions globales
   const char* getFile ()              { return doc_name.c_str() ; }
   bool        isSaved ()              { return doc_saved ; } 
   int setFile (const char* filename)  { doc_name = filename ; return HOK ; }
   int saveFile ();

   int  getLevel ()                    { return doc_db ; }
   void setLevel (int niv)             { doc_db = niv  ; }

   Hexa*   getHexa   (int nro);
   Quad*   getQuad   (int nro);
   Edge*   getEdge   (int nro);
   Vertex* getVertex (int nro);

   int countHexa   ()   { return countElement (EL_HEXA); }
   int countQuad   ()   { return countElement (EL_QUAD); }
   int countEdge   ()   { return countElement (EL_EDGE); }
   int countVertex ()   { return countElement (EL_VERTEX); }

                                   // Creation d'elements
   Vertex* addVertex (double x=0.0, double y=0.0, double z=0.0);
   Edge*   addEdge   (Vertex* va, Vertex* vb);
   Edge*   addEdge   (Vertex* va, Vector* vec);

   Quad*   addQuadVertices   (Vertex* v1, Vertex* v2, Vertex* v3, Vertex* v4);
   Quad*   addQuad   (Edge* v1, Edge* v2, Edge* v3, Edge* v4);

   Hexa*   addHexaVertices   (Vertex* v1, Vertex* v2, Vertex* v3, Vertex* v4, 
                             Vertex* v5, Vertex* v6, Vertex* v7, Vertex* v8);
   Hexa*   addHexa (Quad* qa, Quad* qb, Quad* qc, Quad* qd, Quad* qe, Quad* qf);

   Vector* addVector (double dx=0.0, double dy=0.0, double dz=0.0);
   Vector* addVectorVertices (Vertex* va, Vertex* vb);

   Elements* makeCartesian   (Vertex* v, Vector* v1, 
                       int px, int py, int pz, int mx=0, int my=0, int mz=0);
   Elements* makeCartesian   (Vertex* v, Vector* v1, Vector* v2, Vector* v3, 
                       int px, int py, int pz, int mx=0, int my=0, int mz=0);

   Elements* makeCylindrical (Vertex* c, Vector* b, Vector* h, double dr, 
             double da, double dl, int nr, int na, int nl, bool fill=false);

                                                      // Obsolete
   Elements* makeSpherical (Vertex* v, Vector* dv, int nb, double k=1) {return NULL;}
   Elements* makeSpherical (Vertex* center, double rayon, int nb, double k=1);

   Elements* prismQuad  (Quad*  start, Vector* dv, int nb);
   Elements* prismQuads (Quads& start, Vector* dv, int nb);

   Elements* joinQuad  (Quad*  start, Quad* dest, Vertex* v1, Vertex* v2,  
                                      Vertex* v3, Vertex* v4, int nb);
   Elements* joinQuads (Quads& start, Quad* dest, Vertex* v1, Vertex* v2, 
                                      Vertex* v3, Vertex* v4, int nb);

   Cylinder* addCylinder   (Vertex* b, Vector* d, double r,  double h);
   Elements* makeCylinder  (Cylinder* cyl, Vector* vx, int nr, int na, int nl);
   CrossElements* makeCylinders (Cylinder* cyl1, Cylinder* cyl2);

   Pipe*     addPipe   (Vertex* b, Vector* d, double ri, double re, double h);
   Elements* makePipe  (Pipe* pip, Vector* vx, int nr, int na, int nl);
   CrossElements* makePipes (Pipe* pipe1, Pipe* pipe2);

   int     removeHexa (Hexa* maille);
   int     removeQuad (Quad* maille);
   int     removeConnectedHexa (Hexa* maille);
   int     removeElements  (Elements* bloc);

   Vertex* findVertex (double  vx, double  vy, double vz);
   Edge*   findEdge   (Vertex* va, Vertex* vb);
   Quad*   findQuad   (Vertex* va, Vertex* vb);
   Hexa*   findHexa   (Vertex* va, Vertex* vb);

   int     mergeVertices (Vertex* v1, Vertex* v2);
   int     mergeEdges    (Edge* e1, Edge* e2, Vertex* v1, Vertex* v2);
   int     mergeQuads    (Quad* q1, Quad* q2, Vertex* v1, Vertex* v2, 
                                              Vertex* v3, Vertex* v4);
   int     closeQuads    (Quad* q1, Quad* q2);

   Elements* makeTranslation   (Elements* elts, Vector* trans);
   Elements* makeScale         (Elements* elts, Vertex* ver, double k);
   Elements* makeRotation      (Elements* elts, Vertex* ver, Vector* vec, 
                                double angle);
   Elements* makeSymmetryPoint (Elements* elts, Vertex* ver);
   Elements* makeSymmetryLine  (Elements* elts, Vertex* ver, Vector* vec);
   Elements* makeSymmetryPlane (Elements* elts, Vertex* ver, Vector* vec);

   int performTranslation (Elements* elts, Vector* trans);
   int performScale    (Elements* elts, Vertex* ver, double k);
   int performRotation (Elements* elts, Vertex* ver, Vector* vec, double angle);
   int performSymmetryPoint (Elements* elts, Vertex* ver);
   int performSymmetryLine  (Elements* elts, Vertex* ver, Vector* vec);
   int performSymmetryPlane (Elements* elts, Vertex* ver, Vector* vec);

   void   setTolerance (double tol) { doc_tolerance = std::max (tol, 0.0); }
   double getTolerance ()           { return doc_tolerance; }

   Elements* disconnectQuad   (Hexa* maille, Quad*   face);
   Elements* disconnectEdge   (Hexa* maille, Edge*   arete);
   Elements* disconnectVertex (Hexa* maille, Vertex* noeud);

   Elements* cut (Edge* edge, int nbcuts);

   Group* addGroup    (cpchar name, EnumGroup kind);
   Group* getGroup    (int nro)        { return doc_group [nro];  }
   Group* findGroup   (cpchar name);
   int    removeGroup (Group* grp);
   int    countGroup  ()               { return (int) doc_group.size(); }

   // ---------------------------------------------------

   Law* addLaw    (const char* name, int nbnodes);
   Law* addLaw    (Law* law); // lo-add-lololo
   int  countLaw  ()                            { return nbr_laws ; }
   Law* getLaw    (int nro);
   Law* findLaw   (const char* name);
   int  removeLaw (Law* loi);

   // ---------------------------------------------------
   int          countPropagation ();
   Propagation* getPropagation   (int nro);
   Propagation* findPropagation  (Edge* arete);
   void         dumpPropagation  ();

   int countVector ()              { return doc_vector.size(); }
   int countCylinder ()            { return doc_cylinder.size(); }
   int countPipe ()                { return doc_pipe.size(); }

   Vector*   getVector   (int nro)   { return doc_vector [nro]; }
   Cylinder* getCylinder (int nro)   { return doc_cylinder [nro]; }
   Pipe*     getPipe     (int nro)   { return doc_pipe [nro]; }

   void purge ();
   int  associateOpenedLine (Edge*  mstart, Edges&  mline, Shape* gstart, 
                             double pstart, Shapes& gline, double pend);
   int  associateClosedLine (Vertex* mfirst, Edge*  mstart, Edges&  mline, 
                             Shape*  gstart, double pstart, Shapes& gline);

   void   setShape (Shape* forme)           { doc_shape = forme ; }
   Shape* getShape ()                       { return doc_shape  ; }

   // --------------------------------------------------- Evols Hexa3

   Document* copyDocument ();

   int countUsedHexa   ();
   int countUsedQuad   ();
   int countUsedEdge   ();
   int countUsedVertex ();

   Hexa*   getUsedHexa   (int nro);
   Quad*   getUsedQuad   (int nro);
   Edge*   getUsedEdge   (int nro);
   Vertex* getUsedVertex (int nro);

   Hexa* addHexa2Quads (Quad* q1, Quad* q2);
   Hexa* addHexa3Quads (Quad* q1, Quad* q2, Quad* q3);
   Hexa* addHexa4Quads (Quad* q1, Quad* q2, Quad* q3, Quad* q4);
   Hexa* addHexa5Quads (Quad* q1, Quad* q2, Quad* q3, Quad* q4, Quad* q5);

   Elements* revolutionQuads (Quads& start, Vertex* center, Vector* axis, 
                              RealVector &angles);

   Elements* makeSphere (Vertex* center, Vector* vx, Vector* vz, 
                         double radius, double radhole,
                         Vertex* plorig, 
                         int nrad, int nang, int nhaut);

   Elements* makePartSphere (Vertex* center, Vector* vx, Vector* vz, 
                             double  radius, double radhole,
                             Vertex* plorig, double angle,
                             int nrad, int nang, int nhaut);

   Elements* makeRind (Vertex* center, Vector* vx, Vector* vz, 
                       double  radext, double radint, double radhole,
                       Vertex* plorig, 
                       int nrad, int nang, int nhaut);

   Elements* makePartRind (Vertex* center, Vector* vx, Vector* vz, 
                           double  radext, double radint, double radhole,
                           Vertex* plorig, double angle,
                           int nrad, int nang, int nhaut);

   Elements* replace (Quads& pattern, Vertex* p1, Vertex* c1, 
                      Vertex* p2, Vertex* c2,  Vertex* p3, Vertex* c3);

   Elements* makeCylindricals (Vertex* c, Vector* b, Vector* h, 
             RealVector& tdr, RealVector& tda, RealVector& tdh, 
             bool fill=false);

public:
    Document (cpchar filename);
   ~Document ();

    int    loadXml ();
    int    setXml (cpchar flux);
    cpchar getXml ();

    EltBase* getFirstEltBase (EnumElt type) { return doc_first_elt [type]; }
    EltBase* getLastEltBase  (EnumElt type) { return doc_last_elt  [type]; }
    void     setDeprecated (int level=1);

    void dump ();
    void markAll (int marque, int type=-1);
    int  saveVtk  (cpchar nomfic); 
    int  saveVtk  (cpchar radical, int &nro); 

    void  putError  (cpchar mess, cpchar info1=NULL, cpchar info2=NULL);
    void  nputError (cpchar mess, int info1,  cpchar info2=NULL);
    void  hputError (cpchar mess, EltBase* e1, EltBase* e2=NULL);

    void  majReferences  ();                  // M.A.J relation "utilise par"
    void  update ()            { if (maj_connection) majReferences (); }
    bool  debug (int niv=0)    { return doc_db > niv ; }
    void  reorderFaces ();

public:
   Globale* glob;

private :
   int   genXml   (cpchar filename);
   int   parseXml (XmlTree& xml);

   void  replaceVertex (Vertex* v1, Vertex* v2);
   void  replaceEdge   (Edge*   e1, Edge* e2);
   void  replaceQuad   (Quad*   q1, Quad* q2);

   int   countElement  (EnumElt type);
   EltBase* getElement (EnumElt type, int nro);

   void  majPropagation ();
   void  renumeroter ();

   Elements* clonerElements (Elements* table, Matrix* matrice);
   int associateCascade (Edges& mline, int msens[], Shape* gstart, 
                         Shapes& gline, double pstart, double pend, bool clos);
   int associateLine (Vertex* mfirst, Edge*  mstart, Edges& mline, 
                   Shape* gstart, double pstart, Shapes& gline, double pend);

   // --------------------------------------------------- Evols Hexa3
   Hexa* addHexaQuadsAB   (AnaQuads& strquads);
   Hexa* addHexaQuadsAC   (AnaQuads& strquads);
   Hexa* addHexaQuadsACE  (AnaQuads& strquads);
   Hexa* addHexaQuadsACD  (AnaQuads& strquads);
   Hexa* addHexaQuadsABCD (AnaQuads& strquads);
   Hexa* addHexaQuadsACDE (AnaQuads& strquads);

   Shape* parseShape (XmlTree* node);

private :
   friend class EltBase;

   bool doc_modified; 
   bool count_modified; 
   bool doc_saved; 
   bool maj_connection;
   bool purge_elements;
   int  doc_db;
   int  nbr_errors;

   std::string doc_name; 
   EltBase* doc_first_elt [EL_MAXI];
   EltBase* doc_last_elt  [EL_MAXI];
   int      doc_nbr_elt   [EL_MAXI];
   double   doc_tolerance;

   std::vector <Law*> doc_laws;
   int                nbr_laws;

   int  nbr_propagations;
   bool maj_propagation;

   std::vector <Propagation*> doc_propagation;
   std::vector <Group*>       doc_group;

   std::vector <Vector*>      doc_vector;
   std::vector <Cylinder*>    doc_cylinder;
   std::vector <Pipe*>        doc_pipe;
   XmlWriter*                 doc_xml; 

   // --------------------------------------------------- HexaBlock v3

   Shape*                     doc_shape;

   std::vector <Hexa*>   doc_used_hexas;
   std::vector <Quad*>   doc_used_quads;
   std::vector <Edge*>   doc_used_edges;
   std::vector <Vertex*> doc_used_vertex;

   int nbr_used_hexas;
   int nbr_used_quads;
   int nbr_used_edges;
   int nbr_used_vertex;
};
// ========================================================= saveVtk (avec nro)
inline int Document::saveVtk  (cpchar radical, int &nro)
{
   char num[8];
   sprintf (num, "%d", nro);
   nro ++;

   string filename = radical;
   filename += num;
   filename += ".vtk";
   int ier = saveVtk (filename.c_str());
   return ier;
}
// ========================================================= setDeprecated
inline void Document::setDeprecated (int level)
{
   switch (level)
          {                             
                         // En cas de destruction  : parents invalides
          case 2 : maj_connection  = true;
                   purge_elements  = true;
                         // creation + destruction : propagations a revoir
          case 1 : maj_propagation = true;
                         // Par defaut : 
          default: doc_modified    = true;
                   count_modified  = true;
          }
}

END_NAMESPACE_HEXA
#endif
