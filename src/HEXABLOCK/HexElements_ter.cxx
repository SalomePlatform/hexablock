// C++ : Table d'hexaedres (Evol Versions 3)

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

#include "HexElements.hxx"

#include "HexDocument.hxx"
#include "HexVector.hxx"
#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexHexa.hxx"
#include "HexMatrix.hxx"
#include "HexGlobale.hxx"

#include "HexNewShape.hxx"
#include "HexEdgeShape.hxx"
#include "HexAssoEdge.hxx"

#include <cmath>

BEGIN_NAMESPACE_HEXA

static bool db=false;

// ====================================================== getCylPoint
int Elements::getCylPoint (int nr, int na, int nh, double& px, double& py,
                           double& pz)
{
   if (grid_type == GR_CYLINDRIC)
      {
      px = cyl_radext * cos (na*cyl_dtheta);
      py = cyl_radext * sin (na*cyl_dtheta);
      pz = cyl_length * nh;
      return HOK;
      }

   bool   rind   = (grid_type == GR_RIND || grid_type == GR_PART_RIND);
   double sinphi = sin (cyl_phi0 + nh*cyl_dphi);
   double cosphi = cos (cyl_phi0 + nh*cyl_dphi);

   double rayon = 0;
   if (rind)
      {
      rayon = cyl_radint + nr*(cyl_radext-cyl_radint)/size_hx;
      pz    = rayon*sinphi;
      rayon = rayon*cosphi;
      }
   else
      {
      pz    = cyl_radext*sinphi;
      rayon = cyl_radhole + nr*(cyl_radext*cosphi - cyl_radhole)/size_hx;
      rayon = std::max (cyl_radhole, rayon);
      }

   px = rayon * cos (na*cyl_dtheta);
   py = rayon * sin (na*cyl_dtheta);

   return HOK;
}
// ====================================================== controlRind
int Elements::controlRind (EnumGrid type, Vertex* cx, Vector* vx, Vector* vz,
                           double rext, double rint, double rhole,
                           Vertex* px, double angle,
                           int nrad, int nang, int nhaut,
                           double &phi0, double &phi1)
{
   const double Epsil1 = 1e-6;
   phi0  = phi1 = 0;

   if (cx == NULL || vx == NULL || vz == NULL)
      return HERR;

   if (nrad<=0 || nang<=0 || nhaut<=0)
      return HERR;

   if (rext <= 0.0)
      return HERR;

   if (rint >= rext)
      return HERR;

   if (rhole > rint)
      return HERR;

   double nvx = vx->getNorm();
   double nvz = vz->getNorm();

   if (nvx < Epsil1 || nvz <  Epsil1)
      return HERR;

   double alpha = asin (rhole/rext);
   double beta  = -M_PI*DEMI;
   if (type==GR_HEMISPHERIC || type==GR_PART_SPHERIC)
       alpha = 2*alpha;

   if (px!=NULL)
      {
          // oh = oa.n/|n|
      double oh = ((px->getX() - cx->getX()) * vz->getDx()
                +  (px->getY() - cx->getY()) * vz->getDy()
                +  (px->getZ() - cx->getZ()) * vz->getDz()) / nvz;
      if (oh > rext)
         return HERR;
      else if (oh > -rext)
         beta  = asin (oh/rext);
      }

   phi0 = std::max (alpha - M_PI*DEMI, beta);
   phi1 = M_PI*DEMI - alpha;
   return HOK;
}
// ====================================================== getHexas
int Elements::getHexas (Hexas& liste)
{
   liste.clear ();
   for (int nro = 0 ; nro<nbr_hexas ; nro++)
       {
       Hexa* cell = tab_hexa [nro];
       if (cell!=NULL && cell->isValid())
          liste.push_back (cell);
       }
   return HOK;
}
// ====================================================== assoCylinder
void Elements::assoCylinder (Vertex* ori, Vector* normal, double angle)
{
   if (normal==NULL || ori==NULL)
       return;

   RealVector tangles;
   Real3      vz, center;
   normal->getCoord (vz);
   ori   ->getPoint (center);
   assoCylinders (center, vz, angle, tangles);
}
// ====================================================== assoCylinders
void Elements::assoCylinders (Vertex* ori, Vector* normal, double angle,
                              RealVector& t_angles)
{
   if (normal==NULL || ori==NULL)
       return;

   Real3 vz, center;
   normal->getCoord (vz);
   ori   ->getPoint (center);
   assoCylinders (center, vz, angle, t_angles);
}
// ====================================================== assoCylinders
void Elements::assoCylinders (double* ori, double* vk, double angle,
                              RealVector& t_angles)
{
   char     name [12];
   sprintf (name, "grid_%02d", el_id);
   NewShape* geom = el_root->addShape (name, SH_CYLINDER);
   geom -> openShape();

   string brep;
   // Real3 vk = { normal->getDx(), normal->getDy(), normal->getDz() };
   // normer_vecteur (vk);

   for (int nz=0 ; nz<size_vz ; nz++)
       {
       for (int nx=0 ; nx<size_vx ; nx++)
           {
           Vertex* pm = getVertexIJK (nx, 0, nz);
           Real3   om = { pm->getX() - ori[dir_x], 
                          pm->getY() - ori[dir_y], 
                          pm->getZ() - ori[dir_z] };

           double oh     = prod_scalaire (om, vk);
           double rayon  = 0;
           Real3  ph, hm;
           for (int dd=dir_x; dd<=dir_z ; dd++)
               {
               ph [dd] = ori[dd] + oh*vk[dd];
               hm [dd] = pm ->getCoord(dd) - ph[dd];
               rayon  += hm[dd] * hm[dd];
               }

           rayon = sqrt (rayon);
           int subid = geom->addCircle (ph, rayon, vk, hm);

           for (int ny=0 ; ny<size_hy ; ny++)
               {
               double pmin = t_angles [ny]/360;
               double pmax = t_angles [ny+1]/360;
               Edge*  edge = getEdgeJ (nx, ny, nz);
               geom->addAssociation (edge, subid, pmin, pmax);
               if (db) cout << " assoCylinders : ny= " << ny 
                            << ", nz= " << nz << " param = (" 
                            << pmin << ", " << pmax  << ")\n";
               }
           }
       }
   // Association automatique des vertex non associes -> bph
   // Traitement des faces spheriques

   Real3 vi = { -vk[dir_x],  -vk[dir_y],  -vk[dir_z] };

   switch (grid_type)
      {
      case GR_HEMISPHERIC  :    // Pour l'exterieur
      case GR_PART_SPHERIC :
           assoRind (ori, vi, size_vx-1, geom);
           break;
      case GR_PART_RIND    :    // Exterieur + interieur
      case GR_RIND         :
           assoRind (ori, vi, 0, geom);
           assoRind (ori, vi, size_vx-1, geom);
           break;
      default :
           break;
      }
   geom->closeShape ();
}
// ====================================================== calcul_param
double calcul_param (double* ori, double* vi, Vertex* vert)
{
   Real3  pnt, vj;
   vert->getPoint (pnt);
   calc_vecteur (ori, pnt, vj);
   normer_vecteur  (vj);
   double kos = prod_scalaire (vi, vj);
   double alpha = acos (kos) / (2*M_PI);
   return alpha;
}
// ====================================================== assoRind
// Association des meridiennes
// Creation sphere geometrique + association faces
void Elements::assoRind (double* ori, double* vi, int nx, NewShape* geom)
{
   Real3  vk;
   double radius  = nx==0 ? cyl_radint : cyl_radext;
   int    sphid   = geom->addSphere (ori, radius);

   int nz1 = size_vz/2;
   int nb_secteurs = cyl_closed ? size_vy-1 : size_vy;

   for (int ny=0 ; ny<nb_secteurs ; ny++)
       {
       Vertex* pm = getVertexIJK (nx, ny, nz1);
       Real3   vj = { pm->getX(), pm->getY(), pm->getZ() };
       prod_vectoriel (vi, vj, vk);
       double rayon = cyl_radint + nx*(cyl_radext-cyl_radint)/size_hx;
       int    subid = geom->addCircle (ori, rayon, vk, vi);

       for (int nz=0 ; nz<size_hz ; nz++)
           {
           Quad* quad  = getQuadJK (nx, ny, nz);
           Edge* edge  = getEdgeK  (nx, ny, nz);
           Vertex* nd1 = edge->getVertex (V_AMONT);
           Vertex* nd2 = edge->getVertex (V_AVAL);
           double pmin = calcul_param (ori, vi, nd1);
           double pmax = calcul_param (ori, vi, nd2);
           cout << " assoRind : ny= " << ny << ", nz= " << nz 
                << " param = (" << pmin << ", " << pmax  << ")\n";

           geom->addAssociation (edge, subid, pmin, pmax);
           geom->addAssociation (quad, sphid);
           }
       }
}
// ====================================================== assoCircle
// ==== utilise pour les spheres carrees
void Elements::assoCircle (double* center, Edge* ed1, Edge* ed2, NewShape* geom)
{
   Real3 oa,  ob, normal;
   Real3 pta, ptb, ptc, ptd;
   string brep;

//  Les 2 edges dont les petits cotes d'un rectangle de rapport L/l=sqrt(2)
//  Soit le cercle circonscrit a ce rectangle.
//    * la largeur est balayee par l'angle alpha
//    * la longueur par l'angle beta = pi -alpha

   ed1->getVertex(V_AMONT)->getPoint (pta);
   ed1->getVertex(V_AVAL )->getPoint (ptb);
   ed2->getVertex(V_AMONT)->getPoint (ptc);
   ed2->getVertex(V_AVAL )->getPoint (ptd);

   double d1 = calc_distance (pta, ptc);
   double d2 = calc_distance (pta, ptd);

   if (d1 < d2)
      {
      ed2->getVertex(V_AMONT)->getPoint (ptd);
      ed2->getVertex(V_AVAL )->getPoint (ptc);
      }

   calc_vecteur   (center, pta, oa);
   calc_vecteur   (center, ptb, ob);
   prod_vectoriel (oa, ob, normal);

   double rayon = calc_norme (oa);
   int    subid = geom->addCircle (center, rayon, normal, oa);

   const double alpha = atan (sqrt(2.)/2)/M_PI; //  angle proche de 70.528 degres
   // asso1->setBounds (0,   alpha);
   // asso2->setBounds (0.5, alpha + 0.5);

   geom->addAssociation (ed1, subid, 0.0, alpha);
   geom->addAssociation (ed2, subid, 0.5, alpha+0.5);
}
// ====================================================== assoSphere
void Elements::assoSphere (double* center, Edge* t_edge[], Quad* t_quad[])
{
   char     name [12];
   sprintf (name, "grid_%02d", el_id);
   NewShape* geom = el_root->addShape (name, SH_SPHERE);
   geom -> openShape ();

   Real3 sommet;

   assoCircle (center, t_edge [E_AC], t_edge [E_BD], geom);
   assoCircle (center, t_edge [E_AD], t_edge [E_BC], geom);
   assoCircle (center, t_edge [E_AE], t_edge [E_BF], geom);
   assoCircle (center, t_edge [E_AF], t_edge [E_BE], geom);
   assoCircle (center, t_edge [E_CE], t_edge [E_DF], geom);
   assoCircle (center, t_edge [E_CF], t_edge [E_DE], geom);

   t_edge[E_AC]->getVertex(V_AMONT)->getPoint (sommet);
   double radius = calc_distance (center, sommet);

   int sphid = geom -> addSphere (center, radius);
   geom->closeShape();

   for (int nf=0 ; nf < HQ_MAXI ; nf++)
       t_quad[nf]->addAssociation (geom, sphid);
}
// ==================================================== propagateAssociation
int Elements::propagateAssociation (Edge* orig, Edge* dest, Edge* dir)
{
    return HERR;
#if 0
   if (revo_lution || orig==NULL || dest==NULL || dir==NULL)
      return HERR;

   const Shapes& tab_shapes = orig->getAssociations ();
   const Shapes& tab_dest   = dest->getAssociations ();
   int   nbdest             = tab_dest.size();
   int   nbshapes           = tab_shapes.size();
   bool  on_edge            = nbshapes!=0 && nbdest==0;

   Vertex* vo1 = orig->commonVertex  (dir);
   Vertex* vd1 = dest->commonVertex  (dir);
   Vertex* vo2 = orig->opposedVertex (vo1);
   Vertex* vd2 = dest->opposedVertex (vd1);

   if (vo1==NULL || vd1==NULL)
      return HERR;

   string  trep;
   Real3   pa, pb, vdir1, vdir2;
   calc_vecteur (vo1->getPoint (pa), vd1->getPoint (pb), vdir1);
   calc_vecteur (vo2->getPoint (pa), vd2->getPoint (pb), vdir2);

   double dd = calc_distance (vdir1, vdir2);
   bool para = dd < 1.0e-3;

   if (para && on_edge)
      {
      for (int nro=0 ; nro<nbshapes ; nro++)
          {
          Shape* shape  = tab_shapes[nro];
          if (shape!=NULL)
             {
             string brep   = shape->getBrep();
             translate_brep (brep, vdir1, trep);
             // Shape* tshape = new Shape (trep);
             // tshape->setBounds (shape->getStart(), shape->getEnd());
             // dest->addAssociation (tshape);
             }
          }
      }

   double* vdir = vdir1;
   for (int nro=V_AMONT ; nro<=V_AVAL ; nro++)
       {
       Shape* shape = vo1->getAssociation ();
       if (shape!=NULL && vd1->getAssociation ()==NULL)
          {
          string brep   = shape->getBrep();
          translate_brep (brep, vdir, trep);
          // Shape* tshape = new Shape (trep);
          // vd1->setAssociation (tshape);
          }
       vo1  = vo2;
       vd1  = vd2;
       vdir = vdir2;
       }
   return HOK;
#endif
}
// ==================================================== prismAssociation
int Elements::prismAssociation (Edge* lorig, Edge* ldest, int nh)
{
   if (lorig==NULL || ldest==NULL)
      return HERR;

   updateMatrix (nh);
   int nbshapes = lorig->countAssociation ();
   if (nbshapes==0)
       return HOK;

   NewShape* new_shape = getShape ();
   for (int nro=0 ; nro<nbshapes ; nro++)
       {
       AssoEdge*  asso  = lorig->getAssociation (nro);
       EdgeShape* shape = asso ->getEdgeShape();
       double     p1    = asso ->getStart();
       double     p2    = asso ->getEnd  ();
       int        subid = 0;
       char       name [24];

       sprintf (name, "0x%lx#%d", (unsigned long) shape, nh);
       map<string,int>::iterator iter = map_shape.find (name);
       if (iter != map_shape.end())
          subid = iter->second;
       else
          subid = map_shape[name] = new_shape->transfoShape (cum_matrix, shape);

       new_shape->addAssociation (ldest, subid, p1, p2);
       printf (" prismAsso : %s -> %s(%d)  = %s [ %g , %g]\n",
                 lorig->getName(), ldest->getName(), nh, name, p1, p2);
       }
   return HOK;
}
// ====================================================== assoResiduelle
void Elements::assoResiduelle ()
{
   // int nbre = tab_vertex.size();
   // for (int nv=0 ; nv<nbre ; nv++)
       // {
       // geom_asso_point (tab_vertex [nv]);
       // }
}
// ====================================================== moveDisco
void Elements::moveDisco (Hexa* hexa)
{
   Real3  center;
   Matrix matrix;
   hexa->getCenter (center);
   matrix.defScale (center, 0.55);

   int nbre = tab_vertex.size();
   for (int nv=0 ; nv<nbre ; nv++)
       {
       matrix.perform (tab_vertex [nv]);
       }
}
// =================================================== getStrate
Hexa* Elements::getStrate (int couche, EnumHQuad nroface)
{
   Hexa* cell = NULL;
   int   nro  = couche <= 0 ? 0 : (couche-1)*HQ_MAXI + nroface + 1;

   if (nbr_hexas==0 || nro >= nbr_hexas)
      cell = NULL;
   else
      cell = tab_hexa [nro];

   return cell;
}
// ============================================================  setHexa
void Elements::setHexa (Hexa* elt, int nro)
{
   if (nro >=0 && nro < nbr_hexas)
       tab_hexa [nro] = elt;
}
// ============================================================  setQuad
void Elements::setQuad (Quad* elt, int nro)
{
   if (nro >=0 && nro < nbr_quads)
       tab_quad [nro] = elt;
}
// ============================================================  setEdge
void Elements::setEdge (Edge* elt, int nro)
{
   if (nro >=0 && nro < nbr_edges)
       tab_edge [nro] = elt;
}
// ============================================================  setVertex
void Elements::setVertex (Vertex* elt, int nro)
{
   if (nro >=0 && nro < nbr_vertex)
       tab_vertex [nro] = elt;
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// ============================================================  getHexa
Hexa* Elements::getHexa (int nro)
{
   DumpStart ("getHexa", nro);

   Hexa* elt = NULL;
   int  nombre=tab_hexa.size();
   // if (nro >=0 && nro < nbr_hexas && el_status == HOK Abu 2010/05/06
   if (nro >=0 && nro < nombre && el_status == HOK
               && tab_hexa [nro] != NULL && tab_hexa [nro]->isValid())
      elt = tab_hexa [nro];

   DumpReturn (elt);
   return elt;
}
// ============================================================  getQuad
Quad* Elements::getQuad (int nro)
{
   DumpStart ("getQuad", nro);

   Quad* elt = NULL;
   if (nro >=0 && nro < nbr_quads && el_status == HOK
               && tab_quad [nro] != NULL && tab_quad [nro]->isValid())
      elt = tab_quad [nro];

   DumpReturn (elt);
   return elt;
}
// ============================================================  getEdge
Edge* Elements::getEdge (int nro)
{
   DumpStart ("getEdge", nro);

   Edge* elt = NULL;
   if (nro >=0 && nro < nbr_edges && el_status == HOK
               && tab_edge [nro] != NULL && tab_edge [nro]->isValid())
      elt = tab_edge [nro];

   DumpReturn (elt);
   return elt;
}
// ============================================================  getVertex
Vertex* Elements::getVertex (int nro)
{
   DumpStart ("getVertex", nro);

   Vertex* elt = NULL;
   if (nro >=0 && nro <  nbr_vertex && el_status == HOK
               && tab_vertex [nro] != NULL && tab_vertex [nro]->isValid())
      elt = tab_vertex [nro];

   DumpReturn (elt);
   return elt;
}
// ============================================================  indVertex
int Elements::indVertex (int nquad, int nsommet, int nh)
{
   int nro = nsommet  + QUAD4*nquad + nbr_orig*QUAD4*(nh+1);
   return nro;
}
// ============================================================  nroVertex
int Elements::nroVertex (int nquad, int nsommet, int nh)
{
   int nro = nsommet  + QUAD4*nquad + nbr_orig*QUAD4*nh;
   return nro;
}
// ============================================================  indVertex
int Elements::indVertex (int ref_edge, int nh)
{
   int    nro = ref_edge + nbr_orig*QUAD4*nh;
   return nro;
}
// ============================================================  nroEdgeH
int Elements::nroEdgeH (int nvertex)
{
   return QUAD4*nbr_orig*gr_hauteur + nvertex;
}
// ============================================================  nroEdgeH
int Elements::nroEdgeH (int nquad, int nsommet, int nh)
{
   return QUAD4*nbr_orig*gr_hauteur + indVertex (nquad, nsommet, nh);
}
// ============================================================  nroHexa
int Elements::nroHexa (int nquad, int nh)
{
   int nro = gr_hauteur*nquad + nh;
   return nro;
}

// ============================================================  addHexa
void Elements::addHexa (Hexa* element)
{
   tab_hexa.push_back (element);
   nbr_hexas ++;
}
// ============================================================  addQuad
void Elements::addQuad (Quad* element)
{
   tab_quad.push_back (element);
   nbr_quads ++;
}
// ============================================================  addEdge
void Elements::addEdge (Edge* element)
{
   tab_edge.push_back (element);
   nbr_edges ++;
}
// ============================================================  addVertex
void Elements::addVertex (Vertex* element)
{
   tab_vertex.push_back (element);
   nbr_vertex ++;
}
// ============================================================  findHexa
int Elements::findHexa (Hexa* element)
{
   int nbre = tab_hexa.size();
   for (int nro=0 ; nro<nbre ; nro++)
       if (tab_hexa [nro] == element)
          return nro;
   return NOTHING;
}
// ============================================================  findQuad
int Elements::findQuad (Quad* element)
{
   int nbre = tab_quad.size();
   for (int nro=0 ; nro<nbre ; nro++)
       if (tab_quad [nro] == element)
          return nro;
   return NOTHING;
}
// ============================================================  findEdge
int Elements::findEdge (Edge* element)
{
   int nbre = tab_edge.size();
   for (int nro=0 ; nro<nbre ; nro++)
       if (tab_edge [nro] == element)
          return nro;
   return NOTHING;
}
// ============================================================  findVertex
int Elements::findVertex (Vertex* element)
{
   int nbre = tab_vertex.size();
   for (int nro=0 ; nro<nbre ; nro++)
       if (tab_vertex [nro] == element)
          return nro;
   return NOTHING;
}
// ========================================================= saveVtk (avec nro)
int Elements::saveVtk  (cpchar radical, int &nro)
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

END_NAMESPACE_HEXA
