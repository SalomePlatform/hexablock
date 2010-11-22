
// Singleton : Var globales : Disposition sommets/aretes/faces d'un hexaedre

#ifndef _HEX_GLOBALE_H
#define _HEX_GLOBALE_H

#include "hexa_base.hxx"

/* -----------------------------------------------------

                // ---- Numerotation des faces (%x) 
                   
              +----bf-----+
             /|          /|
           bc |   B    bd |
           /  |        /  |
          +----be-----+   |   
          |  cf     F |   df
          | C |       | D |             z
         ce   | E     de  |             ^
          |   +----af-|---+             |   y
          |  /        |  /              |  /
          | ac    A   | ad              | /
          |/          |/                |/
          +----ae-----+                 +----->  x
                
 * ----------------------------------------------------- */

BEGIN_NAMESPACE_HEXA

class Globale
{
public :
   static Globale* getInstance ();

   int CoordVertex (int edge, EnumCoord dim)
       { return coord_vertex [edge] [dim] ; }

   EnumHVertex EdgeVertex (int edge, EnumVertex sens)
       { return edge_vertex [edge] [sens] ; }

   EnumHEdge QuadEdge (int face, EnumQuad cote)
       { return quad_edge [face] [cote] ; }

   cpchar namofHexaEdge   (int arete)   { return h_edge_name   [arete] ; }
   cpchar namofHexaVertex (int sommet)  { return h_vertex_name [sommet]; }
   cpchar namofHexaQuad   (int face)    { return h_quad_name   [face];   }

   EnumHQuad getOpposedQuad (int face)  { return h_opposed_quad [face]; }

   void setArgs  (int nbargs, cpchar tabargs[]);
   int  findArg  (cpchar opt);
   bool isinArgs (cpchar opt)  { return findArg (opt) >= 0; }
   cpchar getArg  (int nro)    { return nro>=nbr_args ? NULL : tab_args[nro]; }


private :
   Globale ();         // Constructeur prive

   void setCoordVertex (EnumHVertex node, int px, int py, int pz);
   void setEdgeVertex  (EnumHEdge edge, EnumHVertex amont, EnumHVertex aval);
   void setQuadEdge    (EnumHQuad face, EnumHEdge c1, EnumHEdge c2, 
		                        EnumHEdge c3, EnumHEdge c4);
   void setOpposedVertex (EnumHVertex lun, EnumHVertex lautre);
   void setOpposedQuad   (EnumHQuad   lun, EnumHQuad   lautre);

private :
   int     nbr_args;
   cpchar* tab_args;

   int         coord_vertex [HV_MAXI] [DIM3];
   EnumHVertex edge_vertex  [HV_MAXI] [V_TWO];
   EnumHEdge   quad_edge    [HQ_MAXI] [QUAD4];

   cpchar h_edge_name   [HE_MAXI]; 
   cpchar h_vertex_name [HV_MAXI];
   cpchar h_quad_name   [HQ_MAXI]; 

   EnumHVertex  h_opposed_vertex [HV_MAXI];
   EnumHQuad    h_opposed_quad   [HQ_MAXI];

   static Globale* unique_instance;
};
// ===================================================== setArgs
inline void Globale::setArgs (int nbargs, cpchar tabargs[])
{
   nbr_args = nbargs;
   tab_args = tabargs;
}
// ===================================================== findArg
inline int Globale::findArg (cpchar option)
{
   for (int nro=1 ; nro<nbr_args ; nro++)
       if (Cestegal (option, tab_args[nro]))
          return nro;

   return NOTHING;
}
END_NAMESPACE_HEXA

#endif
