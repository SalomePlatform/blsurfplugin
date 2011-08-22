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

  void SetPreCADMergeEdges(CORBA::Boolean theValue);
  CORBA::Boolean GetPreCADMergeEdges();

  void SetPreCADRemoveNanoEdges(CORBA::Boolean theValue);
  CORBA::Boolean GetPreCADRemoveNanoEdges();

  void SetPreCADDiscardInput(CORBA::Boolean theValue);
  CORBA::Boolean GetPreCADDiscardInput();

  void SetPreCADEpsNano(CORBA::Double theValue);
  CORBA::Boolean GetPreCADEpsNano();

  void SetOptionValue(const char* optionName, const char* optionValue) throw (SALOME::SALOME_Exception);
  char* GetOptionValue(const char* optionName) throw (SALOME::SALOME_Exception);

  void UnsetOption(const char* optionName);

  BLSURFPlugin::string_array* GetOptionValues();

  void SetOptionValues(const BLSURFPlugin::string_array& options) throw (SALOME::SALOME_Exception);

  void SetSizeMapEntry(const char* entry, const char* sizeMap) throw (SALOME::SALOME_Exception);

  char* GetSizeMapEntry(const char* entry) throw (SALOME::SALOME_Exception);

  void UnsetEntry(const char* entry);

  BLSURFPlugin::string_array* GetSizeMapEntries();

  void SetSizeMapEntries(const BLSURFPlugin::string_array& options) throw (SALOME::SALOME_Exception);

  void SetSizeMap(GEOM::GEOM_Object_ptr GeomObj, const char* sizeMap);

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
  
  void SetAttractorGeom(GEOM::GEOM_Object_ptr GeomObj, GEOM::GEOM_Object_ptr Attractor, double StartSize, double EndSize, double ActionRadius, double ConstantRadius );

  void UnsetAttractorGeom(GEOM::GEOM_Object_ptr GeomObj);

  void SetClassAttractorEntry(const char* entry, const char* att_entry, double StartSize, double EndSize, double ActionRadius, double ConstantRadius)  throw (SALOME::SALOME_Exception);

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
   * Set/get node group to an enforced vertex
   */
  /* TODO GROUPS
   void  SetEnforcedVertexGroupName(CORBA::Double x, CORBA::Double y, CORBA::Double z, const char* groupName) throw (SALOME::SALOME_Exception);
   char* GetEnforcedVertexGroupName(CORBA::Double x, CORBA::Double y, CORBA::Double z) throw (SALOME::SALOME_Exception);
   */
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
};

#endif
