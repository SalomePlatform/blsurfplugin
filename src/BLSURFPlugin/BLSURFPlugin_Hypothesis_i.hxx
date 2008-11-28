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
// File      : BLSURFPlugin_Hypothesis_i.hxx
// Authors   : Francis KLOSS (OCC) & Patrick LAUG (INRIA) & Lioka RAZAFINDRAZAKA (CEA)
// Date      : 03/04/2006
// Project   : SALOME
//=============================================================================
//
#ifndef _BLSURFPlugin_Hypothesis_i_HXX_
#define _BLSURFPlugin_Hypothesis_i_HXX_

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(BLSURFPlugin_Algorithm)

#include "SMESH_Hypothesis_i.hxx"
#include "BLSURFPlugin_Hypothesis.hxx"

class SMESH_Gen;

// BLSURFPlugin parameters hypothesis

class BLSURFPlugin_Hypothesis_i:
  public virtual POA_BLSURFPlugin::BLSURFPlugin_Hypothesis,
  public virtual SMESH_Hypothesis_i
{
 public:
  // Constructor
  BLSURFPlugin_Hypothesis_i (PortableServer::POA_ptr thePOA,
                             int                     theStudyId,
                             ::SMESH_Gen*            theGenImpl);
  // Destructor
  virtual ~BLSURFPlugin_Hypothesis_i();

  void SetTopology(CORBA::Long theValue);
  CORBA::Long GetTopology();

  void SetPhysicalMesh(CORBA::Long theValue);
  CORBA::Long GetPhysicalMesh();

  void SetPhySize(CORBA::Double theValue);
  CORBA::Double GetPhySize();

  void SetPhyMin(CORBA::Double theMinSize);
  CORBA::Double GetPhyMin();

  void SetPhyMax(CORBA::Double theMaxSize);
  CORBA::Double GetPhyMax();

  void SetGeometricMesh(CORBA::Long theValue);
  CORBA::Long GetGeometricMesh();

  void SetAngleMeshS(CORBA::Double theValue);
  CORBA::Double GetAngleMeshS();

  void SetAngleMeshC(CORBA::Double angle);
  CORBA::Double GetAngleMeshC();

  void SetGeoMin(CORBA::Double theMinSize);
  CORBA::Double GetGeoMin();

  void SetGeoMax(CORBA::Double theMaxSize);
  CORBA::Double GetGeoMax();

  void SetGradation(CORBA::Double theValue);
  CORBA::Double GetGradation();

  void SetQuadAllowed(CORBA::Boolean theValue);
  CORBA::Boolean GetQuadAllowed();

  void SetDecimesh(CORBA::Boolean theValue);
  CORBA::Boolean GetDecimesh();

  void SetVerbosity(CORBA::Short theVal) throw (SALOME::SALOME_Exception);
  CORBA::Short GetVerbosity();

  void SetOptionValue(const char* optionName,
                      const char* optionValue) throw (SALOME::SALOME_Exception);
  char* GetOptionValue(const char* optionName) throw (SALOME::SALOME_Exception);

  void UnsetOption(const char* optionName);

  BLSURFPlugin::string_array* GetOptionValues();

  void SetOptionValues(const BLSURFPlugin::string_array& options) throw (SALOME::SALOME_Exception);

  // Get implementation
  ::BLSURFPlugin_Hypothesis* GetImpl();
  
  // Verify whether hypothesis supports given entity type 
  CORBA::Boolean IsDimSupported( SMESH::Dimension type );
};

#endif
