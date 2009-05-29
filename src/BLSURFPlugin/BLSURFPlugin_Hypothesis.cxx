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
// ---
// File    : BLSURFPlugin_Hypothesis.cxx
// Authors : Francis KLOSS (OCC) & Patrick LAUG (INRIA) & Lioka RAZAFINDRAZAKA (CEA)
//           & Aurelien ALLEAUME (DISTENE)
//           Size maps developement: Nicolas GEIMER (OCC) & Gilles DAVID (EURIWARE)
// ---
//
#include "BLSURFPlugin_Hypothesis.hxx"
#include <utilities.h>
#include <cstring>

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
  
  _sizeMap.clear();
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
void BLSURFPlugin_Hypothesis::SetOptionValue(const std::string& optionName,
                                             const std::string& optionValue)
  throw (std::invalid_argument)
{
  TOptionValues::iterator op_val = _option2value.find( optionName );
  if ( op_val == _option2value.end() ) {
    std::string msg = "Unknown BLSURF option: '" + optionName + "'";
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
    std::string typeName;
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
      std::string msg = "Advanced option '" + optionName + "' = '" + optionValue +
        "' but must be " + typeName;
      throw std::invalid_argument(msg);
    }
    op_val->second = optionValue;
    NotifySubMeshesHypothesisModification();
  }
}

//=============================================================================
std::string BLSURFPlugin_Hypothesis::GetOptionValue(const std::string& optionName)
  throw (std::invalid_argument)
{
  TOptionValues::iterator op_val = _option2value.find( optionName );
  if ( op_val == _option2value.end() ) {
    std::string msg = "Unknown BLSURF option: <";
    msg += optionName + ">";
    throw std::invalid_argument(msg);
  }
  return op_val->second;
}

//=============================================================================
void BLSURFPlugin_Hypothesis::ClearOption(const std::string& optionName)
{
  TOptionValues::iterator op_val = _option2value.find( optionName );
  if ( op_val != _option2value.end() )
    op_val->second.clear();
}


void  BLSURFPlugin_Hypothesis::SetSizeMapEntry(const std::string& entry,const std::string& sizeMap)
{
  if (_sizeMap[entry].compare(sizeMap) != 0) {
    _sizeMap[entry]=sizeMap;
    NotifySubMeshesHypothesisModification();
  }
}

std::string  BLSURFPlugin_Hypothesis::GetSizeMapEntry(const std::string& entry)
{
 TSizeMap::iterator it  = _sizeMap.find( entry );
 if ( it != _sizeMap.end() )
   return it->second;
 else
   return "No_Such_Entry";
}

void  BLSURFPlugin_Hypothesis::SetAttractorEntry(const std::string& entry,const std::string& attractor)
{
  if (_attractors[entry].compare(attractor) != 0) {
    _attractors[entry]=attractor;
    NotifySubMeshesHypothesisModification();
  }
}

std::string  BLSURFPlugin_Hypothesis::GetAttractorEntry(const std::string& entry)
{
 TSizeMap::iterator it  = _attractors.find( entry );
 if ( it != _attractors.end() )
   return it->second;
 else
   return "No_Such_Entry";
}


void BLSURFPlugin_Hypothesis::ClearEntry(const std::string& entry)
{
 TSizeMap::iterator it  = _sizeMap.find( entry );
 if ( it != _sizeMap.end() ) {
   _sizeMap.erase(it);
   NotifySubMeshesHypothesisModification();
 }
 else {
   TSizeMap::iterator itAt  = _attractors.find( entry );
   if ( itAt != _attractors.end() ) {
     _attractors.erase(itAt);
     NotifySubMeshesHypothesisModification();
   }
   else
     std::cout<<"No_Such_Entry"<<std::endl;
 }
}


void BLSURFPlugin_Hypothesis::ClearSizeMaps()
{
  _sizeMap.clear();
  _attractors.clear();
}



//=============================================================================
std::ostream & BLSURFPlugin_Hypothesis::SaveTo(std::ostream & save)
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
  if (op_val != _option2value.end()) {
    save << " " << "__OPTIONS_BEGIN__";
    for ( ; op_val != _option2value.end(); ++op_val ) {
      if ( !op_val->second.empty() )
        save << " " << op_val->first
             << " " << op_val->second << "%#"; // "%#" is a mark of value end
    }
    save << " " << "__OPTIONS_END__";
  }

  TSizeMap::iterator it_sm  = _sizeMap.begin();
  if (it_sm != _sizeMap.end()) {
    save << " " << "__SIZEMAP_BEGIN__";
    for ( ; it_sm != _sizeMap.end(); ++it_sm ) {
        save << " " << it_sm->first
             << " " << it_sm->second << "%#"; // "%#" is a mark of value end
    }
    save << " " << "__SIZEMAP_END__";
  }

  TSizeMap::iterator it_at  = _attractors.begin();
  if (it_at != _attractors.end()) {
    save << " " << "__ATTRACTORS_BEGIN__";
    for ( ; it_at != _attractors.end(); ++it_at ) {
        save << " " << it_at->first
             << " " << it_at->second << "%#"; // "%#" is a mark of value end
    }
    save << " " << "__ATTRACTORS_END__";
  }
  
  
  return save;
}

//=============================================================================
std::istream & BLSURFPlugin_Hypothesis::LoadFrom(std::istream & load)
{
  bool isOK = true;
  int i;
  double val;

  isOK = (load >> i);
  if (isOK)
    _topology = (Topology) i;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> i);
  if (isOK)
    _physicalMesh = (PhysicalMesh) i;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> i);
  if (isOK)
    _geometricMesh = (GeometricMesh) i;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    _phySize = val;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    _angleMeshS = val;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    _gradation = val;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> i);
  if (isOK)
    _quadAllowed = (bool) i;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> i);
  if (isOK)
    _decimesh = (bool) i;
  else
    load.clear(std::ios::badbit | load.rdstate());

  isOK = (load >> val);
  if (isOK)
    _phyMin = val;
  else
    load.clear(std::ios::badbit | load.rdstate());
  
  isOK = (load >> val);
  if (isOK)
    _phyMax = val;
  else
    load.clear(std::ios::badbit | load.rdstate());
  
  isOK = (load >> val);
  if (isOK)
    _angleMeshC = val;
  else
    load.clear(std::ios::badbit | load.rdstate());
  
  isOK = (load >> val);
  if (isOK)
    _hgeoMin = val;
  else
    load.clear(std::ios::badbit | load.rdstate());
  
  isOK = (load >> val);
  if (isOK)
    _hgeoMax = val;
  else
    load.clear(std::ios::badbit | load.rdstate());
  
  isOK = (load >> i);
  if (isOK)
    _verb = i;
  else
    load.clear(std::ios::badbit | load.rdstate());

  std::string option_or_sm;
  bool hasOptions = false;
  bool hasSizeMap = false;
  bool hasAttractor = false;

  isOK = (load >> option_or_sm);
  if (isOK)
    if (option_or_sm == "__OPTIONS_BEGIN__")
      hasOptions = true;
    else if (option_or_sm == "__SIZEMAP_BEGIN__")
      hasSizeMap = true;
    else if (option_or_sm == "__ATTRACTORS_BEGIN__")
      hasAttractor = true;

  std::string optName, optValue;
  while (isOK && hasOptions) {
    isOK = (load >> optName);
    if (isOK) {
      if (optName == "__OPTIONS_END__")
        break;
      isOK = (load >> optValue);
    }
    if (isOK) {
      std::string & value = _option2value[ optName ];
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

  if (hasOptions) {
    isOK = (load >> option_or_sm);
    if (isOK)
      if (option_or_sm == "__SIZEMAP_BEGIN__")
        hasSizeMap = true;
      else if (option_or_sm == "__ATTRACTORS_BEGIN__")
        hasAttractor = true;
  }

  std::string smEntry, smValue;
  while (isOK && hasSizeMap) {
    isOK = (load >> smEntry);
    if (isOK) {
      if (smEntry == "__SIZEMAP_END__")
        break;
      isOK = (load >> smValue);
    }
    if (isOK) {
      std::string & value2 = _sizeMap[ smEntry ];
      value2 = smValue;
      int len2= value2.size();
      // continue reading until "%#" encountered
      while ( value2[len2-1] != '#' || value2[len2-2] != '%' )
      {
        isOK = (load >> smValue);
        if (isOK) {
          value2 += " ";
          value2 += smValue;
          len2 = value2.size();
        }
        else {
          break;
        }
      }
      value2[ len2-2 ] = '\0'; //cut off "%#"
    }
  }

  if (hasSizeMap) {
    isOK = (load >> option_or_sm);
    if (isOK && option_or_sm == "__ATTRACTORS_BEGIN__")
      hasAttractor = true;
  }

  std::string atEntry, atValue;
  while (isOK && hasAttractor) {
    isOK = (load >> atEntry);
    if (isOK) {
      if (atEntry == "__ATTRACTORS_END__")
        break;
      isOK = (load >> atValue);
    }
    if (isOK) {
      std::string & value3 = _attractors[ atEntry ];
      value3 = atValue;
      int len3= value3.size();
      // continue reading until "%#" encountered
      while ( value3[len3-1] != '#' || value3[len3-2] != '%' )
      {
        isOK = (load >> atValue);
        if (isOK) {
          value3 += " ";
          value3 += atValue;
          len3 = value3.size();
        }
        else {
          break;
        }
      }
      value3[ len3-2 ] = '\0'; //cut off "%#"
    }
  }

  return load;
}

//=============================================================================
std::ostream & operator <<(std::ostream & save, BLSURFPlugin_Hypothesis & hyp)
{
  return hyp.SaveTo( save );
}

//=============================================================================
std::istream & operator >>(std::istream & load, BLSURFPlugin_Hypothesis & hyp)
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
/*!
 * \brief Initialize my parameter values by default parameters.
 *  \retval bool - true if parameter values have been successfully defined
 */
//=============================================================================

bool BLSURFPlugin_Hypothesis::SetParametersByDefaults(const TDefaults&   dflts,
                                                      const SMESH_Mesh* theMesh)
{
  return bool( _phySize = dflts._elemLength );
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
