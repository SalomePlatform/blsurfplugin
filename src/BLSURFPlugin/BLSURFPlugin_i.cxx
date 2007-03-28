//  SMESH BLSURFPlugin : implementaion of SMESH idl descriptions
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
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
//
//  File    : BLSURFPlugin_i.cxx
//  Authors : Francis KLOSS (OCC) & Patrick LAUG (INRIA)
//  Module  : SMESH

using namespace std;
#include "utilities.h"

#include "BLSURFPlugin_BLSURF_i.hxx"
#include "BLSURFPlugin_Hypothesis_i.hxx"

template <class T> class BLSURFPlugin_Creator_i:public HypothesisCreator_i<T>
{
  // as we have 'module BLSURFPlugin' in BLSURFPlugin_Algorithm.idl
  virtual std::string GetModuleName() { return "BLSURFPlugin"; }
};

//=============================================================================
/*!
 *
 */
//=============================================================================

extern "C"
{
  GenericHypothesisCreator_i* GetHypothesisCreator (const char* aHypName)
  {
    MESSAGE("GetHypothesisCreator " << aHypName);

    GenericHypothesisCreator_i* aCreator = 0;

    // Algorithms
    if (strcmp(aHypName, "BLSURF") == 0)
      aCreator = new BLSURFPlugin_Creator_i<BLSURFPlugin_BLSURF_i>;
    // Hypotheses
    else if (strcmp(aHypName, "BLSURF_Parameters") == 0)
      aCreator = new BLSURFPlugin_Creator_i<BLSURFPlugin_Hypothesis_i>;
    else ;

    return aCreator;
  }
}
