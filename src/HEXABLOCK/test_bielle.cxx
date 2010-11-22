
// C++ : Test bielle (d'apres Karima D.Y)

#include <cmath>

#include "Hex.hxx"
#include "HexDocument.hxx"
#include "HexElements.hxx"

#include "HexHexa.hxx"
#include "HexQuad.hxx"
#include "HexEdge.hxx"
#include "HexVertex.hxx"

#include "HexPropagation.hxx"
#include "HexShape.hxx"
#include "HexLaw.hxx"

#define DumpElt(x) { printf(" ... "#x " = ") ; if (x) x->dump() ; else printf ("0x\n") ; } 

// ======================================================== test_bielle
int test_bielle ()
{
   Hex::Hex mon_ex;
   Hex::Document* doc = mon_ex.addDocument ();

    // =============================
    //  CREATION DU MODELE
    // =============================

    // Pour la bielle on doit creer deux grilles cylindriques et prismer les quadrangles entre ces deux dernieres

    //=============================
    // PARAMETRES
    //=============================

   double R   = 40.0;

   //  double r_grd = R;
   //  double r_grd_t = R/2;

   //  double r_pte = R;
   //  double r_pte_t = R/2;

   double l = 200.0;
   double h = 40.0;

    // Taille du grand cylindre
   double dr_grd = R;
   double da_grd = 360;
   double dl_grd = h;

   int nr_grd = 1;
   int na_grd = 6;
   int nl_grd = 1;

    // Taille du petit cylindre
   //  double dr_pte = R;
   //  double da_pte = 360;
   //  double dl_pte = h;

   //  double nr_pte = 1.0;
   //  double na_pte = 6.0;
   //  double nl_pte = 1.0;


    //=============================
    // Creation des vecteurs
    //=============================

   Hex::Vector* dx = doc->addVector(l, 0, 0);
   //  Hex::Vector* dy = doc->addVector(0, l, 0);
   Hex::Vector* dz = doc->addVector(0, 0, l);

    //=================================================
    // Creation des centres des grilles cylindriques
    //=================================================

   Hex::Vertex* c_grd = doc->addVertex(0, 0, 0);
   Hex::Vertex* c_pte = doc->addVertex(2*l, 0, 0);
   Hex::Vector* dx_prime = doc->addVectorVertices( c_grd, c_pte );

    //=================================================
    // Creation de la grande grille cylindrique
    //=================================================

   Hex::Elements* grille_cyl_grd = doc->makeCylindrical (c_grd, dx, dz, 
                 dr_grd, da_grd, dl_grd, nr_grd, na_grd, nl_grd, false);


/**************************************************
   enum {size_ex=2, size_ey=7, size_ez=1};
   for (int nx=0 ; nx<size_ex; nx++) 
       for (int ny=0 ; ny<size_ey; ny++) 
           for (int nz=0 ; nz<size_ez; nz++) 
	       {
	       printf ("tab_edge(dir_z,%d,%d,%d) = ", nx,ny,nz);
	       Hex::Edge* ed = grille_cyl_grd->getEdge (dir_z, nx,ny,nz);
	       if (ed!=NULL) ed->printName ("\n");
	       else          printf ("0x0\n");  
	       }
    // on obtient une liste qui contient 6 hexaédres éguaux

    // 30° = pi/6
    // 60° = pi/3
    // 45° = pi/4
    // 90° = pi/2
    // 180° = pi

x1 = doc->findVertex( R, 0, h/2 )
x2 = doc->findVertex( R, R, -h/2 )
x3 = doc->findVertex( R, -R, -h/2 )
x4 = doc->findVertex( R, 0, -h/2 )

quad_11 = doc->findQuad(x1, x2)
quad_12 = doc->findQuad(x1, x3)

quad_list = [ quad_11 , quad_12 ]
 **************************************************/

    //=================================================
    // Creation de la petite grille cylindrique
    //=================================================

   // Hex::Elements* grille_cyl_pte = 
   doc->makeTranslation( grille_cyl_grd, dx_prime );

   /*******************
   Hex::Quad* facea1 = grille_cyl_grd ->getQuad (dir_x, 1,2,0);
   Hex::Quad* facea2 = grille_cyl_grd ->getQuad (dir_x, 1,3,0);

   Hex::Hexa* hexa12 = grille_cyl_grd ->getHexa (2);
   Hex::Hexa* hexa13 = grille_cyl_grd ->getHexa (3);
   Hex::Hexa* hexa20 = grille_cyl_pte ->getHexa (0);

   Quad* qa1 = hexa12->getQuad (Q_D);
   Quad* qa2 = hexa13->getQuad (Q_D);
   Quad* qb1 = hexa20->getQuad (Q_D);

   hexa12-> remove ();
   hexa13-> remove ();
   hexa20-> remove ();
   *********************/

   double alpha_x = 2*R*cos(M_PI/3);
   double alpha_y = 2*R*sin(M_PI/3);

    // on obtient une liste qui contient 6 hexaédres éguaux
   Hex::Vertex* x1 = doc->findVertex( 2*R, 0, h );
   Hex::Vertex* x2 = doc->findVertex( alpha_x,  alpha_y, 0 );
   Hex::Vertex* x3 = doc->findVertex( alpha_x, -alpha_y, 0 );
   Hex::Vertex* x4 = doc->findVertex( 2*R, 0, 0 );

   Hex::Vertex* y1 = doc->findVertex( 2*l - 2*R , 0 , h );
   Hex::Vertex* y2 = doc->findVertex( 2*l - alpha_x ,  alpha_y , 0 );
   Hex::Vertex* y3 = doc->findVertex( 2*l - alpha_x , -alpha_y , 0 );
   Hex::Vertex* y4 = doc->findVertex( 2*l - 2*R , 0 , 0 );

   DumpElt (x1);
   DumpElt (x2);
   DumpElt (x3);
   DumpElt (x4);

   DumpElt (y1);
   DumpElt (y2);
   DumpElt (y3);
   DumpElt (y4);

   Hex::Quad* quad_11 = doc->findQuad(x1, x2);
   Hex::Quad* quad_12 = doc->findQuad(x1, x3);

   Hex::Quad* quad_21 = doc->findQuad(y1, y2);
   Hex::Quad* quad_22 = doc->findQuad(y1, y3);

   Hex::Edge* edge1 = doc->findEdge( x1, x4);
   Hex::Edge* edge2 = doc->findEdge( y1, y4);

   DumpElt (edge1);
   DumpElt (edge2);
   //  doc->addHexa (x1, x2, x3, x4,  y2, y1, y3, y4);

   DumpElt (quad_11);
   DumpElt (quad_12);

   int n2 = 0;
   int n1 = quad_11->inter (quad_12, n2); 
   Display (n1);
   Display (n2);


   DumpElt (quad_21);
   DumpElt (quad_22);

   Hex::Quads quad_list;
   quad_list.push_back (quad_11);
   quad_list.push_back (quad_12);

    //=================================================
    // Assemblage des deux grilles cylindriques
    //=================================================
    doc->dump();
    // Hex::Elements* model_biell_fin = 
    doc->joinQuads (quad_list, quad_21, x1, y1, x4, y4, 1);

/*********************
   for (int nro=0 ; nro< model_biell_fin->countHexa() ; nro++)
       model_biell_fin->getHexa(nro)->dumpPlus();

   model_biell_fin->getHexa(0)->remove();
   //doc->saveVtk ("bielle.vtk");
**************************/
   doc->saveVtk ("bielle.vtk");
   return HOK;
}
/**************************************************
    //===================================================================
    // Recuperation des vertex du model hexa bielle pour l'association
    //===================================================================

    //# NB:
    //# h = haut
    //# b = bas
    //# g = grand
    //# p = petit
    //# t = trou



    // Face du haut grand trou

x_mod_h_g = doc->findVertex( -R , -R , h/2 )
y_mod_h_g = doc->findVertex( -R , 0 , h/2 )
z_mod_h_g = doc->findVertex( -R , R , h/2 )

u_mod_h_g = doc->findVertex( R , -R , h/2 )
v_mod_h_g = doc->findVertex( R , 0 , h/2 )
w_mod_h_g = doc->findVertex( R , R , h/2 )

x_mod_h_g_t = doc->findVertex( -R/2 , -R/2 , h/2 )
y_mod_h_g_t = doc->findVertex( -R/2 , 0 , h/2 )
z_mod_h_g_t = doc->findVertex( -R/2 , R/2 , h/2 )

u_mod_h_g_t = doc->findVertex( R/2 , -R/2 , h/2 )
v_mod_h_g_t = doc->findVertex( R/2 , 0 , h/2 )
w_mod_h_g_t = doc->findVertex( R/2 , R/2 , h/2 )


    // Face du haut petit trou

x_mod_h_p = doc->findVertex( l + R , -R , h/2 )
y_mod_h_p = doc->findVertex( l + R , 0 , h/2 )
z_mod_h_p = doc->findVertex( l + R , R , h/2 )

u_mod_h_p = doc->findVertex( l - R , -R , h/2 )
v_mod_h_p = doc->findVertex( l - R , 0 , h/2 )
w_mod_h_p = doc->findVertex( l - R , R , h/2 )

x_mod_h_p_t = doc->findVertex( l + R/2 , -R/2 , h/2 )
y_mod_h_p_t = doc->findVertex( l + R/2 , 0 , h/2 )
z_mod_h_p_t = doc->findVertex( l + R/2 , R/2 , h/2 )

u_mod_h_p_t = doc->findVertex( l - R/2 , -R/2 , h/2 )
v_mod_h_p_t = doc->findVertex( l - R/2 , 0 , h/2 )
w_mod_h_p_t = doc->findVertex( l - R/2 , R/2 , h/2 )


    // Face du bas grand trou

x_mod_b_g = doc->findVertex( -R , -R , -h/2 )
y_mod_b_g = doc->findVertex( -R , 0 , -h/2 )
z_mod_b_g = doc->findVertex( -R , R , -h/2 )

u_mod_b_g = doc->findVertex( R , -R , -h/2 )
v_mod_b_g = doc->findVertex( R , 0 , -h/2 )
w_mod_b_g = doc->findVertex( R , R , -h/2 )

x_mod_b_g_t = doc->findVertex( -R/2 , -R/2 , -h/2 )
y_mod_b_g_t = doc->findVertex( -R/2 , 0 , -h/2 )
z_mod_b_g_t = doc->findVertex( -R/2 , R/2 , -h/2 )

u_mod_b_g_t = doc->findVertex( R/2 , -R/2 , -h/2 )
v_mod_b_g_t = doc->findVertex( R/2 , 0 , -h/2 )
w_mod_b_g_t = doc->findVertex( R/2 , R/2 , -h/2 )


    // Face du bas petit trou

x_mod_b_p = doc->findVertex( l + R , -R , -h/2 )
y_mod_b_p = doc->findVertex( l + R , 0 , -h/2 )
z_mod_b_p = doc->findVertex( l + R , R , -h/2 )

u_mod_b_p = doc->findVertex( l - R , -R , -h/2 )
v_mod_b_p = doc->findVertex( l - R , 0 , -h/2 )
w_mod_b_p = doc->findVertex( l - R , R , -h/2 )

x_mod_b_p_t = doc->findVertex( l + R/2 , -R/2 , -h/2 )
y_mod_b_p_t = doc->findVertex( l + R/2 , 0 , -h/2 )
z_mod_b_p_t = doc->findVertex( l + R/2 , R/2 , -h/2 )

u_mod_b_p_t = doc->findVertex( l - R/2 , -R/2 , -h/2 )
v_mod_b_p_t = doc->findVertex( l - R/2 , 0 , -h/2 )
w_mod_b_p_t = doc->findVertex( l - R/2 , R/2 , -h/2 )



    //===================================================================
    // Recuperation des points de la geometrie bielle pour l'association
    //===================================================================


Bielle_geom = geompy.Import("/export/home/guest/Bureau/Karima/CAO_CEA/crank.stp", "STEP")

geompy.addToStudy(Bielle_geom, "Bielle_geom")

Pt_A = geompy.MakeVertex(0, 0, h/2.)
Face_haut = geompy.GetFaceNearPoint(Bielle_geom, Pt_A)


Pt_B = geompy.MakeVertex(0, 0, -h/2.)
Face_bas = geompy.GetFaceNearPoint(Bielle_geom, Pt_B)


    // NB:
    // h = haut
    // b = bas
    // g = grand
    // p = petit
    // t = trou


y_h_g = geompy.MakeVertexOnSurface(Face_haut, 1, 0.5)
y_b_g = geompy.MakeVertexWithRef(y_h_g, 0, 0, -h)

y_h_p = geompy.MakeVertexOnSurface(Face_haut, 0, 0.5)
y_b_p = geompy.MakeVertexWithRef(y_h_p, 0, 0, -h)

Edge_haut_grd = geompy.GetEdgeNearPoint(Bielle_geom, y_h_g)
Edge_haut_pte = geompy.GetEdgeNearPoint(Bielle_geom, y_h_p)



x_h_g = geompy.MakeVertexOnCurve(Edge_haut_grd, 0)
x_b_g = geompy.MakeVertexWithRef(y_h_g, 0, 0, -h)

z_h_g = geompy.MakeVertexOnCurve(Edge_haut_grd, 1)
z_b_g = geompy.MakeVertexWithRef(y_h_g, 0, 0, -h)

x_h_p = geompy.MakeVertexOnCurve(Edge_haut_pte, 0)
x_b_p = geompy.MakeVertexWithRef(y_h_g, 0, 0, -h)

z_h_p = geompy.MakeVertexOnCurve(Edge_haut_pte, 1)
z_b_p = geompy.MakeVertexWithRef(y_h_g, 0, 0, -h)



Edge_haut_grd_trou = geompy.GetEdgesByLength(Face_haut, 147, 148)

y_h_g_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0)
y_b_g_t = geompy.MakeVertexWithRef(y_h_g_t, 0, 0, -h)

z_h_g_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.2)
z_b_g_t = geompy.MakeVertexWithRef(z_h_g_t, 0, 0, -h)

w_h_g_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.35)
w_b_g_t = geompy.MakeVertexWithRef(w_h_g_t, 0, 0, -h)

v_h_g_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.5)
v_b_g_t = geompy.MakeVertexWithRef(v_h_g_t, 0, 0, -h)

u_h_g_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.65)
u_b_g_t = geompy.MakeVertexWithRef(u_h_g_t, 0, 0, -h)

x_h_g_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.8)
x_b_g_t = geompy.MakeVertexWithRef(x_h_g_t, 0, 0, -h)



Edge_haut_pte_trou = geompy.GetEdgesByLength(Face_haut, 94, 95)

y_h_p_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0)
y_b_p_t = geompy.MakeVertexWithRef(y_h_p_t, 0, 0, -h)

z_h_p_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.2)
z_b_p_t = geompy.MakeVertexWithRef(z_h_p_t, 0, 0, -h)

w_h_p_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.35)
w_b_p_t = geompy.MakeVertexWithRef(w_h_p_t, 0, 0, -h)

v_h_p_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.5)
v_b_p_t = geompy.MakeVertexWithRef(v_h_p_t, 0, 0, -h)

u_h_p_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.65)
u_b_p_t = geompy.MakeVertexWithRef(u_h_p_t, 0, 0, -h)

x_h_p_t = geompy.MakeVertexOnCurve(Edge_haut_grd_trou, 0.8)
x_b_p_t = geompy.MakeVertexWithRef(x_h_p_t, 0, 0, -h)



Edge_haut_droite = geompy.GetEdgesByLength(Face_haut, 136, 137)

u_h_g = geompy.MakeVertexOnCurve(Edge_haut_droite, 1)
u_b_g = geompy.MakeVertexWithRef(u_h_g, 0, 0, -h)

u_h_p = geompy.MakeVertexOnCurve(Edge_haut_droite, 0)
u_b_p = geompy.MakeVertexWithRef(u_h_p, 0, 0, -h)


Edge_haut_gauche = geompy.GetEdgesByLength(Face_haut, 131, 132)

w_h_g = geompy.MakeVertexOnCurve(Edge_haut_gauche, 1)
w_b_g = geompy.MakeVertexWithRef(w_h_g, 0, 0, -h)

w_h_p = geompy.MakeVertexOnCurve(Edge_haut_gauche, 0)
w_b_p = geompy.MakeVertexWithRef(w_h_p, 0, 0, -h)

Edge_v_grd = geompy.MakeLine(u_h_g, w_h_g)

v_h_g = geompy.MakeVertexOnCurve(Edge_v_grd, 0.5)
v_b_g = geompy.MakeVertexWithRef(v_h_g, 0, 0, -h)


Edge_v_pte = geompy.MakeLine(u_h_p, w_h_p)

v_h_p = geompy.MakeVertexOnCurve(Edge_v_grd, 0.5)
v_b_p = geompy.MakeVertexWithRef(v_h_p, 0, 0, -h)


    //=======================
    // CREATION ASSOCIATION
    //=======================


    //===========================================
    // association des sommets de la face du haut
    //===========================================

    // grand trou

x_mod_h_g.setAssociation( x_h_g )
y_mod_h_g.setAssociation( y_h_g )
z_mod_h_g.setAssociation( z_h_g )
u_mod_h_g.setAssociation( u_h_g )
v_mod_h_g.setAssociation( v_h_g )
w_mod_h_g.setAssociation( w_h_g )

x_mod_h_g_t.setAssociation( x_h_g_t )
y_mod_h_g_t.setAssociation( y_h_g_t )
z_mod_h_g_t.setAssociation( z_h_g_t )
u_mod_h_g_t.setAssociation( u_h_g_t )
v_mod_h_g_t.setAssociation( v_h_g_t )
w_mod_h_g_t.setAssociation( w_h_g_t )

    // petit trou

x_mod_h_p.setAssociation( x_h_p )
y_mod_h_p.setAssociation( y_h_p )
z_mod_h_p.setAssociation( z_h_p )
u_mod_h_p.setAssociation( u_h_p )
v_mod_h_p.setAssociation( v_h_p )
w_mod_h_p.setAssociation( w_h_p )

x_mod_h_p_t.setAssociation( x_h_p_t )
y_mod_h_p_t.setAssociation( y_h_p_t )
z_mod_h_p_t.setAssociation( z_h_p_t )
u_mod_h_p_t.setAssociation( u_h_p_t )
v_mod_h_p_t.setAssociation( v_h_p_t )
w_mod_h_p_t.setAssociation( w_h_p_t )


    //===========================================
    // association des sommets de la face du bas
    //===========================================

    // grand trou

x_mod_b_g.setAssociation( x_b_g )
y_mod_b_g.setAssociation( y_b_g )
z_mod_b_g.setAssociation( z_b_g )
u_mod_b_g.setAssociation( u_b_g )
v_mod_b_g.setAssociation( v_b_g )
w_mod_b_g.setAssociation( w_b_g )

x_mod_b_g_t.setAssociation( x_b_g_t )
y_mod_b_g_t.setAssociation( y_b_g_t )
z_mod_b_g_t.setAssociation( z_b_g_t )
u_mod_b_g_t.setAssociation( u_b_g_t )
v_mod_b_g_t.setAssociation( v_b_g_t )
w_mod_b_g_t.setAssociation( w_b_g_t )

    // petit trou

x_mod_b_p.setAssociation( x_b_p )
y_mod_b_p.setAssociation( y_b_p )
z_mod_b_p.setAssociation( z_b_p )
u_mod_b_p.setAssociation( u_b_p )
v_mod_b_p.setAssociation( v_b_p )
w_mod_b_p.setAssociation( w_b_p )

x_mod_b_p_t.setAssociation( x_b_p_t )
y_mod_b_p_t.setAssociation( y_b_p_t )
z_mod_b_p_t.setAssociation( z_b_p_t )
u_mod_b_p_t.setAssociation( u_b_p_t )
v_mod_b_p_t.setAssociation( v_b_p_t )
w_mod_b_p_t.setAssociation( w_b_p_t )





    //====================================
    // CREATION MAILLAGE
    //====================================


    //=================================================
    // Definir les groupes d'éléments pour le maillage
    //=================================================

    // On définit 3 groupes de mailles

    // groupe d'edges (arretes)

Edge_grp = doc->addEdgeGroup("Edge_grp")


Nbr_Edg = doc->countEdge()
print Nbr_Edg


for i in range(Nbr_Edg):
	Edge_i = doc->getEdge(i)
	Edge_grp.addElement(Edge_i)


Nbr_elm_edg = Edge_grp.countElement()

if Nbr_elm_edg == Nbr_Edg:
	print "YES"
else:
	print "Review your course"

    // groupe de quads (faces)

Quad_grp = doc->addQuadGroup("Quad_grp")


Nbr_Qad = doc->countQuad()
print Nbr_Qad



for i in range(Nbr_Qad):
	Quad_i = doc->getQuad(i)
	Quad_grp.addElement(Quad_i)



Nbr_elm_qad = Quad_grp.countElement()

if Nbr_elm_qad == Nbr_Qad:
	print "YES"
else:
	print "Review your course"


    // groupe d'hexas (solids)

Hexa_grp = doc->addHexaGroup("Hexa_grp")


Nbr_Hex = doc->countHexa()
print Nbr_Hex



for i in range(Nbr_Hex):
	Hexa_i = doc->getHexa(i)
	Hexa_grp.addElement(Hexa_i)



Nbr_elm_hex = Hexa_grp.countElement()

if Nbr_elm_hex == Nbr_Hex:
	print "YES"
else:
	print "Review your course"


    // groupe de noeuds de vertex pour tout le modéle

Vertex_Nod_Grp = doc->addVertexNodeGroup("Vertex_Nod_Grp")

Nbr_Vx = doc->countVertex()
print Nbr_Vx



for i in range(Nbr_Vx):
	Vertex_i = doc->getVertex(i)
	Vertex_Nod_Grp.addElement(Vertex_i)


Nbr_elm_vex = Vertex_Nod_Grp.countElement()

if Nbr_elm_vex == Nbr_Vx:
	print "YES"
else:
	print "Review your course"



    //====================================
    // Definir une loi de discretisation
    //====================================


    // définir une loi: le choix de la loi reste aux utilisateurs

Law = doc->addLaw( "Uniform" , Nbr_Vx )

    //petit test
n = doc->countLaw()
print n

    // chercher les propagations du modéle

N_Propa = doc->countPropagation()

for j in range(N_Propa):
	Propa = doc->getPropagation(j)

    // appliquer la loi de discrétisation sur tout le modéle et générer le maillage

Propa.setLaw( Law )

mesh = smesh.HexaBlocks(doc)

print "Nombre d'hexaédres:", mesh.NbHexas()
print "Nombre de quadrangles:", mesh.NbQuadrangles()
print "Nombre de segments:", mesh.NbEdges()
print "Nombre de noeuds:", mesh.NbNodes()
***********************************************************************/
