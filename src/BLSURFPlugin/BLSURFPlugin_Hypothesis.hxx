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
// File      : BLSURFPlugin_Hypothesis.hxx
// Authors   : Francis KLOSS (OCC) & Patrick LAUG (INRIA) & Lioka RAZAFINDRAZAKA (CEA)
//             & Aurelien ALLEAUME (DISTENE)
// Date      : 27/03/2006
// Project   : SALOME
// $Header$
//=============================================================================

#ifndef _BLSURFPlugin_Hypothesis_HXX_
#define _BLSURFPlugin_Hypothesis_HXX_

#include "SMESH_Hypothesis.hxx"
#include <map>
#include <set>
#include <stdexcept>

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

  static Topology      GetDefaultTopology();
  static PhysicalMesh  GetDefaultPhysicalMesh();
  static double        GetDefaultPhySize();
  static double        GetDefaultMaxSize();
  static double        GetDefaultMinSize();
  static GeometricMesh GetDefaultGeometricMesh();
  static double        GetDefaultAngleMeshS();
  static double        GetDefaultAngleMeshC() { return GetDefaultAngleMeshS(); }
  static double        GetDefaultGradation();
  static bool          GetDefaultQuadAllowed();
  static bool          GetDefaultDecimesh();
  static int           GetDefaultVerbosity() { return 10; }

  static double undefinedDouble() { return -1.0; }

  typedef std::map< std::string, std::string > TOptionValues;
  typedef std::set< std::string >              TOptionNames;

  void SetOptionValue(const std::string& optionName,
                      const std::string& optionValue) throw (std::invalid_argument);
  string GetOptionValue(const std::string& optionName) throw (std::invalid_argument);
  void ClearOption(const std::string& optionName);
  const TOptionValues& GetOptionValues() const { return _option2value; }

  // Persistence
  virtual ostream & SaveTo(ostream & save);
  virtual istream & LoadFrom(istream & load);
  friend ostream & operator <<(ostream & save, BLSURFPlugin_Hypothesis & hyp);
  friend istream & operator >>(istream & load, BLSURFPlugin_Hypothesis & hyp);

  /*!
   * \brief Does nothing
   * \param theMesh - the built mesh
   * \param theShape - the geometry of interest
   * \retval bool - always false
   */
  virtual bool SetParametersByMesh(const SMESH_Mesh* theMesh, const TopoDS_Shape& theShape);

private:
  Topology      _topology;
  PhysicalMesh  _physicalMesh;
  double        _phySize, _phyMin, _phyMax;
  GeometricMesh _geometricMesh;
  double        _angleMeshS, _angleMeshC, _hgeoMin, _hgeoMax;
  double        _gradation;
  bool          _quadAllowed;
  bool          _decimesh;
  int           _verb;
  TOptionValues _option2value;
  TOptionNames  _doubleOptions, _charOptions;
};

#endif
