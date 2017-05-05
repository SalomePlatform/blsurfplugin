# -*- coding: utf-8 -*-

import os
import sys
import salome
import time

salome.salome_init()
theStudy = salome.myStudy

###
### GEOM component
###

import GEOM
from salome.geom import geomBuilder
import math
import SALOMEDS


geompy = geomBuilder.New(theStudy)

O = geompy.MakeVertex(0, 0, 0)
OX = geompy.MakeVectorDXDYDZ(1, 0, 0)
OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)
flight_solid_brep_1 = geompy.ImportBREP(os.path.join(os.getenv("DATA_DIR"),"Shapes/Brep/flight_solid.brep" ))
geompy.addToStudy( O, 'O' )
geompy.addToStudy( OX, 'OX' )
geompy.addToStudy( OY, 'OY' )
geompy.addToStudy( OZ, 'OZ' )
geompy.addToStudy( flight_solid_brep_1, 'flight_solid.brep_1' )

###
### SMESH component
###

import  SMESH, SALOMEDS
from salome.smesh import smeshBuilder

smesh = smeshBuilder.New(theStudy)

Mesh_1 = smesh.Mesh(flight_solid_brep_1)
MG_CADSurf = Mesh_1.Triangle(algo=smeshBuilder.MG_CADSurf)
MG_CADSurf_Parameters_1 = MG_CADSurf.Parameters()
MG_CADSurf_Parameters_1.SetPhySize( 1 )
MG_CADSurf_Parameters_1.SetMaxSize( 1 )
MG_CADSurf_Parameters_1.SetGradation( 1.05 )
MG_CADSurf_Parameters_1.SetAngleMesh( 1 )
MG_CADSurf_Parameters_1.SetChordalError( 2.40018 )
# 4 procs are used by default
# => No need to set an option

time0 = time.time()
isDone = Mesh_1.Compute()
time1 = time.time()

time_multithread = time1-time0

print "Time in 4 proc: %.3s"%(time_multithread)

Mesh_2 = smesh.Mesh(flight_solid_brep_1)
MG_CADSurf = Mesh_2.Triangle(algo=smeshBuilder.MG_CADSurf)
MG_CADSurf_Parameters_2 = MG_CADSurf.Parameters()
MG_CADSurf_Parameters_2.SetPhySize( 1 )
MG_CADSurf_Parameters_2.SetMaxSize( 1 )
MG_CADSurf_Parameters_2.SetGradation( 1.05 )
MG_CADSurf_Parameters_2.SetAngleMesh( 1 )
MG_CADSurf_Parameters_2.SetChordalError( 2.40018 )
# Use only one thread
MG_CADSurf_Parameters_2.SetMaxNumberOfThreads( 1 )

time2 = time.time()
isDone = Mesh_2.Compute()
time3 = time.time()

time_singlethread = time3-time2
print "Time in 1 proc: %.3s"%(time_singlethread)

assert time_multithread < time_singlethread/2.

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser(True)