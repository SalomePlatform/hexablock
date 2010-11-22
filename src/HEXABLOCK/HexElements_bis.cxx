
// C++ : Table d'hexaedres

#include "HexElements.hxx"

#include "HexDocument.hxx"
#include "HexVector.hxx"
#include "HexVertex.hxx"
#include "HexHexa.hxx"
#include "HexEdge.hxx"

#include "HexGlobale.hxx"
#include "HexCylinder.hxx"

#include <map>

static bool db=false;

BEGIN_NAMESPACE_HEXA

// ====================================================== getHexaIJK
Hexa* Elements::getHexaIJK (int nx, int ny, int nz)
{
   if (nx<0 || nx>=size_hx ||  ny<0 || ny>=size_hy || nz<0 || nz>=size_hz)
      return NULL;
   else if (grid_type != GR_CARTESIAN && grid_type != GR_CYLINDRIC)
      return NULL;

   int nro = nx + size_hx*ny + size_hx*size_hy*nz; 

   return tab_hexa [nro]; 
}
// ====================================================== getQuadIJ
Quad* Elements::getQuadIJ (int nx, int ny, int nz)
{
   if (nx<0 || nx>=size_qx ||  ny<0 || ny>=size_qy || nz<0 || nz>=size_qz)
      return NULL;
   else if (grid_type != GR_CARTESIAN && grid_type != GR_CYLINDRIC)
      return NULL;

   int nro = nx + size_qx*ny + size_qx*size_qy*nz 
                + size_qx*size_qy*size_qz*dir_z;
   return tab_quad [nro]; 
}
// ====================================================== getQuadJK
Quad* Elements::getQuadJK (int nx, int ny, int nz)
{
   if (nx<0 || nx>=size_qx ||  ny<0 || ny>=size_qy || nz<0 || nz>=size_qz)
      return NULL;
   else if (grid_type != GR_CARTESIAN && grid_type != GR_CYLINDRIC)
      return NULL;

   int nro = nx + size_qx*ny + size_qx*size_qy*nz; // + dir_x*...

   return tab_quad [nro]; 
}
// ====================================================== getQuadIK
Quad* Elements::getQuadIK (int nx, int ny, int nz)
{
   if (nx<0 || nx>=size_qx ||  ny<0 || ny>=size_qy || nz<0 || nz>=size_qz)
      return NULL;
   else if (grid_type != GR_CARTESIAN && grid_type != GR_CYLINDRIC)
      return NULL;

   int nro = nx + size_qx*ny + size_qx*size_qy*nz + size_qx*size_qy*size_qz;

   return tab_quad [nro]; 
}
// ====================================================== getEdgeI
Edge* Elements::getEdgeI (int nx, int ny, int nz)
{
   if (nx<0 || nx>=size_ex ||  ny<0 || ny>=size_ey || nz<0 || nz>=size_ez)
      return NULL;
   else if (grid_type != GR_CARTESIAN && grid_type != GR_CYLINDRIC)
      return NULL;

   int nro = nx + size_ex*ny + size_ex*size_ey*nz;

   return tab_edge [nro]; 
}
// ====================================================== getEdgeJ
Edge* Elements::getEdgeJ (int nx, int ny, int nz)
{
   if (nx<0 || nx>=size_ex ||  ny<0 || ny>=size_ey || nz<0 || nz>=size_ez)
      return NULL;
   else if (grid_type != GR_CARTESIAN && grid_type != GR_CYLINDRIC)
      return NULL;

   int nro = nx + size_ex*ny + size_ex*size_ey*nz + size_ex*size_ey*size_ez;

   return tab_edge [nro]; 
}
// ====================================================== getEdgeK
Edge* Elements::getEdgeK (int nx, int ny, int nz)
{
   if (nx<0 || nx>=size_ex ||  ny<0 || ny>=size_ey || nz<0 || nz>=size_ez)
      return NULL;
   else if (grid_type != GR_CARTESIAN && grid_type != GR_CYLINDRIC)
      return NULL;

   int nro = nx + size_ex*ny + size_ex*size_ey*nz 
                + size_ex*size_ey*size_ez*dir_z;
   return tab_edge [nro]; 
}
// ====================================================== getVertexIJK
Vertex* Elements::getVertexIJK (int nx, int ny, int nz)
{
   if (nx<0 || nx>=size_vx ||  ny<0 || ny>=size_vy || nz<0 || nz>=size_vz)
      return NULL;
   else if (grid_type != GR_CARTESIAN && grid_type != GR_CYLINDRIC)
      return NULL;

   int nro = nx + size_vx*ny + size_vx*size_vy*nz; 

   return tab_vertex [nro]; 
}
// ====================================================== setVertex
void Elements::setVertex (Vertex* elt, int nx, int ny, int nz)
{
   if (   nx < 0 || nx >= size_vx || ny < 0 || ny >= size_vy 
       || nz < 0 || nz >= size_vz) return; 

   int nro = nx + size_vx*ny + size_vx*size_vy*nz;
   tab_vertex [nro] = elt;
}
// ====================================================== setVertex (2)
void Elements::setVertex (int nx, int ny, int nz, double px, double py, 
                                                  double pz)
{
   if (   nx < 0 || nx >= size_vx || ny < 0 || ny >= size_vy 
       || nz < 0 || nz >= size_vz) return; 

   Vertex*    node = el_root->addVertex (px, py, pz);
   setVertex (node, nx, ny, nz);
}
// ====================================================== setEdge
void Elements::setEdge (Edge* elt, EnumCoord dir, int nx, int ny, int nz)
{
   if (nx<0 || nx>=size_ex ||  ny<0 || ny>=size_ey || nz<0 || nz>=size_ez
            || dir < dir_x || dir > dir_z )
      return;

   int nro = nx + size_ex*ny + size_ex*size_ey*nz + size_ex*size_ey*size_ez*dir;
   tab_edge [nro] = elt; 
}
// ====================================================== setQuad
void Elements::setQuad (Quad* elt, EnumCoord dir, int nx, int ny, int nz)
{
   if (nx<0 || nx>=size_ex ||  ny<0 || ny>=size_ey || nz<0 || nz>=size_ez
            || dir < dir_x || dir > dir_z )
      return;

   int nro = nx + size_ex*ny + size_ex*size_ey*nz + size_ex*size_ey*size_ez*dir;
   tab_quad [nro] = elt; 
}
// ====================================================== setHexa
void Elements::setHexa (Hexa* elt, int nx, int ny, int nz)
{
   if (   nx < 0 || nx >= size_hx || ny < 0 || ny >= size_hy 
       || nz < 0 || nz >= size_hz) return; 

   int nro = nx + size_hx*ny + size_hx*size_hy*nz;
   tab_hexa [nro] = elt;
}
// ====================================================== remove
void Elements::remove ()
{
   int nbre=tab_hexa.size ();
   nbre = nbr_hexas;
   for (int nh=0 ; nh<nbre ; nh++)
       if (tab_hexa[nh] != NULL)
           tab_hexa[nh]->remove();
}
// ====================================================== prismQuads
int Elements::prismQuads (Quads& tstart, Vector* dir, int hauteur)
{
   el_root->markAll (NO_USED);
   int nbcells   = tstart.size ();
   nbr_vertex    = 0;
   nbr_edges     = 0;

   nbr_hexas = nbcells*hauteur;

   tab_hexa.resize (nbr_hexas);
   tab_quad.clear ();          // verticaux
   tab_edge.clear ();
   tab_pilier.clear ();
   tab_vertex.clear ();

   for (int nro=0 ; nro<nbcells ; nro++)
       {
       pushHexas (nro, tstart[nro], dir->getDx(), dir->getDy(), dir->getDz(), 
                  hauteur);
       }
   return HOK;
}
// ====================================================== pushHexas
int  Elements::pushHexas (int nro, Quad* qbase, double dx, double dy, 
                          double dz, int hauteur, Quad* toit, int tcorr[])
{
   int ind_node [QUAD4];

           // ----------------------------- Vertex + aretes verticales
   for (int ns=0 ; ns<QUAD4 ; ns++)
       {
       Vertex* vbase = qbase ->getVertex (ns);
       int     indx  = vbase->getMark ();
       if (indx<0)
          {
          indx = nbr_vertex++;
          vbase->setMark (indx);
          Vertex* nd0 = vbase;
          Vertex* nd1 = NULL;
          for (int nh=0 ; nh<hauteur ; nh++)
              {
              nd1 = el_root->addVertex (nd0->getX() + dx, nd0->getY() + dy,
                                        nd0->getZ() + dz);
              tab_vertex.push_back (nd1);
              tab_pilier.push_back (newEdge (nd0, nd1));
              nd0 = nd1;
              }
          }
       ind_node [ns] = indx;
       }
           // ----------------------------- Aretes horizontales
           // ----------------------------- + face verticales
   int ind_poutre [QUAD4];
   for (int ns=0 ; ns<QUAD4 ; ns++)
       {
       Edge* ebase = qbase ->getEdge (ns);
       int   indx  = ebase->getMark ();
       if (indx<0)
          {
          indx = nbr_edges ++;
          ebase->setMark (indx);
          int   nd1 = ind_node [ns];
          int   nd2 = ind_node [(ns+1) MODULO QUAD4];
          Edge* ed0 = ebase;
          Edge *ed1, *ed2, *ed3;
          for (int nh=0 ; nh<hauteur ; nh++)
              {
              ed2 = ed0;
              ed0 = newEdge (tab_vertex [nd1*hauteur + nh], 
                             tab_vertex [nd2*hauteur + nh]);
              ed1 = tab_pilier [nd1*hauteur + nh];
              ed3 = tab_pilier [nd2*hauteur + nh];

              Quad* mur = newQuad (ed0, ed1, ed2, ed3);
              tab_edge.push_back (ed0);
              tab_quad.push_back (mur);
              }
          }
       ind_poutre [ns] = indx;
       }
           // ----------------------------- Faces horizontales
           // ----------------------------- + Hexas
   Quad* qa = qbase;
   Quad *qb, *qc, *qd, *qe, *qf;
   int nv0 = hauteur*ind_poutre [0];
   int nv1 = hauteur*ind_poutre [1];
   int nv2 = hauteur*ind_poutre [2];
   int nv3 = hauteur*ind_poutre [3];
   for (int nh=0 ; nh<hauteur ; nh++)
       {
       qb = newQuad (tab_edge [nh+nv0], tab_edge [nh+nv1], 
                     tab_edge [nh+nv2], tab_edge [nh+nv3]);
       qc = tab_quad [nh + nv0];
       qd = tab_quad [nh + nv2];
       qe = tab_quad [nh + nv1];
       qf = tab_quad [nh + nv3];

// *** tab_hexa [nh*hauteur + nro] = newHexa (qa, qb, qc, qd, qe, qf); Abu 
       tab_hexa [nro*hauteur + nh] = newHexa (qa, qb, qc, qd, qe, qf);
       qa = qb;
       }
   return HOK;
}
// ====================================================== cutHexas
int Elements::cutHexas0 (const Edges& t_edges, int nbcuts)
{
                                       // 1) marquage des hexas
   el_root->markAll (NO_USED);
                                       // 2) Memo noeuds
   vector <Quad*> q_amont;
   vector <Quad*> q_aval;

   int nbnodes = t_edges.size();
   vector <Vertex*> v_amont (nbnodes);
   vector <Vertex*> v_aval  (nbnodes);

   std::map <Vertex*, Vertex*> vertex_aval;

   for (int nro=0; nro<nbnodes ; nro++)
       {
       Edge*   arete = t_edges [nro];
       Vertex* amont = arete->getAmont ();
       v_amont [nro] = amont;
       vertex_aval [amont] = arete->getAval  ();
       int nbcells   = arete->getNbrParents ();

       for (int nq=0 ; nq<nbcells ; nq++)
           {
           Quad* quad = arete->getParent (nq);
           if (quad->getMark () != IS_USED)
              {
              quad->setMark (IS_USED);
              int nbcubes = quad->getNbrParents ();
              for (int nh=0 ; nh<nbcubes ; nh++)
                  {
                  Hexa* hexa = quad->getParent (nh);
                  if (hexa->getMark () != IS_USED)  
                     {
                     hexa->setMark (IS_USED);
                     int namont = hexa->getBase (v_amont[nro], arete);
                     int naval  = glob->getOpposedQuad (namont);
                     q_amont.push_back (hexa->getQuad  (namont));
                     q_aval .push_back (hexa->getQuad  (naval ));
 
                     if (db)
                        {
                        printf (" Quad = ");
                        hexa->printName (", ");
                        printf (" Faces = (");
                        hexa->getQuad (namont)->printName (", ");
                        hexa->getQuad (naval )->printName (")\n");
                        }
                     }
                  }
              }
           }
       }
                   // ------------------- Dimensionnement
   int hauteur = nbcuts + 1;
   int nbcells = q_amont.size ();
   nbr_hexas   = nbcells * hauteur;

   tab_hexa.resize   (nbr_hexas);
   tab_quad.clear ();          // verticaux
   tab_edge.clear ();
   tab_pilier.clear ();
   tab_vertex.clear ();

   for (int ned=0; ned<nbnodes ; ned++)
       t_edges [ned]->remove ();


   for (int nro=0; nro<nbcells ; nro++)
       {
       int tcorr [QUAD4];
       Quad* sol  = q_amont[nro];
       Quad* toit = q_aval [nro];

       for (int nv=0; nv<QUAD4 ; nv++)
           {
           Vertex* dest = vertex_aval [sol->getVertex(nv)];
           tcorr [nv] = toit->indexVertex (dest);
           }

       pushHexas (nro, sol, 0,0,0, hauteur, toit, tcorr);
       }
   return HOK;
}
// ====================================================== makeCylinder
int Elements::makeCylinder (Cylinder* cyl, Vector* vx, int nr, int na, int nl)
{
   Vertex* orig = cyl->getBase ();
   Vector* dir  = cyl->getDirection ();
   double  ray  = cyl->getRadius ();
   double  haut = cyl->getHeight ();
   
   resize (GR_CYLINDRIC, nr, na, nl);
   cyl_closed = true;
   makeCylindricalNodes (orig, vx, dir, ray, 360, haut, nr, na, nl, true);
   fillGrid ();
   return HOK;
}
// ====================================================== makePipe
int Elements::makePipe (Cylinder* cyl, Vector* vx, int nr, int na, int nl)
{
   Vertex* orig = cyl->getBase ();
   Vector* dir  = cyl->getDirection ();
   double  ray  = cyl->getRadius ();
   double  haut = cyl->getHeight ();
   
   resize (GR_CYLINDRIC, nr, na, nl);
   cyl_closed = true;
   makeCylindricalNodes (orig, vx, dir, ray, 360, haut, nr, na, nl, false);
   fillGrid ();
   return HOK;
}

END_NAMESPACE_HEXA
