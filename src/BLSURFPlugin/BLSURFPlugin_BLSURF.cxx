// Copyright (C) 2006-2008 OPEN CASCADE, CEA/DEN, EDF R&D
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
//
// File    : BLSURFPlugin_BLSURF.cxx
// Authors : Francis KLOSS (OCC) & Patrick LAUG (INRIA) & Lioka RAZAFINDRAZAKA (CEA)
//           & Aurelien ALLEAUME (DISTENE)
//
// ---

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
#include <Standard_ErrorHandler.hxx>

extern "C"{
#include <distene/api.h>
}

#include <Geom_Surface.hxx>
#include <Handle_Geom_Surface.hxx>
#include <Geom2d_Curve.hxx>
#include <Handle_Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Handle_Geom_Curve.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt2d.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRepTools.hxx>

#ifndef WNT
#include <fenv.h>
#endif

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
    if ( _hypothesis->GetPhysicalMesh() == BLSURFPlugin_Hypothesis::DefaultSize &&
         _hypothesis->GetGeometricMesh() == BLSURFPlugin_Hypothesis::DefaultGeom )
      //  hphy_flag = 0 and hgeo_flag = 0 is not allowed (spec)
      aStatus = SMESH_Hypothesis::HYP_BAD_PARAMETER;
    else
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

inline std::string to_string(double d)
{
   std::ostringstream o;
   o << d;
   return o.str();
}

inline std::string to_string(int i)
{
   std::ostringstream o;
   o << i;
   return o.str();
}

void BLSURFPlugin_BLSURF::SetParameters(const BLSURFPlugin_Hypothesis* hyp, blsurf_session_t *bls)
{
  int    _topology      = BLSURFPlugin_Hypothesis::GetDefaultTopology();
  int    _physicalMesh  = BLSURFPlugin_Hypothesis::GetDefaultPhysicalMesh();
  double _phySize       = BLSURFPlugin_Hypothesis::GetDefaultPhySize();
  int    _geometricMesh = BLSURFPlugin_Hypothesis::GetDefaultGeometricMesh();
  double _angleMeshS    = BLSURFPlugin_Hypothesis::GetDefaultAngleMeshS();
  double _angleMeshC    = BLSURFPlugin_Hypothesis::GetDefaultAngleMeshC();
  double _gradation     = BLSURFPlugin_Hypothesis::GetDefaultGradation();
  bool   _quadAllowed   = BLSURFPlugin_Hypothesis::GetDefaultQuadAllowed();
  bool   _decimesh      = BLSURFPlugin_Hypothesis::GetDefaultDecimesh();
  int    _verb          = BLSURFPlugin_Hypothesis::GetDefaultVerbosity();

  if (hyp) {
    MESSAGE("BLSURFPlugin_BLSURF::SetParameters");
    _topology      = (int) hyp->GetTopology();
    _physicalMesh  = (int) hyp->GetPhysicalMesh();
    _phySize       = hyp->GetPhySize();
    _geometricMesh = (int) hyp->GetGeometricMesh();
    _angleMeshS    = hyp->GetAngleMeshS();
    _angleMeshC    = hyp->GetAngleMeshC();
    _gradation     = hyp->GetGradation();
    _quadAllowed   = hyp->GetQuadAllowed();
    _decimesh      = hyp->GetDecimesh();
    _verb          = hyp->GetVerbosity();

    if ( hyp->GetPhyMin() != ::BLSURFPlugin_Hypothesis::undefinedDouble() )
      blsurf_set_param(bls, "hphymin", to_string(hyp->GetPhyMin()).c_str());
    if ( hyp->GetPhyMax() != ::BLSURFPlugin_Hypothesis::undefinedDouble() )
      blsurf_set_param(bls, "hphymax", to_string(hyp->GetPhyMax()).c_str());
    if ( hyp->GetGeoMin() != ::BLSURFPlugin_Hypothesis::undefinedDouble() )
      blsurf_set_param(bls, "hgeomin", to_string(hyp->GetGeoMin()).c_str());
    if ( hyp->GetGeoMax() != ::BLSURFPlugin_Hypothesis::undefinedDouble() )
      blsurf_set_param(bls, "hgeomax", to_string(hyp->GetGeoMax()).c_str());

    const BLSURFPlugin_Hypothesis::TOptionValues & opts = hyp->GetOptionValues();
    BLSURFPlugin_Hypothesis::TOptionValues::const_iterator opIt;
    for ( opIt = opts.begin(); opIt != opts.end(); ++opIt )
      if ( !opIt->second.empty() ) {
#ifdef _DEBUG_
        cout << "blsurf_set_param(): " << opIt->first << " = " << opIt->second << endl;
#endif
        blsurf_set_param(bls, opIt->first.c_str(), opIt->second.c_str());
      }

  } else {
    MESSAGE("BLSURFPlugin_BLSURF::SetParameters using defaults");
  }
  
  blsurf_set_param(bls, "topo_points",       _topology > 0 ? "1" : "0");
  blsurf_set_param(bls, "topo_curves",       _topology > 0 ? "1" : "0");
  blsurf_set_param(bls, "topo_project",      _topology > 0 ? "1" : "0");
  blsurf_set_param(bls, "clean_boundary",    _topology > 1 ? "1" : "0");
  blsurf_set_param(bls, "close_boundary",    _topology > 1 ? "1" : "0");
  blsurf_set_param(bls, "hphy_flag",         to_string(_physicalMesh).c_str());
  blsurf_set_param(bls, "hphydef",           to_string(_phySize).c_str());
  blsurf_set_param(bls, "hgeo_flag",         to_string(_geometricMesh).c_str());
  blsurf_set_param(bls, "angle_meshs",       to_string(_angleMeshS).c_str());
  blsurf_set_param(bls, "angle_meshc",       to_string(_angleMeshC).c_str());
  blsurf_set_param(bls, "gradation",         to_string(_gradation).c_str());
  blsurf_set_param(bls, "patch_independent", _decimesh ? "1" : "0");
  blsurf_set_param(bls, "element",           _quadAllowed ? "q1.0" : "p1");
  blsurf_set_param(bls, "verb",              to_string(_verb).c_str());
}

status_t curv_fun(real t, real *uv, real *dt, real *dtt, void *user_data);
status_t surf_fun(real *uv, real *xyz, real*du, real *dv,
		  real *duu, real *duv, real *dvv, void *user_data);
status_t message_callback(message_t *msg, void *user_data);

//=============================================================================
/*!
 *
 */
//=============================================================================

bool BLSURFPlugin_BLSURF::Compute(SMESH_Mesh& aMesh, const TopoDS_Shape& aShape) {

  MESSAGE("BLSURFPlugin_BLSURF::Compute");

  if (aShape.ShapeType() == TopAbs_COMPOUND) {
    cout << "  the shape is a COMPOUND" << endl;
  }
  else {
    cout << "  the shape is UNKNOWN" << endl;
  };

  context_t *ctx =  context_new();
  context_set_message_callback(ctx, message_callback, &_comment);

  cad_t *c = cad_new(ctx);
 
  TopTools_IndexedMapOfShape fmap;
  TopTools_IndexedMapOfShape emap;
  TopTools_IndexedMapOfShape pmap;
  vector<Handle(Geom2d_Curve)> curves;
  vector<Handle(Geom_Surface)> surfaces;

  fmap.Clear();
  emap.Clear();
  pmap.Clear();
  surfaces.resize(0);
  curves.resize(0);

  int iface = 0;
  for (TopExp_Explorer face_iter(aShape,TopAbs_FACE);face_iter.More();face_iter.Next()) {
    TopoDS_Face f=TopoDS::Face(face_iter.Current());
    if (fmap.FindIndex(f) > 0)
      continue;
    
    fmap.Add(f);
    iface++;
    surfaces.push_back(BRep_Tool::Surface(f));
    cad_face_t *fce = cad_face_new(c, iface, surf_fun, surfaces.back());  
    cad_face_set_tag(fce, iface);
    if(f.Orientation() != TopAbs_FORWARD){
      cad_face_set_orientation(fce, CAD_ORIENTATION_REVERSED);
    } else {
      cad_face_set_orientation(fce, CAD_ORIENTATION_FORWARD);
    }
    
    for (TopExp_Explorer edge_iter(f,TopAbs_EDGE);edge_iter.More();edge_iter.Next()) {
      TopoDS_Edge e = TopoDS::Edge(edge_iter.Current());
      int ic = emap.FindIndex(e);
      if (ic <= 0)
	ic = emap.Add(e);
      
      double tmin,tmax;
      curves.push_back(BRep_Tool::CurveOnSurface(e, f, tmin, tmax));
      cad_edge_t *edg = cad_edge_new(fce, ic, tmin, tmax, curv_fun, curves.back());
      cad_edge_set_tag(edg, ic);
      cad_edge_set_property(edg, EDGE_PROPERTY_SOFT_REQUIRED);
      if (e.Orientation() == TopAbs_INTERNAL)
        cad_edge_set_property(edg, EDGE_PROPERTY_INTERNAL);

      int npts = 0;
      int ip1, ip2, *ip;
      gp_Pnt2d e0 = curves.back()->Value(tmin);
      gp_Pnt ee0 = surfaces.back()->Value(e0.X(), e0.Y());
      Standard_Real d1=0,d2=0;
      for (TopExp_Explorer ex_edge(e ,TopAbs_VERTEX); ex_edge.More(); ex_edge.Next()) {
	TopoDS_Vertex v = TopoDS::Vertex(ex_edge.Current());

	++npts;
	if (npts == 1){
	  ip = &ip1;
	  d1 = ee0.SquareDistance(BRep_Tool::Pnt(v));
	} else {
	  ip = &ip2;
          d2 = ee0.SquareDistance(BRep_Tool::Pnt(v));
	}
	*ip = pmap.FindIndex(v);
	if(*ip <= 0)
	  *ip = pmap.Add(v);
      }
      if (npts != 2) {
	// should not happen 
	cout << "An edge does not have 2 extremities." << endl;
      } else {
	if (d1 < d2)
	  cad_edge_set_extremities(edg, ip1, ip2);
	else
	  cad_edge_set_extremities(edg, ip2, ip1);
      }
    } // for edge
  } //for face




  blsurf_session_t *bls = blsurf_session_new(ctx);
  blsurf_data_set_cad(bls, c);

  SetParameters(_hypothesis, bls);

  cout << endl;
  cout << "Beginning of Surface Mesh generation" << endl;
  cout << endl;

  // Issue 0019864. On DebianSarge, FE signals do not obey to OSD::SetSignal(false)
#ifndef WNT
  feclearexcept( FE_ALL_EXCEPT );
  int oldFEFlags = fedisableexcept( FE_ALL_EXCEPT );
#endif

    status_t status = STATUS_ERROR;

  try {
    OCC_CATCH_SIGNALS;

    status = blsurf_compute_mesh(bls);

  }
  catch ( std::exception& exc ) {
    _comment += exc.what();
  }
  catch (Standard_Failure& ex) {
    _comment += ex.DynamicType()->Name();
    if ( ex.GetMessageString() && strlen( ex.GetMessageString() )) {
      _comment += ": ";
      _comment += ex.GetMessageString();
    }
  }
  catch (...) {
    if ( _comment.empty() )
      _comment = "Exception in blsurf_compute_mesh()";
  }
  if ( status != STATUS_OK) {
    blsurf_session_delete(bls);
    cad_delete(c);
    context_delete(ctx);

    return error(_comment);
  }

  cout << endl;
  cout << "End of Surface Mesh generation" << endl;
  cout << endl;

  mesh_t *msh;
  blsurf_data_get_mesh(bls, &msh);
  if(!msh){
    blsurf_session_delete(bls);
    cad_delete(c);
    context_delete(ctx);
    
    return error(_comment);
    //return false;
  }
  
  integer nv, ne, nt, nq, vtx[4], tag;
  real xyz[3];

  mesh_get_vertex_count(msh, &nv);
  mesh_get_edge_count(msh, &ne);
  mesh_get_triangle_count(msh, &nt);
  mesh_get_quadrangle_count(msh, &nq);

  
  SMESHDS_Mesh* meshDS = aMesh.GetMeshDS();
  SMDS_MeshNode** nodes = new SMDS_MeshNode*[nv+1];
  bool* tags = new bool[nv+1];

  for(int iv=1;iv<=nv;iv++) {
    mesh_get_vertex_coordinates(msh, iv, xyz);
    mesh_get_vertex_tag(msh, iv, &tag);    
    nodes[iv] = meshDS->AddNode(xyz[0], xyz[1], xyz[2]);
    // internal point are tagged to zero
    if(tag){
      meshDS->SetNodeOnVertex(nodes[iv], TopoDS::Vertex(pmap(tag)));
      tags[iv] = false;
    } else {
      tags[iv] = true;
    }
  }

  for(int it=1;it<=ne;it++) {
    mesh_get_edge_vertices(msh, it, vtx);
    SMDS_MeshEdge* edg = meshDS->AddEdge(nodes[vtx[0]], nodes[vtx[1]]);
    mesh_get_edge_tag(msh, it, &tag);    

    if (tags[vtx[0]]) {
      meshDS->SetNodeOnEdge(nodes[vtx[0]], TopoDS::Edge(emap(tag)));
      tags[vtx[0]] = false;
    };
    if (tags[vtx[1]]) {
      meshDS->SetNodeOnEdge(nodes[vtx[1]], TopoDS::Edge(emap(tag)));
      tags[vtx[1]] = false;
    };
    meshDS->SetMeshElementOnShape(edg, TopoDS::Edge(emap(tag)));
    
  }

  for(int it=1;it<=nt;it++) {
    mesh_get_triangle_vertices(msh, it, vtx);
    SMDS_MeshFace* tri = meshDS->AddFace(nodes[vtx[0]], nodes[vtx[1]], nodes[vtx[2]]);
    mesh_get_triangle_tag(msh, it, &tag);    
    meshDS->SetMeshElementOnShape(tri, TopoDS::Face(fmap(tag)));
    if (tags[vtx[0]]) {
      meshDS->SetNodeOnFace(nodes[vtx[0]], TopoDS::Face(fmap(tag)));
      tags[vtx[0]] = false;
    };
    if (tags[vtx[1]]) {
      meshDS->SetNodeOnFace(nodes[vtx[1]], TopoDS::Face(fmap(tag)));
      tags[vtx[1]] = false;
    };
    if (tags[vtx[2]]) {
      meshDS->SetNodeOnFace(nodes[vtx[2]], TopoDS::Face(fmap(tag)));
      tags[vtx[2]] = false;
    };
  }

  for(int it=1;it<=nq;it++) {
    mesh_get_quadrangle_vertices(msh, it, vtx);
    SMDS_MeshFace* quad = meshDS->AddFace(nodes[vtx[0]], nodes[vtx[1]], nodes[vtx[2]], nodes[vtx[3]]);
    mesh_get_quadrangle_tag(msh, it, &tag);    
    meshDS->SetMeshElementOnShape(quad, TopoDS::Face(fmap(tag)));
    if (tags[vtx[0]]) {
      meshDS->SetNodeOnFace(nodes[vtx[0]], TopoDS::Face(fmap(tag)));
      tags[vtx[0]] = false;
    };
    if (tags[vtx[1]]) {
      meshDS->SetNodeOnFace(nodes[vtx[1]], TopoDS::Face(fmap(tag)));
      tags[vtx[1]] = false;
    };
    if (tags[vtx[2]]) {
      meshDS->SetNodeOnFace(nodes[vtx[2]], TopoDS::Face(fmap(tag)));
      tags[vtx[2]] = false;
    };
    if (tags[vtx[3]]) {
      meshDS->SetNodeOnFace(nodes[vtx[3]], TopoDS::Face(fmap(tag)));
      tags[vtx[3]] = false;
    };
  }

  delete nodes;

  /* release the mesh object */
  blsurf_data_regain_mesh(bls, msh);

  /* clean up everything */
  blsurf_session_delete(bls);
  cad_delete(c);

  context_delete(ctx);

  // Issue 0019864. On DebianSarge, FE signals do not obey to OSD::SetSignal(false)
#ifndef WNT
  if ( oldFEFlags > 0 )    
    feenableexcept( oldFEFlags );
  feclearexcept( FE_ALL_EXCEPT );
#endif

  return true;
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

status_t curv_fun(real t, real *uv, real *dt, real *dtt, void *user_data)
{
  const Geom2d_Curve*pargeo = (const Geom2d_Curve*) user_data;

  if (uv){
    gp_Pnt2d P;
    P=pargeo->Value(t);
    uv[0]=P.X(); uv[1]=P.Y();
  }

  if(dt) {
    gp_Vec2d V1;
    V1=pargeo->DN(t,1);
    dt[0]=V1.X(); dt[1]=V1.Y();
  }

  if(dtt){
    gp_Vec2d V2;
    V2=pargeo->DN(t,2);
    dtt[0]=V2.X(); dtt[1]=V2.Y();
  }

  return 0;
}

status_t surf_fun(real *uv, real *xyz, real*du, real *dv,
		  real *duu, real *duv, real *dvv, void *user_data)
{
  const Geom_Surface* geometry = (const Geom_Surface*) user_data;

  if(xyz){
   gp_Pnt P;
   P=geometry->Value(uv[0],uv[1]);   // S.D0(U,V,P);
   xyz[0]=P.X(); xyz[1]=P.Y(); xyz[2]=P.Z();
  }

  if(du && dv){
    gp_Pnt P;
    gp_Vec D1U,D1V;
    
    geometry->D1(uv[0],uv[1],P,D1U,D1V);
    du[0]=D1U.X(); du[1]=D1U.Y(); du[2]=D1U.Z();
    dv[0]=D1V.X(); dv[1]=D1V.Y(); dv[2]=D1V.Z();
  }

  if(duu && duv && dvv){
    gp_Pnt P;
    gp_Vec D1U,D1V;
    gp_Vec D2U,D2V,D2UV;
    
    geometry->D2(uv[0],uv[1],P,D1U,D1V,D2U,D2V,D2UV);
    duu[0]=D2U.X(); duu[1]=D2U.Y(); duu[2]=D2U.Z();
    duv[0]=D2UV.X(); duv[1]=D2UV.Y(); duv[2]=D2UV.Z();
    dvv[0]=D2V.X(); dvv[1]=D2V.Y(); dvv[2]=D2V.Z();    
  }

  return 0;
}

status_t message_callback(message_t *msg, void *user_data)
{
  integer errnumber = 0;
  char *desc;
  message_get_number(msg, &errnumber);
  message_get_description(msg, &desc);
  if ( errnumber < 0 ) {
    string * error = (string*)user_data;
//   if ( !error->empty() )
//     *error += "\n";
    // remove ^A from the tail
    int len = strlen( desc );
    while (len > 0 && desc[len-1] != '\n')
      len--;
    error->append( desc, len );
  }
  else {
    cout << desc;
  }
  return STATUS_OK;
}
