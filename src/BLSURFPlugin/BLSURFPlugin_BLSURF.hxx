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
// File    : BLSURFPlugin_BLSURF.hxx
// Authors : Francis KLOSS (OCC) & Patrick LAUG (INRIA) & Lioka RAZAFINDRAZAKA (CEA)
//           & Aurelien ALLEAUME (DISTENE)
// ---
//
#ifndef _BLSURFPlugin_BLSURF_HXX_
#define _BLSURFPlugin_BLSURF_HXX_

#include "SMESH_2D_Algo.hxx"
#include "SMESH_Mesh.hxx"
#include "Utils_SALOME_Exception.hxx"
extern "C"{
#include "distene/api.h"
}

class BLSURFPlugin_Hypothesis;

class BLSURFPlugin_BLSURF: public SMESH_2D_Algo {
  public:
    BLSURFPlugin_BLSURF(int hypId, int studyId, SMESH_Gen* gen);

    virtual ~BLSURFPlugin_BLSURF();

    virtual bool CheckHypothesis(SMESH_Mesh&                          aMesh,
                                 const TopoDS_Shape&                  aShape,
                                 SMESH_Hypothesis::Hypothesis_Status& aStatus);

    void SetParameters(const BLSURFPlugin_Hypothesis* hyp, blsurf_session_t *bls);

    virtual bool Compute(SMESH_Mesh& aMesh, const TopoDS_Shape& aShape);

    ostream & SaveTo(ostream & save);
    istream & LoadFrom(istream & load);
    friend ostream & operator << (ostream & save, BLSURFPlugin_BLSURF & hyp);
    friend istream & operator >> (istream & load, BLSURFPlugin_BLSURF & hyp);

  protected:
    const BLSURFPlugin_Hypothesis* _hypothesis;
};

#endif
