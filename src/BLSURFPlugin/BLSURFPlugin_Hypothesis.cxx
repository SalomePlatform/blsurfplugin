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
// File      : BLSURFPlugin_Hypothesis.cxx
// Authors   : Francis KLOSS (OCC) & Patrick LAUG (INRIA) & Lioka RAZAFINDRAZAKA (CEA)
//             & Aurelien ALLEAUME (DISTENE)
// Date      : 28/03/2006
// Project   : SALOME
//=============================================================================
//
using namespace std;
#include <BLSURFPlugin_Hypothesis.hxx>
#include <utilities.h>

//=============================================================================
BLSURFPlugin_Hypothesis::BLSURFPlugin_Hypothesis (int hypId, int studyId,
                                                  SMESH_Gen * gen)
  : SMESH_Hypothesis(hypId, studyId, gen),
    _topology(GetDefaultTopology()),
    _physicalMesh(GetDefaultPhysicalMesh()),
    _phySize(GetDefaultPhySize()),
    _phyMax(GetDefaultMaxSize()),
    _phyMin(GetDefaultMinSize()),
    _hgeoMax(GetDefaultMaxSize()),
    _hgeoMin(GetDefaultMinSize()),
    _geometricMesh(GetDefaultGeometricMesh()),
    _angleMeshS(GetDefaultAngleMeshS()),
    _angleMeshC(GetDefaultAngleMeshC()),
    _gradation(GetDefaultGradation()),
    _quadAllowed(GetDefaultQuadAllowed()),
    _decimesh(GetDefaultDecimesh()),
    _verb( GetDefaultVerbosity() )
{
  _name = "BLSURF_Parameters";
  _param_algo_dim = 2;

  // to desable writing boundaries
  //_phyMin = _phyMax = _hgeoMin = _hgeoMax = undefinedDouble();
  

  const char* intOptionNames[] = {
    "addsurf_ivertex",
    "background",
    "CheckAdjacentEdges",
    "CheckCloseEdges",
    "CheckWellDefined",
    "coiter",
    "communication",
    "decim",
    "export_flag",
    "file_h",
    "frontal",
    "gridnu",
    "gridnv",
    "hinterpol_flag",
    "hmean_flag",
    "intermedfile",
    "memory",
    "normals",
    "optim",
    "pardom_flag",
    "pinch",
    "refs",
    "rigid",
    "surforient",
    "tconf",
    "topo_collapse",
    "" // mark of end
  };
  const char* doubleOptionNames[] = {
    "addsurf_angle",
    "addsurf_R",
    "addsurf_H",
    "addsurf_FG",
    "addsurf_r",
    "addsurf_PA",
    "angle_compcurv",
    "angle_ridge",
    "CoefRectangle",
    "eps_collapse",
    "eps_ends",
    "eps_pardom",
    "LSS",
    "topo_eps1",
    "topo_eps2",
    "" // mark of end
  };
  const char* charOptionNames[] = {
    "export_format",
    "export_option",
    "import_option",
    "prefix",
    "" // mark of end
  };

  int i = 0;
  while ( intOptionNames[i][0] )
    _option2value[ intOptionNames[i++] ].clear();

  i = 0;
  while ( doubleOptionNames[i][0] ) {
    _doubleOptions.insert( doubleOptionNames[i] );
    _option2value[ doubleOptionNames[i++] ].clear();
  }
  i = 0;
  while ( charOptionNames[i][0] ) {
    _charOptions.insert( charOptionNames[i] );
    _option2value[ charOptionNames[i++] ].clear();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetTopology(Topology theTopology)
{
  if (theTopology != _topology) {
    _topology = theTopology;
    NotifySubMeshesHypothesisModification();
  }
}

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
void BLSURFPlugin_Hypothesis::SetPhySize(double theVal)
{
  if (theVal != _phySize) {
    _phySize = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetPhyMin(double theMinSize)
{
  if (theMinSize != _phyMin) {
    _phyMin = theMinSize;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetPhyMax(double theMaxSize)
{
  if (theMaxSize != _phyMax) {
    _phyMax = theMaxSize;
    NotifySubMeshesHypothesisModification();
  }
}


//=============================================================================
void BLSURFPlugin_Hypothesis::SetGeoMin(double theMinSize)
{
  if (theMinSize != _hgeoMin) {
    _hgeoMin = theMinSize;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetGeoMax(double theMaxSize)
{
  if (theMaxSize != _hgeoMax) {
    _hgeoMax = theMaxSize;
    NotifySubMeshesHypothesisModification();
  }
}

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
void BLSURFPlugin_Hypothesis::SetAngleMeshS(double theVal)
{
  if (theVal != _angleMeshS) {
    _angleMeshS = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetAngleMeshC(double theVal)
{
  if (theVal != _angleMeshC) {
    _angleMeshC = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetGradation(double theVal)
{
  if (theVal != _gradation) {
    _gradation = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetQuadAllowed(bool theVal)
{
  if (theVal != _quadAllowed) {
    _quadAllowed = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetDecimesh(bool theVal)
{
  if (theVal != _decimesh) {
    _decimesh = theVal;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
void BLSURFPlugin_Hypothesis::SetVerbosity(int theVal)
{
  if (theVal != _verb) {
    _verb = theVal;
    NotifySubMeshesHypothesisModification();
  }
}
//=============================================================================
void BLSURFPlugin_Hypothesis::SetOptionValue(const string& optionName,
                                             const string& optionValue)
  throw (std::invalid_argument)
{
  TOptionValues::iterator op_val = _option2value.find( optionName );
  if ( op_val == _option2value.end() ) {
    string msg = "Unknown BLSURF option: '" + optionName + "'";
    throw std::invalid_argument(msg);
  }
  if ( op_val->second != optionValue ) {
    const char* ptr = optionValue.c_str();
    // strip white spaces
    while ( ptr[0] == ' ' )
      ptr++;
    int i = strlen( ptr );
    while ( i != 0 && ptr[i-1] == ' ')
      i--;
    // check value type
    bool typeOk = true;
    string typeName;
    if ( i == 0 ) {
      // empty string
    }
    else if ( _charOptions.find( optionName ) != _charOptions.end() ) {
      // do not check strings
    }
    else if ( _doubleOptions.find( optionName ) != _doubleOptions.end() ) {
      // check if value is double
      char * endPtr;
      strtod(ptr, &endPtr);
      typeOk = ( ptr != endPtr );
      typeName = "real";
    }
    else {
      // check if value is int
      char * endPtr;
      strtol(ptr, &endPtr,10);
      typeOk = ( ptr != endPtr );
      typeName = "integer";
    }
    if ( !typeOk ) {
      string msg = "Advanced option '" + optionName + "' = '" + optionValue +
        "' but must be " + typeName;
      throw std::invalid_argument(msg);
    }
    op_val->second = optionValue;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
string BLSURFPlugin_Hypothesis::GetOptionValue(const string& optionName)
  throw (std::invalid_argument)
{
  TOptionValues::iterator op_val = _option2value.find( optionName );
  if ( op_val == _option2value.end() ) {
    string msg = "Unknown BLSURF option: <";
    msg += optionName + ">";
    throw std::invalid_argument(msg);
  }
  return op_val->second;
}

//=============================================================================
void BLSURFPlugin_Hypothesis::ClearOption(const string& optionName)
{
  TOptionValues::iterator op_val = _option2value.find( optionName );
  if ( op_val != _option2value.end() )
    op_val->second.clear();
}

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
  save << " " << _phyMin
       << " " << _phyMax
       << " " << _angleMeshC
       << " " << _hgeoMin
       << " " << _hgeoMax
       << " " << _verb;

  TOptionValues::iterator op_val = _option2value.begin();
  for ( ; op_val != _option2value.end(); ++op_val ) {
    if ( !op_val->second.empty() )
      save << " " << op_val->first
           << " " << op_val->second << "%#"; // "%#" is a mark of value end
  }

  return save;
}

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

  isOK = (load >> val);
  if (isOK)
    _phyMin = val;
  else
    load.clear(ios::badbit | load.rdstate());
  
  isOK = (load >> val);
  if (isOK)
    _phyMax = val;
  else
    load.clear(ios::badbit | load.rdstate());
  
  isOK = (load >> val);
  if (isOK)
    _angleMeshC = val;
  else
    load.clear(ios::badbit | load.rdstate());
  
  isOK = (load >> val);
  if (isOK)
    _hgeoMin = val;
  else
    load.clear(ios::badbit | load.rdstate());
  
  isOK = (load >> val);
  if (isOK)
    _hgeoMax = val;
  else
    load.clear(ios::badbit | load.rdstate());
  
  isOK = (load >> i);
  if (isOK)
    _verb = i;
  else
    load.clear(ios::badbit | load.rdstate());

  string optName, optValue;
  while (isOK) {
    isOK = (load >> optName);
    if (isOK)
      isOK = (load >> optValue);
    if (isOK) {
      string & value = _option2value[ optName ];
      value = optValue;
      int len = value.size();
      // continue reading until "%#" encountered
      while ( value[len-1] != '#' || value[len-2] != '%' )
      {
        isOK = (load >> optValue);
        if (isOK) {
          value += " ";
          value += optValue;
          len = value.size();
        }
        else {
          break;
        }
      }
      value[ len-2 ] = '\0'; //cut off "%#"
    }
  }

  return load;
}

//=============================================================================
ostream & operator <<(ostream & save, BLSURFPlugin_Hypothesis & hyp)
{
  return hyp.SaveTo( save );
}

//=============================================================================
istream & operator >>(istream & load, BLSURFPlugin_Hypothesis & hyp)
{
  return hyp.LoadFrom( load );
}

//================================================================================
/*!
 * \brief Does nothing
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

//======================================================================
double BLSURFPlugin_Hypothesis::GetDefaultMaxSize()
{
  return undefinedDouble(); // 1e+4;
}

//======================================================================
double BLSURFPlugin_Hypothesis::GetDefaultMinSize()
{
  return undefinedDouble(); //1e-4;
}

//======================================================================
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
