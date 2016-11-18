// Copyright (C) 2007-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// ---
// File    : BLSURFPlugin_Hypothesis.hxx
// Authors : Francis KLOSS (OCC) & Patrick LAUG (INRIA) & Lioka RAZAFINDRAZAKA (CEA)
//           Size maps developement: Nicolas GEIMER (OCC) & Gilles DAVID (EURIWARE)
// ---
//
#ifndef _BLSURFPlugin_Hypothesis_i_HXX_
#define _BLSURFPlugin_Hypothesis_i_HXX_

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(BLSURFPlugin_Algorithm)

#include "SMESH_Hypothesis_i.hxx"
#include "BLSURFPlugin_Hypothesis.hxx"

class SMESH_Gen;
class GEOM_Object;

// BLSURFPlugin parameters hypothesis

class BLSURFPlugin_Hypothesis_i: public virtual POA_BLSURFPlugin::BLSURFPlugin_Hypothesis,
    public virtual SMESH_Hypothesis_i {
public:
  // Constructor
  BLSURFPlugin_Hypothesis_i(PortableServer::POA_ptr thePOA, int theStudyId, ::SMESH_Gen* theGenImpl);
  // Destructor
  virtual ~BLSURFPlugin_Hypothesis_i();

  void SetPhysicalMesh(CORBA::Long theValue);
  CORBA::Long GetPhysicalMesh();

  void SetGeometricMesh(CORBA::Long theValue);
  CORBA::Long GetGeometricMesh();

  void SetPhySize(CORBA::Double theValue);
  void SetPhySizeRel(CORBA::Double theValue);
  CORBA::Double GetPhySize();
  CORBA::Boolean IsPhySizeRel();

  void SetMinSize(CORBA::Double theMinSize);
  void SetMinSizeRel(CORBA::Double theMinSize);
  CORBA::Double GetMinSize();
  CORBA::Boolean IsMinSizeRel();

  void SetMaxSize(CORBA::Double theMaxSize);
  void SetMaxSizeRel(CORBA::Double theMaxSize);
  CORBA::Double GetMaxSize();
  CORBA::Boolean IsMaxSizeRel();

  void SetUseGradation(CORBA::Boolean theValue);
  CORBA::Boolean GetUseGradation();
  void SetGradation(CORBA::Double theValue);
  CORBA::Double GetGradation();

  void SetUseVolumeGradation(CORBA::Boolean theValue);
  CORBA::Boolean GetUseVolumeGradation();
  void SetVolumeGradation(CORBA::Double theValue);
  CORBA::Double GetVolumeGradation();

  void SetQuadAllowed(CORBA::Boolean theValue);
  CORBA::Boolean GetQuadAllowed();

  void SetAngleMesh(CORBA::Double theValue);
  CORBA::Double GetAngleMesh();

  void SetChordalError(CORBA::Double distance);
  CORBA::Double GetChordalError();

  void SetAnisotropic(CORBA::Boolean anisotropic);
  CORBA::Boolean GetAnisotropic();

  void SetAnisotropicRatio(CORBA::Double ratio);
  CORBA::Double GetAnisotropicRatio();

  void SetRemoveTinyEdges(CORBA::Boolean remove);
  CORBA::Boolean GetRemoveTinyEdges();

  void SetTinyEdgeLength(CORBA::Double length);
  CORBA::Double GetTinyEdgeLength();

  void SetOptimiseTinyEdges(CORBA::Boolean toOptimise);
  CORBA::Boolean GetOptimiseTinyEdges();

  void SetTinyEdgeOptimisationLength(CORBA::Double length);
  CORBA::Double GetTinyEdgeOptimisationLength();

  void SetCorrectSurfaceIntersection(CORBA::Boolean toOptimise);
  CORBA::Boolean GetCorrectSurfaceIntersection();

  void SetCorrectSurfaceIntersectionMaxCost(CORBA::Double maxCost);
  CORBA::Double GetCorrectSurfaceIntersectionMaxCost();

  void SetBadElementRemoval(CORBA::Boolean remove);
  CORBA::Boolean GetBadElementRemoval();

  void SetBadElementAspectRatio(CORBA::Double ratio);
  CORBA::Double GetBadElementAspectRatio();

  void SetOptimizeMesh(CORBA::Boolean optimize);
  CORBA::Boolean GetOptimizeMesh();

  void SetQuadraticMesh(CORBA::Boolean quadratic);
  CORBA::Boolean GetQuadraticMesh();

  void SetTopology(CORBA::Long theValue);
  CORBA::Long GetTopology();

  void SetVerbosity(CORBA::Short theVal) throw (SALOME::SALOME_Exception);
  CORBA::Short GetVerbosity();

  void SetEnforceCadEdgesSize( CORBA::Boolean toEnforce );
  CORBA::Boolean GetEnforceCadEdgesSize();

  void SetJacobianRectificationRespectGeometry( CORBA::Boolean allowRectification );
  CORBA::Boolean GetJacobianRectificationRespectGeometry();

  void SetJacobianRectification( CORBA::Boolean allowRectification );
  CORBA::Boolean GetJacobianRectification();

  void SetMaxNumberOfPointsPerPatch( CORBA::Long nb ) throw (SALOME::SALOME_Exception);
  CORBA::Long GetMaxNumberOfPointsPerPatch();

  void SetRespectGeometry( CORBA::Boolean toRespect );
  CORBA::Boolean GetRespectGeometry();

  void SetTinyEdgesAvoidSurfaceIntersections( CORBA::Boolean toAvoidIntersection );
  CORBA::Boolean GetTinyEdgesAvoidSurfaceIntersections();

  void SetClosedGeometry( CORBA::Boolean isClosed );
  CORBA::Boolean GetClosedGeometry();

  void SetDebug( CORBA::Boolean isDebug );
  bool GetDebug();

  void SetPeriodicTolerance( CORBA::Double tol ) throw (SALOME::SALOME_Exception);
  double GetPeriodicTolerance() throw (SALOME::SALOME_Exception);

  void SetRequiredEntities( const char* howToTreat ) throw (SALOME::SALOME_Exception);
  char* GetRequiredEntities();

  void SetSewingTolerance( CORBA::Double tol ) throw (SALOME::SALOME_Exception);
  CORBA::Double GetSewingTolerance() throw (SALOME::SALOME_Exception);

  void SetTags( const char* howToTreat ) throw (SALOME::SALOME_Exception);
  char* GetTags();

  void SetHyperPatches(const BLSURFPlugin::THyperPatchList& hpl);
  BLSURFPlugin::THyperPatchList* GetHyperPatches();

  void SetPreCADMergeEdges(CORBA::Boolean theValue);
  CORBA::Boolean GetPreCADMergeEdges();

  void SetPreCADRemoveDuplicateCADFaces(CORBA::Boolean theValue);
  CORBA::Boolean GetPreCADRemoveDuplicateCADFaces();

  void SetPreCADProcess3DTopology(CORBA::Boolean theValue);
  CORBA::Boolean GetPreCADProcess3DTopology();

  void SetPreCADDiscardInput(CORBA::Boolean theValue);
  CORBA::Boolean GetPreCADDiscardInput();

  void SetOptionValue(const char* optionName, const char* optionValue) throw (SALOME::SALOME_Exception);
  void SetPreCADOptionValue(const char* optionName, const char* optionValue) throw (SALOME::SALOME_Exception);
  char* GetOptionValue(const char* optionName) throw (SALOME::SALOME_Exception);
  char* GetPreCADOptionValue(const char* optionName) throw (SALOME::SALOME_Exception);

  void UnsetOption(const char* optionName);
  void UnsetPreCADOption(const char* optionName);

  BLSURFPlugin::string_array* GetOptionValues();
  BLSURFPlugin::string_array* GetPreCADOptionValues();
  BLSURFPlugin::string_array* GetAdvancedOptionValues();

  void SetOptionValues(const BLSURFPlugin::string_array& options) throw (SALOME::SALOME_Exception);
  void SetPreCADOptionValues(const BLSURFPlugin::string_array& options) throw (SALOME::SALOME_Exception);
  void SetAdvancedOptionValues(const BLSURFPlugin::string_array& options);
  void SetAdvancedOption(const char* optionsAndValues) throw (SALOME::SALOME_Exception);

  void AddOption(const char* optionName, const char* optionValue);
  void AddPreCADOption(const char* optionName, const char* optionValue);
  char* GetOption(const char* optionName);
  char* GetPreCADOption(const char* optionName);

  void SetSizeMapEntry(const char* entry, const char* sizeMap) throw (SALOME::SALOME_Exception);

  void SetConstantSizeMapEntry(const char* entry, GEOM::shape_type shapeType, CORBA::Double sizeMap) throw (SALOME::SALOME_Exception);

  char* GetSizeMapEntry(const char* entry) throw (SALOME::SALOME_Exception);

  void UnsetEntry(const char* entry);

  BLSURFPlugin::string_array* GetSizeMapEntries();

  void SetSizeMapEntries(const BLSURFPlugin::string_array& sizeMaps) throw (SALOME::SALOME_Exception);

  void SetSizeMap(GEOM::GEOM_Object_ptr GeomObj, const char* sizeMap) throw (SALOME::SALOME_Exception);

  void SetConstantSizeMap(GEOM::GEOM_Object_ptr GeomObj, CORBA::Double sizeMap);

  void UnsetSizeMap(GEOM::GEOM_Object_ptr GeomObj);

  void ClearSizeMaps();

  void SetAttractor(GEOM::GEOM_Object_ptr GeomObj, const char* attractor);

  void UnsetAttractor(GEOM::GEOM_Object_ptr GeomObj);

  void SetAttractorEntry(const char* entry, const char* attractor) throw (SALOME::SALOME_Exception);

  char* GetAttractorEntry(const char* entry) throw (SALOME::SALOME_Exception);

  BLSURFPlugin::string_array* GetAttractorEntries();
  
  
  /*!
    * Set/get/unset an attractor on a face 
    */
  
  void SetAttractorGeom(GEOM::GEOM_Object_ptr GeomObj, GEOM::GEOM_Object_ptr Attractor, CORBA::Double StartSize, CORBA::Double EndSize, CORBA::Double ActionRadius, CORBA::Double ConstantRadius );

  void UnsetAttractorGeom(GEOM::GEOM_Object_ptr GeomObj,
                          GEOM::GEOM_Object_ptr theAttractor);

  void UnsetAttractorEntry(const char* entry, const char* attractor);
  void SetClassAttractorEntry(const char* entry, const char* att_entry, CORBA::Double StartSize, CORBA::Double EndSize, CORBA::Double ActionRadius, CORBA::Double ConstantRadius)  throw (SALOME::SALOME_Exception);

  BLSURFPlugin::TAttParamsMap* GetAttractorParams();
  

  /*
   void SetCustomSizeMap(GEOM::GEOM_Object_ptr GeomObj, const char* sizeMap);

   void UnsetCustomSizeMap(GEOM::GEOM_Object_ptr GeomObj);

   void SetCustomSizeMapEntry(const char* entry,const char* sizeMap )  throw (SALOME::SALOME_Exception);

   char* GetCustomSizeMapEntry(const char* entry)  throw (SALOME::SALOME_Exception);

   BLSURFPlugin::string_array* GetCustomSizeMapEntries();
   */

  ///////////////////////
  // ENFORCED VERTEXES //
  ///////////////////////

  BLSURFPlugin::TFaceEntryEnfVertexListMap* GetAllEnforcedVerticesByFace();
  BLSURFPlugin::TEnfVertexList* GetAllEnforcedVertices();

  BLSURFPlugin::TFaceEntryCoordsListMap* GetAllCoordsByFace();
  BLSURFPlugin::TCoordsEnfVertexMap* GetAllEnforcedVerticesByCoords();

  BLSURFPlugin::TFaceEntryEnfVertexEntryListMap* GetAllEnfVertexEntriesByFace();
  BLSURFPlugin::TEnfVertexEntryEnfVertexMap* GetAllEnforcedVerticesByEnfVertexEntry();

  void ClearAllEnforcedVertices();

  /*!
   * Set/get/unset an enforced vertex on geom object
   */
  // OBSOLETE
  bool SetEnforcedVertex(GEOM::GEOM_Object_ptr theFace, CORBA::Double x, CORBA::Double y, CORBA::Double z)
      throw (SALOME::SALOME_Exception);
  bool SetEnforcedVertexNamed(GEOM::GEOM_Object_ptr theFace, CORBA::Double x, CORBA::Double y, CORBA::Double z, const char* theVertexName)
      throw (SALOME::SALOME_Exception);
  bool SetEnforcedVertexGeom(GEOM::GEOM_Object_ptr theFace, GEOM::GEOM_Object_ptr theVertex)
      throw (SALOME::SALOME_Exception);
  bool SetEnforcedVertexWithGroup(GEOM::GEOM_Object_ptr theFace, CORBA::Double x, CORBA::Double y, CORBA::Double z, const char* theGroupName)
      throw (SALOME::SALOME_Exception);
  bool SetEnforcedVertexNamedWithGroup(GEOM::GEOM_Object_ptr theFace, CORBA::Double x, CORBA::Double y, CORBA::Double z, const char* theVertexName, const char* theGroupName)
      throw (SALOME::SALOME_Exception);
  bool SetEnforcedVertexGeomWithGroup(GEOM::GEOM_Object_ptr theFace, GEOM::GEOM_Object_ptr theVertex, const char* theGroupName)
      throw (SALOME::SALOME_Exception);

  BLSURFPlugin::TEnfVertexList* GetEnforcedVertices(GEOM::GEOM_Object_ptr theFace) throw (SALOME::SALOME_Exception);

  bool UnsetEnforcedVertex(GEOM::GEOM_Object_ptr theFace, CORBA::Double x, CORBA::Double y, CORBA::Double z)
      throw (SALOME::SALOME_Exception);
  bool UnsetEnforcedVertexGeom(GEOM::GEOM_Object_ptr theFace, GEOM::GEOM_Object_ptr theVertex)
      throw (SALOME::SALOME_Exception);
  bool UnsetEnforcedVertices(GEOM::GEOM_Object_ptr theFace) throw (SALOME::SALOME_Exception);

  // NEW - no face
  bool AddEnforcedVertex(CORBA::Double x, CORBA::Double y, CORBA::Double z)
      throw (SALOME::SALOME_Exception);
  bool AddEnforcedVertexNamed(CORBA::Double x, CORBA::Double y, CORBA::Double z, const char* theVertexName)
      throw (SALOME::SALOME_Exception);
  bool AddEnforcedVertexGeom(GEOM::GEOM_Object_ptr theVertex)
      throw (SALOME::SALOME_Exception);
  bool AddEnforcedVertexWithGroup(CORBA::Double x, CORBA::Double y, CORBA::Double z, const char* theGroupName)
      throw (SALOME::SALOME_Exception);
  bool AddEnforcedVertexNamedWithGroup(CORBA::Double x, CORBA::Double y, CORBA::Double z, const char* theVertexName, const char* theGroupName)
      throw (SALOME::SALOME_Exception);
  bool AddEnforcedVertexGeomWithGroup(GEOM::GEOM_Object_ptr theVertex, const char* theGroupName)
      throw (SALOME::SALOME_Exception);

  bool RemoveEnforcedVertex(CORBA::Double x, CORBA::Double y, CORBA::Double z)
      throw (SALOME::SALOME_Exception);
  bool RemoveEnforcedVertexGeom(GEOM::GEOM_Object_ptr theVertex)
      throw (SALOME::SALOME_Exception);
  bool RemoveEnforcedVertices() throw (SALOME::SALOME_Exception);

  /*!
   * Set/get/unset an enforced vertex on geom object given by entry
   */
  bool SetEnforcedVertexEntry(const char* theFaceEntry, CORBA::Double x = 0, CORBA::Double y = 0, CORBA::Double z = 0,
      const char* theVertexName = "", const char* theVertexEntry = "", const char* theGroupName = "")
      throw (SALOME::SALOME_Exception);

  BLSURFPlugin::TEnfVertexList* GetEnforcedVerticesEntry(const char* theFaceEntry) throw (SALOME::SALOME_Exception);

  bool UnsetEnforcedVertexEntry(const char* theFaceEntry, CORBA::Double x, CORBA::Double y, CORBA::Double z,
      const char* theVertexEntry = "") throw (SALOME::SALOME_Exception);
  bool UnsetEnforcedVerticesEntry(const char* theFaceEntry) throw (SALOME::SALOME_Exception);

  /*!
   * To add internal vertices as enforced vertices
   */
  void SetInternalEnforcedVertexAllFaces(CORBA::Boolean toEnforceInternalVertices);
  CORBA::Boolean GetInternalEnforcedVertexAllFaces();
  void SetInternalEnforcedVertexAllFacesGroup(const char*  groupName = "");
  char* GetInternalEnforcedVertexAllFacesGroup();

// Enable internal enforced vertices on specific face if requested by user
//  void SetInternalEnforcedVertex(GEOM::GEOM_Object_ptr theFace, CORBA::Boolean toEnforceInternalVertices) throw (SALOME::SALOME_Exception);
//  void SetInternalEnforcedVertexWithGroup(GEOM::GEOM_Object_ptr theFace, CORBA::Boolean toEnforceInternalVertices, const char* theGroupName = "") throw (SALOME::SALOME_Exception);
//  void SetInternalEnforcedVertexEntry(const char* theFaceEntry, CORBA::Boolean toEnforceInternalVertices, const char* theGroupName = "") throw (SALOME::SALOME_Exception);
//  CORBA::Boolean GetInternalEnforcedVertex(GEOM::GEOM_Object_ptr theFace) throw (SALOME::SALOME_Exception);
//  CORBA::Boolean GetInternalEnforcedVertexEntry(const char* theFaceEntry) throw (SALOME::SALOME_Exception);
  
  ///////////////////////
  // PERIODICITY       //
  ///////////////////////

  void ClearPreCadPeriodicityVectors();

  BLSURFPlugin::TPeriodicityList* GetPreCadFacesPeriodicityVector();
  BLSURFPlugin::TPeriodicityList* GetPreCadEdgesPeriodicityVector();

  BLSURFPlugin::TPeriodicityList* PreCadVectorToSequence(const ::BLSURFPlugin_Hypothesis::TPreCadPeriodicityVector& preCadPeriodicityVector);

  void AddPreCadFacesPeriodicity(GEOM::GEOM_Object_ptr theFace1, GEOM::GEOM_Object_ptr theFace2)
      throw (SALOME::SALOME_Exception);

  void AddPreCadFacesPeriodicityWithVertices(GEOM::GEOM_Object_ptr theFace1, GEOM::GEOM_Object_ptr theFace2,
      const GEOM::ListOfGO& theSourceVertices, const GEOM::ListOfGO& theTargetVertices)
      throw (SALOME::SALOME_Exception);

  void AddPreCadFacesPeriodicityEntry(const char* theFace1Entry, const char* theFace2Entry,
      const BLSURFPlugin::TEntryList& theSourceVerticesEntries, const BLSURFPlugin::TEntryList& theTargetVerticesEntries)
      throw (SALOME::SALOME_Exception);

  void AddPreCadEdgesPeriodicity(GEOM::GEOM_Object_ptr theEdge1, GEOM::GEOM_Object_ptr theEdge2)
      throw (SALOME::SALOME_Exception);

  void AddPreCadEdgesPeriodicityWithVertices(GEOM::GEOM_Object_ptr theEdge1, GEOM::GEOM_Object_ptr theEdge2,
      const GEOM::ListOfGO& theSourceVertices, const GEOM::ListOfGO& theTargetVertices)
      throw (SALOME::SALOME_Exception);

  void AddPreCadEdgesPeriodicityEntry(const char* theEdge1Entry, const char* theEdge2Entry,
      const BLSURFPlugin::TEntryList& theSourceVerticesEntries, const BLSURFPlugin::TEntryList& theTargetVerticesEntries)
      throw (SALOME::SALOME_Exception);

  ///////////////////////
  
  /*!
    * Sets the file for export resulting mesh in GMF format
    */
//   void SetGMFFile(const char* theFileName, CORBA::Boolean isBinary);
  void SetGMFFile(const char* theFileName);
  char* GetGMFFile();
//   CORBA::Boolean GetGMFFileMode();

  // Get implementation
  ::BLSURFPlugin_Hypothesis* GetImpl();

  // Verify whether hypothesis supports given entity type 
  CORBA::Boolean IsDimSupported(SMESH::Dimension type);


  //
  // Obsolete methods - To be removed in V7
  //
  void SetPhyMin(CORBA::Double theMinSize);
  CORBA::Double GetPhyMin();
  void SetPhyMax(CORBA::Double theMaxSize);
  CORBA::Double GetPhyMax();
  void SetGeoMin(CORBA::Double theMinSize);
  CORBA::Double GetGeoMin();
  void SetGeoMax(CORBA::Double theMaxSize);
  CORBA::Double GetGeoMax();
  void SetAngleMeshS(CORBA::Double angle);
  CORBA::Double GetAngleMeshS();
  void SetAngleMeshC(CORBA::Double angle);
  CORBA::Double GetAngleMeshC();
  void SetDecimesh(CORBA::Boolean toIgnoreEdges);
  CORBA::Boolean GetDecimesh();
  void SetPreCADRemoveNanoEdges(CORBA::Boolean toRemoveNanoEdges);
  CORBA::Boolean GetPreCADRemoveNanoEdges();
  void SetPreCADEpsNano(CORBA::Double epsNano);
  CORBA::Double GetPreCADEpsNano();

private:
  ///////////////////////
  // PERIODICITY       //
  ///////////////////////
  std::string ShapeTypeToString(GEOM::shape_type theShapeType);
  void CheckShapeType(GEOM::GEOM_Object_ptr shape, GEOM::shape_type theShapeType);
  void CheckShapeTypes(GEOM::GEOM_Object_ptr shape, std::vector<GEOM::shape_type> theShapeTypes);
  std::string PublishIfNeeded(GEOM::GEOM_Object_ptr shape, GEOM::shape_type theShapeType, std::string prefix);
  std::string FormatVerticesEntries(std::vector<std::string> &theSourceVerticesEntries, std::vector<std::string> &theTargetVerticesEntries);

};

#endif
