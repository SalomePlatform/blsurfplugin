// Copyright (C) 2007-2013  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
// File    : BLSURFPlugin_Hypothesis.cxx
// Authors : Francis KLOSS (OCC) & Patrick LAUG (INRIA) & Lioka RAZAFINDRAZAKA (CEA)
//           & Aurelien ALLEAUME (DISTENE)
//           Size maps development: Nicolas GEIMER (OCC) & Gilles DAVID (EURIWARE)
// ---
//
#include "BLSURFPlugin_Hypothesis.hxx"
#include "BLSURFPlugin_Attractor.hxx"
#include "SMESH_Gen_i.hxx"
#include <utilities.h>
#include <cstring>
#include <iostream>
#include <sstream>

// cascade include
#include "ShapeAnalysis.hxx"

// CORBA includes
#include CORBA_CLIENT_HEADER(SALOMEDS)
#include CORBA_CLIENT_HEADER(GEOM_Gen)

//=============================================================================
BLSURFPlugin_Hypothesis::BLSURFPlugin_Hypothesis(int hypId, int studyId, SMESH_Gen * gen) :
  SMESH_Hypothesis(hypId, studyId, gen), 
  _physicalMesh(GetDefaultPhysicalMesh()),
  _geometricMesh(GetDefaultGeometricMesh()),
  _phySize(GetDefaultPhySize()),
  _phySizeRel(GetDefaultPhySizeRel()),
  _minSize(GetDefaultMinSize()),
  _minSizeRel(GetDefaultMinSizeRel()),
  _maxSize(GetDefaultMaxSize()),
  _maxSizeRel(GetDefaultMaxSizeRel()),
  _gradation(GetDefaultGradation()),
  _quadAllowed(GetDefaultQuadAllowed()),
  _angleMesh(GetDefaultAngleMesh()),
  _chordalError(GetDefaultChordalError()), 
  _anisotropic(GetDefaultAnisotropic()),
  _anisotropicRatio(GetDefaultAnisotropicRatio()),
  _removeTinyEdges(GetDefaultRemoveTinyEdges()),
  _tinyEdgeLength(GetDefaultTinyEdgeLength()),
  _badElementRemoval(GetDefaultBadElementRemoval()),
  _badElementAspectRatio(GetDefaultBadElementAspectRatio()),
  _optimizeMesh(GetDefaultOptimizeMesh()),
  _quadraticMesh(GetDefaultQuadraticMesh()),
  _verb(GetDefaultVerbosity()),
  _topology(GetDefaultTopology()),
  _preCADMergeEdges(GetDefaultPreCADMergeEdges()),
  _preCADProcess3DTopology(GetDefaultPreCADProcess3DTopology()),
  _preCADDiscardInput(GetDefaultPreCADDiscardInput()),
  _sizeMap(GetDefaultSizeMap()),
  _attractors(GetDefaultSizeMap()),
  _classAttractors(GetDefaultAttractorMap()),
  _faceEntryEnfVertexListMap(GetDefaultFaceEntryEnfVertexListMap()),
  _enfVertexList(GetDefaultEnfVertexList()),
  _faceEntryCoordsListMap(GetDefaultFaceEntryCoordsListMap()),
  _coordsEnfVertexMap(GetDefaultCoordsEnfVertexMap()),
  _faceEntryEnfVertexEntryListMap(GetDefaultFaceEntryEnfVertexEntryListMap()),
  _enfVertexEntryEnfVertexMap(GetDefaultEnfVertexEntryEnfVertexMap()),
  _groupNameNodeIDMap(GetDefaultGroupNameNodeIDMap()),
  _GMFFileName(GetDefaultGMFFile()),
  _enforcedInternalVerticesAllFaces(GetDefaultInternalEnforcedVertex()),
  _preCadFacesPeriodicityVector(GetDefaultPreCadFacesPeriodicityVector()),
  _preCadEdgesPeriodicityVector(GetDefaultPreCadEdgesPeriodicityVector())
{
  _name = "BLSURF_Parameters";
  _param_algo_dim = 2;
  
//   _GMFFileMode = false; // GMF ascii mode

  const char* boolOptionNames[] = {         "correct_surface_intersections",            // default = 1
                                            "create_tag_on_collision",                  // default = 1
                                            "debug",                                    // default = 0
                                            "enforce_cad_edge_sizes",                   // default = 0
                                            "frontal",                                  // ok default = 1
                                            "jacobian_rectification_respect_geometry",  // default = 1
                                            "proximity",                                // default = 0
                                            "rectify_jacobian",                         // default = 1
                                            "respect_geometry",                         // default = 1
                                            "" // mark of end
      };

  const char* intOptionNames[] = {          "hinterpol_flag",                           // ok default = 0
                                            "hmean_flag",                               // ok default = 0
                                            "max_number_of_points_per_patch",           // default = 100000
                                            "prox_nb_layer",                            // detects the volumic proximity of surfaces
                                            "" // mark of end
      };
  const char* doubleOptionNames[] = {       "surface_intersections_processing_max_cost",// default = 15
                                            "periodic_tolerance",                       // default = diag/100
                                            "prox_ratio",
                                            "" // mark of end
      };
  const char* charOptionNames[] = {         "required_entities",                        // default = "respect"
                                            "tags",                                     // default = "respect"
                                            "" // mark of end
      };

  // PreCAD advanced options
  const char* preCADboolOptionNames[] = {   "closed_geometry",                          // default = 0
                                            "create_tag_on_collision",                  // default = 1
                                            "debug",                                    // default = 0 
                                            "remove_tiny_edges",                        // default = 0
                                            "" // mark of end
      };
  const char* preCADintOptionNames[] = {    "manifold_geometry",                        // default = 0
                                            "" // mark of end
      };
  const char* preCADdoubleOptionNames[] = { "periodic_tolerance",                       // default = diag * 1e-5
                                            "sewing_tolerance",                         // default = diag * 5e-4
                                            "tiny_edge_length",                         // default = diag * 1e-5
                                            "" // mark of end
      };
  const char* preCADcharOptionNames[] = {   "required_entities",                        // default = "respect"
                                            "tags",                                     // default = "respect"
                                            "" // mark of end
      };
  
  int i = 0;
  while (boolOptionNames[i][0])
    _option2value[boolOptionNames[i++]].clear();
  
  i = 0;
  while (preCADboolOptionNames[i][0])
    _preCADoption2value[preCADboolOptionNames[i++]].clear();
  
  i = 0;
  while (intOptionNames[i][0])
    _option2value[intOptionNames[i++]].clear();
  
  i = 0;
  while (preCADintOptionNames[i][0])
    _preCADoption2value[preCADintOptionNames[i++]].clear();

  i = 0;
  while (doubleOptionNames[i][0]) {
    _doubleOptions.insert(doubleOptionNames[i]);
    _option2value[doubleOptionNames[i++]].clear();
  }
  i = 0;
  while (preCADdoubleOptionNames[i][0]) {
    _preCADdoubleOptions.insert(preCADdoubleOptionNames[i]);
    _preCADoption2value[preCADdoubleOptionNames[i++]].clear();
  }
  i = 0;
  while (charOptionNames[i][0]) {
    _charOptions.insert(charOptionNames[i]);
    _option2value[charOptionNames[i++]].clear();
  }
  i = 0;
  while (preCADcharOptionNames[i][0]) {
    _preCADdoubleOptions.insert(preCADcharOptionNames[i]);
    _preCADoption2value[preCADdoubleOptionNames[i++]].clear();
  }
  

      
  _sizeMap.clear();
  _attractors.clear();
  _faceEntryEnfVertexListMap.clear();
  _enfVertexList.clear();
  _faceEntryCoordsListMap.clear();
  _coordsEnfVertexMap.clear();
  _faceEntryEnfVertexEntryListMap.clear();
  _enfVertexEntryEnfVertexMap.clear();
  _groupNameNodeIDMap.clear();

  /* TODO GROUPS
   _groupNameEnfVertexListMap.clear();
   _enfVertexGroupNameMap.clear();
   */
}

TopoDS_Shape BLSURFPlugin_Hypothesis::entryToShape(std::string entry)
{
  MESSAGE("BLSURFPlugin_Hypothesis::entryToShape "<<entry );
  GEOM::GEOM_Object_var aGeomObj;
  SMESH_Gen_i* smeshGen_i = SMESH_Gen_i::GetSMESHGen();
  SALOMEDS::Study_ptr myStudy = smeshGen_i->GetCurrentStudy();
  
  TopoDS_Shape S = TopoDS_Shape();
  SALOMEDS::SObject_var aSObj = myStudy->FindObjectID( entry.c_str() );
  if (!aSObj->_is_nil() ) {
    CORBA::Object_var obj = aSObj->GetObject();
    aGeomObj = GEOM::GEOM_Object::_narrow(obj);
    aSObj->UnRegister();
  }
  if ( !aGeomObj->_is_nil() )
    S = smeshGen_i->GeomObjectToShape( aGeomObj.in() );
  return S;
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetPhysicalMesh(PhysicalMesh thePhysicalMesh) {
  if (thePhysicalMesh != _physicalMesh) {
    _physicalMesh = thePhysicalMesh;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetGeometricMesh(GeometricMesh theGeometricMesh) {
  if (theGeometricMesh != _geometricMesh) {
    _geometricMesh = theGeometricMesh;
//     switch (_geometricMesh) {
//       case DefaultGeom:
//       default:
//         _angleMesh = GetDefaultAngleMesh();
//         _gradation = GetDefaultGradation();
//         break;
//     }
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetPhySize(double theVal, bool isRelative) {
  if ((theVal != _phySize) || (isRelative != _phySizeRel)) {
    _phySizeRel = isRelative;
    if (theVal == 0) {
      _phySize = GetMaxSize();
      MESSAGE("Warning: nul physical size is not allowed");
    }
    else
      _phySize = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetMinSize(double theMinSize, bool isRelative) {
  if ((theMinSize != _minSize) || (isRelative != _minSizeRel)) {
    _minSizeRel = isRelative;
    _minSize = theMinSize;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetMaxSize(double theMaxSize, bool isRelative) {
  if ((theMaxSize != _maxSize) || (isRelative != _maxSizeRel)) {
    _maxSizeRel = isRelative;
    _maxSize = theMaxSize;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetGradation(double theVal) {
  if (theVal != _gradation) {
    _gradation = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetQuadAllowed(bool theVal) {
  if (theVal != _quadAllowed) {
    _quadAllowed = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetAngleMesh(double theVal) {
  if (theVal != _angleMesh) {
    _angleMesh = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetChordalError(double theDistance) {
  if (theDistance != _chordalError) {
    _chordalError = theDistance;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetAnisotropic(bool theVal) {
  if (theVal != _anisotropic) {
    _anisotropic = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetAnisotropicRatio(double theVal) {
  if (theVal != _anisotropicRatio) {
    _anisotropicRatio = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetRemoveTinyEdges(bool theVal) {
  if (theVal != _removeTinyEdges) {
    _removeTinyEdges = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetTinyEdgeLength(double theVal) {
  if (theVal != _tinyEdgeLength) {
    _tinyEdgeLength = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetBadElementRemoval(bool theVal) {
  if (theVal != _badElementRemoval) {
    _badElementRemoval = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetBadElementAspectRatio(double theVal) {
  if (theVal != _badElementAspectRatio) {
    _badElementAspectRatio = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetOptimizeMesh(bool theVal) {
  if (theVal != _optimizeMesh) {
    _optimizeMesh = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetQuadraticMesh(bool theVal) {
  if (theVal != _quadraticMesh) {
    _quadraticMesh = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetTopology(Topology theTopology) {
  if (theTopology != _topology) {
    _topology = theTopology;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetVerbosity(int theVal) {
  if (theVal != _verb) {
    _verb = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetPreCADMergeEdges(bool theVal) {
  if (theVal != _preCADMergeEdges) {
//     SetTopology(PreCAD);
    _preCADMergeEdges = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetPreCADProcess3DTopology(bool theVal) {
  if (theVal != _preCADProcess3DTopology) {
//     SetTopology(PreCAD);
    _preCADProcess3DTopology = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetPreCADDiscardInput(bool theVal) {
  if (theVal != _preCADDiscardInput) {
//     SetTopology(PreCAD);
    _preCADDiscardInput = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
// void BLSURFPlugin_Hypothesis::SetGMFFile(const std::string& theFileName, bool isBinary)
void BLSURFPlugin_Hypothesis::SetGMFFile(const std::string& theFileName)
{
  _GMFFileName = theFileName;
//   _GMFFileMode = isBinary;
  NotifySubMeshesHypothesisModification();
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetOptionValue(const std::string& optionName, const std::string& optionValue)
    throw (std::invalid_argument) {
  TOptionValues::iterator op_val = _option2value.find(optionName);
  if (op_val == _option2value.end()) {
    std::string msg = "Unknown BLSURF option: '" + optionName + "'";
    throw std::invalid_argument(msg);
  }
  if (op_val->second != optionValue) {
    const char* ptr = optionValue.c_str();
    // strip white spaces
    while (ptr[0] == ' ')
      ptr++;
    int i = strlen(ptr);
    while (i != 0 && ptr[i - 1] == ' ')
      i--;
    // check value type
    bool typeOk = true;
    std::string typeName;
    if (i == 0) {
      // empty string
    } else if (_charOptions.find(optionName) != _charOptions.end()) {
      // do not check strings
    } else if (_doubleOptions.find(optionName) != _doubleOptions.end()) {
      // check if value is double
      char * endPtr;
      strtod(ptr, &endPtr);
      typeOk = (ptr != endPtr);
      typeName = "real";
    } else {
      // check if value is int
      char * endPtr;
      strtol(ptr, &endPtr, 10);
      typeOk = (ptr != endPtr);
      typeName = "integer";
    }
    if (!typeOk) {
      std::string msg = "Advanced option '" + optionName + "' = '" + optionValue + "' but must be " + typeName;
      throw std::invalid_argument(msg);
    }
    op_val->second = optionValue;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetPreCADOptionValue(const std::string& optionName, const std::string& optionValue)
    throw (std::invalid_argument) {
  TOptionValues::iterator op_val = _preCADoption2value.find(optionName);
  if (op_val == _preCADoption2value.end()) {
    std::string msg = "Unknown BLSURF option: '" + optionName + "'";
    throw std::invalid_argument(msg);
  }
  if (op_val->second != optionValue) {
    const char* ptr = optionValue.c_str();
    // strip white spaces
    while (ptr[0] == ' ')
      ptr++;
    int i = strlen(ptr);
    while (i != 0 && ptr[i - 1] == ' ')
      i--;
    // check value type
    bool typeOk = true;
    std::string typeName;
    if (i == 0) {
      // empty string
    } else if (_preCADdoubleOptions.find(optionName) != _preCADdoubleOptions.end()) {
      // check if value is double
      char * endPtr;
      strtod(ptr, &endPtr);
      typeOk = (ptr != endPtr);
      typeName = "real";
    } else {
      // check if value is int
      char * endPtr;
      strtol(ptr, &endPtr, 10);
      typeOk = (ptr != endPtr);
      typeName = "integer";
    }
    if (!typeOk) {
      std::string msg = "PreCAD advanced option '" + optionName + "' = '" + optionValue + "' but must be " + typeName;
      throw std::invalid_argument(msg);
    }
    op_val->second = optionValue;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
std::string BLSURFPlugin_Hypothesis::GetOptionValue(const std::string& optionName) throw (std::invalid_argument) {
  TOptionValues::iterator op_val = _option2value.find(optionName);
  if (op_val == _option2value.end()) {
    std::string msg = "Unknown BLSURF option: <";
    msg += optionName + ">";
    throw std::invalid_argument(msg);
  }
  return op_val->second;
}

//=============================================================================
std::string BLSURFPlugin_Hypothesis::GetPreCADOptionValue(const std::string& optionName) throw (std::invalid_argument) {
  TOptionValues::iterator op_val = _preCADoption2value.find(optionName);
  if (op_val == _preCADoption2value.end()) {
    std::string msg = "Unknown PRECAD option: <";
    msg += optionName + ">";
    throw std::invalid_argument(msg);
  }
  return op_val->second;
}

//=============================================================================
void BLSURFPlugin_Hypothesis::ClearOption(const std::string& optionName) {
  TOptionValues::iterator op_val = _option2value.find(optionName);
  if (op_val != _option2value.end())
    op_val->second.clear();
}

//=============================================================================
void BLSURFPlugin_Hypothesis::ClearPreCADOption(const std::string& optionName) {
  TOptionValues::iterator op_val = _preCADoption2value.find(optionName);
  if (op_val != _preCADoption2value.end())
    op_val->second.clear();
}

//=======================================================================
//function : SetSizeMapEntry
//=======================================================================
void BLSURFPlugin_Hypothesis::SetSizeMapEntry(const std::string& entry, const std::string& sizeMap) {
  if (_sizeMap[entry].compare(sizeMap) != 0) {
    SetPhysicalMesh(PhysicalLocalSize);
    _sizeMap[entry] = sizeMap;
    NotifySubMeshesHypothesisModification();
  }
}

//=======================================================================
//function : GetSizeMapEntry
//=======================================================================
std::string BLSURFPlugin_Hypothesis::GetSizeMapEntry(const std::string& entry) {
  TSizeMap::iterator it = _sizeMap.find(entry);
  if (it != _sizeMap.end())
    return it->second;
  else
    return "No_Such_Entry";
}

/*!
 * \brief Return the size maps
 */
BLSURFPlugin_Hypothesis::TSizeMap BLSURFPlugin_Hypothesis::GetSizeMapEntries(const BLSURFPlugin_Hypothesis* hyp) {
  return hyp ? hyp->_GetSizeMapEntries() : GetDefaultSizeMap();
}

//=======================================================================
//function : SetAttractorEntry
//=======================================================================
void BLSURFPlugin_Hypothesis::SetAttractorEntry(const std::string& entry, const std::string& attractor) {
  if (_attractors[entry].compare(attractor) != 0) {
    SetPhysicalMesh(PhysicalLocalSize);
    _attractors[entry] = attractor;
    NotifySubMeshesHypothesisModification();
  }
}

//=======================================================================
//function : GetAttractorEntry
//=======================================================================
std::string BLSURFPlugin_Hypothesis::GetAttractorEntry(const std::string& entry) {
  TSizeMap::iterator it = _attractors.find(entry);
  if (it != _attractors.end())
    return it->second;
  else
    return "No_Such_Entry";
}

/*!
 * \brief Return the attractors
 */
BLSURFPlugin_Hypothesis::TSizeMap BLSURFPlugin_Hypothesis::GetAttractorEntries(const BLSURFPlugin_Hypothesis* hyp) {
  return hyp ? hyp->_GetAttractorEntries() : GetDefaultSizeMap();
}

//=======================================================================
//function : SetClassAttractorEntry
//=======================================================================
void BLSURFPlugin_Hypothesis::SetClassAttractorEntry(const std::string& entry, const std::string& attEntry, double StartSize, double EndSize, double ActionRadius, double ConstantRadius)
{
  SetPhysicalMesh(PhysicalLocalSize);

  // The new attractor can't be defined on the same face as another sizemap
  TSizeMap::iterator it  = _sizeMap.find( entry );
  if ( it != _sizeMap.end() ) {
    _sizeMap.erase(it);
    NotifySubMeshesHypothesisModification();
  }
  else {
    TSizeMap::iterator itAt  = _attractors.find( entry );
    if ( itAt != _attractors.end() ) {
      _attractors.erase(itAt);
      NotifySubMeshesHypothesisModification();
    }
  }
  
  const TopoDS_Shape AttractorShape = BLSURFPlugin_Hypothesis::entryToShape(attEntry);
  const TopoDS_Face FaceShape = TopoDS::Face(BLSURFPlugin_Hypothesis::entryToShape(entry));
  bool attExists = (_classAttractors.find(entry) != _classAttractors.end());
  double u1,u2,v1,v2, diag;
  
  if ( !attExists || (attExists && _classAttractors[entry]->GetAttractorEntry().compare(attEntry) != 0)){ 
    ShapeAnalysis::GetFaceUVBounds(FaceShape,u1,u2,v1,v2);
//     diag = sqrt((u2 - u1) * (u2 - u1) + (v2 - v1) * (v2 - v1));  
    BLSURFPlugin_Attractor* myAttractor = new BLSURFPlugin_Attractor(FaceShape, AttractorShape, attEntry);//, 0.1 ); // test 0.002 * diag); 
    myAttractor->BuildMap();
    myAttractor->SetParameters(StartSize, EndSize, ActionRadius, ConstantRadius);
    _classAttractors[entry] = myAttractor;
    NotifySubMeshesHypothesisModification();
  }
  else {
    _classAttractors[entry]->SetParameters(StartSize, EndSize, ActionRadius, ConstantRadius);
    if (!_classAttractors[entry]->IsMapBuilt()){
      _classAttractors[entry]->BuildMap();
    }
    NotifySubMeshesHypothesisModification();
  }
    
}

//=======================================================================
//function : SetConstantSizeOnAdjacentFaces
//=======================================================================
// TODO uncomment and test (include the needed .hxx)
// SetConstantSizeOnAdjacentFaces(myShape, att_entry, startSize, endSize = user_size, const_dist  ) {
//   TopTools_IndexedMapOfShapListOdShape anEdge2FaceMap;
//   TopExp::MapShapesAnAncestors(myShape,TopAbs_EDGE, TopAbs_FACE, anEdge2FaceMap);
//   TopTools_IndexedMapOfShapListOdShape::iterator it;
//   for (it = anEdge2FaceMap.begin();it != anEdge2FaceMap.end();it++){
//       SetClassAttractorEntry((*it).first, att_entry, startSize, endSize, 0, const_dist)
//   }






//=======================================================================
//function : GetClassAttractorEntry
//=======================================================================
// BLSURFPlugin_Attractor&  BLSURFPlugin_Hypothesis::GetClassAttractorEntry(const std::string& entry)
// {
//  TAttractorMap::iterator it  = _classAttractors.find( entry );
//  if ( it != _classAttractors.end() )
//    return it->second;
//  else
//    return "No_Such_Entry";
// }
// 
  /*!
   * \brief Return the map of attractor instances
   */
BLSURFPlugin_Hypothesis::TAttractorMap BLSURFPlugin_Hypothesis::GetClassAttractorEntries(const BLSURFPlugin_Hypothesis* hyp)
{
    return hyp ? hyp->_GetClassAttractorEntries():GetDefaultAttractorMap();
}

//=======================================================================
//function : ClearEntry
//=======================================================================
void BLSURFPlugin_Hypothesis::ClearEntry(const std::string& entry)
{
 TSizeMap::iterator it  = _sizeMap.find( entry );
 
 if ( it != _sizeMap.end() ) {
   _sizeMap.erase(it);
   NotifySubMeshesHypothesisModification();
 }
 else {
   TSizeMap::iterator itAt  = _attractors.find( entry );
   if ( itAt != _attractors.end() ) {
     _attractors.erase(itAt);
     NotifySubMeshesHypothesisModification();
   }
   else {
     TAttractorMap::iterator it_clAt = _classAttractors.find( entry );
     if ( it_clAt != _classAttractors.end() ) {
       _classAttractors.erase(it_clAt);
       MESSAGE("_classAttractors.size() = "<<_classAttractors.size())
       NotifySubMeshesHypothesisModification();
     }
     else
       std::cout<<"No_Such_Entry"<<std::endl;
   }
 }
}

//=======================================================================
//function : ClearSizeMaps
//=======================================================================
void BLSURFPlugin_Hypothesis::ClearSizeMaps() {
  _sizeMap.clear();
  _attractors.clear();
  _classAttractors.clear();
}

// Enable internal enforced vertices on specific face if requested by user

////=======================================================================
////function : SetInternalEnforcedVertex
////=======================================================================
//void BLSURFPlugin_Hypothesis::SetInternalEnforcedVertex(TEntry theFaceEntry,
//                                                        bool toEnforceInternalVertices,
//                                                        TEnfGroupName theGroupName) {

//  MESSAGE("BLSURFPlugin_Hypothesis::SetInternalEnforcedVertex("<< theFaceEntry << ", "
//      << toEnforceInternalVertices << ", " << theGroupName << ")");
  
//  TFaceEntryInternalVerticesList::iterator it = _faceEntryInternalVerticesList.find(theFaceEntry);
//  if (it != _faceEntryInternalVerticesList.end()) {
//    if (!toEnforceInternalVertices) {
//      _faceEntryInternalVerticesList.erase(it);
//    }
//  }
//  else {
//    if (toEnforceInternalVertices) {
//      _faceEntryInternalVerticesList.insert(theFaceEntry);
//    }
//  }
  
//  // TODO
//  // Take care of groups
//}


//=======================================================================
//function : SetEnforcedVertex
//=======================================================================
bool BLSURFPlugin_Hypothesis::SetEnforcedVertex(TEntry theFaceEntry, TEnfName theVertexName, TEntry theVertexEntry,
                                                TEnfGroupName theGroupName, double x, double y, double z) {

  MESSAGE("BLSURFPlugin_Hypothesis::SetEnforcedVertex("<< theFaceEntry << ", "
      << x << ", " << y << ", " << z << ", " << theVertexName << ", " << theVertexEntry << ", " << theGroupName << ")");

  SetPhysicalMesh(PhysicalLocalSize);

  //  TEnfVertexList::iterator it;
  bool toNotify = false;
  bool toCreate = true;

  TEnfVertex *oldEnVertex;
  TEnfVertex *newEnfVertex = new TEnfVertex();
  newEnfVertex->name = theVertexName;
  newEnfVertex->geomEntry = theVertexEntry;
  newEnfVertex->coords.clear();
  if (theVertexEntry == "") {
    newEnfVertex->coords.push_back(x);
    newEnfVertex->coords.push_back(y);
    newEnfVertex->coords.push_back(z);
  }
  newEnfVertex->grpName = theGroupName;
  newEnfVertex->faceEntries.clear();
  newEnfVertex->faceEntries.insert(theFaceEntry);
  
  
  // update _enfVertexList
  TEnfVertexList::iterator it = _enfVertexList.find(newEnfVertex);
  if (it != _enfVertexList.end()) {
    toCreate = false;
    oldEnVertex = (*it);
    MESSAGE("Enforced Vertex was found => Update");
    if (oldEnVertex->name != theVertexName) {
      MESSAGE("Update name from \"" << oldEnVertex->name << "\" to \"" << theVertexName << "\"");
      oldEnVertex->name = theVertexName;
      toNotify = true;
    }
    if (oldEnVertex->grpName != theGroupName) {
      MESSAGE("Update group name from \"" << oldEnVertex->grpName << "\" to \"" << theGroupName << "\"");
      oldEnVertex->grpName = theGroupName;
      toNotify = true;
    }
    TEntryList::iterator it_faceEntries = oldEnVertex->faceEntries.find(theFaceEntry);
    if (it_faceEntries == oldEnVertex->faceEntries.end()) {
      MESSAGE("Update face list by adding \"" << theFaceEntry << "\"");
      oldEnVertex->faceEntries.insert(theFaceEntry);
      _faceEntryEnfVertexListMap[theFaceEntry].insert(oldEnVertex);
      toNotify = true;
    }
    if (toNotify) {
      // update map coords / enf vertex if needed
      if (oldEnVertex->coords.size()) {
        _coordsEnfVertexMap[oldEnVertex->coords] = oldEnVertex;
        _faceEntryCoordsListMap[theFaceEntry].insert(oldEnVertex->coords);
      }

      // update map geom entry / enf vertex if needed
      if (oldEnVertex->geomEntry != "") {
        _enfVertexEntryEnfVertexMap[oldEnVertex->geomEntry] = oldEnVertex;
        _faceEntryEnfVertexEntryListMap[theFaceEntry].insert(oldEnVertex->geomEntry);
      }
    }
  }

//   //////// CREATE ////////////
  if (toCreate) {
    toNotify = true;
    MESSAGE("Creating new enforced vertex");
    _faceEntryEnfVertexListMap[theFaceEntry].insert(newEnfVertex);
    _enfVertexList.insert(newEnfVertex);
    if (theVertexEntry == "") {
      _faceEntryCoordsListMap[theFaceEntry].insert(newEnfVertex->coords);
      _coordsEnfVertexMap[newEnfVertex->coords] = newEnfVertex;
    }
    else {
      _faceEntryEnfVertexEntryListMap[theFaceEntry].insert(newEnfVertex->geomEntry);
      _enfVertexEntryEnfVertexMap[newEnfVertex->geomEntry] = newEnfVertex;
    }
  }

  if (toNotify)
    NotifySubMeshesHypothesisModification();

  MESSAGE("BLSURFPlugin_Hypothesis::SetEnforcedVertex END");
  return toNotify;
}


//=======================================================================
//function : GetEnforcedVertices
//=======================================================================

BLSURFPlugin_Hypothesis::TEnfVertexList BLSURFPlugin_Hypothesis::GetEnfVertexList(const TEntry& theFaceEntry)
    throw (std::invalid_argument) {

  if (_faceEntryEnfVertexListMap.count(theFaceEntry) > 0)
    return _faceEntryEnfVertexListMap[theFaceEntry];
  else
    return GetDefaultEnfVertexList();

  std::ostringstream msg;
  msg << "No enforced vertex for face entry " << theFaceEntry;
  throw std::invalid_argument(msg.str());
}

//=======================================================================
//function : GetEnfVertexCoordsList
//=======================================================================

BLSURFPlugin_Hypothesis::TEnfVertexCoordsList BLSURFPlugin_Hypothesis::GetEnfVertexCoordsList(
    const TEntry& theFaceEntry) throw (std::invalid_argument) {

  if (_faceEntryCoordsListMap.count(theFaceEntry) > 0)
    return _faceEntryCoordsListMap[theFaceEntry];

  std::ostringstream msg;
  msg << "No enforced vertex coords for face entry " << theFaceEntry;
  throw std::invalid_argument(msg.str());
}

//=======================================================================
//function : GetEnfVertexEntryList
//=======================================================================

BLSURFPlugin_Hypothesis::TEntryList BLSURFPlugin_Hypothesis::GetEnfVertexEntryList(const TEntry& theFaceEntry)
    throw (std::invalid_argument) {

  if (_faceEntryEnfVertexEntryListMap.count(theFaceEntry) > 0)
    return _faceEntryEnfVertexEntryListMap[theFaceEntry];

  std::ostringstream msg;
  msg << "No enforced vertex entry for face entry " << theFaceEntry;
  throw std::invalid_argument(msg.str());
}

//=======================================================================
//function : GetEnfVertex(TEnfVertexCoords coords)
//=======================================================================

BLSURFPlugin_Hypothesis::TEnfVertex* BLSURFPlugin_Hypothesis::GetEnfVertex(TEnfVertexCoords coords)
    throw (std::invalid_argument) {

  if (_coordsEnfVertexMap.count(coords) > 0)
    return _coordsEnfVertexMap[coords];

  std::ostringstream msg;
  msg << "No enforced vertex with coords (" << coords[0] << ", " << coords[1] << ", " << coords[2] << ")";
  throw std::invalid_argument(msg.str());
}

//=======================================================================
//function : GetEnfVertex(const TEntry& theEnfVertexEntry)
//=======================================================================

BLSURFPlugin_Hypothesis::TEnfVertex* BLSURFPlugin_Hypothesis::GetEnfVertex(const TEntry& theEnfVertexEntry)
    throw (std::invalid_argument) {

  if (_enfVertexEntryEnfVertexMap.count(theEnfVertexEntry) > 0)
    return _enfVertexEntryEnfVertexMap[theEnfVertexEntry];

  std::ostringstream msg;
  msg << "No enforced vertex with entry " << theEnfVertexEntry;
  throw std::invalid_argument(msg.str());
}

//Enable internal enforced vertices on specific face if requested by user
////=======================================================================
////function : GetInternalEnforcedVertex
////=======================================================================

//bool BLSURFPlugin_Hypothesis::GetInternalEnforcedVertex(const TEntry& theFaceEntry)
//{
//  if (_faceEntryInternalVerticesList.count(theFaceEntry) > 0)
//    return true;
//  return false;
//}

//=======================================================================
//function : ClearEnforcedVertex
//=======================================================================

bool BLSURFPlugin_Hypothesis::ClearEnforcedVertex(const TEntry& theFaceEntry, double x, double y, double z,
    const TEntry& theVertexEntry) throw (std::invalid_argument) {

  bool toNotify = false;
  std::ostringstream msg;
  TEnfVertex *oldEnfVertex;
  TEnfVertexCoords coords;
  coords.clear();
  coords.push_back(x);
  coords.push_back(y);
  coords.push_back(z);

  // check that enf vertex with given enf vertex entry exists
  TEnfVertexEntryEnfVertexMap::iterator it_enfVertexEntry = _enfVertexEntryEnfVertexMap.find(theVertexEntry);
  if (it_enfVertexEntry != _enfVertexEntryEnfVertexMap.end()) {
    // Success
    MESSAGE("Found enforced vertex with geom entry " << theVertexEntry);
    oldEnfVertex = it_enfVertexEntry->second;

    _enfVertexEntryEnfVertexMap.erase(it_enfVertexEntry);

    TEntryList& enfVertexEntryList = _faceEntryEnfVertexEntryListMap[theFaceEntry];
    enfVertexEntryList.erase(theVertexEntry);
    if (enfVertexEntryList.size() == 0)
      _faceEntryEnfVertexEntryListMap.erase(theFaceEntry);
    //    TFaceEntryEnfVertexEntryListMap::iterator it_entry_entry = _faceEntryEnfVertexEntryListMap.find(theFaceEntry);
    //    TEntryList::iterator it_entryList = it_entry_entry->second.find(theVertexEntry);
    //    it_entry_entry->second.erase(it_entryList);
    //    if (it_entry_entry->second.size() == 0)
    //      _faceEntryEnfVertexEntryListMap.erase(it_entry_entry);
  } else {
    // Fail
    MESSAGE("Enforced vertex with geom entry " << theVertexEntry << " not found");
    msg << "No enforced vertex with geom entry " << theVertexEntry;
    // check that enf vertex with given coords exists
    TCoordsEnfVertexMap::iterator it_coords_enf = _coordsEnfVertexMap.find(coords);
    if (it_coords_enf != _coordsEnfVertexMap.end()) {
      // Success
      MESSAGE("Found enforced vertex with coords " << x << ", " << y << ", " << z);
      oldEnfVertex = it_coords_enf->second;

      _coordsEnfVertexMap.erase(it_coords_enf);

      TEnfVertexCoordsList& enfVertexCoordsList = _faceEntryCoordsListMap[theFaceEntry];
      enfVertexCoordsList.erase(coords);
      if (enfVertexCoordsList.size() == 0)
        _faceEntryCoordsListMap.erase(theFaceEntry);
      //      TFaceEntryCoordsListMap::iterator it_entry_coords = _faceEntryCoordsListMap.find(theFaceEntry);
      //      TEnfVertexCoordsList::iterator it_coordsList = it_entry_coords->second.find(coords);
      //      it_entry_coords->second.erase(it_coordsList);
      //      if (it_entry_coords->second.size() == 0)
      //        _faceEntryCoordsListMap.erase(it_entry_coords);
    } else {
      // Fail
      MESSAGE("Enforced vertex with coords " << x << ", " << y << ", " << z << " not found");
      msg << std::endl;
      msg << "No enforced vertex at " << x << ", " << y << ", " << z;
      throw std::invalid_argument(msg.str());
    }
  }

  MESSAGE("Remove enf vertex from _enfVertexList");

  // update _enfVertexList
  TEnfVertexList::iterator it = _enfVertexList.find(oldEnfVertex);
  if (it != _enfVertexList.end()) {
    (*it)->faceEntries.erase(theFaceEntry);
    if ((*it)->faceEntries.size() == 0){
      _enfVertexList.erase(it);
      toNotify = true;
    }
    MESSAGE("Done");
  }

  // update _faceEntryEnfVertexListMap
  TEnfVertexList& currentEnfVertexList = _faceEntryEnfVertexListMap[theFaceEntry];
  currentEnfVertexList.erase(oldEnfVertex);

  if (currentEnfVertexList.size() == 0) {
    MESSAGE("Remove _faceEntryEnfVertexListMap[" << theFaceEntry <<"]");
    _faceEntryEnfVertexListMap.erase(theFaceEntry);
    MESSAGE("Done");
  }

  if (toNotify)
    NotifySubMeshesHypothesisModification();

  return toNotify;
}

//=======================================================================
//function : ClearEnforcedVertices
//=======================================================================

bool BLSURFPlugin_Hypothesis::ClearEnforcedVertices(const TEntry& theFaceEntry) throw (std::invalid_argument) {

  bool toNotify = false;
  TEnfVertex *oldEnfVertex;

  TFaceEntryCoordsListMap::iterator it_entry_coords = _faceEntryCoordsListMap.find(theFaceEntry);
  if (it_entry_coords != _faceEntryCoordsListMap.end()) {
    toNotify = true;
    TEnfVertexCoordsList coordsList = it_entry_coords->second;
    TEnfVertexCoordsList::iterator it_coordsList = coordsList.begin();
    for (; it_coordsList != coordsList.end(); ++it_coordsList) {
      TEnfVertexCoords coords = (*it_coordsList);
      oldEnfVertex = _coordsEnfVertexMap[coords];
      _coordsEnfVertexMap.erase(coords);
      // update _enfVertexList
      TEnfVertexList::iterator it = _enfVertexList.find(oldEnfVertex);
      if (it != _enfVertexList.end()) {
        (*it)->faceEntries.erase(theFaceEntry);
        if ((*it)->faceEntries.size() == 0){
          _enfVertexList.erase(it);
          toNotify = true;
        }
        MESSAGE("Done");
      }
    }
    _faceEntryCoordsListMap.erase(it_entry_coords);
    _faceEntryEnfVertexListMap.erase(theFaceEntry);
  }

  TFaceEntryEnfVertexEntryListMap::iterator it_entry_entry = _faceEntryEnfVertexEntryListMap.find(theFaceEntry);
  if (it_entry_entry != _faceEntryEnfVertexEntryListMap.end()) {
    toNotify = true;
    TEntryList enfVertexEntryList = it_entry_entry->second;
    TEntryList::iterator it_enfVertexEntryList = enfVertexEntryList.begin();
    for (; it_enfVertexEntryList != enfVertexEntryList.end(); ++it_enfVertexEntryList) {
      TEntry enfVertexEntry = (*it_enfVertexEntryList);
      oldEnfVertex = _enfVertexEntryEnfVertexMap[enfVertexEntry];
      _enfVertexEntryEnfVertexMap.erase(enfVertexEntry);
      // update _enfVertexList
      TEnfVertexList::iterator it = _enfVertexList.find(oldEnfVertex);
      if (it != _enfVertexList.end()) {
        (*it)->faceEntries.erase(theFaceEntry);
        if ((*it)->faceEntries.size() == 0){
          _enfVertexList.erase(it);
          toNotify = true;
        }
        MESSAGE("Done");
      }
    }
    _faceEntryEnfVertexEntryListMap.erase(it_entry_entry);
    _faceEntryEnfVertexListMap.erase(theFaceEntry);
  }

  if (toNotify)
    NotifySubMeshesHypothesisModification();

  return toNotify;
  //  std::ostringstream msg;
  //  msg << "No enforced vertex for " << theFaceEntry;
  //  throw std::invalid_argument(msg.str());
}

//=======================================================================
//function : ClearAllEnforcedVertices
//=======================================================================
void BLSURFPlugin_Hypothesis::ClearAllEnforcedVertices() {
  _faceEntryEnfVertexListMap.clear();
  _enfVertexList.clear();
  _faceEntryCoordsListMap.clear();
  _coordsEnfVertexMap.clear();
  _faceEntryEnfVertexEntryListMap.clear();
  _enfVertexEntryEnfVertexMap.clear();
//  Enable internal enforced vertices on specific face if requested by user
//  _faceEntryInternalVerticesList.clear();
  NotifySubMeshesHypothesisModification();
}

//================================================================================
/*!
 * \brief Return the enforced vertices
 */
//================================================================================


BLSURFPlugin_Hypothesis::TFaceEntryEnfVertexListMap BLSURFPlugin_Hypothesis::GetAllEnforcedVerticesByFace(
    const BLSURFPlugin_Hypothesis* hyp) {
  return hyp ? hyp->_GetAllEnforcedVerticesByFace() : GetDefaultFaceEntryEnfVertexListMap();
}

//Enable internal enforced vertices on specific face if requested by user
//BLSURFPlugin_Hypothesis::TFaceEntryInternalVerticesList BLSURFPlugin_Hypothesis::GetAllInternalEnforcedVerticesByFace(
//    const BLSURFPlugin_Hypothesis* hyp) {
//  return hyp ? hyp->_GetAllInternalEnforcedVerticesByFace() : GetDefaultFaceEntryInternalVerticesMap();
//}

bool BLSURFPlugin_Hypothesis::GetInternalEnforcedVertexAllFaces(const BLSURFPlugin_Hypothesis* hyp)
{
  return hyp ? hyp->_GetInternalEnforcedVertexAllFaces() : GetDefaultInternalEnforcedVertex();
}

BLSURFPlugin_Hypothesis::TEnfGroupName BLSURFPlugin_Hypothesis::GetInternalEnforcedVertexAllFacesGroup(const BLSURFPlugin_Hypothesis* hyp)
{
  return hyp ? hyp->_GetInternalEnforcedVertexAllFacesGroup() : BLSURFPlugin_Hypothesis::TEnfGroupName();
}

BLSURFPlugin_Hypothesis::TEnfVertexList BLSURFPlugin_Hypothesis::GetAllEnforcedVertices(
    const BLSURFPlugin_Hypothesis* hyp) {
  return hyp ? hyp->_GetAllEnforcedVertices() : GetDefaultEnfVertexList();
}

BLSURFPlugin_Hypothesis::TFaceEntryCoordsListMap BLSURFPlugin_Hypothesis::GetAllCoordsByFace(
    const BLSURFPlugin_Hypothesis* hyp) {
  return hyp ? hyp->_GetAllCoordsByFace() : GetDefaultFaceEntryCoordsListMap();
}

BLSURFPlugin_Hypothesis::TCoordsEnfVertexMap BLSURFPlugin_Hypothesis::GetAllEnforcedVerticesByCoords(
    const BLSURFPlugin_Hypothesis* hyp) {
  return hyp ? hyp->_GetAllEnforcedVerticesByCoords() : GetDefaultCoordsEnfVertexMap();
}

BLSURFPlugin_Hypothesis::TFaceEntryEnfVertexEntryListMap BLSURFPlugin_Hypothesis::GetAllEnfVertexEntriesByFace(
    const BLSURFPlugin_Hypothesis* hyp) {
  return hyp ? hyp->_GetAllEnfVertexEntriesByFace() : GetDefaultFaceEntryEnfVertexEntryListMap();
}

BLSURFPlugin_Hypothesis::TEnfVertexEntryEnfVertexMap BLSURFPlugin_Hypothesis::GetAllEnforcedVerticesByEnfVertexEntry(
    const BLSURFPlugin_Hypothesis* hyp) {
  return hyp ? hyp->_GetAllEnforcedVerticesByEnfVertexEntry() : GetDefaultEnfVertexEntryEnfVertexMap();
}

std::set<int> BLSURFPlugin_Hypothesis::GetEnfVertexNodeIDs(TEnfGroupName theGroupName) throw (std::invalid_argument)
{
  TGroupNameNodeIDMap::const_iterator it = _groupNameNodeIDMap.find(theGroupName);
  if (it != _groupNameNodeIDMap.end()) {
    return it->second;
  }
  std::ostringstream msg;
  msg << "No group " << theGroupName;
  throw std::invalid_argument(msg.str());
}

void BLSURFPlugin_Hypothesis::AddEnfVertexNodeID(TEnfGroupName theGroupName,int theNodeID)
{
  _groupNameNodeIDMap[theGroupName].insert(theNodeID);
}

void BLSURFPlugin_Hypothesis::RemoveEnfVertexNodeID(TEnfGroupName theGroupName,int theNodeID) throw (std::invalid_argument)
{
  TGroupNameNodeIDMap::iterator it = _groupNameNodeIDMap.find(theGroupName);
  if (it != _groupNameNodeIDMap.end()) {
    std::set<int>::iterator IDit = it->second.find(theNodeID);
    if (IDit != it->second.end())
      it->second.erase(IDit);
    std::ostringstream msg;
    msg << "No node IDs " << theNodeID << " for group " << theGroupName;
    throw std::invalid_argument(msg.str());
  }
  std::ostringstream msg;
  msg << "No group " << theGroupName;
  throw std::invalid_argument(msg.str());
}


//=============================================================================
void BLSURFPlugin_Hypothesis::SetInternalEnforcedVertexAllFaces(bool toEnforceInternalVertices) {
  if (toEnforceInternalVertices != _enforcedInternalVerticesAllFaces) {
    _enforcedInternalVerticesAllFaces = toEnforceInternalVertices;
    if (toEnforceInternalVertices)
      SetPhysicalMesh(PhysicalLocalSize);
    NotifySubMeshesHypothesisModification();
  }
}


//=============================================================================
void BLSURFPlugin_Hypothesis::SetInternalEnforcedVertexAllFacesGroup(BLSURFPlugin_Hypothesis::TEnfGroupName theGroupName) {
  if (string(theGroupName) != string(_enforcedInternalVerticesAllFacesGroup)) {
    _enforcedInternalVerticesAllFacesGroup = theGroupName;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
BLSURFPlugin_Hypothesis::TPreCadPeriodicityVector BLSURFPlugin_Hypothesis::GetPreCadFacesPeriodicityVector(
    const BLSURFPlugin_Hypothesis* hyp) {
  return hyp ? hyp->_GetPreCadFacesPeriodicityVector() : GetDefaultPreCadFacesPeriodicityVector();
}

//=============================================================================
BLSURFPlugin_Hypothesis::TPreCadPeriodicityVector BLSURFPlugin_Hypothesis::GetPreCadEdgesPeriodicityVector(
    const BLSURFPlugin_Hypothesis* hyp) {
  return hyp ? hyp->_GetPreCadEdgesPeriodicityVector() : GetDefaultPreCadEdgesPeriodicityVector();
}

//=============================================================================
BLSURFPlugin_Hypothesis::TFacesPeriodicityVector BLSURFPlugin_Hypothesis::GetFacesPeriodicityVector(
    const BLSURFPlugin_Hypothesis* hyp) {
  return hyp ? hyp->_GetFacesPeriodicityVector() : GetDefaultFacesPeriodicityVector();
}

//=============================================================================
BLSURFPlugin_Hypothesis::TEdgesPeriodicityVector BLSURFPlugin_Hypothesis::GetEdgesPeriodicityVector(
    const BLSURFPlugin_Hypothesis* hyp){
  return hyp ? hyp->_GetEdgesPeriodicityVector() : GetDefaultEdgesPeriodicityVector();
}

//=============================================================================
BLSURFPlugin_Hypothesis::TVerticesPeriodicityVector BLSURFPlugin_Hypothesis::GetVerticesPeriodicityVector(
    const BLSURFPlugin_Hypothesis* hyp){
  return hyp ? hyp->_GetVerticesPeriodicityVector() : GetDefaultVerticesPeriodicityVector();
}

//=======================================================================
//function : ClearAllEnforcedVertices
//=======================================================================
void BLSURFPlugin_Hypothesis::ClearPreCadPeriodicityVectors() {
  _preCadFacesPeriodicityVector.clear();
  _preCadEdgesPeriodicityVector.clear();
  NotifySubMeshesHypothesisModification();
}

//=======================================================================
//function : AddPreCadFacesPeriodicity
//=======================================================================
void BLSURFPlugin_Hypothesis::AddPreCadFacesPeriodicity(TEntry theFace1Entry, TEntry theFace2Entry,
    vector<string> &theSourceVerticesEntries, vector<string> &theTargetVerticesEntries) {

  TPreCadPeriodicity preCadFacesPeriodicity;
  preCadFacesPeriodicity.shape1Entry = theFace1Entry;
  preCadFacesPeriodicity.shape2Entry = theFace2Entry;
  preCadFacesPeriodicity.theSourceVerticesEntries = theSourceVerticesEntries;
  preCadFacesPeriodicity.theTargetVerticesEntries = theTargetVerticesEntries;

  _preCadFacesPeriodicityVector.push_back(preCadFacesPeriodicity);

  NotifySubMeshesHypothesisModification();
}

//=======================================================================
//function : AddPreCadEdgesPeriodicity
//=======================================================================
void BLSURFPlugin_Hypothesis::AddPreCadEdgesPeriodicity(TEntry theEdge1Entry, TEntry theEdge2Entry,
    vector<string> &theSourceVerticesEntries, vector<string> &theTargetVerticesEntries) {

  TPreCadPeriodicity preCadEdgesPeriodicity;
  preCadEdgesPeriodicity.shape1Entry = theEdge1Entry;
  preCadEdgesPeriodicity.shape2Entry = theEdge2Entry;
  preCadEdgesPeriodicity.theSourceVerticesEntries = theSourceVerticesEntries;
  preCadEdgesPeriodicity.theTargetVerticesEntries = theTargetVerticesEntries;

  _preCadEdgesPeriodicityVector.push_back(preCadEdgesPeriodicity);

  NotifySubMeshesHypothesisModification();
}

//=======================================================================
//function : AddFacePeriodicity
//=======================================================================
void BLSURFPlugin_Hypothesis::AddFacePeriodicity(TEntry theFace1Entry, TEntry theFace2Entry) {

  std::pair< TEntry, TEntry > pairOfFacesEntries = std::make_pair(theFace1Entry, theFace2Entry);

  _facesPeriodicityVector.push_back(pairOfFacesEntries);

  NotifySubMeshesHypothesisModification();
}


//=======================================================================
//function : AddEdgePeriodicity
//=======================================================================
void BLSURFPlugin_Hypothesis::AddEdgePeriodicity(TEntry theFace1Entry, TEntry theEdge1Entry, TEntry theFace2Entry, TEntry theEdge2Entry, int edge_orientation) {

  TEdgePeriodicity edgePeriodicity;
  edgePeriodicity.theFace1Entry = theFace1Entry;
  edgePeriodicity.theEdge1Entry = theEdge1Entry;
  edgePeriodicity.theFace2Entry = theFace2Entry;
  edgePeriodicity.theEdge2Entry = theEdge2Entry;
  edgePeriodicity.edge_orientation = edge_orientation;

  _edgesPeriodicityVector.push_back(edgePeriodicity);

  NotifySubMeshesHypothesisModification();
}

//=======================================================================
//function : AddVertexPeriodicity
//=======================================================================
void BLSURFPlugin_Hypothesis::AddVertexPeriodicity(TEntry theEdge1Entry, TEntry theVertex1Entry, TEntry theEdge2Entry, TEntry theVertex2Entry) {

  TVertexPeriodicity vertexPeriodicity;
  vertexPeriodicity.theEdge1Entry = theEdge1Entry;
  vertexPeriodicity.theVertex1Entry = theVertex1Entry;
  vertexPeriodicity.theEdge2Entry = theEdge2Entry;
  vertexPeriodicity.theVertex2Entry = theVertex2Entry;

  _verticesPeriodicityVector.push_back(vertexPeriodicity);

  NotifySubMeshesHypothesisModification();
}

//=============================================================================
std::ostream & BLSURFPlugin_Hypothesis::SaveTo(std::ostream & save) {
   // We must keep at least the same number of arguments when increasing the SALOME version
   // When BLSURF becomes CADMESH, some parameters were fused into a single one. Thus the same
   // parameter can be written several times to keep the old global number of parameters.

   // Treat old options which are now in the advanced options
   TOptionValues::iterator op_val;
   int _decimesh = -1;
   int _preCADRemoveNanoEdges = -1;
   double _preCADEpsNano = -1.0;
   op_val = _option2value.find("respect_geometry");
   if (op_val != _option2value.end()) {
     std::string value = op_val->second;
     if (!value.empty())
       _decimesh = value.compare("1") == 0 ? 1 : 0;
   }
   op_val = _preCADoption2value.find("remove_tiny_edges");
   if (op_val != _preCADoption2value.end()) {
     std::string value = op_val->second;
     if (!value.empty())
       _preCADRemoveNanoEdges = value.compare("1") == 0 ? 1 : 0;
   }
   op_val = _preCADoption2value.find("tiny_edge_length");
   if (op_val != _preCADoption2value.end()) {
     std::string value = op_val->second;
     if (!value.empty())
       _preCADEpsNano = strtod(value.c_str(), NULL);
   }
   
  save << " " << (int) _topology << " " << (int) _physicalMesh << " " << (int) _geometricMesh << " " << _phySize << " "
      << _angleMesh << " " << _gradation << " " << (int) _quadAllowed << " " << _decimesh;
  save << " " << _minSize << " " << _maxSize << " " << _angleMesh << " " << _minSize << " " << _maxSize << " " << _verb;
  save << " " << (int) _preCADMergeEdges << " " << _preCADRemoveNanoEdges << " " << (int) _preCADDiscardInput << " " << _preCADEpsNano ;
  save << " " << (int) _enforcedInternalVerticesAllFaces;
  save << " " << (int) _phySizeRel << " " << (int) _minSizeRel << " " << (int) _maxSizeRel << " " << _chordalError ;
  save << " " << (int) _anisotropic << " " << _anisotropicRatio << " " << (int) _removeTinyEdges << " " << _tinyEdgeLength ;
  save << " " << (int) _badElementRemoval << " " << _badElementAspectRatio << " " << (int) _optimizeMesh << " " << (int) _quadraticMesh ;
  save << " " << (int) _preCADProcess3DTopology;

  op_val = _option2value.begin();
  if (op_val != _option2value.end()) {
    save << " " << "__OPTIONS_BEGIN__";
    for (; op_val != _option2value.end(); ++op_val) {
      if (!op_val->second.empty())
        save << " " << op_val->first << " " << op_val->second << "%#"; // "%#" is a mark of value end
    }
    save << " " << "__OPTIONS_END__";
  }
  
  op_val = _preCADoption2value.begin();
  if (op_val != _preCADoption2value.end()) {
    save << " " << "__PRECAD_OPTIONS_BEGIN__";
    for (; op_val != _preCADoption2value.end(); ++op_val) {
      if (!op_val->second.empty())
        save << " " << op_val->first << " " << op_val->second << "%#"; // "%#" is a mark of value end
    }
    save << " " << "__PRECAD_OPTIONS_END__";
  }

  TSizeMap::iterator it_sm = _sizeMap.begin();
  if (it_sm != _sizeMap.end()) {
    save << " " << "__SIZEMAP_BEGIN__";
    for (; it_sm != _sizeMap.end(); ++it_sm) {
      save << " " << it_sm->first << " " << it_sm->second << "%#"; // "%#" is a mark of value end
    }
    save << " " << "__SIZEMAP_END__";
  }

  TSizeMap::iterator it_at = _attractors.begin();
  if (it_at != _attractors.end()) {
    save << " " << "__ATTRACTORS_BEGIN__";
    for (; it_at != _attractors.end(); ++it_at) {
      save << " " << it_at->first << " " << it_at->second << "%#"; // "%#" is a mark of value end
    }
    save << " " << "__ATTRACTORS_END__";
  }
  
  TAttractorMap::iterator it_At = _classAttractors.begin();
  if (it_At != _classAttractors.end()) {
    std::ostringstream test;
    save << " " << "__NEW_ATTRACTORS_BEGIN__";
    test << " " << "__NEW_ATTRACTORS_BEGIN__";
    for (; it_At != _classAttractors.end(); ++it_At) {
      std::vector<double> attParams;
      attParams   = it_At->second->GetParameters();
//       double step = it_At->second->GetStep();
      save << " " << it_At->first;
      save << " " << it_At->second->GetAttractorEntry();
      save << " " << attParams[0]  << " " <<  attParams[1] << " " <<  attParams[2] << " " <<  attParams[3];
//       save << " " << step;
      test << " " << it_At->first;
      test << " " << it_At->second->GetAttractorEntry();
      test << " " << attParams[0]  << " " <<  attParams[1] << " " <<  attParams[2] << " " <<  attParams[3];
//       test << " " << step;
    }
    save << " " << "__NEW_ATTRACTORS_END__";
    test << " " << "__NEW_ATTRACTORS_END__";
    MESSAGE(" Attractor hypothesis saved as "<<test.str())
  }

  TEnfVertexList::const_iterator it_enf = _enfVertexList.begin();
  if (it_enf != _enfVertexList.end()) {
    save << " " << "__ENFORCED_VERTICES_BEGIN__";
    for (; it_enf != _enfVertexList.end(); ++it_enf) {
      TEnfVertex *enfVertex = (*it_enf);
      save << " " << "__BEGIN_VERTEX__";
      if (!enfVertex->name.empty()) {
        save << " " << "__BEGIN_NAME__";
        save << " " << enfVertex->name;
        save << " " << "__END_NAME__";
      }
      if (!enfVertex->geomEntry.empty()) {
        save << " " << "__BEGIN_ENTRY__";
        save << " " << enfVertex->geomEntry;
        save << " " << "__END_ENTRY__";
      }
      if (!enfVertex->grpName.empty()) {
        save << " " << "__BEGIN_GROUP__";
        save << " " << enfVertex->grpName;
        save << " " << "__END_GROUP__";
      }
      if (enfVertex->coords.size()) {
        save << " " << "__BEGIN_COORDS__";
        for (int i=0;i<enfVertex->coords.size();i++)
          save << " " << enfVertex->coords[i];
        save << " " << "__END_COORDS__";
      }
      TEntryList::const_iterator faceEntriesIt = enfVertex->faceEntries.begin();
      bool hasFaces = false;
      if (faceEntriesIt != enfVertex->faceEntries.end()) {
        hasFaces = true;
        save << " " << "__BEGIN_FACELIST__";
      }
      for (; faceEntriesIt != enfVertex->faceEntries.end(); ++faceEntriesIt)
        save << " " << (*faceEntriesIt);
      if (hasFaces)
        save << " " << "__END_FACELIST__";
      save << " " << "__END_VERTEX__";
    }
    save << " " << "__ENFORCED_VERTICES_END__";
  }

  //PERIODICITY

  SavePreCADPeriodicity(save, "FACES");
  SavePreCADPeriodicity(save, "EDGES");

  SaveFacesPeriodicity(save);
  SaveEdgesPeriodicity(save);
  SaveVerticesPeriodicity(save);

  return save;
}

void BLSURFPlugin_Hypothesis::SaveFacesPeriodicity(std::ostream & save){

  TFacesPeriodicityVector::const_iterator it_faces_periodicity = _facesPeriodicityVector.begin();
  if (it_faces_periodicity != _facesPeriodicityVector.end()) {
    MESSAGE("__FACES_PERIODICITY_BEGIN__");
    save << " " << "__FACES_PERIODICITY_BEGIN__";
    for (; it_faces_periodicity != _facesPeriodicityVector.end(); ++it_faces_periodicity) {
      TFacesPeriodicity periodicity_i = (*it_faces_periodicity);
      save << " " << "__BEGIN_PERIODICITY_DESCRIPTION__";
      save << " " << "__BEGIN_ENTRY1__";
      save << " " << periodicity_i.first;
      save << " " << "__END_ENTRY1__";
      save << " " << "__BEGIN_ENTRY2__";
      save << " " << periodicity_i.second;
      save << " " << "__END_ENTRY2__";
      save << " " << "__END_PERIODICITY_DESCRIPTION__";
    }
    save << " " << "__FACES_PERIODICITY_END__";
    MESSAGE("__FACES_PERIODICITY_END__");
  }
}

void BLSURFPlugin_Hypothesis::SaveEdgesPeriodicity(std::ostream & save){

  TEdgesPeriodicityVector::const_iterator it_edges_periodicity = _edgesPeriodicityVector.begin();
  if (it_edges_periodicity != _edgesPeriodicityVector.end()) {
    save << " " << "__EDGES_PERIODICITY_BEGIN__";
    MESSAGE("__EDGES_PERIODICITY_BEGIN__");
    for (; it_edges_periodicity != _edgesPeriodicityVector.end(); ++it_edges_periodicity) {
      TEdgePeriodicity periodicity_i = (*it_edges_periodicity);
      save << " " << "__BEGIN_PERIODICITY_DESCRIPTION__";
      if (not periodicity_i.theFace1Entry.empty()){
        save << " " << "__BEGIN_FACE1__";
        save << " " << periodicity_i.theFace1Entry;
        save << " " << "__END_FACE1__";
      }
      save << " " << "__BEGIN_EDGE1__";
      save << " " << periodicity_i.theEdge1Entry;
      save << " " << "__END_EDGE1__";
      if (not periodicity_i.theFace2Entry.empty()){
        save << " " << "__BEGIN_FACE2__";
        save << " " << periodicity_i.theFace2Entry;
        save << " " << "__END_FACE2__";
      }
      save << " " << "__BEGIN_EDGE2__";
      save << " " << periodicity_i.theEdge2Entry;
      save << " " << "__END_EDGE2__";
      save << " " << "__BEGIN_EDGE_ORIENTATION__";
      save << " " << periodicity_i.edge_orientation;
      save << " " << "__END_EDGE_ORIENTATION__";
      save << " " << "__END_PERIODICITY_DESCRIPTION__";
    }
    save << " " << "__EDGES_PERIODICITY_END__";
    MESSAGE("__EDGES_PERIODICITY_END__");
  }
}

void BLSURFPlugin_Hypothesis::SaveVerticesPeriodicity(std::ostream & save){

  TVerticesPeriodicityVector::const_iterator it_vertices_periodicity = _verticesPeriodicityVector.begin();
  if (it_vertices_periodicity != _verticesPeriodicityVector.end()) {
    MESSAGE("__VERTICES_PERIODICITY_BEGIN__");
    save << " " << "__VERTICES_PERIODICITY_BEGIN__";
    for (; it_vertices_periodicity != _verticesPeriodicityVector.end(); ++it_vertices_periodicity) {
      TVertexPeriodicity periodicity_i = (*it_vertices_periodicity);
      save << " " << "__BEGIN_PERIODICITY_DESCRIPTION__";
      save << " " << "__BEGIN_EDGE1__";
      save << " " << periodicity_i.theEdge1Entry;
      save << " " << "__END_EDGE1__";
      save << " " << "__BEGIN_VERTEX1__";
      save << " " << periodicity_i.theVertex1Entry;
      save << " " << "__END_VERTEX1__";
      save << " " << "__BEGIN_EDGE2__";
      save << " " << periodicity_i.theEdge2Entry;
      save << " " << "__END_EDGE2__";
      save << " " << "__BEGIN_VERTEX2__";
      save << " " << periodicity_i.theVertex2Entry;
      save << " " << "__END_VERTEX2__";
      save << " " << "__END_PERIODICITY_DESCRIPTION__";
    }
    save << " " << "__VERTICES_PERIODICITY_END__";
    MESSAGE("__VERTICES_PERIODICITY_END__");
  }
}

void BLSURFPlugin_Hypothesis::SavePreCADPeriodicity(std::ostream & save, const char* shapeType) {
  TPreCadPeriodicityVector precad_periodicity;
  if (shapeType == "FACES")
    precad_periodicity = _preCadFacesPeriodicityVector;
  else
    precad_periodicity = _preCadEdgesPeriodicityVector;
  TPreCadPeriodicityVector::const_iterator it_precad_periodicity = precad_periodicity.begin();
  if (it_precad_periodicity != precad_periodicity.end()) {
    save << " " << "__PRECAD_" << shapeType << "_PERIODICITY_BEGIN__";
    for (; it_precad_periodicity != precad_periodicity.end(); ++it_precad_periodicity) {
      TPreCadPeriodicity periodicity_i = (*it_precad_periodicity);
      save << " " << "__BEGIN_PERIODICITY_DESCRIPTION__";
      if (!periodicity_i.shape1Entry.empty()) {
        save << " " << "__BEGIN_ENTRY1__";
        save << " " << periodicity_i.shape1Entry;
        save << " " << "__END_ENTRY1__";
      }
      if (!periodicity_i.shape2Entry.empty()) {
        save << " " << "__BEGIN_ENTRY2__";
        save << " " << periodicity_i.shape2Entry;
        save << " " << "__END_ENTRY2__";
      }

      std::vector<std::string>::const_iterator sourceVerticesEntriesIt = periodicity_i.theSourceVerticesEntries.begin();
      bool hasSourceVertices = false;
      if (sourceVerticesEntriesIt != periodicity_i.theSourceVerticesEntries.end()) {
        hasSourceVertices = true;
        save << " " << "__BEGIN_SOURCE_VERTICES_LIST__";
      }
      for (; sourceVerticesEntriesIt != periodicity_i.theSourceVerticesEntries.end(); ++sourceVerticesEntriesIt)
        save << " " << (*sourceVerticesEntriesIt);
      if (hasSourceVertices)
        save << " " << "__END_SOURCE_VERTICES_LIST__";

      std::vector<std::string>::const_iterator targetVerticesEntriesIt = periodicity_i.theTargetVerticesEntries.begin();
      bool hasTargetVertices = false;
      if (targetVerticesEntriesIt != periodicity_i.theTargetVerticesEntries.end()) {
        hasTargetVertices = true;
        save << " " << "__BEGIN_TARGET_VERTICES_LIST__";
      }
      for (; targetVerticesEntriesIt != periodicity_i.theTargetVerticesEntries.end(); ++targetVerticesEntriesIt)
        save << " " << (*targetVerticesEntriesIt);
      if (hasTargetVertices)
        save << " " << "__END_TARGET_VERTICES_LIST__";

      save << " " << "__END_PERIODICITY_DESCRIPTION__";
    }
    save << " " << "__PRECAD_" << shapeType << "_PERIODICITY_END__";
  }

}

//=============================================================================
std::istream & BLSURFPlugin_Hypothesis::LoadFrom(std::istream & load) {
  bool isOK = true;
  int i;
  double val;
  std::string option_or_sm;

  isOK = (load >> i);
  if (isOK)
    _topology = (Topology) i;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> i);
  if (isOK)
    _physicalMesh = (PhysicalMesh) i;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> i);
  if (isOK)
    _geometricMesh = (GeometricMesh) i;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    _phySize = val;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    _angleMesh = val;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    _gradation = val;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> i);
  if (isOK)
    _quadAllowed = (bool) i;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> i);
  if (isOK) {
    if ( i != -1) { // if value is -1, then this is no longer a standard option
      std::string & value = _option2value["respect_geometry"];
      bool _decimesh = (bool) i;
      value = _decimesh ? "1" : "0";
    }
  }
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    _minSize = val;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    _maxSize = val;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    // former parameter: get min value
    _angleMesh = min(val,_angleMesh);
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    // former parameter: get min value
    _minSize = min(val,_minSize);
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    // former parameter: get max value
    _maxSize = max(val,_maxSize);
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> i);
  if (isOK)
    _verb = i;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> i);
  if (isOK)
    _preCADMergeEdges = (bool) i;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> i);
  if (isOK) {
    if ( i != -1) { // if value is -1, then this is no longer a standard option
      std::string & value = _preCADoption2value["remove_tiny_edges"];
      bool _preCADRemoveNanoEdges = (bool) i;
      value = _preCADRemoveNanoEdges ? "1" : "0";
    }
  }
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> i);
  if (isOK)
    _preCADDiscardInput = (bool) i;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK) { // _preCADEpsNano
    if ( (i + 1.0) < 1e-6 ) { // if value is -1, then this is no longer a standard option: get optional value "tiny_edge_length" instead
      std::string & value = _preCADoption2value["tiny_edge_length"];
      std::ostringstream oss;
      oss << i;
      value = oss.str();
    }
  }
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> i);
  if (isOK)
    _enforcedInternalVerticesAllFaces = (bool) i;
  else
    load.clear(std::ios::badbit | load.rdstate());

  // New options with MeshGems-CADSurf

  bool hasCADSurfOptions = false;
  bool hasOptions = false;
  bool hasPreCADOptions = false;
  bool hasSizeMap = false;
  bool hasAttractor = false;
  bool hasNewAttractor = false;
  bool hasEnforcedVertex = false;
  bool hasPreCADFacesPeriodicity = false;
  bool hasPreCADEdgesPeriodicity = false;
  bool hasFacesPeriodicity = false;
  bool hasEdgesPeriodicity = false;
  bool hasVerticesPeriodicity = false;

  isOK = (load >> option_or_sm);
  if (isOK)
    if ( (option_or_sm == "1")||(option_or_sm == "0") ) {
      i = atoi(option_or_sm.c_str());
      hasCADSurfOptions = true;
      _phySizeRel = (bool) i;
    }
    if (option_or_sm == "__OPTIONS_BEGIN__")
      hasOptions = true;
    else if (option_or_sm == "__PRECAD_OPTIONS_BEGIN__")
      hasPreCADOptions = true;
    else if (option_or_sm == "__SIZEMAP_BEGIN__")
      hasSizeMap = true;
    else if (option_or_sm == "__ATTRACTORS_BEGIN__")
      hasAttractor = true;
    else if (option_or_sm == "__NEW_ATTRACTORS_BEGIN__")
      hasNewAttractor = true;
    else if (option_or_sm == "__ENFORCED_VERTICES_BEGIN__")
      hasEnforcedVertex = true;
    else if (option_or_sm == "__PRECAD_FACES_PERIODICITY_BEGIN__")
      hasPreCADFacesPeriodicity = true;
    else if (option_or_sm == "__PRECAD_EDGES_PERIODICITY_BEGIN__")
      hasPreCADEdgesPeriodicity = true;
    else if (option_or_sm == "__FACES_PERIODICITY_BEGIN__")
      hasFacesPeriodicity = true;
    else if (option_or_sm == "__EDGES_PERIODICITY_BEGIN__")
      hasEdgesPeriodicity = true;
    else if (option_or_sm == "__VERTICES_PERIODICITY_BEGIN__")
      hasVerticesPeriodicity = true;

  if (isOK && hasCADSurfOptions) {
    isOK = (load >> i);
    if (isOK)
      _minSizeRel = (bool) i;
    else
      load.clear(std::ios::badbit | load.rdstate());

    isOK = (load >> i);
    if (isOK)
      _maxSizeRel = (bool) i;
    else
      load.clear(std::ios::badbit | load.rdstate());

    isOK = (load >> val);
    if (isOK)
      _chordalError = val;
    else
      load.clear(std::ios::badbit | load.rdstate());

    isOK = (load >> i);
    if (isOK)
      _anisotropic = (bool) i;
    else
      load.clear(std::ios::badbit | load.rdstate());

    isOK = (load >> val);
    if (isOK)
      _anisotropicRatio = val;
    else
      load.clear(std::ios::badbit | load.rdstate());

    isOK = (load >> i);
    if (isOK)
      _removeTinyEdges = (bool) i;
    else
      load.clear(std::ios::badbit | load.rdstate());

    isOK = (load >> val);
    if (isOK)
      _tinyEdgeLength = val;
    else
      load.clear(std::ios::badbit | load.rdstate());

    isOK = (load >> i);
    if (isOK)
      _badElementRemoval = (bool) i;
    else
      load.clear(std::ios::badbit | load.rdstate());

    isOK = (load >> val);
    if (isOK)
      _badElementAspectRatio = val;
    else
      load.clear(std::ios::badbit | load.rdstate());

    isOK = (load >> i);
    if (isOK)
      _optimizeMesh = (bool) i;
    else
      load.clear(std::ios::badbit | load.rdstate());

    isOK = (load >> i);
    if (isOK)
      _quadraticMesh = (bool) i;
    else
      load.clear(std::ios::badbit | load.rdstate());

    isOK = (load >> i);
    if (isOK)
      _preCADProcess3DTopology = (bool) i;
    else
      load.clear(std::ios::badbit | load.rdstate());

  }
  

  if (hasCADSurfOptions) {
    isOK = (load >> option_or_sm);
    if (isOK)
      if (option_or_sm == "__OPTIONS_BEGIN__")
        hasOptions = true;
      else if (option_or_sm == "__PRECAD_OPTIONS_BEGIN__")
        hasPreCADOptions = true;
      else if (option_or_sm == "__SIZEMAP_BEGIN__")
        hasSizeMap = true;
      else if (option_or_sm == "__ATTRACTORS_BEGIN__")
        hasAttractor = true;
      else if (option_or_sm == "__NEW_ATTRACTORS_BEGIN__")
        hasNewAttractor = true;
      else if (option_or_sm == "__ENFORCED_VERTICES_BEGIN__")
        hasEnforcedVertex = true;
      else if (option_or_sm == "__PRECAD_FACES_PERIODICITY_BEGIN__")
        hasPreCADFacesPeriodicity = true;
      else if (option_or_sm == "__PRECAD_EDGES_PERIODICITY_BEGIN__")
        hasPreCADEdgesPeriodicity = true;
      else if (option_or_sm == "__FACES_PERIODICITY_BEGIN__")
        hasFacesPeriodicity = true;
      else if (option_or_sm == "__EDGES_PERIODICITY_BEGIN__")
        hasEdgesPeriodicity = true;
      else if (option_or_sm == "__VERTICES_PERIODICITY_BEGIN__")
        hasVerticesPeriodicity = true;
  }
  
  std::string optName, optValue;
  while (isOK && hasOptions) {
    isOK = (load >> optName);
    if (isOK) {
      if (optName == "__OPTIONS_END__")
        break;
      isOK = (load >> optValue);
    }
    if (isOK) {
      std::string & value = _option2value[optName];
      value = optValue;
      int len = value.size();
      // continue reading until "%#" encountered
      while (value[len - 1] != '#' || value[len - 2] != '%') {
        isOK = (load >> optValue);
        if (isOK) {
          value += " ";
          value += optValue;
          len = value.size();
        } else {
          break;
        }
      }
      value[len - 2] = '\0'; //cut off "%#"
    }
  }

  if (hasOptions) {
    isOK = (load >> option_or_sm);
    if (isOK)
      if (option_or_sm == "__PRECAD_OPTIONS_BEGIN__")
        hasPreCADOptions = true;
      else if (option_or_sm == "__SIZEMAP_BEGIN__")
        hasSizeMap = true;
      else if (option_or_sm == "__ATTRACTORS_BEGIN__")
        hasAttractor = true;
      else if (option_or_sm == "__NEW_ATTRACTORS_BEGIN__")
        hasNewAttractor = true;
      else if (option_or_sm == "__ENFORCED_VERTICES_BEGIN__")
        hasEnforcedVertex = true;
      else if (option_or_sm == "__PRECAD_FACES_PERIODICITY_BEGIN__")
        hasPreCADFacesPeriodicity = true;
      else if (option_or_sm == "__PRECAD_EDGES_PERIODICITY_BEGIN__")
        hasPreCADEdgesPeriodicity = true;
      else if (option_or_sm == "__FACES_PERIODICITY_BEGIN__")
        hasFacesPeriodicity = true;
      else if (option_or_sm == "__EDGES_PERIODICITY_BEGIN__")
        hasEdgesPeriodicity = true;
      else if (option_or_sm == "__VERTICES_PERIODICITY_BEGIN__")
        hasVerticesPeriodicity = true;
  }

  while (isOK && hasPreCADOptions) {
    isOK = (load >> optName);
    if (isOK) {
      if (optName == "__PRECAD_OPTIONS_END__")
        break;
      isOK = (load >> optValue);
    }
    if (isOK) {
      std::string & value = _preCADoption2value[optName];
      value = optValue;
      int len = value.size();
      // continue reading until "%#" encountered
      while (value[len - 1] != '#' || value[len - 2] != '%') {
        isOK = (load >> optValue);
        if (isOK) {
          value += " ";
          value += optValue;
          len = value.size();
        } else {
          break;
        }
      }
      value[len - 2] = '\0'; //cut off "%#"
    }
  }

  if (hasPreCADOptions) {
    isOK = (load >> option_or_sm);
    if (isOK)
      if (option_or_sm == "__SIZEMAP_BEGIN__")
        hasSizeMap = true;
      else if (option_or_sm == "__ATTRACTORS_BEGIN__")
        hasAttractor = true;
      else if (option_or_sm == "__NEW_ATTRACTORS_BEGIN__")
        hasNewAttractor = true;
      else if (option_or_sm == "__ENFORCED_VERTICES_BEGIN__")
        hasEnforcedVertex = true;
      else if (option_or_sm == "__PRECAD_FACES_PERIODICITY_BEGIN__")
        hasPreCADFacesPeriodicity = true;
      else if (option_or_sm == "__PRECAD_EDGES_PERIODICITY_BEGIN__")
        hasPreCADEdgesPeriodicity = true;
      else if (option_or_sm == "__FACES_PERIODICITY_BEGIN__")
        hasFacesPeriodicity = true;
      else if (option_or_sm == "__EDGES_PERIODICITY_BEGIN__")
        hasEdgesPeriodicity = true;
      else if (option_or_sm == "__VERTICES_PERIODICITY_BEGIN__")
        hasVerticesPeriodicity = true;
  }
  
  std::string smEntry, smValue;
  while (isOK && hasSizeMap) {
    isOK = (load >> smEntry);
    if (isOK) {
      if (smEntry == "__SIZEMAP_END__")
        break;
      isOK = (load >> smValue);
    }
    if (isOK) {
      std::string & value2 = _sizeMap[smEntry];
      value2 = smValue;
      int len2 = value2.size();
      // continue reading until "%#" encountered
      while (value2[len2 - 1] != '#' || value2[len2 - 2] != '%') {
        isOK = (load >> smValue);
        if (isOK) {
          value2 += " ";
          value2 += smValue;
          len2 = value2.size();
        } else {
          break;
        }
      }
      value2[len2 - 2] = '\0'; //cut off "%#"
    }
  }

  if (hasSizeMap) {
    isOK = (load >> option_or_sm);
    if (isOK)
      if (option_or_sm == "__ATTRACTORS_BEGIN__")
        hasAttractor = true;
      if (option_or_sm == "__NEW_ATTRACTORS_BEGIN__")
        hasNewAttractor = true;
      else if (option_or_sm == "__ENFORCED_VERTICES_BEGIN__")
        hasEnforcedVertex = true;
      else if (option_or_sm == "__PRECAD_FACES_PERIODICITY_BEGIN__")
        hasPreCADFacesPeriodicity = true;
      else if (option_or_sm == "__PRECAD_EDGES_PERIODICITY_BEGIN__")
        hasPreCADEdgesPeriodicity = true;
      else if (option_or_sm == "__FACES_PERIODICITY_BEGIN__")
        hasFacesPeriodicity = true;
      else if (option_or_sm == "__EDGES_PERIODICITY_BEGIN__")
        hasEdgesPeriodicity = true;
      else if (option_or_sm == "__VERTICES_PERIODICITY_BEGIN__")
        hasVerticesPeriodicity = true;
  }

  std::string atEntry, atValue;
  while (isOK && hasAttractor) {
    isOK = (load >> atEntry);
    if (isOK) {
      if (atEntry == "__ATTRACTORS_END__")
        break;
      isOK = (load >> atValue);
    }
    if (isOK) {
      std::string & value3 = _attractors[atEntry];
      value3 = atValue;
      int len3 = value3.size();
      // continue reading until "%#" encountered
      while (value3[len3 - 1] != '#' || value3[len3 - 2] != '%') {
        isOK = (load >> atValue);
        if (isOK) {
          value3 += " ";
          value3 += atValue;
          len3 = value3.size();
        } else {
          break;
        }
      }
      value3[len3 - 2] = '\0'; //cut off "%#"
    }
  }

  if (hasAttractor) {
    isOK = (load >> option_or_sm);
    if (isOK)
      if (option_or_sm == "__NEW_ATTRACTORS_BEGIN__")
        hasNewAttractor = true;
      else if (option_or_sm == "__ENFORCED_VERTICES_BEGIN__")
        hasEnforcedVertex = true;
      else if (option_or_sm == "__PRECAD_FACES_PERIODICITY_BEGIN__")
        hasPreCADFacesPeriodicity = true;
      else if (option_or_sm == "__PRECAD_EDGES_PERIODICITY_BEGIN__")
        hasPreCADEdgesPeriodicity = true;
      else if (option_or_sm == "__FACES_PERIODICITY_BEGIN__")
        hasFacesPeriodicity = true;
      else if (option_or_sm == "__EDGES_PERIODICITY_BEGIN__")
        hasEdgesPeriodicity = true;
      else if (option_or_sm == "__VERTICES_PERIODICITY_BEGIN__")
        hasVerticesPeriodicity = true;
  }

  std::string newAtFaceEntry, atTestString;
  std::string newAtShapeEntry;
  double attParams[4];
  double step;
  while (isOK && hasNewAttractor) {
    std::cout<<"Load new attractor"<<std::endl;
    isOK = (load >> newAtFaceEntry);
    if (isOK) {
      if (newAtFaceEntry == "__NEW_ATTRACTORS_END__")
        break;
      isOK = (load >> newAtShapeEntry);
      if (!isOK)
    break;
      isOK = (load >> attParams[0]>>attParams[1]>>attParams[2]>>attParams[3]); //>>step);
    }
    if (isOK) {
      MESSAGE(" LOADING ATTRACTOR HYPOTHESIS ")
      const TopoDS_Shape attractorShape = BLSURFPlugin_Hypothesis::entryToShape(newAtShapeEntry);
      const TopoDS_Face faceShape = TopoDS::Face(BLSURFPlugin_Hypothesis::entryToShape(newAtFaceEntry));
      BLSURFPlugin_Attractor* attractor = new BLSURFPlugin_Attractor(faceShape, attractorShape, newAtShapeEntry);//, step);
      attractor->SetParameters(attParams[0], attParams[1], attParams[2], attParams[3]);
      attractor->BuildMap();                     
      _classAttractors[newAtFaceEntry]=attractor;
    }
  }
  
  
  if (hasNewAttractor) {
    isOK = (load >> option_or_sm);
    if (isOK)
      if (option_or_sm == "__ENFORCED_VERTICES_BEGIN__")
        hasEnforcedVertex = true;
      else if (option_or_sm == "__PRECAD_FACES_PERIODICITY_BEGIN__")
        hasPreCADFacesPeriodicity = true;
      else if (option_or_sm == "__PRECAD_EDGES_PERIODICITY_BEGIN__")
        hasPreCADEdgesPeriodicity = true;
      else if (option_or_sm == "__FACES_PERIODICITY_BEGIN__")
        hasFacesPeriodicity = true;
      else if (option_or_sm == "__EDGES_PERIODICITY_BEGIN__")
        hasEdgesPeriodicity = true;
      else if (option_or_sm == "__VERTICES_PERIODICITY_BEGIN__")
        hasVerticesPeriodicity = true;
  }


// 
// Here is a example of the saved stream:
// __ENFORCED_VERTICES_BEGIN__ 
// __BEGIN_VERTEX__  => no name, no entry
// __BEGIN_GROUP__ mon groupe __END_GROUP__
// __BEGIN_COORDS__ 10 10 10 __END_COORDS__ 
// __BEGIN_FACELIST__ 0:1:1:1:1 __END_FACELIST__ 
// __END_VERTEX__ 
// __BEGIN_VERTEX__ => no coords
// __BEGIN_NAME__ mes points __END_NAME__ 
// __BEGIN_ENTRY__ 0:1:1:4 __END_ENTRY__
// __BEGIN_GROUP__ mon groupe __END_GROUP__
// __BEGIN_FACELIST__ 0:1:1:1:3 __END_FACELIST__
// __END_VERTEX__ 
// __ENFORCED_VERTICES_END__
// 

  std::string enfSeparator;
  std::string enfName;
  std::string enfGeomEntry;
  std::string enfGroup;
  TEntryList enfFaceEntryList;
  double enfCoords[3];
  bool hasCoords = false;
  
  _faceEntryEnfVertexListMap.clear();
  _enfVertexList.clear();
  _faceEntryCoordsListMap.clear();
  _coordsEnfVertexMap.clear();
  _faceEntryEnfVertexEntryListMap.clear();
  _enfVertexEntryEnfVertexMap.clear();
  
  
  while (isOK && hasEnforcedVertex) {
    isOK = (load >> enfSeparator); // __BEGIN_VERTEX__
    TEnfVertex *enfVertex = new TEnfVertex();
//     MESSAGE("enfSeparator: " <<enfSeparator);
    if (enfSeparator == "__ENFORCED_VERTICES_END__")
      break; // __ENFORCED_VERTICES_END__
    if (enfSeparator != "__BEGIN_VERTEX__")
      throw std::exception();
    
    while (isOK) {
      isOK = (load >> enfSeparator);
      MESSAGE("enfSeparator: " <<enfSeparator);
      if (enfSeparator == "__END_VERTEX__") {
        
        enfVertex->name = enfName;
        enfVertex->geomEntry = enfGeomEntry;
        enfVertex->grpName = enfGroup;
        enfVertex->coords.clear();
        if (hasCoords)
          enfVertex->coords.assign(enfCoords,enfCoords+3);
        enfVertex->faceEntries = enfFaceEntryList;
        
        _enfVertexList.insert(enfVertex);
        
        if (enfVertex->coords.size()) {
          _coordsEnfVertexMap[enfVertex->coords] = enfVertex;
          for (TEntryList::const_iterator it = enfVertex->faceEntries.begin() ; it != enfVertex->faceEntries.end(); ++it) {
            _faceEntryCoordsListMap[(*it)].insert(enfVertex->coords);
            _faceEntryEnfVertexListMap[(*it)].insert(enfVertex);
          }
        }
        if (!enfVertex->geomEntry.empty()) {
          _enfVertexEntryEnfVertexMap[enfVertex->geomEntry] = enfVertex;
          for (TEntryList::const_iterator it = enfVertex->faceEntries.begin() ; it != enfVertex->faceEntries.end(); ++it) {
            _faceEntryEnfVertexEntryListMap[(*it)].insert(enfVertex->geomEntry);
            _faceEntryEnfVertexListMap[(*it)].insert(enfVertex);
          }
        }
        
        enfName.clear();
        enfGeomEntry.clear();
        enfGroup.clear();
        enfFaceEntryList.clear();
        hasCoords = false;
        break; // __END_VERTEX__
      }
        
      if (enfSeparator == "__BEGIN_NAME__") {  // __BEGIN_NAME__
        while (isOK && (enfSeparator != "__END_NAME__")) {
          isOK = (load >> enfSeparator);
          if (enfSeparator != "__END_NAME__") {
            if (!enfName.empty())
              enfName += " ";
            enfName += enfSeparator;
          }
        }
        MESSAGE("enfName: " <<enfName);
      }
        
      if (enfSeparator == "__BEGIN_ENTRY__") {  // __BEGIN_ENTRY__
        isOK = (load >> enfGeomEntry);
        isOK = (load >> enfSeparator); // __END_ENTRY__
        if (enfSeparator != "__END_ENTRY__")
          throw std::exception();
        MESSAGE("enfGeomEntry: " <<enfGeomEntry);
      }
        
      if (enfSeparator == "__BEGIN_GROUP__") {  // __BEGIN_GROUP__
        while (isOK && (enfSeparator != "__END_GROUP__")) {
          isOK = (load >> enfSeparator);
          if (enfSeparator != "__END_GROUP__") {
            if (!enfGroup.empty())
              enfGroup += " ";
            enfGroup += enfSeparator;
          }
        }
        MESSAGE("enfGroup: " <<enfGroup);
      }
        
      if (enfSeparator == "__BEGIN_COORDS__") {  // __BEGIN_COORDS__
        hasCoords = true;
        isOK = (load >> enfCoords[0] >> enfCoords[1] >> enfCoords[2]);
        isOK = (load >> enfSeparator); // __END_COORDS__
        if (enfSeparator != "__END_COORDS__")
          throw std::exception();
        MESSAGE("enfCoords: " << enfCoords[0] <<","<< enfCoords[1] <<","<< enfCoords[2]);
      } 
        
      if (enfSeparator == "__BEGIN_FACELIST__") {  // __BEGIN_FACELIST__
        while (isOK && (enfSeparator != "__END_FACELIST__")) {
          isOK = (load >> enfSeparator);
          if (enfSeparator != "__END_FACELIST__") {
            enfFaceEntryList.insert(enfSeparator);
            MESSAGE(enfSeparator << " was inserted into enfFaceEntryList");
          }
        }
      } 
    }
  }

  // PERIODICITY

  if (hasPreCADFacesPeriodicity){
    LoadPreCADPeriodicity(load, "FACES");

    isOK = (load >> option_or_sm);
    if (isOK)
      if (option_or_sm == "__PRECAD_EDGES_PERIODICITY_BEGIN__")
        hasPreCADEdgesPeriodicity = true;
      else if (option_or_sm == "__FACES_PERIODICITY_BEGIN__")
        hasFacesPeriodicity = true;
      else if (option_or_sm == "__EDGES_PERIODICITY_BEGIN__")
        hasEdgesPeriodicity = true;
      else if (option_or_sm == "__VERTICES_PERIODICITY_BEGIN__")
        hasVerticesPeriodicity = true;
  }

  if (hasPreCADEdgesPeriodicity){
    LoadPreCADPeriodicity(load, "EDGES");

    isOK = (load >> option_or_sm);
    if (isOK)
      if (option_or_sm == "__FACES_PERIODICITY_BEGIN__")
        hasFacesPeriodicity = true;
      else if (option_or_sm == "__EDGES_PERIODICITY_BEGIN__")
        hasEdgesPeriodicity = true;
      else if (option_or_sm == "__VERTICES_PERIODICITY_BEGIN__")
        hasVerticesPeriodicity = true;
  }

  if (hasFacesPeriodicity){
      LoadFacesPeriodicity(load);

    isOK = (load >> option_or_sm);
    if (isOK)
      if (option_or_sm == "__EDGES_PERIODICITY_BEGIN__")
        hasEdgesPeriodicity = true;
      else if (option_or_sm == "__VERTICES_PERIODICITY_BEGIN__")
        hasVerticesPeriodicity = true;
  }

  if (hasEdgesPeriodicity){
      LoadEdgesPeriodicity(load);

    isOK = (load >> option_or_sm);
    if (isOK)
      if (option_or_sm == "__VERTICES_PERIODICITY_BEGIN__")
        hasVerticesPeriodicity = true;
  }

  if (hasVerticesPeriodicity)
      LoadVerticesPeriodicity(load);

  return load;
}

void BLSURFPlugin_Hypothesis::LoadFacesPeriodicity(std::istream & load){

  bool isOK = true;

  std::string periodicitySeparator;
  TEntry shape1Entry;
  TEntry shape2Entry;

  _facesPeriodicityVector.clear();

  while (isOK) {
    isOK = (load >> periodicitySeparator); // __BEGIN_PERIODICITY_DESCRIPTION__
    MESSAGE("periodicitySeparator 1: " <<periodicitySeparator);
    TFacesPeriodicity *periodicity_i = new TFacesPeriodicity();
    if (periodicitySeparator == "__FACES_PERIODICITY_END__")
      break; // __FACES_PERIODICITY_END__
    if (periodicitySeparator != "__BEGIN_PERIODICITY_DESCRIPTION__"){
      MESSAGE("//" << periodicitySeparator << "//");
      throw std::exception();
    }

    while (isOK) {
      isOK = (load >> periodicitySeparator);
      MESSAGE("periodicitySeparator 2: " <<periodicitySeparator);
      if (periodicitySeparator == "__END_PERIODICITY_DESCRIPTION__") {

        periodicity_i->first = shape1Entry;
        periodicity_i->second = shape2Entry;

        _facesPeriodicityVector.push_back(*periodicity_i);

        break; // __END_PERIODICITY_DESCRIPTION__
      }

      if (periodicitySeparator == "__BEGIN_ENTRY1__") {  // __BEGIN_ENTRY1__
        isOK = (load >> shape1Entry);
        isOK = (load >> periodicitySeparator); // __END_ENTRY1__
        if (periodicitySeparator != "__END_ENTRY1__")
          throw std::exception();
        MESSAGE("shape1Entry: " <<shape1Entry);
      }

      if (periodicitySeparator == "__BEGIN_ENTRY2__") {  // __BEGIN_ENTRY2__
        isOK = (load >> shape2Entry);
        isOK = (load >> periodicitySeparator); // __END_ENTRY2__
        if (periodicitySeparator != "__END_ENTRY2__")
          throw std::exception();
        MESSAGE("shape2Entry: " <<shape2Entry);
      }
    }
  }
}


void BLSURFPlugin_Hypothesis::LoadEdgesPeriodicity(std::istream & load){

  bool isOK = true;

  std::string periodicitySeparator;
  TEntry theFace1Entry;
  TEntry theEdge1Entry;
  TEntry theFace2Entry;
  TEntry theEdge2Entry;
  int edge_orientation = 0;

  _edgesPeriodicityVector.clear();

  while (isOK) {
    isOK = (load >> periodicitySeparator); // __BEGIN_PERIODICITY_DESCRIPTION__
    MESSAGE("periodicitySeparator 1: " <<periodicitySeparator);
    TEdgePeriodicity *periodicity_i = new TEdgePeriodicity();
    if (periodicitySeparator == "__EDGES_PERIODICITY_END__")
      break; // __EDGES_PERIODICITY_END__
    if (periodicitySeparator != "__BEGIN_PERIODICITY_DESCRIPTION__"){
      MESSAGE("//" << periodicitySeparator << "//");
      throw std::exception();
    }

    while (isOK) {
      isOK = (load >> periodicitySeparator);
      MESSAGE("periodicitySeparator 2: " <<periodicitySeparator);
      if (periodicitySeparator == "__END_PERIODICITY_DESCRIPTION__") {

        periodicity_i->theFace1Entry = theFace1Entry;
        periodicity_i->theEdge1Entry = theEdge1Entry;
        periodicity_i->theFace2Entry = theFace2Entry;
        periodicity_i->theEdge2Entry = theEdge2Entry;
        periodicity_i->edge_orientation = edge_orientation;

        _edgesPeriodicityVector.push_back(*periodicity_i);

        break; // __END_PERIODICITY_DESCRIPTION__
      }

      if (periodicitySeparator == "__BEGIN_FACE1__") {  // __BEGIN_FACE1__
        isOK = (load >> theFace1Entry);
        MESSAGE("//" << theFace1Entry << "//");
        isOK = (load >> periodicitySeparator); // __END_FACE1__
        if (periodicitySeparator != "__END_FACE1__"){
          MESSAGE("//" << periodicitySeparator << "//");
          throw std::exception();
        }
        MESSAGE("theFace1Entry: " <<theFace1Entry);
      }

      if (periodicitySeparator == "__BEGIN_EDGE1__") {  // __BEGIN_EDGE1__
        isOK = (load >> theEdge1Entry);
        isOK = (load >> periodicitySeparator); // __END_EDGE1__
        if (periodicitySeparator != "__END_EDGE1__")
          throw std::exception();
        MESSAGE("theEdge1Entry: " <<theEdge1Entry);
      }

      if (periodicitySeparator == "__BEGIN_FACE2__") {  // __BEGIN_FACE2__
        isOK = (load >> theFace2Entry);
        isOK = (load >> periodicitySeparator); // __END_FACE2__
        if (periodicitySeparator != "__END_FACE2__")
          throw std::exception();
        MESSAGE("theFace2Entry: " <<theFace2Entry);
      }

      if (periodicitySeparator == "__BEGIN_EDGE2__") {  // __BEGIN_EDGE2__
        isOK = (load >> theEdge2Entry);
        isOK = (load >> periodicitySeparator); // __END_EDGE2__
        if (periodicitySeparator != "__END_EDGE2__")
          throw std::exception();
        MESSAGE("theEdge2Entry: " <<theEdge2Entry);
      }

      if (periodicitySeparator == "__BEGIN_EDGE_ORIENTATION__") {  // __BEGIN_EDGE_ORIENTATION__
        isOK = (load >> edge_orientation);
        isOK = (load >> periodicitySeparator); // __END_EDGE_ORIENTATION__
        if (periodicitySeparator != "__END_EDGE_ORIENTATION__")
          throw std::exception();
        MESSAGE("edge_orientation: " <<edge_orientation);
      }
    }
  }
}

void BLSURFPlugin_Hypothesis::LoadVerticesPeriodicity(std::istream & load){

  bool isOK = true;

  std::string periodicitySeparator;
  TEntry theEdge1Entry;
  TEntry theVertex1Entry;
  TEntry theEdge2Entry;
  TEntry theVertex2Entry;

  _verticesPeriodicityVector.clear();

  while (isOK) {
    isOK = (load >> periodicitySeparator); // __BEGIN_PERIODICITY_DESCRIPTION__
    MESSAGE("periodicitySeparator 1: " <<periodicitySeparator);
    TVertexPeriodicity *periodicity_i = new TVertexPeriodicity();
    if (periodicitySeparator == "__VERTICES_PERIODICITY_END__")
      break; // __VERTICES_PERIODICITY_END__
    if (periodicitySeparator != "__BEGIN_PERIODICITY_DESCRIPTION__"){
      MESSAGE("//" << periodicitySeparator << "//");
      throw std::exception();
    }

    while (isOK) {
      isOK = (load >> periodicitySeparator);
      MESSAGE("periodicitySeparator 2: " <<periodicitySeparator);
      if (periodicitySeparator == "__END_PERIODICITY_DESCRIPTION__") {

        periodicity_i->theEdge1Entry = theEdge1Entry;
        periodicity_i->theVertex1Entry = theVertex1Entry;
        periodicity_i->theEdge2Entry = theEdge2Entry;
        periodicity_i->theVertex2Entry = theVertex2Entry;

        _verticesPeriodicityVector.push_back(*periodicity_i);

        break; // __END_PERIODICITY_DESCRIPTION__
      }

      if (periodicitySeparator == "__BEGIN_EDGE1__") {  // __BEGIN_EDGE1__
        isOK = (load >> theEdge1Entry);
        isOK = (load >> periodicitySeparator); // __END_EDGE1__
        if (periodicitySeparator != "__END_EDGE1__")
          throw std::exception();
        MESSAGE("theEdge1Entry: " <<theEdge1Entry);
      }

      if (periodicitySeparator == "__BEGIN_VERTEX1__") {  // __BEGIN_VERTEX1__
        isOK = (load >> theVertex1Entry);
        isOK = (load >> periodicitySeparator); // __END_VERTEX1__
        if (periodicitySeparator != "__END_VERTEX1__")
          throw std::exception();
        MESSAGE("theVertex1Entry: " <<theVertex1Entry);
      }

      if (periodicitySeparator == "__BEGIN_EDGE2__") {  // __BEGIN_EDGE2__
        isOK = (load >> theEdge2Entry);
        isOK = (load >> periodicitySeparator); // __END_EDGE2__
        if (periodicitySeparator != "__END_EDGE2__")
          throw std::exception();
        MESSAGE("theEdge2Entry: " <<theEdge2Entry);
      }

      if (periodicitySeparator == "__BEGIN_VERTEX2__") {  // __BEGIN_VERTEX2__
        isOK = (load >> theVertex2Entry);
        isOK = (load >> periodicitySeparator); // __END_VERTEX2__
        if (periodicitySeparator != "__END_VERTEX2__")
          throw std::exception();
        MESSAGE("theVertex2Entry: " <<theVertex2Entry);
      }
    }
  }
}

void BLSURFPlugin_Hypothesis::LoadPreCADPeriodicity(std::istream & load, const char* shapeType) {

  bool isOK = true;

  std::string periodicitySeparator;
  TEntry shape1Entry;
  TEntry shape2Entry;
  std::vector<std::string> theSourceVerticesEntries;
  std::vector<std::string> theTargetVerticesEntries;

  bool hasSourceVertices = false;
  bool hasTargetVertices = false;

  if (shapeType == "FACES")
    _preCadFacesPeriodicityVector.clear();
  else
    _preCadEdgesPeriodicityVector.clear();


  while (isOK) {
    isOK = (load >> periodicitySeparator); // __BEGIN_PERIODICITY_DESCRIPTION__
    MESSAGE("periodicitySeparator 1: " <<periodicitySeparator);
    TPreCadPeriodicity *periodicity_i = new TPreCadPeriodicity();
//     MESSAGE("periodicitySeparator: " <<periodicitySeparator);
    std::string endSeparator = "__PRECAD_" + std::string(shapeType) + "_PERIODICITY_END__";
    if (periodicitySeparator == endSeparator)
      break; // __PRECAD_FACES_PERIODICITY_END__
    if (periodicitySeparator != "__BEGIN_PERIODICITY_DESCRIPTION__"){
      MESSAGE("//" << endSeparator << "//");
      MESSAGE("//" << periodicitySeparator << "//");
      throw std::exception();
    }

    while (isOK) {
      isOK = (load >> periodicitySeparator);
      MESSAGE("periodicitySeparator 2: " <<periodicitySeparator);
      if (periodicitySeparator == "__END_PERIODICITY_DESCRIPTION__") {

        periodicity_i->shape1Entry = shape1Entry;
        periodicity_i->shape2Entry = shape2Entry;

        MESSAGE("theSourceVerticesEntries.size(): " << theSourceVerticesEntries.size());
        MESSAGE("theTargetVerticesEntries.size(): " << theTargetVerticesEntries.size());

        if (hasSourceVertices)
          periodicity_i->theSourceVerticesEntries = theSourceVerticesEntries;
        if (hasTargetVertices)
          periodicity_i->theTargetVerticesEntries = theTargetVerticesEntries;

        if (shapeType == "FACES")
          _preCadFacesPeriodicityVector.push_back(*periodicity_i);
        else
          _preCadEdgesPeriodicityVector.push_back(*periodicity_i);

        theSourceVerticesEntries.clear();
        theTargetVerticesEntries.clear();
        hasSourceVertices = false;
        hasTargetVertices = false;
        break; // __END_PERIODICITY_DESCRIPTION__
      }

      if (periodicitySeparator == "__BEGIN_ENTRY1__") {  // __BEGIN_ENTRY1__
        isOK = (load >> shape1Entry);
        isOK = (load >> periodicitySeparator); // __END_ENTRY1__
        if (periodicitySeparator != "__END_ENTRY1__")
          throw std::exception();
        MESSAGE("shape1Entry: " <<shape1Entry);
      }

      if (periodicitySeparator == "__BEGIN_ENTRY2__") {  // __BEGIN_ENTRY2__
        isOK = (load >> shape2Entry);
        isOK = (load >> periodicitySeparator); // __END_ENTRY2__
        if (periodicitySeparator != "__END_ENTRY2__")
          throw std::exception();
        MESSAGE("shape2Entry: " <<shape2Entry);
      }

      if (periodicitySeparator == "__BEGIN_SOURCE_VERTICES_LIST__") {  // __BEGIN_SOURCE_VERTICES_LIST__
        hasSourceVertices = true;
        while (isOK && (periodicitySeparator != "__END_SOURCE_VERTICES_LIST__")) {
          isOK = (load >> periodicitySeparator);
          if (periodicitySeparator != "__END_SOURCE_VERTICES_LIST__") {
            theSourceVerticesEntries.push_back(periodicitySeparator);
            MESSAGE("theSourceVerticesEntries: " <<periodicitySeparator);
          }
        }
      }

      if (periodicitySeparator == "__BEGIN_TARGET_VERTICES_LIST__") {  // __BEGIN_TARGET_VERTICES_LIST__
        hasTargetVertices = true;
        while (isOK && (periodicitySeparator != "__END_TARGET_VERTICES_LIST__")) {
          isOK = (load >> periodicitySeparator);
          if (periodicitySeparator != "__END_TARGET_VERTICES_LIST__") {
            theTargetVerticesEntries.push_back(periodicitySeparator);
            MESSAGE("theTargetVerticesEntries: " <<periodicitySeparator);
          }
        }
      }
    }
  }
}

//=============================================================================
std::ostream & operator <<(std::ostream & save, BLSURFPlugin_Hypothesis & hyp) {
  return hyp.SaveTo(save);
}

//=============================================================================
std::istream & operator >>(std::istream & load, BLSURFPlugin_Hypothesis & hyp) {
  return hyp.LoadFrom(load);
}

//================================================================================
/*!
 * \brief Does nothing
 */
//================================================================================

bool BLSURFPlugin_Hypothesis::SetParametersByMesh(const SMESH_Mesh* theMesh, const TopoDS_Shape& theShape) {
  return false;
}

//================================================================================
/*!
 * \brief Returns default global constant physical size given a default value of element length ratio
 */
//================================================================================

double BLSURFPlugin_Hypothesis::GetDefaultPhySize(double diagonal, double bbSegmentation) {
  if (bbSegmentation != 0 && diagonal != 0)
    return diagonal / bbSegmentation ;
  return 10;
}

//================================================================================
/*!
 * \brief Returns default min size given a default value of element length ratio
 */
//================================================================================

double BLSURFPlugin_Hypothesis::GetDefaultMinSize(double diagonal) {
  if (diagonal != 0)
    return diagonal / 1000.0 ;
  return undefinedDouble();
}

//================================================================================
/*!
 * \brief Returns default max size given a default value of element length ratio
 */
//================================================================================

double BLSURFPlugin_Hypothesis::GetDefaultMaxSize(double diagonal) {
  if (diagonal != 0)
    return diagonal / 5.0 ;
  return undefinedDouble();
}

//================================================================================
/*!
 * \brief Returns default chordal error given a default value of element length ratio
 */
//================================================================================

double BLSURFPlugin_Hypothesis::GetDefaultChordalError(double diagonal) {
  if (diagonal != 0)
    return diagonal;
  return undefinedDouble();
}

//================================================================================
/*!
 * \brief Returns default tiny edge length given a default value of element length ratio
 */
//================================================================================

double BLSURFPlugin_Hypothesis::GetDefaultTinyEdgeLength(double diagonal) {
  if (diagonal != 0)
    return diagonal * 1e-6 ;
  return undefinedDouble();
}

//=============================================================================
/*!
 * \brief Initialize my parameter values by default parameters.
 *  \retval bool - true if parameter values have been successfully defined
 */
//=============================================================================

bool BLSURFPlugin_Hypothesis::SetParametersByDefaults(const TDefaults& dflts, const SMESH_Mesh* theMesh) {
  double diagonal = dflts._elemLength*_gen->GetBoundaryBoxSegmentation();
  _phySize = GetDefaultPhySize(diagonal, _gen->GetBoundaryBoxSegmentation());
  _minSize = GetDefaultMinSize(diagonal);
  _maxSize = GetDefaultMaxSize(diagonal);
  _chordalError = GetDefaultChordalError(diagonal);
  _tinyEdgeLength = GetDefaultTinyEdgeLength(diagonal);

  return true;
//   return bool(_phySize = dflts._elemLength);
}
