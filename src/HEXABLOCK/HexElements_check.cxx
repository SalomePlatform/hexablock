
// C++ : Controle arguments

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
#include "HexHexa.hxx"
#include "HexQuad.hxx"
#include "HexEdge.hxx"
#include "HexVertex.hxx"

#include "HexGlobale.hxx"

BEGIN_NAMESPACE_HEXA

double calcul_phimax (double radhole, double radext, bool sphere);

// ======================================================== checkSystem
void Elements::checkSystem (int arg, Vector* vx, Vector* vy, Vector* vz,
                            double* ux, double* uy, double* uz)
{
   int jer [DIM3];
   jer[dir_x] = vx->getUnitVector (ux); 
   jer[dir_y] = vy->getUnitVector (uy); 
   jer[dir_z] = vz->getUnitVector (uz); 

   bool sortie = false;
   for (int dd=dir_x ; dd<=dir_z ; dd++)
       if (jer[dd]!=HOK)
          {
          setError (HERR);
          sortie = true;
          Mess << "Vector number " << dd+1 << " is null" ;
          }
   if (sortie)
       return;

   double prod = prod_mixte (ux, uy, uz);
   if (prod > -Epsil && prod < Epsil)
      {
      setError (HERR);
      Mess << "These vectors are coplanar" ; 
      }
}
// ======================================================== checkAxis
void Elements::checkAxis (Vector* vz, double* uz)
{
   int ier = vz->getUnitVector (uz); 
   if (ier != HOK)
      {
      setError (HERR);
      Mess << " Second argument vector is null" ;
      }
}
// ======================================================== checkDiams
void Elements::checkDiams (int arg, double rint, double rext)
{
   if (rint < 0.0) 
      {
      setError (HERR);
      Mess << "Argument number " << arg << " :";
      Mess << "The internal radius must be positive";
      }
   if (rext < 0.0) 
      {
      setError (HERR);
      Mess << "Argument number " << arg+1 << " :";
      Mess << "The external radius must be positive";
      }
   if (rext <= rint ) 
      {
      setError (HERR);
      Mess << "The internal radius (arg" << arg << " = " << rint
           << ") must be smaller than the external (arg" << arg+1
           << " = " << rext << ")";
      }
}
// ======================================================== checkSystem
void Elements::checkSystem (int arg, Vector* vx, Vector* vz, 
                                     double* ux, double* uz)
{
   int ier1 = vx->getUnitVector (ux); 
   int ier2 = vz->getUnitVector (uz); 

   if (ier1!=HOK)
      {
      setError (HERR);
      Mess << "First vector is null" ;
      return;
      }
   else if (ier2!=HOK)
      {
      setError (HERR);
      Mess << "Second vector is null" ;
      return;
      }

   Real3 pv;
   prod_vectoriel (ux, uz, pv);
   double prod = calc_norme (pv);
   if (prod > -Epsil && prod < Epsil)
      {
      setError (HERR);
      Mess << "These two vectors are colinear" ; 
      }
}
// ======================================================== checkSize
void Elements::checkSize (int arg, int nx, int ny, int nz, bool rad)
{
   if (nx <= 0) 
      {
      Mess << "Argument number " << arg << " :";
      Mess << "This dimension must be positive";
      setError (HERR);
      }
   if (nz <= 0) 
      {
      Mess << "Argument number " << arg+2 << " :";
      Mess << "This dimension must be positive";
      setError (HERR);
      }

   if (rad)
      {
      if (ny < 3) 
         {
         Mess << "Argument number " << arg+1 << " :";
         Mess << "The number of sectors must be greather than 3"; 
         setError (HERR);
         }
      }
   else if (nx < 1) 
      {
      Mess << "Argument number " << arg+1 << " :";
      Mess << "This dimension must be positive";
      setError (HERR);
      }
}
// ======================================================== checkVector
void Elements::checkVector (int arg, RealVector& table, int lgmin, bool relative)
{
   int nbre = table.size();
   if (nbre<lgmin) 
      {
      setError (HERR);
      Mess << "Argument number " << arg+1 << " :";
      Mess << "Size of real vector must be greather or equal than " << lgmin; 
      return;
      }

   for (int nv=0 ; nv<nbre ; nv++)
       {
       double val = table [nv];

       if (relative && (val<=0.0 || val >= 1.0))
          {
          setError (HERR);
          Mess << "Argument number " << arg+1 << " :";
          Mess << "This vector contains relative lengths" ; 
          Mess << "Values must be between 0 and 1" ; 
          Mess <<    "(value[" << nv   << "] = " << val;
          return;
          }

       if (nv>0 && val <= table [nv-1])
          {
          setError (HERR);
          Mess << "Argument number " << arg+1 << " :";
          Mess << "Vector is not growing" ; 
          Mess <<    "(value[" << nv   << "] = " << val
               << " <= value[" << nv-1 << "] = " << table [nv-1]  ; 
          return;
          }
       }
}
// ======================================================== checkPipe
void Elements::checkPipe (int arg, double rint, double rext, double angle, 
                                                             double hauteur)
{
   if (rint <= Epsil)
      {
      setError (HERR);
      Mess << "Argument number " << arg << " :";
      Mess << "Internal radius (=" << rint << ") must be greather than 0"; 
      }
   if (rext <= Epsil)
      {
      setError (HERR);
      Mess << "Argument number " << arg+1 << " :";
      Mess << "External radius (=" << rext << ") must be greather than 0"; 
      }
   if (rint >= rext)
      {
      setError (HERR);
      Mess << "Arguments number " << arg << " and " << arg+1 << " :";
      Mess << "Internal radius (" << rint 
           << ") must be smaller than external (=" << rext << ")"; 
      }
   if (angle <= Epsil)
      {
      setError (HERR);
      Mess << "Argument number " << arg+2 << " :";
      Mess << "Angle (=" << angle << ") must be greather than"; 
      }
   if (hauteur <= Epsil)
      {
      setError (HERR);
      Mess << "Argument number " << arg+3 << " :";
      Mess << "Height (=" << hauteur << ") must be greather than"; 
      }
}
// ======================================================== checkOrig
void Elements::checkOrig (int arg, Vertex* orig)
{
   if (orig==NULL || orig->isBad())
      {
      setError (HERR);
      Mess << "Argument number " << arg << " :";
      Mess << "Origin vertex is not valid"; 
      }
}
// ======================================================== checkQuads
void Elements::checkQuads (Quads& tquad)
{
   int nbre = tquad.size();
   if (nbre==0)
      {
      setError (HERR);
      Mess << "Start quads list is empty";
      }

   for (int nro=0 ; nro<nbre ; nro++)
       {
       checkQuad (tquad [nro], nro+1);
       }
}
// ======================================================== checkQuad
void Elements::checkQuad (Quad* quad, int nro)
{
   if (quad==NULL || quad->isBad())
      {
      setError (HERR);
      if (nro>0) 
         Mess << "Start quad nr " << nro << " is not valid"; 
      else if (nro<0) 
         Mess << "Target quad must is not valid"; 
      else 
         Mess << "Start quad is not valid"; 
      return;
      }

   if (quad->getNbrParents () == 2)
      {
      setError (HERR);
      if (nro>0) 
         Mess << "Start quad nr " << nro << " must be an external face"; 
      else if (nro<0) 
         Mess << "Target quad must be an external face"; 
      else 
         Mess << "Start quad must be an external face"; 
      return;
      }
}
// ======================================================== checkSense
void Elements::checkSense (int nro, Vertex* v1, Vertex* v2, Quad* quad)
{
   if (v1==NULL || v1->isBad())
      {
      setError (HERR);
      Mess << "Argument nr " << nro << " : vertex is not valid"; 
      return;
      }

   if (v2==NULL || v2->isBad())
      {
      setError (HERR);
      Mess << "Argument nr " << nro+1 << " : vertex is not valid"; 
      return;
      }
   Edge* edge = quad -> findEdge (v1, v2);
   if (edge!=NULL) 
       return;

   cpchar where = nro < 4 ? "start" : "target";
   setError (HERR);
   Mess << "Arguments nr " << nro << " and " << nro+2 
        << " : these vertices doenst define an edge of the "
        << where << " quad";
}
// ======================================================== checkPhi
int Elements::checkPhi (bool sphere, double* orig, double* norm, double rmax, 
                         double rhole, Vertex* plan, double& phi0, double& phi1)
{
   int    ier  = HOK;
   double beta = -M_PI/2;

   if (plan!=NULL)
      {
      Real3 oplan;
      plan->getPoint (oplan);

      Real3  om   = { oplan[0]-orig[0], oplan[1]-orig[1], oplan[2]-orig[2] };
      double hmin = prod_scalaire (norm, om);
      if (hmin >= rmax)
         {
         ier = HERR;
         setError (ier);
         Mess << "Sphere is under the horizontal plan";
         }
      else if (hmin > -rmax)
         beta = asin (hmin/rmax);
      }

   phi1 = calcul_phimax (rhole, rmax, sphere);
   phi0 = std::max (-phi1, beta);

   phi1 = rad2degres (phi1);
   phi0 = rad2degres (phi0);
   return ier;
}
// ======================================================== checkInter
/* === Intersection de 2 cylindres (A,ua) et (B,ub)
                               ua et ub : vecteurs directeurs unitaires.
      A a le gros diametre,  B le petit
      H = proj ortho de B sur (A, ua)
      AH = AB . ua
      (mesure algebrique de AH = produit scalaire de AB par ua

    ---------------------------------------------------------------- */
int Elements::checkInter (double* pa, double* ua, double ra, double lga, 
                          double* pb, double* ub, double rb, double lgb, 
                          double* center, bool& left, bool& right)
{
   int ier = HOK;
   left = right = true;

   Real3 ab, cprim;
   calc_vecteur (pa, pb, ab);
   double ah =  prod_scalaire (ab, ua);   // ah > 0 ???? : A voir avec FK
   double bh = -prod_scalaire (ab, ub); 
 
   for (int dd=dir_x ; dd<=dir_z ; dd++)
       {
       center [dd] = pa [dd] + ah * ua [dd];
       cprim  [dd] = pb [dd] + bh * ub [dd];
       }
                              // Controles : 
   ah = fabs (ah);
   bh = fabs (bh);

   double dist  = calc_distance (center, cprim);
   double dcmin = lga*0.05;
   double lgmin = (ah - rb)*1.1;

   if (ah < ra )
      {
      ier = HERR;
      setError (ier);
      Mess << "The base of the big cylinder is included in the smaller" ;
      }
   else if (lga < lgmin)
      {
      ier = HERR;
      setError (ier);
      Mess << "The big cylinder is not long enough too reach the smaller" ;
      Mess << "Actual lengh   = " << lga;
      Mess << "Minimal length = " << lgmin;
      }
   else if (lgb < bh - ra)
      {
      ier = HERR;
      setError (ier);
      Mess << "The small cylinder is not long enough too reach the large" ;
      Mess << "Actual lengh   = " << lgb;
      Mess << "Minimal length = " << bh-rb;
      }
   else if (dist > dcmin)
      {
      ier = HERR;
      setError (ier);
      Mess << "Cylinders axes are not secant";
      Mess << "Distance between axes = " << dist;
      }
   else 
      {
      if (bh < rb) 
         left = false;
      if (bh + rb > lgb ) 
         right = false;
      }

   if (NOT left && NOT right)
      {
      ier = HERR;
      setError (ier);
      Mess << "The small cylinder is included in the large" ;
      }
     
   return ier;
}
// ======================================================== checkDisco
void Elements::checkDisco (Hexa* cell, Vertex* element)
{
   if (BadElement (cell))
      {
      Mess << "Hexaedron is not valid";
      setError (HERR);
      return;
      }

   if (BadElement (element))
      {
      Mess << "Vertex is not valid";
      setError (HERR);
      return;
      }

   int node = cell->findVertex (element);
   if (node==NOTHING)
      {
      setError (HERR);
      Mess << "Vertex doesn't belong to Hexaedron";
      return;
      }
}
// ======================================================== checkDisco
void Elements::checkDisco (Hexa* cell, Edge* element)
{
   if (BadElement (cell))
      {
      Mess << "Hexaedron is not valid";
      setError (HERR);
      return;
      }

   if (BadElement (element))
      {
      Mess << "Edge is not valid";
      setError (HERR);
      return;
      }

   int node = cell->findEdge (element);
   if (node==NOTHING)
      {
      setError (HERR);
      Mess << "Edge doesn't belong to Hexaedron";
      return;
      }
}
// ======================================================== checkDisco
void Elements::checkDisco (Hexa* cell, Quad* element)
{
   if (BadElement (cell))
      {
      Mess << "Hexaedron is not valid";
      setError (HERR);
      return;
      }

   if (BadElement (element))
      {
      Mess << "Quad is not valid";
      setError (HERR);
      return;
      }

   int node = cell->findQuad (element);
   if (node==NOTHING)
      {
      setError (HERR);
      Mess << "Quadrangle doesn't belong to Hexaedron";
      return;
      }
}
// ======================================================== checkDisco
void Elements::checkDisco (Hexas& thexas, Edges& tedges)
{
   int nbedges = tedges.size();
   int nbhexas = thexas.size();

   if (nbhexas != nbedges) 
      {
      Mess << " **** Error in Document::disconnectEdges";
      Mess << " **** Number of Edges and number of Hexas are different";
      setError (HERR);
      return;
      }

   for (int nro=0 ; nro<nbedges ; nro++)
       {
       Edge*  edge = tedges [nro];
       Hexa*  hexa = thexas [nro];
       if (BadElement (edge))
          {
          Mess << " **** Eddge number " << nro+1 << " is incorrect";
          setError (HERR);
          return;
          }

       if (BadElement (hexa))
          {
          Mess << " **** Hexa number " << nro+1 << " is incorrect";
          setError (HERR);
          return;
          }

       int ned = hexa->findEdge (edge);
       if (ned==NOTHING)
          {
          Mess << " **** Edge number " << nro+1 
               << " doesnt belong to corresponding hexa";
          setError (HERR);
          return;
          }
       if (nro>0)
          {
          Vertex* vertex = edge->commonVertex (tedges[nro-1]);
          if (vertex==NULL)
             {
             setError (HERR);
             Mess << " **** Edge number " << nro 
                  << " doesnt intesect next edge";
             }
          }
       }
}
// ====================================================== checkContour 
void Elements::checkContour (Quads& tquad, Vertex* v1, Vertex* v2, bool target,
                             Edges& tedge)
{
   tedge.clear ();
   cpchar who    = target ? "Target" : "Pattern";
   string nmedge = target ? "Vertices of target (args 5 and 6)" 
                          : "Vertices of pattern (args 3 and 4)" ;
   nmedge += "don't define an edge" ;

   Edge*  edge1 = el_root->findEdge (v1, v2);
   if (BadElement (edge1))
      {
      setError (HERR);
      Mess << nmedge;
      return;
      }

   map <Edge*, int> edge_count;
   map <Edge*, int> :: iterator iter;
   int nbre = tquad.size();
   for (int nq=0 ; nq<nbre ; ++nq)
        {
        Quad* quad = tquad [nq];
        if (BadElement (quad)) 
           {
           setError (HERR);
           Mess << who << " quad nr " << nq+1 << " is not valid";
           return;
           }
         else if (target && quad->getNbrParents() != 1)
           {
           setError (HERR);
           Mess << " Target quad nr " << nq+1 
                << " must be an external face";
           return;
           }
       
        for (int ned=0 ; ned<QUAD4 ; ++ned)
            {
            Edge* edge = quad->getEdge (ned);
            edge_count [edge] += 1;
            }
        }
   
   int pos1 = edge_count [edge1];
   if (pos1==0)
       {
       setError (HERR);
       Mess << nmedge << " of the " << who << " quads";
       return;
       }
   else if (pos1==2)
       {
       setError (HERR);
       Mess << nmedge << " of the " << who << " contour";
       return;
       }

   tedge.push_back (edge1);
   Vertex* vlast = v2;
   Edge*   elast = edge1;
   while (vlast != v1)
         {
         int   nbre  = vlast->getNbrParents(); 
         Edge* enext = NULL;
         for (int ned=0 ; ned<nbre && enext == NULL; ++ned)
             {
             Edge* edge = vlast->getParent(ned);
             if (edge != elast && edge_count [edge]==1)
                 enext = edge;
             }
         if (enext==NULL)
            {
            setError (HERR);
            Mess << who << " as an unclosed contour";
            return;
            }
         tedge.push_back (enext);
         vlast = enext->opposedVertex (vlast);
         elast = enext;
         }
}
// ====================================================== checkContour 
void Elements::checkContour (Quads& tquad, Vertex* v1, Vertex* v2, bool target,
                             Vertices& tvertex)
{
   tvertex.clear ();
   cpchar who    = target ? "Target" : "Pattern";
   string nmedge = target ? "Vertices of target (args 4 and 6)" 
                          : "Vertices of pattern (args 3 and 5)" ;
   nmedge += "don't define an edge" ;

   Edge*  edge1 = el_root->findEdge (v1, v2);
   if (BadElement (edge1))
      {
      setError (HERR);
      Mess << nmedge;
      return;
      }

   map <Edge*, int> edge_count;
   map <Edge*, int> :: iterator iter;
   int nbre = tquad.size();
   for (int nq=0 ; nq<nbre ; ++nq)
        {
        Quad* quad = tquad [nq];
        if (BadElement (quad)) 
           {
           setError (HERR);
           Mess << who << " quad nr " << nq+1 << " is not valid";
           return;
           }
         else if (target && quad->getNbrParents() != 1)
           {
           setError (HERR);
           Mess << " Target quad nr " << nq+1 
                << " must be an external face";
           return;
           }
       
        for (int ned=0 ; ned<QUAD4 ; ++ned)
            {
            Edge* edge = quad->getEdge (ned);
            edge_count [edge] += 1;
            }
        }
   
   int pos1 = edge_count [edge1];
   if (pos1==0)
       {
       setError (HERR);
       Mess << nmedge << " of the " << who << " quads";
       return;
       }
   else if (pos1==2)
       {
       setError (HERR);
       Mess << nmedge << " of the " << who << " contour";
       return;
       }

   tvertex.push_back (v1);
   Vertex* vlast = v2;
   Edge*   elast = edge1;
   while (vlast != v1)
         {
         tvertex.push_back (vlast);
         int   nbre  = vlast->getNbrParents(); 
         Edge* enext = NULL;
         for (int ned=0 ; ned<nbre && enext == NULL; ++ned)
             {
             Edge* edge = vlast->getParent(ned);
             if (edge != elast && edge_count [edge]==1)
                 enext = edge;
             }
         if (enext==NULL)
            {
            setError (HERR);
            Mess << who << " as an unclosed contour";
            return;
            }
         vlast = enext->opposedVertex (vlast);
         elast = enext;
         }
}
// ======================================================== calcul_phimax
double calcul_phimax (double radhole, double radext, bool sphere)
{
   double phi = asin (radhole/radext);
   if (sphere)
       phi = 2*phi;
   phi = M_PI*DEMI - phi;
   return phi;
}

END_NAMESPACE_HEXA
