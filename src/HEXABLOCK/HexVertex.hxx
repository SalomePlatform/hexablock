
// Class : Gestion des sommets

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

#ifndef __VERTEX_H_
#define __VERTEX_H_

#include "HexEltBase.hxx"

BEGIN_NAMESPACE_HEXA

class HexaExport Vertex : public EltBase
{
public :
   double getX()   { return v_x; }
   double getY()   { return v_y; }
   double getZ()   { return v_z; }

   void setX (double v)   { v_x = v ; }
   void setY (double v)   { v_y = v ; }
   void setZ (double v)   { v_z = v ; }

   double getAssoX ()   { return is_associated ? gc_x : v_x ; }
   double getAssoY ()   { return is_associated ? gc_y : v_y ; }
   double getAssoZ ()   { return is_associated ? gc_z : v_z ; }

   int  setAssociation   (NewShape* geom, int subid);
   void clearAssociation ();

   void setColor (double valeur)       { v_scalar = valeur ; }

   Vertex (Document* prev, double x, double y, double z);

#ifndef SWIG
public : 
   double getCoord (int dir);
   virtual int     countVertex ()       { return 1; }
   virtual Vertex* getVertex (int nro)  { return this; }

                                                   // Hexa5
   int  setAssociation (VertexShape* forme);
   int  setAssociation (double* point);
   int  setAssociation (double px, double py, double pz);

   VertexShape* getAssoVertex ()              { return v_shape ;}
   VertexShape* getAssociation ()             { return v_shape ;}

   void   getAssoCoord (double &x, double &y, double &z);
   void   getAssoCoord (double* point);

   static void anaVertices (Vertices& cont, double tmin[], double tmax[], double center[]);

public :
   Vertex (Vertex* other);
   virtual ~Vertex () {}
   virtual void dump () ;
   virtual void saveXml (XmlWriter* xml);

   void    setScalar (double val)             { setColor (val) ; }
   double  getScalar ()                       { return v_scalar ; }
   double* getPoint (double point[]);

   void setCoord  (double x, double y, double z);
   void setCoord  (double point[]) { setCoord (point[0], point[1], point[2]); }

   bool isin      (double xmin, double xmax, double ymin, double ymax,
                                            double zmin, double zmax);
   Edge* getParent (int nro);
   void  translate (Vector* vecteur, double fact=1.0);
   void  replace   (Vertex* old);

   Vertex* getClone ()              {  return v_clone ; }
   void    duplicate (Document* doc);

   static Vertex* createMiddle (Vertex* left, Vertex* right);
   bool   definedBy (double px, double py, double pz, double eps2=1e-4);
   double dist2 (Vertex* other);

private :
    double v_x;
    double v_y;
    double v_z;

    double  v_scalar;
    Vertex* v_clone;
    double  gc_x, gc_y, gc_z;
    VertexShape*  v_shape;
#endif
};
END_NAMESPACE_HEXA
#endif
