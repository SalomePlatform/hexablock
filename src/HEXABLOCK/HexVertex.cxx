
//  C++ Les noeuds

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

#include "HexVertex.hxx"
#include "HexEdge.hxx"
#include "HexVector.hxx"

#include "HexXmlWriter.hxx"
#include "HexDocument.hxx"
#include "HexNewShape.hxx"
#include "HexVertexShape.hxx"
#include "HexKas_functions.hxx"

BEGIN_NAMESPACE_HEXA

static bool db =  on_debug ();  // == getenv ("HEXA_DB") > 0

// ====================================================== Constructeur
Vertex::Vertex  (Document* doc, double x, double y, double z)
      : EltBase (doc, EL_VERTEX)
{
   gc_x = v_x = x;
   gc_y = v_y = y;
   gc_z = v_z = z;

   v_shape  = NULL;
   v_scalar = 0;
   v_clone  = NULL;

   if (el_root != NULL)
       el_root->addVertex (this);
}
// ========================================================= Constructeur bis
Vertex::Vertex (Vertex* other)
      : EltBase (other->dad(), EL_VERTEX)
{
   if (other!= NULL)
      {
      v_x = other->v_x;
      v_y = other->v_y;
      v_z = other->v_z;
      v_scalar = other->v_scalar;
      gc_x   = other->gc_x;
      gc_y   = other->gc_y;
      gc_z   = other->gc_z;
      v_clone  = NULL;
      v_shape  = other->v_shape;
      }
   else
      {
      v_x  = v_y  = v_z  = 0;
      gc_x = gc_y = gc_z = 0;
      v_scalar = 0;
      v_shape  = NULL;
      v_clone  = NULL;
      }
}
// ========================================================= getParent
Edge* Vertex::getParent  (int nro)
{
   return static_cast <Edge*> (getFather (nro));
}
// ========================================================= saveXml
void Vertex::saveXml  (XmlWriter* xml)
{
   char buffer[12], coord[80];

   sprintf (coord, "%g %g %g", v_x, v_y, v_z);

   xml->openMark     ("Vertex");
   xml->addAttribute ("id",    getName (buffer));
   xml->addAttribute ("coord", coord);
   if (el_name!=buffer)
       xml->addAttribute ("name", el_name);
   xml->closeMark ();

   if (v_shape != NULL)
      {
      sprintf (coord, "%g %g %g", gc_x, gc_y, gc_z);
      v_shape->callXml (xml);
      xml->addAttribute ("coord", coord);
      xml->closeMark ();
      }

// if (el_assoc!=NULL) xml->addAttribute ("shape", el_assoc->getBrep().c_str());
}
// ========================================================= translate
void  Vertex::translate  (Vector* vecteur, double fact)
{
    v_x += fact*vecteur->getDx ();
    v_y += fact*vecteur->getDy ();
    v_z += fact*vecteur->getDz ();
}
// ========================================================= createMiddle
Vertex* Vertex::createMiddle  (Vertex* left, Vertex* right)
{
    Vertex* milieu = new Vertex (left);

    milieu->v_x = (left->v_x + right->v_x) / 2;
    milieu->v_y = (left->v_y + right->v_y) / 2;
    milieu->v_z = (left->v_z + right->v_z) / 2;

    return milieu;
}
// ========================================================= setAssociation
int Vertex::setAssociation (VertexShape* forme)
{
   if (forme==NULL)
      return HERR;

   forme->addAssociation (this);
   forme->getCoords (gc_x, gc_y, gc_z);
   v_shape = forme;

   if (db) cout << " Vertex "           << el_name
                << " setAssociation-> " << forme->getName ()
                << " = (" << gc_x << ", " << gc_y << ", " << gc_z << ") \n" ;

   is_associated = true;
   return HOK;
}
// ========================================================= setAssociation
int Vertex::setAssociation (NewShape* geom, int subid)
{
   if (geom==NULL)
      return HERR;

   VertexShape* shape = geom->findVertex (subid);
   if (shape==NULL)
      return HERR;

   int ier = setAssociation (shape);
   return ier;
}
// ========================================================= setAssociation
int Vertex::setAssociation (double* point)
{
   NewShape* cloud = el_root->getCloud();

   int subid = cloud->addPoint (point);
   int ier   = setAssociation  (cloud, subid);
   return ier;
}
// ========================================================= setAssociation
int Vertex::setAssociation (double px, double py, double pz)
{
   Real3 point = { px, py, pz };
   int   ier   = setAssociation (point);
   return ier;
}
// ========================================================= clearAssociation
void Vertex::clearAssociation ()
{
   v_shape = NULL;
   is_associated = false;
}
// ========================================================= getAssoCoord
void Vertex::getAssoCoord (double &px, double &py, double &pz)
{
   if (is_associated)
      {
      px = gc_x;
      py = gc_y;
      pz = gc_z;
      }
   else
      {
      px = v_x;
      py = v_y;
      pz = v_z;
      }
}
// ========================================================= getAssoCoord
void Vertex::getAssoCoord (double* point)
{
   getAssoCoord (point[dir_x], point[dir_y], point[dir_z]);
}
// ===================================================== getCoord
double Vertex::getCoord (int dir)
{
   double val = 0;
   switch (dir)
          {
          case dir_x : val = v_x;
               break;
          case dir_y : val = v_y;
               break;
          case dir_z : val = v_z;
               break;
          }
   return val;
}
// ========================================================= dump
void Vertex::dump ()
{
   printName (" = ");
   if (NOT isHere ())
      {
      printf ("(*** deleted ***)\n");
      return;
      }

   printf ("(%g, %g, %g)", v_x,v_y,v_z);
   dumpRef ();
}
// ========================================================= setCoord
void Vertex::setCoord (double x, double y, double z)
{
   v_x = x;
   v_y = y;
   v_z = z;
}
// ========================================================= isin
bool Vertex::isin  (double xmin, double xmax, double ymin, double ymax,
                                             double zmin, double zmax)
{
   bool   rep =   v_x >= xmin && v_x <= xmax
               && v_y >= ymin && v_y <= ymax
               && v_z >= zmin && v_z <= zmax;
   return rep;
}
// ========================================================= getPoint
double* Vertex::getPoint (double point[])
{
   point [dir_x] = v_x;
   point [dir_y] = v_y;
   point [dir_z] = v_z;
   return point;
}
// ========================================================= duplicate
void Vertex::duplicate (Document* cible)
{
   v_clone = new Vertex (cible, v_x, v_y, v_z);
   v_clone->v_scalar = v_scalar;
}
// ========================================================= definedBy
bool Vertex::definedBy (double px, double py, double pz, double eps2)
{
   double dist2 = carre (v_x-px) + carre (v_y-py) + carre (v_z-pz);
   return dist2 < eps2;
}
// ========================================================= dist2
double Vertex::dist2 (Vertex* other)
{
   if (BadElement (other))
      return 1e+77;

   double dist = carre (v_x-other->v_x) + carre (v_y-other->v_y) 
                                        + carre (v_z-other->v_z);
   return dist;
}
// ========================================================= anaVertices
void Vertex::anaVertices (Vertices& cont, double pmin[], double pmax[], 
                                                         double center[])
{
    Real3 point;
    int nbre = cont.size();
    if (nbre==0)
       {
       for (int nc=0 ; nc<DIM3 ; nc++)
            pmin [nc] = pmax[nc] =  center[nc] = 0;
       return;
       }

    cont[0]->getPoint (pmin);
    cont[0]->getPoint (pmax);
    for (int nro=1 ; nro<nbre ; ++nro)
        {
        cont[nro]->getPoint (point);
        for (int nc=0 ; nc<DIM3 ; nc++)
            {
            pmin [nc] = std::min (pmin[nc], point[nc]);
            pmax [nc] = std::max (pmax[nc], point[nc]);
            }
        }

    for (int nc=0 ; nc<DIM3 ; nc++)
        center [nc] = (pmin[nc] + pmax[nc])/2;
}
END_NAMESPACE_HEXA
