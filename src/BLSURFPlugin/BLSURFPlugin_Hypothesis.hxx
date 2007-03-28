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
// Date      : 27/03/2006
// Project   : SALOME
// $Header$
//=============================================================================

#ifndef _BLSURFPlugin_Hypothesis_HXX_
#define _BLSURFPlugin_Hypothesis_HXX_

#include "SMESH_Hypothesis.hxx"
#include "Utils_SALOME_Exception.hxx"

//  Parameters for work of BLSURF

class BLSURFPlugin_Hypothesis: public SMESH_Hypothesis
{
public:
  BLSURFPlugin_Hypothesis(int hypId, int studyId, SMESH_Gen * gen);

  enum PhysicalMesh {
    DefaultSize,
    PhysicalUserDefined
  };

  enum GeometricMesh {
    DefaultGeom,
    UserDefined
  };

  void SetPhysicalMesh(PhysicalMesh thePhysicalMesh);
  PhysicalMesh GetPhysicalMesh() const { return _physicalMesh; }

  void SetPhySize(double thePhySize);
  double GetPhySize() const { return _phySize; }

  void SetGeometricMesh(GeometricMesh theGeometricMesh);
  GeometricMesh GetGeometricMesh() const { return _geometricMesh; }

  void SetAngleMeshS(double theAngle);
  double GetAngleMeshS() const { return _angleMeshS; }

  void SetGradation(double theGradation);
  double GetGradation() const { return _gradation; }

  void SetQuadAllowed(bool theVal);
  bool GetQuadAllowed() const { return _quadAllowed; }

  void SetDecimesh(bool theVal);
  bool GetDecimesh() const { return _decimesh; }

  static PhysicalMesh GetDefaultPhysicalMesh();
  static double GetDefaultPhySize();
  static GeometricMesh GetDefaultGeometricMesh();
  static double GetDefaultAngleMeshS();
  static double GetDefaultGradation();
  static bool GetDefaultQuadAllowed();
  static bool GetDefaultDecimesh();

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
  PhysicalMesh  _physicalMesh;
  double        _phySize;
  GeometricMesh _geometricMesh;
  double        _angleMeshS;
  double        _gradation;
  bool          _quadAllowed;
  bool          _decimesh;
};

#endif
