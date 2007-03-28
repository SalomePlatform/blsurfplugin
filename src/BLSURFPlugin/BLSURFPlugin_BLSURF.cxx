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
// File    : BLSURFPlugin_BLSURF.cxx
// Authors : Francis KLOSS (OCC) & Patrick LAUG (INRIA) & Lioka RAZAFINDRAZAKA (CEA)
// Date    : 20/03/2006
// Project : SALOME
//=============================================================================
using namespace std;

#include "BLSURFPlugin_BLSURF.hxx"
#include "BLSURFPlugin_Hypothesis.hxx"

#include <SMESH_Gen.hxx>
#include <SMESH_Mesh.hxx>
#include <SMESH_ControlsDef.hxx>

#include <SMESHDS_Mesh.hxx>
#include <SMDS_MeshElement.hxx>
#include <SMDS_MeshNode.hxx>

#include <utilities.h>

#include <list>
#include <vector>

#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <NCollection_Map.hxx>

#include <cad_occ.h>

//=============================================================================
/*!
 *  
 */
//=============================================================================

BLSURFPlugin_BLSURF::BLSURFPlugin_BLSURF(int hypId, int studyId,
                                               SMESH_Gen* gen)
  : SMESH_2D_Algo(hypId, studyId, gen)
{
  MESSAGE("BLSURFPlugin_BLSURF::BLSURFPlugin_BLSURF");

  _name = "BLSURF";
  _shapeType = (1 << TopAbs_FACE); // 1 bit /shape type
  _compatibleHypothesis.push_back("BLSURF_Parameters");
  _requireDescretBoundary = false;
  _onlyUnaryInput = false;
  _hypothesis = NULL;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

BLSURFPlugin_BLSURF::~BLSURFPlugin_BLSURF()
{
  MESSAGE("BLSURFPlugin_BLSURF::~BLSURFPlugin_BLSURF");
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

bool BLSURFPlugin_BLSURF::CheckHypothesis
                         (SMESH_Mesh&                          aMesh,
                          const TopoDS_Shape&                  aShape,
                          SMESH_Hypothesis::Hypothesis_Status& aStatus)
{
  _hypothesis = NULL;

  list<const SMESHDS_Hypothesis*>::const_iterator itl;
  const SMESHDS_Hypothesis* theHyp;

  const list<const SMESHDS_Hypothesis*>& hyps = GetUsedHypothesis(aMesh, aShape);
  int nbHyp = hyps.size();
  if (!nbHyp)
  {
    aStatus = SMESH_Hypothesis::HYP_OK;
    return true;  // can work with no hypothesis
  }

  itl = hyps.begin();
  theHyp = (*itl); // use only the first hypothesis

  string hypName = theHyp->GetName();

  if (hypName == "BLSURF_Parameters")
  {
    _hypothesis = static_cast<const BLSURFPlugin_Hypothesis*> (theHyp);
    ASSERT(_hypothesis);
    aStatus = SMESH_Hypothesis::HYP_OK;
  }
  else
    aStatus = SMESH_Hypothesis::HYP_INCOMPATIBLE;

  return aStatus == SMESH_Hypothesis::HYP_OK;
}

//=============================================================================
/*!
 * Pass parameters to BLSURF
 */
//=============================================================================

void BLSURFPlugin_BLSURF::SetParameters(const BLSURFPlugin_Hypothesis* hyp) {
  if (hyp) {
    MESSAGE("BLSURFPlugin_BLSURF::SetParameters");
    _physicalMesh = (int) hyp->GetPhysicalMesh();
    _phySize = hyp->GetPhySize();
    _geometricMesh = (int) hyp->GetGeometricMesh();
    _angleMeshS = hyp->GetAngleMeshS();
    _gradation = hyp->GetGradation();
    _quadAllowed = hyp->GetQuadAllowed();
    _decimesh = hyp->GetDecimesh();
  }

  bool BlsurfEnvFile = true;

  if ( BlsurfEnvFile ) {
    TCollection_AsciiString SalomeExecDir;

    char * Dir = getenv("PWD");
    SalomeExecDir = Dir;
    SalomeExecDir += "/blsurf.env";

    ofstream theBlsurfEnv  ( SalomeExecDir.ToCString()  , ios::out);
    theBlsurfEnv << "verb 10"                          << endl;
    theBlsurfEnv << "hphy_flag "     << _physicalMesh  << endl;
    theBlsurfEnv << "hphydef "       << _phySize       << endl;
    theBlsurfEnv << "hgeo_flag "     << _geometricMesh << endl;
    theBlsurfEnv << "angle_meshs "   << _angleMeshS    << endl;
    theBlsurfEnv << "gradation "     << _gradation     << endl;
    theBlsurfEnv << "topo_points 1"                    << endl;
    theBlsurfEnv << "topo_project 1"                   << endl;
    theBlsurfEnv << "topo_curves 1"                    << endl;
    theBlsurfEnv << "surforient 1"                     << endl;
    theBlsurfEnv << "decim "         << _decimesh      << endl;
    theBlsurfEnv.close();
  }
}

//=============================================================================
/*!
 *
 */
//=============================================================================

bool BLSURFPlugin_BLSURF::Compute(SMESH_Mesh& aMesh, const TopoDS_Shape& aShape) {

  SetParameters(_hypothesis);

  MESSAGE("BLSURFPlugin_BLSURF::Compute");

  if (aShape.ShapeType() == TopAbs_COMPOUND) {
    cout << "  the shape is a COMPOUND" << endl;
  }
  else {
    cout << "  the shape is UNKNOWN" << endl;
  };

  int i=0;
  if (1) {
    for (TopExp_Explorer expf(aShape, TopAbs_FACE); expf.More(); expf.Next()) {
      const TopoDS_Shape& face = expf.Current();
      i++;
      int j=0;
      for (TopExp_Explorer expe(face, TopAbs_EDGE); expe.More(); expe.Next()) {
        const TopoDS_Shape& edge = expe.Current();
        j++;
        int k=0;
        for (TopExp_Explorer expv(edge, TopAbs_VERTEX); expv.More(); expv.Next()) {
          k++;
        }
        // cout << "  face " << i << " and its edge " << j << " has " << k << " vertices" << endl;
      }
      // cout << "  face " << i << " has " << j << " edges" << endl;
    }
    // cout << "  total number of faces = " << i << endl;
  }

  BL_SURF mesh;
  blw_* blw;

  cout << endl;
  cout << "Beginning of Surface Mesh generation" << endl;
  cout << endl;
  if (!mesh.init(aShape))
    return(false);
  cout << endl;
  cout << "End of Surface Mesh generation" << endl;
  cout << endl;
  mesh.get_blw(blw);

  SMESHDS_Mesh* meshDS = aMesh.GetMeshDS();

  if ( !_decimesh ) {
    /* cf. export_mesh_all */
    int j, ip, ic, nptri, is, oriented, iF, nF, idom, ndom, v[5], verb;
    double centre[3];
    char element[8];

    verb = blw->env.verb;
    blw->env.verb = 0;
    assign_bls_mesh_num(blw);
    blw->env.verb = verb;
  
    FOR (j, 0, 2) {
      centre[j] = (blw->bls_glo.xyzmin[j] + blw->bls_glo.xyzmax[j]) * 0.5;
      cout << "centre[" << j << "] : " << centre[j] << endl;
    }

    /* points sommets des edges et des triangles */
    nptri = blw->bls_mesh_num.number_of_nodes;
    if (blw->env.verb >= 10)
      fprintf(blw->out, "export_salome: surface mesh contains %d vertices\n", nptri);
    SMDS_MeshNode** nodes = new SMDS_MeshNode*[nptri+1];
    j = 0;
    FOR (ip, 1, nptri) {
      if (blw->bls_glo.vertices_xyz[3*ip-3] == BLHUGE) {
        if (++j <= 10) {
	      fprintf(blw->out, "export_salome: unconnected vertex %d\n", ip);
	      if (j == 10) fprintf(blw->out, "export_salome: ...\n");
        }
        nodes[ip] = meshDS->AddNode(centre[0], centre[1], centre[2]);
      }
      else {
        double floatVal = blw->bls_glo.vertices_xyz[3*ip-3];
        // cout << "j : " << j << " node nmuber : " << ip << " --> first coordinate = " << floatVal << " != " << BLHUGE << endl;
        nodes[ip] = meshDS->AddNode(blw->bls_glo.vertices_xyz[3*ip-3], blw->bls_glo.vertices_xyz[3*ip-2], blw->bls_glo.vertices_xyz[3*ip-1]);
        // cout << "nodes[" << ip << "] : " << blw->bls_glo.vertices_xyz[3*ip-3] << ", "
        //                                  << blw->bls_glo.vertices_xyz[3*ip-2] << ", "
        //                                  << blw->bls_glo.vertices_xyz[3*ip-1] << endl;
      }
    }

    /* edges */
    int nbEdges = C3D.number_of_curves;
    cout << "Number Of Edges : " << nbEdges << endl;
    FOR (ic, 1, C3D.number_of_curves) {
      //TopoDS_Edge topo_edge = mesh.all_edges[ic-1]->topology;
      int np = C3D.TC[ic].number_of_points;
      cout << "Number Of Nodes for edge " << ic << " : " << np << endl;
      FOR (ip, 1, np-1) {
        meshDS->AddEdge(nodes[C3D.TC[ic].iglopc[ip]], nodes[C3D.TC[ic].iglopc[ip+1]]);
      }
    }

    /* faces (triangles or quadrilaterals) */ 
    SMDS_MeshFace* face;
    if (blw->bls_glo.number_of_patches <= 0) {
      strcpy(element, "p1");
    }
    else {
      strcpy(element, TC2D[1].ms2d.element);
    }
    cout << endl;
    cout << "Number_of_patches    : " << blw->bls_glo.number_of_patches << endl;
    cout << "Element              : " << element << endl;
    cout << "TC2D[1].ms2d.element : " << TC2D[1].ms2d.element << endl;
    cout << endl;
    if (STREQUAL(element, "p1")) {
      if (blw->env.verb >= 10)
        fprintf(blw->out, "export_salome: surface mesh contains %d triangles\n", blw->bls_mesh_num.ntri);
      FOR (is, 1, blw->bls_glo.number_of_patches) {
        TopoDS_Face topo_face = mesh.surfaces[is-1]->topology;
        oriented = (TC2D[is].surforient >= 0);
        // cout << endl;
        // cout << "TC2D[is].surforient : " << TC2D[is].surforient << " --> oriented : " << oriented << endl;
        nF = TC2D[is].ms2d.jmax_F;
        ndom = TC2D[is].ms2d.ndom;
        // cout << "nF   : " << nF   << endl;
        // cout << "ndom : " << ndom << endl;
        // cout << endl;
        FOR (iF, 1, nF) {
          idom = TC2D[is].ms2d.FRef[iF];
          if (!(1 <= idom && idom <= ndom))
            continue;
          v[0] = TC2D[is].iglops[TC2D[is].ms2d.F[3*iF-3]];
          v[1] = TC2D[is].iglops[TC2D[is].ms2d.F[3*iF-2]];
          v[2] = TC2D[is].iglops[TC2D[is].ms2d.F[3*iF-1]];
          if (v[0]==v[1] || v[1]==v[2] || v[2]==v[0])
            continue;                                                 /* triangle degenere */
          // cout << "Triangle " << iF << " of face " << is;
          if ( oriented) {
            // cout << " doesn't need to be re-oriented" << endl;
            face = meshDS->AddFace(nodes[v[2]], nodes[v[1]], nodes[v[0]]);
          }
          else {
            // cout << " needs to be re-oriented" << endl;
            face = meshDS->AddFace(nodes[v[0]], nodes[v[1]], nodes[v[2]]);
          }
	      meshDS->SetMeshElementOnShape(face, topo_face);
        }
      }
    }
    delete nodes;
  }
  else {
    cout << "decimesh is started"  << endl;
    system("decimesh");
    cout << "decimesh is finished" << endl;

    FILE* fic = fopen("x_h100.mesh", "r");
    char buf[200];
    do {
      fscanf(fic, "%s\n", buf);
    }
    while (strcmp(buf, "Vertices")!=0);
    int n_vtx;
    fscanf(fic, "%d\n", &n_vtx);
    cout << "number of vertices: " << n_vtx << endl;

    SMDS_MeshNode** nodes = new SMDS_MeshNode*[n_vtx+1];
    double coo_x, coo_y, coo_z;
    for (int i=1; i<=n_vtx; i++) {
      fscanf(fic, "%lf %lf %lf %*d\n", &coo_x, &coo_y, &coo_z);
      nodes[i] = meshDS->AddNode(coo_x, coo_y, coo_z);
    }
    cout << "nodes are updated" << endl;

    fscanf(fic, "%*s\n");
    int n_tri, n1, n2, n3, iFace;
    SMDS_MeshFace* face;
    fscanf(fic, "%d\n", &n_tri);
    cout << "number of triangles: " << n_tri << endl;
    for (int i=0; i<n_tri; i++) {
      fscanf(fic, "%d %d %d %d\n", &n1, &n2, &n3, &iFace);
      face = meshDS->AddFace(nodes[n3], nodes[n2], nodes[n1]);
    }
    cout << "triangles are updated" << endl;

    char *keyWord;
    fscanf(fic, "%s\n", keyWord);
    cout << "keyWord : " << keyWord << endl;
    int n_edges;
    SMDS_MeshEdge* edge;
    fscanf(fic, "%d\n", &n_edges);
    cout << "number of edges: " << n_edges << endl;
    for (int i=0; i<n_edges; i++) {
      fscanf(fic, "%d %d %*d\n", &n1, &n2);
      edge = meshDS->AddEdge(nodes[n1], nodes[n2]);
      //meshDS->SetMeshElementOnShape(edge, topo_edge);
    }
    cout << "edges are updated, and finished" << endl;

    fscanf(fic, "%s\n", keyWord); cout << "keyWord : " << keyWord << endl;
    fscanf(fic, "%s\n", keyWord); cout << "keyWord : " << keyWord << endl;
    fscanf(fic, "%s\n", keyWord); cout << "keyWord : " << keyWord << endl;
    fscanf(fic, "%s\n", keyWord); cout << "keyWord : " << keyWord << endl;
    fscanf(fic, "%s\n", keyWord); cout << "keyWord : " << keyWord << endl;
    fscanf(fic, "%s\n", keyWord); cout << "keyWord : " << keyWord << endl;

    int number = fscanf(fic, "%lf %lf %*lf\n", &coo_x, &coo_y);
    cout << "coo_x  : " << coo_x << endl;
    cout << "coo_y  : " << coo_y << endl;
    cout << "number : " << number << endl;

    fclose(fic);
  }

  bool b = mesh.end();
  return(b);
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

ostream & BLSURFPlugin_BLSURF::SaveTo(ostream & save)
{
  return save;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

istream & BLSURFPlugin_BLSURF::LoadFrom(istream & load)
{
  return load;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

ostream & operator << (ostream & save, BLSURFPlugin_BLSURF & hyp)
{
  return hyp.SaveTo( save );
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

istream & operator >> (istream & load, BLSURFPlugin_BLSURF & hyp)
{
  return hyp.LoadFrom( load );
}
