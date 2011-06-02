# -*- coding: latin-1 -*-
# Copyright (C) 2009-2011  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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


# Francis KLOSS - 2010 - CEA-Saclay, DEN, DM2S, SFME, LGLS, F-91191 Gif-sur-Yvette, France
# ========================================================================================

import geompy
import hexablock

# Build the geometry to mesh with hexahedra: a sphere
# ---------------------------------------------------

name = "Sphere"

cx = 0
cy = 0
cz = 0

radius = 5

sphere = geompy.MakeSphere(cx, cy, cz, radius)

geompy.addToStudy(sphere, name)

# Add a new document
# ------------------

doc = hexablock.addDocument()

# fkl: doc.addShape(sphere)

# Build the model of blocks: a spherical grid
# -------------------------------------------

center = doc.addVertex(cx, cy, cz)

axis_x = doc.addVector(1, 0, 0)
axis_y = doc.addVector(0, 1, 0)
axis_z = doc.addVector(0, 0, 1)

#fkl: doc.makeSpherical(center, axis_x, 3, 1.0)
doc.makeCartesian(center, axis_x, axis_y, axis_z, 5, 4, 3)

# Associate the model of block to the geometry
# --------------------------------------------

# fkl: to do

# Define group of all hexahedra
# -----------------------------

group_h = doc.addHexaGroup(name+":hexas")
for i in xrange(doc.countHexa()):
    e = doc.getHexa(i)
    group_h.addElement(e)

# Define group of all quadrangles
# -------------------------------

group_q = doc.addQuadGroup(name+":quadrangles")
for i in xrange(doc.countQuad()):
    e = doc.getQuad(i)
    group_q.addElement(e)

# Define the laws for discretization
# ----------------------------------

law = doc.addLaw("Uniform" , 3)

# Set the law on all edges of the model of blocks
# -----------------------------------------------

for i in xrange(doc.countPropagation()):
    p = doc.getPropagation(i)
    p.setLaw(law)

# Generate the hexehadral mesh on FactoryServer
# ---------------------------------------------

mesh_1 = hexablock.mesh(name, doc)

print "Number of hexaedra   :", mesh_1.NbHexas()
print "Number of quadrangles:", mesh_1.NbQuadrangles()
print "Number of segments   :", mesh_1.NbEdges()
print "Number de nodes      :", mesh_1.NbNodes()

# Generate the hexehadral mesh on FooBar container
# ------------------------------------------------

container = "FooBar"

mesh_2 = hexablock.mesh(name+":"+container, doc, 3, container)

print "Number of hexaedra   :", mesh_2.NbHexas()
print "Number of quadrangles:", mesh_2.NbQuadrangles()
print "Number of segments   :", mesh_2.NbEdges()
print "Number de nodes      :", mesh_2.NbNodes()
