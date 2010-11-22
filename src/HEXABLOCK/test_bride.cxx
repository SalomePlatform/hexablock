
// C++ : Test bride

#include "Hex.hxx"
#include "HexDocument.hxx"
#include "HexElements.hxx"
#include "HexCrossElements.hxx"

#include "HexHexa.hxx"
#include "HexQuad.hxx"
#include "HexEdge.hxx"
#include "HexVertex.hxx"

#include "HexPropagation.hxx"
#include "HexShape.hxx"
#include "HexLaw.hxx"
#include "HexMatrix.hxx"


// ======================================================== test_bride
int test_bride ()
{
   Hex::Hex mon_ex;
   Hex::Document*doc = mon_ex.addDocument();

   int h = 70;
   int nl = 7;

   int R  = 5;
   int a1 = 45;
   int l1 = h;

   int nr1 = 4;
   int na1 = 1;
   int nl1 = nl = 1;


// Taille de la 2eme grille cylindrique
   // int r2 = 20;
   // int r2_t = 2;

//r = r2-r2_t
   int r = 18;
   int a2 = 180;
   int l2 = h;

   int nr2 = 5;
   int na2 = 5;
   int nl2 = nl = 1;



//=============================
// Creation des vecteurs
//=============================

   Hex::Vector * DX = doc->addVector( h , 0 , 0 );
   // Hex::Vector * DY = doc->addVector( 0 , h , 0 );
   Hex::Vector * DZ = doc->addVector( 0 , 0 , h );


//=================================================
// Creation des centres des grilles cylindriques
//=================================================

   int P = 200;
   Hex::Vertex* c1 = doc->addVertex( 0 , 0 , 0 );
   Hex::Vertex* c2 = doc->addVertex( P , 0 , 0 );


//=================================================
// Creation de la 1ere grille cylindrique
//=================================================

   Hex::Elements* grille_cyl_1 = doc->makeCylindrical( c1 , DX , DZ , R , a1 , l1 , nr1 , na1 , nl1 , false );


   Hex::Vertex* vtx_A_1_g1 = grille_cyl_1->getVertexIJK( 4 , 0 , 0 );
   Hex::Vertex* vtx_A_2_g1 = grille_cyl_1->getVertexIJK( 4 , 1 , 0 );

   Hex::Vertex* vtx_B_1_g1 = grille_cyl_1->getVertexIJK( 3 , 0 , 0 );
   Hex::Vertex* vtx_B_2_g1 = grille_cyl_1->getVertexIJK( 4 , 0 , 0 );

// reperer les quads

   Hex::Quad* quad_A_g1 = grille_cyl_1->getQuadJK( 4 , 0 , 0 );
   Hex::Quad* quad_B_g1 = grille_cyl_1->getQuadIK( 3 , 0 , 0 );
   // Hex::Quad* quad_C_g1 = grille_cyl_1->getQuadIK( 2 , 0 , 0 );


// reperer les hexas

   // Hex::Hexa* hexa_A_g1 = grille_cyl_1->getHexaIJK( 3 , 0 , 0 );
   // Hex::Hexa* hexa_B_g1 = grille_cyl_1->getHexaIJK( 2 , 0 , 0 );


   Hex::Elements* grille_cyl_2 = doc->makeCylindrical( c2 , DX , DZ , r , a2 , l2 , nr2 , na2 , nl2 , false );

   Hex::Vertex* vtx_A_1_g2 = grille_cyl_2->getVertexIJK( 4 , 3 , 0 );
   Hex::Vertex* vtx_A_2_g2 = grille_cyl_2->getVertexIJK( 5 , 3 , 0 );

   Hex::Vertex* vtx_B_1_g2 = grille_cyl_2->getVertexIJK( 4 , 3 , 0 );
   Hex::Vertex* vtx_B_2_g2 = grille_cyl_2->getVertexIJK( 5 , 3 , 0 );

// reperer les quads

   Hex::Quad* quad_A_g2 = grille_cyl_2->getQuadIK( 2 , 0 , 0 );
   // Hex::Quad* quad_B_g2 = grille_cyl_2->getQuadIK( 3 , 0 , 0 );

   Hex::Quad* quad_AB_g2 = grille_cyl_2->getQuadIK( 4 , 3 , 0 );


// reperer les hexas


   Hex::Hexa* hexa_A_g2 = grille_cyl_2->getHexaIJK( 4 , 2 , 0 );
   // Hex::Hexa* hexa_B_g2 = grille_cyl_2->getHexaIJK( 4 , 3 , 0 );
   // Hex::Hexa* hexa_C_g3 = grille_cyl_2->getHexaIJK( 4 , 4 , 0 );

// deconnexion des quads
//
   Hex::Elements* nouveaux = doc->disconnectQuad( hexa_A_g2 , quad_AB_g2 );
   //  quad_A_g2 = doc->disconnectQuad( hexa_A_g2 , quad_AB_g2 );
   HexDump (quad_A_g2);
   HexDump (quad_AB_g2);

//quad_B_g2 = doc->disconnectQuad( hexa_B_g2 , quad_AB_g2 );

   vtx_A_1_g2 = grille_cyl_2->getVertexIJK( 4 , 3 , 0 );
   vtx_A_1_g1 = grille_cyl_1->getVertexIJK( 4 , 0 , 0 );
   vtx_A_2_g2 = grille_cyl_2->getVertexIJK( 5 , 3 , 0 );
   vtx_A_2_g1 = grille_cyl_1->getVertexIJK( 4 , 1 , 0 );

   vtx_B_1_g2 = grille_cyl_2->getVertexIJK( 4 , 3 , 0 );
   vtx_B_2_g2 = grille_cyl_2->getVertexIJK( 5 , 3 , 0 );
   vtx_B_1_g1 = grille_cyl_1->getVertexIJK( 3 , 0 , 0 );
   vtx_B_2_g1 = grille_cyl_1->getVertexIJK( 4 , 0 , 0 );

// fusionner les quads

   Hex::Edge*  arete = grille_cyl_2->getEdgeK( 5 , 0 , 0 );
   Hex::Propagation* prop  = NULL;

/* *******************************************************************
   Hex::Propagation* prop  = doc->findPropagation (arete);
   const Hex::Edges& table = prop->getEdges ();
       for (int ned=0 ; ned<(int)table.size() ; ned++)
           {
           bool way = table [ned]->getWay ();
           printf ("  %2d -> ", ned);
           table [ned]->printName (" : ");

           if (way)
              {
              table [ned]->getVertex (0)->printName (", ");
              table [ned]->getVertex (1)->printName ("\n");
              }
          else
              {
              table [ned]->getVertex (1)->printName (", ");
              table [ned]->getVertex (0)->printName ("\n");
              }
           }


   ******************************************************************* */
   int fusion_g1_g2_quad_A = doc->mergeQuads( quad_A_g2 , quad_A_g1 , 
		   vtx_A_1_g2 , vtx_A_1_g1 , vtx_A_2_g2 , vtx_A_2_g1 );
   Display (fusion_g1_g2_quad_A);
   HexDump (quad_A_g2);
   HexDump (quad_AB_g2);

/* *******************************************************************
   prop  = doc->findPropagation (arete);
   const Hex::Edges& table2 = prop->getEdges ();
       for (int ned=0 ; ned<(int)table.size() ; ned++)
           {
           bool way = table2 [ned]->getWay ();
           printf ("  %2d -> ", ned);
           table [ned]->printName (" : ");

           if (way)
              {
              table [ned]->getVertex (0)->printName (", ");
              table [ned]->getVertex (1)->printName ("\n");
              }
          else
              {
              table [ned]->getVertex (1)->printName (", ");
              table [ned]->getVertex (0)->printName ("\n");
              }
           }

   ******************************************************************* */
   int fusion_g1_g2_quad_B = doc->mergeQuads( quad_AB_g2 , quad_B_g1 , 
		   vtx_B_1_g2 , vtx_B_2_g1 , vtx_B_2_g2 , vtx_B_1_g1 );
   Display (fusion_g1_g2_quad_B);


   prop  = doc->findPropagation (arete);
   const Hex::Edges& table3 = prop->getEdges ();
       for (int ned=0 ; ned<(int)table3.size() ; ned++)
           {
           bool way = table3 [ned]->getWay ();
           printf ("  %2d -> ", ned);
           table3 [ned]->printName (" : ");

           if (way)
              {
              table3 [ned]->getVertex (0)->printName (", ");
              table3 [ned]->getVertex (1)->printName ("\n");
              }
          else
              {
              table3 [ned]->getVertex (1)->printName (", ");
              table3 [ned]->getVertex (0)->printName ("\n");
              }
           }
   exit (0);

   Hex::Vertex* vtx_B_1_h_g2 = grille_cyl_2->getVertexIJK( 5 , 3 , 1 );
   Hex::Vertex* vtx_C_1_h_g1 = grille_cyl_1->getVertexIJK( 3 , 0 , 1 );

   Hex::Edge* edge_B_g2 = grille_cyl_2->getEdgeK( 5 , 3 , 0 );
   Hex::Edge* edge_C_g1 = grille_cyl_1->getEdgeK( 3 , 0 , 0 );



   int fusion_g1_g2 = doc->mergeEdges( edge_B_g2 , edge_C_g1 , vtx_B_1_h_g2 , vtx_C_1_h_g1 );

   Display (fusion_g1_g2);


// rajouter l hexa qui manque entre les deux grilles

   Hex::Vertex* vtx_C_h_g2 = grille_cyl_2->getVertexIJK( 5 , 5 , 1 );
   Hex::Vertex* vtx_C_h_g1 = grille_cyl_1->getVertexIJK( 2 , 0 , 1 );

   Hex::Vertex* vtx_C_b_g2 = grille_cyl_2->getVertexIJK( 5 , 5 , 0 );
   Hex::Vertex* vtx_C_b_g1 = grille_cyl_1->getVertexIJK( 2 , 0 , 0 );

   Hex::Edge* edge_h = doc->addEdge( vtx_C_h_g2 , vtx_C_h_g1 );
   Hex::Edge* edge_b = doc->addEdge( vtx_C_b_g2 , vtx_C_b_g1 );

   Hex::Edge* edge_B_h_g2 = grille_cyl_2->getEdgeJ( 5 , 3 , 1 );
   Hex::Edge* edge_C_h_g2 = grille_cyl_2->getEdgeJ( 5 , 4 , 1 );

   Hex::Edge* edge_B_b_g2 = grille_cyl_2->getEdgeJ( 5 , 3 , 0 );
   Hex::Edge* edge_C_b_g2 = grille_cyl_2->getEdgeJ( 5 , 4 , 0 );

   Hex::Vertex* X_C_h_g1 = grille_cyl_1->getVertexIJK( 2 , 0 , 1 );
   Hex::Vertex* X_C_h_g2 = grille_cyl_2->getVertexIJK( 5 , 3 , 1 );
   Hex::Edge*   edge_C_h_g1 = doc->findEdge( X_C_h_g1 , X_C_h_g2 );

   Hex::Vertex* X_C_b_g1 = grille_cyl_1->getVertexIJK( 2 , 0 , 0 );
   Hex::Vertex* X_C_b_g2 = grille_cyl_2->getVertexIJK( 5 , 3 , 0 );
   Hex::Edge*   edge_C_b_g1 = doc->findEdge( X_C_b_g1 , X_C_b_g2 );



   edge_C_g1 = grille_cyl_1->getEdgeK( 2 , 0 , 0 );
   Hex::Edge* edge_C_g2 = grille_cyl_2->getEdgeK( 5 , 5 , 0 );




   Hex::Quad* quad_h = doc->addQuad( edge_B_h_g2 , edge_C_h_g2 , edge_h , edge_C_h_g1 );
   Hex::Quad* quad_b = doc->addQuad( edge_B_b_g2 , edge_C_b_g2 , edge_b , edge_C_b_g1 );
   Hex::Quad* quad_f = doc->addQuad( edge_b , edge_C_g1 , edge_h , edge_C_g2 );
   // Hex::Quad* quad_C_g2 = grille_cyl_2->getQuadJK( 5 , 4 , 0 );

   Hex::Vertex* X_B_h_g2 = grille_cyl_2->getVertexIJK( 5 , 3 , 0 );
   Hex::Vertex* X_A_h_g2 = grille_cyl_2->getVertexIJK( 5 , 4 , 1 );
   X_C_h_g2 = grille_cyl_2->getVertexIJK( 5 , 5 , 0 );


   Hex::Quad* quad_B_prime_g2 = doc->findQuad( X_B_h_g2 , X_A_h_g2 );
   Hex::Quad* quad_C_prime_g2 = doc->findQuad( X_A_h_g2 , X_C_h_g2 );

   Hex::Quad* quad_C_prime_g1 = doc->findQuad( X_C_h_g1 , X_C_b_g2 );

   Hex::Hexa* hexa_g2_g1 = doc->addHexa( quad_b , quad_h , quad_f , quad_B_prime_g2 , quad_C_prime_g1 , quad_C_prime_g2 );


// rajouter trois hexa qui servent au remplissage de la partie centrale de la 2eme grille

   Hex::Vertex* vtx_1_h_g2 = grille_cyl_2->getVertexIJK( 0 , 0 , 1 );
   // Hex::Vertex* vtx_2_h_g2 = grille_cyl_2->getVertexIJK( 0 , 1 , 1 );
   Hex::Vertex* vtx_3_h_g2 = grille_cyl_2->getVertexIJK( 0 , 2 , 1 );
   Hex::Vertex* vtx_4_h_g2 = grille_cyl_2->getVertexIJK( 0 , 3 , 1 );
   // Hex::Vertex* vtx_5_h_g2 = grille_cyl_2->getVertexIJK( 0 , 4 , 1 );
   Hex::Vertex* vtx_6_h_g2 = grille_cyl_2->getVertexIJK( 0 , 5 , 1 );

   Hex::Vertex* vtx_1_b_g2 = grille_cyl_2->getVertexIJK( 0 , 0 , 0 );
   // Hex::Vertex* vtx_2_b_g2 = grille_cyl_2->getVertexIJK( 0 , 1 , 0 );
   Hex::Vertex* vtx_3_b_g2 = grille_cyl_2->getVertexIJK( 0 , 2 , 0 );
   Hex::Vertex* vtx_4_b_g2 = grille_cyl_2->getVertexIJK( 0 , 3 , 0 );
   // Hex::Vertex* vtx_5_b_g2 = grille_cyl_2->getVertexIJK( 0 , 4 , 0 );
   Hex::Vertex* vtx_6_b_g2 = grille_cyl_2->getVertexIJK( 0 , 5 , 0 );


   Hex::Vertex* vtx_7_h_g2 = doc->addVertex( P + 5 , 0 , h );
   Hex::Vertex* vtx_8_h_g2 = doc->addVertex( P - 5 , 0 , h );

   Hex::Vertex* vtx_7_b_g2 = doc->addVertex( P + 5 , 0 , 0 );
   Hex::Vertex* vtx_8_b_g2 = doc->addVertex( P - 5 , 0 , 0 );

   // Hex::Vertex* Pt_dtr_1 = doc->addVertex( P , 0 , h );
   // Hex::Vertex* Pt_dtr_2 = doc->addVertex( P , 0 , 0 );


   // Hex::Vector* dtr = doc->addVectorVertices( Pt_dtr_1 , Pt_dtr_2 );


   Hex::Edge* edge_1_2_h_g2 = grille_cyl_2->getEdgeJ( 0 , 0 , 1 );
   Hex::Edge* edge_2_3_h_g2 = grille_cyl_2->getEdgeJ( 0 , 1 , 1 );
   Hex::Edge* edge_3_4_h_g2 = grille_cyl_2->getEdgeJ( 0 , 2 , 1 );
   Hex::Edge* edge_4_5_h_g2 = grille_cyl_2->getEdgeJ( 0 , 3 , 1 );
   Hex::Edge* edge_5_6_h_g2 = grille_cyl_2->getEdgeJ( 0 , 4 , 1 );

   Hex::Edge* edge_1_2_b_g2 = grille_cyl_2->getEdgeJ( 0 , 0 , 0 );
   Hex::Edge* edge_2_3_b_g2 = grille_cyl_2->getEdgeJ( 0 , 1 , 0 );
   Hex::Edge* edge_3_4_b_g2 = grille_cyl_2->getEdgeJ( 0 , 2 , 0 );
   Hex::Edge* edge_4_5_b_g2 = grille_cyl_2->getEdgeJ( 0 , 3 , 0 );
   Hex::Edge* edge_5_6_b_g2 = grille_cyl_2->getEdgeJ( 0 , 4 , 0 );

   Hex::Edge* edge_1_z_g2 = grille_cyl_2->getEdgeK( 0 , 0 , 0 );
   // Hex::Edge* edge_2_z_g2 = grille_cyl_2->getEdgeK( 0 , 1 , 0 );
   Hex::Edge* edge_3_z_g2 = grille_cyl_2->getEdgeK( 0 , 2 , 0 );
   Hex::Edge* edge_4_z_g2 = grille_cyl_2->getEdgeK( 0 , 3 , 0 );
   // Hex::Edge* edge_5_z_g2 = grille_cyl_2->getEdgeK( 0 , 4 , 0 );
   Hex::Edge* edge_6_z_g2 = grille_cyl_2->getEdgeK( 0 , 5 , 0 );


//creer les edges manquants

   Hex::Edge* edge_1_7_h_g2 = doc->addEdge( vtx_1_h_g2 , vtx_7_h_g2 );
   Hex::Edge* edge_3_7_h_g2 = doc->addEdge( vtx_7_h_g2 , vtx_3_h_g2 );
   Hex::Edge* edge_7_8_h_g2 = doc->addEdge( vtx_8_h_g2 , vtx_7_h_g2 );
   Hex::Edge* edge_4_8_h_g2 = doc->addEdge( vtx_4_h_g2 , vtx_8_h_g2 );
   Hex::Edge* edge_6_8_h_g2 = doc->addEdge( vtx_6_h_g2 , vtx_8_h_g2 );

   Hex::Edge* edge_1_7_b_g2 = doc->addEdge( vtx_1_b_g2 , vtx_7_b_g2 );
   Hex::Edge* edge_3_7_b_g2 = doc->addEdge( vtx_3_b_g2 , vtx_7_b_g2 );
   Hex::Edge* edge_7_8_b_g2 = doc->addEdge( vtx_7_b_g2 , vtx_8_b_g2 );
   Hex::Edge* edge_4_8_b_g2 = doc->addEdge( vtx_4_b_g2 , vtx_8_b_g2 );
   Hex::Edge* edge_6_8_b_g2 = doc->addEdge( vtx_6_b_g2 , vtx_8_b_g2 );

   Hex::Edge* edge_7_h_b_g2 = doc->addEdge( vtx_7_h_g2 , vtx_7_b_g2 );
   Hex::Edge* edge_8_h_b_g2 = doc->addEdge( vtx_8_h_g2 , vtx_8_b_g2 );

//creer les quads manquants

   Hex::Quad* quad_7_h_g2 = doc->addQuad( edge_1_2_h_g2 , edge_2_3_h_g2 , edge_3_7_h_g2 , edge_1_7_h_g2 );
//print quad_7_h_g2

   Hex::Quad* quad_7_8_h_g2 = doc->addQuad( edge_3_4_h_g2 , edge_4_8_h_g2 , edge_7_8_h_g2 , edge_3_7_h_g2 );
//print quad_7_8_h_g2

   Hex::Quad* quad_8_h_g2 = doc->addQuad( edge_4_5_h_g2 , edge_5_6_h_g2 , edge_6_8_h_g2 , edge_4_8_h_g2 );
//print quad_8_h_g2

   Hex::Quad* quad_7_b_g2 = doc->addQuad( edge_1_2_b_g2 , edge_2_3_b_g2 , edge_3_7_b_g2 , edge_1_7_b_g2 );
//print quad_7_b_g2

   Hex::Quad* quad_7_8_b_g2 = doc->addQuad( edge_3_4_b_g2 , edge_4_8_b_g2 , edge_7_8_b_g2 , edge_3_7_b_g2 );
//print quad_7_8_b_g2

   Hex::Quad* quad_8_b_g2 = doc->addQuad( edge_4_5_b_g2 , edge_5_6_b_g2 , edge_6_8_b_g2 , edge_4_8_b_g2 );
//print quad_8_b_g2

   Hex::Quad* quad_7_f_g2 = doc->addQuad( edge_1_7_h_g2 , edge_7_h_b_g2 , edge_1_7_b_g2 , edge_1_z_g2 );
//print quad_7_f_g2

   Hex::Quad* quad_7_8_f_g2 = doc->addQuad( edge_7_8_h_g2 , edge_7_h_b_g2 , edge_7_8_b_g2 , edge_8_h_b_g2 );
//print quad_7_8_f_g2

   Hex::Quad* quad_8_f_g2 = doc->addQuad( edge_6_8_h_g2 , edge_8_h_b_g2 , edge_6_8_b_g2 , edge_6_z_g2 );
//print quad_8_f_g2

   Hex::Quad* quad_3_7_f_g2 = doc->addQuad( edge_3_z_g2 , edge_3_7_h_g2 , edge_7_h_b_g2 , edge_3_7_b_g2 );
//print quad_3_7_f_g2

   Hex::Quad* quad_4_8_f_g2 = doc->addQuad( edge_4_z_g2 , edge_4_8_h_g2 , edge_8_h_b_g2 , edge_4_8_b_g2 );
//print quad_4_8_f_g2

// reperer les quads

   Hex::Quad* quad_1_2_z_g2 = grille_cyl_2->getQuadJK( 0 , 0 , 0 );
   Hex::Quad* quad_2_3_z_g2 = grille_cyl_2->getQuadJK( 0 , 1 , 0 );
   Hex::Quad* quad_3_4_z_g2 = grille_cyl_2->getQuadJK( 0 , 2 , 0 );
   Hex::Quad* quad_4_5_z_g2 = grille_cyl_2->getQuadJK( 0 , 3 , 0 );
   Hex::Quad* quad_5_6_z_g2 = grille_cyl_2->getQuadJK( 0 , 4 , 0 );

//# creer les hexas manquants

   Hex::Hexa* hexa_rmp_7_g2 = doc->addHexa( quad_7_b_g2 , quad_7_h_g2 ,quad_7_f_g2 , quad_2_3_z_g2 , quad_1_2_z_g2 , quad_3_7_f_g2 );
   if (hexa_rmp_7_g2 == NULL)
      return HERR;

//print "Hexa_7"
//print hexa_rmp_7_g2

   Hex::Hexa* hexa_rmp_7_8_g2 = doc->addHexa( quad_7_8_b_g2 , quad_7_8_h_g2 , quad_7_8_f_g2 , quad_3_4_z_g2 , quad_4_8_f_g2 , quad_3_7_f_g2 );
   if (hexa_rmp_7_8_g2==NULL)
      return HERR;

   Hex::Hexa* hexa_rmp_8_g2 = doc->addHexa( quad_8_b_g2 , quad_8_h_g2 , quad_8_f_g2 , quad_4_5_z_g2 , quad_5_6_z_g2 , quad_4_8_f_g2);
   if (hexa_rmp_8_g2==NULL)
      return HERR;



   Hex::Edge* edge_cut = grille_cyl_2->getEdgeK( 5 , 0 , 0 );
   Hex::Elements* mod_7_couche = doc->cut( edge_cut , 1 );
   if (mod_7_couche==NULL)
      return HERR;

   doc->saveVtk ("bride.vtk");
   return HOK;
}
