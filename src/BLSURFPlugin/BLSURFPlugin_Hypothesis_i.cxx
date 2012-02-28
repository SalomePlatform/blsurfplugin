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
//           Size maps developement: Nicolas GEIMER (OCC) & Gilles DAVID (EURIWARE)
// ---
//
#include "BLSURFPlugin_Hypothesis_i.hxx"
#include "SMESH_Gen.hxx"
#include "SMESH_Gen_i.hxx"
#include "SMESH_PythonDump.hxx"
#include "GEOM_Object.hxx"

#include "Utils_CorbaException.hxx"
#include "utilities.h"

#include <stdexcept>
#include <cstring>
#include "boost/regex.hpp"

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::BLSURFPlugin_Hypothesis_i
 *
 *  Constructor
 */
//=============================================================================
BLSURFPlugin_Hypothesis_i::BLSURFPlugin_Hypothesis_i(PortableServer::POA_ptr thePOA, int theStudyId,
    ::SMESH_Gen* theGenImpl) :
  SALOME::GenericObj_i(thePOA), SMESH_Hypothesis_i(thePOA) {
  MESSAGE( "BLSURFPlugin_Hypothesis_i::BLSURFPlugin_Hypothesis_i" );
  myBaseImpl = new ::BLSURFPlugin_Hypothesis(theGenImpl->GetANewId(), theStudyId, theGenImpl);
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::~BLSURFPlugin_Hypothesis_i
 *
 *  Destructor
 */
//=============================================================================
BLSURFPlugin_Hypothesis_i::~BLSURFPlugin_Hypothesis_i() {
  MESSAGE( "BLSURFPlugin_Hypothesis_i::~BLSURFPlugin_Hypothesis_i" );
}

/*!
 *  BLSURFPlugin_Hypothesis_i::SetTopology
 *
 *  Set topology
 */

//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetTopology(CORBA::Long theValue) {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::SetTopology");
  ASSERT(myBaseImpl);
  this->GetImpl()->SetTopology((::BLSURFPlugin_Hypothesis::Topology) theValue);
  SMESH::TPythonDump() << _this() << ".SetTopology( " << theValue << " )";
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetTopology
 *
 *  Get Topology
 */
//=============================================================================
CORBA::Long BLSURFPlugin_Hypothesis_i::GetTopology() {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::GetTopology");
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetTopology();
}

//=============================================================================

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::SetPhysicalMesh
 *
 *  Set PhysicalMesh
 */

//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetPhysicalMesh(CORBA::Long theValue) {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::SetPhysicalMesh");
  ASSERT(myBaseImpl);
  this->GetImpl()->SetPhysicalMesh((::BLSURFPlugin_Hypothesis::PhysicalMesh) theValue);
  SMESH::TPythonDump() << _this() << ".SetPhysicalMesh( " << theValue << " )";
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetPhysicalMesh
 *
 *  Get PhysicalMesh
 */
//=============================================================================
CORBA::Long BLSURFPlugin_Hypothesis_i::GetPhysicalMesh() {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::GetPhysicalMesh");
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetPhysicalMesh();
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::SetPhySize
 *
 *  Set PhySize
 */
//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetPhySize(CORBA::Double theValue) {
//   MESSAGE("BLSURFPlugin_Hypothesis_i::SetPhySize");
  ASSERT(myBaseImpl);
  this->GetImpl()->SetPhySize(theValue);
  SMESH::TPythonDump() << _this() << ".SetPhySize( " << theValue << " )";
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetPhySize
 *
 *  Get PhySize
 */
//=============================================================================
CORBA::Double BLSURFPlugin_Hypothesis_i::GetPhySize() {
//   MESSAGE("BLSURFPlugin_Hypothesis_i::GetPhySize");
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetPhySize();
}

//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetPhyMin(CORBA::Double theMinSize) {
  ASSERT(myBaseImpl);
  if (GetPhyMin() != theMinSize) {
    this->GetImpl()->SetPhyMin(theMinSize);
    SMESH::TPythonDump() << _this() << ".SetPhyMin( " << theMinSize << " )";
  }
}

//=============================================================================
CORBA::Double BLSURFPlugin_Hypothesis_i::GetPhyMin() {
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetPhyMin();
}

//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetPhyMax(CORBA::Double theMaxSize) {
  ASSERT(myBaseImpl);
  if (GetPhyMax() != theMaxSize) {
    this->GetImpl()->SetPhyMax(theMaxSize);
    SMESH::TPythonDump() << _this() << ".SetPhyMax( " << theMaxSize << " )";
  }
}

//=============================================================================
CORBA::Double BLSURFPlugin_Hypothesis_i::GetPhyMax() {
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetPhyMax();
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::SetGeometricMesh
 *
 *  Set GeometricMesh
 */

//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetGeometricMesh(CORBA::Long theValue) {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::SetGeometricMesh");
  ASSERT(myBaseImpl);
  this->GetImpl()->SetGeometricMesh((::BLSURFPlugin_Hypothesis::GeometricMesh) theValue);
  SMESH::TPythonDump() << _this() << ".SetGeometricMesh( " << theValue << " )";
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetGeometricMesh
 *
 *  Get GeometricMesh
 */
//=============================================================================
CORBA::Long BLSURFPlugin_Hypothesis_i::GetGeometricMesh() {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::GetGeometricMesh");
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetGeometricMesh();
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::SetAngleMeshS
 *
 *  Set AngleMeshS
 */
//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetAngleMeshS(CORBA::Double theValue) {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::SetAngleMeshS");
  ASSERT(myBaseImpl);
  this->GetImpl()->SetAngleMeshS(theValue);
  SMESH::TPythonDump() << _this() << ".SetAngleMeshS( " << theValue << " )";
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetAngleMeshS
 *
 *  Get AngleMeshS
 */
//=============================================================================
CORBA::Double BLSURFPlugin_Hypothesis_i::GetAngleMeshS() {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::GetAngleMeshS");
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetAngleMeshS();
}

//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetAngleMeshC(CORBA::Double angle) {
  ASSERT(myBaseImpl);
  this->GetImpl()->SetAngleMeshC(angle);
  SMESH::TPythonDump() << _this() << ".SetAngleMeshC( " << angle << " )";
}

//=============================================================================
CORBA::Double BLSURFPlugin_Hypothesis_i::GetAngleMeshC() {
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetAngleMeshC();
}

//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetGeoMin(CORBA::Double theMinSize) {
  ASSERT(myBaseImpl);
  if (GetGeoMin() != theMinSize) {
    this->GetImpl()->SetGeoMin(theMinSize);
    SMESH::TPythonDump() << _this() << ".SetGeoMin( " << theMinSize << " )";
  }
}

//=============================================================================
CORBA::Double BLSURFPlugin_Hypothesis_i::GetGeoMin() {
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetGeoMin();
}

//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetGeoMax(CORBA::Double theMaxSize) {
  ASSERT(myBaseImpl);
  if (GetGeoMax() != theMaxSize) {
    this->GetImpl()->SetGeoMax(theMaxSize);
    SMESH::TPythonDump() << _this() << ".SetGeoMax( " << theMaxSize << " )";
  }
}

//=============================================================================
CORBA::Double BLSURFPlugin_Hypothesis_i::GetGeoMax() {
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetGeoMax();
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::SetGradation
 *
 *  Set Gradation
 */
//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetGradation(CORBA::Double theValue) {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::SetGradation");
  ASSERT(myBaseImpl);
  this->GetImpl()->SetGradation(theValue);
  SMESH::TPythonDump() << _this() << ".SetGradation( " << theValue << " )";
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetGradation
 *
 *  Get Gradation
 */
//=============================================================================
CORBA::Double BLSURFPlugin_Hypothesis_i::GetGradation() {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::GetGradation");
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetGradation();
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::SetQuadAllowed
 *
 *  Set true or false
 */
//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetQuadAllowed(CORBA::Boolean theValue) {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::SetQuadAllowed");
  ASSERT(myBaseImpl);
  this->GetImpl()->SetQuadAllowed(theValue);
  std::string theValueStr = theValue ? "True" : "False";
  SMESH::TPythonDump() << _this() << ".SetQuadAllowed( " << theValueStr.c_str() << " )";
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetQuadAllowed
 *
 *  Get true or false
 */
//=============================================================================
CORBA::Boolean BLSURFPlugin_Hypothesis_i::GetQuadAllowed() {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::GetQuadAllowed");
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetQuadAllowed();
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::SetDecimesh
 *
 *  Set true or false
 */
//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetDecimesh(CORBA::Boolean theValue) {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::SetDecimesh");
  ASSERT(myBaseImpl);
  this->GetImpl()->SetDecimesh(theValue);
  std::string theValueStr = theValue ? "True" : "False";
  SMESH::TPythonDump() << _this() << ".SetDecimesh( " << theValueStr.c_str() << " )";
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetDecimesh
 *
 *  Get true or false
 */
//=============================================================================
CORBA::Boolean BLSURFPlugin_Hypothesis_i::GetDecimesh() {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::GetDecimesh");
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetDecimesh();
}

//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetVerbosity(CORBA::Short theVal) throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);
  if (theVal < 0 || theVal > 100)
    THROW_SALOME_CORBA_EXCEPTION( "Invalid verbosity level",SALOME::BAD_PARAM );
  this->GetImpl()->SetVerbosity(theVal);
  SMESH::TPythonDump() << _this() << ".SetVerbosity( " << theVal << " )";
}

//=============================================================================

CORBA::Short BLSURFPlugin_Hypothesis_i::GetVerbosity() {
  ASSERT(myBaseImpl);
  return (CORBA::Short) this->GetImpl()->GetVerbosity();
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::SetPreCADMergeEdges
 *
 *  Set true or false
 */
//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetPreCADMergeEdges(CORBA::Boolean theValue) {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::SetPreCADMergeEdges");
  ASSERT(myBaseImpl);
  this->GetImpl()->SetPreCADMergeEdges(theValue);
  std::string theValueStr = theValue ? "True" : "False";
  SMESH::TPythonDump() << _this() << ".SetPreCADMergeEdges( " << theValueStr.c_str() << " )";
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetPreCADMergeEdges
 *
 *  Get true or false
 */
//=============================================================================
CORBA::Boolean BLSURFPlugin_Hypothesis_i::GetPreCADMergeEdges() {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::GetPreCADMergeEdges");
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetPreCADMergeEdges();
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::SetPreCADRemoveNanoEdges
 *
 *  Set true or false
 */
//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetPreCADRemoveNanoEdges(CORBA::Boolean theValue) {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::SetPreCADRemoveNanoEdges");
  ASSERT(myBaseImpl);
  this->GetImpl()->SetPreCADRemoveNanoEdges(theValue);
  std::string theValueStr = theValue ? "True" : "False";
  SMESH::TPythonDump() << _this() << ".SetPreCADRemoveNanoEdges( " << theValueStr.c_str() << " )";
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetPreCADRemoveNanoEdges
 *
 *  Get true or false
 */
//=============================================================================
CORBA::Boolean BLSURFPlugin_Hypothesis_i::GetPreCADRemoveNanoEdges() {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::GetPreCADRemoveNanoEdges");
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetPreCADRemoveNanoEdges();
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::SetPreCADDiscardInput
 *
 *  Set true or false
 */
//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetPreCADDiscardInput(CORBA::Boolean theValue) {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::SetPreCADDiscardInput");
  ASSERT(myBaseImpl);
  this->GetImpl()->SetPreCADDiscardInput(theValue);
  std::string theValueStr = theValue ? "True" : "False";
  SMESH::TPythonDump() << _this() << ".SetPreCADDiscardInput( " << theValueStr.c_str() << " )";
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetPreCADDiscardInput
 *
 *  Get true or false
 */
//=============================================================================
CORBA::Boolean BLSURFPlugin_Hypothesis_i::GetPreCADDiscardInput() {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::GetPreCADDiscardInput");
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetPreCADDiscardInput();
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::SetPreCADEpsNano
 *
 *  Set length for nano edges
 */
//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetPreCADEpsNano(CORBA::Double theValue) {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::SetPreCADEpsNano");
  ASSERT(myBaseImpl);
  this->GetImpl()->SetPreCADEpsNano(theValue);
  SMESH::TPythonDump() << _this() << ".SetPreCADEpsNano( " << theValue << " )";
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetPreCADEpsNano
 *
 *  Get length of nano edges
 */
//=============================================================================
CORBA::Double BLSURFPlugin_Hypothesis_i::GetPreCADEpsNano() {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::GetPreCADEpsNano");
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetPreCADEpsNano();
}

//=============================================================================

void BLSURFPlugin_Hypothesis_i::SetOptionValue(const char* optionName, const char* optionValue)
    throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);
  bool valueChanged = false;
  try {
    valueChanged = (this->GetImpl()->GetOptionValue(optionName) != optionValue);
    if (valueChanged)
      this->GetImpl()->SetOptionValue(optionName, optionValue);
  } catch (const std::invalid_argument& ex) {
    SALOME::ExceptionStruct ExDescription;
    ExDescription.text = ex.what();
    ExDescription.type = SALOME::BAD_PARAM;
    ExDescription.sourceFile = "BLSURFPlugin_Hypothesis::SetOptionValue(name,value)";
    ExDescription.lineNumber = 0;
    throw SALOME::SALOME_Exception(ExDescription);
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
  if (valueChanged)
    SMESH::TPythonDump() << _this() << ".SetOptionValue( '" << optionName << "', '" << optionValue << "' )";
}

//=============================================================================

void BLSURFPlugin_Hypothesis_i::SetPreCADOptionValue(const char* optionName, const char* optionValue)
    throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);
  bool valueChanged = false;
  try {
    valueChanged = (this->GetImpl()->GetPreCADOptionValue(optionName) != optionValue);
    if (valueChanged)
      this->GetImpl()->SetPreCADOptionValue(optionName, optionValue);
  } catch (const std::invalid_argument& ex) {
    SALOME::ExceptionStruct ExDescription;
    ExDescription.text = ex.what();
    ExDescription.type = SALOME::BAD_PARAM;
    ExDescription.sourceFile = "BLSURFPlugin_Hypothesis::SetPreCADOptionValue(name,value)";
    ExDescription.lineNumber = 0;
    throw SALOME::SALOME_Exception(ExDescription);
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
  if (valueChanged)
    SMESH::TPythonDump() << _this() << ".SetPreCADOptionValue( '" << optionName << "', '" << optionValue << "' )";
}

//=============================================================================

char* BLSURFPlugin_Hypothesis_i::GetOptionValue(const char* optionName) throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);
  try {
    return CORBA::string_dup(this->GetImpl()->GetOptionValue(optionName).c_str());
  } catch (const std::invalid_argument& ex) {
    SALOME::ExceptionStruct ExDescription;
    ExDescription.text = ex.what();
    ExDescription.type = SALOME::BAD_PARAM;
    ExDescription.sourceFile = "BLSURFPlugin_Hypothesis::GetOptionValue(name)";
    ExDescription.lineNumber = 0;
    throw SALOME::SALOME_Exception(ExDescription);
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
  return 0;
}

//=============================================================================

char* BLSURFPlugin_Hypothesis_i::GetPreCADOptionValue(const char* optionName) throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);
  try {
    return CORBA::string_dup(this->GetImpl()->GetPreCADOptionValue(optionName).c_str());
  } catch (const std::invalid_argument& ex) {
    SALOME::ExceptionStruct ExDescription;
    ExDescription.text = ex.what();
    ExDescription.type = SALOME::BAD_PARAM;
    ExDescription.sourceFile = "BLSURFPlugin_Hypothesis::GetPreCADOptionValue(name)";
    ExDescription.lineNumber = 0;
    throw SALOME::SALOME_Exception(ExDescription);
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
  return 0;
}

//=============================================================================

void BLSURFPlugin_Hypothesis_i::UnsetOption(const char* optionName) {
  ASSERT(myBaseImpl);
  this->GetImpl()->ClearOption(optionName);
  SMESH::TPythonDump() << _this() << ".UnsetOption( '" << optionName << "' )";
}

//=============================================================================

void BLSURFPlugin_Hypothesis_i::UnsetPreCADOption(const char* optionName) {
  ASSERT(myBaseImpl);
  this->GetImpl()->ClearPreCADOption(optionName);
  SMESH::TPythonDump() << _this() << ".UnsetPreCADOption( '" << optionName << "' )";
}

//=============================================================================

BLSURFPlugin::string_array* BLSURFPlugin_Hypothesis_i::GetOptionValues() {
  ASSERT(myBaseImpl);
  BLSURFPlugin::string_array_var result = new BLSURFPlugin::string_array();

  const ::BLSURFPlugin_Hypothesis::TOptionValues & opts = this->GetImpl()->GetOptionValues();
  result->length(opts.size());

  ::BLSURFPlugin_Hypothesis::TOptionValues::const_iterator opIt = opts.begin();
  for (int i = 0; opIt != opts.end(); ++opIt, ++i) {
    string name_value = opIt->first;
    if (!opIt->second.empty()) {
      name_value += ":";
      name_value += opIt->second;
    }
    result[i] = CORBA::string_dup(name_value.c_str());
  }
  return result._retn();
}

//=============================================================================

BLSURFPlugin::string_array* BLSURFPlugin_Hypothesis_i::GetPreCADOptionValues() {
  ASSERT(myBaseImpl);
  BLSURFPlugin::string_array_var result = new BLSURFPlugin::string_array();

  const ::BLSURFPlugin_Hypothesis::TOptionValues & opts = this->GetImpl()->GetPreCADOptionValues();
  result->length(opts.size());

  ::BLSURFPlugin_Hypothesis::TOptionValues::const_iterator opIt = opts.begin();
  for (int i = 0; opIt != opts.end(); ++opIt, ++i) {
    string name_value = opIt->first;
    if (!opIt->second.empty()) {
      name_value += ":";
      name_value += opIt->second;
    }
    result[i] = CORBA::string_dup(name_value.c_str());
  }
  return result._retn();
}

//=============================================================================

void BLSURFPlugin_Hypothesis_i::SetOptionValues(const BLSURFPlugin::string_array& options)
    throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);
  for (int i = 0; i < options.length(); ++i) {
    string name_value = options[i].in();
    int colonPos = name_value.find(':');
    string name, value;
    if (colonPos == string::npos) // ':' not found
      name = name_value;
    else {
      name = name_value.substr(0, colonPos);
      if (colonPos < name_value.size() - 1 && name_value[colonPos] != ' ')
        value = name_value.substr(colonPos + 1);
    }
    SetOptionValue(name.c_str(), value.c_str());
  }
}

//=============================================================================

void BLSURFPlugin_Hypothesis_i::SetPreCADOptionValues(const BLSURFPlugin::string_array& options)
    throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);
  for (int i = 0; i < options.length(); ++i) {
    string name_value = options[i].in();
    int colonPos = name_value.find(':');
    string name, value;
    if (colonPos == string::npos) // ':' not found
      name = name_value;
    else {
      name = name_value.substr(0, colonPos);
      if (colonPos < name_value.size() - 1 && name_value[colonPos] != ' ')
        value = name_value.substr(colonPos + 1);
    }
    SetPreCADOptionValue(name.c_str(), value.c_str());
  }
}

//=============================================================================

void BLSURFPlugin_Hypothesis_i::SetSizeMapEntry(const char* entry, const char* sizeMap)
    throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);
  MESSAGE("ENGINE : SETSIZEMAP START ENTRY : " << entry);
  bool valueChanged = false;
  try {
    valueChanged = (this->GetImpl()->GetSizeMapEntry(entry) != sizeMap);
    if (valueChanged)
      this->GetImpl()->SetSizeMapEntry(entry, sizeMap);
  } catch (const std::invalid_argument& ex) {
    SALOME::ExceptionStruct ExDescription;
    ExDescription.text = ex.what();
    ExDescription.type = SALOME::BAD_PARAM;
    ExDescription.sourceFile = "BLSURFPlugin_Hypothesis::SetSizeMapEntry(entry,sizemap)";
    ExDescription.lineNumber = 0;
    throw SALOME::SALOME_Exception(ExDescription);
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
  MESSAGE("ENGINE : SETSIZEMAP END ENTRY : " << entry);
  if (valueChanged)
    SMESH::TPythonDump() << _this() << ".SetSizeMap(" << entry << ", '" << sizeMap << "' )";
}

//=============================================================================

void BLSURFPlugin_Hypothesis_i::SetAttractorEntry(const char* entry, const char* attractor)
    throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);
  MESSAGE("ENGINE : SETATTRACTOR START ENTRY : " << entry);
  bool valueChanged = false;
  try {
    valueChanged = ( this->GetImpl()->GetAttractorEntry(entry) != attractor );
    if ( valueChanged ) {
      boost::regex re("^ATTRACTOR\\((?:(-?0(\\.\\d*)*|-?[1-9]+\\d*(\\.\\d*)*|-?\\.(\\d)+);){5}(True|False)(?:;(-?0(\\.\\d*)*|-?[1-9]+\\d*(\\.\\d*)*|-?\\.(\\d)+))?\\)$");
      if (!boost::regex_match(string(attractor), re))
        throw std::invalid_argument("Error: an attractor is defined with the following pattern: ATTRACTOR(xa;ya;za;a;b;True|False;d(opt.))");
      this->GetImpl()->SetAttractorEntry(entry, attractor);
    }
  } catch (const std::invalid_argument& ex) {
    SALOME::ExceptionStruct ExDescription;
    ExDescription.text = ex.what();
    ExDescription.type = SALOME::BAD_PARAM;
    ExDescription.sourceFile = "BLSURFPlugin_Hypothesis::SetAttractorEntry(entry,attractor)";
    ExDescription.lineNumber = 0;
    throw SALOME::SALOME_Exception(ExDescription);
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
  MESSAGE("ENGINE : SETATTRACTOR END ENTRY : " << entry);
  if (valueChanged)
    SMESH::TPythonDump() << _this() << ".SetAttractor(" << entry << ", '" << attractor << "' )";
}

//=============================================================================

void BLSURFPlugin_Hypothesis_i::SetClassAttractorEntry(const char* entry, const char* att_entry, double StartSize, double EndSize, double ActionRadius, double ConstantRadius) //TODO à finir
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
  MESSAGE("ENGINE : SETATTRACTOR START ENTRY : " << entry);
  bool valueChanged = false;
  try {
    this->GetImpl()->SetClassAttractorEntry(entry, att_entry, StartSize, EndSize, ActionRadius, ConstantRadius);
  }
  catch (const std::invalid_argument& ex) {
    SALOME::ExceptionStruct ExDescription;
    ExDescription.text = ex.what();
    ExDescription.type = SALOME::BAD_PARAM;
    ExDescription.sourceFile = "BLSURFPlugin_Hypothesis::SetClassAttractorEntry(entry, att_entry, StartSize, EndSize, ActionRadius, ConstantRadius)";
    ExDescription.lineNumber = 0;
    throw SALOME::SALOME_Exception(ExDescription);
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
  MESSAGE("ENGINE : SETATTRACTOR END ENTRY : " << entry);
  //if ( valueChanged )
  SMESH::TPythonDump() << _this() << ".SetAttractorGeom("
                       << entry << ", " << att_entry << ", "<<StartSize<<", "<<EndSize<<", "<<ActionRadius<<", "<<ConstantRadius<<" )";
}

//=============================================================================

char* BLSURFPlugin_Hypothesis_i::GetSizeMapEntry(const char* entry) throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);
  try {
    return CORBA::string_dup(this->GetImpl()->GetSizeMapEntry(entry).c_str());
  } catch (const std::invalid_argument& ex) {
    SALOME::ExceptionStruct ExDescription;
    ExDescription.text = ex.what();
    ExDescription.type = SALOME::BAD_PARAM;
    ExDescription.sourceFile = "BLSURFPlugin_Hypothesis::GetSizeMapEntry(name)";
    ExDescription.lineNumber = 0;
    throw SALOME::SALOME_Exception(ExDescription);
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
  return 0;
}

//=============================================================================

char* BLSURFPlugin_Hypothesis_i::GetAttractorEntry(const char* entry) throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);
  try {
    return CORBA::string_dup(this->GetImpl()->GetAttractorEntry(entry).c_str());
  } catch (const std::invalid_argument& ex) {
    SALOME::ExceptionStruct ExDescription;
    ExDescription.text = ex.what();
    ExDescription.type = SALOME::BAD_PARAM;
    ExDescription.sourceFile = "BLSURFPlugin_Hypothesis::GetAttractorEntry(name)";
    ExDescription.lineNumber = 0;
    throw SALOME::SALOME_Exception(ExDescription);
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
  return 0;
}

// //=============================================================================
// 
// // TODO coder cette fonction (utilisée pour savoir si la valeur a changé
// // A finir pour le dump
// char* BLSURFPlugin_Hypothesis_i::GetClassAttractorEntry(const char* entry)
//   throw (SALOME::SALOME_Exception)
// {
//   ASSERT(myBaseImpl);
//   try {
//     return CORBA::string_dup( this->GetImpl()->GetClassAttractorEntry(entry).c_str());
//   }
//   catch (const std::invalid_argument& ex) {
//     SALOME::ExceptionStruct ExDescription;
//     ExDescription.text = ex.what();
//     ExDescription.type = SALOME::BAD_PARAM;
//     ExDescription.sourceFile = "BLSURFPlugin_Hypothesis::GetClassAttractorEntry(name)";
//     ExDescription.lineNumber = 0;
//     throw SALOME::SALOME_Exception(ExDescription);
//   }
//   catch (SALOME_Exception& ex) {
//     THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
//   }
//   return 0;
// }

//=============================================================================

void BLSURFPlugin_Hypothesis_i::UnsetEntry(const char* entry) {
  ASSERT(myBaseImpl);
  this->GetImpl()->ClearEntry(entry);
  //  SMESH::TPythonDump() << _this() << ".UnsetSizeMap( " << entry << " )";
}

//=============================================================================

BLSURFPlugin::string_array* BLSURFPlugin_Hypothesis_i::GetSizeMapEntries() {
  ASSERT(myBaseImpl);
  BLSURFPlugin::string_array_var result = new BLSURFPlugin::string_array();

  const ::BLSURFPlugin_Hypothesis::TSizeMap sizeMaps = this->GetImpl()->_GetSizeMapEntries();
  result->length(sizeMaps.size());

  ::BLSURFPlugin_Hypothesis::TSizeMap::const_iterator smIt = sizeMaps.begin();
  for (int i = 0; smIt != sizeMaps.end(); ++smIt, ++i) {
    string entry_sizemap = smIt->first;
    if (!smIt->second.empty()) {
      entry_sizemap += "|";
      entry_sizemap += smIt->second;
    }
    result[i] = CORBA::string_dup(entry_sizemap.c_str());
  }
  return result._retn();
}

//=============================================================================

BLSURFPlugin::string_array* BLSURFPlugin_Hypothesis_i::GetAttractorEntries() {
  ASSERT(myBaseImpl);
  BLSURFPlugin::string_array_var result = new BLSURFPlugin::string_array();

  const ::BLSURFPlugin_Hypothesis::TSizeMap attractors = this->GetImpl()->_GetAttractorEntries();
  result->length(attractors.size());

  ::BLSURFPlugin_Hypothesis::TSizeMap::const_iterator atIt = attractors.begin();
  for (int i = 0; atIt != attractors.end(); ++atIt, ++i) {
    string entry_attractor = atIt->first;
    if (!atIt->second.empty()) {
      entry_attractor += "|";
      entry_attractor += atIt->second;
    }
    result[i] = CORBA::string_dup(entry_attractor.c_str());
  }
  return result._retn();
}

//=============================================================================

BLSURFPlugin::TAttParamsMap* BLSURFPlugin_Hypothesis_i::GetAttractorParams()
{
  ASSERT(myBaseImpl);
  BLSURFPlugin::TAttParamsMap_var result = new BLSURFPlugin::TAttParamsMap();

  const ::BLSURFPlugin_Hypothesis::TAttractorMap attractors= this->GetImpl()->_GetClassAttractorEntries();
  result->length( attractors.size() );

  ::BLSURFPlugin_Hypothesis::TAttractorMap::const_iterator atIt = attractors.begin();
  for ( int i = 0 ; atIt != attractors.end(); ++atIt, ++i ) {
    string faceEntry = atIt->first;
    string attEntry;
    double startSize, endSize, infDist, constDist;
    if ( !atIt->second->Empty() ) {
      attEntry = atIt->second->GetAttractorEntry();
      MESSAGE("GetAttractorParams : attEntry ="<<attEntry)
      std::vector<double> params = atIt->second->GetParameters();
      startSize = params[0];
      endSize = params[1];
      infDist = params[2];
      constDist = params[3];
    }
    result[i].faceEntry = CORBA::string_dup(faceEntry.c_str());
    result[i].attEntry = CORBA::string_dup(attEntry.c_str());
    result[i].startSize = startSize;
    result[i].endSize = endSize;
    result[i].infDist = infDist;
    result[i].constDist = constDist;
    MESSAGE("GetAttractorParams : result[i].attEntry ="<<result[i].attEntry)
    MESSAGE("GetAttractorParams : result[i].faceEntry ="<<result[i].faceEntry)
  }
  return result._retn();
}

//=============================================================================

void BLSURFPlugin_Hypothesis_i::SetSizeMapEntries(const BLSURFPlugin::string_array& sizeMaps)
    throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);
  for (int i = 0; i < sizeMaps.length(); ++i) {
    string entry_sizemap = sizeMaps[i].in();
    int colonPos = entry_sizemap.find('|');
    string entry, sizemap;
    if (colonPos == string::npos) // '|' separator not found
      entry = entry_sizemap;
    else {
      entry = entry_sizemap.substr(0, colonPos);
      if (colonPos < entry_sizemap.size() - 1 && entry_sizemap[colonPos] != ' ')
        sizemap = entry_sizemap.substr(colonPos + 1);
    }
    this->GetImpl()->SetSizeMapEntry(entry.c_str(), sizemap.c_str());
  }
}

//=============================================================================

void BLSURFPlugin_Hypothesis_i::ClearSizeMaps() {
  ASSERT(myBaseImpl);
  this->GetImpl()->ClearSizeMaps();
}

//=============================================================================

void BLSURFPlugin_Hypothesis_i::SetSizeMap(const GEOM::GEOM_Object_ptr GeomObj, const char* sizeMap) {
  ASSERT(myBaseImpl);
  string entry;
  entry = GeomObj->GetStudyEntry();
  MESSAGE("IDL : GetName : " << GeomObj->GetName());
  MESSAGE("IDL : SETSIZEMAP ( "<< entry << " , " << sizeMap << ")");
  SetSizeMapEntry(entry.c_str(), sizeMap);
}

//=============================================================================
void BLSURFPlugin_Hypothesis_i::UnsetSizeMap(const GEOM::GEOM_Object_ptr GeomObj) {
  ASSERT(myBaseImpl);
  string entry;
  entry = GeomObj->GetStudyEntry();
  MESSAGE("IDL : GetName : " << GeomObj->GetName());
  MESSAGE("IDL : UNSETSIZEMAP ( "<< entry << ")");
  UnsetEntry(entry.c_str());
  SMESH::TPythonDump() << _this() << ".UnsetSizeMap( " << entry.c_str() << " )";
}

void BLSURFPlugin_Hypothesis_i::SetAttractor(GEOM::GEOM_Object_ptr GeomObj, const char* attractor) {
  ASSERT(myBaseImpl);
  string entry;
  entry = GeomObj->GetStudyEntry();
  MESSAGE("IDL : GetName : " << GeomObj->GetName());
  MESSAGE("IDL : SETATTRACTOR ( "<< entry << " , " << attractor << ")");
  SetAttractorEntry(entry.c_str(), attractor);
}

void BLSURFPlugin_Hypothesis_i::UnsetAttractor(GEOM::GEOM_Object_ptr GeomObj) {
  ASSERT(myBaseImpl);
  string entry;
  entry = GeomObj->GetStudyEntry();
  MESSAGE("IDL : GetName : " << GeomObj->GetName());
  MESSAGE("IDL : UNSETATTRACTOR ( "<< entry << ")");
  UnsetEntry(entry.c_str());
  SMESH::TPythonDump() << _this() << ".UnsetAttractor( " << entry.c_str() << " )";
}

void BLSURFPlugin_Hypothesis_i::SetAttractorGeom(GEOM::GEOM_Object_ptr theFace, GEOM::GEOM_Object_ptr theAttractor, double StartSize, double EndSize, double ActionRadius, double ConstantRadius)
{
  ASSERT(myBaseImpl);
  string theFaceEntry;
  string theAttEntry;
  theFaceEntry = theFace->GetStudyEntry();
  theAttEntry  = theAttractor->GetStudyEntry();
  
  GEOM::GEOM_Gen_ptr geomGen = SMESH_Gen_i::GetGeomEngine();
  SMESH_Gen_i *smeshGen = SMESH_Gen_i::GetSMESHGen();
  string aName;
  
  if (theFaceEntry.empty()) {
    aName = "Face_";
    aName += theFace->GetEntry();
    SALOMEDS::SObject_ptr theSFace = geomGen->PublishInStudy(smeshGen->GetCurrentStudy(), NULL, theFace, aName.c_str());
    if (!theSFace->_is_nil())
      theFaceEntry = theSFace->GetID();
  }
  if (theFaceEntry.empty())
    THROW_SALOME_CORBA_EXCEPTION( "Geom object is not published in study" ,SALOME::BAD_PARAM );
  
  if (theAttEntry.empty()) {
    if (theAttractor->GetShapeType() == GEOM::VERTEX)
      aName = "Vertex_";
    if (theAttractor->GetShapeType() == GEOM::EDGE)
      aName = "Edge_";
    if (theAttractor->GetShapeType() == GEOM::WIRE)
      aName = "Wire_";
    if (theAttractor->GetShapeType() == GEOM::COMPOUND)
      aName = "Compound_";
    aName += theAttractor->GetEntry();
    SALOMEDS::SObject_ptr theSAtt = geomGen->PublishInStudy(smeshGen->GetCurrentStudy(), NULL, theAttractor, aName.c_str());
    if (!theSAtt->_is_nil())
      theAttEntry = theSAtt->GetID();
  }
  if (theAttEntry.empty())
    THROW_SALOME_CORBA_EXCEPTION( "Geom object is not published in study" ,SALOME::BAD_PARAM );
  
  TopoDS_Face FaceShape = TopoDS::Face(SMESH_Gen_i::GetSMESHGen()->GeomObjectToShape( theFace ));
  TopoDS_Shape AttractorShape = SMESH_Gen_i::GetSMESHGen()->GeomObjectToShape( theAttractor );
  MESSAGE("IDL : GetName : " << theFace->GetName());
  MESSAGE("IDL : SETATTRACTOR () ");//<< entry << " , " << att_entry << ")");
  SetClassAttractorEntry( theFaceEntry.c_str(), theAttEntry.c_str(), StartSize, EndSize, ActionRadius, ConstantRadius);
}

void BLSURFPlugin_Hypothesis_i::UnsetAttractorGeom(GEOM::GEOM_Object_ptr theFace)
{
  ASSERT(myBaseImpl);
  string theFaceEntry;
  theFaceEntry = theFace->GetStudyEntry();
  
  GEOM::GEOM_Gen_ptr geomGen = SMESH_Gen_i::GetGeomEngine();
  SMESH_Gen_i *smeshGen = SMESH_Gen_i::GetSMESHGen();
  string aName;
  
  if (theFaceEntry.empty()) {
    aName = "Face_";
    aName += theFace->GetEntry();
    SALOMEDS::SObject_ptr theSFace = geomGen->PublishInStudy(smeshGen->GetCurrentStudy(), NULL, theFace, aName.c_str());
    if (!theSFace->_is_nil())
      theFaceEntry = theSFace->GetID();
  }
  if (theFaceEntry.empty())
    THROW_SALOME_CORBA_EXCEPTION( "Geom object is not published in study" ,SALOME::BAD_PARAM );
  
  MESSAGE("IDL : GetName : " << theFace->GetName());
  MESSAGE("IDL : UNSETATTRACTOR ( "<< theFaceEntry << ")");
  UnsetEntry( theFaceEntry.c_str());
  SMESH::TPythonDump() << _this() << ".UnsetAttractorGeom( " << theFaceEntry.c_str() << " )";
}

/*
 void BLSURFPlugin_Hypothesis_i::SetCustomSizeMap(GEOM::GEOM_Object_ptr GeomObj, const char* sizeMap)
 {}

 void BLSURFPlugin_Hypothesis_i::UnsetCustomSizeMap(GEOM::GEOM_Object_ptr GeomObj)
 {}

 void BLSURFPlugin_Hypothesis_i::SetCustomSizeMapEntry(const char* entry,const char* sizeMap )  throw (SALOME::SALOME_Exception)
 {}

 char* BLSURFPlugin_Hypothesis_i::GetCustomSizeMapEntry(const char* entry)  throw (SALOME::SALOME_Exception)
 {}

 void BLSURFPlugin_Hypothesis_i::UnsetCustomSizeMapEntry(const char* entry)
 {
 ASSERT(myBaseImpl);
 this->GetImpl()->UnsetCustomSizeMap(entry);
 SMESH::TPythonDump() << _this() << ".UnsetCustomSizeMap( " << entry << " )";
 }


 BLSURFPlugin::string_array* BLSURFPlugin_Hypothesis_i::GetCustomSizeMapEntries()
 {}

 */

// ///////////////////////
// // ENFORCED VERTICES //
// ///////////////////////


/**
 * Returns the list of enforced vertices for a given Face entry
 * @return A map of Face entry / List of enforced vertices
 *
 */
BLSURFPlugin::TFaceEntryEnfVertexListMap* BLSURFPlugin_Hypothesis_i::GetAllEnforcedVerticesByFace() {
  MESSAGE("IDL: GetAllEnforcedVerticesByFace()");
  ASSERT(myBaseImpl);

  BLSURFPlugin::TFaceEntryEnfVertexListMap_var resultMap = new BLSURFPlugin::TFaceEntryEnfVertexListMap();

  const ::BLSURFPlugin_Hypothesis::TFaceEntryEnfVertexListMap faceEntryEnfVertexListMap =
      this->GetImpl()->_GetAllEnforcedVerticesByFace();
  resultMap->length(faceEntryEnfVertexListMap.size());
  MESSAGE("Face entry to Enforced Vertex map size is " << resultMap->length());

  ::BLSURFPlugin_Hypothesis::TEnfVertexList _enfVertexList;
  ::BLSURFPlugin_Hypothesis::TFaceEntryEnfVertexListMap::const_iterator it_entry = faceEntryEnfVertexListMap.begin();
  for (int i = 0; it_entry != faceEntryEnfVertexListMap.end(); ++it_entry, ++i) {
    BLSURFPlugin::TFaceEntryEnfVertexListMapElement_var mapElement =
        new BLSURFPlugin::TFaceEntryEnfVertexListMapElement();
    mapElement->faceEntry = CORBA::string_dup(it_entry->first.c_str());
    MESSAGE("Face Entry: " << mapElement->faceEntry);

    _enfVertexList = it_entry->second;
    BLSURFPlugin::TEnfVertexList_var enfVertexList = new BLSURFPlugin::TEnfVertexList();
    enfVertexList->length(_enfVertexList.size());
    MESSAGE("Number of enf vertex: " << enfVertexList->length());

    ::BLSURFPlugin_Hypothesis::TEnfVertexList::const_iterator it_enfVertex = _enfVertexList.begin();
    ::BLSURFPlugin_Hypothesis::TEnfVertex *currentEnfVertex;
    for (int j = 0; it_enfVertex != _enfVertexList.end(); ++it_enfVertex, ++j) {
      currentEnfVertex = (*it_enfVertex);

      BLSURFPlugin::TEnfVertex_var enfVertex = new BLSURFPlugin::TEnfVertex();

      // Name
      enfVertex->name = CORBA::string_dup(currentEnfVertex->name.c_str());

      // Geom entry
      enfVertex->geomEntry = CORBA::string_dup(currentEnfVertex->geomEntry.c_str());

      // Coords
      BLSURFPlugin::TEnfVertexCoords_var coords = new BLSURFPlugin::TEnfVertexCoords();
      coords->length(currentEnfVertex->coords.size());
      for (int i=0;i<coords->length();i++)
        coords[i] = currentEnfVertex->coords[i];
      enfVertex->coords = coords;

      // Group name
      enfVertex->grpName = CORBA::string_dup(currentEnfVertex->grpName.c_str());
      
      // Face entry list
      BLSURFPlugin::TEntryList_var faceEntryList = new BLSURFPlugin::TEntryList();
      faceEntryList->length(currentEnfVertex->faceEntries.size());
      ::BLSURFPlugin_Hypothesis::TEntryList::const_iterator it_entry = currentEnfVertex->faceEntries.begin();
      for (int ind = 0; it_entry != currentEnfVertex->faceEntries.end();++it_entry, ++ind)
        faceEntryList[ind] = CORBA::string_dup((*it_entry).c_str());
      enfVertex->faceEntries = faceEntryList;

      ostringstream msg;
      msg << "Enforced vertex: \n"
          << "Name: " << enfVertex->name << "\n";
      if (coords->length())
          msg << "Coords: " << enfVertex->coords[0] << ", " << enfVertex->coords[1] << ", " << enfVertex->coords[2] << "\n";
      msg << "Geom entry: " << enfVertex->geomEntry << "\n"
          << "Group Name: " << enfVertex->grpName;
      MESSAGE(msg.str());

      enfVertexList[j] = enfVertex;
    }
    mapElement->enfVertexList = enfVertexList;

    resultMap[i] = mapElement;

  }
  return resultMap._retn();
}

/**
 * Returns the list of all enforced vertices
 * @return a list of enforced vertices
 *
 */
BLSURFPlugin::TEnfVertexList* BLSURFPlugin_Hypothesis_i::GetAllEnforcedVertices() {
  MESSAGE("IDL: GetAllEnforcedVertices()");
  ASSERT(myBaseImpl);
  BLSURFPlugin::TEnfVertexList_var resultMap = new BLSURFPlugin::TEnfVertexList();
  const ::BLSURFPlugin_Hypothesis::TEnfVertexList enfVertexList = this->GetImpl()->_GetAllEnforcedVertices();
  resultMap->length(enfVertexList.size());
  MESSAGE("Enforced Vertex map size is " << resultMap->length());

  ::BLSURFPlugin_Hypothesis::TEnfVertexList::const_iterator evlIt = enfVertexList.begin();
  ::BLSURFPlugin_Hypothesis::TEnfVertex *currentEnfVertex;
  for (int i = 0; evlIt != enfVertexList.end(); ++evlIt, ++i) {
    MESSAGE("Enforced Vertex #" << i);
    currentEnfVertex = (*evlIt);
    BLSURFPlugin::TEnfVertex_var enfVertex = new BLSURFPlugin::TEnfVertex();
    // Name
    enfVertex->name = CORBA::string_dup(currentEnfVertex->name.c_str());
    // Geom entry
    enfVertex->geomEntry = CORBA::string_dup(currentEnfVertex->geomEntry.c_str());
    // Coords
    BLSURFPlugin::TEnfVertexCoords_var coords = new BLSURFPlugin::TEnfVertexCoords();
    coords->length(currentEnfVertex->coords.size());
    for (int ind = 0; ind < coords->length(); ind++)
      coords[ind] = currentEnfVertex->coords[ind];
    enfVertex->coords = coords;
    // Group name
    enfVertex->grpName = CORBA::string_dup(currentEnfVertex->grpName.c_str());  
    // Face entry list
    BLSURFPlugin::TEntryList_var faceEntryList = new BLSURFPlugin::TEntryList();
    faceEntryList->length(currentEnfVertex->faceEntries.size());
    ::BLSURFPlugin_Hypothesis::TEntryList::const_iterator it_entry = currentEnfVertex->faceEntries.begin();
    for (int ind = 0; it_entry != currentEnfVertex->faceEntries.end();++it_entry, ++ind)
      faceEntryList[ind] = CORBA::string_dup((*it_entry).c_str());
    enfVertex->faceEntries = faceEntryList;

    ostringstream msg;
    msg << "Enforced vertex: \n"
        << "Name: " << enfVertex->name << "\n";
    if (coords->length())
        msg << "Coords: " << enfVertex->coords[0] << ", " << enfVertex->coords[1] << ", " << enfVertex->coords[2] << "\n";
    msg << "Geom entry: " << enfVertex->geomEntry << "\n"
        << "Group Name: " << enfVertex->grpName;
    MESSAGE(msg.str());

    resultMap[i] = enfVertex;
  }
  return resultMap._retn();

}

/**
 * Returns the list of enforced vertices coords for a given Face entry.
 * They are the coords of the "manual" enforced vertices.
 * @return A map of Face entry / List of enforced vertices coords
 *
 */
BLSURFPlugin::TFaceEntryCoordsListMap* BLSURFPlugin_Hypothesis_i::GetAllCoordsByFace() {
  MESSAGE("IDL: GetAllCoordsByFace()");
  ASSERT(myBaseImpl);

  BLSURFPlugin::TFaceEntryCoordsListMap_var resultMap = new BLSURFPlugin::TFaceEntryCoordsListMap();

  const ::BLSURFPlugin_Hypothesis::TFaceEntryCoordsListMap entryCoordsListMap = this->GetImpl()->_GetAllCoordsByFace();
  resultMap->length(entryCoordsListMap.size());
  MESSAGE("Enforced Vertex map size is " << resultMap->length());

  ::BLSURFPlugin_Hypothesis::TEnfVertexCoordsList _coordsList;
  ::BLSURFPlugin_Hypothesis::TFaceEntryCoordsListMap::const_iterator it_entry = entryCoordsListMap.begin();
  for (int i = 0; it_entry != entryCoordsListMap.end(); ++it_entry, ++i) {
    BLSURFPlugin::TFaceEntryCoordsListMapElement_var mapElement = new BLSURFPlugin::TFaceEntryCoordsListMapElement();
    mapElement->faceEntry = CORBA::string_dup(it_entry->first.c_str());
    MESSAGE("Face Entry: " << mapElement->faceEntry);

    _coordsList = it_entry->second;
    BLSURFPlugin::TEnfVertexCoordsList_var coordsList = new BLSURFPlugin::TEnfVertexCoordsList();
    coordsList->length(_coordsList.size());
    MESSAGE("Number of coords: " << coordsList->length());

    ::BLSURFPlugin_Hypothesis::TEnfVertexCoordsList::const_iterator it_coords = _coordsList.begin();
    for (int j = 0; it_coords != _coordsList.end(); ++it_coords, ++j) {
      BLSURFPlugin::TEnfVertexCoords_var coords = new BLSURFPlugin::TEnfVertexCoords();
      coords->length((*it_coords).size());
      for (int i=0;i<coords->length();i++)
        coords[i] = (*it_coords)[i];
      coordsList[j] = coords;
      MESSAGE("Coords #" << j << ": " << coords[0] << ", " << coords[1] << ", " << coords[2]);
    }
    mapElement->coordsList = coordsList;

    resultMap[i] = mapElement;

  }
  return resultMap._retn();
}

/**
 * Returns a map of enforced vertices coords / enforced vertex.
 * They are the coords of the "manual" enforced vertices.
 */
BLSURFPlugin::TCoordsEnfVertexMap* BLSURFPlugin_Hypothesis_i::GetAllEnforcedVerticesByCoords() {
  MESSAGE("IDL: GetAllEnforcedVerticesByCoords()");
  ASSERT(myBaseImpl);

  BLSURFPlugin::TCoordsEnfVertexMap_var resultMap = new BLSURFPlugin::TCoordsEnfVertexMap();
  const ::BLSURFPlugin_Hypothesis::TCoordsEnfVertexMap coordsEnfVertexMap =
      this->GetImpl()->_GetAllEnforcedVerticesByCoords();
  resultMap->length(coordsEnfVertexMap.size());
  MESSAGE("Enforced Vertex map size is " << resultMap->length());

  ::BLSURFPlugin_Hypothesis::TCoordsEnfVertexMap::const_iterator it_coords = coordsEnfVertexMap.begin();
  ::BLSURFPlugin_Hypothesis::TEnfVertex *currentEnfVertex;
  for (int i = 0; it_coords != coordsEnfVertexMap.end(); ++it_coords, ++i) {
    MESSAGE("Enforced Vertex #" << i);
    currentEnfVertex = (it_coords->second);
    BLSURFPlugin::TCoordsEnfVertexElement_var mapElement = new BLSURFPlugin::TCoordsEnfVertexElement();
    BLSURFPlugin::TEnfVertexCoords_var coords = new BLSURFPlugin::TEnfVertexCoords();
    coords->length(it_coords->first.size());
    for (int ind=0;ind<coords->length();ind++)
      coords[ind] = it_coords->first[ind];
    mapElement->coords = coords;
    MESSAGE("Coords: " << mapElement->coords[0] << ", " << mapElement->coords[1] << ", " << mapElement->coords[2]);

    BLSURFPlugin::TEnfVertex_var enfVertex = new BLSURFPlugin::TEnfVertex();
    // Name
    enfVertex->name = CORBA::string_dup(currentEnfVertex->name.c_str());
    // Geom entry
    enfVertex->geomEntry = CORBA::string_dup(currentEnfVertex->geomEntry.c_str());
    // Coords
    BLSURFPlugin::TEnfVertexCoords_var coords2 = new BLSURFPlugin::TEnfVertexCoords();
    coords2->length(currentEnfVertex->coords.size());
    for (int ind=0;ind<coords2->length();ind++)
      coords2[ind] = currentEnfVertex->coords[ind];
    enfVertex->coords = coords2;
    // Group name
    enfVertex->grpName = CORBA::string_dup(currentEnfVertex->grpName.c_str());  
    // Face entry list
    BLSURFPlugin::TEntryList_var faceEntryList = new BLSURFPlugin::TEntryList();
    faceEntryList->length(currentEnfVertex->faceEntries.size());
    ::BLSURFPlugin_Hypothesis::TEntryList::const_iterator it_entry = currentEnfVertex->faceEntries.begin();
    for (int ind = 0; it_entry != currentEnfVertex->faceEntries.end();++it_entry, ++ind)
      faceEntryList[ind] = CORBA::string_dup((*it_entry).c_str());
    enfVertex->faceEntries = faceEntryList;
      
    mapElement->enfVertex = enfVertex;
    ostringstream msg;
    msg << "Enforced vertex: \n"
        << "Name: " << enfVertex->name << "\n";
    if (coords->length())
        msg << "Coords: " << enfVertex->coords[0] << ", " << enfVertex->coords[1] << ", " << enfVertex->coords[2] << "\n";
    msg << "Geom entry: " << enfVertex->geomEntry << "\n"
        << "Group Name: " << enfVertex->grpName;
    MESSAGE(msg.str());

    resultMap[i] = mapElement;
  }
  return resultMap._retn();
}

/**
 * Returns the list of enforced vertices entries for a given Face entry.
 * They are the geom entries of the enforced vertices based on geom shape (vertex, compound, group).
 * @return A map of Face entry / List of enforced vertices geom entries
 *
 */
BLSURFPlugin::TFaceEntryEnfVertexEntryListMap* BLSURFPlugin_Hypothesis_i::GetAllEnfVertexEntriesByFace() {
  MESSAGE("IDL: GetAllEnfVertexEntriesByFace()");
  ASSERT(myBaseImpl);

  BLSURFPlugin::TFaceEntryEnfVertexEntryListMap_var resultMap = new BLSURFPlugin::TFaceEntryEnfVertexEntryListMap();

  const ::BLSURFPlugin_Hypothesis::TFaceEntryEnfVertexEntryListMap entryEnfVertexEntryListMap =
      this->GetImpl()->_GetAllEnfVertexEntriesByFace();
  resultMap->length(entryEnfVertexEntryListMap.size());
  MESSAGE("Enforced Vertex map size is " << resultMap->length());

  ::BLSURFPlugin_Hypothesis::TEntryList _enfVertexEntryList;
  ::BLSURFPlugin_Hypothesis::TFaceEntryEnfVertexEntryListMap::const_iterator it_entry =
      entryEnfVertexEntryListMap.begin();
  for (int i = 0; it_entry != entryEnfVertexEntryListMap.end(); ++it_entry, ++i) {
    BLSURFPlugin::TFaceEntryEnfVertexEntryListMapElement_var mapElement =
        new BLSURFPlugin::TFaceEntryEnfVertexEntryListMapElement();
    mapElement->faceEntry = CORBA::string_dup(it_entry->first.c_str());
    MESSAGE("Face Entry: " << mapElement->faceEntry);

    _enfVertexEntryList = it_entry->second;
    BLSURFPlugin::TEntryList_var enfVertexEntryList = new BLSURFPlugin::TEntryList();
    enfVertexEntryList->length(_enfVertexEntryList.size());
    MESSAGE("Number of enf vertex entries: " << enfVertexEntryList->length());

    ::BLSURFPlugin_Hypothesis::TEntryList::const_iterator it_enfVertexEntry = _enfVertexEntryList.begin();
    for (int j = 0; it_enfVertexEntry != _enfVertexEntryList.end(); ++it_enfVertexEntry, ++j) {
      enfVertexEntryList[j] = CORBA::string_dup((*it_enfVertexEntry).c_str());
      MESSAGE("Enf Vertex Entry #" << j << ": " << enfVertexEntryList[j]);
    }
    mapElement->enfVertexEntryList = enfVertexEntryList;

    resultMap[i] = mapElement;

  }
  return resultMap._retn();
}

/**
 * Returns a map of enforced vertices geom entry / enforced vertex.
 * They are the geom entries of the enforced vertices defined with geom shape (vertex, compound, group).
 */
BLSURFPlugin::TEnfVertexEntryEnfVertexMap* BLSURFPlugin_Hypothesis_i::GetAllEnforcedVerticesByEnfVertexEntry() {
  MESSAGE("IDL: GetAllEnforcedVerticesByEnfVertexEntry()");
  ASSERT(myBaseImpl);

  BLSURFPlugin::TEnfVertexEntryEnfVertexMap_var resultMap = new BLSURFPlugin::TEnfVertexEntryEnfVertexMap();
  const ::BLSURFPlugin_Hypothesis::TEnfVertexEntryEnfVertexMap enfVertexEntryEnfVertexMap =
      this->GetImpl()->_GetAllEnforcedVerticesByEnfVertexEntry();
  resultMap->length(enfVertexEntryEnfVertexMap.size());
  MESSAGE("Enforced Vertex map size is " << resultMap->length());

  ::BLSURFPlugin_Hypothesis::TEnfVertexEntryEnfVertexMap::const_iterator it_enfVertexEntry = enfVertexEntryEnfVertexMap.begin();
  ::BLSURFPlugin_Hypothesis::TEnfVertex *currentEnfVertex;
  for (int i = 0; it_enfVertexEntry != enfVertexEntryEnfVertexMap.end(); ++it_enfVertexEntry, ++i) {
    MESSAGE("Enforced Vertex #" << i);
    currentEnfVertex = it_enfVertexEntry->second;
    BLSURFPlugin::TEnfVertexEntryEnfVertexElement_var mapElement = new BLSURFPlugin::TEnfVertexEntryEnfVertexElement();
    mapElement->enfVertexEntry = CORBA::string_dup(it_enfVertexEntry->first.c_str());;
    MESSAGE("Enf Vertex Entry #" << i << ": " << mapElement->enfVertexEntry);

    BLSURFPlugin::TEnfVertex_var enfVertex = new BLSURFPlugin::TEnfVertex();
    // Name
    enfVertex->name = CORBA::string_dup(currentEnfVertex->name.c_str());
    // Geom entry
    enfVertex->geomEntry = CORBA::string_dup(currentEnfVertex->geomEntry.c_str());
    // Coords
    BLSURFPlugin::TEnfVertexCoords_var coords = new BLSURFPlugin::TEnfVertexCoords();
    coords->length(currentEnfVertex->coords.size());
    for (int ind=0;ind<coords->length();ind++)
      coords[ind] = currentEnfVertex->coords[ind];
    enfVertex->coords = coords;
    // Group name
    enfVertex->grpName = CORBA::string_dup(currentEnfVertex->grpName.c_str());  
    // Face entry list
    BLSURFPlugin::TEntryList_var faceEntryList = new BLSURFPlugin::TEntryList();
    faceEntryList->length(currentEnfVertex->faceEntries.size());
    ::BLSURFPlugin_Hypothesis::TEntryList::const_iterator it_entry = currentEnfVertex->faceEntries.begin();
    for (int ind = 0; it_entry != currentEnfVertex->faceEntries.end();++it_entry, ++ind)
      faceEntryList[ind] = CORBA::string_dup((*it_entry).c_str());
    enfVertex->faceEntries = faceEntryList;

    ostringstream msg;
    msg << "Enforced vertex: \n"
        << "Name: " << enfVertex->name << "\n";
    if (coords->length())
        msg << "Coords: " << enfVertex->coords[0] << ", " << enfVertex->coords[1] << ", " << enfVertex->coords[2] << "\n";
    msg << "Geom entry: " << enfVertex->geomEntry << "\n"
        << "Group Name: " << enfVertex->grpName;
    MESSAGE(msg.str());

    mapElement->enfVertex = enfVertex;
    resultMap[i] = mapElement;
  }
  return resultMap._retn();
}

/**
 * Erase all enforced vertices
 */
void BLSURFPlugin_Hypothesis_i::ClearAllEnforcedVertices() {
  ASSERT(myBaseImpl);
  this->GetImpl()->ClearAllEnforcedVertices();
  SMESH::TPythonDump() << _this() << ".ClearAllEnforcedVertices()";
}

/*!
 * Set/get/unset an enforced vertex on face
 */
bool BLSURFPlugin_Hypothesis_i::SetEnforcedVertex(GEOM::GEOM_Object_ptr theFace, CORBA::Double x, CORBA::Double y,
    CORBA::Double z) throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);

  if ((theFace->GetShapeType() != GEOM::FACE) && (theFace->GetShapeType() != GEOM::COMPOUND)) {
    MESSAGE("theFace shape type is not FACE or COMPOUND");
    THROW_SALOME_CORBA_EXCEPTION("theFace shape type is not FACE or COMPOUND", SALOME::BAD_PARAM);
  }

  string theFaceEntry = theFace->GetStudyEntry();
  
  if (theFaceEntry.empty()) {
    GEOM::GEOM_Gen_ptr geomGen = SMESH_Gen_i::GetGeomEngine();
    SMESH_Gen_i *smeshGen = SMESH_Gen_i::GetSMESHGen();
    string aName;
    if (theFace->GetShapeType() == GEOM::FACE)
      aName = "Face_";
    if (theFace->GetShapeType() == GEOM::COMPOUND)
      aName = "Compound_";
    aName += theFace->GetEntry();
    SALOMEDS::SObject_ptr theSFace = geomGen->PublishInStudy(smeshGen->GetCurrentStudy(), NULL, theFace, aName.c_str());
    if (!theSFace->_is_nil())
      theFaceEntry = theSFace->GetID();
  }
  if (theFaceEntry.empty())
    THROW_SALOME_CORBA_EXCEPTION( "Geom object is not published in study" ,SALOME::BAD_PARAM );
  MESSAGE("IDL : GetName : " << theFace->GetName());
  MESSAGE("IDL : SetEnforcedVertex( "<< theFaceEntry << ", " << x << ", " << y << ", " << z << ")");
  try {
    return SetEnforcedVertexEntry(theFaceEntry.c_str(), x, y, z);
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
}

/*!
 * Set/get/unset an enforced vertex with name on face
 */
bool BLSURFPlugin_Hypothesis_i::SetEnforcedVertexNamed(GEOM::GEOM_Object_ptr theFace, CORBA::Double x, CORBA::Double y,
    CORBA::Double z, const char* theVertexName) throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);

  if ((theFace->GetShapeType() != GEOM::FACE) && (theFace->GetShapeType() != GEOM::COMPOUND)) {
    MESSAGE("theFace shape type is not FACE or COMPOUND");
    THROW_SALOME_CORBA_EXCEPTION("theFace shape type is not FACE or COMPOUND", SALOME::BAD_PARAM);
  }

  string theFaceEntry = theFace->GetStudyEntry();
  
  if (theFaceEntry.empty()) {
    GEOM::GEOM_Gen_ptr geomGen = SMESH_Gen_i::GetGeomEngine();
    SMESH_Gen_i *smeshGen = SMESH_Gen_i::GetSMESHGen();
    string aName;
    if (theFace->GetShapeType() == GEOM::FACE)
      aName = "Face_";
    if (theFace->GetShapeType() == GEOM::COMPOUND)
      aName = "Compound_";
    aName += theFace->GetEntry();
    SALOMEDS::SObject_ptr theSFace = geomGen->PublishInStudy(smeshGen->GetCurrentStudy(), NULL, theFace, aName.c_str());
    if (!theSFace->_is_nil())
      theFaceEntry = theSFace->GetID();
  }
  if (theFaceEntry.empty())
    THROW_SALOME_CORBA_EXCEPTION( "Geom object is not published in study" ,SALOME::BAD_PARAM );
  
  MESSAGE("IDL : GetName : " << theFace->GetName());
  MESSAGE("IDL : SetEnforcedVertexNamed( "<< theFaceEntry << ", " << x << ", " << y << ", " << z << ", " << theVertexName << ")");
  try {
    return SetEnforcedVertexEntry(theFaceEntry.c_str(), x, y, z, theVertexName);
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
}

/*!
 * Set/get/unset an enforced vertex with geom object on face
 */
bool BLSURFPlugin_Hypothesis_i::SetEnforcedVertexGeom(GEOM::GEOM_Object_ptr theFace, GEOM::GEOM_Object_ptr theVertex)
    throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);

  if ((theFace->GetShapeType() != GEOM::FACE) && (theFace->GetShapeType() != GEOM::COMPOUND)) {
    MESSAGE("theFace shape type is not FACE or COMPOUND");
    THROW_SALOME_CORBA_EXCEPTION("theFace shape type is not FACE or COMPOUND", SALOME::BAD_PARAM);
  }

  if ((theVertex->GetShapeType() != GEOM::VERTEX) && (theVertex->GetShapeType() != GEOM::COMPOUND)) {
    MESSAGE("theVertex shape type is not VERTEX or COMPOUND");
    THROW_SALOME_CORBA_EXCEPTION("theVertex shape type is not VERTEX or COMPOUND", SALOME::BAD_PARAM);
  }

  //  GEOM::GEOM_Gen_ptr geomGen = SMESH_Gen_i::GetGeomEngine();
  //  GEOM::GEOM_IMeasureOperations_var measureOp = geomGen->GetIMeasureOperations(this->GetImpl()->GetStudyId());
  //  if (CORBA::is_nil(measureOp))
  //    return false;
  //
  //  CORBA::Double x, y, z;
  //  x = y = z = 0.;
  //  measureOp->PointCoordinates(theVertex, x, y, z);

  string theFaceEntry = theFace->GetStudyEntry();
  string theVertexEntry = theVertex->GetStudyEntry();
  
  GEOM::GEOM_Gen_ptr geomGen = SMESH_Gen_i::GetGeomEngine();
  SMESH_Gen_i *smeshGen = SMESH_Gen_i::GetSMESHGen();
  string aName;
  
  if (theFaceEntry.empty()) {
    if (theFace->GetShapeType() == GEOM::FACE)
      aName = "Face_";
    if (theFace->GetShapeType() == GEOM::COMPOUND)
      aName = "Compound_";
    aName += theFace->GetEntry();
    SALOMEDS::SObject_ptr theSFace = geomGen->PublishInStudy(smeshGen->GetCurrentStudy(), NULL, theFace, aName.c_str());
    if (!theSFace->_is_nil())
      theFaceEntry = theSFace->GetID();
  }
  if (theFaceEntry.empty())
    THROW_SALOME_CORBA_EXCEPTION( "Geom object is not published in study" ,SALOME::BAD_PARAM );
  
  if (theVertexEntry.empty()) {
    if (theVertex->GetShapeType() == GEOM::VERTEX)
      aName = "Vertex_";
    if (theVertex->GetShapeType() == GEOM::COMPOUND)
      aName = "Compound_";
    aName += theVertex->GetEntry();
    SALOMEDS::SObject_ptr theSVertex = geomGen->PublishInStudy(smeshGen->GetCurrentStudy(), NULL, theVertex, aName.c_str());
    if (!theSVertex->_is_nil())
      theVertexEntry = theSVertex->GetID();
  }
  if (theVertexEntry.empty())
    THROW_SALOME_CORBA_EXCEPTION( "Geom object is not published in study" ,SALOME::BAD_PARAM );

  string theVertexName = theVertex->GetName();
  MESSAGE("IDL : theFace->GetName : " << theFace->GetName());
  MESSAGE("IDL : theVertex->GetName : " << theVertexName);
  MESSAGE("IDL : SetEnforcedVertexGeom( "<< theFaceEntry << ", " << theVertexEntry<< ")");
  try {
    return SetEnforcedVertexEntry(theFaceEntry.c_str(), 0, 0, 0, theVertexName.c_str(), theVertexEntry.c_str());
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
}

/*!
 * Set an enforced vertex with group name on face
 */
bool BLSURFPlugin_Hypothesis_i::SetEnforcedVertexWithGroup(GEOM::GEOM_Object_ptr theFace, CORBA::Double x, CORBA::Double y, CORBA::Double z, const char* theGroupName)
 throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);

  if ((theFace->GetShapeType() != GEOM::FACE) && (theFace->GetShapeType() != GEOM::COMPOUND)) {
    MESSAGE("theFace shape type is not FACE or COMPOUND");
    THROW_SALOME_CORBA_EXCEPTION("theFace shape type is not FACE or COMPOUND", SALOME::BAD_PARAM);
  }

  string theFaceEntry = theFace->GetStudyEntry();
  
  if (theFaceEntry.empty()) {
    GEOM::GEOM_Gen_ptr geomGen = SMESH_Gen_i::GetGeomEngine();
    SMESH_Gen_i *smeshGen = SMESH_Gen_i::GetSMESHGen();
    string aName;
    if (theFace->GetShapeType() == GEOM::FACE)
      aName = "Face_";
    if (theFace->GetShapeType() == GEOM::COMPOUND)
      aName = "Compound_";
    aName += theFace->GetEntry();
    SALOMEDS::SObject_ptr theSFace = geomGen->PublishInStudy(smeshGen->GetCurrentStudy(), NULL, theFace, aName.c_str());
    if (!theSFace->_is_nil())
      theFaceEntry = theSFace->GetID();
  }
  if (theFaceEntry.empty())
    THROW_SALOME_CORBA_EXCEPTION( "Geom object is not published in study" ,SALOME::BAD_PARAM );
  MESSAGE("IDL : GetName : " << theFace->GetName());
  MESSAGE("IDL : SetEnforcedVertexWithGroup( "<< theFaceEntry << ", " << x << ", " << y << ", " << z << ", " << theGroupName << ")");
  try {
    return SetEnforcedVertexEntry(theFaceEntry.c_str(), x, y, z, "", "", theGroupName);
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
}

/*!
 * Set an enforced vertex with name and group name on face
 */
bool BLSURFPlugin_Hypothesis_i::SetEnforcedVertexNamedWithGroup(GEOM::GEOM_Object_ptr theFace, CORBA::Double x, CORBA::Double y, CORBA::Double z, 
                                                                const char* theVertexName, const char* theGroupName)
 throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);

  if ((theFace->GetShapeType() != GEOM::FACE) && (theFace->GetShapeType() != GEOM::COMPOUND)) {
    MESSAGE("theFace shape type is not FACE or COMPOUND");
    THROW_SALOME_CORBA_EXCEPTION("theFace shape type is not FACE or COMPOUND", SALOME::BAD_PARAM);
  }

  string theFaceEntry = theFace->GetStudyEntry();
  
  if (theFaceEntry.empty()) {
    GEOM::GEOM_Gen_ptr geomGen = SMESH_Gen_i::GetGeomEngine();
    SMESH_Gen_i *smeshGen = SMESH_Gen_i::GetSMESHGen();
    string aName;
    if (theFace->GetShapeType() == GEOM::FACE)
      aName = "Face_";
    if (theFace->GetShapeType() == GEOM::COMPOUND)
      aName = "Compound_";
    aName += theFace->GetEntry();
    SALOMEDS::SObject_ptr theSFace = geomGen->PublishInStudy(smeshGen->GetCurrentStudy(), NULL, theFace, aName.c_str());
    if (!theSFace->_is_nil())
      theFaceEntry = theSFace->GetID();
  }
  if (theFaceEntry.empty())
    THROW_SALOME_CORBA_EXCEPTION( "Geom object is not published in study" ,SALOME::BAD_PARAM );
  MESSAGE("IDL : GetName : " << theFace->GetName());
  MESSAGE("IDL : SetEnforcedVertexNamedWithGroup( "<< theFaceEntry << ", " << x << ", " << y << ", " << z << ", " << theVertexName << ", " << theGroupName << ")");
  try {
    return SetEnforcedVertexEntry(theFaceEntry.c_str(), x, y, z, theVertexName, "", theGroupName);
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
}

/*!
 * Set an enforced vertex with geom entry and group name on face
 */
bool BLSURFPlugin_Hypothesis_i::SetEnforcedVertexGeomWithGroup(GEOM::GEOM_Object_ptr theFace, GEOM::GEOM_Object_ptr theVertex, const char* theGroupName)
 throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);

  if ((theFace->GetShapeType() != GEOM::FACE) && (theFace->GetShapeType() != GEOM::COMPOUND)) {
    MESSAGE("theFace shape type is not FACE or COMPOUND");
    THROW_SALOME_CORBA_EXCEPTION("theFace shape type is not FACE or COMPOUND", SALOME::BAD_PARAM);
  }

  if ((theVertex->GetShapeType() != GEOM::VERTEX) && (theVertex->GetShapeType() != GEOM::COMPOUND)) {
    MESSAGE("theVertex shape type is not VERTEX or COMPOUND");
    THROW_SALOME_CORBA_EXCEPTION("theVertex shape type is not VERTEX or COMPOUND", SALOME::BAD_PARAM);
  }

  string theFaceEntry = theFace->GetStudyEntry();
  string theVertexEntry = theVertex->GetStudyEntry();
  
  GEOM::GEOM_Gen_ptr geomGen = SMESH_Gen_i::GetGeomEngine();
  SMESH_Gen_i *smeshGen = SMESH_Gen_i::GetSMESHGen();
  string aName;
  
  if (theFaceEntry.empty()) {
    if (theFace->GetShapeType() == GEOM::FACE)
      aName = "Face_";
    if (theFace->GetShapeType() == GEOM::COMPOUND)
      aName = "Compound_";
    aName += theFace->GetEntry();
    SALOMEDS::SObject_ptr theSFace = geomGen->PublishInStudy(smeshGen->GetCurrentStudy(), NULL, theFace, aName.c_str());
    if (!theSFace->_is_nil())
      theFaceEntry = theSFace->GetID();
  }
  if (theFaceEntry.empty())
    THROW_SALOME_CORBA_EXCEPTION( "Geom object is not published in study" ,SALOME::BAD_PARAM );
  
  if (theVertexEntry.empty()) {
    if (theVertex->GetShapeType() == GEOM::VERTEX)
      aName = "Vertex_";
    if (theVertex->GetShapeType() == GEOM::COMPOUND)
      aName = "Compound_";
    aName += theVertex->GetEntry();
    SALOMEDS::SObject_ptr theSVertex = geomGen->PublishInStudy(smeshGen->GetCurrentStudy(), NULL, theVertex, aName.c_str());
    if (!theSVertex->_is_nil())
      theVertexEntry = theSVertex->GetID();
  }
  if (theVertexEntry.empty())
    THROW_SALOME_CORBA_EXCEPTION( "Geom object is not published in study" ,SALOME::BAD_PARAM );

  string theVertexName = theVertex->GetName();
  MESSAGE("IDL : theFace->GetName : " << theFace->GetName());
  MESSAGE("IDL : theVertex->GetName : " << theVertexName);
  MESSAGE("IDL : SetEnforcedVertexGeomWithGroup( "<< theFaceEntry << ", " << theVertexEntry<< ", " << theGroupName<< ")");
  try {
    return SetEnforcedVertexEntry(theFaceEntry.c_str(), 0, 0, 0, theVertexName.c_str(), theVertexEntry.c_str(), theGroupName);
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
}

//Enable internal enforced vertices on specific face if requested by user
///*!
// * Are internal enforced vertices used for a face ?
// */
//CORBA::Boolean BLSURFPlugin_Hypothesis_i::GetInternalEnforcedVertex(GEOM::GEOM_Object_ptr theFace)
//    throw (SALOME::SALOME_Exception) {
//  ASSERT(myBaseImpl);

//  if ((theFace->GetShapeType() != GEOM::FACE) && (theFace->GetShapeType() != GEOM::COMPOUND)) {
//    MESSAGE("theFace shape type is not FACE or COMPOUND");
//    THROW_SALOME_CORBA_EXCEPTION("theFace shape type is not FACE or COMPOUND", SALOME::BAD_PARAM);
//  }

//  string theFaceEntry = theFace->GetStudyEntry();
  
//  if (theFaceEntry.empty()) {
//    GEOM::GEOM_Gen_ptr geomGen = SMESH_Gen_i::GetGeomEngine();
//    SMESH_Gen_i *smeshGen = SMESH_Gen_i::GetSMESHGen();
//    string aName;
//    if (theFace->GetShapeType() == GEOM::FACE)
//      aName = "Face_";
//    if (theFace->GetShapeType() == GEOM::COMPOUND)
//      aName = "Compound_";
//    aName += theFace->GetEntry();
//    SALOMEDS::SObject_ptr theSFace = geomGen->PublishInStudy(smeshGen->GetCurrentStudy(), NULL, theFace, aName.c_str());
//    if (!theSFace->_is_nil())
//      theFaceEntry = theSFace->GetID();
//  }
//  if (theFaceEntry.empty())
//    THROW_SALOME_CORBA_EXCEPTION( "Geom object is not published in study" ,SALOME::BAD_PARAM );

//  MESSAGE("IDL : GetName : " << theFace->GetName());
//  MESSAGE("IDL : GetInternalEnforcedVertexEntry ( "<< theFaceEntry << ")");
//  try {
//    return GetInternalEnforcedVertexEntry(theFaceEntry.c_str());
//  } catch (SALOME_Exception& ex) {
//    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
//  }
//}

/*!
 * Get the list of all enforced vertices
 */
BLSURFPlugin::TEnfVertexList* BLSURFPlugin_Hypothesis_i::GetEnforcedVertices(GEOM::GEOM_Object_ptr theFace)
    throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);

  if ((theFace->GetShapeType() != GEOM::FACE) && (theFace->GetShapeType() != GEOM::COMPOUND)) {
    MESSAGE("theFace shape type is not FACE or COMPOUND");
    THROW_SALOME_CORBA_EXCEPTION("theFace shape type is not FACE or COMPOUND", SALOME::BAD_PARAM);
  }

  string theFaceEntry = theFace->GetStudyEntry();
  
  if (theFaceEntry.empty()) {
    GEOM::GEOM_Gen_ptr geomGen = SMESH_Gen_i::GetGeomEngine();
    SMESH_Gen_i *smeshGen = SMESH_Gen_i::GetSMESHGen();
    string aName;
    if (theFace->GetShapeType() == GEOM::FACE)
      aName = "Face_";
    if (theFace->GetShapeType() == GEOM::COMPOUND)
      aName = "Compound_";
    aName += theFace->GetEntry();
    SALOMEDS::SObject_ptr theSFace = geomGen->PublishInStudy(smeshGen->GetCurrentStudy(), NULL, theFace, aName.c_str());
    if (!theSFace->_is_nil())
      theFaceEntry = theSFace->GetID();
  }
  if (theFaceEntry.empty())
    THROW_SALOME_CORBA_EXCEPTION( "Geom object is not published in study" ,SALOME::BAD_PARAM );

  MESSAGE("IDL : GetName : " << theFace->GetName());
  MESSAGE("IDL : GetEnforcedVerticesEntry ( "<< theFaceEntry << ")");
  try {
    return GetEnforcedVerticesEntry(theFaceEntry.c_str());
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
}

bool BLSURFPlugin_Hypothesis_i::UnsetEnforcedVertex(GEOM::GEOM_Object_ptr theFace, CORBA::Double x, CORBA::Double y,
    CORBA::Double z) throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);

  if ((theFace->GetShapeType() != GEOM::FACE) && (theFace->GetShapeType() != GEOM::COMPOUND)) {
    MESSAGE("theFace shape type is not FACE or COMPOUND");
    THROW_SALOME_CORBA_EXCEPTION("theFace shape type is not FACE or COMPOUND", SALOME::BAD_PARAM);
  }

  string theFaceEntry = theFace->GetStudyEntry();
  
  if (theFaceEntry.empty()) {
    GEOM::GEOM_Gen_ptr geomGen = SMESH_Gen_i::GetGeomEngine();
    SMESH_Gen_i *smeshGen = SMESH_Gen_i::GetSMESHGen();
    string aName;
    if (theFace->GetShapeType() == GEOM::FACE)
      aName = "Face_";
    if (theFace->GetShapeType() == GEOM::COMPOUND)
      aName = "Compound_";
    aName += theFace->GetEntry();
    SALOMEDS::SObject_ptr theSFace = geomGen->PublishInStudy(smeshGen->GetCurrentStudy(), NULL, theFace, aName.c_str());
    if (!theSFace->_is_nil())
      theFaceEntry = theSFace->GetID();
  }
  if (theFaceEntry.empty())
    THROW_SALOME_CORBA_EXCEPTION( "Geom object is not published in study" ,SALOME::BAD_PARAM );
  MESSAGE("IDL : GetName : " << theFace->GetName());
  MESSAGE("IDL : UnsetEnforcedVertex ( "<< theFaceEntry << ", " << x << ", " << y << ", " << z << ")");

  try {
    return UnsetEnforcedVertexEntry(theFaceEntry.c_str(), x, y, z);
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
}

bool BLSURFPlugin_Hypothesis_i::UnsetEnforcedVertexGeom(GEOM::GEOM_Object_ptr theFace, GEOM::GEOM_Object_ptr theVertex)
    throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);

  if ((theFace->GetShapeType() != GEOM::FACE) && (theFace->GetShapeType() != GEOM::COMPOUND)) {
    MESSAGE("theFace shape type is not FACE or COMPOUND");
    THROW_SALOME_CORBA_EXCEPTION("theFace shape type is not FACE or COMPOUND", SALOME::BAD_PARAM);
  }
  if ((theVertex->GetShapeType() != GEOM::VERTEX) && (theVertex->GetShapeType() != GEOM::COMPOUND)) {
    MESSAGE("theVertex shape type is not VERTEX or COMPOUND");
    THROW_SALOME_CORBA_EXCEPTION("theVertex shape type is not VERTEX or COMPOUND", SALOME::BAD_PARAM);
  }

  //  GEOM::GEOM_Gen_ptr geomGen = SMESH_Gen_i::GetGeomEngine();
  //  GEOM::GEOM_IMeasureOperations_var measureOp = geomGen->GetIMeasureOperations(this->GetImpl()->GetStudyId());
  //  if (CORBA::is_nil(measureOp))
  //    return false;
  //
  //  CORBA::Double x, y, z;
  //  x = y = z = 0.;
  //  measureOp->PointCoordinates(theVertex, x, y, z);

  std::string theFaceEntry = theFace->GetStudyEntry();
  std::string theVertexEntry = theVertex->GetStudyEntry();
  
  GEOM::GEOM_Gen_ptr geomGen = SMESH_Gen_i::GetGeomEngine();
  SMESH_Gen_i *smeshGen = SMESH_Gen_i::GetSMESHGen();
  string aName;
  
  if (theFaceEntry.empty()) {
    if (theFace->GetShapeType() == GEOM::FACE)
      aName = "Face_";
    if (theFace->GetShapeType() == GEOM::COMPOUND)
      aName = "Compound_";
    aName += theFace->GetEntry();
    SALOMEDS::SObject_ptr theSFace = geomGen->PublishInStudy(smeshGen->GetCurrentStudy(), NULL, theFace, aName.c_str());
    if (!theSFace->_is_nil())
      theFaceEntry = theSFace->GetID();
  }
  if (theFaceEntry.empty())
    THROW_SALOME_CORBA_EXCEPTION( "Geom object is not published in study" ,SALOME::BAD_PARAM );
  
  if (theVertexEntry.empty()) {
    if (theVertex->GetShapeType() == GEOM::VERTEX)
      aName = "Vertex_";
    if (theVertex->GetShapeType() == GEOM::COMPOUND)
      aName = "Compound_";
    aName += theVertex->GetEntry();
    SALOMEDS::SObject_ptr theSVertex = geomGen->PublishInStudy(smeshGen->GetCurrentStudy(), NULL, theVertex, aName.c_str());
    if (!theSVertex->_is_nil())
      theVertexEntry = theSVertex->GetID();
  }
  if (theVertexEntry.empty())
    THROW_SALOME_CORBA_EXCEPTION( "Geom object is not published in study" ,SALOME::BAD_PARAM );
  
  MESSAGE("IDL : UnsetEnforcedVertexGeom ( "<< theFaceEntry << ", " << theVertexEntry << ")");

  try {
    return UnsetEnforcedVertexEntry(theFaceEntry.c_str(), 0, 0, 0, theVertexEntry.c_str());
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
}

bool BLSURFPlugin_Hypothesis_i::UnsetEnforcedVertices(GEOM::GEOM_Object_ptr theFace) throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);

  if ((theFace->GetShapeType() != GEOM::FACE) && (theFace->GetShapeType() != GEOM::COMPOUND)) {
    MESSAGE("theFace shape type is not FACE or COMPOUND");
    THROW_SALOME_CORBA_EXCEPTION("theFace shape type is not FACE or COMPOUND", SALOME::BAD_PARAM);
  }

  string theFaceEntry = theFace->GetStudyEntry();
  
  if (theFaceEntry.empty()) {
    GEOM::GEOM_Gen_ptr geomGen = SMESH_Gen_i::GetGeomEngine();
    SMESH_Gen_i *smeshGen = SMESH_Gen_i::GetSMESHGen();
    string aName;
    if (theFace->GetShapeType() == GEOM::FACE)
      aName = "Face_";
    if (theFace->GetShapeType() == GEOM::COMPOUND)
      aName = "Compound_";
    aName += theFace->GetEntry();
    SALOMEDS::SObject_ptr theSFace = geomGen->PublishInStudy(smeshGen->GetCurrentStudy(), NULL, theFace, aName.c_str());
    if (!theSFace->_is_nil())
      theFaceEntry = theSFace->GetID();
  }
  if (theFaceEntry.empty())
    THROW_SALOME_CORBA_EXCEPTION( "Geom object is not published in study" ,SALOME::BAD_PARAM );
  
  MESSAGE("IDL : GetName : " << theFace->GetName());
  MESSAGE("IDL : UnsetEnforcedVertices ( "<< theFaceEntry << ")");

  try {
    return UnsetEnforcedVerticesEntry(theFaceEntry.c_str());
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
}

/*!
 * Set/get/unset an enforced vertex on geom object given by entry
 */
bool BLSURFPlugin_Hypothesis_i::SetEnforcedVertexEntry(const char* theFaceEntry, CORBA::Double x, CORBA::Double y,
    CORBA::Double z, const char* theVertexName, const char* theVertexEntry, const char* theGroupName)
    throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);
  MESSAGE("IDL : SetEnforcedVertexEntry(" << theFaceEntry << ", " << x << ", " << y << ", " << z << ", \"" << theVertexName << "\", \"" << theVertexEntry << "\", \"" << theGroupName << "\")");
  bool newValue = false;
  if (string(theVertexEntry).empty()) {
    try {
      ::BLSURFPlugin_Hypothesis::TEnfVertexCoordsList coordsList =
          this->GetImpl()->GetEnfVertexCoordsList(theFaceEntry);
      ::BLSURFPlugin_Hypothesis::TEnfVertexCoords coords;
      coords.push_back(x);
      coords.push_back(y);
      coords.push_back(z);
      if (coordsList.find(coords) == coordsList.end()) {
        MESSAGE("Coords not found: add it in coordsList");
        newValue = true;
      } else {
        MESSAGE("Coords already found, compare names");
        ::BLSURFPlugin_Hypothesis::TEnfVertex *enfVertex = this->GetImpl()->GetEnfVertex(coords);
        if ((enfVertex->name != theVertexName) || (enfVertex->grpName != theGroupName)) {
          MESSAGE("The names are different: update");
//          this->GetImpl()->ClearEnforcedVertex(theFaceEntry, x, y, z);
          newValue = true;
        }
        else {
          MESSAGE("The names are identical");
        }
      }
    } catch (const std::invalid_argument& ex) {
      // no enforced vertex for entry
      MESSAGE("Face entry not found : add it to the list");
      newValue = true;
    }
    if (newValue)
      if (string(theVertexName).empty())
        if (string(theGroupName).empty())
          SMESH::TPythonDump() << _this() << ".SetEnforcedVertex(" << theFaceEntry << ", " << x << ", " << y << ", " << z << ")";
        else
          SMESH::TPythonDump() << _this() << ".SetEnforcedVertexWithGroup(" << theFaceEntry << ", " << x << ", " << y << ", " << z << ", \"" << theGroupName << "\")";
      else
        if (string(theGroupName).empty())
          SMESH::TPythonDump() << _this() << ".SetEnforcedVertexNamed(" << theFaceEntry << ", " << x << ", " << y << ", " << z << ", \"" << theVertexName << "\")";
        else
          SMESH::TPythonDump() << _this() << ".SetEnforcedVertexNamedWithGroup(" << theFaceEntry << ", " << x << ", " << y << ", " << z << ", \"" 
                                          << theVertexName << "\", \"" << theGroupName << "\")";
  } else {
    try {
      ::BLSURFPlugin_Hypothesis::TEntryList enfVertexEntryList = this->GetImpl()->GetEnfVertexEntryList(theFaceEntry);
      ::BLSURFPlugin_Hypothesis::TEntryList::const_iterator it = enfVertexEntryList.find(theVertexEntry);
      if ( it == enfVertexEntryList.end()) {
        MESSAGE("Geom entry not found: add it in enfVertexEntryList");
        newValue = true;
      }
      else {
        MESSAGE("Geom entry already found, compare names");
        ::BLSURFPlugin_Hypothesis::TEnfVertex *enfVertex = this->GetImpl()->GetEnfVertex((*it));
        if ((enfVertex->name != theVertexName) || (enfVertex->grpName != theGroupName)) {
          MESSAGE("The names are different: update");
//          this->GetImpl()->ClearEnforcedVertex(theFaceEntry, x, y, z);
          newValue = true;
        }
        else {
          MESSAGE("The names are identical");
        }
      }
    } catch (const std::invalid_argument& ex) {
      // no enforced vertex for entry
      MESSAGE("Face entry not found : add it to the list");
      newValue = true;
    }
    if (newValue)
        if (string(theGroupName).empty())
          SMESH::TPythonDump() << _this() << ".SetEnforcedVertexGeom(" << theFaceEntry << ", " << theVertexEntry << ")";
        else
          SMESH::TPythonDump() << _this() << ".SetEnforcedVertexGeomWithGroup(" << theFaceEntry << ", " << theVertexEntry << ", \"" << theGroupName << "\")";
  }

  if (newValue)
    this->GetImpl()->SetEnforcedVertex(theFaceEntry, theVertexName, theVertexEntry, theGroupName, x, y, z);

  MESSAGE("IDL : SetEnforcedVertexEntry END");
  return newValue;
}

//Enable internal enforced vertices on specific face if requested by user
//CORBA::Boolean BLSURFPlugin_Hypothesis_i::GetInternalEnforcedVertexEntry(const char* theFaceEntry)
//    throw (SALOME::SALOME_Exception) {
//  ASSERT(myBaseImpl);
//  try {
//    return this->GetImpl()->GetInternalEnforcedVertex(theFaceEntry);
//  } catch (const std::exception& ex) {
//    std::cout << "Exception: " << ex.what() << std::endl;
//    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
//  }
//}
  
BLSURFPlugin::TEnfVertexList* BLSURFPlugin_Hypothesis_i::GetEnforcedVerticesEntry(const char* entry)
    throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);
  MESSAGE("ENGINE : GetEnforcedVerticesEntry START ENTRY : " << entry);

  try {
    BLSURFPlugin::TEnfVertexList_var vertexList = new BLSURFPlugin::TEnfVertexList();
    ::BLSURFPlugin_Hypothesis::TEnfVertexList _vList = this->GetImpl()->GetEnfVertexList(entry);
    vertexList->length(_vList.size());
    MESSAGE("Number of enforced vertices: " << vertexList->length());
    ::BLSURFPlugin_Hypothesis::TEnfVertexList::const_iterator evlIt = _vList.begin();
    for (int i = 0; evlIt != _vList.end(); ++evlIt, ++i) {
      ::BLSURFPlugin_Hypothesis::TEnfVertex *_enfVertex = (*evlIt);

      BLSURFPlugin::TEnfVertex_var enfVertex = new BLSURFPlugin::TEnfVertex();

      // Name
      enfVertex->name = CORBA::string_dup(_enfVertex->name.c_str());
      // Geom Vertex Entry
      enfVertex->geomEntry = CORBA::string_dup(_enfVertex->geomEntry.c_str());
      // Coords
      BLSURFPlugin::TEnfVertexCoords_var coords = new BLSURFPlugin::TEnfVertexCoords();
      coords->length(_enfVertex->coords.size());
      for (int ind=0;ind<coords->length();ind++)
        coords[ind] = _enfVertex->coords[ind];
      enfVertex->coords = coords;
      // Group Name
      enfVertex->grpName = CORBA::string_dup(_enfVertex->grpName.c_str());
      // Face entry list
      BLSURFPlugin::TEntryList_var faceEntryList = new BLSURFPlugin::TEntryList();
      faceEntryList->length(_enfVertex->faceEntries.size());
      ::BLSURFPlugin_Hypothesis::TEntryList::const_iterator it_entry = _enfVertex->faceEntries.begin();
      for (int ind = 0; it_entry != _enfVertex->faceEntries.end();++it_entry, ++ind)
        faceEntryList[ind] = CORBA::string_dup((*it_entry).c_str());
      enfVertex->faceEntries = faceEntryList;

      vertexList[i] = enfVertex;
    }
    MESSAGE("ENGINE : GetEnforcedVerticesEntry END ENTRY : " << entry);
    return vertexList._retn();
  } catch (const std::invalid_argument& ex) {
    SALOME::ExceptionStruct ExDescription;
    ExDescription.text = ex.what();
    ExDescription.type = SALOME::BAD_PARAM;
    ExDescription.sourceFile = "BLSURFPlugin_Hypothesis_i::GetEnforcedVerticesEntry(entry)";
    ExDescription.lineNumber = 1385;
    throw SALOME::SALOME_Exception(ExDescription);
  } catch (const std::exception& ex) {
    std::cout << "Exception: " << ex.what() << std::endl;
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
}

bool BLSURFPlugin_Hypothesis_i::UnsetEnforcedVertexEntry(const char* theFaceEntry, CORBA::Double x, CORBA::Double y,
    CORBA::Double z, const char* theVertexEntry) throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);
  MESSAGE("IDL : UnsetEnforcedVertexEntry(" << theFaceEntry << "," << x << "," << y << "," << z << ", " << theVertexEntry << ")");

  bool res = false;
  try {
    res = this->GetImpl()->ClearEnforcedVertex(theFaceEntry, x, y, z, theVertexEntry);

    if (string(theVertexEntry).empty())
      SMESH::TPythonDump() << "isDone = " << _this() << ".UnsetEnforcedVertex(" << theFaceEntry << ", " << x << ", " << y << ", " << z
          << ")";
    else
      SMESH::TPythonDump() << "isDone = " << _this() << ".UnsetEnforcedVertexGeom(" << theFaceEntry << ", " << theVertexEntry << ")";

  } catch (const std::invalid_argument& ex) {
    return false;
  } catch (const std::exception& ex) {
    std::cout << "Exception: " << ex.what() << std::endl;
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }

  MESSAGE("ENGINE : UnsetEnforcedVertexEntry END ENTRY : " << theFaceEntry);
  return res;
}

//bool BLSURFPlugin_Hypothesis_i::UnsetEnforcedVertexEntryWithPoint(const char* theFaceEntry, const char* theVertexEntry,
//    CORBA::Double x, CORBA::Double y, CORBA::Double z) throw (SALOME::SALOME_Exception) {
//  MESSAGE("IDL : UnsetEnforcedVertexEntryWithPoint START theFaceEntry=" << theFaceEntry << ", theVertexEntry=" << theVertexEntry);
//
//  bool ret = false;
//
//  try {
//    ret = _unsetEnfVertex(theFaceEntry, x, y, z);
//  } catch (SALOME_Exception& ex) {
//    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
//  }
//
//  if (ret)
//    SMESH::TPythonDump() << _this() << ".UnsetEnforcedVertexWithPoint(" << theFaceEntry << ", " << theVertexEntry
//        << ")";
//
//  MESSAGE("IDL : UnsetEnforcedVertexEntryWithPoint END ENTRY : " << theFaceEntry);
//  return ret;
//}

bool BLSURFPlugin_Hypothesis_i::UnsetEnforcedVerticesEntry(const char* theFaceEntry) throw (SALOME::SALOME_Exception) {
  ASSERT(myBaseImpl);
  MESSAGE("IDL : UnsetEnforcedVerticesEntry(" << theFaceEntry << ")");

  try {
    this->GetImpl()->ClearEnforcedVertices(theFaceEntry);
    SMESH::TPythonDump() << _this() << ".UnsetEnforcedVertices(" << theFaceEntry << ")";
  } catch (const std::invalid_argument& ex) {
    return false;
  } catch (const std::exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }

  MESSAGE("IDL : UnsetEnforcedVerticesEntry END ENTRY : " << theFaceEntry);
  return true;
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::SetInternalEnforcedVertexAllFaces
 *
 *  Set true or false
 */
//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetInternalEnforcedVertexAllFaces(CORBA::Boolean theValue) {
  MESSAGE("BLSURFPlugin_Hypothesis_i::SetInternalEnforcedVertexAllFaces");
  ASSERT(myBaseImpl);
  this->GetImpl()->SetInternalEnforcedVertexAllFaces(theValue);
  std::string theValueStr = theValue ? "True" : "False";
  SMESH::TPythonDump() << _this() << ".SetInternalEnforcedVertexAllFaces( " << theValueStr.c_str() << " )";
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetInternalEnforcedVertexAllFaces
 *
 *  Get true or false
 */
//=============================================================================
CORBA::Boolean BLSURFPlugin_Hypothesis_i::GetInternalEnforcedVertexAllFaces() {
  MESSAGE("BLSURFPlugin_Hypothesis_i::GetInternalEnforcedVertexAllFaces");
  ASSERT(myBaseImpl);
  return this->GetImpl()->_GetInternalEnforcedVertexAllFaces();
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::SetInternalEnforcedVertexAllFacesGroup
 *
 *  Set group name
 */
//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetInternalEnforcedVertexAllFacesGroup(const char*  groupName) {
  MESSAGE("BLSURFPlugin_Hypothesis_i::SetInternalEnforcedVertexAllFacesGroup");
  ASSERT(myBaseImpl);
  this->GetImpl()->SetInternalEnforcedVertexAllFacesGroup(groupName);
  SMESH::TPythonDump() << _this() << ".SetInternalEnforcedVertexAllFacesGroup( \"" << groupName << "\" )";
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetInternalEnforcedVertexAllFacesGroup
 *
 *  Get group name
 */
//=============================================================================
char* BLSURFPlugin_Hypothesis_i::GetInternalEnforcedVertexAllFacesGroup() {
  MESSAGE("BLSURFPlugin_Hypothesis_i::GetInternalEnforcedVertexAllFacesGroup");
  ASSERT(myBaseImpl);
  return CORBA::string_dup(this->GetImpl()->_GetInternalEnforcedVertexAllFacesGroup().c_str());
}

/*
 * Enable internal enforced vertices on specific face if requested by user
 *
void BLSURFPlugin_Hypothesis_i::SetInternalEnforcedVertex(GEOM::GEOM_Object_ptr theFace, CORBA::Boolean toEnforceInternalVertices)
 throw (SALOME::SALOME_Exception)
{
  MESSAGE("BLSURFPlugin_Hypothesis_i::SetInternalEnforcedVertexForFace");
  try {
    SetInternalEnforcedVertexWithGroup(theFace, toEnforceInternalVertices);
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
}

void BLSURFPlugin_Hypothesis_i::SetInternalEnforcedVertexWithGroup(GEOM::GEOM_Object_ptr theFace, CORBA::Boolean toEnforceInternalVertices, const char* theGroupName)
 throw (SALOME::SALOME_Exception)
{
  MESSAGE("BLSURFPlugin_Hypothesis_i::SetInternalEnforcedVertexForFaceWithGroup");

  if ((theFace->GetShapeType() != GEOM::FACE) && (theFace->GetShapeType() != GEOM::COMPOUND)) {
    MESSAGE("theFace shape type is not FACE or COMPOUND");
    THROW_SALOME_CORBA_EXCEPTION("theFace shape type is not FACE or COMPOUND", SALOME::BAD_PARAM);
  }

  string theFaceEntry = theFace->GetStudyEntry();

  if (theFaceEntry.empty()) {
    GEOM::GEOM_Gen_ptr geomGen = SMESH_Gen_i::GetGeomEngine();
    SMESH_Gen_i *smeshGen = SMESH_Gen_i::GetSMESHGen();
    string aName;
    if (theFace->GetShapeType() == GEOM::FACE)
      aName = "Face_";
    if (theFace->GetShapeType() == GEOM::COMPOUND)
      aName = "Compound_";
    aName += theFace->GetEntry();
    SALOMEDS::SObject_ptr theSFace = geomGen->PublishInStudy(smeshGen->GetCurrentStudy(), NULL, theFace, aName.c_str());
    if (!theSFace->_is_nil())
      theFaceEntry = theSFace->GetID();
  }
  if (theFaceEntry.empty())
    THROW_SALOME_CORBA_EXCEPTION( "Geom object is not published in study" ,SALOME::BAD_PARAM );

  MESSAGE("IDL : GetName : " << theFace->GetName());
  MESSAGE("IDL : GetInternalEnforcedVertexEntry ( "<< theFaceEntry << ")");
  try {
    SetInternalEnforcedVertexEntry(theFaceEntry.c_str(), toEnforceInternalVertices, theGroupName);
  } catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
}

void BLSURFPlugin_Hypothesis_i::SetInternalEnforcedVertexEntry(const char* theFaceEntry, CORBA::Boolean toEnforceInternalVertices, const char* theGroupName)
    throw (SALOME::SALOME_Exception)
{
  MESSAGE("BLSURFPlugin_Hypothesis_i::SetInternalEnforcedVertexForFaceEntry");
  ASSERT(myBaseImpl);
  try {
    this->GetImpl()->SetInternalEnforcedVertex(theFaceEntry, toEnforceInternalVertices, theGroupName);
    std::string theValueStr = toEnforceInternalVertices ? "True" : "False";
    if (string(theGroupName).empty())
      SMESH::TPythonDump() << _this() << ".SetInternalEnforcedVertex( " << theFaceEntry << ", " << theValueStr.c_str() << " )";
    else
      SMESH::TPythonDump() << _this() << ".SetInternalEnforcedVertexWithGroup( " << theFaceEntry << ", " << theValueStr.c_str() << ", \"" << theGroupName << "\" )";
  } catch (const std::exception& ex) {
    std::cout << "Exception: " << ex.what() << std::endl;
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
}

*/

/* TODO GROUPS
 char* BLSURFPlugin_Hypothesis_i::GetEnforcedVertexGroupName(CORBA::Double x, CORBA::Double y, CORBA::Double z)
 throw (SALOME::SALOME_Exception)
 {
 ASSERT(myBaseImpl);
 MESSAGE("ENGINE : GetEnforcedVertexGroupName START ");
 try {
 return CORBA::string_dup( this->GetImpl()->GetEnforcedVertexGroupName(x, y, z).c_str());
 }
 catch (const std::invalid_argument& ex) {
 SALOME::ExceptionStruct ExDescription;
 ExDescription.text = ex.what();
 ExDescription.type = SALOME::BAD_PARAM;
 ExDescription.sourceFile = "BLSURFPlugin_Hypothesis_i::GetEnforcedVertexGroupName(entry)";
 ExDescription.lineNumber = 1146;
 throw SALOME::SALOME_Exception(ExDescription);
 }
 catch (SALOME_Exception& ex) {
 THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
 }
 MESSAGE("ENGINE : GetEnforcedVertexGroupName END ");
 return 0;
 }


 void BLSURFPlugin_Hypothesis_i::SetEnforcedVertexGroupName(CORBA::Double x, CORBA::Double y, CORBA::Double z, const char* groupName)
 throw (SALOME::SALOME_Exception)
 {
 ASSERT(myBaseImpl);
 MESSAGE("ENGINE : SetEnforcedVertexGroupName START ");
 try {
 this->GetImpl()->SetEnforcedVertexGroupName(x, y, z, groupName);
 }
 catch (const std::invalid_argument& ex) {
 SALOME::ExceptionStruct ExDescription;
 ExDescription.text = ex.what();
 ExDescription.type = SALOME::BAD_PARAM;
 ExDescription.sourceFile = "BLSURFPlugin_Hypothesis::SetEnforcedVertexGroupName(x,y,z)";
 ExDescription.lineNumber = 1170;
 throw SALOME::SALOME_Exception(ExDescription);
 }
 catch (SALOME_Exception& ex) {
 THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
 }

 SMESH::TPythonDump() << _this() << ".SetEnforcedVertexGroupName("
 << x << ", " << y << ", " << z << ", '" << groupName << "' )";

 MESSAGE("ENGINE : SetEnforcedVertexGroupName END ");
 }
 */
///////////////////////


//================================================================================
/*!
 * \brief Sets the file for export resulting mesh in GMF format
 * \param theFileName - full name of the file (.mesh, .meshb)
 * 
 * After compute, export the resulting mesh in the given file with the GMF format (.mesh)
 */
//================================================================================  
// void BLSURFPlugin_Hypothesis_i::SetGMFFile(const char* theFileName, CORBA::Boolean isBinary) {
void BLSURFPlugin_Hypothesis_i::SetGMFFile(const char* theFileName) {
  ASSERT(myBaseImpl);
  MESSAGE("IDL : SetGMFFile(" << theFileName << ")");
  bool valueChanged/*, modeChanged*/ = false;
  try {
    valueChanged = (this->GetImpl()->GetGMFFile() != theFileName);
//     modeChanged = (this->GetImpl()->GetGMFFileMode() != isBinary);
    if (valueChanged)// or (!valueChanged && modeChanged))
      this->GetImpl()->SetGMFFile(theFileName);// ,isBinary);
  } catch (const std::exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
  if (valueChanged)// or (!valueChanged && modeChanged))
    SMESH::TPythonDump() << _this() << ".SetGMFFile(\"" << theFileName << "\")"; //", " << isBinary << ")";
  MESSAGE("IDL : SetGMFFile END ");
}

//================================================================================
/*!
 * \brief Gets the file name for export resulting mesh in GMF format
 * \retval char* - The file name
 * 
 * Returns the GMF file name
 */
//================================================================================  
char* BLSURFPlugin_Hypothesis_i::GetGMFFile() {
  ASSERT(myBaseImpl);
//   MESSAGE("IDL : GetGMFFile()");
  return CORBA::string_dup(this->GetImpl()->GetGMFFile().c_str());
}

// //================================================================================
// /*!
//  * \brief Gets the file mode for export resulting mesh in GMF format
//  * \retval CORBA::Boolean - TRUE if binary mode, FALSE if ascii mode
//  * 
//  * Returns the GMF file mode
//  */
// //================================================================================  
// CORBA::Boolean BLSURFPlugin_Hypothesis_i::GetGMFFileMode() {
//   ASSERT(myBaseImpl);
//   MESSAGE("IDL : GetGMFFileMode()");
//   return this->GetImpl()->GetGMFFileMode();
// }

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetImpl
 *
 *  Get implementation
 */
//=============================================================================
::BLSURFPlugin_Hypothesis* BLSURFPlugin_Hypothesis_i::GetImpl() {
  // MESSAGE("BLSURFPlugin_Hypothesis_i::GetImpl");
  return (::BLSURFPlugin_Hypothesis*) myBaseImpl;
}

//================================================================================
/*!
 * \brief Verify whether hypothesis supports given entity type 
 * \param type - dimension (see SMESH::Dimension enumeration)
 * \retval CORBA::Boolean - TRUE if dimension is supported, FALSE otherwise
 * 
 * Verify whether hypothesis supports given entity type (see SMESH::Dimension enumeration)
 */
//================================================================================  
CORBA::Boolean BLSURFPlugin_Hypothesis_i::IsDimSupported(SMESH::Dimension type) {
  return type == SMESH::DIM_2D;
}
