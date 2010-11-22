
// C++ : Initialisation du singleton

#include "HexGlobale.hxx"

BEGIN_NAMESPACE_HEXA

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

Globale* Globale::unique_instance = NULL;


// ====================================================== Constructeur
Globale::Globale  ()
{
   setCoordVertex ( V_ACE, 0, 0, 0 );
   setCoordVertex ( V_ACF, 1, 0, 0 );
   setCoordVertex ( V_ADF, 0, 1, 0 );
   setCoordVertex ( V_ADE, 1, 1, 0 );

   setCoordVertex ( V_BCE, 0, 0, 1 );
   setCoordVertex ( V_BCF, 1, 0, 1 );
   setCoordVertex ( V_BDF, 1, 1, 1 );
   setCoordVertex ( V_BDE, 0, 1, 1 );
   
   setEdgeVertex ( E_AC, V_ACE, V_ACF );
   setEdgeVertex ( E_AF, V_ACF, V_ADF );
   setEdgeVertex ( E_AD, V_ADF, V_ADE );
   setEdgeVertex ( E_AE, V_ADE, V_ACE );

   setEdgeVertex ( E_BC, V_BCE, V_BCF );
   setEdgeVertex ( E_BF, V_BCF, V_BDF );
   setEdgeVertex ( E_BD, V_BDF, V_BDE );
   setEdgeVertex ( E_BE, V_BDE, V_BCE );

   setEdgeVertex ( E_CE, V_ACE, V_BCE );
   setEdgeVertex ( E_CF, V_ACF, V_BCF );
   setEdgeVertex ( E_DF, V_ADF, V_BDF );
   setEdgeVertex ( E_DE, V_ADE, V_BDE );

   setQuadEdge ( Q_A, E_AC, E_AF, E_AD, E_AE );
   setQuadEdge ( Q_B, E_BC, E_BF, E_BD, E_BE );
   setQuadEdge ( Q_C, E_AC, E_CF, E_BC, E_CE );
   setQuadEdge ( Q_D, E_AD, E_DF, E_BD, E_DE );
   setQuadEdge ( Q_E, E_AF, E_DF, E_BF, E_CF );

   setOpposedVertex ( V_ACE, V_BDF);
   setOpposedVertex ( V_ACF, V_BDE);
   setOpposedVertex ( V_ADF, V_BCE);
   setOpposedVertex ( V_ADE, V_BCF);

   setOpposedQuad   ( Q_A, Q_B );
   setOpposedQuad   ( Q_C, Q_D );
   setOpposedQuad   ( Q_E, Q_F );

#define SetNameHexaVertex(v) h_vertex_name [v] = #v
#define SetNameHexaEdge(e)   h_edge_name   [e] = #e
#define SetNameHexaQuad(q)   h_quad_name   [q] = #q

   SetNameHexaVertex ( V_ACE );
   SetNameHexaVertex ( V_ACF );
   SetNameHexaVertex ( V_ADF );
   SetNameHexaVertex ( V_ADE );
   SetNameHexaVertex ( V_BCE );
   SetNameHexaVertex ( V_BCF );
   SetNameHexaVertex ( V_BDF );
   SetNameHexaVertex ( V_BDE );
   
   SetNameHexaEdge ( E_AC );
   SetNameHexaEdge ( E_AF );
   SetNameHexaEdge ( E_AD );
   SetNameHexaEdge ( E_AE );
   SetNameHexaEdge ( E_BC );
   SetNameHexaEdge ( E_BF );
   SetNameHexaEdge ( E_BD );
   SetNameHexaEdge ( E_BE );
   SetNameHexaEdge ( E_CE );
   SetNameHexaEdge ( E_CF );
   SetNameHexaEdge ( E_DF );
   SetNameHexaEdge ( E_DE );

   SetNameHexaQuad ( Q_A );
   SetNameHexaQuad ( Q_B );
   SetNameHexaQuad ( Q_C );
   SetNameHexaQuad ( Q_D );
   SetNameHexaQuad ( Q_E );
   SetNameHexaQuad ( Q_F );
}
// ====================================================== getInstance
Globale* Globale::getInstance  ()
{
   if (unique_instance==NULL) 
       unique_instance = new Globale ();

   return unique_instance;
}
// ====================================================== setCoordVertex
void Globale::setCoordVertex (EnumHVertex node, int px, int py, int pz)
{
    coord_vertex [node] [dir_x] = 2*px-1;
    coord_vertex [node] [dir_y] = 2*py-1;
    coord_vertex [node] [dir_z] = 2*pz-1;
}
// ====================================================== setEdgeVertex 
void Globale::setEdgeVertex (EnumHEdge edge, EnumHVertex v1, EnumHVertex v2)
{
    edge_vertex [edge] [V_AMONT] = v1;
    edge_vertex [edge] [V_AVAL ] = v2;
}
// ====================================================== setQuadEdge
void Globale::setQuadEdge (EnumHQuad face, EnumHEdge c1, EnumHEdge c2, 
		                           EnumHEdge c3, EnumHEdge c4)
{
    quad_edge [face] [E_A] = c1;
    quad_edge [face] [E_B] = c2;
    quad_edge [face] [E_C] = c3;
    quad_edge [face] [E_D] = c4;
}
// ====================================================== setopposedVertex 
void Globale::setOpposedVertex (EnumHVertex lun, EnumHVertex lautre)
{
    h_opposed_vertex [lun]    = lautre;
    h_opposed_vertex [lautre] = lun;
}
// ====================================================== setopposedQuad 
void Globale::setOpposedQuad (EnumHQuad lun, EnumHQuad lautre)
{
    h_opposed_quad [lun]    = lautre;
    h_opposed_quad [lautre] = lun;
}

END_NAMESPACE_HEXA
