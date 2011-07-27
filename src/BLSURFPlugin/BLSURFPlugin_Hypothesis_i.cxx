//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// ---
// File    : BLSURFPlugin_Hypothesis.cxx
// Authors : Francis KLOSS (OCC) & Patrick LAUG (INRIA) & Lioka RAZAFINDRAZAKA (CEA)
//           Size maps developement: Nicolas GEIMER (OCC) & Gilles DAVID (EURIWARE)
// ---
//
#include "BLSURFPlugin_Hypothesis_i.hxx"
#include "SMESH_Gen.hxx"
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
BLSURFPlugin_Hypothesis_i::
BLSURFPlugin_Hypothesis_i (PortableServer::POA_ptr thePOA,
                           int                     theStudyId,
                           ::SMESH_Gen*            theGenImpl)
  : SALOME::GenericObj_i( thePOA ), 
    SMESH_Hypothesis_i( thePOA )
{
  MESSAGE( "BLSURFPlugin_Hypothesis_i::BLSURFPlugin_Hypothesis_i" );
  myBaseImpl = new ::BLSURFPlugin_Hypothesis (theGenImpl->GetANewId(),
                                              theStudyId,
                                              theGenImpl);
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::~BLSURFPlugin_Hypothesis_i
 *
 *  Destructor
 */
//=============================================================================
BLSURFPlugin_Hypothesis_i::~BLSURFPlugin_Hypothesis_i()
{
  MESSAGE( "BLSURFPlugin_Hypothesis_i::~BLSURFPlugin_Hypothesis_i" );
}

/*!
 *  BLSURFPlugin_Hypothesis_i::SetTopology
 *
 *  Set topology
 */

//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetTopology (CORBA::Long theValue)
{
  // MESSAGE("BLSURFPlugin_Hypothesis_i::SetTopology");
  ASSERT(myBaseImpl);
  this->GetImpl()->SetTopology((::BLSURFPlugin_Hypothesis::Topology)theValue);
  SMESH::TPythonDump() << _this() << ".SetTopology( " << theValue << " )";
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetTopology
 *
 *  Get Topology
 */
//=============================================================================
CORBA::Long BLSURFPlugin_Hypothesis_i::GetTopology()
{
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
void BLSURFPlugin_Hypothesis_i::SetPhysicalMesh (CORBA::Long theValue)
{
  // MESSAGE("BLSURFPlugin_Hypothesis_i::SetPhysicalMesh");
  ASSERT(myBaseImpl);
  this->GetImpl()->SetPhysicalMesh((::BLSURFPlugin_Hypothesis::PhysicalMesh)theValue);
  SMESH::TPythonDump() << _this() << ".SetPhysicalMesh( " << theValue << " )";
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetPhysicalMesh
 *
 *  Get PhysicalMesh
 */
//=============================================================================
CORBA::Long BLSURFPlugin_Hypothesis_i::GetPhysicalMesh()
{
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
void BLSURFPlugin_Hypothesis_i::SetPhySize (CORBA::Double theValue)
{
  // MESSAGE("BLSURFPlugin_Hypothesis_i::SetPhySize");
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
CORBA::Double BLSURFPlugin_Hypothesis_i::GetPhySize()
{
  // MESSAGE("BLSURFPlugin_Hypothesis_i::GetPhySize");
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetPhySize();
}

//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetPhyMin(CORBA::Double theMinSize)
{
  ASSERT(myBaseImpl);
  if ( GetPhyMin() != theMinSize ) {
    this->GetImpl()->SetPhyMin(theMinSize);
    SMESH::TPythonDump() << _this() << ".SetPhyMin( " << theMinSize << " )";
  }
}

//=============================================================================
CORBA::Double BLSURFPlugin_Hypothesis_i::GetPhyMin()
{
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetPhyMin();
}


//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetPhyMax(CORBA::Double theMaxSize)
{
  ASSERT(myBaseImpl);
  if ( GetPhyMax() != theMaxSize ) {
    this->GetImpl()->SetPhyMax(theMaxSize);
    SMESH::TPythonDump() << _this() << ".SetPhyMax( " << theMaxSize << " )";
  }
}

//=============================================================================
CORBA::Double BLSURFPlugin_Hypothesis_i::GetPhyMax()
{
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
void BLSURFPlugin_Hypothesis_i::SetGeometricMesh (CORBA::Long theValue)
{
  // MESSAGE("BLSURFPlugin_Hypothesis_i::SetGeometricMesh");
  ASSERT(myBaseImpl);
  this->GetImpl()->SetGeometricMesh((::BLSURFPlugin_Hypothesis::GeometricMesh)theValue);
  SMESH::TPythonDump() << _this() << ".SetGeometricMesh( " << theValue << " )";
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetGeometricMesh
 *
 *  Get GeometricMesh
 */
//=============================================================================
CORBA::Long BLSURFPlugin_Hypothesis_i::GetGeometricMesh()
{
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
void BLSURFPlugin_Hypothesis_i::SetAngleMeshS (CORBA::Double theValue)
{
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
CORBA::Double BLSURFPlugin_Hypothesis_i::GetAngleMeshS()
{
  // MESSAGE("BLSURFPlugin_Hypothesis_i::GetAngleMeshS");
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetAngleMeshS();
}

//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetAngleMeshC(CORBA::Double angle)
{
  ASSERT(myBaseImpl);
  this->GetImpl()->SetAngleMeshC(angle);
  SMESH::TPythonDump() << _this() << ".SetAngleMeshC( " << angle << " )";
}

//=============================================================================
CORBA::Double BLSURFPlugin_Hypothesis_i::GetAngleMeshC()
{
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetAngleMeshC();
}

//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetGeoMin(CORBA::Double theMinSize)
{
  ASSERT(myBaseImpl);
  if ( GetGeoMin() != theMinSize ) {
    this->GetImpl()->SetGeoMin(theMinSize);
    SMESH::TPythonDump() << _this() << ".SetGeoMin( " << theMinSize << " )";
  }
}

//=============================================================================
CORBA::Double BLSURFPlugin_Hypothesis_i::GetGeoMin()
{
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetGeoMin();
}

//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetGeoMax(CORBA::Double theMaxSize)
{
  ASSERT(myBaseImpl);
  if ( GetGeoMax() != theMaxSize ) {
    this->GetImpl()->SetGeoMax(theMaxSize);
    SMESH::TPythonDump() << _this() << ".SetGeoMax( " << theMaxSize << " )";
  }
}

//=============================================================================
CORBA::Double BLSURFPlugin_Hypothesis_i::GetGeoMax()
{
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
void BLSURFPlugin_Hypothesis_i::SetGradation (CORBA::Double theValue)
{
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
CORBA::Double BLSURFPlugin_Hypothesis_i::GetGradation()
{
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
void BLSURFPlugin_Hypothesis_i::SetQuadAllowed (CORBA::Boolean theValue)
{
  // MESSAGE("BLSURFPlugin_Hypothesis_i::SetQuadAllowed");
  ASSERT(myBaseImpl);
  this->GetImpl()->SetQuadAllowed(theValue);
  SMESH::TPythonDump() << _this() << ".SetQuadAllowed( " << theValue << " )";
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetQuadAllowed
 *
 *  Get true or false
 */
//=============================================================================
CORBA::Boolean BLSURFPlugin_Hypothesis_i::GetQuadAllowed()
{
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
void BLSURFPlugin_Hypothesis_i::SetDecimesh (CORBA::Boolean theValue)
{
  // MESSAGE("BLSURFPlugin_Hypothesis_i::SetDecimesh");
  ASSERT(myBaseImpl);
  this->GetImpl()->SetDecimesh(theValue);
  SMESH::TPythonDump() << _this() << ".SetDecimesh( " << theValue << " )";
}

//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetDecimesh
 *
 *  Get true or false
 */
//=============================================================================
CORBA::Boolean BLSURFPlugin_Hypothesis_i::GetDecimesh()
{
  // MESSAGE("BLSURFPlugin_Hypothesis_i::GetDecimesh");
  ASSERT(myBaseImpl);
  return this->GetImpl()->GetDecimesh();
}

//=============================================================================
void BLSURFPlugin_Hypothesis_i::SetVerbosity(CORBA::Short theVal)
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
  if ( theVal < 0 || theVal > 100 )
    THROW_SALOME_CORBA_EXCEPTION( "Invalid verbosity level",SALOME::BAD_PARAM );
  this->GetImpl()->SetVerbosity(theVal);
  SMESH::TPythonDump() << _this() << ".SetVerbosity( " << theVal << " )";
}

//=============================================================================

CORBA::Short BLSURFPlugin_Hypothesis_i::GetVerbosity()
{
  ASSERT(myBaseImpl);
  return (CORBA::Short) this->GetImpl()->GetVerbosity();
}

//=============================================================================

void BLSURFPlugin_Hypothesis_i::SetOptionValue(const char* optionName,
                                               const char* optionValue)
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
  bool valueChanged = false;
  try {
    valueChanged = ( this->GetImpl()->GetOptionValue(optionName) != optionValue );
    if ( valueChanged )
      this->GetImpl()->SetOptionValue(optionName, optionValue);
  }
  catch (const std::invalid_argument& ex) {
    SALOME::ExceptionStruct ExDescription;
    ExDescription.text = ex.what();
    ExDescription.type = SALOME::BAD_PARAM;
    ExDescription.sourceFile = "BLSURFPlugin_Hypothesis::SetOptionValue(name,value)";
    ExDescription.lineNumber = 0;
    throw SALOME::SALOME_Exception(ExDescription);
  }
  catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
  if ( valueChanged )
    SMESH::TPythonDump() << _this() << ".SetOptionValue( '"
                         << optionName << "', '" << optionValue << "' )";
}

//=============================================================================

char* BLSURFPlugin_Hypothesis_i::GetOptionValue(const char* optionName)
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
  try {
    return CORBA::string_dup( this->GetImpl()->GetOptionValue(optionName).c_str() );
  }
  catch (const std::invalid_argument& ex) {
    SALOME::ExceptionStruct ExDescription;
    ExDescription.text = ex.what();
    ExDescription.type = SALOME::BAD_PARAM;
    ExDescription.sourceFile = "BLSURFPlugin_Hypothesis::GetOptionValue(name)";
    ExDescription.lineNumber = 0;
    throw SALOME::SALOME_Exception(ExDescription);
  }
  catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
  return 0;
}

//=============================================================================

void BLSURFPlugin_Hypothesis_i::UnsetOption(const char* optionName)
{
  ASSERT(myBaseImpl);
  this->GetImpl()->ClearOption(optionName);
  SMESH::TPythonDump() << _this() << ".UnsetOption( '" << optionName << "' )";
}

//=============================================================================

BLSURFPlugin::string_array* BLSURFPlugin_Hypothesis_i::GetOptionValues()
{
  ASSERT(myBaseImpl);
  BLSURFPlugin::string_array_var result = new BLSURFPlugin::string_array();

  const ::BLSURFPlugin_Hypothesis::TOptionValues & opts = this->GetImpl()->GetOptionValues();
  result->length( opts.size() );

  ::BLSURFPlugin_Hypothesis::TOptionValues::const_iterator opIt = opts.begin();
  for ( int i = 0 ; opIt != opts.end(); ++opIt, ++i ) {
    string name_value = opIt->first;
    if ( !opIt->second.empty() ) {
      name_value += ":";
      name_value += opIt->second;
    }
    result[i] = CORBA::string_dup(name_value.c_str());
  }
  return result._retn();
}

//=============================================================================

void BLSURFPlugin_Hypothesis_i::SetOptionValues(const BLSURFPlugin::string_array& options)
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
  for (int i = 0; i < options.length(); ++i)
  {
    string name_value = options[i].in();
    int colonPos = name_value.find( ':' );
    string name, value;
    if ( colonPos == string::npos ) // ':' not found
      name = name_value;
    else {
      name = name_value.substr( 0, colonPos);
      if ( colonPos < name_value.size()-1 && name_value[colonPos] != ' ')
        value = name_value.substr( colonPos+1 );
    }
    SetOptionValue( name.c_str(), value.c_str() );
  }
}

//=============================================================================

void BLSURFPlugin_Hypothesis_i::SetSizeMapEntry(const char* entry,const char* sizeMap)
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
  MESSAGE("ENGINE : SETSIZEMAP START ENTRY : " << entry); 
  bool valueChanged = false;
  try {
    valueChanged = ( this->GetImpl()->GetSizeMapEntry(entry) != sizeMap );
    if ( valueChanged )
      this->GetImpl()->SetSizeMapEntry(entry, sizeMap);
  }
  catch (const std::invalid_argument& ex) {
    SALOME::ExceptionStruct ExDescription;
    ExDescription.text = ex.what();
    ExDescription.type = SALOME::BAD_PARAM;
    ExDescription.sourceFile = "BLSURFPlugin_Hypothesis::SetSizeMapEntry(entry,sizemap)";
    ExDescription.lineNumber = 0;
    throw SALOME::SALOME_Exception(ExDescription);
  }
  catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
  MESSAGE("ENGINE : SETSIZEMAP END ENTRY : " << entry);
  if ( valueChanged )
    SMESH::TPythonDump() << _this() << ".SetSizeMap("
                         << entry << ", '" << sizeMap << "' )";
}

//=============================================================================

void BLSURFPlugin_Hypothesis_i::SetAttractorEntry(const char* entry,const char* attractor )
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
  MESSAGE("ENGINE : SETATTRACTOR START ENTRY : " << entry);
  bool valueChanged = false;
  try {
    valueChanged = ( this->GetImpl()->GetAttractorEntry(entry) != attractor );
    if ( valueChanged ) {
      boost::regex re("^ATTRACTOR\\((?:(-?0(\\.\\d*)*|-?[1-9]+\\d*(\\.\\d*)*|-?\\.(\\d)+);){5}(True|False)\\)$");
      if (!boost::regex_match(string(attractor), re))
        throw std::invalid_argument("Error: an attractor is defined with the following pattern: ATTRACTOR(xa;ya;za;a;b;True|False)");
      this->GetImpl()->SetAttractorEntry(entry, attractor);
    }
  }
  catch (const std::invalid_argument& ex) {
    SALOME::ExceptionStruct ExDescription;
    ExDescription.text = ex.what();
    ExDescription.type = SALOME::BAD_PARAM;
    ExDescription.sourceFile = "BLSURFPlugin_Hypothesis::SetAttractorEntry(entry,attractor)";
    ExDescription.lineNumber = 0;
    throw SALOME::SALOME_Exception(ExDescription);
  }
  catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
  MESSAGE("ENGINE : SETATTRACTOR END ENTRY : " << entry);
  if ( valueChanged )
    SMESH::TPythonDump() << _this() << ".SetAttractor("
                         << entry << ", '" << attractor << "' )";
}


//=============================================================================

char* BLSURFPlugin_Hypothesis_i::GetSizeMapEntry(const char* entry) 
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
  try {
    return CORBA::string_dup( this->GetImpl()->GetSizeMapEntry(entry).c_str());
  }
  catch (const std::invalid_argument& ex) {
    SALOME::ExceptionStruct ExDescription;
    ExDescription.text = ex.what();
    ExDescription.type = SALOME::BAD_PARAM;
    ExDescription.sourceFile = "BLSURFPlugin_Hypothesis::GetSizeMapEntry(name)";
    ExDescription.lineNumber = 0;
    throw SALOME::SALOME_Exception(ExDescription);
  }
  catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
  return 0;
}

//=============================================================================

char* BLSURFPlugin_Hypothesis_i::GetAttractorEntry(const char* entry)
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
  try {
    return CORBA::string_dup( this->GetImpl()->GetAttractorEntry(entry).c_str());
  }
  catch (const std::invalid_argument& ex) {
    SALOME::ExceptionStruct ExDescription;
    ExDescription.text = ex.what();
    ExDescription.type = SALOME::BAD_PARAM;
    ExDescription.sourceFile = "BLSURFPlugin_Hypothesis::GetAttractorEntry(name)";
    ExDescription.lineNumber = 0;
    throw SALOME::SALOME_Exception(ExDescription);
  }
  catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
  return 0;
}

//=============================================================================

void BLSURFPlugin_Hypothesis_i::UnsetEntry(const char* entry)
{
  ASSERT(myBaseImpl); 
  this->GetImpl()->ClearEntry(entry);
//  SMESH::TPythonDump() << _this() << ".UnsetSizeMap( " << entry << " )";
}

//=============================================================================

BLSURFPlugin::string_array* BLSURFPlugin_Hypothesis_i::GetSizeMapEntries()
{
  ASSERT(myBaseImpl);
  BLSURFPlugin::string_array_var result = new BLSURFPlugin::string_array();

  const ::BLSURFPlugin_Hypothesis::TSizeMap sizeMaps= this->GetImpl()->_GetSizeMapEntries();
  result->length( sizeMaps.size() );

  ::BLSURFPlugin_Hypothesis::TSizeMap::const_iterator smIt = sizeMaps.begin();
  for ( int i = 0 ; smIt != sizeMaps.end(); ++smIt, ++i ) {
    string entry_sizemap = smIt->first;
    if ( !smIt->second.empty() ) {
      entry_sizemap += "|";
      entry_sizemap += smIt->second;
    }
    result[i] = CORBA::string_dup(entry_sizemap.c_str());
  }
  return result._retn();
}

//=============================================================================

BLSURFPlugin::string_array* BLSURFPlugin_Hypothesis_i::GetAttractorEntries()
{
  ASSERT(myBaseImpl);
  BLSURFPlugin::string_array_var result = new BLSURFPlugin::string_array();

  const ::BLSURFPlugin_Hypothesis::TSizeMap attractors= this->GetImpl()->_GetAttractorEntries();
  result->length( attractors.size() );

  ::BLSURFPlugin_Hypothesis::TSizeMap::const_iterator atIt = attractors.begin();
  for ( int i = 0 ; atIt != attractors.end(); ++atIt, ++i ) {
    string entry_attractor = atIt->first;
    if ( !atIt->second.empty() ) {
      entry_attractor += "|";
      entry_attractor += atIt->second;
    }
    result[i] = CORBA::string_dup(entry_attractor.c_str());
  }
  return result._retn();
}

//=============================================================================

void BLSURFPlugin_Hypothesis_i::SetSizeMapEntries(const BLSURFPlugin::string_array& sizeMaps)
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
  for (int i = 0; i < sizeMaps.length(); ++i)
  {
    string entry_sizemap = sizeMaps[i].in();
    int colonPos = entry_sizemap.find( '|' );
    string entry, sizemap;
    if ( colonPos == string::npos ) // '|' separator not found
      entry = entry_sizemap;
    else {
      entry = entry_sizemap.substr( 0, colonPos);
      if ( colonPos < entry_sizemap.size()-1 && entry_sizemap[colonPos] != ' ')
        sizemap = entry_sizemap.substr( colonPos+1 );
    }
    this->GetImpl()->SetSizeMapEntry( entry.c_str(), sizemap.c_str() );
  }
}

//=============================================================================

void BLSURFPlugin_Hypothesis_i::ClearSizeMaps()
{
  ASSERT(myBaseImpl);
  this->GetImpl()->ClearSizeMaps();
}


//=============================================================================

void BLSURFPlugin_Hypothesis_i::SetSizeMap(const GEOM::GEOM_Object_ptr GeomObj,const char* sizeMap)
{
  ASSERT(myBaseImpl);
  string entry;
  entry=GeomObj->GetStudyEntry();
  MESSAGE("IDL : GetName : " << GeomObj->GetName());
  MESSAGE("IDL : SETSIZEMAP ( "<< entry << " , " << sizeMap << ")"); 
  SetSizeMapEntry( entry.c_str(),sizeMap);
}
  
//=============================================================================
void BLSURFPlugin_Hypothesis_i::UnsetSizeMap(const GEOM::GEOM_Object_ptr GeomObj)
{
  ASSERT(myBaseImpl);
  string entry;
  entry=GeomObj->GetStudyEntry();
  MESSAGE("IDL : GetName : " << GeomObj->GetName());
  MESSAGE("IDL : UNSETSIZEMAP ( "<< entry << ")");
  UnsetEntry( entry.c_str());
  SMESH::TPythonDump() << _this() << ".UnsetSizeMap( " << entry.c_str() << " )";
}


void BLSURFPlugin_Hypothesis_i::SetAttractor(GEOM::GEOM_Object_ptr GeomObj, const char* attractor)
{
  ASSERT(myBaseImpl);
  string entry;
  entry=GeomObj->GetStudyEntry();
  MESSAGE("IDL : GetName : " << GeomObj->GetName());
  MESSAGE("IDL : SETATTRACTOR ( "<< entry << " , " << attractor << ")");
  SetAttractorEntry( entry.c_str(),attractor);
}

void BLSURFPlugin_Hypothesis_i::UnsetAttractor(GEOM::GEOM_Object_ptr GeomObj)
{
  ASSERT(myBaseImpl);
  string entry;
  entry=GeomObj->GetStudyEntry();
  MESSAGE("IDL : GetName : " << GeomObj->GetName());
  MESSAGE("IDL : UNSETATTRACTOR ( "<< entry << ")");
  UnsetEntry( entry.c_str());
  SMESH::TPythonDump() << _this() << ".UnsetAttractor( " << entry.c_str() << " )";
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


///////////////////////
// ENFORCED VERTEXES //
///////////////////////

BLSURFPlugin::TEntryEnfVertexListMap* BLSURFPlugin_Hypothesis_i::GetAllEnforcedVertices()
{
  MESSAGE("IDL: GetAllEnforcedVertices()");
  ASSERT(myBaseImpl);
  BLSURFPlugin::TEntryEnfVertexListMap_var resultMap = new BLSURFPlugin::TEntryEnfVertexListMap();
  const ::BLSURFPlugin_Hypothesis::TEntryEnfVertexListMap entryEnfVertexListMap = this->GetImpl()->_GetAllEnforcedVertices();
  resultMap->length(entryEnfVertexListMap.size());
  MESSAGE("Enforced Vertex map size is " << entryEnfVertexListMap.size());

  ::BLSURFPlugin_Hypothesis::TEnfVertexList enfVertexList;
  ::BLSURFPlugin_Hypothesis::TEntryEnfVertexListMap::const_iterator evmIt = entryEnfVertexListMap.begin();
  for ( int i = 0 ; evmIt != entryEnfVertexListMap.end(); ++evmIt, ++i ) {
    std::string entry = evmIt->first;
    MESSAGE("Entry: " << entry);
    enfVertexList = evmIt->second;

    BLSURFPlugin::TEntryEnfVertexListMapElement_var mapElement = new BLSURFPlugin::TEntryEnfVertexListMapElement();

    BLSURFPlugin::TEnfVertexList_var vertexList = new BLSURFPlugin::TEnfVertexList();
    vertexList->length(enfVertexList.size());
    MESSAGE("Number of enforced vertices: " << enfVertexList.size());

    ::BLSURFPlugin_Hypothesis::TEnfVertexList::const_iterator evlIt = enfVertexList.begin();
    for ( int j = 0 ; evlIt != enfVertexList.end(); ++evlIt, ++j ) {
      MESSAGE("Enforced Vertex #" << j);
      BLSURFPlugin::TEnfVertex_var enfVertex = new BLSURFPlugin::TEnfVertex();
      enfVertex->length(3);
      enfVertex[0] = (*evlIt)[0];
      enfVertex[1] = (*evlIt)[1];
      enfVertex[2] = (*evlIt)[2];
      vertexList[j] = enfVertex;
      MESSAGE("Enforced vertex: " << enfVertex[0] << ", " << enfVertex[1] << ", " << enfVertex[2]);
    }

    mapElement->entry = CORBA::string_dup(entry.c_str());
    mapElement->vertexList = vertexList;

    resultMap[i] = mapElement;

  }
  return resultMap._retn();
}

void BLSURFPlugin_Hypothesis_i::ClearAllEnforcedVertices()
{
  ASSERT(myBaseImpl);
  this->GetImpl()->ClearAllEnforcedVertices();
  SMESH::TPythonDump() << _this() << ".ClearAllEnforcedVertices()";
}

/*!
  * Set/get/unset an enforced vertex on geom object
  */
void BLSURFPlugin_Hypothesis_i::SetEnforcedVertex(GEOM::GEOM_Object_ptr GeomObj, CORBA::Double x, CORBA::Double y, CORBA::Double z)
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
  // TODO check that GeomObj is a face => in engine ?
  // TODO Affecter un nom de groupe vide
  string entry = GeomObj->GetStudyEntry();
  MESSAGE("IDL : GetName : " << GeomObj->GetName());
  MESSAGE("IDL : SetEnforcedVertex ( "<< entry << ", " << x << ", " << y << ", " << z << ")");
  try {
    SetEnforcedVertexEntry(entry.c_str(), x, y, z);
  }
  catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
}

/*!
  * Set/get/unset an enforced vertex on geom object with group name
  */
/* TODO GROUPS
void BLSURFPlugin_Hypothesis_i::SetEnforcedVertexWithGroup(GEOM::GEOM_Object_ptr GeomObj,
                  CORBA::Double x, CORBA::Double y, CORBA::Double z, const char* groupName)
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
  // TODO check that GeomObj is a face => in engine ?
  string entry = GeomObj->GetStudyEntry();
  MESSAGE("IDL : GetName : " << GeomObj->GetName());
  MESSAGE("IDL : SetEnforcedVertexWithGroup ( "<< entry << ", " << x << ", " << y << ", " << z << ", " << groupName <<")");
  try {
    SetEnforcedVertexEntryWithGroup(entry.c_str(), x, y, z, groupName);
  }
  catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
}
*/
BLSURFPlugin::TEnfVertexList* BLSURFPlugin_Hypothesis_i::GetEnforcedVertices(GEOM::GEOM_Object_ptr GeomObj)
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
  string entry = GeomObj->GetStudyEntry();
  MESSAGE("IDL : GetName : " << GeomObj->GetName());
  MESSAGE("IDL : GetEnforcedVertexList ( "<< entry << ")");
  try {
    return GetEnforcedVerticesEntry(entry.c_str());
  }
  catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
}


void BLSURFPlugin_Hypothesis_i::UnsetEnforcedVertex(GEOM::GEOM_Object_ptr GeomObj, CORBA::Double x, CORBA::Double y, CORBA::Double z)
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
  string entry = GeomObj->GetStudyEntry();
  MESSAGE("IDL : GetName : " << GeomObj->GetName());
  MESSAGE("IDL : UnsetEnforcedVertex ( "<< entry << ", " << x << ", " << y << ", " << z << ")");

  try {
    UnsetEnforcedVertexEntry(entry.c_str(), x, y, z);
  }
  catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
}


void BLSURFPlugin_Hypothesis_i::UnsetEnforcedVertices(GEOM::GEOM_Object_ptr GeomObj)
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);;
  string entry = GeomObj->GetStudyEntry();
  MESSAGE("IDL : GetName : " << GeomObj->GetName());
  MESSAGE("IDL : UnsetEnforcedVertices ( "<< entry << ")");

  try {
    UnsetEnforcedVerticesEntry(entry.c_str());
  }
  catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
}


/*!
  * Set/get/unset an enforced vertex on geom object given by entry
  */
void BLSURFPlugin_Hypothesis_i::SetEnforcedVertexEntry(const char* entry, double x, double y, double z)
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
  MESSAGE("IDL : SETENFORCEDVERTEX START - ENTRY: " << entry << " VERTEX: " << x << " " << y << " " << z);
  bool newValue = false;
  
  try {
    ::BLSURFPlugin_Hypothesis::TEnfVertexList vertexList = this->GetImpl()->GetEnforcedVertices(entry);
    ::BLSURFPlugin_Hypothesis::TEnfVertex vertex;
    vertex.push_back(x);
    vertex.push_back(y);
    vertex.push_back(z);
    if (vertexList.find(vertex) == vertexList.end()) {
      MESSAGE("Vertex not found: add it in vertexList")
      newValue = true;
    }
    else
      MESSAGE("Vertex already found")
  }
  catch (const std::invalid_argument& ex) {
    // no enforced vertex for entry
    MESSAGE("Entry not found : add it to the list")
    newValue = true;
  }
  catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
  
  if ( newValue ) {
    this->GetImpl()->SetEnforcedVertex(entry, x, y, z);
    SMESH::TPythonDump() << _this() << ".SetEnforcedVertex("
                         << entry << ", "
                         << x << ", "
                         << y << ", "
                         << z << ")";
  }
  MESSAGE("IDL : SETENFORCEDVERTEX END - ENTRY: " << entry);
}

/*!
  * Set/get/unset an enforced vertex on geom object given by entry
  */
/* TODO GROUPS
void BLSURFPlugin_Hypothesis_i::SetEnforcedVertexEntryWithGroup(const char* entry,
                                 double x, double y, double z, const char* groupName)
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
  MESSAGE("IDL : SETENFORCEDVERTEXWITHGROUP START - ENTRY: " << entry << " VERTEX: "
            << x << " " << y << " " << z << " group name: " << groupName);
  bool newValue = false;
  bool newGroupName = false;
  try {
    ::BLSURFPlugin_Hypothesis::TEnfVertexList vertexList = this->GetImpl()->GetEnforcedVertices(entry);
    ::BLSURFPlugin_Hypothesis::TEnfVertex vertex;
    vertex.push_back(x);
    vertex.push_back(y);
    vertex.push_back(z);
    if (vertexList.find(vertex) == vertexList.end()) {
      MESSAGE("Vertex not found: add it in vertexList");
      newValue = true;
    }
    else {
      MESSAGE("Vertex already found");
      std::string oldGroupName = this->GetImpl()->GetEnforcedVertexGroupName(x,y,z);
      if (strcmp(oldGroupName.c_str(),groupName)!=0)
        newGroupName = true;
    }
  }
  catch (const std::invalid_argument& ex) {
    // no enforced vertex for entry
    MESSAGE("Entry not found : add it to the list");
    newValue = true;
  }
  catch (SALOME_Exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }
  
  if ( newValue ) {
    this->GetImpl()->SetEnforcedVertex(entry, x, y, z, groupName);
    SMESH::TPythonDump() << _this() << ".SetEnforcedVertexWithGroup("
                         << entry << ", "
                         << x << ", "
                         << y << ", "
                         << z << ", '"
                         << groupName << "')";
  }
  else {
    if (newGroupName) {
      this->GetImpl()->SetEnforcedVertexGroupName(x, y, z, groupName);
      SMESH::TPythonDump() << _this() << ".SetEnforcedVertexGroupName("
                          << x << ", "
                          << y << ", "
                          << z << ", '"
                          << groupName << "')";
    }
  }
  MESSAGE("IDL : SETENFORCEDVERTEXWITHGROUP END - ENTRY: " << entry);
}
*/
/*
void BLSURFPlugin_Hypothesis_i::SetEnforcedVertexListEntry(const char* entry, BLSURFPlugin::TEnfVertexList& vertexList)
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
}
*/

BLSURFPlugin::TEnfVertexList* BLSURFPlugin_Hypothesis_i::GetEnforcedVerticesEntry(const char* entry)
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
  MESSAGE("ENGINE : GETENFORCEDVERTICES START ENTRY : " << entry);
  
  try {
    BLSURFPlugin::TEnfVertexList_var vertexList = new BLSURFPlugin::TEnfVertexList();
    ::BLSURFPlugin_Hypothesis::TEnfVertexList _vList = this->GetImpl()->GetEnforcedVertices(entry);
    vertexList->length(_vList.size());
    MESSAGE("Number of enforced vertices: " << _vList.size());
    ::BLSURFPlugin_Hypothesis::TEnfVertexList::const_iterator evlIt = _vList.begin();
    for ( int i = 0; evlIt != _vList.end() ; ++evlIt, ++i ) {
      BLSURFPlugin::TEnfVertex_var enfVertex = new BLSURFPlugin::TEnfVertex();
      enfVertex->length(3);
      MESSAGE("Enforced vertex #" << i << ": "<< (*evlIt)[0] << ", " << (*evlIt)[1] << ", " << (*evlIt)[2]);
      enfVertex[0] = (*evlIt)[0];
      enfVertex[1] = (*evlIt)[1];
      enfVertex[2] = (*evlIt)[2];
      vertexList[i] = enfVertex;
    }
    return vertexList._retn();
  }
  catch (const std::invalid_argument& ex) {
    SALOME::ExceptionStruct ExDescription;
    ExDescription.text = ex.what();
    ExDescription.type = SALOME::BAD_PARAM;
    ExDescription.sourceFile = "BLSURFPlugin_Hypothesis_i::GetEnforcedVerticesEntry(entry)";
    ExDescription.lineNumber = 1048;
    throw SALOME::SALOME_Exception(ExDescription);
  }
  catch(const std::exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }

  MESSAGE("ENGINE : GETENFORCEDVERTICES END ENTRY : " << entry);
}


void BLSURFPlugin_Hypothesis_i::UnsetEnforcedVertexEntry(const char* entry, CORBA::Double x, CORBA::Double y, CORBA::Double z)
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
  MESSAGE("ENGINE : UNSETENFORCEDVERTEX START ENTRY : " << entry);
  
  try {
    this->GetImpl()->ClearEnforcedVertex(entry, x, y, z);
    SMESH::TPythonDump() << _this() << ".UnsetEnforcedVertex("
                         << entry << ", "
                         << x << ", "
                         << y << ", "
                         << z << ")";
  }
  catch (const std::invalid_argument& ex) {
    SALOME::ExceptionStruct ExDescription;
    ExDescription.text = ex.what();
    ExDescription.type = SALOME::BAD_PARAM;
    ExDescription.sourceFile = "BLSURFPlugin_Hypothesis_i::UnsetEnforcedVertexEntry(entry,x,y,z)";
    ExDescription.lineNumber = 1086;
    throw SALOME::SALOME_Exception(ExDescription);
  }
  catch(const std::exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }

  MESSAGE("ENGINE : UNSETENFORCEDVERTEX END ENTRY : " << entry);
}
/*
void BLSURFPlugin_Hypothesis_i::UnsetEnforcedVertexListEntry(const char* entry, BLSURFPlugin::TEnfVertexList& vertexList)
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
}
*/
void BLSURFPlugin_Hypothesis_i::UnsetEnforcedVerticesEntry(const char* entry)
  throw (SALOME::SALOME_Exception)
{
  ASSERT(myBaseImpl);
  MESSAGE("ENGINE : UNSETENFORCEDVERTICES START ENTRY : " << entry);
  
  try {
    this->GetImpl()->ClearEnforcedVertices(entry);
    SMESH::TPythonDump() << _this() << ".UnsetEnforcedVertices(" << entry << ")";
  }
  catch (const std::invalid_argument& ex) {
    SALOME::ExceptionStruct ExDescription;
    ExDescription.text = ex.what();
    ExDescription.type = SALOME::BAD_PARAM;
    ExDescription.sourceFile = "BLSURFPlugin_Hypothesis_i::UnsetEnforcedVerticesEntry(entry)";
    ExDescription.lineNumber = 1121;
    throw SALOME::SALOME_Exception(ExDescription);
  }
  catch(const std::exception& ex) {
    THROW_SALOME_CORBA_EXCEPTION( ex.what() ,SALOME::BAD_PARAM );
  }

  MESSAGE("ENGINE : UNSETENFORCEDVERTICES END ENTRY : " << entry);
}

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





//=============================================================================
/*!
 *  BLSURFPlugin_Hypothesis_i::GetImpl
 *
 *  Get implementation
 */
//=============================================================================
::BLSURFPlugin_Hypothesis* BLSURFPlugin_Hypothesis_i::GetImpl()
{
  // MESSAGE("BLSURFPlugin_Hypothesis_i::GetImpl");
  return (::BLSURFPlugin_Hypothesis*)myBaseImpl;
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
CORBA::Boolean BLSURFPlugin_Hypothesis_i::IsDimSupported( SMESH::Dimension type )
{
  return type == SMESH::DIM_2D;
}
