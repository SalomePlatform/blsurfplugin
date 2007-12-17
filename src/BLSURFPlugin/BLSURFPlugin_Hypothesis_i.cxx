//  BLSURFPlugin : C++ implementation
//
//  Copyright (C) 2006  OPEN CASCADE, CEA/DEN, EDF R&D
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
// File      : BLSURFPlugin_Hypothesis_i.cxx
// Authors   : Francis KLOSS (OCC) & Patrick LAUG (INRIA) & Lioka RAZAFINDRAZAKA (CEA)
// Date      : 03/04/2006
// Project   : SALOME
//=============================================================================
using namespace std;

#include "BLSURFPlugin_Hypothesis_i.hxx"
#include "SMESH_Gen.hxx"
#include "SMESH_PythonDump.hxx"

#include "Utils_CorbaException.hxx"
#include "utilities.h"

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
