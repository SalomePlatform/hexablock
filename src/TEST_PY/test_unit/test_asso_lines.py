# -*- coding: latin-1 -*-
# Copyright (C) 2009-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

# Hexa : Association par lignes

import os
import hexablock
geompy = hexablock.geompy

#---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8
#
#   .             ed_nord1     ed_nord2
#   .          v_nw        v_n         v_ne
#              v9          v10         v11
#   1           +----e18----+----e19----+
#   .           |           |           |
#   .           |           |           |
#   0 ed_west  e13          o          e17  ed_est
#   .           |           |           |
#   .           |           |           |
#  -1           +----e14----+----e16----+
#   .          v6          v10          v8
#   .          v_sw        v_s          v_se
#   .              ed_sud1     ed_sud2
#   .
#   +..........-2...........0...........2...............

global  ed_west, ed_est, ed_nord1, ed_nord2, ed_sud1, ed_sud2
global  v_sw, v_s , v_se, v_nw, v_n , v_ne
global  p_w, p_e, p_n, p_s, p_n1, p_n2, p_s1, p_s2, p_nw, p_sw, p_ne, p_se
global  arc_s, arc_e, arc_n, arc_w, arc_n1, arc_n2, arc_s1, arc_s2


# ======================================================= init_globale
def init_globale (name) :

    global  ed_west, ed_est, ed_nord1, ed_nord2, ed_sud1, ed_sud2
    global  v_sw, v_s , v_se, v_nw, v_n , v_ne
    global  p_w, p_e, p_n, p_s, p_n1, p_n2, p_s1, p_s2, p_nw, p_sw, p_ne, p_se
    global  arc_s, arc_e, arc_n, arc_w, arc_n1, arc_n2, arc_s1, arc_s2

    nom  = name
    doc  = hexablock.addDocument (nom)

    orig = doc.addVertex (-2, -1, -1)
    vx   = doc.addVector (2,0,0)
    vy   = doc.addVector (0,2,0)
    vz   = doc.addVector (0,0,2)
    grid = doc.makeCartesian  (orig, vx, vy, vz, 2, 1, 1) 

    nz = 1
    vz = 1.0

    ed_west  = grid.getEdgeJ (0, 0, nz)
    ed_est   = grid.getEdgeJ (2, 0, nz)
    ed_nord1 = grid.getEdgeI (0, 1, nz)
    ed_nord2 = grid.getEdgeI (1, 1, nz)
    ed_sud1  = grid.getEdgeI (0, 0, nz)
    ed_sud2  = grid.getEdgeI (1, 0, nz)

    v_sw    = grid.getVertexIJK (0, 0, nz)
    v_s     = grid.getVertexIJK (1, 0, nz)
    v_se    = grid.getVertexIJK (2, 0, nz)

    v_nw    = grid.getVertexIJK (0, 1, nz)
    v_n     = grid.getVertexIJK (1, 1, nz)
    v_ne    = grid.getVertexIJK (2, 1, nz)

    doc.saveVtk (nom + ".vtk");
    ## ______________________________________________________________ Geom

    p_w   = geompy.MakeVertex (-2.55,  0, vz)
    p_e   = geompy.MakeVertex ( 2.55,  0, vz)
    p_n   = geompy.MakeVertex ( 0,  2.0, vz)
    p_s   = geompy.MakeVertex ( 0, -2.0, vz)

    p_n1  = geompy.MakeVertex ( 0.5,  2.0, vz)
    p_n2  = geompy.MakeVertex (-0.5,  2.0, vz)
    p_s1  = geompy.MakeVertex (-0.5, -2.0, vz)
    p_s2  = geompy.MakeVertex ( 0.5, -2.0, vz)

    p_nw  = geompy.MakeVertex (-2,  1, vz)
    p_sw  = geompy.MakeVertex (-2, -1, vz)
    p_ne  = geompy.MakeVertex ( 2,  1, vz)
    p_se  = geompy.MakeVertex ( 2, -1, vz)

    arc_s = geompy.MakeArc (p_sw, p_s, p_se)
    arc_e = geompy.MakeArc (p_se, p_e, p_ne)
    arc_n = geompy.MakeArc (p_ne, p_n, p_nw)
    arc_w = geompy.MakeArc (p_nw, p_w, p_sw)

    arc_n1 = geompy.MakeArc (p_ne, p_n1, p_n)
    arc_n2 = geompy.MakeArc (p_n,  p_n2, p_nw)
    arc_s1 = geompy.MakeArc (p_sw, p_s1, p_s)
    arc_s2 = geompy.MakeArc (p_s,  p_s2, p_se)

    return doc

# ======================================================= test_asso_mano
def test_asso_mano () :

    doc = init_globale ("asso_mano")

    ed_west.addAssociation (arc_w, 0, 1)
    ed_est .addAssociation (arc_e, 0, 1)

    v_n.setAssociation (p_n)
    ed_nord2.addAssociation (arc_n1, 0.0, 1.0)
    ed_nord1.addAssociation (arc_n2, 0.0, 1.0)

    v_s.setAssociation (p_s)
    ##  ed_sud1.addAssociation (arc_s, 0.0, 0.5)
    ##  ed_sud2.addAssociation (arc_s, 0.5, 1.0)

    ed_sud1.addAssociation (arc_s1, 0.0, 1.0)
    ed_sud2.addAssociation (arc_s2, 0.0, 1.0)

    return doc

# ======================================================= test_asso_closed
def test_asso_closed () :

    doc = init_globale ("asso_closed")

    les_edges = [ ed_sud1, ed_sud2, ed_est, ed_nord1, ed_nord2,  ed_west ]
    les_arcs  = [ arc_s1,  arc_s2,  arc_e,  arc_n1,   arc_n2,    arc_w ]

    doc.associateClosedLine (v_sw, les_edges[0], les_edges[1:], 
                                   les_arcs [0], 0.0, False, les_arcs[1:])
    return doc

# ======================================================= test_asso_closed_inv
def test_asso_closed_inv () :

    doc = init_globale ("asso_closed_inv")

    les_edges = [ ed_sud1, ed_sud2, ed_est, ed_nord1, ed_nord2,  ed_west ]
    les_arcs  = [ arc_s1,  arc_s2,  arc_e,  arc_n1,   arc_n2,    arc_w ]

    doc.associateClosedLine (v_s , les_edges[0], les_edges[1:], 
                                   les_arcs [0], 1.0, True, les_arcs[1:])
    return doc

# ======================================================= test_asso_opened
def test_asso_opened () :

    doc = init_globale ("asso_opened")

    les_edges = [ ed_sud2, ed_est, ed_nord2 ]
    les_arcs  = [ arc_s,   arc_e,  arc_n ]

    doc.associateOpenedLine (les_edges[0], les_edges[1:], 
                             les_arcs [0], 0.5, les_arcs[1:], 0.5)
    return doc

# ================================================================= Begin

doc = test_asso_closed_inv  ()

law = doc.addLaw("Uniform", 3)

for j in range(doc.countPropagation()):
    propa = doc.getPropagation(j)
    propa.setLaw(law) 

mesh_hexas = hexablock.mesh (doc)


