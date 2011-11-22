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
// File    : BLSURFPlugin_BLSURF.hxx
// Authors : Francis KLOSS (OCC) & Patrick LAUG (INRIA) & Lioka RAZAFINDRAZAKA (CEA)
//           & Aurelien ALLEAUME (DISTENE)
//           Size maps developement: Nicolas GEIMER (OCC) & Gilles DAVID (EURIWARE)
// ---
//
#ifndef _BLSURFPlugin_BLSURF_HXX_
#define _BLSURFPlugin_BLSURF_HXX_

#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

#ifdef HAVE_FINITE
#undef HAVE_FINITE            // VSR: avoid compilation warning on Linux : "HAVE_FINITE" redefined
#endif
// rnv: avoid compilation warning on Linux : "_POSIX_C_SOURCE" and "_XOPEN_SOURCE" are redefined
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>
#include "SMESH_2D_Algo.hxx"
#include "SMESH_Mesh.hxx"
#include <SMESHDS_Mesh.hxx>
#include <SMDS_MeshElement.hxx>
#include <SMDS_MeshNode.hxx>
#include <SMESH_Gen_i.hxx>
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOMEDS)
#include CORBA_CLIENT_HEADER(GEOM_Gen)
#include "Utils_SALOME_Exception.hxx"

extern "C"{
#include "distene/blsurf.h"
#include "distene/api.h"
#include "distene/precad.h"
}

#include <BRepClass_FaceClassifier.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTools.hxx>
#include <BRepAdaptor_HSurface.hxx>

#include "BLSURFPlugin_Hypothesis.hxx"

class TopoDS_Shape;

class BLSURFPlugin_BLSURF: public SMESH_2D_Algo {
  public:
    BLSURFPlugin_BLSURF(int hypId, int studyId, SMESH_Gen* gen);

    virtual ~BLSURFPlugin_BLSURF();

    virtual bool CheckHypothesis(SMESH_Mesh&                          aMesh,
                                 const TopoDS_Shape&                  aShape,
                                 SMESH_Hypothesis::Hypothesis_Status& aStatus);

    void SetParameters(const BLSURFPlugin_Hypothesis* hyp, blsurf_session_t *bls, precad_session_t *pcs, SMESH_Mesh& aMesh, bool *use_precad);

    virtual bool Compute(SMESH_Mesh& aMesh, const TopoDS_Shape& aShape);

#ifdef WITH_SMESH_CANCEL_COMPUTE
    virtual void CancelCompute();
    bool computeCanceled() { return _compute_canceled;};
#endif

    virtual bool Evaluate(SMESH_Mesh& aMesh, const TopoDS_Shape& aShape,
                          MapShapeNbElems& aResMap);

    ostream & SaveTo(ostream & save);
    istream & LoadFrom(istream & load);
    friend ostream & operator << (ostream & save, BLSURFPlugin_BLSURF & hyp);
    friend istream & operator >> (istream & load, BLSURFPlugin_BLSURF & hyp);

  protected:
    const BLSURFPlugin_Hypothesis* _hypothesis;

  private:
    TopoDS_Shape entryToShape(std::string entry);
    void createEnforcedVertexOnFace(TopoDS_Shape FaceShape, BLSURFPlugin_Hypothesis::TEnfVertexList enfVertexList);
    void Set_NodeOnEdge(SMESHDS_Mesh* meshDS, SMDS_MeshNode* node, const TopoDS_Shape& ed);
    void BRepClass_FaceClassifierPerform(BRepClass_FaceClassifier* fc, const TopoDS_Face& face, const gp_Pnt& P, const Standard_Real Tol);

  private:
      PyObject *          main_mod;
      PyObject *          main_dict;
      SALOMEDS::Study_var myStudy;
      SMESH_Gen_i*        smeshGen_i;

#ifdef WITH_SMESH_CANCEL_COMPUTE
      volatile bool _compute_canceled;
#endif
};

#endif
