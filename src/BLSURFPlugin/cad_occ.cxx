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
// File      : BLSURFlugin_Mesher.cxx
// Authors   : Francis KLOSS (OCC) & Patrick LAUG (INRIA)
// Date      : 31/03/2006
// Project   : SALOME
//=============================================================================
// using namespace std;
// #include "BLSURFPlugin_Mesher.hxx"
// #include "BLSURFPlugin_Hypothesis.hxx"
// #include <SMESHDS_Mesh.hxx>
// #include <SMDS_MeshElement.hxx>
// #include <SMDS_MeshNode.hxx>
// #include <utilities.h>
// #include <vector>
// #include <BRep_Tool.hxx>
// #include <TopExp.hxx>
// #include <TopExp_Explorer.hxx>
// #include <TopoDS.hxx>
// #include <NCollection_Map.hxx>
//
#include <iostream>
#include <fstream>

#include <Occ_utility.h>
#include <TDataStd_Name.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopExp_Explorer.hxx>
#include <AIS_InteractiveObject.hxx>
#include <ShapeFix_Edge.hxx>
#include <TopoDS.hxx>
#include <TopLoc_Location.hxx>
#include <Geom_Surface.hxx>
#include <Handle_Geom_Surface.hxx>
#include <BRep_Tool.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_BoundedSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_SweptSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <ShapeUpgrade_ClosedFaceDivide.hxx>
#include <ShapeUpgrade_ShapeDivide.hxx>
#include <TNaming_Builder.hxx>
#include <TDataStd_Integer.hxx>
#include <cad_occ.h>

#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <BRepAlgoAPI_Common.hxx>

#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <Handle_BRep_TEdge.hxx>
#include <BRep_GCurve.hxx>
#include <Handle_BRep_GCurve.hxx>
#include <ShapeFix_Edge.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#define TSMO blw->bls_glo.tsmo

bool BL_SURF::init(const TopoDS_Shape& _shape) {
  cout << "BLSURF_init: begin" << endl;

  if (_shape.ShapeType() == TopAbs_COMPOUND) {
    cout << "BLSURF_init: the shape is a COMPOUND" << endl;
  } else {
    cout << "BLSURF_init: the shape is UNKNOWN" << endl;
  }

  cout << "BLSURF_init: exploring faces and edges " << endl;
  int i=0;
  for (TopExp_Explorer expf(_shape, TopAbs_FACE); expf.More(); expf.Next()) {
      const TopoDS_Shape& face = expf.Current();
      i++;
      int j=0;
      for (TopExp_Explorer expe(face, TopAbs_EDGE); expe.More(); expe.Next()) {
          // const TopoDS_Shape& edge = expe.Current(); -> warning: unused variable 'edge'
          j++;
      }
      // cout << "BLSURF_init: face " << i << " has " << j << " edges" << endl;
  }
  cout << "BLSURF_init: total number of faces = " << i << endl;

  TopoDS_Shape the_shape;
  the_shape = _shape;

  // return(true);
  return(run(the_shape));
}

// #define TC2D blw.bls_glo.TC2dNew

void BL_SURF::get_blw(blw_*& blw_param) {
  blw_param = &blw;
}

bool BL_SURF::end() {
  bls_free(&blw);
  if (blw.error[0]) {
    return(false);
  } else {
    return(true);
  }
}

double norm(vector<double> v)
{
  double res=0.;
  for (int i=0;i<v.size();i++) res+=POW2(v[i]);
  return(sqrt(res));
}

vector<double> operator-(vector<double> v1, vector<double> v2)
{
  assert(v1.size()==v2.size());
  vector<double> res(v1.size(),0.);
  for (int i=0;i<v1.size();i++) res[i]=v1[i]-v2[i];
  return(res);
}

bool BL_SURF::run(TopoDS_Shape &s)
{
  the_object=s;
  return(run());
}

bool BL_SURF::run()
{
  active_blsurf = this;
  build_surfaces(the_object);
  int ret = bls_main(&blw);
  active_blsurf = NULL;
  if (ret == 0) {
    return(true);
  } else {
    return(false);
  }
}

void BL_SURF::build_surfaces(TopoDS_Shape &shape)
{
  int i, iface;

//
// Explore the whole shape to extract surfaces, and build BL objects
//

  for (i=0;i<surfaces.size();i++) delete(surfaces[i]);
  surfaces.resize(0);

  TopTools_IndexedMapOfShape fmap;
  fmap.Clear();
  iface = 0;
  for (TopExp_Explorer ex_face(shape,TopAbs_FACE);ex_face.More();ex_face.Next()) {
    TopoDS_Face f=TopoDS::Face(ex_face.Current());
    if (fmap.FindIndex(f) <= 0) {
      fmap.Add(f);
      iface++;
      BL_SURFACE *bl_surface=new BL_SURFACE(f);
      if(bl_surface->build()) {
           surfaces.push_back(bl_surface);
      } else {
	cout<<"Weird surface detected !"<<endl;
	delete(bl_surface);
      }
    }
  }
}

BL_SURFACE::BL_SURFACE(TopoDS_Face &f) 
{ 
  sizer=NULL;
  topology=f;
  geometry=BRep_Tool::Surface(f);
}

BL_SURFACE::~BL_SURFACE() 
{ 
  for (int i=0;i<edges.size();i++) delete(edges[i]);
}

bool BL_SURFACE::build()
{
//
// Explore the surface to extract all edges
//
  for (TopExp_Explorer ex_face(topology,TopAbs_EDGE);ex_face.More();ex_face.Next()) {
    TopoDS_Edge e=TopoDS::Edge(ex_face.Current());
    if(!add(e)) return(false);
  }
  return(true);
}

void BL_SURFACE::add(double u, double v)
{
  points.push_back(new BL_POINT(this,u,v));
}

bool BL_SURFACE::add(TopoDS_Edge &edge)
{
//
// add an edge ( make some checks, and build the associated BL_EDGE object )
//
  Handle(Geom2d_Curve) pargeo;
  double tmin,tmax;

  pargeo=BRep_Tool::CurveOnSurface(edge,topology,tmin,tmax);

  BL_EDGE *bl_edge=new BL_EDGE(edge,this,pargeo,tmin,tmax);
  edges.push_back(bl_edge);
  return(true);
}

BL_EDGE::BL_EDGE(TopoDS_Edge &ed,BL_SURFACE *b, Handle(Geom2d_Curve) pargeo, double tmin, double tmax)
{
  sizer=NULL;
  refc=-1;
  typc=1;
  topology=ed; boss=b;
  parametric_geometry=pargeo;
  parametric_min=tmin; parametric_max=tmax;
}

BL_EDGE::~BL_EDGE() { }

void BL_EDGE::method_curv0(const double &t, double *C)
{
  gp_Pnt2d P;

  P=parametric_geometry->Value(t);
  C[0]=P.X(); C[1]=P.Y();
}

void BL_EDGE::method_curv1(const double &t, double *C)
{
  gp_Vec2d V1;

  V1=parametric_geometry->DN(t,1);
  C[0]=V1.X(); C[1]=V1.Y();
}

void BL_EDGE::method_curv2(const double &t, double *C)
{
  gp_Vec2d V2;

  V2=parametric_geometry->DN(t,2);
  C[0]=V2.X(); C[1]=V2.Y();
}

void BL_SURFACE::method_surf0(const double *UV, double *S)
{
  gp_Pnt P;

  P=geometry->Value(UV[0],UV[1]);   // S.D0(U,V,P);
  S[0]=P.X(); S[1]=P.Y(); S[2]=P.Z();
}

void BL_SURFACE::method_surf1(const double *UV, double *Su, double *Sv)
{
  gp_Pnt P;
  gp_Vec D1U,D1V;

  geometry->D1(UV[0],UV[1],P,D1U,D1V);
  Su[0]=D1U.X(); Su[1]=D1U.Y(); Su[2]=D1U.Z();
  Sv[0]=D1V.X(); Sv[1]=D1V.Y(); Sv[2]=D1V.Z();
} 

void BL_SURFACE::method_surf2(const double *UV, double *Suu, double *Suv, double *Svv)
{
  gp_Pnt P;
  gp_Vec D1U,D1V;
  gp_Vec D2U,D2V,D2UV;

  geometry->D2(UV[0],UV[1],P,D1U,D1V,D2U,D2V,D2UV);
  Suu[0]=D2U.X(); Suu[1]=D2U.Y(); Suu[2]=D2U.Z();
  Suv[0]=D2UV.X(); Suv[1]=D2UV.Y(); Suv[2]=D2UV.Z();
  Svv[0]=D2V.X(); Svv[1]=D2V.Y(); Svv[2]=D2V.Z();
}

static vector<BL_SURFACE*> *current_list_of_faces=NULL;
BL_SURF* BL_SURF::active_blsurf=NULL;

void cad_surf0 (blw_ *blw, int *refs, double *uv, double *S)
{
  BL_SURFACE *surf=(*current_list_of_faces)[*refs-1];
  surf->method_surf0(uv,S);
}

void cad_surf1 (blw_ *blw, int *refs, double *uv, double *Su, double *Sv)
{
  BL_SURFACE *surf=(*current_list_of_faces)[*refs-1];
  surf->method_surf1(uv,Su,Sv);
}

// calcul discret
// double du, dv, uvg[2], uvd[2], uvb[2], uvh[2], Sg[3], Sd[3], Sb[3], Sh[3];
// double Sucont[3], Svcont[3];
// if (*refs == 33) {
//   Sucont[0] = Su[0]; Sucont[1] = Su[1]; Sucont[2] = Su[2];
//   Svcont[0] = Sv[0]; Svcont[1] = Sv[1]; Svcont[2] = Sv[2];
//   BL_SURFACE *surf=(*current_list_of_faces)[*refs-1];
//   du = (TSMO[*refs].uvmax[0] - TSMO[*refs].uvmin[0]) *  0.08;
//   dv = (TSMO[*refs].uvmax[1] - TSMO[*refs].uvmin[1]) *  0.08;
//   uvg[0] = uv[0]-du;
//   uvg[1] = uv[1];
//   uvd[0] = uv[0]+du;
//   uvd[1] = uv[1];
//   uvb[0] = uv[0];
//   uvb[1] = uv[1]-dv;
//   uvh[0] = uv[0];
//   uvh[1] = uv[1]+dv;
//   if (uvg[0] < TSMO[*refs].uvmin[0]) uvg[0] = TSMO[*refs].uvmin[0];
//   if (uvd[0] > TSMO[*refs].uvmax[0]) uvd[0] = TSMO[*refs].uvmax[0];
//   if (uvb[1] < TSMO[*refs].uvmin[1]) uvb[1] = TSMO[*refs].uvmin[1];
//   if (uvh[1] > TSMO[*refs].uvmax[1]) uvh[1] = TSMO[*refs].uvmax[1];
//   surf->method_surf0(uvg,Sg);
//   surf->method_surf0(uvd,Sd);
//   surf->method_surf0(uvb,Sb);
//   surf->method_surf0(uvh,Sh);
//   du = uvd[0] - uvg[0];
//   dv = uvh[1] - uvb[1];
//   Su[0] = (Sd[0] - Sg[0]) / du;
//   Su[1] = (Sd[1] - Sg[1]) / du;
//   Su[2] = (Sd[2] - Sg[2]) / du;
//   Sv[0] = (Sh[0] - Sb[0]) / dv;
//   Sv[1] = (Sh[1] - Sb[1]) / dv;
//   Sv[2] = (Sh[2] - Sb[2]) / dv;
//   fprintf(blw->out, "Su cont %g %g %g\n", Sucont[0], Sucont[1], Sucont[2]);
//   fprintf(blw->out, "Su disc %g %g %g\n", Su[0], Su[1], Su[2]);
//   fprintf(blw->out, "Sv cont %g %g %g\n", Svcont[0], Svcont[1], Svcont[2]);
//   fprintf(blw->out, "Sv disc %g %g %g\n", Sv[0], Sv[1], Sv[2]);
// }

void cad_surf2 (blw_ *blw, int *refs, double uv[2], double Suu[3], double Suv[3], double Svv[3])
{
  BL_SURFACE *surf=(*current_list_of_faces)[*refs-1];
  surf->method_surf2(uv,Suu,Suv,Svv);

//  // calcul discret
//  double du, dv, uvg[2], uvd[2], uvb[2], uvh[2], Sug[3], Sud[3], Sub[3], Suh[3], Svg[3], Svd[3], Svb[3], Svh[3];
//  double Suucont[3], Suvcont[3], Svvcont[3];
//  if (*refs >= 0) {
//    Suucont[0] = Suu[0]; Suucont[1] = Suu[1]; Suucont[2] = Suu[2];
//    Suvcont[0] = Suv[0]; Suvcont[1] = Suv[1]; Suvcont[2] = Suv[2];
//    Svvcont[0] = Svv[0]; Svvcont[1] = Svv[1]; Svvcont[2] = Svv[2];
//    du = (TSMO[*refs].uvmax[0] - TSMO[*refs].uvmin[0]) *  0.08;
//    dv = (TSMO[*refs].uvmax[1] - TSMO[*refs].uvmin[1]) *  0.08;
//    uvg[0] = uv[0]-du;
//    uvg[1] = uv[1];
//    uvd[0] = uv[0]+du;
//    uvd[1] = uv[1];
//    uvb[0] = uv[0];
//    uvb[1] = uv[1]-dv;
//    uvh[0] = uv[0];
//    uvh[1] = uv[1]+dv;
//    if (uvg[0] < TSMO[*refs].uvmin[0]) uvg[0] = TSMO[*refs].uvmin[0];
//    if (uvd[0] > TSMO[*refs].uvmax[0]) uvd[0] = TSMO[*refs].uvmax[0];
//    if (uvb[1] < TSMO[*refs].uvmin[1]) uvb[1] = TSMO[*refs].uvmin[1];
//    if (uvh[1] > TSMO[*refs].uvmax[1]) uvh[1] = TSMO[*refs].uvmax[1];
//    surf->method_surf1(uvg,Sug,Svg);
//    surf->method_surf1(uvd,Sud,Svd);
//    surf->method_surf1(uvb,Sub,Svb);
//    surf->method_surf1(uvh,Suh,Svh);
//    du = uvd[0] - uvg[0];
//    dv = uvh[1] - uvb[1];
//    Suu[0] = (Sud[0] - Sug[0]) / du;
//    Suu[1] = (Sud[1] - Sug[1]) / du;
//    Suu[2] = (Sud[2] - Sug[2]) / du;
//    Svv[0] = (Svh[0] - Svb[0]) / dv;
//    Svv[1] = (Svh[1] - Svb[1]) / dv;
//    Svv[2] = (Svh[2] - Svb[2]) / dv;
//    Suv[0] = ((Suh[0] - Sub[0]) / dv + (Svd[0] - Svg[0]) / du) * 0.5;
//    Suv[1] = ((Suh[1] - Sub[1]) / dv + (Svd[1] - Svg[1]) / du) * 0.5;
//    Suv[2] = ((Suh[2] - Sub[2]) / dv + (Svd[2] - Svg[2]) / du) * 0.5;
//    // fprintf(blw->out, "disc uv %g %g Suu %g %g %g\n", uv[0], uv[1], Suu[0], Suu[1], Suu[2]);
//    // fprintf(blw->out, "Suv disc %g %g %g\n", Suv[0], Suv[1], Suv[2]);
//    // fprintf(blw->out, "Svv disc %g %g %g\n", Svv[0], Svv[1], Svv[2]);
//  }
}

void cad_curvint (blw_ *blw, int *refs, int *ic, double *a, double *b)
{
  *a=(*current_list_of_faces)[*refs-1]->edges[*ic-1]->parametric_min;
  *b=(*current_list_of_faces)[*refs-1]->edges[*ic-1]->parametric_max;
}

void cad_curv0 (blw_ *blw, int *refs, int *ic, double *t, double *C)
{
  (*current_list_of_faces)[*refs-1]->edges[*ic-1]->method_curv0(*t,C);
}

void cad_curv1 (blw_ *blw, int *refs, int *ic, double *t, double *Ct)
{
  (*current_list_of_faces)[*refs-1]->edges[*ic-1]->method_curv1(*t,Ct);
}

void cad_curv2 (blw_ *blw, int *refs, int *ic, double *t, double *Ctt)
{
  (*current_list_of_faces)[*refs-1]->edges[*ic-1]->method_curv2(*t,Ctt);
}

void cad_hphys (blw_ *blw, int *refs, double *uv, double *h) {}
void cad_hphyc (blw_ *blw, int *refs, int *ic, double *t, double *h) {}
void cad_hphyp (blw_ *blw, int *refp, double *h) {}

void cad_rads (blw_ *blw, int *refs, double *uv, double *rhos) {
  sprintf(blw->error, "cad_rads should not be called\n");
}

void cad_radc (blw_ *blw, int *refs, int *ic, double *t, double *rhoc) {
  sprintf(blw->error, "cad_radc should not be called");
}

void cad_hageos(blw_ *blw, int *refs, double uv[2], double hageos[6]) {
  sprintf(blw->error, "cad_hageos should not be called\n");
}

void cad_refphyc(blw_ *blw, int *refc, int *phyc) {
  *phyc = *refc;
}

void cad_refphyp(blw_ *blw, int *refp, int *phyp) {
  *phyp = *refp;
}

void cad_refphys(blw_ *blw, int *refs, int *phys) {
  *phys = *refs;
}

void cad_edgetag(blw_ *blw, int *refc, int *required) {
  *required = 1;
}

void cad_curvparam(blw_ *blw, int *refs, int *ic, int *n, double tab[]) {
  *n = 0;
}

void BL_SURF::init_tsmo(blw_ *blw) {   /* cf. bls_read_pardom */
  int is, ns, ic, nc, j, igloP, igloQ, isurf;
  TopoDS_Vertex v;
  gp_Pnt p;
  double a, b, uv[2], xyz[3], xyzP[3], xyzQ[3];

  ns = surfaces.size();
  if (blw->env.verb >= 10) fprintf(blw->out, "\ninit_tsmo: %d surfaces\n", ns);
  if (ns <= 0) {
    TSMO = NULL;
    blw->bls_glo.number_of_patches = 0;
    return;
  }

  TopTools_IndexedMapOfShape fmap_edges;
  TopTools_IndexedMapOfShape fmap_points;
  fmap_edges.Clear();
  fmap_points.Clear();
  for (isurf=0; isurf<ns; isurf++) {
    TopoDS_Face face = surfaces[isurf]->topology;
    for (TopExp_Explorer ex_face(face ,TopAbs_EDGE); ex_face.More(); ex_face.Next()) {
      TopoDS_Edge e = TopoDS::Edge(ex_face.Current());
      if (fmap_edges.FindIndex(e) <= 0) {   /* not found => FindIndex = 0, found => FindIndex > 0 */
	fmap_edges.Add(e);
	j = 0;
	for (TopExp_Explorer ex_edge(e ,TopAbs_VERTEX); ex_edge.More(); ex_edge.Next()) {
	  v = TopoDS::Vertex(ex_edge.Current());
	  ++j;
	  if (fmap_points.FindIndex(v) <= 0) fmap_points.Add(v);
	}
	if (j != 2) {
	  sprintf(blw->error, "init_tsmo: surface is=%d: an edge has %d != 2 extremities\n", isurf+1, j); return;
	}
      }
    }
  }

  MALLOC(surface_, TSMO, ns+1); if (blw->error[0]) return;
  blw->bls_glo.number_of_patches = ns;
  for (isurf=0; isurf<ns; isurf++) {
    is = isurf+1;
    nc = surfaces[isurf]->edges.size();
    if (blw->env.verb >= 70) fprintf(blw->out, "init_tsmo: surface is=%d: %d curves\n", is, nc);

    bls_init_surface(blw, &(TSMO[is]), nc);
    TSMO[is].iglos = is;
    TSMO[is].pardom_side = 0;
    TSMO[is].surforient = +1;
    for (int icurv=0;icurv<nc;icurv++) {
      ic = icurv+1;
      TSMO[is].TC[ic].typc = 1;
      BL_EDGE* curve = surfaces[isurf]->edges[icurv];
      TopoDS_Edge edge = curve->topology;
      TSMO[is].TC[ic].igloc = fmap_edges.FindIndex(edge);

      TopExp_Explorer ex_edge(edge, TopAbs_VERTEX);
      v = TopoDS::Vertex(ex_edge.Current());
      p = BRep_Tool::Pnt(v);
      xyzP[0] = p.X(); xyzP[1] = p.Y(); xyzP[2] = p.Z();
      igloP = fmap_points.FindIndex(v);

      ex_edge.Next();
      v = TopoDS::Vertex(ex_edge.Current());
      p = BRep_Tool::Pnt(v);
      xyzQ[0] = p.X(); xyzQ[1] = p.Y(); xyzQ[2] = p.Z();
      igloQ = fmap_points.FindIndex(v);

      cad_curvint (blw, &is, &ic, &a, &b);
      cad_curv0   (blw, &is, &ic, &a, uv);
      cad_surf0   (blw, &is, uv, xyz);      
      xyzP[0] -= xyz[0]; xyzP[1] -= xyz[1]; xyzP[2] -= xyz[2];
      xyzQ[0] -= xyz[0]; xyzQ[1] -= xyz[1]; xyzQ[2] -= xyz[2];
      if (NORM3DPOW2(xyzP) <= NORM3DPOW2(xyzQ)) {
	TSMO[is].TC[ic].iglope[0] = igloP;
	TSMO[is].TC[ic].iglope[1] = igloQ;
      } else {
	TSMO[is].TC[ic].iglope[0] = igloQ;
	TSMO[is].TC[ic].iglope[1] = igloP;
      }
    }
  }
}

int BL_SURF::bls_main(blw_ *blw) {
  FILE *file;

  /* banner */
  blw->out = stdout;
  bls_banner(blw, 1);
  
  /* initialiser l'environnement de BLSURF */
  init_blsenv(blw);

  /* lire l'environnement de BLSURF */
  /* par defaut : strcpy(blw->dirname, ""); */
  /* sinon : strcpy(blw->dirname, "C:\\Documents and Settings\\xxx\\"); */
  sprintf(blw->filename, "%sblsurf.env", blw->dirname);
  FOPEN(file, "r");
  if (blw->error[0]) {
    fprintf(blw->out, "File blsurf.env not found. Default values will be used.\n");
    blw->error[0] = 0;
  } else {
    read_blsenv(blw, file);
    FCLOSE(file);
    if (blw->error[0]) goto error;   /* read_blsenv: unknown keyword */
  }

  /* copier les variables communes entre "bl2denv" et blsenv (en particulier verb) */
  copy_common_env(blw);

  /* imprimer l'environnement de BLSURF */
  if (blw->env.verb >= 70) print_blsenv(blw);

  /* initialiser TSMO */
  current_list_of_faces = &surfaces;
  BL_SURF::init_tsmo(blw); if (blw->error[0]) goto error;

  /* generer le maillage surfacique */
  bls_init(blw);             if (blw->error[0]) goto error;
  bls_mesh(blw);             if (blw->error[0]) goto error;
  bls_surforient(blw);       if (blw->error[0]) goto error;
  bls_export(blw);           if (blw->error[0]) goto error;
  /* a voir : les tableaux pointes par current_list_of_faces sont liberes ? handles ? */

  bls_banner(blw, 2);
  return(0);

 error:
  fprintf(blw->out, "\nBLSURF ERROR MESSAGE:\n%s\n", blw->error);
  bls_banner(blw, 2);
  return(1);
}
