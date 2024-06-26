// Copyright (C) 2007-2024  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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
// File    : BLSURFPlugin_BLSURF_i.cxx
// Authors : Francis KLOSS (OCC) & Patrick LAUG (INRIA)
// ---
//
#include "BLSURFPlugin_BLSURF_i.hxx"
#include "SMESH_Gen.hxx"

#include "Utils_CorbaException.hxx"
#include "utilities.h"

//=============================================================================
/*!
 *  BLSURFPlugin_BLSURF_i::BLSURFPlugin_BLSURF_i
 *
 *  Constructor
 */
//=============================================================================

BLSURFPlugin_BLSURF_i::BLSURFPlugin_BLSURF_i( PortableServer::POA_ptr thePOA,
                                              ::SMESH_Gen*            theGenImpl,
                                              bool                    theHasGEOM )
     : SALOME::GenericObj_i( thePOA ), 
       SMESH_Hypothesis_i( thePOA ), 
       SMESH_Algo_i( thePOA ),
       SMESH_2D_Algo_i( thePOA )
{
  myBaseImpl = new ::BLSURFPlugin_BLSURF( theGenImpl->GetANewId(),
                                          theGenImpl,
                                          theHasGEOM);
}

//=============================================================================
/*!
 *  BLSURFPlugin_BLSURF_i::~BLSURFPlugin_BLSURF_i
 *
 *  Destructor
 */
//=============================================================================

BLSURFPlugin_BLSURF_i::~BLSURFPlugin_BLSURF_i()
{
}

//=============================================================================
/*!
 *  BLSURFPlugin_BLSURF_i::GetImpl
 *
 *  Get implementation
 */
//=============================================================================

::BLSURFPlugin_BLSURF* BLSURFPlugin_BLSURF_i::GetImpl()
{
  return ( ::BLSURFPlugin_BLSURF* )myBaseImpl;
}
