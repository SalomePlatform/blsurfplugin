//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D
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
//  BLSURFPlugin : C++ implementation
// File      : BLSURFPlugin_Hypothesis_i.cxx
// Authors   : Francis KLOSS (OCC) & Patrick LAUG (INRIA) & Lioka RAZAFINDRAZAKA (CEA)
// Date      : 03/04/2006
// Project   : SALOME
//=============================================================================
//
using namespace std;

#include "BLSURFPlugin_Hypothesis_i.hxx"
#include "SMESH_Gen.hxx"
#include "SMESH_PythonDump.hxx"

#include "Utils_CorbaException.hxx"
#include "utilities.h"

#include <stdexcept>

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
