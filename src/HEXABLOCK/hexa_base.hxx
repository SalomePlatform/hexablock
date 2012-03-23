
// Incl : Common definitions for HEXA-BLOCK

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

#ifndef _HEXA_BASE_H_
#define _HEXA_BASE_H_

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
#define NOT    !  
#define XOR    ^
#define MODULO %
#define INOT   `
// #define IOR    |
#define IAND   &
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

#ifndef PI
#define PI    3.1415926535898
#endif
                       // Conventions C++
#include <string>
#include <vector>
                       // Impressions de mise au point
#include <iostream>
using namespace std;

#define HexDisplay(x)      cout << " ... " #x " = " << x << endl
#define PutData(x)      cout << " ... " #x " = " << x << endl
#define PutCoord(x)      cout << " ... " #x " = (" << x[0] << ", " \
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
BEGIN_NAMESPACE_HEXA

enum EnumCoord  { dir_x, dir_y, dir_z, DIM3 };
enum EnumElt    { EL_NONE, EL_VERTEX, EL_EDGE, EL_QUAD, EL_HEXA, EL_VECTOR, 
                  EL_REMOVED, EL_MAXI };

enum EnumGroup  { HexaCell, QuadCell, EdgeCell, 
                  HexaNode, QuadNode, EdgeNode, VertexNode};

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

enum EnumQDirection {Q_INSIDE, Q_DIR_OUT, Q_DIR_IN, Q_UNDEFINED };

enum { CylSmall=0, CylBig=1, NxInt=1, NxExt=2 };

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
class  GridElements;
class  SphericalGrid;
class  CrossElements;

struct  StrOrient;
class   Shape;
class   Globale;
class   Propagation;
class   Law;
class   Group;

class   AnaQuads;
class   Pattern;

typedef std::vector <Hexa*>  Hexas;
typedef std::vector <Quad*>  Quads;
typedef std::vector <Edge*>  Edges;
typedef std::vector <Shape*> Shapes;
typedef std::vector <double> RealVector;

typedef double Real;
typedef double Real3 [DIM3];

                        // Quelques fonctions :

pchar   get_temp_name (cpchar format, pchar nomfic);
void    fatal_error   (cpchar format, cpchar info1=NULL, cpchar info2=NULL);

double  prod_scalaire  (double v1[], double v2[]);
double* prod_vectoriel (double v1[], double v2[], double v3[]);
double  prod_mixte     (double vi[], double vj[], double vk[]);

double  calc_norme     (double v1[]);
double  calc_distance  (double v1[], double v2[]);
void    calc_vecteur   (double pta[], double ptb[], double vab[]);
void    calc_milieu    (double pta[], double ptb[], double milieu[]);
int     normer_vecteur (double v1[]);

double carre       (double val);
bool   same_coords (double* pa, double* pb, double epsilon=1e-6);

bool   on_debug();
int    niv_debug();

const double Epsil   = 1e-6;
const double UnEpsil = 0.999999;
const double Epsil2  = 1e-12;


END_NAMESPACE_HEXA

#endif
