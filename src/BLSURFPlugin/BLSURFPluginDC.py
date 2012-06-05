# Copyright (C) 2007-2012  CEA/DEN, EDF R&D
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

from smesh import Mesh_Algorithm, AssureGeomPublished

# Topology treatment way of BLSURF
FromCAD, PreProcess, PreProcessPlus, PreCAD = 0,1,2,3

# Element size flag of BLSURF
DefaultSize, DefaultGeom, BLSURF_Custom, SizeMap = 0,0,1,2


# import BLSURFPlugin module if possible
noBLSURFPlugin = 0
try:
    import BLSURFPlugin
except ImportError:
    noBLSURFPlugin = 1
    pass



# a constant to call Mesh.Triangle(BLSURF,geom=0)
BLSURF = "BLSURF"

## BLSURF 2D algorithm. It is created by calling Mesh.Triangle(BLSURF,geom=0)
#
class BLSURF_Algorithm(Mesh_Algorithm):

    meshMethod = "Triangle"
    algoType   = BLSURF

    _angleMeshS = 8
    _gradation  = 1.1

    ## Private constructor.
    def __init__(self, mesh, geom=0):
        Mesh_Algorithm.__init__(self)
        if noBLSURFPlugin:
            print "Warning: BLSURFPlugin module unavailable"            
        self.Create(mesh, geom, BLSURF, "libBLSURFEngine.so")
        self.params=None
        #self.SetPhysicalMesh() - PAL19680

    ## Sets a way to define size of mesh elements to generate.
    #  @param thePhysicalMesh is: DefaultSize, BLSURF_Custom or SizeMap.
    def SetPhysicalMesh(self, thePhysicalMesh=DefaultSize):
        self.Parameters().SetPhysicalMesh(thePhysicalMesh)

    ## Sets size of mesh elements to generate.
    def SetPhySize(self, theVal):
        self.Parameters().SetPhySize(theVal)

    ## Sets lower boundary of mesh element size (PhySize).
    def SetPhyMin(self, theVal=-1):
        self.Parameters().SetPhyMin(theVal)

    ## Sets upper boundary of mesh element size (PhySize).
    def SetPhyMax(self, theVal=-1):
        self.Parameters().SetPhyMax(theVal)

    ## Sets a way to define maximum angular deflection of mesh from CAD model.
    #  @param theGeometricMesh is: 0 (None) or 1 (Custom)
    def SetGeometricMesh(self, theGeometricMesh=0):
        if self.Parameters().GetPhysicalMesh() == 0: theGeometricMesh = 1
        self.Parameters().SetGeometricMesh(theGeometricMesh)

    ## Sets angular deflection (in degrees) of a mesh face from CAD surface.
    def SetAngleMeshS(self, theVal=_angleMeshS):
        if self.Parameters().GetGeometricMesh() == 0: theVal = self._angleMeshS
        self.Parameters().SetAngleMeshS(theVal)

    ## Sets angular deflection (in degrees) of a mesh edge from CAD curve.
    def SetAngleMeshC(self, theVal=_angleMeshS):
        if self.Parameters().GetGeometricMesh() == 0: theVal = self._angleMeshS
        self.Parameters().SetAngleMeshC(theVal)

    ## Sets lower boundary of mesh element size computed to respect angular deflection.
    def SetGeoMin(self, theVal=-1):
        self.Parameters().SetGeoMin(theVal)

    ## Sets upper boundary of mesh element size computed to respect angular deflection.
    def SetGeoMax(self, theVal=-1):
        self.Parameters().SetGeoMax(theVal)

    ## Sets maximal allowed ratio between the lengths of two adjacent edges.
    def SetGradation(self, theVal=_gradation):
        if self.Parameters().GetGeometricMesh() == 0: theVal = self._gradation
        self.Parameters().SetGradation(theVal)

    ## Sets topology usage way.
    # @param way defines how mesh conformity is assured <ul>
    # <li>FromCAD - mesh conformity is assured by conformity of a shape</li>
    # <li>PreProcess or PreProcessPlus - by pre-processing a CAD model</li>
    # <li>PreCAD - by pre-processing with PreCAD a CAD model</li></ul>
    def SetTopology(self, way):
        self.Parameters().SetTopology(way)

    ## To respect geometrical edges or not.
    def SetDecimesh(self, toIgnoreEdges=False):
        self.Parameters().SetDecimesh(toIgnoreEdges)

    ## Sets verbosity level in the range 0 to 100.
    def SetVerbosity(self, level):
        self.Parameters().SetVerbosity(level)

    ## To optimize merges edges.
    def SetPreCADMergeEdges(self, toMergeEdges=False):
        self.Parameters().SetPreCADMergeEdges(toMergeEdges)

    ## To remove nano edges.
    def SetPreCADRemoveNanoEdges(self, toRemoveNanoEdges=False):
        self.Parameters().SetPreCADRemoveNanoEdges(toRemoveNanoEdges)

    ## To compute topology from scratch
    def SetPreCADDiscardInput(self, toDiscardInput=False):
        self.Parameters().SetPreCADDiscardInput(toDiscardInput)

    ## Sets the length below which an edge is considered as nano 
    #  for the topology processing.
    def SetPreCADEpsNano(self, epsNano):
        self.Parameters().SetPreCADEpsNano(epsNano)

    ## Sets advanced option value.
    def SetOptionValue(self, optionName, level):
        self.Parameters().SetOptionValue(optionName,level)

    ## Sets advanced PreCAD option value.
    #  Keyword arguments:
    #  optionName: name of the option
    #  optionValue: value of the option
    def SetPreCADOptionValue(self, optionName, optionValue):
        self.Parameters().SetPreCADOptionValue(optionName,optionValue)

    ## Sets GMF file for export at computation
    def SetGMFFile(self, fileName):
        self.Parameters().SetGMFFile(fileName)

    ## Enforced vertices (BLSURF)

    ## To get all the enforced vertices
    def GetAllEnforcedVertices(self):
        return self.Parameters().GetAllEnforcedVertices()

    ## To get all the enforced vertices sorted by face (or group, compound)
    def GetAllEnforcedVerticesByFace(self):
        return self.Parameters().GetAllEnforcedVerticesByFace()

    ## To get all the enforced vertices sorted by coords of input vertices
    def GetAllEnforcedVerticesByCoords(self):
        return self.Parameters().GetAllEnforcedVerticesByCoords()

    ## To get all the coords of input vertices sorted by face (or group, compound)
    def GetAllCoordsByFace(self):
        return self.Parameters().GetAllCoordsByFace()

    ## To get all the enforced vertices on a face (or group, compound)
    #  @param theFace : GEOM face (or group, compound) on which to define an enforced vertex
    def GetEnforcedVertices(self, theFace):
        AssureGeomPublished( self.mesh, theFace )
        return self.Parameters().GetEnforcedVertices(theFace)

    ## To clear all the enforced vertices
    def ClearAllEnforcedVertices(self):
        return self.Parameters().ClearAllEnforcedVertices()

    ## To set an enforced vertex on a face (or group, compound) given the coordinates of a point. If the point is not on the face, it will projected on it. If there is no projection, no enforced vertex is created.
    #  @param theFace      : GEOM face (or group, compound) on which to define an enforced vertex
    #  @param x            : x coordinate
    #  @param y            : y coordinate
    #  @param z            : z coordinate
    #  @param vertexName   : name of the enforced vertex
    #  @param groupName    : name of the group
    def SetEnforcedVertex(self, theFace, x, y, z, vertexName = "", groupName = ""):
        AssureGeomPublished( self.mesh, theFace )
        if vertexName == "":
            if groupName == "":
                return self.Parameters().SetEnforcedVertex(theFace, x, y, z)
            else:
                return self.Parameters().SetEnforcedVertexWithGroup(theFace, x, y, z, groupName)
        else:
            if groupName == "":
                return self.Parameters().SetEnforcedVertexNamed(theFace, x, y, z, vertexName)
            else:
                return self.Parameters().SetEnforcedVertexNamedWithGroup(theFace, x, y, z, vertexName, groupName)

    ## To set an enforced vertex on a face (or group, compound) given a GEOM vertex, group or compound.
    #  @param theFace      : GEOM face (or group, compound) on which to define an enforced vertex
    #  @param theVertex    : GEOM vertex (or group, compound) to be projected on theFace.
    #  @param groupName    : name of the group
    def SetEnforcedVertexGeom(self, theFace, theVertex, groupName = ""):
        AssureGeomPublished( self.mesh, theFace )
        AssureGeomPublished( self.mesh, theVertex )
        if groupName == "":
            return self.Parameters().SetEnforcedVertexGeom(theFace, theVertex)
        else:
            return self.Parameters().SetEnforcedVertexGeomWithGroup(theFace, theVertex,groupName)

    ## To remove an enforced vertex on a given GEOM face (or group, compound) given the coordinates.
    #  @param theFace      : GEOM face (or group, compound) on which to remove the enforced vertex
    #  @param x            : x coordinate
    #  @param y            : y coordinate
    #  @param z            : z coordinate
    def UnsetEnforcedVertex(self, theFace, x, y, z):
        AssureGeomPublished( self.mesh, theFace )
        return self.Parameters().UnsetEnforcedVertex(theFace, x, y, z)

    ## To remove an enforced vertex on a given GEOM face (or group, compound) given a GEOM vertex, group or compound.
    #  @param theFace      : GEOM face (or group, compound) on which to remove the enforced vertex
    #  @param theVertex    : GEOM vertex (or group, compound) to remove.
    def UnsetEnforcedVertexGeom(self, theFace, theVertex):
        AssureGeomPublished( self.mesh, theFace )
        AssureGeomPublished( self.mesh, theVertex )
        return self.Parameters().UnsetEnforcedVertexGeom(theFace, theVertex)

    ## To remove all enforced vertices on a given face.
    #  @param theFace      : face (or group/compound of faces) on which to remove all enforced vertices
    def UnsetEnforcedVertices(self, theFace):
        AssureGeomPublished( self.mesh, theFace )
        return self.Parameters().UnsetEnforcedVertices(theFace)

    ## To tell BLSURF to add a node on internal vertices
    #  @param toEnforceInternalVertices : boolean; if True the internal vertices are added as enforced vertices
    def SetInternalEnforcedVertexAllFaces(self, toEnforceInternalVertices):
        return self.Parameters().SetInternalEnforcedVertexAllFaces(toEnforceInternalVertices)

    ## To know if BLSURF will add a node on internal vertices
    def GetInternalEnforcedVertexAllFaces(self):
        return self.Parameters().GetInternalEnforcedVertexAllFaces()

    ## To define a group for the nodes of internal vertices
    #  @param groupName : string; name of the group
    def SetInternalEnforcedVertexAllFacesGroup(self, groupName):
        return self.Parameters().SetInternalEnforcedVertexAllFacesGroup(groupName)

    ## To get the group name of the nodes of internal vertices
    def GetInternalEnforcedVertexAllFacesGroup(self):
        return self.Parameters().GetInternalEnforcedVertexAllFacesGroup()

    ## Attractors

    ## Sets an attractor on the chosen face. The mesh size will decrease exponentially with the distance from theAttractor, following the rule h(d) = theEndSize - (theEndSize - theStartSize) * exp [ - ( d / theInfluenceDistance ) ^ 2 ] 
    #  @param theFace      : face on which the attractor will be defined
    #  @param theAttractor : geometrical object from which the mesh size "h" decreases exponentially   
    #  @param theStartSize : mesh size on theAttractor      
    #  @param theEndSize   : maximum size that will be reached on theFace                                                     
    #  @param theInfluenceDistance : influence of the attractor ( the size grow slower on theFace if it's high)                                                      
    #  @param theConstantSizeDistance : distance until which the mesh size will be kept constant on theFace                                                      
    def SetAttractorGeom(self, theFace, theAttractor, theStartSize, theEndSize, theInfluenceDistance, theConstantSizeDistance):
        AssureGeomPublished( self.mesh, theFace )
        AssureGeomPublished( self.mesh, theAttractor )
        self.Parameters().SetAttractorGeom(theFace, theAttractor, theStartSize, theEndSize, theInfluenceDistance, theConstantSizeDistance)

    ## Unsets an attractor on the chosen face. 
    #  @param theFace      : face on which the attractor has to be removed                               
    def UnsetAttractorGeom(self, theFace):
        AssureGeomPublished( self.mesh, theFace )
        self.Parameters().SetAttractorGeom(theFace)

    ## Size maps (BLSURF)

    ## To set a size map on a face, edge or vertex (or group, compound) given Python function.
    #  If theObject is a face, the function can be: def f(u,v): return u+v
    #  If theObject is an edge, the function can be: def f(t): return t/2
    #  If theObject is a vertex, the function can be: def f(): return 10
    #  @param theObject   : GEOM face, edge or vertex (or group, compound) on which to define a size map
    #  @param theSizeMap  : Size map defined as a string
    def SetSizeMap(self, theObject, theSizeMap):
        AssureGeomPublished( self.mesh, theObject )
        self.Parameters().SetSizeMap(theObject, theSizeMap)

    ## To remove a size map defined on a face, edge or vertex (or group, compound)
    #  @param theObject   : GEOM face, edge or vertex (or group, compound) on which to define a size map
    def UnsetSizeMap(self, theObject):
        AssureGeomPublished( self.mesh, theObject )
        self.Parameters().UnsetSizeMap(theObject)

    ## To remove all the size maps
    def ClearSizeMaps(self):
        self.Parameters().ClearSizeMaps()

    ## Sets QuadAllowed flag.
    def SetQuadAllowed(self, toAllow=True):
        self.Parameters().SetQuadAllowed(toAllow)

    ## Defines hypothesis having several parameters
    #
    def Parameters(self):
        if not self.params:
            self.params = self.Hypothesis("BLSURF_Parameters", [],
                                          "libBLSURFEngine.so", UseExisting=0)
        return self.params
