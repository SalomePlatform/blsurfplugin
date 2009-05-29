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
// File    : BLSURFPlugin_BLSURF.cxx
// Authors : Francis KLOSS (OCC) & Patrick LAUG (INRIA) & Lioka RAZAFINDRAZAKA (CEA)
//           & Aurelien ALLEAUME (DISTENE)
//           Size maps developement: Nicolas GEIMER (OCC) & Gilles DAVID (EURIWARE)
// ---
//
#include "BLSURFPlugin_BLSURF.hxx"
#include "BLSURFPlugin_Hypothesis.hxx"

#include <structmember.h>


#include <SMESH_Gen.hxx>
#include <SMESH_Mesh.hxx>
#include <SMESH_ControlsDef.hxx>
#include <SMESHGUI_Utils.h>

#include <SMESHDS_Mesh.hxx>
#include <SMDS_MeshElement.hxx>
#include <SMDS_MeshNode.hxx>

#include <utilities.h>

#include <limits>
#include <list>
#include <vector>
#include <cstdlib>

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

#include  <GeomSelectionTools.h>

#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>

/* ==================================
 * ===========  PYTHON ==============
 * ==================================*/

typedef struct {
  PyObject_HEAD
  int softspace;
  std::string *out;
  } PyStdOut;

static void
PyStdOut_dealloc(PyStdOut *self)
{
  PyObject_Del(self);
}

static PyObject *
PyStdOut_write(PyStdOut *self, PyObject *args)
{
  char *c;
  int l;
  if (!PyArg_ParseTuple(args, "t#:write",&c, &l))
    return NULL;

  //std::cerr << c ;
  *(self->out)=*(self->out)+c;

  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef PyStdOut_methods[] = {
  {"write",  (PyCFunction)PyStdOut_write,  METH_VARARGS,
    PyDoc_STR("write(string) -> None")},
  {NULL,    NULL}   /* sentinel */
};

static PyMemberDef PyStdOut_memberlist[] = {
  {"softspace", T_INT,  offsetof(PyStdOut, softspace), 0,
   "flag indicating that a space needs to be printed; used by print"},
  {NULL} /* Sentinel */
};

static PyTypeObject PyStdOut_Type = {
  /* The ob_type field must be initialized in the module init function
   * to be portable to Windows without using C++. */
  PyObject_HEAD_INIT(NULL)
  0,      /*ob_size*/
  "PyOut",   /*tp_name*/
  sizeof(PyStdOut),  /*tp_basicsize*/
  0,      /*tp_itemsize*/
  /* methods */
  (destructor)PyStdOut_dealloc, /*tp_dealloc*/
  0,      /*tp_print*/
  0, /*tp_getattr*/
  0, /*tp_setattr*/
  0,      /*tp_compare*/
  0,      /*tp_repr*/
  0,      /*tp_as_number*/
  0,      /*tp_as_sequence*/
  0,      /*tp_as_mapping*/
  0,      /*tp_hash*/
        0,                      /*tp_call*/
        0,                      /*tp_str*/
        PyObject_GenericGetAttr,                      /*tp_getattro*/
        /* softspace is writable:  we must supply tp_setattro */
        PyObject_GenericSetAttr,    /* tp_setattro */
        0,                      /*tp_as_buffer*/
        Py_TPFLAGS_DEFAULT,     /*tp_flags*/
        0,                      /*tp_doc*/
        0,                      /*tp_traverse*/
        0,                      /*tp_clear*/
        0,                      /*tp_richcompare*/
        0,                      /*tp_weaklistoffset*/
        0,                      /*tp_iter*/
        0,                      /*tp_iternext*/
        PyStdOut_methods,                      /*tp_methods*/
        PyStdOut_memberlist,                      /*tp_members*/
        0,                      /*tp_getset*/
        0,                      /*tp_base*/
        0,                      /*tp_dict*/
        0,                      /*tp_descr_get*/
        0,                      /*tp_descr_set*/
        0,                      /*tp_dictoffset*/
        0,                      /*tp_init*/
        0,                      /*tp_alloc*/
        0,                      /*tp_new*/
        0,                      /*tp_free*/
        0,                      /*tp_is_gc*/
};

PyObject * newPyStdOut( std::string& out )
{
  PyStdOut *self;
  self = PyObject_New(PyStdOut, &PyStdOut_Type);
  if (self == NULL)
    return NULL;
  self->softspace = 0;
  self->out=&out;
  return (PyObject*)self;
}


////////////////////////END PYTHON///////////////////////////

//////////////////MY MAPS////////////////////////////////////////
std::map<int,string> FaceId2SizeMap;
std::map<int,string> EdgeId2SizeMap;
std::map<int,string> VertexId2SizeMap;
std::map<int,PyObject*> FaceId2PythonSmp;
std::map<int,PyObject*> EdgeId2PythonSmp;
std::map<int,PyObject*> VertexId2PythonSmp;


bool HasSizeMapOnFace=false;
bool HasSizeMapOnEdge=false;
bool HasSizeMapOnVertex=false;

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


  /* Initialize the Python interpreter */
  assert(Py_IsInitialized());
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();

  main_mod = NULL;
  main_mod = PyImport_AddModule("__main__");

  main_dict = NULL;
  main_dict = PyModule_GetDict(main_mod);

  PyRun_SimpleString("from math import *");
  PyGILState_Release(gstate);
  
  FaceId2SizeMap.clear();
  EdgeId2SizeMap.clear();
  VertexId2SizeMap.clear();
  FaceId2PythonSmp.clear();
  EdgeId2PythonSmp.clear();
  VertexId2PythonSmp.clear();
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

double _smp_phy_size;
status_t size_on_surface(integer face_id, real *uv, real *size, void *user_data);
status_t size_on_edge(integer edge_id, real t, real *size, void *user_data); 
status_t size_on_vertex(integer vertex_id, real *size, void *user_data);

double my_u_min=1e6,my_v_min=1e6,my_u_max=-1e6,my_v_max=-1e6;

/////////////////////////////////////////////////////////
gp_XY getUV(const TopoDS_Face& face, const gp_XYZ& point)
{
  Handle(Geom_Surface) surface = BRep_Tool::Surface(face);
  GeomAPI_ProjectPointOnSurf projector( point, surface );
  if ( !projector.IsDone() || projector.NbPoints()==0 )
    throw "Can't project";

  Quantity_Parameter u,v;
  projector.LowerDistanceParameters(u,v);
  return gp_XY(u,v);
}
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
double getT(const TopoDS_Edge& edge, const gp_XYZ& point)
{
  Standard_Real f,l;
  Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, f,l);
  GeomAPI_ProjectPointOnCurve projector( point, curve);
  if ( projector.NbPoints() == 0 )
    throw;
  return projector.LowerDistanceParameter();
}
/////////////////////////////////////////////////////////

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
        MESSAGE("blsurf_set_param(): " << opIt->first << " = " << opIt->second);
        blsurf_set_param(bls, opIt->first.c_str(), opIt->second.c_str());
      }

  } else {
    MESSAGE("BLSURFPlugin_BLSURF::SetParameters using defaults");
  }
  _smp_phy_size = _phySize;
  blsurf_set_param(bls, "topo_points",       _topology > 0 ? "1" : "0");
  blsurf_set_param(bls, "topo_curves",       _topology > 0 ? "1" : "0");
  blsurf_set_param(bls, "topo_project",      _topology > 0 ? "1" : "0");
  blsurf_set_param(bls, "clean_boundary",    _topology > 1 ? "1" : "0");
  blsurf_set_param(bls, "close_boundary",    _topology > 1 ? "1" : "0");
  blsurf_set_param(bls, "hphy_flag",         to_string(_physicalMesh).c_str());
//  blsurf_set_param(bls, "hphy_flag",         "2");
  if ((to_string(_physicalMesh))=="2"){
    GeomSelectionTools* GeomST = new GeomSelectionTools::GeomSelectionTools( SMESH::GetActiveStudyDocument());

    TopoDS_Shape GeomShape;
    TopAbs_ShapeEnum GeomType;
    //
    // Standard Size Maps
    //
    MESSAGE("Setting a Size Map");
    const BLSURFPlugin_Hypothesis::TSizeMap & sizeMaps = hyp->GetSizeMapEntries();
    BLSURFPlugin_Hypothesis::TSizeMap::const_iterator smIt;
    int i=0;
    for ( smIt = sizeMaps.begin(); smIt != sizeMaps.end(); ++smIt ) {
      if ( !smIt->second.empty() ) {
        MESSAGE("blsurf_set_sizeMap(): " << smIt->first << " = " << smIt->second);
        GeomShape = GeomST->entryToShape(smIt->first);
        GeomType  = GeomShape.ShapeType();
        if (GeomType == TopAbs_FACE){
          HasSizeMapOnFace = true;
          FaceId2SizeMap[TopoDS::Face(GeomShape).HashCode(471662)] = smIt->second;
        }
        if (GeomType == TopAbs_EDGE){
          HasSizeMapOnEdge = true;
          HasSizeMapOnFace = true;
          EdgeId2SizeMap[TopoDS::Edge(GeomShape).HashCode(471662)] = smIt->second;
        }
        if (GeomType == TopAbs_VERTEX){
          HasSizeMapOnVertex = true;
          HasSizeMapOnEdge   = true;
          HasSizeMapOnFace   = true;
          VertexId2SizeMap[TopoDS::Vertex(GeomShape).HashCode(471662)] = smIt->second;
        }
      }
    }

    //
    // Attractors
    //
    MESSAGE("Setting Attractors");
    const BLSURFPlugin_Hypothesis::TSizeMap & attractors = hyp->GetAttractorEntries();
    BLSURFPlugin_Hypothesis::TSizeMap::const_iterator atIt;
    for ( atIt = attractors.begin(); atIt != attractors.end(); ++atIt ) {
      if ( !atIt->second.empty() ) {
        MESSAGE("blsurf_set_attractor(): " << atIt->first << " = " << atIt->second);
        GeomShape = GeomST->entryToShape(atIt->first);
        GeomType  = GeomShape.ShapeType();

        if (GeomType == TopAbs_FACE){
          HasSizeMapOnFace = true;

          double xa, ya, za; // Coordinates of attractor point
          double a, b;       // Attractor parameter
          int pos1, pos2;
          // atIt->second has the following pattern:
          // ATTRACTOR(xa;ya;za;a;b)
          // where:
          // xa;ya;za : coordinates of  attractor
          // a        : desired size on attractor
          // b        : distance of influence of attractor
          //
          // We search the parameters in the string
          pos1 = atIt->second.find(";");
          xa = atof(atIt->second.substr(10, pos1-10).c_str());
          pos2 = atIt->second.find(";", pos1+1);
          ya = atof(atIt->second.substr(pos1+1, pos2-pos1-1).c_str());
          pos1 = pos2;
          pos2 = atIt->second.find(";", pos1+1);
          za = atof(atIt->second.substr(pos1+1, pos2-pos1-1).c_str());
          pos1 = pos2;
          pos2 = atIt->second.find(";", pos1+1);
          a = atof(atIt->second.substr(pos1+1, pos2-pos1-1).c_str());
          pos1 = pos2;
          pos2 = atIt->second.find(")");
          b = atof(atIt->second.substr(pos1+1, pos2-pos1-1).c_str());
          
          // Get the (u,v) values of the attractor on the face
          gp_XY uvPoint = getUV(TopoDS::Face(GeomShape),gp_XYZ(xa,ya,za));
          Standard_Real u0 = uvPoint.X();
          Standard_Real v0 = uvPoint.Y();
          // We construct the python function
          ostringstream attractorFunction;
          attractorFunction << "def f(u,v): return ";
          attractorFunction << _smp_phy_size << "-(" << _smp_phy_size <<"-" << a << ")";
          attractorFunction << "*exp(-((u-("<<u0<<"))*(u-("<<u0<<"))+(v-("<<v0<<"))*(v-("<<v0<<")))/(" << b << "*" << b <<"))";  

          MESSAGE("Python function for attractor:" << std::endl << attractorFunction.str());

          FaceId2SizeMap[TopoDS::Face(GeomShape).HashCode(471662)] =attractorFunction.str();
        }
/*
        if (GeomType == TopAbs_EDGE){
          HasSizeMapOnEdge = true;
          HasSizeMapOnFace = true;
        EdgeId2SizeMap[TopoDS::Edge(GeomShape).HashCode(471662)] = atIt->second;
        }
        if (GeomType == TopAbs_VERTEX){
          HasSizeMapOnVertex = true;
          HasSizeMapOnEdge   = true;
          HasSizeMapOnFace   = true;
        VertexId2SizeMap[TopoDS::Vertex(GeomShape).HashCode(471662)] = atIt->second;
        }
*/
      }
    }    


//    if (HasSizeMapOnFace){
    // In all size map cases (hphy_flag = 2), at least map on face must be defined
    MESSAGE("Setting Size Map on FACES ");
    blsurf_data_set_sizemap_iso_cad_face(bls, size_on_surface, &_smp_phy_size);
//    }

    if (HasSizeMapOnEdge){
      MESSAGE("Setting Size Map on EDGES ");
      blsurf_data_set_sizemap_iso_cad_edge(bls, size_on_edge, &_smp_phy_size);
    }
    if (HasSizeMapOnVertex){
      MESSAGE("Setting Size Map on VERTICES ");
      blsurf_data_set_sizemap_iso_cad_point(bls, size_on_vertex, &_smp_phy_size);
    }
  }
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
    MESSAGE("  the shape is a COMPOUND");
  }
  else {
    MESSAGE("  the shape is UNKNOWN");
  };

  context_t *ctx =  context_new();
  context_set_message_callback(ctx, message_callback, &_comment);

  cad_t *c = cad_new(ctx);
 
  blsurf_session_t *bls = blsurf_session_new(ctx);
  

  SetParameters(_hypothesis, bls);

  TopTools_IndexedMapOfShape fmap;
  TopTools_IndexedMapOfShape emap;
  TopTools_IndexedMapOfShape pmap;
  vector<Handle(Geom2d_Curve)> curves;
  vector<Handle(Geom_Surface)> surfaces;



  fmap.Clear();
  FaceId2PythonSmp.clear();
  emap.Clear();
  EdgeId2PythonSmp.clear();
  pmap.Clear();
  VertexId2PythonSmp.clear();
  surfaces.resize(0);
  curves.resize(0);

  assert(Py_IsInitialized());
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();
/*
  Standard_Real u_min;
  Standard_Real v_min;
  Standard_Real u_max;
  Standard_Real v_max;
*/
  int iface = 0;
  string bad_end = "return"; 
  for (TopExp_Explorer face_iter(aShape,TopAbs_FACE);face_iter.More();face_iter.Next()) {
    TopoDS_Face f=TopoDS::Face(face_iter.Current());
    if (fmap.FindIndex(f) > 0)
      continue;
    
    fmap.Add(f);
    iface++;
    surfaces.push_back(BRep_Tool::Surface(f));
    // Get bound values of uv surface
    //BRep_Tool::Surface(f)->Bounds(u_min,u_max,v_min,v_max);
    //MESSAGE("BRep_Tool::Surface(f)->Bounds(u_min,u_max,v_min,v_max): " << u_min << ", " << u_max << ", " << v_min << ", " << v_max);
    
    if ((HasSizeMapOnFace) && FaceId2SizeMap.find(f.HashCode(471662))!=FaceId2SizeMap.end()){
        MESSAGE("FaceId2SizeMap[f.HashCode(471662)].find(bad_end): " << FaceId2SizeMap[f.HashCode(471662)].find(bad_end));
        MESSAGE("FaceId2SizeMap[f.HashCode(471662)].size(): " << FaceId2SizeMap[f.HashCode(471662)].size());
        MESSAGE("bad_end.size(): " << bad_end.size());
      // check if function ends with "return"
        if (FaceId2SizeMap[f.HashCode(471662)].find(bad_end) == (FaceId2SizeMap[f.HashCode(471662)].size()-bad_end.size()-1))
        continue;
      // Expr To Python function, verification is performed at validation in GUI
      PyObject * obj = NULL;
      obj= PyRun_String(FaceId2SizeMap[f.HashCode(471662)].c_str(), Py_file_input, main_dict, NULL);
      Py_DECREF(obj);
      PyObject * func = NULL;
      func = PyObject_GetAttrString(main_mod, "f");
      FaceId2PythonSmp[iface]=func;
      FaceId2SizeMap.erase(f.HashCode(471662));
    }
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
      if ((HasSizeMapOnEdge) && EdgeId2SizeMap.find(e.HashCode(471662))!=EdgeId2SizeMap.end()){
          if (EdgeId2SizeMap[e.HashCode(471662)].find(bad_end) == (EdgeId2SizeMap[e.HashCode(471662)].size()-bad_end.size()-1))
          continue;
        // Expr To Python function, verification is performed at validation in GUI
        PyObject * obj = NULL;
        obj= PyRun_String(EdgeId2SizeMap[e.HashCode(471662)].c_str(), Py_file_input, main_dict, NULL);
        Py_DECREF(obj);
        PyObject * func = NULL;
        func = PyObject_GetAttrString(main_mod, "f");
        EdgeId2PythonSmp[ic]=func;
        EdgeId2SizeMap.erase(e.HashCode(471662));
      }
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
    if ((HasSizeMapOnVertex) && VertexId2SizeMap.find(v.HashCode(471662))!=VertexId2SizeMap.end()){
        if (VertexId2SizeMap[v.HashCode(471662)].find(bad_end) == (VertexId2SizeMap[v.HashCode(471662)].size()-bad_end.size()-1))
            continue;
          // Expr To Python function, verification is performed at validation in GUI
          PyObject * obj = NULL;
          obj= PyRun_String(VertexId2SizeMap[v.HashCode(471662)].c_str(), Py_file_input, main_dict, NULL);
          Py_DECREF(obj);
          PyObject * func = NULL;
          func = PyObject_GetAttrString(main_mod, "f");
          VertexId2PythonSmp[*ip]=func;
          VertexId2SizeMap.erase(v.HashCode(471662));
        }
      }
      if (npts != 2) {
	// should not happen 
	MESSAGE("An edge does not have 2 extremities.");
      } else {
	if (d1 < d2)
	  cad_edge_set_extremities(edg, ip1, ip2);
	else
	  cad_edge_set_extremities(edg, ip2, ip1);
      }
    } // for edge
  } //for face


  PyGILState_Release(gstate);

  blsurf_data_set_cad(bls, c);

  std::cout << std::endl;
  std::cout << "Beginning of Surface Mesh generation" << std::endl;
  std::cout << std::endl;

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

  std::cout << std::endl;
  std::cout << "End of Surface Mesh generation" << std::endl;
  std::cout << std::endl;

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


status_t size_on_surface(integer face_id, real *uv, real *size, void *user_data)
{
  if (face_id == 1) {
    if (my_u_min > uv[0]) {
      my_u_min = uv[0];
    }
    if (my_v_min > uv[1]) {
      my_v_min = uv[1];
    }
    if (my_u_max < uv[0]) {
      my_u_max = uv[0];
    }
    if (my_v_max < uv[1]) {
      my_v_max = uv[1];
    }
  }

  if (FaceId2PythonSmp.count(face_id) != 0){
    PyObject * pyresult = NULL;
    PyObject* new_stderr = NULL;
    assert(Py_IsInitialized());
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    pyresult = PyObject_CallFunction(FaceId2PythonSmp[face_id],"(f,f)",uv[0],uv[1]);
    double result;
    if ( pyresult == NULL){
      fflush(stderr);
      string err_description="";
      new_stderr = newPyStdOut(err_description);
      PySys_SetObject("stderr", new_stderr);
      PyErr_Print();
      PySys_SetObject("stderr", PySys_GetObject("__stderr__"));
      Py_DECREF(new_stderr);
      MESSAGE("Can't evaluate f(" << uv[0] << "," << uv[1] << ")" << " error is " << err_description);
      result = *((double*)user_data);
      }
    else {
      result = PyFloat_AsDouble(pyresult);
      Py_DECREF(pyresult);
    }
    *size = result;
    //MESSAGE("f(" << uv[0] << "," << uv[1] << ")" << " = " << result);
    PyGILState_Release(gstate);
  }
  else {
    *size = *((double*)user_data);
  }
  return STATUS_OK;
}

status_t size_on_edge(integer edge_id, real t, real *size, void *user_data)
{
  if (EdgeId2PythonSmp.count(edge_id) != 0){
    PyObject * pyresult = NULL;
    PyObject* new_stderr = NULL;
    assert(Py_IsInitialized());
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    pyresult = PyObject_CallFunction(EdgeId2PythonSmp[edge_id],"(f)",t);
    double result;
    if ( pyresult == NULL){
      fflush(stderr);
      string err_description="";
      new_stderr = newPyStdOut(err_description);
      PySys_SetObject("stderr", new_stderr);
      PyErr_Print();
      PySys_SetObject("stderr", PySys_GetObject("__stderr__"));
      Py_DECREF(new_stderr);
      MESSAGE("Can't evaluate f(" << t << ")" << " error is " << err_description);
      result = *((double*)user_data);
      }
    else {
      result = PyFloat_AsDouble(pyresult);
      Py_DECREF(pyresult);
    }
    *size = result;
    PyGILState_Release(gstate);
  }
  else {
    *size = *((double*)user_data);
  }
  return STATUS_OK;
}

status_t size_on_vertex(integer point_id, real *size, void *user_data)
{
  if (VertexId2PythonSmp.count(point_id) != 0){
    PyObject * pyresult = NULL;
    PyObject* new_stderr = NULL;
    assert(Py_IsInitialized());
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    pyresult = PyObject_CallFunction(VertexId2PythonSmp[point_id],"");
    double result;
    if ( pyresult == NULL){
      fflush(stderr);
      string err_description="";
      new_stderr = newPyStdOut(err_description);
      PySys_SetObject("stderr", new_stderr);
      PyErr_Print();
      PySys_SetObject("stderr", PySys_GetObject("__stderr__"));
      Py_DECREF(new_stderr);
      MESSAGE("Can't evaluate f()" << " error is " << err_description);
      result = *((double*)user_data);
      }
    else {
      result = PyFloat_AsDouble(pyresult);
      Py_DECREF(pyresult);
    }
    *size = result;
    PyGILState_Release(gstate);
  }
  else {
    *size = *((double*)user_data);
  }
 return STATUS_OK;
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
      std::cout << desc << std::endl;
  }
  return STATUS_OK;
}
