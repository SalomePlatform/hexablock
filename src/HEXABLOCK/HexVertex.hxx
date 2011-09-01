
// Class : Gestion des sommets 

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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com

#ifndef __VERTEX_H_
#define __VERTEX_H_

#include "HexEltBase.hxx"

BEGIN_NAMESPACE_HEXA

class Vertex : public EltBase 
{
public :
   double getX()   { return v_x; }
   double getY()   { return v_y; }
   double getZ()   { return v_z; }

   void setX (double v)   { v_x = v ; }
   void setY (double v)   { v_y = v ; }
   void setZ (double v)   { v_z = v ; }
 
   void assoCoord (double x, double y, double z);

public :
   Vertex (Document* prev, double x=0.0, double y=0.0, double z=0.0); 
   Vertex (Document* cible, Vertex* other);
   Vertex (Vertex* other);
   virtual ~Vertex () {}
   virtual void dump () ;
   virtual void saveXml (XmlWriter* xml);

   void   setScalar (double val)             { v_scalar = val ; }
   double getScalar ()                       { return v_scalar ; }
   void   getPoint (double point[]);

   void setCoord  (double x, double y, double z);
   bool isin      (double xmin, double xmax, double ymin, double ymax, 
                                            double zmin, double zmax);
   Edge* getParent (int nro); 
   Vertex* makeSymetric (Vertex* other);
   void    translate (Vector* vecteur, double fact=1.0);
   void    replace   (Vertex* old);

   Vertex* getClone ()              {  return v_clone ; }
   void    duplicate (Document* doc);

   static Vertex* createMiddle (Vertex* left, Vertex* right);

private :
    double v_x;
    double v_y;
    double v_z;

    double  v_scalar;
    Vertex* v_clone;
    double  gc_x, gc_y, gc_z;
    bool    gc_ass;
};
// ========================================================= Constructeur bis 
inline Vertex::Vertex (Vertex* other)
      : EltBase (other->dad(), EL_VERTEX)
{
   if (other!= NULL)
      {
      v_x = other->v_x;
      v_y = other->v_y;
      v_z = other->v_z;
      gc_ass = other->gc_ass;
      gc_x   = other->gc_x;
      gc_y   = other->gc_y;
      gc_z   = other->gc_z;
      }
   else 
      {
      v_x  = v_y  = v_z  = 0;
      gc_x = gc_y = gc_z = 0;
      gc_ass = false;
      }
}
// ========================================================= dump 
inline void Vertex::dump ()
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
inline void Vertex::setCoord (double x, double y, double z)
{
   v_x = x;
   v_y = y;
   v_z = z;
}
// ========================================================= assoCoord 
inline void Vertex::assoCoord (double x, double y, double z)
{
   gc_ass = true;
   gc_x = x;
   gc_y = y;
   gc_z = z;
}
// ========================================================= isin 
inline bool Vertex::isin  (double xmin, double xmax, double ymin, double ymax, 
                                             double zmin, double zmax)
{
   bool   rep =   v_x >= xmin && v_x <= xmax
               && v_y >= ymin && v_y <= ymax
               && v_z >= zmin && v_z <= zmax;
   return rep;
}
// ========================================================= getPoint
inline void Vertex::getPoint (double point[])
{
   point [dir_x] = v_x;
   point [dir_y] = v_y;
   point [dir_z] = v_z;
}
// ========================================================= duplicate 
inline void Vertex::duplicate (Document* cible)
{
   v_clone = new Vertex (cible, v_x, v_y, v_z);
   v_clone->v_scalar = v_scalar;
}
END_NAMESPACE_HEXA
#endif
