
// Incl : Common definitions for HEXA-BLOCK

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

#define DumpActif true

#ifndef _HEXA_BASE_H_
#define _HEXA_BASE_H_

#include "Hex_defines.hxx"

#include <cstdio>
#include <cstring>

                    // Conventions C Ansi compatibles
typedef char*       pchar;
typedef const char* cpchar;
typedef FILE*       pfile;

#define Cestegal(c1,c2)      !strcmp(c1,c2)
#define Cestdifferent(c1,c2)  strcmp(c1,c2)
#define Cestvide(c1)    !c1[0]
#define Cestnonvide(c1)  c1[0]

                     // Pour rendre les operateurs plus visibles
                     // ... mais ne passe pas avec ce @#! de swig
#ifndef SWIG
#define NOT    !
#define XOR    ^
#define MODULO %
#define INOT   `
// #define IOR    |
#define IAND   &
#endif    // Swig
                    // Chaines de bits
#define DeuxPuissance(n) (1 << (n))

                     // Codes retour d'une fonction
#define HOK  0       // Code retour=0 :  deroulement correct
#define HERR 1       // Code retour>0 = numero d'erreur, 1 par defaut
#define NOTHING -1   // Code retour fonction de recherche d'un index

#define EOL '\n'
#define EOS 0
#define CHVIDE ""
                      // Calculs reels en 3D
#define ZEROR 0.0
#define UNR   1.0
#define DEMI  0.5
#define DIM2  2

// enum EnumCoord { dir_x, dir_y, dir_z, DIM3 };
// typedef double Real;
// typedef double Real3 [DIM3];

#ifndef M_PI
#define M_PI  3.14159265358979323846
#endif
#define Degre2Radian M_PI/180

                       // Conventions C++
#include <string>
#include <vector>

typedef const std::string& rcstring;
                       // Impressions de mise au point
#include <iostream>
using namespace std;

#define HexDisplay(x)   cout << " ... " #x " = " << x << endl
#define PutData(x)      cout << " ... " #x " = " << x << endl
#define PutCoord(x)     cout << " ... " #x " = (" << x[0] << ", " \
                              << x[1] << ", " << x[2] << ")"  << endl
#define Echo(m)         cout << " _______________ " << m << endl

//  #define Libere(obj)     { if (obj!=NULL) free (obj)      ;  obj=NULL; }
//  #define Destroy(obj)    { delete obj       ;  obj=NULL   ;            }
//  #define Deltable(table) { delete [] table  ;  table=NULL ;            }

// ---------------------------------- Definitions propres a HEXA
#define HEXA_NS Hex
#define BEGIN_NAMESPACE_HEXA namespace Hex {
#define END_NAMESPACE_HEXA   }

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

//--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8

class  TopoDS_Shape;
class  BRepAdaptor_Curve;

BEGIN_NAMESPACE_HEXA

enum EnumCoord  { dir_x, dir_y, dir_z, DIM3 };
enum EnumElt    { EL_NONE, EL_VERTEX, EL_EDGE, EL_QUAD, EL_HEXA, EL_VECTOR,
                  EL_GRID, EL_CYLINDER, EL_PIPE, EL_GROUP, EL_LAW,
                  EL_SHAPE, EL_SUBSHAPE, EL_PROPAGATION,
                  EL_DOCU,  EL_REMOVED, EL_MAXI };

const cpchar ABR_TYPES = "xveqhwtcpglsupdz???";

enum EnumGroup  { HexaCell, QuadCell, EdgeCell,
                  HexaNode, QuadNode, EdgeNode, VertexNode};

                  // Origine des shapes
enum EnumShape { SH_NONE,  SH_IMPORT, SH_CYLINDER, SH_INTER, SH_SPHERE,
                 SH_CLOUD, SH_EXTRUD };

                //  Modes de remplissage des grilles cylindriques
enum EnumCyl   { CYL_NOFILL, CYL_CL4, CYL_CL6, CYL_CLOSED, CYL_PEER, CYL_ODD};
enum EnumGrid   { GR_NONE, GR_CARTESIAN, GR_CYLINDRIC, GR_SPHERIC, GR_JOINT,
                  GR_BICYL, GR_BIPIPE, GR_REPLACE,
                  GR_HEMISPHERIC, GR_RIND, GR_PART_SPHERIC, GR_PART_RIND };
                //  Sommets de la decomposition canonique du bicylindre
                // 0    1     2    3     4    5     6    7     8
enum EnumVCyl   { S_E, S_NE, S_N, S_NW, S_W, S_SW, S_S, S_SE, S_MAXI };

enum KindLaw    { Uniform, Arithmetic, Geometric };

enum EnumOrient { OR_FRONT, OR_LEFT, OR_RIGHT, OR_BACK }; // respecter l'ordre
enum EnumMark   { IS_NONE=-1,  IS_MARRIED=-2 };
enum EnumCount  { NO_COUNTED=-3, NO_USED=-2, IS_USED=-1};

enum EnumVertex { V_AMONT, V_AVAL, V_TWO };
enum EnumQuad   { E_A, E_B, E_C, E_D, QUAD4 };

          //      z=0   z=1   y=0   y=1   x=0  x=1
enum EnumHQuad   {Q_A,  Q_B,  Q_C,  Q_D,  Q_E, Q_F,  HQ_MAXI};

enum EnumHEdge   {E_AC, E_AD, E_BC, E_BD,
                  E_AE, E_AF, E_BE, E_BF,
                  E_CE, E_CF, E_DE, E_DF,  HE_MAXI };

enum EnumHVertex {V_ACE, V_ACF, V_ADE, V_ADF, V_BCE, V_BCF, V_BDE, V_BDF,
                  HV_MAXI };

enum EnumQDirection {Q_INSIDE, Q_DIRECT, Q_INVERSE, Q_UNDEFINED, Q_WAITING };

enum EnumKindOfShape {KS_Line, KS_Circle, KS_Ellipse, KS_Hyperbola,
                      KS_Parabola, KS_BezierCurve, KS_BSplineCurve,
                      KS_OtherCurve, KS_None=-1 };

enum { CylSmall=0, CylBig=1, NxInt=1, NxExt=2 };

class Hex;
class Document;
class EltBase;

class Hexa;
class Quad;
class Edge;
class Vertex;

class Cylinder;
class Pipe;
class Prism;

class Vector;
class Matrix;
class Cloner;
class XmlWriter;
class XmlTree;

class  Elements;
class  BiCylinder;

struct  StrOrient;
class   Globale;
class   Propagation;
class   Law;
class   Group;

class   AnaQuads;
class   Pattern;

class   Shape;
class   SubShape;
class   VertexShape;
class   BiCylinderShape;
class   EdgeShape;
class   FaceShape;
class   AssoEdge;
class   NewShape;  // 1) Shape -> OldShape, 2) NewShape->Shape
typedef Shape OldShape;

typedef std::vector <Hexa*>  Hexas;
typedef std::vector <Quad*>  Quads;
typedef std::vector <Edge*>  Edges;
typedef std::vector <Shape*> Shapes;
typedef std::vector <Vertex*> Vertices;

typedef std::vector <NewShape*>  NewShapes;
typedef std::vector <SubShape*>  SubShapes;
typedef std::vector <AssoEdge*>  AssoEdges;
typedef std::vector <FaceShape*> FaceShapes;
typedef std::vector <EdgeShape*> EdgeShapes;

typedef std::vector <EltBase*>    TabElts;
typedef std::vector <double>      RealVector;
typedef std::vector <int>         IntVector;
typedef std::vector <std::string> TabText;

typedef double Real;
typedef double Real3 [DIM3];

                                  // a cause de Swig
#ifndef HEXABLOCKENGINE_EXPORT 
#define HEXABLOCKENGINE_EXPORT 
#endif 

#ifndef HexaExport 
#define HexaExport 
#endif 

                        // Quelques fonctions :

HexaExport pchar   get_temp_name (cpchar format, pchar nomfic);
HexaExport void    fatal_error   (cpchar format, cpchar info1=NULL, cpchar info2=NULL);

HexaExport double  prod_scalaire  (double v1[], double v2[]);
HexaExport double* prod_vectoriel (double v1[], double v2[], double v3[]);
HexaExport double  prod_mixte     (double vi[], double vj[], double vk[]);

HexaExport inline  double deg2radians (double angle) {  return (angle*M_PI/180.0); }
HexaExport inline  double rad2degres  (double angle) {  return (angle*180.0/M_PI); }

HexaExport double  calc_norme     (double v1[]);
HexaExport double  calc_distance  (double v1[],  double v2[]);
HexaExport double  calc_d2        (double v1[],  double v2[]);
HexaExport double  calc_d2        (double v1[], double v2[]);
HexaExport void    calc_vecteur   (double pta[], double ptb[], double vab[]);
HexaExport void    copy_vecteur   (double va [], double vb []);
HexaExport void    calc_milieu    (double pta[], double ptb[], double milieu[]);
HexaExport int     normer_vecteur (double v1[]);

HexaExport double  carre       (double val);
HexaExport bool    same_coords (double* pa, double* pb, double epsilon=1e-6);

HexaExport bool requals (const double  v1, const double  v2);
HexaExport bool requals (const double* v1, const double* v2);

HexaExport bool   on_debug();     // == getenv ("HEXA_DB") > 0
HexaExport bool   in_test ();     // == getenv ("HEXA_TEST") > 0
HexaExport int    niv_debug();    // Implemente prochainement

HexaExport void   set_minus (string& chaine);

HexaExport bool   special_option ();
HexaExport void   set_special_option (bool opt);

HexaExport int    sizeof_file (cpchar filename);
HexaExport char*  read_file   (cpchar filename, int& size);
HexaExport cpchar get_time   (string& buffer);
HexaExport int make_basename (cpchar filename, string& base);

const double Epsil   = 1e-6;
const double UnEpsil = 0.999999;
const double Epsil2  = 1e-12;

END_NAMESPACE_HEXA

#endif
