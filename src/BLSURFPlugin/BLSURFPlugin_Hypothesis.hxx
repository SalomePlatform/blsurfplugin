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
//           & Aurelien ALLEAUME (DISTENE)
//           Size maps developement: Nicolas GEIMER (OCC) & Gilles DAVID (EURIWARE)
// ---
//
#ifndef _BLSURFPlugin_Hypothesis_HXX_
#define _BLSURFPlugin_Hypothesis_HXX_

#include "SMESH_Hypothesis.hxx"
#include <vector>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <cstring>
#include <sstream>
#include <utilities.h>

//  Parameters for work of BLSURF

class BLSURFPlugin_Hypothesis: public SMESH_Hypothesis
{
public:
  BLSURFPlugin_Hypothesis(int hypId, int studyId, SMESH_Gen * gen);

  enum Topology {
    FromCAD,
    Process,
    Process2
  };

  enum PhysicalMesh {
    DefaultSize,
    PhysicalUserDefined
  };

  enum GeometricMesh {
    DefaultGeom,
    UserDefined
  };

  void SetTopology(Topology theTopology);
  Topology GetTopology() const { return _topology; }

  void SetPhysicalMesh(PhysicalMesh thePhysicalMesh);
  PhysicalMesh GetPhysicalMesh() const { return _physicalMesh; }

  void SetPhySize(double thePhySize);
  double GetPhySize() const { return _phySize; }

  void SetPhyMin(double theMinSize);
  double GetPhyMin() const { return _phyMin; }

  void SetPhyMax(double theMaxSize);
  double GetPhyMax() const { return _phyMax; }

  void SetGeometricMesh(GeometricMesh theGeometricMesh);
  GeometricMesh GetGeometricMesh() const { return _geometricMesh; }

  void SetAngleMeshS(double theAngle);
  double GetAngleMeshS() const { return _angleMeshS; }

  void SetAngleMeshC(double theAngle);
  double GetAngleMeshC() const { return _angleMeshC; }

  void SetGeoMin(double theMinSize);
  double GetGeoMin() const { return _hgeoMin; }

  void SetGeoMax(double theMaxSize);
  double GetGeoMax() const { return _hgeoMax; }

  void SetGradation(double theGradation);
  double GetGradation() const { return _gradation; }

  void SetQuadAllowed(bool theVal);
  bool GetQuadAllowed() const { return _quadAllowed; }

  void SetDecimesh(bool theVal);
  bool GetDecimesh() const { return _decimesh; }

  void SetVerbosity(int theVal);
  int GetVerbosity() const { return _verb; }
  
  void ClearEntry(const std::string& entry);
  void ClearSizeMaps();

  typedef std::map<std::string,std::string> TSizeMap;

  void SetSizeMapEntry(const std::string& entry,const std::string& sizeMap );
  std::string  GetSizeMapEntry(const std::string& entry);
  const TSizeMap& _GetSizeMapEntries() const { return _sizeMap; }
  /*!
   * \brief Return the size maps
   */
  static TSizeMap GetSizeMapEntries(const BLSURFPlugin_Hypothesis* hyp);


  void SetAttractorEntry(const std::string& entry,const std::string& attractor );
  std::string GetAttractorEntry(const std::string& entry);
  const TSizeMap& _GetAttractorEntries() const { return _attractors; };
  /*!
   * \brief Return the attractors
   */
  static TSizeMap GetAttractorEntries(const BLSURFPlugin_Hypothesis* hyp);


/*
  void SetCustomSizeMapEntry(const std::string& entry,const std::string& sizeMap );
  std::string  GetCustomSizeMapEntry(const std::string& entry);
  void UnsetCustomSizeMap(const std::string& entry);
  const TSizeMap& GetCustomSizeMapEntries() const { return _customSizeMap; }
 */

  /*!
   * To set/get/unset an enforced vertex
   */
  // Entry
  typedef std::string TEntry;
  // List of entries
  typedef std::set<TEntry> TEntryList;
  // Name
  typedef std::string TEnfName;
  // Group name
  typedef std::string TEnfGroupName;
  // Coordinates
  typedef std::vector<double> TEnfVertexCoords;
  typedef std::set< TEnfVertexCoords > TEnfVertexCoordsList;

  // Enforced vertex
  struct TEnfVertex {
    TEnfName name;
    TEntry geomEntry;
    TEnfVertexCoords coords;
    TEnfGroupName grpName;
    TEntryList faceEntries;
  };
    
  struct CompareEnfVertices
  {
    bool operator () (const TEnfVertex* e1, const TEnfVertex* e2) const {
      if (e1 && e2) {
        if (e1->coords.size() && e2->coords.size())
          return (e1->coords < e2->coords);
        else
          return (e1->geomEntry < e2->geomEntry);
      }
      return false;
    }
  };

  // List of enforced vertices
  typedef std::set< TEnfVertex*, CompareEnfVertices > TEnfVertexList;

  // Map Face Entry / List of enforced vertices
  typedef std::map< TEntry, TEnfVertexList > TFaceEntryEnfVertexListMap;

  // Map Face Entry / List of coords
  typedef std::map< TEntry, TEnfVertexCoordsList > TFaceEntryCoordsListMap;

  // Map Face Entry / List of Vertex entry
  typedef std::map< TEntry, TEntryList > TFaceEntryEnfVertexEntryListMap;
  
  // Map Coords / Enforced vertex
  typedef std::map< TEnfVertexCoords, TEnfVertex* > TCoordsEnfVertexMap;

  // Map Vertex entry / Enforced vertex
  typedef std::map< TEntry, TEnfVertex* > TEnfVertexEntryEnfVertexMap;

  typedef std::map< TEnfGroupName, std::set<int> > TGroupNameNodeIDMap;
  /* TODO GROUPS
  // Map Group Name / List of enforced vertices
  typedef std::map< TEnfGroupName , TEnfVertexList > TGroupNameEnfVertexListMap;
  */

  
//   TODO GROUPS
//   void SetEnforcedVertex(const TEntry& entry, double x, double y, double z, const TEnfGroupName& groupName="");
  bool                  SetEnforcedVertex(TEntry theFaceEntry, TEnfName theVertexName, TEntry theVertexEntry, TEnfGroupName theGroupName,
                                          double x = 0.0, double y = 0.0, double z = 0.0);
  TEnfVertexList        GetEnfVertexList(const TEntry& theFaceEntry) throw (std::invalid_argument);
  TEnfVertexCoordsList  GetEnfVertexCoordsList(const TEntry& theFaceEntry) throw (std::invalid_argument);
  TEntryList            GetEnfVertexEntryList (const TEntry& theFaceEntry) throw (std::invalid_argument);
  TEnfVertex*           GetEnfVertex(TEnfVertexCoords coords) throw (std::invalid_argument);
  TEnfVertex*           GetEnfVertex(const TEntry& theEnfVertexEntry) throw (std::invalid_argument);
  void                  AddEnfVertexNodeID(TEnfGroupName theGroupName,int theNodeID);
  std::set<int>         GetEnfVertexNodeIDs(TEnfGroupName theGroupName) throw (std::invalid_argument);
  void                  RemoveEnfVertexNodeID(TEnfGroupName theGroupName,int theNodeID) throw (std::invalid_argument);
  
  bool ClearEnforcedVertex(const TEntry& theFaceEntry, double x = 0.0, double y = 0.0, double z = 0.0, const TEntry& theVertexEntry="") throw (std::invalid_argument);
  bool ClearEnforcedVertices(const TEntry& theFaceEntry) throw (std::invalid_argument);

  void ClearAllEnforcedVertices();

  const TFaceEntryEnfVertexListMap  _GetAllEnforcedVerticesByFace() const { return _faceEntryEnfVertexListMap; }
  const TEnfVertexList              _GetAllEnforcedVertices() const { return _enfVertexList; }

  const TFaceEntryCoordsListMap     _GetAllCoordsByFace() const { return _faceEntryCoordsListMap; }
  const TCoordsEnfVertexMap         _GetAllEnforcedVerticesByCoords() const { return _coordsEnfVertexMap; }

  const TFaceEntryEnfVertexEntryListMap _GetAllEnfVertexEntriesByFace() const { return _faceEntryEnfVertexEntryListMap; }
  const TEnfVertexEntryEnfVertexMap     _GetAllEnforcedVerticesByEnfVertexEntry() const { return _enfVertexEntryEnfVertexMap; }

//   TODO GROUPS
//   const TEnfVertexGroupNameMap _GetEnforcedVertexGroupNameMap() const { return _enfVertexGroupNameMap; }
  

  /*!
   * \brief Return the enforced vertices
   */
  static TFaceEntryEnfVertexListMap       GetAllEnforcedVerticesByFace(const BLSURFPlugin_Hypothesis* hyp);
  static TEnfVertexList                   GetAllEnforcedVertices(const BLSURFPlugin_Hypothesis* hyp);

  static TFaceEntryCoordsListMap          GetAllCoordsByFace(const BLSURFPlugin_Hypothesis* hyp);
  static TCoordsEnfVertexMap              GetAllEnforcedVerticesByCoords(const BLSURFPlugin_Hypothesis* hyp);

  static TFaceEntryEnfVertexEntryListMap  GetAllEnfVertexEntriesByFace(const BLSURFPlugin_Hypothesis* hyp);
  static TEnfVertexEntryEnfVertexMap      GetAllEnforcedVerticesByEnfVertexEntry(const BLSURFPlugin_Hypothesis* hyp);

  
  /*!
    * \brief Set/get node group to an enforced vertex
    */
  /* TODO GROUPS
  void SetEnforcedVertexGroupName(double x, double y, double z, const TEnfGroupName& groupName) throw (std::invalid_argument);
  TEnfGroupName GetEnforcedVertexGroupName(double x, double y, double z) throw (std::invalid_argument);
  TEnfVertexList GetEnforcedVertexByGroupName(TEnfGroupName& groupName) throw (std::invalid_argument);
  */

  static Topology        GetDefaultTopology();
  static PhysicalMesh    GetDefaultPhysicalMesh();
  static double          GetDefaultPhySize();
  static double          GetDefaultMaxSize();
  static double          GetDefaultMinSize();
  static GeometricMesh   GetDefaultGeometricMesh();
  static double          GetDefaultAngleMeshS();
  static double          GetDefaultAngleMeshC() { return GetDefaultAngleMeshS(); }
  static double          GetDefaultGradation();
  static bool            GetDefaultQuadAllowed();
  static bool            GetDefaultDecimesh();
  static int             GetDefaultVerbosity() { return 10; }
  static TSizeMap        GetDefaultSizeMap() { return TSizeMap();}

  static TFaceEntryEnfVertexListMap       GetDefaultFaceEntryEnfVertexListMap() { return TFaceEntryEnfVertexListMap(); }
  static TEnfVertexList                   GetDefaultEnfVertexList() { return TEnfVertexList(); }
  static TFaceEntryCoordsListMap          GetDefaultFaceEntryCoordsListMap() { return TFaceEntryCoordsListMap(); }
  static TCoordsEnfVertexMap              GetDefaultCoordsEnfVertexMap() { return TCoordsEnfVertexMap(); }
  static TFaceEntryEnfVertexEntryListMap  GetDefaultFaceEntryEnfVertexEntryListMap() { return TFaceEntryEnfVertexEntryListMap(); }
  static TEnfVertexEntryEnfVertexMap      GetDefaultEnfVertexEntryEnfVertexMap() { return TEnfVertexEntryEnfVertexMap(); }
  static TGroupNameNodeIDMap              GetDefaultGroupNameNodeIDMap() { return TGroupNameNodeIDMap(); }

  /* TODO GROUPS
  static TGroupNameEnfVertexListMap GetDefaultGroupNameEnfVertexListMap() { return TGroupNameEnfVertexListMap(); }
  static TEnfVertexGroupNameMap     GetDefaultEnfVertexGroupNameMap() { return TEnfVertexGroupNameMap(); }
  */

  static double undefinedDouble() { return -1.0; }

  typedef std::map< std::string, std::string > TOptionValues;
  typedef std::set< std::string >              TOptionNames;

  void SetOptionValue(const std::string& optionName,
                      const std::string& optionValue) throw (std::invalid_argument);
  std::string GetOptionValue(const std::string& optionName) throw (std::invalid_argument);
  void ClearOption(const std::string& optionName);
  const TOptionValues& GetOptionValues() const { return _option2value; }

  // Persistence
  virtual std::ostream & SaveTo(std::ostream & save);
  virtual std::istream & LoadFrom(std::istream & load);
  friend std::ostream & operator <<(std::ostream & save, BLSURFPlugin_Hypothesis & hyp);
  friend std::istream & operator >>(std::istream & load, BLSURFPlugin_Hypothesis & hyp);

  /*!
   * \brief Does nothing
   * \param theMesh - the built mesh
   * \param theShape - the geometry of interest
   * \retval bool - always false
   */
  virtual bool SetParametersByMesh(const SMESH_Mesh* theMesh, const TopoDS_Shape& theShape);

  /*!
   * \brief Initialize my parameter values by default parameters.
   *  \retval bool - true if parameter values have been successfully defined
   */
  virtual bool SetParametersByDefaults(const TDefaults& dflts, const SMESH_Mesh* theMesh=0);


private:
  Topology        _topology;
  PhysicalMesh    _physicalMesh;
  double          _phySize, _phyMin, _phyMax;
  GeometricMesh   _geometricMesh;
  double          _angleMeshS, _angleMeshC, _hgeoMin, _hgeoMax;
  double          _gradation;
  bool            _quadAllowed;
  bool            _decimesh;
  int             _verb;
  TOptionValues   _option2value;
  TOptionNames    _doubleOptions, _charOptions;
  TSizeMap        _sizeMap;
  TSizeMap        _attractors;

  TFaceEntryEnfVertexListMap  _faceEntryEnfVertexListMap;
  TEnfVertexList              _enfVertexList;
  // maps to get "manual" enf vertex (through their coordinates)
  TFaceEntryCoordsListMap     _faceEntryCoordsListMap;
  TCoordsEnfVertexMap         _coordsEnfVertexMap;
  // maps to get "geom" enf vertex (through their geom entries)
  TFaceEntryEnfVertexEntryListMap _faceEntryEnfVertexEntryListMap;
  TEnfVertexEntryEnfVertexMap     _enfVertexEntryEnfVertexMap;
  TGroupNameNodeIDMap             _groupNameNodeIDMap;
  
  /* TODO GROUPS
  TGroupNameEnfVertexListMap _groupNameEnfVertexListMap;
  */
//   TSizeMap      _customSizeMap;
};

#endif
