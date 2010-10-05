//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
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

  void SetSizeMapEntry(const char* entry,const char* sizeMap )  throw (SALOME::SALOME_Exception);
  
  char* GetSizeMapEntry(const char* entry)  throw (SALOME::SALOME_Exception);

  void UnsetEntry(const char* entry);

  BLSURFPlugin::string_array* GetSizeMapEntries();

  void SetSizeMapEntries(const BLSURFPlugin::string_array& options) throw (SALOME::SALOME_Exception);

  void SetSizeMap(GEOM::GEOM_Object_ptr GeomObj, const char* sizeMap);

  void UnsetSizeMap(GEOM::GEOM_Object_ptr GeomObj);

  void ClearSizeMaps();


  void SetAttractor(GEOM::GEOM_Object_ptr GeomObj, const char* attractor);

  void UnsetAttractor(GEOM::GEOM_Object_ptr GeomObj);

  void SetAttractorEntry(const char* entry,const char* attractor )  throw (SALOME::SALOME_Exception);

  char* GetAttractorEntry(const char* entry)  throw (SALOME::SALOME_Exception);

  BLSURFPlugin::string_array* GetAttractorEntries();


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
  
  BLSURFPlugin::TEntryEnfVertexListMap* GetAllEnforcedVertices();
  void                                  ClearAllEnforcedVertices();

  /*!
    * Set/get/unset an enforced vertex on geom object
    */
  void SetEnforcedVertex(GEOM::GEOM_Object_ptr GeomObj, CORBA::Double x, CORBA::Double y, CORBA::Double z)
      throw (SALOME::SALOME_Exception);
  /* TODO GROUPS
  void SetEnforcedVertexWithGroup(GEOM::GEOM_Object_ptr GeomObj,
                                  CORBA::Double x, CORBA::Double y, CORBA::Double z,
                                  const char* groupName)
      throw (SALOME::SALOME_Exception);
  */
//   void SetEnforcedVertexList(GEOM::GEOM_Object_ptr GeomObj, const BLSURFPlugin::TEnfVertexList& vertexList)
//       throw (SALOME::SALOME_Exception);
  
  BLSURFPlugin::TEnfVertexList* GetEnforcedVertices(GEOM::GEOM_Object_ptr GeomObj)
      throw (SALOME::SALOME_Exception);
  
  void UnsetEnforcedVertex(GEOM::GEOM_Object_ptr GeomObj, CORBA::Double x, CORBA::Double y, CORBA::Double z)
      throw (SALOME::SALOME_Exception);
  
//   void UnsetEnforcedVertexList(GEOM::GEOM_Object_ptr GeomObj, BLSURFPlugin::TEnfVertexList& vertexList)
//       throw (SALOME::SALOME_Exception);
  
  void UnsetEnforcedVertices(GEOM::GEOM_Object_ptr GeomObj)
      throw (SALOME::SALOME_Exception);

  /*!
    * Set/get/unset an enforced vertex on geom object given by entry
    */
  void SetEnforcedVertexEntry(const char* entry, CORBA::Double x, CORBA::Double y, CORBA::Double z)
      throw (SALOME::SALOME_Exception);
  /* TODO GROUPS
  void SetEnforcedVertexEntryWithGroup(const char* entry, CORBA::Double x, CORBA::Double y, CORBA::Double z,
                                       const char* groupName)
      throw (SALOME::SALOME_Exception);
  */
//   void SetEnforcedVertexListEntry(const char* entry, BLSURFPlugin::TEnfVertexList& vertexList)
//       throw (SALOME::SALOME_Exception);
  
  BLSURFPlugin::TEnfVertexList* GetEnforcedVerticesEntry(const char* entry)
      throw (SALOME::SALOME_Exception);
  
  void UnsetEnforcedVertexEntry(const char* entry, CORBA::Double x, CORBA::Double y, CORBA::Double z)
      throw (SALOME::SALOME_Exception);
//   void UnsetEnforcedVertexListEntry(const char* entry, BLSURFPlugin::TEnfVertexList& vertexList)
//       throw (SALOME::SALOME_Exception);
  void UnsetEnforcedVerticesEntry(const char* entry)
      throw (SALOME::SALOME_Exception);
  
  /*!
    * Set/get node group to an enforced vertex
    */
  /* TODO GROUPS
  void  SetEnforcedVertexGroupName(CORBA::Double x, CORBA::Double y, CORBA::Double z,
                                   const char* groupName)
      throw (SALOME::SALOME_Exception);
  char* GetEnforcedVertexGroupName(CORBA::Double x, CORBA::Double y, CORBA::Double z)
      throw (SALOME::SALOME_Exception);
  */
  ///////////////////////

  // Get implementation
  ::BLSURFPlugin_Hypothesis* GetImpl();

  // Verify whether hypothesis supports given entity type 
  CORBA::Boolean IsDimSupported( SMESH::Dimension type );
};

#endif
