
//  C++ Les noeuds

// Copyright (C) 2009-2013  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
#include "HexOldShape.hxx"
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
END_NAMESPACE_HEXA
