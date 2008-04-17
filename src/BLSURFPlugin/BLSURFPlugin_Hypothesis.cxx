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
// File      : BLSURFPlugin_Hypothesis.cxx
// Authors   : Francis KLOSS (OCC) & Patrick LAUG (INRIA) & Lioka RAZAFINDRAZAKA (CEA)
//             & Aurelien ALLEAUME (DISTENE)
// Date      : 28/03/2006
// Project   : SALOME
//=============================================================================

using namespace std;
#include <BLSURFPlugin_Hypothesis.hxx>
#include <utilities.h>

//=============================================================================
/*!
 *  
 */
//=============================================================================
BLSURFPlugin_Hypothesis::BLSURFPlugin_Hypothesis (int hypId, int studyId,
                                                  SMESH_Gen * gen)
  : SMESH_Hypothesis(hypId, studyId, gen),
    _topology(GetDefaultTopology()),
    _physicalMesh(GetDefaultPhysicalMesh()),
    _phySize(GetDefaultPhySize()),
    _geometricMesh(GetDefaultGeometricMesh()),
    _angleMeshS(GetDefaultAngleMeshS()),
    _gradation(GetDefaultGradation()),
    _quadAllowed(GetDefaultQuadAllowed()),
    _decimesh(GetDefaultDecimesh())
{
  _name = "BLSURF_Parameters";
  _param_algo_dim = 2;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================
void BLSURFPlugin_Hypothesis::SetTopology(Topology theTopology)
{
  if (theTopology != _topology) {
    _topology = theTopology;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
/*!
 *  
 */
//=============================================================================
void BLSURFPlugin_Hypothesis::SetPhysicalMesh(PhysicalMesh thePhysicalMesh)
{
  if (thePhysicalMesh != _physicalMesh) {
    _physicalMesh = thePhysicalMesh;
    switch( _physicalMesh ) {
      case DefaultSize:
      default:
        _phySize = GetDefaultPhySize();
        break;
      }
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
/*!
 *  
 */
//=============================================================================
void BLSURFPlugin_Hypothesis::SetPhySize(double theVal)
{
  if (theVal != _phySize) {
    _phySize = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
/*!
 *  
 */
//=============================================================================
void BLSURFPlugin_Hypothesis::SetGeometricMesh(GeometricMesh theGeometricMesh)
{
  if (theGeometricMesh != _geometricMesh) {
    _geometricMesh = theGeometricMesh;
    switch( _geometricMesh ) {
      case DefaultGeom:
      default:
        _angleMeshS = GetDefaultAngleMeshS();
        _gradation  = GetDefaultGradation();
        break;
      }
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
/*!
 *  
 */
//=============================================================================
void BLSURFPlugin_Hypothesis::SetAngleMeshS(double theVal)
{
  if (theVal != _angleMeshS) {
    _angleMeshS = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
/*!
 *  
 */
//=============================================================================
void BLSURFPlugin_Hypothesis::SetGradation(double theVal)
{
  if (theVal != _gradation) {
    _gradation = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
/*!
 *  
 */
//=============================================================================
void BLSURFPlugin_Hypothesis::SetQuadAllowed(bool theVal)
{
  if (theVal != _quadAllowed) {
    _quadAllowed = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
/*!
 *  
 */
//=============================================================================
void BLSURFPlugin_Hypothesis::SetDecimesh(bool theVal)
{
  if (theVal != _decimesh) {
    _decimesh = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
/*!
 *  
 */
//=============================================================================
ostream & BLSURFPlugin_Hypothesis::SaveTo(ostream & save)
{
  save << " " << (int)_topology
       << " " << (int)_physicalMesh
       << " " << (int)_geometricMesh
       << " " << _phySize
       << " " << _angleMeshS   
       << " " << _gradation     
       << " " << (int)_quadAllowed 
       << " " << (int)_decimesh;

  return save;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================
istream & BLSURFPlugin_Hypothesis::LoadFrom(istream & load)
{
  bool isOK = true;
  int i;
  double val;

  isOK = (load >> i);
  if (isOK)
    _topology = (Topology) i;
  else
    load.clear(ios::badbit | load.rdstate());

  isOK = (load >> i);
  if (isOK)
    _physicalMesh = (PhysicalMesh) i;
  else
    load.clear(ios::badbit | load.rdstate());

  isOK = (load >> i);
  if (isOK)
    _geometricMesh = (GeometricMesh) i;
  else
    load.clear(ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    _phySize = val;
  else
    load.clear(ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    _angleMeshS = val;
  else
    load.clear(ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    _gradation = val;
  else
    load.clear(ios::badbit | load.rdstate());

  isOK = (load >> i);
  if (isOK)
    _quadAllowed = (bool) i;
  else
    load.clear(ios::badbit | load.rdstate());

  isOK = (load >> i);
  if (isOK)
    _decimesh = (bool) i;
  else
    load.clear(ios::badbit | load.rdstate());

  return load;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================
ostream & operator <<(ostream & save, BLSURFPlugin_Hypothesis & hyp)
{
  return hyp.SaveTo( save );
}

//=============================================================================
/*!
 *  
 */
//=============================================================================
istream & operator >>(istream & load, BLSURFPlugin_Hypothesis & hyp)
{
  return hyp.LoadFrom( load );
}


//================================================================================
/*!
 * \brief Does nothing
 * \param theMesh - the built mesh
 * \param theShape - the geometry of interest
 * \retval bool - always false
 */
//================================================================================
bool BLSURFPlugin_Hypothesis::SetParametersByMesh(const SMESH_Mesh*   theMesh,
                                                      const TopoDS_Shape& theShape)
{
  return false;
}

//=============================================================================
BLSURFPlugin_Hypothesis::Topology BLSURFPlugin_Hypothesis::GetDefaultTopology()
{
  return FromCAD;
}

//=============================================================================
BLSURFPlugin_Hypothesis::PhysicalMesh BLSURFPlugin_Hypothesis::GetDefaultPhysicalMesh()
{
  return PhysicalUserDefined;
}

//=============================================================================
double BLSURFPlugin_Hypothesis::GetDefaultPhySize()
{
  return 10;
}

//=============================================================================
BLSURFPlugin_Hypothesis::GeometricMesh BLSURFPlugin_Hypothesis::GetDefaultGeometricMesh()
{
  return DefaultGeom;
}

//=============================================================================
double BLSURFPlugin_Hypothesis::GetDefaultAngleMeshS()
{
  return 8;
}

//=============================================================================
double BLSURFPlugin_Hypothesis::GetDefaultGradation()
{
  return 1.1;
}

//=============================================================================
bool BLSURFPlugin_Hypothesis::GetDefaultQuadAllowed()
{
  return false;
}

//=============================================================================
bool BLSURFPlugin_Hypothesis::GetDefaultDecimesh()
{
  return false;
}
