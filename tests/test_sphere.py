# -*- coding: utf-8 -*-

import sys
import salome

salome.salome_init()
theStudy = salome.myStudy

###
### GEOM component
###

import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)


geompy.init_geom(theStudy)

sphere = geompy.MakeSphereR(10)
geompy.addToStudy( sphere, 'sphere' )

###
### SMESH component
###

import SMESH
from salome.smesh import smeshBuilder
smesh = smeshBuilder.New(salome.myStudy)


Mesh_1 = smesh.Mesh(sphere)

BLSURF_1 = Mesh_1.Triangle(algo=smeshBuilder.BLSURF)
BLSURF_Parameters = BLSURF_1.Parameters()
BLSURF_Parameters.SetPhySize( 5 )
BLSURF_Parameters.SetGeometricMesh( 1 )
BLSURF_Parameters.SetAngleMesh( 8 )
# Activate PreCAD
# REMARK: These lines are the default behaviour from Salome 7.5.0
# => It is not needed to add them
#BLSURF_Parameters.SetTopology( 3 )
#BLSURF_Parameters.SetPreCADMergeEdges( False )
#BLSURF_Parameters.SetPreCADProcess3DTopology( False )

ok = Mesh_1.Compute()

if not ok:
    raise Exception("Error when computing surface mesh")

Mesh_1.Tetrahedron(algo=smeshBuilder.MG_Tetra)

ok = Mesh_1.Compute()

if not ok:
    raise Exception("Error when computing volume mesh")

## Check that vertices are merged by preCAD preprocessing
#nodes = []
#for p in vertices:
    #x, y, z = geompy.PointCoordinates(p)
    #id_node = Mesh_1.FindNodeClosestTo(x, y, z)
    #nodes.append(id_node)

#nodes = set(nodes)

#assert(len(nodes) == 1)

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser(1)
