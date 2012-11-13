:tocdepth: 3

.. _intropython:

===========================================
Example: Python script for a connecting rod
===========================================

.. image:: _static/cad_bielle.png
   :align: center

.. centered::
   Connecting rod 

.. _menu:

Steps
=====
    - :ref:`modelcreation`
    - :ref:`recover`
    - :ref:`assoc`
    - :ref:`groupdef`
    - :ref:`lawdisc`
    - :ref:`meshgener`

.. _modelcreation:

Model creation
==============

::

        import os
        import geompy
        import hexablock
        import math

        STEP_PATH = os.path.expandvars("$HEXABLOCK_ROOT_DIR/bin/salome/crank.stp")

        #=============================
        # CREATION DOCUMENT
        #=============================

        doc = hexablock.addDocument()

        #=============================
        # MODEL CREATION
        #=============================

        # For the connecting rod, two cylindrical grids have to be build and
        # the quadrangles have to be prismed between these wo grids

        #=============================
        # PARAMETRES
        #=============================

        R = 0.095168291790720005

        r_pte = R
        r_pte_t = R/2.0

        xpetit = 0.0
        xgrand = 1.35739 + 0.1595
        longueur = (xgrand - xpetit)/2.0
        hauteur = 0.019999999553*2

        dr_pte = R
        da_pte = 360
        dl_pte = hauteur

        nr_pte = 1
        na_pte = 6
        nl_pte = 1

        #=============================
        # Vectors Creation 
        #=============================

        dx = doc.addVector(longueur, 0, 0)
        dy = doc.addVector(0, longueur, 0)
        dz = doc.addVector(0, 0, longueur)

        #=================================================
        # Creation of cylindrical grid centers
        #=================================================

        c_pte = doc.addVertex(xpetit, 0, 0)
        c_grd = doc.addVertex(2*longueur, 0, 0)
        dx_prime = doc.addVectorVertices(c_pte, c_grd)

        #=================================================
        # small cylindrical grid creation
        #=================================================

        grille_cyl_pte = doc.makeCylindrical(c_pte, dx, dz, dr_pte, da_pte, dl_pte, nr_pte, na_pte, nl_pte, False)

        #=================================
        #  Small cylindrical grid creation
        #=================================

        grille_cyl_grd = doc.makeTranslation(grille_cyl_pte, dx_prime)

        #==================================
        # Joining the two cylindrical grids
        #==================================

        mod_x1 = grille_cyl_pte.getVertexIJK(1, 0, 1)
        mod_x2 = grille_cyl_pte.getVertexIJK(1, 1, 0)
        mod_x3 = grille_cyl_pte.getVertexIJK(1, 5, 0)
        mod_x4 = grille_cyl_pte.getVertexIJK(1, 0, 0)
        quad_11 = doc.findQuad(mod_x1, mod_x2)
        quad_12 = doc.findQuad(mod_x1, mod_x3)

        mod_y1 = grille_cyl_grd.getVertexIJK(1, 3, 1)
        mod_y2 = grille_cyl_grd.getVertexIJK(1, 2, 0)
        mod_y3 = grille_cyl_grd.getVertexIJK(1, 4, 0)
        mod_y4 = grille_cyl_grd.getVertexIJK(1, 3, 0)

        quad_21 = doc.findQuad(mod_y1, mod_y2)
        quad_22 = doc.findQuad(mod_y1, mod_y3)

        model_biell_fin = doc.joinQuads([quad_11, quad_12], quad_21, mod_x1, mod_y1, mod_x4, mod_y4, 1)


Go back to :ref:`menu`

.. _recover:

Recovering edges and faces before the association
=================================================

::

        bielle_geom = geompy.ImportFile(STEP_PATH, "STEP")
        doc.setShape(bielle_geom)
        geompy.addToStudy(bielle_geom, "bielle_geom")
        all_edges_bielle = geompy.SubShapeAllSorted(bielle_geom, geompy.ShapeType["EDGE"])



        # dictionnaire des edges de la géométrie :
        # key = nom, value = indice dans all_edges_bielle
        dic_edge_names = {"edge_ray_pte_b": 0, "edge_ray_pte_h": 1,
                          "edge_trou_pte_b": 2, "edge_trou_pte_h" :3,
                          "edge_arr_pte_g_b": 7, "edge_arr_pte_g_h": 8,
                          "edge_arr_pte_d_b": 9, "edge_arr_pte_d_h": 10,
                          "edge_arr_grd_g_b": 19, "edge_arr_grd_g_h": 20,
                          "edge_arr_grd_d_b": 21, "edge_arr_grd_d_h": 22,
                          "edge_trou_grd_b": 25, "edge_trou_grd_h": 26,
                          "edge_ray_grd_b": 27, "edge_ray_grd_h": 28,
                          "edge_long_g_b": 13, "edge_long_g_h": 14,
                          "edge_long_d_b": 15, "edge_long_d_h": 16
                          }


        all_faces_bielle = geompy.SubShapeAllSorted(bielle_geom, geompy.ShapeType["FACE"])
        # dictionnaire des faces de la geometrie :
        # key = nom, value = indice dans all_faces_bielle
        dic_face_names = {"face_ray_pte": 0, "face_trou_pte": 1, "face_pte_g": 2,
                          "face_pte_d": 3, "face_long_g": 4, "face_long_d": 5,
                          "face_bas": 6, "face_haut": 7, "face_grd_g": 8,
                          "face_grd_d": 9, "face_trou_grd": 10,
                          "face_ray_grd": 11
                         }




Go back to :ref:`menu`

.. _assoc:

Association of the model to the geometry
========================================

::

 dico_haut_bas = {"h": 1, "b": 0}

 # 1. lignes internes (trou) haut/bas du petit cylindre
 # ====================================================
 for z in dico_haut_bas.iteritems():

     mod_line = [grille_cyl_pte.getEdgeJ(0, j, z[1]) for j in range(5)]
     mod_start = grille_cyl_pte.getEdgeJ(0, 5, z[1])
     mod_first = mod_start.getVertex(1)

     # geometrie : 1 seule ligne
     edge_hole_in = all_edges_bielle[dic_edge_names["edge_trou_pte_"+z[0]]]
     geo_start = edge_hole_in
     par_start = 0.0
     geo_line  = []

     # association :
     ier = doc.associateClosedLine(mod_first, mod_start, mod_line,
                                   geo_start, par_start, geo_line)
 

 # 2. lignes internes (trou) haut/bas du grand cylindre
 # =====================================================
 for z in dico_haut_bas.iteritems():

     mod_start = grille_cyl_grd.getEdgeJ(0, 5, z[1])
     mod_first = mod_start.getVertex(1)
     mod_line = [grille_cyl_grd.getEdgeJ(0, j, z[1]) for j in range (5)]

     # geometrie : 1 seule ligne
     edge_hole_in = all_edges_bielle[dic_edge_names["edge_trou_grd_"+z[0]]]
     geo_start = edge_hole_in
     par_start = 0.0
     geo_line  = []

     # association :
     ier = doc.associateClosedLine(mod_first, mod_start, mod_line,
                                   geo_start, par_start, geo_line)


 # 3. lignes externes haut/bas du petit cylindre
 # =============================================
 for z in dico_haut_bas.iteritems():

     # modele de blocs :
     mod_start = grille_cyl_pte.getEdgeJ(1, 1, z[1])
     # table des edges :
     mod_line = [grille_cyl_pte.getEdgeJ(1, j, z[1]) for j in [2, 3, 4]]

     # geometrie :
     # les edges de la geometrie correspondant sont, dans l'ordre (par
     # exemple pour le haut) :
     # edge_arr_pte_d_h, edge_ray_pte_h, edge_arr_pte_g_h
     geo_start = all_edges_bielle[dic_edge_names["edge_arr_pte_d_"+z[0]]]

     geo_line  = []
     geo_line.append(all_edges_bielle[dic_edge_names["edge_ray_pte_"+z[0]]])
     geo_line.append(all_edges_bielle[dic_edge_names["edge_arr_pte_g_"+z[0]]])

     # association :
     # la première est la dernière ligne sont orientées "dans le
     # mauvais sens" => on fournit cette info :
     par_start = 0.0
     par_end = 1.0
     ier = doc.associateOpenedLine(mod_start, mod_line,
                                   geo_start, par_start, geo_line, par_end)


 ## # 4. lignes externes haut/bas du grand cylindre
 ## # =============================================
 for z in dico_haut_bas.iteritems():

     # modele de blocs :
     mod_start = grille_cyl_grd.getEdgeJ(1, 4, z[1])
     # table des edges :
     mod_line = [grille_cyl_grd.getEdgeJ(1, j, z[1]) for j in [5, 0, 1]]

     # geometrie :
     # les edges de la geometrie correspondant sont, dans l'ordre (par
     # exemple pour le haut) :
     # edge_arr_grd_g_h, edge_ray_grd_h, edge_arr_grd_d_h
     geo_start = all_edges_bielle[dic_edge_names["edge_arr_grd_g_"+z[0]]]

     geo_line  = []
     geo_line.append(all_edges_bielle[dic_edge_names["edge_ray_grd_"+z[0]]])
     geo_line.append(all_edges_bielle[dic_edge_names["edge_arr_grd_d_"+z[0]]])

     # association :
     # la première est la dernière ligne sont orientées "dans le
     # mauvais sens" => on fournit cette info :
     par_start = 0.0
     par_end = 1.0
     ier = doc.associateOpenedLine(mod_start, mod_line,
                                   geo_start, par_start, geo_line, par_end)

 # 6. association des 4 points restants (x1, x4, y1, y4) :
 # =======================================================

 # NB:
 # h = top (haut)
 # b = bottom (bas)
 # g = big (grand)
 # p = small (petit)
 # t = hole (trou)

 face_haut = all_faces_bielle[dic_face_names["face_haut"]]

 edge_haut_droite = geompy.GetEdgesByLength(face_haut, 0.136, 0.137)
 edge_haut_gauche = geompy.GetEdgesByLength(face_haut, 0.131, 0.132)

 # 1. grand cylindre :
 y_h_g = geompy.MakeVertexOnSurface(face_haut, 1, 0.5)
 u_h_g = geompy.MakeVertexOnCurve(edge_haut_droite, 1)
 w_h_g = geompy.MakeVertexOnCurve(edge_haut_gauche, 0)
 edge_v_grd = geompy.MakeLineTwoPnt(u_h_g, w_h_g)

 geo_y1 = geompy.MakeVertexOnCurve(edge_v_grd, 0.5)
 geo_y4 = geompy.MakeVertexWithRef(geo_y1, 0.0, 0.0, -hauteur)

 # vertex cote grande grille cylindrique :
 mod_y1.setAssociation(geo_y1)
 mod_y4.setAssociation(geo_y4)

 # 2. petit cylindre :
 # REM : le modele grand cylindre a ete cree par translation / au petit
 # cylindre.
 v_h_p = geompy.MakeVertexOnSurface(face_haut, 0, 0.5)
 x_h_p = geompy.MakeVertexOnCurve(edge_haut_droite, 0)
 z_h_p = geompy.MakeVertexOnCurve(edge_haut_gauche, 1)
 edge_v_pte = geompy.MakeLineTwoPnt(x_h_p, z_h_p)

 geo_x1 = geompy.MakeVertexOnCurve(edge_v_pte, 0.5)
 geo_x4 = geompy.MakeVertexWithRef(geo_x1, 0.0, 0.0, -hauteur)

 # vertex cote petite grille cylindrique :
 mod_x1.setAssociation(geo_x1)
 mod_x4.setAssociation(geo_x4)

 # 7. association des faces :
 quad1 = grille_cyl_pte.getQuadJK(1, 1, 0)
 quad1.addAssociation(all_faces_bielle[dic_face_names["face_pte_d"]])
 quad2 = grille_cyl_pte.getQuadJK(1, 4, 0)
 quad2.addAssociation(all_faces_bielle[dic_face_names["face_pte_g"]])
 quad3 = grille_cyl_grd.getQuadJK(1, 1, 0)
 quad3.addAssociation(all_faces_bielle[dic_face_names["face_grd_d"]])
 quad4 = grille_cyl_grd.getQuadJK(1, 4, 0)
 quad4.addAssociation(all_faces_bielle[dic_face_names["face_grd_g"]])


Go back to :ref:`menu`
 
.. _groupdef:

Definition of elements groups for the mesh
==========================================

::

 # We define 3 groups 

 # groupe d edges (arretes)
 edge_grp = doc.addEdgeGroup("Edge_grp")
 for i in range(doc.countEdge()):
     edge_grp.addElement(doc.getEdge(i))

 # groupe de quads (faces)
 quad_grp = doc.addQuadGroup("Quad_grp")
 for i in range(doc.countQuad()):
     quad_grp.addElement(doc.getQuad(i))

 # groupe d hexas (solids)
 hexa_grp = doc.addHexaGroup("Hexa_grp")
 for i in range(doc.countHexa()):
     hexa_grp.addElement(doc.getHexa(i))

 # groupe de noeuds de vertex pour tout le modele 
 vertex_nod_grp = doc.addVertexNodeGroup("Vertex_Nod_Grp")
 for i in range(doc.countVertex()):
     vertex_nod_grp.addElement(doc.getVertex(i))

Go back to :ref:`menu`

.. _lawdisc:

Definition of a law discretization
==================================

::

 law = doc.addLaw("Uniform", 4)

 for j in range(doc.countPropagation()):
     propa = doc.getPropagation(j)
     propa.setLaw(law)  # apply discretization law on the model and generate the mesh

Go back to :ref:`menu`


.. _meshgener:

Mesh generation
===============

::

 print  " --- HEXAHEDRICAL MESH --- "
 mesh_hexas = hexablock.mesh("Bielle:hexas", doc)

 print "Number of hexahedra:", mesh_hexas.NbHexas()
 print "Number of quadrangles:", mesh_hexas.NbQuadrangles()
 print "Number of segments:", mesh_hexas.NbEdges()
 print "Number of nodes:", mesh_hexas.NbNodes()

