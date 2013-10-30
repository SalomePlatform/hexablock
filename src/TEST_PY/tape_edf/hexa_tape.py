
# Hexa : Utilitaires de creation de la tape

import hexablock
import geompy

nbr_points = 0

the_doc   = None
the_shape = None
the_name  = "xxxx"

tab_x = []
tab_z = []

x_moy = 0.0
z_moy = 0.0
z_min = 0.0
z_max = 0.0

Epsilon = 1e-4

# ======================================================== add_document
def add_document (name, load_geom=True) :

    global the_doc, the_shape, the_name

    the_name  = name
    the_doc   = hexablock.addDocument (name)
    the_shape = None

    if load_geom :
       brep      = 'cas_edf/' + name + '.brep'
       geom      = geompy.ImportBREP (brep)
       the_shape = hexablock.addShape (the_doc, geom , name)

    return the_doc

# ============================================================= add_edge
def add_edge (nro, v1,v2, ass=True) :
   edge = the_doc.addEdge (v1,v2)
   if ass :
      edge.addAssociation (the_shape, nro, 0.0, 1.0)
   return edge

# ============================================================= mailler0
def mailler0 (doc=None) :

    if doc == None :
       doc = the_doc

    law = doc.addLaw('Uniform', 3)
    for j in range(doc.countPropagation()) :
        propa = doc.getPropagation(j)
        propa.setLaw(law)

    mesh_hexas = hexablock.mesh(doc)
    return  mesh_hexas

# ============================================================= mailler
def mailler (doc=None) :

    if doc == None :
       doc = the_doc

    doc.addLaws (10, True)
    mesh_hexas = hexablock.mesh(doc)
    return  mesh_hexas

# ============================================================= egalite
def egalite (px, py) :
    delta = py - px
    return delta > -Epsilon and delta < Epsilon

# ============================================================= add_value
def add_value (table, value) :

    pasla = True
    for val in table :
        if egalite (val, value) :
           pasla = False

    if pasla :
       table.append (value)

# ========================================================== raz_points
def raz_points () :

    global nbr_points, x_moy, y_fix, z_min, z_max
    global tab_x, tab_z
    nbr_points = 0
    x_moy      = 0
    y_fix      = 0

    tab_x = []
    tab_z = []

# ============================================================= add_point
def add_point (px, py, pz) :

    global nbr_points, x_moy, y_fix, z_min, z_max
    global tab_x, tab_z

    if nbr_points == 0 :
       x_moy = px
       z_min = pz
       z_max = pz
    else :
       x_moy  += px
       if pz < z_min : z_min = pz
       if pz > z_max : z_max = pz

    y_fix       = py
    nbr_points += 1

    tab_x.append (px)
    tab_z.append (pz)

# ========================================================== gen_cylinder
def gen_cylinder () :

    global nbr_points, x_moy, y_fix, z_min
    global tab_x, tab_z
    x_moy = x_moy / nbr_points

    ta = [ 0, 90, 180, 270, 360 ]
    tab_ray  = []
    tab_haut = []

    for val in tab_x :
        if val > x_moy :
           add_value (tab_ray,  val-x_moy)

    tab_ray.sort()

    for val in tab_z :
        add_value (tab_haut, val)

    tab_haut.sort()

    orig = the_doc.addVertex (x_moy, y_fix, 0)
    vx   = the_doc.addVector (1, 0, 0)
    vz   = the_doc.addVector (0, 0, 1)
    grid = the_doc.makePipe (orig, vx, vz, tab_ray, ta, tab_haut)
    return grid


# ========================================================== gen_vis
def gen_vis () :

    global nbr_points, x_moy, y_fix, z_min
    global tab_x, tab_z
    x_moy = x_moy / nbr_points

    xmed = tab_x [0] - x_moy
    xmax = xmed
    xmin = xmed

    for px in tab_x :
        if px > x_moy :
           val = px - x_moy
           if val < xmin :
              xmin = val
           elif val > xmax :
              xmed = xmax
              xmax = val
           elif val < xmax and val > xmed :
              xmed = val

    z_top = z_min
    for val in tab_z :
        if val < z_max and val > z_top :
           z_top = val

    tr = [ xmin,  xmed,  xmax ]
    tz = [ z_min, z_top, z_max ]
    ta = [ 0, 90, 180, 270, 360 ]

    orig = the_doc.addVertex (x_moy, y_fix, 0)
    vx   = the_doc.addVector (1, 0, 0)
    vz   = the_doc.addVector (0, 0, 1)

    grid = the_doc.makePipe (orig, vx, vz, tr, ta, tz)

    for na in range (4) :
        grid.getHexaIJK (1, na, 0).remove()

    return grid


