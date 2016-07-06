# Copyright (C) 2007-2016  CEA/DEN, EDF R&D
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

##
# @package BLSURFPluginBuilder
# Python API for the MG-CADSurf meshing plug-in module.

from salome.smesh.smesh_algorithm import Mesh_Algorithm

LIBRARY = "libBLSURFEngine.so"

# Topology treatment way of MG-CADSurf
FromCAD, PreProcess, PreProcessPlus, PreCAD = 0,1,2,3

# Element size flag of MG-CADSurf
DefaultSize, DefaultGeom, MG_CADSURF_GlobalSize, MG_CADSURF_LocalSize = 0,0,1,2
# Retrocompatibility
MG_CADSURF_Custom, SizeMap = MG_CADSURF_GlobalSize, MG_CADSURF_LocalSize
BLSURF_Custom, BLSURF_GlobalSize, BLSURF_LocalSize = MG_CADSURF_Custom, MG_CADSURF_GlobalSize, MG_CADSURF_LocalSize

# import BLSURFPlugin module if possible
noBLSURFPlugin = 0
try:
  import BLSURFPlugin
except ImportError:
  noBLSURFPlugin = 1
  pass

#----------------------------
# Mesh algo type identifiers
#----------------------------

## Algorithm type: MG-CADSurf triangle algorithm, see BLSURF_Algorithm
MG_CADSurf = "MG-CADSurf"
BLSURF = MG_CADSurf

#----------------------
# Algorithms
#----------------------

## MG-CADSurf 2D algorithm.
#
#  It can be created by calling smeshBuilder.Mesh.Triangle(smeshBuilder.MG-CADSurf,geom=0)
#
class BLSURF_Algorithm(Mesh_Algorithm):

  ## name of the dynamic method in smeshBuilder.Mesh class
  #  @internal
  meshMethod = "Triangle"
  ## type of algorithm used with helper function in smeshBuilder.Mesh class
  #  @internal
  algoType   = MG_CADSurf
  ## doc string of the method
  #  @internal
  docHelper  = "Creates triangle algorithm for faces"

  _anisotropic_ratio = 0
  _bad_surface_element_aspect_ratio = 1000
  _geometric_approximation = 22
  _gradation  = 1.3
  _metric = "isotropic"
  _remove_tiny_edges = 0

  ## Private constructor.
  #  @param mesh parent mesh object algorithm is assigned to
  #  @param geom geometry (shape/sub-shape) algorithm is assigned to;
  #              if it is @c 0 (default), the algorithm is assigned to the main shape
  def __init__(self, mesh, geom=0):
    Mesh_Algorithm.__init__(self)
    if noBLSURFPlugin:
      print "Warning: BLSURFPlugin module unavailable"
    self.Create(mesh, geom, BLSURF, LIBRARY)
    self.params=None
    self.geompyD = mesh.geompyD
    #self.SetPhysicalMesh() - PAL19680
    pass

  ## Sets a way to define size of mesh elements to generate.
  #  @param thePhysicalMesh is: DefaultSize, MG_CADSURF_Custom or SizeMap.
  def SetPhysicalMesh(self, thePhysicalMesh=DefaultSize):
    physical_size_mode = thePhysicalMesh
    if self.Parameters().GetGeometricMesh() == DefaultGeom:
      if physical_size_mode == DefaultSize:
        physical_size_mode = MG_CADSURF_GlobalSize
    self.Parameters().SetPhysicalMesh(physical_size_mode)
    pass

  ## Sets a way to define maximum angular deflection of mesh from CAD model.
  #  @param theGeometricMesh is: DefaultGeom (0)) or MG_CADSURF_GlobalSize (1))
  def SetGeometricMesh(self, theGeometricMesh=DefaultGeom):
    geometric_size_mode = theGeometricMesh
    if self.Parameters().GetPhysicalMesh() == DefaultSize:
      if geometric_size_mode == DefaultGeom:
        geometric_size_mode = MG_CADSURF_GlobalSize
    self.Parameters().SetGeometricMesh(geometric_size_mode)
    pass

  ## Sets size of mesh elements to generate.
  #  @param theVal : constant global size when using a global physical size.
  #  @param isRelative : if True, the value is relative to the length of the diagonal of the bounding box
  def SetPhySize(self, theVal, isRelative = False):
    if self.Parameters().GetPhysicalMesh() == DefaultSize:
      self.SetPhysicalMesh(MG_CADSURF_GlobalSize)
    if isRelative:
      self.Parameters().SetPhySizeRel(theVal)
    else:
      self.Parameters().SetPhySize(theVal)
    pass

  ## Sets lower boundary of mesh element size.
  #  @param theVal : global minimal cell size desired.
  #  @param isRelative : if True, the value is relative to the length of the diagonal of the bounding box
  def SetMinSize(self, theVal=-1, isRelative = False):
    if isRelative:
      self.Parameters().SetMinSizeRel(theVal)
    else:
      self.Parameters().SetMinSize(theVal)
    pass

  ## Sets upper boundary of mesh element size.
  #  @param theVal : global maximal cell size desired.
  #  @param isRelative : if True, the value is relative to the length of the diagonal of the bounding box
  def SetMaxSize(self, theVal=-1, isRelative = False):
    if isRelative:
      self.Parameters().SetMaxSizeRel(theVal)
    else:
      self.Parameters().SetMaxSize(theVal)
    pass

  ## Sets angular deflection (in degrees) from CAD surface.
  #  @param theVal value of angular deflection
  def SetAngleMesh(self, theVal=_geometric_approximation):
    if self.Parameters().GetGeometricMesh() == DefaultGeom:
      self.SetGeometricMesh(MG_CADSURF_GlobalSize)
    self.Parameters().SetAngleMesh(theVal)
    pass

  ## Sets the maximum desired distance between a triangle and its supporting CAD surface
  #  @param distance the distance between a triangle and a surface
  def SetChordalError(self, distance):
    self.Parameters().SetChordalError(distance)
    pass

  ## Sets maximal allowed ratio between the lengths of two adjacent edges.
  #  @param theVal value of maximal length ratio
  def SetGradation(self, theVal=_gradation):
    if self.Parameters().GetGeometricMesh() == 0: theVal = self._gradation
    self.Parameters().SetGradation(theVal)
    pass

  ## Sets topology usage way.
  # @param way defines how mesh conformity is assured <ul>
  # <li>FromCAD - mesh conformity is assured by conformity of a shape</li>
  # <li>PreProcess or PreProcessPlus - by pre-processing a CAD model (OBSOLETE: FromCAD will be used)</li>
  # <li>PreCAD - by pre-processing with PreCAD a CAD model</li></ul>
  def SetTopology(self, way):
    if way != PreCAD and way != FromCAD:
      print "Warning: topology mode %d is no longer supported. Mode FromCAD is used."%way
      way = FromCAD
    self.Parameters().SetTopology(way)
    pass

  ## To respect geometrical edges or not.
  #  @param toIgnoreEdges "ignore edges" flag value
  def SetDecimesh(self, toIgnoreEdges=False):
    if toIgnoreEdges:
      self.SetOptionValue("respect_geometry","0")
    else:
      self.SetOptionValue("respect_geometry","1")
    pass

  ## Sets verbosity level in the range 0 to 100.
  #  @param level verbosity level
  def SetVerbosity(self, level):
    self.Parameters().SetVerbosity(level)
    pass

  ## To optimize merges edges.
  #  @param toMergeEdges "merge edges" flag value
  def SetPreCADMergeEdges(self, toMergeEdges=False):
    if self.Parameters().GetTopology() != PreCAD:
      self.SetTopology(PreCAD)
    self.Parameters().SetPreCADMergeEdges(toMergeEdges)
    pass

  ## To remove duplicate CAD Faces
  #  @param toRemoveDuplicateCADFaces "merge edges" flag value
  def SetPreCADRemoveDuplicateCADFaces(self, toRemoveDuplicateCADFaces=False):
    if self.Parameters().GetTopology() != PreCAD:
      self.SetTopology(PreCAD)
    self.Parameters().SetPreCADRemoveDuplicateCADFaces(toRemoveDuplicateCADFaces)
    pass

  ## To process 3D topology.
  #  @param toProcess "PreCAD process 3D" flag value
  def SetPreCADProcess3DTopology(self, toProcess=False):
    if self.Parameters().GetTopology() != PreCAD:
      self.SetTopology(PreCAD)
    self.Parameters().SetPreCADProcess3DTopology(toProcess)
    pass

  ## To remove nano edges.
  #  @param toRemoveNanoEdges "remove nano edges" flag value
  def SetPreCADRemoveNanoEdges(self, toRemoveNanoEdges=False):
    if toRemoveNanoEdges:
      self.SetPreCADOptionValue("remove_tiny_edges","1")
    else:
      self.SetPreCADOptionValue("remove_tiny_edges","0")
    pass

  ## To compute topology from scratch
  #  @param toDiscardInput "discard input" flag value
  def SetPreCADDiscardInput(self, toDiscardInput=False):
    if self.Parameters().GetTopology() != PreCAD:
      self.SetTopology(PreCAD)
    self.Parameters().SetPreCADDiscardInput(toDiscardInput)
    pass

  ## Sets the length below which an edge is considered as nano
  #  for the topology processing.
  #  @param epsNano nano edge length threshold value
  def SetPreCADEpsNano(self, epsNano):
    self.SetPreCADOptionValue("tiny_edge_length","%f"%epsNano)
    pass

  ## Sets advanced option value.
  #  @param optionName advanced option name
  #  @param level advanced option value
  def SetOptionValue(self, optionName, level):
    self.Parameters().SetOptionValue(optionName,level)
    pass

  ## Sets advanced PreCAD option value.
  #  @param optionName name of the option
  #  @param optionValue value of the option
  def SetPreCADOptionValue(self, optionName, optionValue):
    if self.Parameters().GetTopology() != PreCAD:
      self.SetTopology(PreCAD)
    self.Parameters().SetPreCADOptionValue(optionName,optionValue)
    pass
  
  ## Adds custom advanced option value.
  #  @param optionName custom advanced option name
  #  @param level custom advanced option value
  def AddOption(self, optionName, level):
    self.Parameters().AddOption(optionName,level)
    pass

  ## Adds custom advanced PreCAD option value.
  #  @param optionName custom name of the option
  #  @param optionValue value of the option
  def AddPreCADOption(self, optionName, optionValue):
    if self.Parameters().GetTopology() != PreCAD:
      self.SetTopology(PreCAD)
    self.Parameters().AddPreCADOption(optionName,optionValue)
    pass

  ## Sets GMF file for export at computation
  #  @param fileName GMF file name
  def SetGMFFile(self, fileName):
    self.Parameters().SetGMFFile(fileName)
    pass

  #-----------------------------------------
  # Enforced vertices (BLSURF)
  #-----------------------------------------

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
    from salome.smesh.smeshBuilder import AssureGeomPublished
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
    from salome.smesh.smeshBuilder import AssureGeomPublished
    AssureGeomPublished( self.mesh, theFace )
    if vertexName == "":
      if groupName == "":
        return self.Parameters().SetEnforcedVertex(theFace, x, y, z)
      else:
        return self.Parameters().SetEnforcedVertexWithGroup(theFace, x, y, z, groupName)
      pass
    else:
      if groupName == "":
        return self.Parameters().SetEnforcedVertexNamed(theFace, x, y, z, vertexName)
      else:
        return self.Parameters().SetEnforcedVertexNamedWithGroup(theFace, x, y, z, vertexName, groupName)
      pass
    pass

  ## To set an enforced vertex on a face (or group, compound) given a GEOM vertex, group or compound.
  #  @param theFace      : GEOM face (or group, compound) on which to define an enforced vertex
  #  @param theVertex    : GEOM vertex (or group, compound) to be projected on theFace.
  #  @param groupName    : name of the group
  def SetEnforcedVertexGeom(self, theFace, theVertex, groupName = ""):
    from salome.smesh.smeshBuilder import AssureGeomPublished
    AssureGeomPublished( self.mesh, theFace )
    AssureGeomPublished( self.mesh, theVertex )
    if groupName == "":
      return self.Parameters().SetEnforcedVertexGeom(theFace, theVertex)
    else:
      return self.Parameters().SetEnforcedVertexGeomWithGroup(theFace, theVertex,groupName)
    pass

  ## To remove an enforced vertex on a given GEOM face (or group, compound) given the coordinates.
  #  @param theFace      : GEOM face (or group, compound) on which to remove the enforced vertex
  #  @param x            : x coordinate
  #  @param y            : y coordinate
  #  @param z            : z coordinate
  def UnsetEnforcedVertex(self, theFace, x, y, z):
    from salome.smesh.smeshBuilder import AssureGeomPublished
    AssureGeomPublished( self.mesh, theFace )
    return self.Parameters().UnsetEnforcedVertex(theFace, x, y, z)

  ## To remove an enforced vertex on a given GEOM face (or group, compound) given a GEOM vertex, group or compound.
  #  @param theFace      : GEOM face (or group, compound) on which to remove the enforced vertex
  #  @param theVertex    : GEOM vertex (or group, compound) to remove.
  def UnsetEnforcedVertexGeom(self, theFace, theVertex):
    from salome.smesh.smeshBuilder import AssureGeomPublished
    AssureGeomPublished( self.mesh, theFace )
    AssureGeomPublished( self.mesh, theVertex )
    return self.Parameters().UnsetEnforcedVertexGeom(theFace, theVertex)

  ## To remove all enforced vertices on a given face.
  #  @param theFace      : face (or group/compound of faces) on which to remove all enforced vertices
  def UnsetEnforcedVertices(self, theFace):
    from salome.smesh.smeshBuilder import AssureGeomPublished
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

  #-----------------------------------------
  #  Attractors
  #-----------------------------------------

  ## Sets an attractor on the chosen face. The mesh size will decrease exponentially with the distance from theAttractor, following the rule h(d) = theEndSize - (theEndSize - theStartSize) * exp [ - ( d / theInfluenceDistance ) ^ 2 ]
  #  @param theFace      : face on which the attractor will be defined
  #  @param theAttractor : geometrical object from which the mesh size "h" decreases exponentially
  #  @param theStartSize : mesh size on theAttractor
  #  @param theEndSize   : maximum size that will be reached on theFace
  #  @param theInfluenceDistance : influence of the attractor ( the size grow slower on theFace if it's high)
  #  @param theConstantSizeDistance : distance until which the mesh size will be kept constant on theFace
  def SetAttractorGeom(self, theFace, theAttractor, theStartSize, theEndSize, theInfluenceDistance, theConstantSizeDistance):
    from salome.smesh.smeshBuilder import AssureGeomPublished
    AssureGeomPublished( self.mesh, theFace )
    AssureGeomPublished( self.mesh, theAttractor )
    self.Parameters().SetAttractorGeom(theFace, theAttractor, theStartSize, theEndSize, theInfluenceDistance, theConstantSizeDistance)
    pass

  ## Unsets an attractor on the chosen face.
  #  @param theFace      : face on which the attractor has to be removed
  def UnsetAttractorGeom(self, theFace):
    from salome.smesh.smeshBuilder import AssureGeomPublished
    AssureGeomPublished( self.mesh, theFace )
    self.Parameters().SetAttractorGeom(theFace)
    pass

  #-----------------------------------------
  # Size maps (BLSURF)
  #-----------------------------------------

  ## To set a size map on a face, edge or vertex (or group, compound) given Python function.
  #  If theObject is a face, the function can be: def f(u,v): return u+v
  #  If theObject is an edge, the function can be: def f(t): return t/2
  #  If theObject is a vertex, the function can be: def f(): return 10
  #  @param theObject   : GEOM face, edge or vertex (or group, compound) on which to define a size map
  #  @param theSizeMap  : Size map defined as a string
  def SetSizeMap(self, theObject, theSizeMap):
    from salome.smesh.smeshBuilder import AssureGeomPublished
    AssureGeomPublished( self.mesh, theObject )
    self.Parameters().SetSizeMap(theObject, theSizeMap)
    pass

  ## To set a constant size map on a face, edge or vertex (or group, compound).
  #  @param theObject   : GEOM face, edge or vertex (or group, compound) on which to define a size map
  #  @param theSizeMap  : Size map defined as a double
  def SetConstantSizeMap(self, theObject, theSizeMap):
    from salome.smesh.smeshBuilder import AssureGeomPublished
    AssureGeomPublished( self.mesh, theObject )
    self.Parameters().SetConstantSizeMap(theObject, theSizeMap)

  ## To remove a size map defined on a face, edge or vertex (or group, compound)
  #  @param theObject   : GEOM face, edge or vertex (or group, compound) on which to define a size map
  def UnsetSizeMap(self, theObject):
    from salome.smesh.smeshBuilder import AssureGeomPublished
    AssureGeomPublished( self.mesh, theObject )
    self.Parameters().UnsetSizeMap(theObject)
    pass

  ## To remove all the size maps
  def ClearSizeMaps(self):
    self.Parameters().ClearSizeMaps()
    pass

  ## Sets QuadAllowed flag.
  #  @param toAllow "allow quadrangles" flag value
  def SetQuadAllowed(self, toAllow=True):
    self.Parameters().SetQuadAllowed(toAllow)
    pass

  ## Defines hypothesis having several parameters
  #  @return hypothesis object
  def Parameters(self):
    if not self.params:
      self.params = self.Hypothesis("MG-CADSurf Parameters", [],
                                    LIBRARY, UseExisting=0)
      pass
    return self.params

  #-----------------------------------------
  # Periodicity (BLSURF with PreCAD)
  #-----------------------------------------
  
  ## Defines periodicity between two groups of faces, using PreCAD
  #  @param theFace1 : GEOM face (or group, compound) to associate with theFace2
  #  @param theFace2 : GEOM face (or group, compound) associated with theFace1
  #  @param theSourceVertices (optionnal): list of GEOM vertices on theFace1 defining the transformation from theFace1 to theFace2.
  #    If None, PreCAD tries to find a simple translation. Else, need at least 3 not aligned vertices.
  #  @param theTargetVertices (optionnal): list of GEOM vertices on theFace2 defining the transformation from theFace1 to theFace2.
  #    If None, PreCAD tries to find a simple translation. Else, need at least 3 not aligned vertices.
  def AddPreCadFacesPeriodicity(self, theFace1, theFace2, theSourceVertices=[], theTargetVertices=[]):
    """calls preCad function:
    status_t cad_add_face_multiple_periodicity_with_transformation_function(cad t *cad,
          integer *fid1, integer size1, integer *fid2, integer size2,
          periodicity_transformation_t transf, void *user data);
    """
    if theSourceVertices and theTargetVertices:
      self.Parameters().AddPreCadFacesPeriodicityWithVertices(theFace1, theFace2, theSourceVertices, theTargetVertices)
    else:
      self.Parameters().AddPreCadFacesPeriodicity(theFace1, theFace2)
    pass

  ## Defines periodicity between two groups of edges, using PreCAD
  #  @param theEdge1 : GEOM edge (or group, compound) to associate with theEdge2
  #  @param theEdge2 : GEOM edge (or group, compound) associated with theEdge1
  #  @param theSourceVertices (optionnal): list of GEOM vertices on theEdge1 defining the transformation from theEdge1 to theEdge2.
  #    If None, PreCAD tries to find a simple translation. Else, need at least 3 not aligned vertices.
  #  @param  theTargetVertices (optionnal): list of GEOM vertices on theEdge2 defining the transformation from theEdge1 to theEdge2.
  #    If None, PreCAD tries to find a simple translation. Else, need at least 3 not aligned vertices.
  def AddPreCadEdgesPeriodicity(self, theEdge1, theEdge2, theSourceVertices=[], theTargetVertices=[]):
    """calls preCad function:
    status_t cad_add_edge_multiple_periodicity_with_transformation_function(cad t *cad,
          integer *eid1, integer size1, integer *eid2, integer size2,
          periodicity_transformation_t transf, void *user data);
    """
    if theSourceVertices and theTargetVertices:
        self.Parameters().AddPreCadEdgesPeriodicityWithVertices(theEdge1, theEdge2, theSourceVertices, theTargetVertices)
    else:
        self.Parameters().AddPreCadEdgesPeriodicity(theEdge1, theEdge2)
    pass

  #=====================
  # Obsolete methods
  #=====================
  #
  # SALOME 6.6.0
  #

  ## Sets lower boundary of mesh element size (PhySize).
  def SetPhyMin(self, theVal=-1):
    """
    Obsolete function. Use SetMinSize.
    """
    print "Warning: SetPhyMin is obsolete. Please use SetMinSize"
    self.SetMinSize(theVal)
    pass

  ## Sets upper boundary of mesh element size (PhySize).
  def SetPhyMax(self, theVal=-1):
    """
    Obsolete function. Use SetMaxSize.
    """
    print "Warning: SetPhyMax is obsolete. Please use SetMaxSize"
    self.SetMaxSize(theVal)
    pass

  ## Sets angular deflection (in degrees) of a mesh face from CAD surface.
  def SetAngleMeshS(self, theVal=_geometric_approximation):
    """
    Obsolete function. Use SetAngleMesh.
    """
    print "Warning: SetAngleMeshS is obsolete. Please use SetAngleMesh"
    self.SetAngleMesh(theVal)
    pass

  ## Sets angular deflection (in degrees) of a mesh edge from CAD curve.
  def SetAngleMeshC(self, theVal=_geometric_approximation):
    """
    Obsolete function. Use SetAngleMesh.
    """
    print "Warning: SetAngleMeshC is obsolete. Please use SetAngleMesh"
    self.SetAngleMesh(theVal)
    pass

  ## Sets lower boundary of mesh element size computed to respect angular deflection.
  def SetGeoMin(self, theVal=-1):
    """
    Obsolete function. Use SetMinSize.
    """
    print "Warning: SetGeoMin is obsolete. Please use SetMinSize"
    self.SetMinSize(theVal)
    pass

  ## Sets upper boundary of mesh element size computed to respect angular deflection.
  def SetGeoMax(self, theVal=-1):
    """
    Obsolete function. Use SetMaxSize.
    """
    print "Warning: SetGeoMax is obsolete. Please use SetMaxSize"
    self.SetMaxSize(theVal)
    pass


  pass # end of BLSURF_Algorithm class
