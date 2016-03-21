
// Class : Gestion des tableaux d'hexaedres

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

#ifndef __ELEMENTS_H
#define __ELEMENTS_H

#include "HexEltBase.hxx"
#include "HexMatrix.hxx"

#include <map>

BEGIN_NAMESPACE_HEXA

class Qpattern;
class HexaExport Elements : public EltBase
{
public:
   virtual Hexa*   getHexa   (int nro);
   virtual Quad*   getQuad   (int nro);
   virtual Edge*   getEdge   (int nro);
   virtual Vertex* getVertex (int nro);

   Hexa*   getHexaIJK   (int nx, int ny, int nz);
   Quad*   getQuadIJ    (int nx, int ny, int nz);
   Quad*   getQuadJK    (int nx, int ny, int nz);
   Quad*   getQuadIK    (int nx, int ny, int nz);
   Edge*   getEdgeI     (int nx, int ny, int nz);
   Edge*   getEdgeJ     (int nx, int ny, int nz);
   Edge*   getEdgeK     (int nx, int ny, int nz);
   Vertex* getVertexIJK (int nx, int ny, int nz);

   virtual int countHexa   () { return nbr_hexas ; }
   virtual int countQuad   () { return nbr_quads ; }
   virtual int countEdge   () { return nbr_edges ; }
   virtual int countVertex () { return nbr_vertex ; }

   Vertex* nearestVertex (Vertex* other);

   int findVertex (Vertex* elt);
   virtual void clearAssociation ();
   int saveVtk  (cpchar nomfic);
   Elements (Document* doc);

#ifndef SWIG
public:
   int findHexa   (Hexa*   elt);
   int findQuad   (Quad*   elt);   // indexQuad   ?
   int findEdge   (Edge*   elt);   // indexVertex ?

   int findVertex (double vx, double vy, double vz);
   Edge* findEdge   (Vertex* v1, Vertex* v2);
   Quad* findQuad   (Edge*   e1, Edge*   e2);

   Elements (Document* doc, int nx, int ny, int nz);
   Elements (Elements* orig);

   virtual ~Elements () {}
   virtual void remove ();

   Hexa*   getStrate (int couche, EnumHQuad face);

   int     getHexas (Hexas& liste);

   void   addHexa (Hexa* element);
   void   addQuad (Quad* element);
   void   addEdge (Edge* element);
   void   addVertex (Vertex* element);
   int    transform (Matrix* matrice);

   int saveVtk  (cpchar nomfic, int& nro);

   int coupler  (int nro, Quad* other, StrOrient* orient);

   int  cutHexas   (const Edges& edges, int nbcuts);

   void setVertex (Vertex* node, int nx, int ny, int nz);

   void transfoVertices (double* orig, double* base, double* haut);

   void setVertex (Vertex* node, int nro);
   void setEdge   (Edge*   edge, int nro);
   void setQuad   (Quad*   quad, int nro);
   void setHexa   (Hexa*   cell, int nro);

   int nroVertex (int nsommet,  int nquad, int nh);

              // Evols Hexa3

   int getCylPoint (int nr, int na, int nh, double& px, double& py, double& pz);

   static int controlRind (EnumGrid type, Vertex* cx, Vector* vx, Vector* vz,
                           double rext, double rint, double radhole,
                           Vertex* plorig, double angle,
                           int nrad, int nang, int nhaut,
                           double &phi0, double &phi1);

   int replaceHexas (Quads& pattern, Vertex* p1, Vertex* c1, // To Delete
                     Vertex* p2, Vertex* c2,  Vertex* p3, Vertex* c3);

   int replaceHexas (Quads& pattern, Quads& target, Vertex* p1, Vertex* c1,
                     Vertex* p2, Vertex* c2);

   int replaceHexa  (int nh, Pattern* pat, Hexa* hexa);
   int replaceQuad  (int nh, Pattern* pat, Quad* quad, Vertex* tvert[]);
   int extrudeQuad  (Pattern* pat);

   int replaceHexa  (int nh, Qpattern* pat);
   int replaceQuad  (int nh, Qpattern* pat);
   int extrudeQuad  (Qpattern* pat);

   void repVertex (int nh, int nro, Vertex* node);
   void repEdgeH  (int nh, int nro, Edge* node);
   void repEdgeV  (int nh, int nro, Edge* node);
   void repQuadH  (int nh, int nro, Quad* node);
   void repQuadV  (int nh, int nro, Quad* node);

   Vertex* repVertex (int nh, int nro, double px, double py, double pz);
   Edge*   repEdgeV  (int nh, int nro, Vertex* v1, Vertex*v2);
   Edge*   repEdgeH  (int nh, int nro, Vertex* v1, Vertex*v2);
   Quad*   repQuadH  (int nh, int nro, Edge* ea, Edge* eb, Edge* ec, Edge* ed);
   Quad*   repQuadV  (int nh, int nro, Edge* ea, Edge* eb, Edge* ec, Edge* ed);
   Hexa*   repHexa   (int nh, int nro, Quad* qa, Quad* qb, Quad* qc,
                                       Quad* qd, Quad* qe, Quad* qf);

   Vertex* repVertex (int nh, int nro);
   Edge*   repEdgeV  (int nh, int nro);
   Edge*   repEdgeH  (int nh, int nro);
   Quad*   repQuadH  (int nh, int nro);
   Quad*   repQuadV  (int nh, int nro);

   virtual Hexa*   getKerHexa   (int nro) { return ker_hexa  [nro] ; }
   virtual Quad*   getKerHQuad  (int nro) { return ker_hquad [nro] ; }
   virtual Quad*   getKerVQuad  (int nro) { return ker_vquad [nro] ; }
   virtual Edge*   getKerHEdge  (int nro) { return ker_hedge [nro] ; }
   virtual Edge*   getKerVEdge  (int nro) { return ker_vedge [nro] ; }

   void moveDisco (Hexa* hexa);


   // Hexa v6
   int makeCartesian (Vertex* origine, double* vx, double* vy, double* vz, 
                      RealVector& lx, RealVector& ly, RealVector& lz);
   int makeCylinder (Vertex* origine, double* vx, double* vz,
                     RealVector& rayon, RealVector& angle, RealVector& hauteur, 
                     bool fill);
   int makeSpherical (double* centre, double* vx, double* vz, RealVector& ray, 
                      int critere);
   int makeRind (EnumGrid type, double* centre, double* vx, double* vz, 
                 RealVector& rayon, RealVector& angle, RealVector& hauteur);

   int extrudeQuads (Quads& quads, double* axe, RealVector& val, 
                     bool revol=false, Vertex* center=NULL);

   int joinQuads (Quads& quadsa, Quad* quadb, Vertex* va1, Vertex* vb1, 
                  Vertex* va2, Vertex* vb2, RealVector& longueur);

   int cutEdge (Edge* edge, RealVector& longueur);

   // Internal Hexa v6
   void checkSystem (int nro, Vector* vx, Vector* vy, Vector* vz,
                     double* ux, double* uy, double* uz);
   void checkSystem (int nro, Vector* vx, Vector* vz, double* ux, double* uz);
   void checkSize   (int nro, int nx, int ny, int nz, bool rad=false);
   void checkVector (int nro, RealVector& table, int lgmin=1, bool relative=false);
   void checkPipe   (int nro, double rint, double rext, double ang, double hot);
   void checkOrig   (int nro, Vertex* orig);
   void checkDiams  (int nro, double rint, double rext);

   int  checkPhi (bool sphere, double* orig, double* norm, double rmax, 
                  double rhole, Vertex* plan, double& phi0, double& phi1);

   int  checkInter (double* pa, double* ua, double raya, double lga, 
                    double* pb, double* ub, double rayb, double lgb, 
                    double* center, bool& left, bool& right);

   void makeNormale (Quads& tquads, double* normale, double& larg) { } // TODO
   void checkQuads  (Quads& tquads);
   void checkQuad   (Quad*  quad, int nro=0);
   void checkAxis   (Vector* vect, double* axe);
   void checkSense  (int nro, Vertex* v1, Vertex* v2, Quad* quad);

   void checkDisco  (Hexas& thexas, Edges&  tedges);
   void checkDisco  (Hexa* cell,    Quad*   element);
   void checkDisco  (Hexa* cell,    Edge*   element);
   void checkDisco  (Hexa* cell,    Vertex* element);
   void checkContour (Quads& tquad, Vertex* v1, Vertex* v2, bool target, 
                      Edges& tedge);
   void checkContour (Quads& tquad, Vertex* v1, Vertex* v2, bool target, 
                      Vertices& tvertex);

protected :
                                        // Evols Hexa3
   void cutAssociation (Shapes& tshapes, Edges& tedges, bool exist=true);
   void cutAssociation (Edge* orig, Edges& tedges);
   void assoCylinder   (Vertex* center, Vector* vz, double rayon);
   void assoCylinders  (Vertex* center, Vector* vz, double ray, RealVector& ta);
   void assoCylinders  (double* orig, double* vz, double ray, RealVector& ta);

   void assoRind       (double* center, double* vx, int nx, NewShape* geom);

   void assoSphere     (double* center, Edge* t_edge[], Quad* t_quad[]);
   void assoCircle     (double* center, Edge* ed1, Edge* ed2, NewShape* geom);
   void assoResiduelle ();

   int makeBasicCylinder (RealVector& tdr, RealVector& tda, RealVector& tdh,
                          bool fill=false);

   int  propagateAssociation (Edge* orig, Edge* dest, Edge* dir);

                                        // Evols Hexa4
   void  updateMatrix (int hauteur);
   void  endPrism ();
                                        // Evols Hexa5
   int  prismAssociation     (Edge* orig, Edge* dest, int nh);

                                        // Evols Hexa6
   int makeRindPoint (double ray, double ang, double haut, double& px, 
                                              double& py,  double& pz);


protected :

   int   fillGrid ();
   void  fillCenter    ();
   void  fillCenter4   ();
   void  fillCenter6   ();
   void  fillCenterOdd ();

   int  prismHexas (int nro, Quad* sol, int hauteur);

   void setVertex (int nx, int ny, int nz, double px, double py, double pz);
   void setEdge   (Edge*   edge, EnumCoord dir, int nx, int ny, int nz);
   void setQuad   (Quad*   quad, EnumCoord dir, int nx, int ny, int nz);
   void setHexa   (Hexa*   cell, int nx, int ny, int nz);

   Edge* newEdge (Vertex* v1, Vertex* v2);
   Quad* newQuad (Edge* e1, Edge* e2, Edge* e3, Edge* e4);
   Hexa* newHexa (Quad* e1, Quad* e2, Quad* e3, Quad* e4, Quad* e5, Quad* e6);

   void resize (EnumGrid type, int nx, int ny=0, int nz=0, int nplus=0);

   int makeCylindricalNodes (Vertex* c, Vector* b, Vector* h,
         double dr, double da, double dl, int nr, int na, int nl, bool fill);

   int addStrate (Quad* i_quad[], Edge* i_edge[], Vertex* i_node[],
                  double* center,  double lambda);

   int indVertex (int nsommet,  int nquad, int nh);
   int indVertex (int ref_vert, int nh);
   int nroEdgeH  (int nvertex);
   int nroEdgeH  (int nsommet,  int nquad, int nh);
   int nroHexa   (int nquad, int nh);

   void copyVertex (Elements* other, int px, int py, int pz,
                                     int nx, int ny, int nz);
   void completerGrille  (double hauteur);
   NewShape* getShape   ();
   void      closeShape ();

protected :
    Globale* glob;
    EnumGrid grid_type;

    std::vector<Hexa*>   tab_hexa;
    std::vector<Quad*>   tab_quad;
    std::vector<Edge*>   tab_edge;
    std::vector<Edge*>   tab_pilier;
    std::vector<Vertex*> tab_vertex;

    std::vector<Quad*>   tab_orig;

    int size_hx, size_hy, size_hz, size_hplus;
    int size_qx, size_qy, size_qz, size_qhplus, size_qvplus;
    int size_ex, size_ey, size_ez, size_ehplus, size_evplus;
    int size_vx, size_vy, size_vz;

    int nbr_hexas, nbr_quads, nbr_edges, nbr_vertex;
    int nbr_orig, nbr_piliers, ker_vertex;

    int   gr_rayon;              // Spheric
    int   gr_hauteur;            // Joint
    int   nbr_secteurs;          // Cyl

    bool     cyl_closed;         // Angle = 180 degres
    bool     cyl_fill;           // Interieur rempli
    bool     val_absolues;       // Version Hexa6
    EnumCyl  cyl_dispo;          // Type de remplissage

                                 // Evols Hexa3
    bool       revo_lution;  // Number 9 ...
    Vertex*    revo_center;
    Real3      revo_axe;
    bool       under_v6;

    RealVector gen_values;    // Angle pour la revolution
    RealVector cum_values;    // Valeurs cumulees
    Matrix     gen_matrix;
    Matrix     cum_matrix;    // Matrice angles cumules
                                 // Evols Hexa4
    bool       prism_vec ;
    Real3      prism_dir;
    double     prism_len;

//  EnumGrid grid_type;   // deja la
    bool    grid_nocart;   // Grille non cartesienne : no acces aux getTrucIJK
    double  cyl_length;
    double  cyl_radhole, cyl_radext, cyl_radint;
    double  cyl_dtheta;              // angle = na*dtheta
    double  cyl_phi0,    cyl_dphi;   // angle = phi0 + nh*dphi;
    double  cyl_phimax;

    int pat_nbedges;
    int pat_nbvertex;

    std::vector<Hexa*>   ker_hexa;
    std::vector<Quad*>   ker_hquad, ker_vquad;
    std::vector<Edge*>   ker_hedge, ker_vedge;

    NewShape*                    grid_geom; // Utilise pour faire la revolution
    std::map <std::string, int>  map_shape;
#endif
};
END_NAMESPACE_HEXA
#endif
