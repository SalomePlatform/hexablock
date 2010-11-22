
// C++ : Construction de la bride

#include "Hex.hxx"
#include "HexDocument.hxx"
#include "HexElements.hxx"
#include "HexHexa.hxx"
#include "HexQuad.hxx"
#include "HexEdge.hxx"
#include "HexVertex.hxx"

static const int k0 = 0;
static const int k1 = 1;
static const int OPT_INV_EDGE = 1;
static const int OPT_QUAD_IK = 2;

// ======================================================== sauver_schema
bool is_here (int masque, int option)
{
   bool   rep = (masque IAND option) > 0;
   return rep;
}
// ======================================================== sauver_schema
void sauver_schema (Hex::Document* doc)
{
   static int numero = 0;
   char nomfic [32];
   sprintf (nomfic, "essai%d.vtk", numero);
   numero++;

   doc->saveVtk (nomfic);
}
// ======================================================== merge_quads
void merge_quads (Hex::Document* doc, Hex::Elements* quart, Hex::Elements* demi,
                  int ni1, int nj1, int ni2, int nj2, int mask=0)
{
   int  inv     = is_here (mask, OPT_INV_EDGE);
   bool quad_ik = is_here (mask, OPT_QUAD_IK);
   int  iq1     = 0;
   int  iq3     = 1;

   if (quad_ik || inv) 
      {
      iq1 = 1;
      iq3 = 0;
      }

   Hex::Quad* orig  = quad_ik ? quart->getQuadIK (ni1, nj1, k0)
                              : quart->getQuadJK (ni1, nj1, k0);
   Hex::Quad* dest  = demi->getQuadJK  (ni2, nj2, k0);

   if (quad_ik) orig->dumpPlus();

   Hex::Vertex* v1  = dest->getVertex (iq1);
   Hex::Vertex* v3  = dest->getVertex (iq3);

   Hex::Vertex* v2  = orig->getVertex (0);
   Hex::Vertex* v4  = orig->getVertex (1);

   doc->mergeQuads (dest, orig, v1, v2, v3, v4);
   sauver_schema (doc);
}
// ======================================================== test_bug0
int test_bug0 ()
{
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex* orig1 = doc->addVertex (0,0,0);
   Hex::Vertex* orig2 = doc->addVertex (7,0,0);

   Hex::Vector* dz = doc->addVector (0,0,1);
   Hex::Vector* dx = doc->addVector (1,0,0);

   const int dim_z  = 1;
   double drd = 0.5;
   double drq = 2*drd;
   double dl  = 1;
   int    nrq = 10;
   int    nrd = 5;
   int    naq = 4;
   int    nad = 10;
                               // Les grilles initiales

   Hex::Elements* quart = doc->makeCylindrical (orig1,dx,dz,drq, 45, 
                                                dl,nrq,naq,dim_z);

   Hex::Elements* demi  = doc->makeCylindrical (orig2,dx,dz,drd, 180, 
                                                dl,nrd,nad, dim_z);
   sauver_schema (doc);
                               // Creuser les fondations de demi dans quart
   int jmax= nrq-1;
   for (int nj= 0; nj<2 ; nj++)
       {
       for (int ni=3; ni<jmax-nj ; ni++)
           {
           Hex::Hexa* cell = quart->getHexaIJK (ni, nj, k0);
           cell->remove ();
           }
   }
   sauver_schema (doc);
   // doc->setLevel (1);
   // doc->dump ();
                               // On fusionne les bords
   merge_quads (doc, quart, demi, 9, 0,   nrd, 0);
   merge_quads (doc, quart, demi, 8, 1,   nrd, 1, OPT_QUAD_IK);
   // doc->dump ();
   doc->countPropagation ();
   merge_quads (doc, quart, demi, 6, 2,   nrd, 4, OPT_QUAD_IK);
   //  doc->countPropagation ();
   merge_quads (doc, quart, demi, 8, 1,   nrd, 2);
   merge_quads (doc, quart, demi, 7, 2,   nrd, 3, OPT_QUAD_IK);
   merge_quads (doc, quart, demi, 6, 2,   nrd, 4, OPT_QUAD_IK);

   merge_quads (doc, quart, demi, 5, 2,   nrd, 5, OPT_QUAD_IK);
   merge_quads (doc, quart, demi, 4, 2,   nrd, 6, OPT_QUAD_IK);
   merge_quads (doc, quart, demi, 3, 2,   nrd, 7, OPT_QUAD_IK);
   merge_quads (doc, quart, demi, 3, 1,   nrd, 8, OPT_INV_EDGE);
   merge_quads (doc, quart, demi, 3, 0,   nrd, 9, OPT_INV_EDGE);

   return HOK;
}
// ======================================================== test_bug1
int test_bug1 ()
{
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

   Hex::Vertex* orig1 = doc->addVertex (0,0,0);

   Hex::Vector* vx = doc->addVector (1,0,0);
   Hex::Vector* vy = doc->addVector (0,1,0);
   Hex::Vector* vz = doc->addVector (0,0,1);

   Hex::Elements* grid = doc->makeCartesian (orig1, vx,vy,vz, 4,3,2);
   Hex::Edge*     edge = grid->getEdgeI (0,0,0);

   doc->saveVtk ("bug1.vtk");
   Hex::Elements* cut  = doc->cut (edge, 1);

   doc->saveVtk ("bug2.vtk");
   grid->saveVtk ("bug3.vtk");

   int nro=0;
   cut->saveVtk ("cut.vtk", nro);
   int nbre = cut->countHexa ();
   for (int nc=0 ; nc<nbre ; nc++)
       {
       cut->getHexa(nc)->remove ();
       cut->saveVtk ("cut.vtk", nro);
       }


   return HOK;
}
