// Copyright (C) 2007-2011  CEA/DEN, EDF R&D
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
  SMESH_Hypothesis(hypId, studyId, gen), _topology(GetDefaultTopology()),
  _physicalMesh(GetDefaultPhysicalMesh()),
  _phySize(GetDefaultPhySize()),
  _phyMax(GetDefaultMaxSize()),
  _phyMin(GetDefaultMinSize()),
  _hgeoMax(GetDefaultMaxSize()),
  _hgeoMin(GetDefaultMinSize()), 
  _geometricMesh(GetDefaultGeometricMesh()),
  _angleMeshS(GetDefaultAngleMeshS()),
  _angleMeshC(GetDefaultAngleMeshC()),
  _gradation(GetDefaultGradation()),
  _quadAllowed(GetDefaultQuadAllowed()),
  _decimesh(GetDefaultDecimesh()),
  _verb(GetDefaultVerbosity()),
  _preCADMergeEdges(GetDefaultPreCADMergeEdges()),
  _preCADRemoveNanoEdges(GetDefaultPreCADRemoveNanoEdges()),
  _preCADDiscardInput(GetDefaultPreCADDiscardInput()),
  _preCADEpsNano(GetDefaultPreCADEpsNano()),
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
  _enforcedInternalVerticesAllFaces(GetDefaultInternalEnforcedVertex())
{
  _name = "BLSURF_Parameters";
  _param_algo_dim = 2;
  
//   _GMFFileMode = false; // GMF ascii mode
  
  // to disable writing boundaries
  //_phyMin = _phyMax = _hgeoMin = _hgeoMax = undefinedDouble();


  const char* intOptionNames[] = { "addsurf_ivertex", "anisotropic", "background", "CheckAdjacentEdges", "CheckCloseEdges",
      "CheckWellDefined", "coiter", "communication", "debug", "decim", "export_flag", "file_h", "frontal", "gridnu", "gridnv",
      "hinterpol_flag", "hmean_flag", "intermedfile", "memory", "normals", "optim", "pardom_flag", "pinch", "refs",
      "rigid", "surforient", "tconf", "topo_collapse",
      "proximity", "prox_nb_layer", "prox_ratio", // detects the volumic proximity of surfaces
      "" // mark of end
      };
  const char* doubleOptionNames[] = { "addsurf_angle", "addsurf_R", "addsurf_H", "addsurf_FG", "addsurf_r",
      "addsurf_PA", "angle_compcurv", "angle_ridge", "anisotropic_ratio", "CoefRectangle", "eps_collapse", "eps_ends", "eps_pardom", "LSS",
      "topo_eps1", "topo_eps2", "" // mark of end
      };
  const char* charOptionNames[] = { "export_format", "export_option", "import_option", "prefix", "" // mark of end
      };

  // PreCAD advanced options
  const char* preCADintOptionNames[] = { "closed_geometry", "debug", "manifold_geometry", "create_tag_on_collision","" // mark of end
      };
  const char* preCADdoubleOptionNames[] = { "eps_nano_relative", "eps_sewing", "eps_sewing_relative", "periodic_tolerance",
      "periodic_tolerance_relative", "periodic_split_tolerance", "periodic_split_tolerance_relative", "" // mark of end
      };
  
  int i = 0;
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
  SALOMEDS::GenericAttribute_var anAttr;

  if (!aSObj->_is_nil() && aSObj->FindAttribute(anAttr, "AttributeIOR")) {
    SALOMEDS::AttributeIOR_var anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
    CORBA::String_var aVal = anIOR->Value();
    CORBA::Object_var obj = myStudy->ConvertIORToObject(aVal);
    aGeomObj = GEOM::GEOM_Object::_narrow(obj);
  }
  if ( !aGeomObj->_is_nil() )
    S = smeshGen_i->GeomObjectToShape( aGeomObj.in() );
  return S;
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetTopology(Topology theTopology) {
  if (theTopology != _topology) {
    _topology = theTopology;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetPhysicalMesh(PhysicalMesh thePhysicalMesh) {
  if (thePhysicalMesh != _physicalMesh) {
    _physicalMesh = thePhysicalMesh;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetPhySize(double theVal) {
  if (theVal != _phySize) {
    if (theVal == 0) {
      _phySize = GetPhyMax();
      MESSAGE("Warning: nul physical size is not allowed");
    }
    else
      _phySize = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetPhyMin(double theMinSize) {
  if (theMinSize != _phyMin) {
    _phyMin = theMinSize;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetPhyMax(double theMaxSize) {
  if (theMaxSize != _phyMax) {
    _phyMax = theMaxSize;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetGeoMin(double theMinSize) {
  if (theMinSize != _hgeoMin) {
    _hgeoMin = theMinSize;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetGeoMax(double theMaxSize) {
  if (theMaxSize != _hgeoMax) {
    _hgeoMax = theMaxSize;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetGeometricMesh(GeometricMesh theGeometricMesh) {
  if (theGeometricMesh != _geometricMesh) {
    _geometricMesh = theGeometricMesh;
    switch (_geometricMesh) {
      case DefaultGeom:
      default:
        _angleMeshS = GetDefaultAngleMeshS();
        _gradation = GetDefaultGradation();
        break;
    }
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetAngleMeshS(double theVal) {
  if (theVal != _angleMeshS) {
    _angleMeshS = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetAngleMeshC(double theVal) {
  if (theVal != _angleMeshC) {
    _angleMeshC = theVal;
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
void BLSURFPlugin_Hypothesis::SetDecimesh(bool theVal) {
  if (theVal != _decimesh) {
    _decimesh = theVal;
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
    SetTopology(PreCAD);
    _preCADMergeEdges = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetPreCADRemoveNanoEdges(bool theVal) {
  if (theVal != _preCADRemoveNanoEdges) {
    SetTopology(PreCAD);
    _preCADRemoveNanoEdges = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetPreCADDiscardInput(bool theVal) {
  if (theVal != _preCADDiscardInput) {
    SetTopology(PreCAD);
    _preCADDiscardInput = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetPreCADEpsNano(double theVal) {
  if (theVal != _preCADEpsNano) {
    SetTopology(PreCAD);
    _preCADEpsNano = theVal;
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
    SetPhysicalMesh(SizeMap);
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
    SetPhysicalMesh(SizeMap);
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
  SetPhysicalMesh(SizeMap);

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

  SetPhysicalMesh(SizeMap);

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
      SetPhysicalMesh(SizeMap);
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
std::ostream & BLSURFPlugin_Hypothesis::SaveTo(std::ostream & save) {
  save << " " << (int) _topology << " " << (int) _physicalMesh << " " << (int) _geometricMesh << " " << _phySize << " "
      << _angleMeshS << " " << _gradation << " " << (int) _quadAllowed << " " << (int) _decimesh;
  save << " " << _phyMin << " " << _phyMax << " " << _angleMeshC << " " << _hgeoMin << " " << _hgeoMax << " " << _verb;
  save << " " << (int) _preCADMergeEdges << " " << (int) _preCADRemoveNanoEdges << " " << (int) _preCADDiscardInput << " " << _preCADEpsNano ;

  TOptionValues::iterator op_val = _option2value.begin();
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

  return save;
}

//=============================================================================
std::istream & BLSURFPlugin_Hypothesis::LoadFrom(std::istream & load) {
  bool isOK = true;
  int i;
  double val;

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
    _angleMeshS = val;
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
  if (isOK)
    _decimesh = (bool) i;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    _phyMin = val;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    _phyMax = val;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    _angleMeshC = val;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    _hgeoMin = val;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    _hgeoMax = val;
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
  if (isOK)
    _preCADRemoveNanoEdges = (bool) i;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> i);
  if (isOK)
    _preCADDiscardInput = (bool) i;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    _preCADEpsNano = val;
  else
    load.clear(std::ios::badbit | load.rdstate());

  std::string option_or_sm;
  bool hasOptions = false;
  bool hasPreCADOptions = false;
  bool hasSizeMap = false;
  bool hasAttractor = false;
  bool hasNewAttractor = false;
  bool hasEnforcedVertex = false;

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

  return load;
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

//=============================================================================
/*!
 * \brief Initialize my parameter values by default parameters.
 *  \retval bool - true if parameter values have been successfully defined
 */
//=============================================================================

bool BLSURFPlugin_Hypothesis::SetParametersByDefaults(const TDefaults& dflts, const SMESH_Mesh* theMesh) {
  return bool(_phySize = dflts._elemLength);
}

//=============================================================================
BLSURFPlugin_Hypothesis::Topology BLSURFPlugin_Hypothesis::GetDefaultTopology() {
  return FromCAD;
}

//=============================================================================
BLSURFPlugin_Hypothesis::PhysicalMesh BLSURFPlugin_Hypothesis::GetDefaultPhysicalMesh() {
  return PhysicalUserDefined;
}

//=============================================================================
double BLSURFPlugin_Hypothesis::GetDefaultPhySize() {
  return 10;
}

//======================================================================
double BLSURFPlugin_Hypothesis::GetDefaultMaxSize() {
  return undefinedDouble(); // 1e+4;
}

//======================================================================
double BLSURFPlugin_Hypothesis::GetDefaultMinSize() {
  return undefinedDouble(); //1e-4;
}

//======================================================================
BLSURFPlugin_Hypothesis::GeometricMesh BLSURFPlugin_Hypothesis::GetDefaultGeometricMesh() {
  return DefaultGeom;
}

//=============================================================================
double BLSURFPlugin_Hypothesis::GetDefaultAngleMeshS() {
  return 8;
}

//=============================================================================
double BLSURFPlugin_Hypothesis::GetDefaultGradation() {
  return 1.1;
}

//=============================================================================
bool BLSURFPlugin_Hypothesis::GetDefaultQuadAllowed() {
  return false;
}

//=============================================================================
bool BLSURFPlugin_Hypothesis::GetDefaultDecimesh() {
  return false;
}

//======================================================================
double BLSURFPlugin_Hypothesis::GetDefaultPreCADEpsNano() {
  return undefinedDouble(); //1e-4;
}

//======================================================================
std::string BLSURFPlugin_Hypothesis::GetDefaultGMFFile() {
  return "";
}

//=============================================================================
bool BLSURFPlugin_Hypothesis::GetDefaultInternalEnforcedVertex() {
  return false;
}

