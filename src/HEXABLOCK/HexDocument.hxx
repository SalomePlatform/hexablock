
// class : Document

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

#ifndef __DOCUMENT_H_
#define __DOCUMENT_H_

#include "Hex_defines.hxx"
#include "HexEltBase.hxx"

#ifdef WIN32
 #ifdef max
  #undef max
#endif
#endif


BEGIN_NAMESPACE_HEXA

class NewShape;

class HexaExport Document : public EltBase
{
                                   // Fonctions utilisateur
public :
   Document (cpchar name, Hex* dad=NULL);
   void dump ();
   int  setName  (const char* name);
   void setLevel (int niv);
   int  save         (const char* ficxml); // Genere le xml dans un seul fichier
   int  saveVtk      (cpchar nomfic);

   void purge ();

   void   setTolerance (double tol) { doc_tolerance = std::max (tol, 0.0); }
   double getTolerance ()           { return doc_tolerance; }

                                   // Creation d'elements
   Vertex* addVertex (double x=0.0, double y=0.0, double z=0.0);

   Edge* addEdge       (Vertex* va, Vertex* vb);
   Edge* addEdgeVector (Vertex* va, Vector* vec);

   Quad* addQuad   (Edge* v1, Edge* v2, Edge* v3, Edge* v4);
   Quad* addQuadVertices   (Vertex* v1, Vertex* v2, Vertex* v3, Vertex* v4);

   Hexa* addHexa (Quad* qa, Quad* qb, Quad* qc, Quad* qd, Quad* qe, Quad* qf);
   Hexa* addHexaVertices   (Vertex* v1, Vertex* v2, Vertex* v3, Vertex* v4,
                             Vertex* v5, Vertex* v6, Vertex* v7, Vertex* v8);

   Hexa* addHexa2Quads (Quad* q1, Quad* q2);
   Hexa* addHexa3Quads (Quad* q1, Quad* q2, Quad* q3);
   Hexa* addHexa4Quads (Quad* q1, Quad* q2, Quad* q3, Quad* q4);
   Hexa* addHexa5Quads (Quad* q1, Quad* q2, Quad* q3, Quad* q4, Quad* q5);

   Vector*   addVector (double dx=0.0, double dy=0.0, double dz=0.0);
   Vector*   addVectorVertices (Vertex* va, Vertex* vb);
   Law*      addLaw    (const char* name, int nbnodes);
   int       addLaws   (double lgmoy, bool usemax=true);

   Group*    addHexaGroup       (cpchar name);
   Group*    addQuadGroup       (cpchar name);
   Group*    addQuadNodeGroup   (cpchar name);
   Group*    addHexaNodeGroup   (cpchar name);
   Group*    addEdgeGroup       (cpchar name);
   Group*    addEdgeNodeGroup   (cpchar name);
   Group*    addVertexNodeGroup (cpchar name);


                                 // ----------------- Transformations
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

                                 // ----------------- Modifications
   Elements* disconnectQuad   (Hexa* maille, Quad*   face);
   Elements* disconnectEdge   (Hexa* maille, Edge*   arete);
   Elements* disconnectVertex (Hexa* maille, Vertex* noeud);
   Elements* disconnectEdges  (Hexas thexas, Edges   edges);

   Elements* replace (Quads pattern, Quads cible, Vertex* p1, Vertex* c1,
                                                  Vertex* p2, Vertex* c2);

   int     mergeVertices (Vertex* v1, Vertex* v2);
   int     mergeEdges    (Edge* e1, Edge* e2, Vertex* v1, Vertex* v2);
   int     mergeQuads    (Quad* q1, Quad* q2, Vertex* v1, Vertex* v2,
                                              Vertex* v3, Vertex* v4);

                                 // ----------------- Associations
   void clearAssociation ();
   int  associateOpenedLine (Edges  mline, NewShapes gline, IntVector tabid,
                             double pstart, double pend);

   int  associateClosedLine (Vertex* mfirst,   Edges  mline,
                             NewShapes gline, IntVector tabid,
                             double pstart, bool inv);
                                 // ----------------- count
   int countHexa   ();
   int countQuad   ();
   int countEdge   ();
   int countVertex ();
   int countVector ()               { return doc_vector.size(); }

   int countGroup  ()               { return (int) doc_group.size(); }
   int countLaw  ()                 { return nbr_laws ; }
   int countPropagation ();
   int countShape ()                { return doc_tab_shape.size() ; }

   int countUsedHexa   ();
   int countUsedQuad   ();
   int countUsedEdge   ();
   int countUsedVertex ();

                                 // ----------------- get par indice
   Hexa*   getHexa   (int nro);
   Quad*   getQuad   (int nro);
   Edge*   getEdge   (int nro);
   Vertex* getVertex (int nro);

   Hexa*   getUsedHexa   (int nro);
   Quad*   getUsedQuad   (int nro);
   Edge*   getUsedEdge   (int nro);
   Vertex* getUsedVertex (int nro);

   Vector*      getVector   (int nro);
   NewShape*    getShape    (int nro);
   Group*       getGroup    (int nro);
   Law*         getLaw      (int nro);
   Propagation* getPropagation   (int nro);
   cpchar       getFirstExplicitShape();

                                 // ----------------- find
   Vertex* findVertex (double  vx, double  vy, double vz);
   Edge*   findEdge   (Vertex* va, Vertex* vb);
   Quad*   findQuad   (Vertex* va, Vertex* vb);
   Quad*   findQuad   (Edge*   ea, Edge*   eb);
   Hexa*   findHexa   (Vertex* va, Vertex* vb);

   Group*       findGroup   (cpchar name);
   Law*         findLaw     (cpchar name);
   Propagation* findPropagation  (Edge* arete);

                                 // ----------------- Suppressions
   int  removeHexa          (Hexa* maille);
   int  removeQuad          (Quad* maille);
   int  removeConnectedHexa (Hexa* maille);
   int  removeElements      (Elements* bloc);
   int  removeGroup         (Group* grp);
   int  removeLaw           (Law*   lau);

public :
                                 // ----------------- Creations Hexav6
   Elements* makeCartesianTop (int nx, int ny, int nz);
   Elements* makeCartesianUni (Vertex* ori, Vector* vx, Vector* vy, Vector* vz,
                               double lx, double ly, double lz, 
                               int    nx, int    ny, int    nz);
   Elements* makeCartesian    (Vertex* ori, Vector* vx, Vector* vy, Vector* vz,
                               RealVector tlx, RealVector tly, RealVector tlz);

   Elements* makeCylinderTop (int nr, int na, int nh);
   Elements* makeCylinderUni (Vertex* orig, Vector* vx, Vector* vz, 
                              double rint, double rext, double ang, double haut,
                              int nr, int na, int nh);
   Elements* makeCylinder (Vertex* orig, Vector* vx, Vector* vz, 
                           RealVector tray, RealVector tang, RealVector thaut);

   Elements* makePipeTop (int nr, int na, int nh);
   Elements* makePipeUni (Vertex* orig, Vector* vx, Vector* vz, 
                          double rint, double rext, double angle, double haut,
                          int nr, int na, int nh);
   Elements* makePipe    (Vertex* orig, Vector* vx, Vector* vz, 
                          RealVector tray, RealVector tang, RealVector thaut);

   Elements* makeSphericalTop (int nbre, int crit=0);
   Elements* makeSphericalUni (Vertex* centre, Vector* vx, Vector* vz, 
                               double rayon, int nbre, int crit=0);
   Elements* makeSpherical    (Vertex* centre, Vector* vx, Vector* vz, 
                               RealVector rayon, int crit=0);
   Elements* makeSphereTop (int nr, int na, int nh);
   Elements* makeSphereUni (Vertex* centre, Vector* vx, Vector* vz, 
                            double rtrou, double rext, double ang, 
                            Vertex* vplan, int nr, int na, int nh);
   Elements* makeSphere    (Vertex* centre, Vector* vx, Vector* vz, 
                            RealVector tray, RealVector tang, RealVector thaut);

   Elements* makeRindTop (int nr, int na, int nh);
   Elements* makeRindUni (Vertex* centre, Vector* vx, Vector* vz, 
                          double raytrou, double rint, double rext,
                          double ang, Vertex* vplan, int nr, int na, int nh);
   Elements* makeRind    (Vertex* centre, Vector* vx, Vector* vz, 
                          RealVector tray, RealVector tang, RealVector thaut);

   BiCylinder* makeCylinders (Vertex* ori1, Vector* z1, double r1, double h1,
                              Vertex* ori2, Vector* z2, double r2, double h2);
   BiCylinder* makePipes (Vertex* ori1, Vector* z1, double rint1, double rex1, 
                          double h1, Vertex* ori2, Vector* z2, double rint2, 
                          double rex2, double h2);

   Elements* extrudeQuadTop (Quad* start, int nbre);
   Elements* extrudeQuadUni (Quad* start, Vector* dv, double len, int nbre);
   Elements* extrudeQuad    (Quad* start, Vector* dv, RealVector tlen);

   Elements* extrudeQuadsTop (Quads start, int nbre);
   Elements* extrudeQuadsUni (Quads start, Vector* axis, double len, int nbre);
   Elements* extrudeQuads    (Quads start, Vector* axis, RealVector tlen);

   Elements* revolutionQuadUni  (Quad* start, Vertex* center, Vector* axis,
                                 double angle, int nbre);
   Elements* revolutionQuad     (Quad* start, Vertex* center, Vector* axis,
                                 RealVector angles);
   Elements* revolutionQuadsUni (Quads start, Vertex* center, Vector* axis,
                                 double angle, int nbre);
   Elements* revolutionQuads    (Quads start, Vertex* center, Vector* axis,
                                 RealVector angles);

   Elements* joinQuadUni  (Quad*  start, Quad* dest, Vertex* v1, Vertex* v2,
                           Vertex* v3, Vertex* v4, int nb);

   Elements* joinQuadsUni (Quads start, Quad* dest, Vertex* v1, Vertex* v2,
                           Vertex* v3, Vertex* v4, int nb);

   Elements* joinQuad     (Quad*  start, Quad* dest, Vertex* va1, Vertex* vb1,
                           Vertex* va2, Vertex* vb2, RealVector& tlen);

   Elements* joinQuads    (Quads start, Quad* dest, Vertex* va1, Vertex* vb1,
                           Vertex* va2, Vertex* vb2, RealVector& tlen);

   Elements* cutUni     (Edge* eddge, int nbre);
   Elements* cut        (Edge* eddge, RealVector& tlen);

   Group*    addGroup  (cpchar name, EnumGroup kind);
                                   // Fonctions globales
   bool        isSaved ()              { return doc_saved ; }

   int appendXml (pfile fstudy);       // Genere le xml dans un fichier ouvert
   cpchar getXml ();                   // Genere le xml et rend un flux

   int  getLevel ()                    { return doc_db ; }

   Vertex* findVertex (int id);

   int     closeQuads    (Quad* q1, Quad* q2);

   // ---------------------------------------------------

   Law* addLaw    (Law* law); // lo-add-lololo

   // ---------------------------------------------------

   int  checkAssociations ();
                                                        // addShape version Python
   NewShape* addShape (long forme, const char* name);

#ifndef SWIG
                                                        // addShape HorsPython
   NewShape* addShape (TopoDS_Shape& forme, const char* name);

public :
   void         dumpPropagation  ();

   Document* copyDocument ();

   NewShape*  getCloud ()          { return doc_cloud ; }
   NewShape*  addShape (const char* name, EnumShape type);
   NewShape*  findShape  (rcstring name);

   int associateLine (Vertex* mfirst, Edges& mline,
                      NewShapes& gline, IntVector& tabid,
                      double pstart, double pend, bool inv);

   int  clearAssociation  (EnumElt type);
   void clearAssoVertices ();
   void clearAssoEdges   ();
   void clearAssoQuads   ();

                               // PERIMES
   Elements* replaceHexa (Quads pattern, Vertex* p1, Vertex* c1, Vertex* p2,
                          Vertex* c2,  Vertex* p3, Vertex* c3);
   Elements* replace (Quads pattern, Vertex* p1, Vertex* c1, Vertex* p2,
                      Vertex* c2,  Vertex* p3, Vertex* c3)
   { return  replaceHexa (pattern, p1, c1, p2, c2, p3, c3);  }

   Elements* replaceHexas (Quads pattern, Quads cible, Vertex* p1, Vertex* c1,
                           Vertex* p2, Vertex* c2, Vertex* p3, Vertex* c3)
   { return  replace (pattern, cible, p1, c1, p2, c2); }


public:
   ~Document ();

    int    loadXml (cpchar name);
    int    setXml  (cpchar flux);
    int    setXml  (cpchar flux, int& position);

    EltBase* getFirstEltBase (EnumElt type) { return doc_first_elt [type]; }
    EltBase* getLastEltBase  (EnumElt type) { return doc_last_elt  [type]; }
    int      getNbrElt  (EnumElt type)      { return doc_nbr_elt   [type]; }
    void     setDeprecated (int level=1);

    void markAll (int marque, int type=-1);
    int  saveVtk0 (cpchar nomfic);
    int  saveVtk  (cpchar radical, int &nro);

    void  putError  (cpchar mess, cpchar info1=NULL, cpchar info2=NULL);
    void  hputError (cpchar mess, EltBase* e1, EltBase* e2=NULL);

    void  majReferences  ();                  // M.A.J relation "utilise par"
    void  update ()            { if (maj_connection) majReferences (); }
    bool  debug (int niv=0)    { return doc_db > niv ; }

    void  reorderFaces ();
    void  reorderQuads ();

   // --------------------------------------------------- Evols Hexa5
   cpchar getNextName (EnumElt type, string& name);
   string getNextName (EnumElt type);

   virtual char* makeVarName (char* name);
   void    lockDump();
   void    restoreDump();
   bool    isEmpty ();

   // --------------------------------------------------- Evols Hexa6

   void addHexa   (Hexa* elt)      { doc_hexas .push_back (elt) ; }
   void addQuad   (Quad* elt)      { doc_quads .push_back (elt) ; }
   void addEdge   (Edge* elt)      { doc_edges .push_back (elt) ; }
   void addVertex (Vertex* elt)    { doc_vertex.push_back (elt) ; }

public:
   Globale* glob;

private :
   int   genXml   ();
   int   parseXml (XmlTree& xml);

   void  replaceVertex (Vertex* v1, Vertex* v2);
   void  replaceEdge   (Edge*   e1, Edge* e2);
   int   replaceQuad   (Quad*   q1, Quad* q2);

   int   countElement  (EnumElt type);
   EltBase* getElement (EnumElt type, int nro);

   void  majPropagation ();
   void  recountUsed ();
   void  renumeroter ();

   Elements* clonerElements (Elements* table, Matrix* matrice);

   // --------------------------------------------------- Evols Hexa3
   Hexa* addHexaQuadsAB   (AnaQuads& strquads);
   Hexa* addHexaQuadsAC   (AnaQuads& strquads);
   Hexa* addHexaQuadsACE  (AnaQuads& strquads);
   Hexa* addHexaQuadsACD  (AnaQuads& strquads);
   Hexa* addHexaQuadsABCD (AnaQuads& strquads);
   Hexa* addHexaQuadsACDE (AnaQuads& strquads);

   EltBase* findElement (EnumElt type, int ident);
   // --------------------------------------------------- Evols Hexa5
   void addCloud ();
   int  sortShapes (NewShapes& tshapes, IntVector& subid,
                    bool closed, bool inv, double pstart, double pend,
                    EdgeShapes& glines, IntVector& sens, double& lgtotale);
   int sortEdges (Vertex* vfirst, Edges& mline, Edges& edges, IntVector& sens);

   void parseShapes         (XmlTree& root);
   void parseAssociation    (XmlTree* root, Quad* elt);
   void parseAssociation    (XmlTree* root, Edge* elt);
   void parseAssociation    (XmlTree* root, Vertex* elt);
   SubShape* parseSubShape (XmlTree* root);

private :
   friend class EltBase;

   bool doc_modified;
   bool count_modified;
   bool doc_saved;
   bool maj_connection;
   bool purge_elements;
   bool holes_elements;
   int  doc_db;
   int  nbr_errors;

   EltBase* doc_first_elt [EL_MAXI];
   EltBase* doc_last_elt  [EL_MAXI];
   int      doc_nbr_elt   [EL_MAXI];
   double   doc_tolerance;

   std::vector <Law*> doc_laws;
   int                nbr_laws;
   Law* defaultLaw;

   int  nbr_propagations;
   bool maj_propagation;

   std::vector <Propagation*> doc_propagation;
   std::vector <Group*>       doc_group;
   std::vector <Vector*>      doc_vector;
   XmlWriter*                 doc_xml;

   // --------------------------------------------------- Evols Hexa3

   std::vector <Hexa*>   doc_used_hexas;
   std::vector <Quad*>   doc_used_quads;
   std::vector <Edge*>   doc_used_edges;
   std::vector <Vertex*> doc_used_vertex;

   int nbr_used_hexas;
   int nbr_used_quads;
   int nbr_used_edges;
   int nbr_used_vertex;

   // --------------------------------------------------- Evols Hexa5

   static int doc_number;
   char      doc_ident [8];
   Hex*      hex_parent;

   NewShape*   doc_cloud;                     // Nuage de points du doc;
   std::vector <NewShape*> doc_tab_shape;

   // --------------------------------------------------- Evols internes Hexa6
   std::vector <Hexa*>   doc_hexas;
   std::vector <Quad*>   doc_quads;
   std::vector <Edge*>   doc_edges;
   std::vector <Vertex*> doc_vertex;
#endif
};
END_NAMESPACE_HEXA
#endif
