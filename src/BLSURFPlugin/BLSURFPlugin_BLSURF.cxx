// Copyright (C) 2007-2012  CEA/DEN, EDF R&D
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

// ---
// File    : BLSURFPlugin_BLSURF.cxx
// Authors : Francis KLOSS (OCC) & Patrick LAUG (INRIA) & Lioka RAZAFINDRAZAKA (CEA)
//           & Aurelien ALLEAUME (DISTENE)
//           Size maps developement: Nicolas GEIMER (OCC) & Gilles DAVID (EURIWARE)
// ---

#include "BLSURFPlugin_BLSURF.hxx"
#include "BLSURFPlugin_Hypothesis.hxx"
#include "BLSURFPlugin_Attractor.hxx"

extern "C"{
#include <meshgems/meshgems.h>
#include <meshgems/cadsurf.h>
#include <meshgems/precad.h>
}

#include <structmember.h>


#include <Basics_Utils.hxx>
#include <Basics_OCCTVersion.hxx>

#include <SMDS_EdgePosition.hxx>
#include <SMESHDS_Group.hxx>
#include <SMESH_Gen.hxx>
#include <SMESH_Group.hxx>
#include <SMESH_Mesh.hxx>
#include <SMESH_MeshEditor.hxx>
#include <SMESH_MesherHelper.hxx>
#include <StdMeshers_FaceSide.hxx>
#include <StdMeshers_ViscousLayers2D.hxx>

#include <utilities.h>

#include <limits>
#include <list>
#include <vector>
#include <set>
#include <cstdlib>

// OPENCASCADE includes
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
//#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepGProp.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <GProp_GProps.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_Map.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>

#ifndef WNT
#include <fenv.h>
#endif

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
  {(char*)"softspace", T_INT,  offsetof(PyStdOut, softspace), 0,
   (char*)"flag indicating that a space needs to be printed; used by print"},
  {NULL} /* Sentinel */
};

static PyTypeObject PyStdOut_Type = {
  /* The ob_type field must be initialized in the module init function
   * to be portable to Windows without using C++. */
  PyObject_HEAD_INIT(NULL)
  0,                            /*ob_size*/
  "PyOut",                      /*tp_name*/
  sizeof(PyStdOut),             /*tp_basicsize*/
  0,                            /*tp_itemsize*/
  /* methods */
  (destructor)PyStdOut_dealloc, /*tp_dealloc*/
  0,                            /*tp_print*/
  0,                            /*tp_getattr*/
  0,                            /*tp_setattr*/
  0,                            /*tp_compare*/
  0,                            /*tp_repr*/
  0,                            /*tp_as_number*/
  0,                            /*tp_as_sequence*/
  0,                            /*tp_as_mapping*/
  0,                            /*tp_hash*/
  0,                            /*tp_call*/
  0,                            /*tp_str*/
  PyObject_GenericGetAttr,      /*tp_getattro*/
  /* softspace is writable:  we must supply tp_setattro */
  PyObject_GenericSetAttr,      /* tp_setattro */
  0,                            /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT,           /*tp_flags*/
  0,                            /*tp_doc*/
  0,                            /*tp_traverse*/
  0,                            /*tp_clear*/
  0,                            /*tp_richcompare*/
  0,                            /*tp_weaklistoffset*/
  0,                            /*tp_iter*/
  0,                            /*tp_iternext*/
  PyStdOut_methods,             /*tp_methods*/
  PyStdOut_memberlist,          /*tp_members*/
  0,                            /*tp_getset*/
  0,                            /*tp_base*/
  0,                            /*tp_dict*/
  0,                            /*tp_descr_get*/
  0,                            /*tp_descr_set*/
  0,                            /*tp_dictoffset*/
  0,                            /*tp_init*/
  0,                            /*tp_alloc*/
  0,                            /*tp_new*/
  0,                            /*tp_free*/
  0,                            /*tp_is_gc*/
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
TopTools_IndexedMapOfShape FacesWithSizeMap;
std::map<int,string> FaceId2SizeMap;
TopTools_IndexedMapOfShape EdgesWithSizeMap;
std::map<int,string> EdgeId2SizeMap;
TopTools_IndexedMapOfShape VerticesWithSizeMap;
std::map<int,string> VertexId2SizeMap;

std::map<int,PyObject*> FaceId2PythonSmp;
std::map<int,PyObject*> EdgeId2PythonSmp;
std::map<int,PyObject*> VertexId2PythonSmp;

std::map<int,std::vector<double> > FaceId2AttractorCoords;
std::map<int,BLSURFPlugin_Attractor*> FaceId2ClassAttractor;
std::map<int,BLSURFPlugin_Attractor*> FaceIndex2ClassAttractor;

TopTools_IndexedMapOfShape FacesWithEnforcedVertices;
std::map< int, BLSURFPlugin_Hypothesis::TEnfVertexCoordsList > FaceId2EnforcedVertexCoords;
std::map< BLSURFPlugin_Hypothesis::TEnfVertexCoords, BLSURFPlugin_Hypothesis::TEnfVertexCoords > EnfVertexCoords2ProjVertex;
std::map< BLSURFPlugin_Hypothesis::TEnfVertexCoords, BLSURFPlugin_Hypothesis::TEnfVertexList > EnfVertexCoords2EnfVertexList;

bool HasSizeMapOnFace=false;
bool HasSizeMapOnEdge=false;
bool HasSizeMapOnVertex=false;
//bool HasAttractorOnFace=false;

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
  _compatibleHypothesis.push_back(BLSURFPlugin_Hypothesis::GetHypType());
  _compatibleHypothesis.push_back(StdMeshers_ViscousLayers2D::GetHypType());
  _requireDiscreteBoundary = false;
  _onlyUnaryInput = false;
  _hypothesis = NULL;
  _supportSubmeshes = true;

  smeshGen_i = SMESH_Gen_i::GetSMESHGen();
  CORBA::Object_var anObject = smeshGen_i->GetNS()->Resolve("/myStudyManager");
  SALOMEDS::StudyManager_var aStudyMgr = SALOMEDS::StudyManager::_narrow(anObject);

  MESSAGE("studyid = " << _studyId);

  myStudy = NULL;
  myStudy = aStudyMgr->GetStudyByID(_studyId);
  if (myStudy)
    MESSAGE("myStudy->StudyId() = " << myStudy->StudyId());

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

  FacesWithSizeMap.Clear();
  FaceId2SizeMap.clear();
  EdgesWithSizeMap.Clear();
  EdgeId2SizeMap.clear();
  VerticesWithSizeMap.Clear();
  VertexId2SizeMap.clear();
  FaceId2PythonSmp.clear();
  EdgeId2PythonSmp.clear();
  VertexId2PythonSmp.clear();
  FaceId2AttractorCoords.clear();
  FaceId2ClassAttractor.clear();
  FaceIndex2ClassAttractor.clear();
  FacesWithEnforcedVertices.Clear();
  FaceId2EnforcedVertexCoords.clear();
  EnfVertexCoords2ProjVertex.clear();
  EnfVertexCoords2EnfVertexList.clear();

#ifdef WITH_SMESH_CANCEL_COMPUTE
  _compute_canceled = false;
#endif
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
  _hypothesis        = NULL;
  _haveViscousLayers = false;

  list<const SMESHDS_Hypothesis*>::const_iterator itl;
  const SMESHDS_Hypothesis* theHyp;

  const list<const SMESHDS_Hypothesis*>& hyps = GetUsedHypothesis(aMesh, aShape,
                                                                  /*ignoreAuxiliary=*/false);
  aStatus = SMESH_Hypothesis::HYP_OK;
  if ( hyps.empty() )
  {
    return true;  // can work with no hypothesis
  }

  for ( itl = hyps.begin(); itl != hyps.end(); ++itl )
  {
    theHyp = *itl;
    string hypName = theHyp->GetName();
    if ( hypName == BLSURFPlugin_Hypothesis::GetHypType() )
    {
      _hypothesis = static_cast<const BLSURFPlugin_Hypothesis*> (theHyp);
      ASSERT(_hypothesis);
      if ( _hypothesis->GetPhysicalMesh() == BLSURFPlugin_Hypothesis::DefaultSize &&
           _hypothesis->GetGeometricMesh() == BLSURFPlugin_Hypothesis::DefaultGeom )
        //  hphy_flag = 0 and hgeo_flag = 0 is not allowed (spec)
        aStatus = SMESH_Hypothesis::HYP_BAD_PARAMETER;
    }
    else if ( hypName == StdMeshers_ViscousLayers2D::GetHypType() )
    {
      _haveViscousLayers = true;
    }
    else
    {
      aStatus = SMESH_Hypothesis::HYP_INCOMPATIBLE;
    }
  }
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

inline std::string to_string_rel(double d)
{
   std::ostringstream o;
   o << d;
   o << 'r';
   return o.str();
}

inline std::string to_string(int i)
{
   std::ostringstream o;
   o << i;
   return o.str();
}

inline std::string to_string_rel(int i)
{
   std::ostringstream o;
   o << i;
   o << 'r';
   return o.str();
}

double _smp_phy_size;
// #if BLSURF_VERSION_LONG >= "3.1.1"
// //   sizemap_t *geo_sizemap_e, *geo_sizemap_f;
//   sizemap_t *iso_sizemap_p, *iso_sizemap_e, *iso_sizemap_f;
// //   sizemap_t *clean_geo_sizemap_e, *clean_geo_sizemap_f;
//   sizemap_t *clean_iso_sizemap_p, *clean_iso_sizemap_e, *clean_iso_sizemap_f;
// #endif
status_t size_on_surface(integer face_id, real *uv, real *size, void *user_data);
status_t size_on_edge(integer edge_id, real t, real *size, void *user_data);
status_t size_on_vertex(integer vertex_id, real *size, void *user_data);

typedef struct {
        gp_XY uv;
        gp_XYZ xyz;
} projectionPoint;
/////////////////////////////////////////////////////////
projectionPoint getProjectionPoint(const TopoDS_Face& face, const gp_Pnt& point)
{
  projectionPoint myPoint;
  Handle(Geom_Surface) surface = BRep_Tool::Surface(face);
  GeomAPI_ProjectPointOnSurf projector( point, surface );
  if ( !projector.IsDone() || projector.NbPoints()==0 )
    throw "getProjectionPoint: Can't project";

  Quantity_Parameter u,v;
  projector.LowerDistanceParameters(u,v);
  myPoint.uv = gp_XY(u,v);
  gp_Pnt aPnt = projector.NearestPoint();
  myPoint.xyz = gp_XYZ(aPnt.X(),aPnt.Y(),aPnt.Z());
  //return gp_XY(u,v);
  return myPoint;
}
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
double getT(const TopoDS_Edge& edge, const gp_Pnt& point)
{
  Standard_Real f,l;
  Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, f,l);
  GeomAPI_ProjectPointOnCurve projector( point, curve);
  if ( projector.NbPoints() == 0 )
    throw;
  return projector.LowerDistanceParameter();
}

/////////////////////////////////////////////////////////
TopoDS_Shape BLSURFPlugin_BLSURF::entryToShape(std::string entry)
{
  MESSAGE("BLSURFPlugin_BLSURF::entryToShape "<<entry );
  GEOM::GEOM_Object_var aGeomObj;
  TopoDS_Shape S = TopoDS_Shape();
  SALOMEDS::SObject_var aSObj = myStudy->FindObjectID( entry.c_str() );
  SALOMEDS::GenericAttribute_var anAttr;

  if (!aSObj->_is_nil() && aSObj->FindAttribute(anAttr, "AttributeIOR")) {
    SALOMEDS::AttributeIOR_var anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
    CORBA::String_var aVal = anIOR->Value();
    CORBA::Object_var obj = myStudy->ConvertIORToObject(aVal);
    aGeomObj = GEOM::GEOM_Object::_narrow(obj);
  }
  if ( !aGeomObj->_is_nil() )
    S = smeshGen_i->GeomObjectToShape( aGeomObj.in() );
  return S;
}

void _createEnforcedVertexOnFace(TopoDS_Face faceShape, gp_Pnt aPnt, BLSURFPlugin_Hypothesis::TEnfVertex *enfVertex)
{
  BLSURFPlugin_Hypothesis::TEnfVertexCoords enf_coords, coords, s_coords;
  enf_coords.clear();
  coords.clear();
  s_coords.clear();

  // Get the (u,v) values of the enforced vertex on the face
  projectionPoint myPoint = getProjectionPoint(faceShape,aPnt);

  MESSAGE("Enforced Vertex: " << aPnt.X() << ", " << aPnt.Y() << ", " << aPnt.Z());
  MESSAGE("Projected Vertex: " << myPoint.xyz.X() << ", " << myPoint.xyz.Y() << ", " << myPoint.xyz.Z());
  MESSAGE("Parametric coordinates: " << myPoint.uv.X() << ", " << myPoint.uv.Y() );

  enf_coords.push_back(aPnt.X());
  enf_coords.push_back(aPnt.Y());
  enf_coords.push_back(aPnt.Z());

  coords.push_back(myPoint.uv.X());
  coords.push_back(myPoint.uv.Y());
  coords.push_back(myPoint.xyz.X());
  coords.push_back(myPoint.xyz.Y());
  coords.push_back(myPoint.xyz.Z());

  s_coords.push_back(myPoint.xyz.X());
  s_coords.push_back(myPoint.xyz.Y());
  s_coords.push_back(myPoint.xyz.Z());

  // Save pair projected vertex / enf vertex
  MESSAGE("Storing pair projected vertex / enf vertex:");
  MESSAGE("("<< myPoint.xyz.X() << ", " << myPoint.xyz.Y() << ", " << myPoint.xyz.Z() <<") / (" << aPnt.X() << ", " << aPnt.Y() << ", " << aPnt.Z()<<")");
  EnfVertexCoords2ProjVertex[s_coords] = enf_coords;
  MESSAGE("Group name is: \"" << enfVertex->grpName << "\"");
  pair<BLSURFPlugin_Hypothesis::TEnfVertexList::iterator,bool> ret;
  BLSURFPlugin_Hypothesis::TEnfVertexList::iterator it;
  ret = EnfVertexCoords2EnfVertexList[s_coords].insert(enfVertex);
  if (ret.second == false) {
    it = ret.first;
    (*it)->grpName = enfVertex->grpName;
  }

  int key = 0;
  if (! FacesWithEnforcedVertices.Contains(faceShape)) {
    key = FacesWithEnforcedVertices.Add(faceShape);
  }
  else {
    key = FacesWithEnforcedVertices.FindIndex(faceShape);
  }

  // If a node is already created by an attractor, do not create enforced vertex
  int attractorKey = FacesWithSizeMap.FindIndex(faceShape);
  bool sameAttractor = false;
  if (attractorKey >= 0)
    if (FaceId2AttractorCoords.count(attractorKey) > 0)
      if (FaceId2AttractorCoords[attractorKey] == coords)
        sameAttractor = true;

  if (FaceId2EnforcedVertexCoords.find(key) != FaceId2EnforcedVertexCoords.end()) {
    MESSAGE("Map of enf. vertex has key " << key)
    MESSAGE("Enf. vertex list size is: " << FaceId2EnforcedVertexCoords[key].size())
    if (! sameAttractor)
      FaceId2EnforcedVertexCoords[key].insert(coords); // there should be no redondant coords here (see std::set management)
    else
      MESSAGE("An attractor node is already defined: I don't add the enforced vertex");
    MESSAGE("New Enf. vertex list size is: " << FaceId2EnforcedVertexCoords[key].size())
  }
  else {
    MESSAGE("Map of enf. vertex has not key " << key << ": creating it")
    if (! sameAttractor) {
      BLSURFPlugin_Hypothesis::TEnfVertexCoordsList ens;
      ens.insert(coords);
      FaceId2EnforcedVertexCoords[key] = ens;
    }
    else
      MESSAGE("An attractor node is already defined: I don't add the enforced vertex");
  }
}

/////////////////////////////////////////////////////////
void BLSURFPlugin_BLSURF::createEnforcedVertexOnFace(TopoDS_Shape faceShape, BLSURFPlugin_Hypothesis::TEnfVertexList enfVertexList)
{
  BLSURFPlugin_Hypothesis::TEnfVertex* enfVertex;
  gp_Pnt aPnt;

  BLSURFPlugin_Hypothesis::TEnfVertexList::const_iterator enfVertexListIt = enfVertexList.begin();

  for( ; enfVertexListIt != enfVertexList.end() ; ++enfVertexListIt ) {
    enfVertex = *enfVertexListIt;
    // Case of manual coords
    if (enfVertex->coords.size() != 0) {
      aPnt.SetCoord(enfVertex->coords[0],enfVertex->coords[1],enfVertex->coords[2]);
      _createEnforcedVertexOnFace( TopoDS::Face(faceShape),  aPnt, enfVertex);
    }

    // Case of geom vertex coords
    if (enfVertex->geomEntry != "") {
      TopoDS_Shape GeomShape = entryToShape(enfVertex->geomEntry);
      TopAbs_ShapeEnum GeomType  = GeomShape.ShapeType();
       if (GeomType == TopAbs_VERTEX){
         aPnt = BRep_Tool::Pnt(TopoDS::Vertex(GeomShape));
         _createEnforcedVertexOnFace( TopoDS::Face(faceShape),  aPnt, enfVertex);
       }
       // Group Management
       if (GeomType == TopAbs_COMPOUND){
         for (TopoDS_Iterator it (GeomShape); it.More(); it.Next()){
           if (it.Value().ShapeType() == TopAbs_VERTEX){
             aPnt = BRep_Tool::Pnt(TopoDS::Vertex(it.Value()));
             _createEnforcedVertexOnFace( TopoDS::Face(faceShape),  aPnt, enfVertex);
           }
         }
       }
    }
  }
}

/////////////////////////////////////////////////////////
void createAttractorOnFace(TopoDS_Shape GeomShape, std::string AttractorFunction, double defaultSize)
{
  MESSAGE("Attractor function: "<< AttractorFunction);
  double xa, ya, za; // Coordinates of attractor point
  double a, b;       // Attractor parameter
  double d = 0.;
  bool createNode=false; // To create a node on attractor projection
  int pos1, pos2;
  const char *sep = ";";
  // atIt->second has the following pattern:
  // ATTRACTOR(xa;ya;za;a;b;True|False;d)
  // where:
  // xa;ya;za : coordinates of  attractor
  // a        : desired size on attractor
  // b        : distance of influence of attractor
  // d        : distance until which the size remains constant
  //
  // We search the parameters in the string
  // xa
  pos1 = AttractorFunction.find(sep);
  if (pos1!=string::npos)
  xa = atof(AttractorFunction.substr(10, pos1-10).c_str());
  // ya
  pos2 = AttractorFunction.find(sep, pos1+1);
  if (pos2!=string::npos) {
  ya = atof(AttractorFunction.substr(pos1+1, pos2-pos1-1).c_str());
  pos1 = pos2;
  }
  // za
  pos2 = AttractorFunction.find(sep, pos1+1);
  if (pos2!=string::npos) {
  za = atof(AttractorFunction.substr(pos1+1, pos2-pos1-1).c_str());
  pos1 = pos2;
  }
  // a
  pos2 = AttractorFunction.find(sep, pos1+1);
  if (pos2!=string::npos) {
  a = atof(AttractorFunction.substr(pos1+1, pos2-pos1-1).c_str());
  pos1 = pos2;
  }
  // b
  pos2 = AttractorFunction.find(sep, pos1+1);
  if (pos2!=string::npos) {
  b = atof(AttractorFunction.substr(pos1+1, pos2-pos1-1).c_str());
  pos1 = pos2;
  }
  // createNode
  pos2 = AttractorFunction.find(sep, pos1+1);
  if (pos2!=string::npos) {
    string createNodeStr = AttractorFunction.substr(pos1+1, pos2-pos1-1);
    MESSAGE("createNode: " << createNodeStr);
    createNode = (AttractorFunction.substr(pos1+1, pos2-pos1-1) == "True");
    pos1=pos2;
  }
  // d
  pos2 = AttractorFunction.find(")");
  if (pos2!=string::npos) {
  d = atof(AttractorFunction.substr(pos1+1, pos2-pos1-1).c_str());
  }

  // Get the (u,v) values of the attractor on the face
  projectionPoint myPoint = getProjectionPoint(TopoDS::Face(GeomShape),gp_Pnt(xa,ya,za));
  gp_XY uvPoint = myPoint.uv;
  gp_XYZ xyzPoint = myPoint.xyz;
  Standard_Real u0 = uvPoint.X();
  Standard_Real v0 = uvPoint.Y();
  Standard_Real x0 = xyzPoint.X();
  Standard_Real y0 = xyzPoint.Y();
  Standard_Real z0 = xyzPoint.Z();
  std::vector<double> coords;
  coords.push_back(u0);
  coords.push_back(v0);
  coords.push_back(x0);
  coords.push_back(y0);
  coords.push_back(z0);
  // We construct the python function
  ostringstream attractorFunctionStream;
  attractorFunctionStream << "def f(u,v): return ";
  attractorFunctionStream << defaultSize << "-(" << defaultSize <<"-" << a << ")";
  //attractorFunctionStream << "*exp(-((u-("<<u0<<"))*(u-("<<u0<<"))+(v-("<<v0<<"))*(v-("<<v0<<")))/(" << b << "*" << b <<"))";
  // rnc: make possible to keep the size constant until
  // a defined distance. Distance is expressed as the positiv part
  // of r-d where r is the distance to (u0,v0)
  attractorFunctionStream << "*exp(-(0.5*(sqrt((u-"<<u0<<")**2+(v-"<<v0<<")**2)-"<<d<<"+abs(sqrt((u-"<<u0<<")**2+(v-"<<v0<<")**2)-"<<d<<"))/(" << b << "))**2)";

  MESSAGE("Python function for attractor:" << std::endl << attractorFunctionStream.str());

  int key;
  if (! FacesWithSizeMap.Contains(TopoDS::Face(GeomShape))) {
    key = FacesWithSizeMap.Add(TopoDS::Face(GeomShape));
  }
  else {
    key = FacesWithSizeMap.FindIndex(TopoDS::Face(GeomShape));
  }
  FaceId2SizeMap[key] =attractorFunctionStream.str();
  if (createNode) {
    MESSAGE("Creating node on ("<<x0<<","<<y0<<","<<z0<<")");
    FaceId2AttractorCoords[key] = coords;
  }
//   // Test for new attractors
//   gp_Pnt myP(xyzPoint);
//   TopoDS_Vertex myV = BRepBuilderAPI_MakeVertex(myP);
//   BLSURFPlugin_Attractor myAttractor(TopoDS::Face(GeomShape),myV,200);
//   myAttractor.SetParameters(a, defaultSize, b, d);
//   myAttractor.SetType(1);
//   FaceId2ClassAttractor[key] = myAttractor;
//   if(FaceId2ClassAttractor[key].GetFace().IsNull()){
//     MESSAGE("face nulle ");
//   }
//   else
//     MESSAGE("face OK");
//
//   if (FaceId2ClassAttractor[key].GetAttractorShape().IsNull()){
//     MESSAGE("pas de point");
//   }
//   else
//     MESSAGE("point OK");
}

/////////////////////////////////////////////////////////

void BLSURFPlugin_BLSURF::SetParameters(
// #if BLSURF_VERSION_LONG >= "3.1.1"
//                                         cad_t *                          c,
// #endif
                                        const BLSURFPlugin_Hypothesis* hyp,
                                        cadsurf_session_t *            css,
                                        precad_session_t *             pcs,
                                        SMESH_Mesh&                   mesh,
                                        bool *                  use_precad
                                       )
{
  // rnc : Bug 1457
  // Clear map so that it is not stored in the algorithm with old enforced vertices in it
  EnfVertexCoords2EnfVertexList.clear();
  
   double diagonal               = mesh.GetShapeDiagonalSize();
   double bbSegmentation         = _gen->GetBoundaryBoxSegmentation();
   int    _physicalMesh          = BLSURFPlugin_Hypothesis::GetDefaultPhysicalMesh();
   int    _geometricMesh         = BLSURFPlugin_Hypothesis::GetDefaultGeometricMesh();
   double _phySize               = BLSURFPlugin_Hypothesis::GetDefaultPhySize(diagonal, bbSegmentation);
   bool   _phySizeRel            = BLSURFPlugin_Hypothesis::GetDefaultPhySizeRel();
   double _minSize               = BLSURFPlugin_Hypothesis::GetDefaultMinSize(diagonal);
   bool   _minSizeRel            = BLSURFPlugin_Hypothesis::GetDefaultMinSizeRel();
   double _maxSize               = BLSURFPlugin_Hypothesis::GetDefaultMaxSize(diagonal);
   bool   _maxSizeRel            = BLSURFPlugin_Hypothesis::GetDefaultMaxSizeRel();
   double _gradation             = BLSURFPlugin_Hypothesis::GetDefaultGradation();
   bool   _quadAllowed           = BLSURFPlugin_Hypothesis::GetDefaultQuadAllowed();
   double _angleMesh             = BLSURFPlugin_Hypothesis::GetDefaultAngleMesh();
   double _chordalError          = BLSURFPlugin_Hypothesis::GetDefaultChordalError(diagonal);
   bool   _anisotropic           = BLSURFPlugin_Hypothesis::GetDefaultAnisotropic();
   double _anisotropicRatio      = BLSURFPlugin_Hypothesis::GetDefaultAnisotropicRatio();
   bool   _removeTinyEdges       = BLSURFPlugin_Hypothesis::GetDefaultRemoveTinyEdges();
   double _tinyEdgeLength        = BLSURFPlugin_Hypothesis::GetDefaultTinyEdgeLength(diagonal);
   bool   _badElementRemoval     = BLSURFPlugin_Hypothesis::GetDefaultBadElementRemoval();
   double _badElementAspectRatio = BLSURFPlugin_Hypothesis::GetDefaultBadElementAspectRatio();
   bool   _optimizeMesh          = BLSURFPlugin_Hypothesis::GetDefaultOptimizeMesh();
   bool   _quadraticMesh         = BLSURFPlugin_Hypothesis::GetDefaultQuadraticMesh();
   int    _verb                  = BLSURFPlugin_Hypothesis::GetDefaultVerbosity();
   int    _topology              = BLSURFPlugin_Hypothesis::GetDefaultTopology();

  // PreCAD
   int _precadMergeEdges         = BLSURFPlugin_Hypothesis::GetDefaultPreCADMergeEdges();
   int _precadProcess3DTopology  = BLSURFPlugin_Hypothesis::GetDefaultPreCADProcess3DTopology();
   int _precadDiscardInput       = BLSURFPlugin_Hypothesis::GetDefaultPreCADDiscardInput();


  if (hyp) {
    MESSAGE("BLSURFPlugin_BLSURF::SetParameters");
    _physicalMesh  = (int) hyp->GetPhysicalMesh();
    _geometricMesh = (int) hyp->GetGeometricMesh();
     if (hyp->GetPhySize() > 0)
       _phySize       = hyp->GetPhySize();
     _phySizeRel    = hyp->IsPhySizeRel();
     if (hyp->GetMinSize() > 0)
       _minSize       = hyp->GetMinSize();
     _minSizeRel    = hyp->IsMinSizeRel();
     if (hyp->GetMaxSize() > 0)
       _maxSize       = hyp->GetMaxSize();
     _maxSizeRel    = hyp->IsMaxSizeRel();
     if (hyp->GetGradation() > 0)
       _gradation     = hyp->GetGradation();
    _quadAllowed   = hyp->GetQuadAllowed();
     if (hyp->GetAngleMesh() > 0)
     _angleMesh     = hyp->GetAngleMesh();
     if (hyp->GetChordalError() > 0)
       _chordalError           = hyp->GetChordalError();
     _anisotropic            = hyp->GetAnisotropic();
     if (hyp->GetAnisotropicRatio() >= 0)
       _anisotropicRatio       = hyp->GetAnisotropicRatio();
     _removeTinyEdges        = hyp->GetRemoveTinyEdges();
     if (hyp->GetTinyEdgeLength() > 0)
       _tinyEdgeLength         = hyp->GetTinyEdgeLength();
     _badElementRemoval      = hyp->GetBadElementRemoval();
     if (hyp->GetBadElementAspectRatio() >= 0)
       _badElementAspectRatio  = hyp->GetBadElementAspectRatio();
     _optimizeMesh  = hyp->GetOptimizeMesh();
     _quadraticMesh = hyp->GetQuadraticMesh();
    _verb          = hyp->GetVerbosity();
     _topology      = (int) hyp->GetTopology();
     // PreCAD
     _precadMergeEdges = hyp->GetPreCADMergeEdges();
     _precadProcess3DTopology = hyp->GetPreCADProcess3DTopology();
     _precadDiscardInput = hyp->GetPreCADDiscardInput();

    const BLSURFPlugin_Hypothesis::TOptionValues & opts = hyp->GetOptionValues();
    BLSURFPlugin_Hypothesis::TOptionValues::const_iterator opIt;
    for ( opIt = opts.begin(); opIt != opts.end(); ++opIt )
      if ( !opIt->second.empty() ) {
        MESSAGE("cadsurf_set_param(): " << opIt->first << " = " << opIt->second);
        cadsurf_set_param(css, opIt->first.c_str(), opIt->second.c_str());
      }
      
    const BLSURFPlugin_Hypothesis::TOptionValues & preCADopts = hyp->GetPreCADOptionValues();
    for ( opIt = preCADopts.begin(); opIt != preCADopts.end(); ++opIt )
      if ( !opIt->second.empty() ) {
        if (_topology == BLSURFPlugin_Hypothesis::PreCAD) {
          MESSAGE("precad_set_param(): " << opIt->first << " = " << opIt->second);
          precad_set_param(pcs, opIt->first.c_str(), opIt->second.c_str());
        }
      }
  }
//   else {
//     //0020968: EDF1545 SMESH: Problem in the creation of a mesh group on geometry
//     // GetDefaultPhySize() sometimes leads to computation failure
//     // GDD 26/07/2012 From Distene documentation, global physical size default value = diag/100
//     _phySize = BLSURFPlugin_Hypothesis::GetDefaultPhySize(diagonal);
//     _minSize = BLSURFPlugin_Hypothesis::GetDefaultMinSize(diagonal);
//     _maxSize = BLSURFPlugin_Hypothesis::GetDefaultMaxSize(diagonal);
//     _chordalError = BLSURFPlugin_Hypothesis::GetDefaultChordalError(diagonal);
//     _tinyEdgeLength = BLSURFPlugin_Hypothesis::GetDefaultTinyEdgeLength(diagonal);
//     MESSAGE("BLSURFPlugin_BLSURF::SetParameters using defaults");
//   }

  // PreCAD
  if (_topology == BLSURFPlugin_Hypothesis::PreCAD) {
    *use_precad = true;
    precad_set_param(pcs, "verbose",                to_string(_verb).c_str());
    precad_set_param(pcs, "merge_edges",            _precadMergeEdges ? "1" : "0");
    precad_set_param(pcs, "process_3d_topology",    _precadProcess3DTopology ? "1" : "0");
    precad_set_param(pcs, "discard_input_topology", _precadDiscardInput ? "1" : "0");
  }
  
   bool useGradation = false;
   switch (_physicalMesh)
   {
     case BLSURFPlugin_Hypothesis::PhysicalGlobalSize:
       cadsurf_set_param(css, "physical_size_mode", "global");
       cadsurf_set_param(css, "global_physical_size", _phySizeRel ? to_string_rel(_phySize).c_str() : to_string(_phySize).c_str());
       break;
     case BLSURFPlugin_Hypothesis::PhysicalLocalSize:
       cadsurf_set_param(css, "physical_size_mode", "local");
       cadsurf_set_param(css, "global_physical_size", _phySizeRel ? to_string_rel(_phySize).c_str() : to_string(_phySize).c_str());
       useGradation = true;
       break;
     default:
       cadsurf_set_param(css, "physical_size_mode", "none");
   }

   switch (_geometricMesh)
   {
     case BLSURFPlugin_Hypothesis::GeometricalGlobalSize:
       cadsurf_set_param(css, "geometric_size_mode", "global");
       cadsurf_set_param(css, "geometric_approximation", to_string(_angleMesh).c_str());
       cadsurf_set_param(css, "chordal_error", to_string(_chordalError).c_str());
       useGradation = true;
       break;
     case BLSURFPlugin_Hypothesis::GeometricalLocalSize:
       cadsurf_set_param(css, "geometric_size_mode", "local");
       cadsurf_set_param(css, "geometric_approximation", to_string(_angleMesh).c_str());
       cadsurf_set_param(css, "chordal_error", to_string(_chordalError).c_str());
       useGradation = true;
       break;
     default:
       cadsurf_set_param(css, "geometric_size_mode", "none");
   }

   cadsurf_set_param(css, "minsize",                           _minSizeRel ? to_string_rel(_minSize).c_str() : to_string(_minSize).c_str());
   cadsurf_set_param(css, "maxsize",                           _maxSizeRel ? to_string_rel(_maxSize).c_str() : to_string(_maxSize).c_str());
   if ( useGradation )
     cadsurf_set_param(css, "gradation",                         to_string(_gradation).c_str());
   cadsurf_set_param(css, "element_generation",                _quadAllowed ? "quad_dominant" : "triangle");


   cadsurf_set_param(css, "metric",                            _anisotropic ? "anisotropic" : "isotropic");
   if ( _anisotropic )
     cadsurf_set_param(css, "anisotropic_ratio",                 to_string(_anisotropicRatio).c_str());
   cadsurf_set_param(css, "remove_tiny_edges",                 _removeTinyEdges ? "1" : "0");
   if ( _removeTinyEdges )
     cadsurf_set_param(css, "tiny_edge_length",                  to_string(_tinyEdgeLength).c_str());
   cadsurf_set_param(css, "force_bad_surface_element_removal", _badElementRemoval ? "1" : "0");
   if ( _badElementRemoval )
     cadsurf_set_param(css, "bad_surface_element_aspect_ratio",  to_string(_badElementAspectRatio).c_str());
   cadsurf_set_param(css, "optimisation",                      _optimizeMesh ? "yes" : "no");
   cadsurf_set_param(css, "element_order",                     _quadraticMesh ? "quadratic" : "linear");
   cadsurf_set_param(css, "verbose",                           to_string(_verb).c_str());

   _smp_phy_size = _phySizeRel ? _phySize*diagonal : _phySize;
   std::cout << "_smp_phy_size = " << _smp_phy_size << std::endl;

   if (_physicalMesh == BLSURFPlugin_Hypothesis::PhysicalLocalSize){
    TopoDS_Shape GeomShape;
    TopoDS_Shape AttShape;
    TopAbs_ShapeEnum GeomType;
    //
    // Standard Size Maps
    //
    MESSAGE("Setting a Size Map");
    const BLSURFPlugin_Hypothesis::TSizeMap sizeMaps = BLSURFPlugin_Hypothesis::GetSizeMapEntries(hyp);
    BLSURFPlugin_Hypothesis::TSizeMap::const_iterator smIt = sizeMaps.begin();
    for ( ; smIt != sizeMaps.end(); ++smIt ) {
      if ( !smIt->second.empty() ) {
        MESSAGE("cadsurf_set_sizeMap(): " << smIt->first << " = " << smIt->second);
        GeomShape = entryToShape(smIt->first);
        GeomType  = GeomShape.ShapeType();
        MESSAGE("Geomtype is " << GeomType);
        int key = -1;
        // Group Management
        if (GeomType == TopAbs_COMPOUND){
          for (TopoDS_Iterator it (GeomShape); it.More(); it.Next()){
            // Group of faces
            if (it.Value().ShapeType() == TopAbs_FACE){
              HasSizeMapOnFace = true;
              if (! FacesWithSizeMap.Contains(TopoDS::Face(it.Value()))) {
                key = FacesWithSizeMap.Add(TopoDS::Face(it.Value()));
              }
              else {
                key = FacesWithSizeMap.FindIndex(TopoDS::Face(it.Value()));
//                 MESSAGE("Face with key " << key << " already in map");
              }
              FaceId2SizeMap[key] = smIt->second;
            }
            // Group of edges
            if (it.Value().ShapeType() == TopAbs_EDGE){
              HasSizeMapOnEdge = true;
              HasSizeMapOnFace = true;
              if (! EdgesWithSizeMap.Contains(TopoDS::Edge(it.Value()))) {
                key = EdgesWithSizeMap.Add(TopoDS::Edge(it.Value()));
              }
              else {
                key = EdgesWithSizeMap.FindIndex(TopoDS::Edge(it.Value()));
//                 MESSAGE("Edge with key " << key << " already in map");
              }
              EdgeId2SizeMap[key] = smIt->second;
            }
            // Group of vertices
            if (it.Value().ShapeType() == TopAbs_VERTEX){
              HasSizeMapOnVertex = true;
              HasSizeMapOnEdge = true;
              HasSizeMapOnFace = true;
              if (! VerticesWithSizeMap.Contains(TopoDS::Vertex(it.Value()))) {
                key = VerticesWithSizeMap.Add(TopoDS::Vertex(it.Value()));
              }
              else {
                key = VerticesWithSizeMap.FindIndex(TopoDS::Vertex(it.Value()));
                MESSAGE("Group of vertices with key " << key << " already in map");
              }
              MESSAGE("Group of vertices with key " << key << " has a size map: " << smIt->second);
              VertexId2SizeMap[key] = smIt->second;
            }
          }
        }
        // Single face
        if (GeomType == TopAbs_FACE){
          HasSizeMapOnFace = true;
          if (! FacesWithSizeMap.Contains(TopoDS::Face(GeomShape))) {
            key = FacesWithSizeMap.Add(TopoDS::Face(GeomShape));
          }
          else {
            key = FacesWithSizeMap.FindIndex(TopoDS::Face(GeomShape));
//             MESSAGE("Face with key " << key << " already in map");
          }
          FaceId2SizeMap[key] = smIt->second;
        }
        // Single edge
        if (GeomType == TopAbs_EDGE){
          HasSizeMapOnEdge = true;
          HasSizeMapOnFace = true;
          if (! EdgesWithSizeMap.Contains(TopoDS::Edge(GeomShape))) {
            key = EdgesWithSizeMap.Add(TopoDS::Edge(GeomShape));
          }
          else {
            key = EdgesWithSizeMap.FindIndex(TopoDS::Edge(GeomShape));
//             MESSAGE("Edge with key " << key << " already in map");
          }
          EdgeId2SizeMap[key] = smIt->second;
        }
        // Single vertex
        if (GeomType == TopAbs_VERTEX){
          HasSizeMapOnVertex = true;
          HasSizeMapOnEdge   = true;
          HasSizeMapOnFace   = true;
          if (! VerticesWithSizeMap.Contains(TopoDS::Vertex(GeomShape))) {
            key = VerticesWithSizeMap.Add(TopoDS::Vertex(GeomShape));
          }
          else {
            key = VerticesWithSizeMap.FindIndex(TopoDS::Vertex(GeomShape));
             MESSAGE("Vertex with key " << key << " already in map");
          }
          MESSAGE("Vertex with key " << key << " has a size map: " << smIt->second);
          VertexId2SizeMap[key] = smIt->second;
        }
      }
    }

    //
    // Attractors
    //
    // TODO appeler le constructeur des attracteurs directement ici
    MESSAGE("Setting Attractors");
//     if ( !_phySizeRel ) {
      const BLSURFPlugin_Hypothesis::TSizeMap attractors = BLSURFPlugin_Hypothesis::GetAttractorEntries(hyp);
      BLSURFPlugin_Hypothesis::TSizeMap::const_iterator atIt = attractors.begin();
      for ( ; atIt != attractors.end(); ++atIt ) {
        if ( !atIt->second.empty() ) {
          MESSAGE("cadsurf_set_attractor(): " << atIt->first << " = " << atIt->second);
          GeomShape = entryToShape(atIt->first);
          GeomType  = GeomShape.ShapeType();
          // Group Management
          if (GeomType == TopAbs_COMPOUND){
            for (TopoDS_Iterator it (GeomShape); it.More(); it.Next()){
              if (it.Value().ShapeType() == TopAbs_FACE){
                HasSizeMapOnFace = true;
                createAttractorOnFace(it.Value(), atIt->second, _phySizeRel ? _phySize*diagonal : _phySize);
              }
            }
          }

          if (GeomType == TopAbs_FACE){
            HasSizeMapOnFace = true;
            createAttractorOnFace(GeomShape, atIt->second, _phySizeRel ? _phySize*diagonal : _phySize);
          }
  /*
          if (GeomType == TopAbs_EDGE){
            HasSizeMapOnEdge = true;
            HasSizeMapOnFace = true;
          EdgeId2SizeMap[TopoDS::Edge(GeomShape).HashCode(IntegerLast())] = atIt->second;
          }
          if (GeomType == TopAbs_VERTEX){
            HasSizeMapOnVertex = true;
            HasSizeMapOnEdge   = true;
            HasSizeMapOnFace   = true;
          VertexId2SizeMap[TopoDS::Vertex(GeomShape).HashCode(IntegerLast())] = atIt->second;
          }
  */
        }
      }
//     }
//     else
//       MESSAGE("Impossible to create the attractors when the physical size is relative");

    // Class Attractors
    // temporary commented out for testing
    // TODO
    //  - Fill in the BLSURFPlugin_Hypothesis::TAttractorMap map in the hypothesis
    //  - Uncomment and complete this part to construct the attractors from the attractor shape and the passed parameters on each face of the map
    //  - To do this use the public methodss: SetParameters(several double parameters) and SetType(int type)
    //  OR, even better:
    //  - Construct the attractors with an empty dist. map in the hypothesis
    //  - build the map here for each face with an attractor set and only if the attractor shape as changed since the last call to _buildmap()
    //  -> define a bool _mapbuilt in the class that is set to false by default and set to true when calling _buildmap()  OK

    const BLSURFPlugin_Hypothesis::TAttractorMap class_attractors = BLSURFPlugin_Hypothesis::GetClassAttractorEntries(hyp);
    int key=-1;
    BLSURFPlugin_Hypothesis::TAttractorMap::const_iterator AtIt = class_attractors.begin();
    for ( ; AtIt != class_attractors.end(); ++AtIt ) {
      if ( !AtIt->second->Empty() ) {
       // MESSAGE("cadsurf_set_attractor(): " << AtIt->first << " = " << AtIt->second);
        GeomShape = entryToShape(AtIt->first);
        AttShape = AtIt->second->GetAttractorShape();
        GeomType  = GeomShape.ShapeType();
        // Group Management
//         if (GeomType == TopAbs_COMPOUND){
//           for (TopoDS_Iterator it (GeomShape); it.More(); it.Next()){
//             if (it.Value().ShapeType() == TopAbs_FACE){
//               HasAttractorOnFace = true;
//               myAttractor = BLSURFPluginAttractor(GeomShape, AttShape);
//             }
//           }
//         }

        if (GeomType == TopAbs_FACE
          && (AttShape.ShapeType() == TopAbs_VERTEX
           || AttShape.ShapeType() == TopAbs_EDGE
           || AttShape.ShapeType() == TopAbs_WIRE
           || AttShape.ShapeType() == TopAbs_COMPOUND) ){
            HasSizeMapOnFace = true;

            if (! FacesWithSizeMap.Contains(TopoDS::Face(GeomShape)) ) {
                key = FacesWithSizeMap.Add(TopoDS::Face(GeomShape) );
            }
            else {
              key = FacesWithSizeMap.FindIndex(TopoDS::Face(GeomShape));
//                 MESSAGE("Face with key " << key << " already in map");
            }

            FaceId2ClassAttractor[key] = AtIt->second;
        }
        else{
          MESSAGE("Wrong shape type !!")
        }

      }
    }


    //
    // Enforced Vertices
    //
    MESSAGE("Setting Enforced Vertices");
    const BLSURFPlugin_Hypothesis::TFaceEntryEnfVertexListMap entryEnfVertexListMap = BLSURFPlugin_Hypothesis::GetAllEnforcedVerticesByFace(hyp);
    BLSURFPlugin_Hypothesis::TFaceEntryEnfVertexListMap::const_iterator enfIt = entryEnfVertexListMap.begin();
    for ( ; enfIt != entryEnfVertexListMap.end(); ++enfIt ) {
      if ( !enfIt->second.empty() ) {
        GeomShape = entryToShape(enfIt->first);
        GeomType  = GeomShape.ShapeType();
        // Group Management
        if (GeomType == TopAbs_COMPOUND){
          for (TopoDS_Iterator it (GeomShape); it.More(); it.Next()){
            if (it.Value().ShapeType() == TopAbs_FACE){
              HasSizeMapOnFace = true;
              createEnforcedVertexOnFace(it.Value(), enfIt->second);
            }
          }
        }

        if (GeomType == TopAbs_FACE){
          HasSizeMapOnFace = true;
          createEnforcedVertexOnFace(GeomShape, enfIt->second);
        }
      }
    }

    // Internal vertices
    bool useInternalVertexAllFaces = BLSURFPlugin_Hypothesis::GetInternalEnforcedVertexAllFaces(hyp);
    if (useInternalVertexAllFaces) {
      std::string grpName = BLSURFPlugin_Hypothesis::GetInternalEnforcedVertexAllFacesGroup(hyp);
      MESSAGE("Setting Internal Enforced Vertices");
      GeomShape = mesh.GetShapeToMesh();
      gp_Pnt aPnt;
      TopExp_Explorer exp (GeomShape, TopAbs_FACE);
      for (; exp.More(); exp.Next()){
        MESSAGE("Iterating shapes. Shape type is " << exp.Current().ShapeType());
        TopExp_Explorer exp_face (exp.Current(), TopAbs_VERTEX, TopAbs_EDGE);
        for (; exp_face.More(); exp_face.Next())
        {
          // Get coords of vertex
          // Check if current coords is already in enfVertexList
          // If coords not in enfVertexList, add new enfVertex
          aPnt = BRep_Tool::Pnt(TopoDS::Vertex(exp_face.Current()));
          MESSAGE("Found vertex on face at " << aPnt.X() <<", "<<aPnt.Y()<<", "<<aPnt.Z());
          BLSURFPlugin_Hypothesis::TEnfVertex* enfVertex = new BLSURFPlugin_Hypothesis::TEnfVertex();
          enfVertex->coords.push_back(aPnt.X());
          enfVertex->coords.push_back(aPnt.Y());
          enfVertex->coords.push_back(aPnt.Z());
          enfVertex->name = "";
          enfVertex->faceEntries.clear();
          enfVertex->geomEntry = "";
          enfVertex->grpName = grpName;
          enfVertex->vertex = TopoDS::Vertex( exp_face.Current() );
          _createEnforcedVertexOnFace( TopoDS::Face(exp.Current()),  aPnt, enfVertex);
          HasSizeMapOnFace = true;
        }
      }
    }

    MESSAGE("Setting Size Map on FACES ");
// #if BLSURF_VERSION_LONG < "3.1.1"
    cadsurf_data_set_sizemap_iso_cad_face(css, size_on_surface, &_smp_phy_size);
// #else
//     if (*use_precad)
//       iso_sizemap_f = sizemap_new(c, distene_sizemap_type_iso_cad_face, (void *)size_on_surface, NULL);
//     else
//       clean_iso_sizemap_f = sizemap_new(c, distene_sizemap_type_iso_cad_face, (void *)size_on_surface, NULL);
// #endif

    if (HasSizeMapOnEdge){
      MESSAGE("Setting Size Map on EDGES ");
// #if BLSURF_VERSION_LONG < "3.1.1"
      cadsurf_data_set_sizemap_iso_cad_edge(css, size_on_edge, &_smp_phy_size);
// #else
//       if (*use_precad)
//         iso_sizemap_e = sizemap_new(c, distene_sizemap_type_iso_cad_edge, (void *)size_on_edge, NULL);
//       else
//         clean_iso_sizemap_e = sizemap_new(c, distene_sizemap_type_iso_cad_edge, (void *)size_on_edge, NULL);
// #endif
    }
    if (HasSizeMapOnVertex){
      MESSAGE("Setting Size Map on VERTICES ");
// #if BLSURF_VERSION_LONG < "3.1.1"
      cadsurf_data_set_sizemap_iso_cad_point(css, size_on_vertex, &_smp_phy_size);
// #else
//       if (*use_precad)
//         iso_sizemap_p = sizemap_new(c, distene_sizemap_type_iso_cad_point, (void *)size_on_vertex, NULL);
//       else
//         clean_iso_sizemap_p = sizemap_new(c, distene_sizemap_type_iso_cad_point, (void *)size_on_vertex, NULL);
// #endif
    }
  }
}

namespace
{
  // --------------------------------------------------------------------------
  /*!
   * \brief Class correctly terminating usage of BLSURF library at destruction
   */
  class BLSURF_Cleaner
  {
    context_t *       _ctx;
    cadsurf_session_t* _css;
    cad_t *           _cad;
    dcad_t *          _dcad;
  public:
    BLSURF_Cleaner(context_t *       ctx,
                   cadsurf_session_t* css,
                   cad_t *           cad,
                   dcad_t *          dcad)
      : _ctx ( ctx  ),
        _css ( css  ),
        _cad ( cad  ),
        _dcad( dcad )
    {
    }
    ~BLSURF_Cleaner()
    {
      Clean( /*exceptContext=*/false );
    }
    void Clean(const bool exceptContext)
    {
      if ( _css )
      {
        cadsurf_session_delete(_css); _css = 0;

        // #if BLSURF_VERSION_LONG >= "3.1.1"
        // //     if(geo_sizemap_e)
        // //       distene_sizemap_delete(geo_sizemap_e);
        // //     if(geo_sizemap_f)
        // //       distene_sizemap_delete(geo_sizemap_f);
        //     if(iso_sizemap_p)
        //       distene_sizemap_delete(iso_sizemap_p);
        //     if(iso_sizemap_e)
        //       distene_sizemap_delete(iso_sizemap_e);
        //     if(iso_sizemap_f)
        //       distene_sizemap_delete(iso_sizemap_f);
        // 
        // //     if(clean_geo_sizemap_e)
        // //       distene_sizemap_delete(clean_geo_sizemap_e);
        // //     if(clean_geo_sizemap_f)
        // //       distene_sizemap_delete(clean_geo_sizemap_f);
        //     if(clean_iso_sizemap_p)
        //       distene_sizemap_delete(clean_iso_sizemap_p);
        //     if(clean_iso_sizemap_e)
        //       distene_sizemap_delete(clean_iso_sizemap_e);
        //     if(clean_iso_sizemap_f)
        //       distene_sizemap_delete(clean_iso_sizemap_f);
        // #endif

        cad_delete(_cad); _cad = 0;
        dcad_delete(_dcad); _dcad = 0;
        if ( !exceptContext )
        {
          context_delete(_ctx); _ctx = 0;
        }
      }
    }
  };

  // --------------------------------------------------------------------------
  // comparator to sort nodes and sub-meshes
  struct ShapeTypeCompare
  {
    // sort nodes by position in the following order:
    // SMDS_TOP_FACE=2, SMDS_TOP_EDGE=1, SMDS_TOP_VERTEX=0, SMDS_TOP_3DSPACE=3
    int operator()( const SMDS_MeshNode* n1, const SMDS_MeshNode* n2 ) const
    {
      SMDS_TypeOfPosition pos1 = n1->GetPosition()->GetTypeOfPosition();
      SMDS_TypeOfPosition pos2 = n2->GetPosition()->GetTypeOfPosition();
      if ( pos1 == pos2 ) return 0;
      if ( pos1 < pos2 || pos1 == SMDS_TOP_3DSPACE ) return 1;
      return -1;
    }
    // sort sub-meshes in order: EDGE, VERTEX
    bool operator()( const SMESHDS_SubMesh* s1, const SMESHDS_SubMesh* s2 ) const
    {
      int isVertex1 = ( s1 && s1->NbElements() == 0 );
      int isVertex2 = ( s2 && s2->NbElements() == 0 );
      if ( isVertex1 == isVertex2 )
        return s1 < s2;
      return isVertex1 < isVertex2;
    }
  };

  //================================================================================
  /*!
   * \brief Fills groups on nodes to be merged
   */
  //================================================================================

  void getNodeGroupsToMerge( const SMESHDS_SubMesh*                smDS,
                             const TopoDS_Shape&                   shape,
                             SMESH_MeshEditor::TListOfListOfNodes& nodeGroupsToMerge)
  {
    SMDS_NodeIteratorPtr nIt = smDS->GetNodes();
    switch ( shape.ShapeType() )
    {
    case TopAbs_VERTEX: {
      std::list< const SMDS_MeshNode* > nodes;
      while ( nIt->more() )
        nodes.push_back( nIt->next() );
      if ( nodes.size() > 1 )
        nodeGroupsToMerge.push_back( nodes );
      break;
    }
    case TopAbs_EDGE: {
      std::multimap< double, const SMDS_MeshNode* > u2node;
      const SMDS_EdgePosition* ePos;
      while ( nIt->more() )
      {
        const SMDS_MeshNode* n = nIt->next();
        if (( ePos = dynamic_cast< const SMDS_EdgePosition* >( n->GetPosition() )))
          u2node.insert( make_pair( ePos->GetUParameter(), n ));
      }
      if ( u2node.size() < 2 ) return;

      double tol = (( u2node.rbegin()->first - u2node.begin()->first ) / 20.) / u2node.size();
      std::multimap< double, const SMDS_MeshNode* >::iterator un2, un1;
      for ( un2 = u2node.begin(), un1 = un2++; un2 != u2node.end(); un1 = un2++ )
      {
        if (( un2->first - un1->first ) <= tol )
        {
          std::list< const SMDS_MeshNode* > nodes;
          nodes.push_back( un1->second );
          while (( un2->first - un1->first ) <= tol )
          {
            nodes.push_back( un2->second );
            if ( ++un2 == u2node.end()) {
              --un2;
              break;
            }
          }
          // make nodes created on the boundary of viscous layer replace nodes created
          // by BLSURF as their SMDS_Position is more correct
          nodes.sort( ShapeTypeCompare() );
          nodeGroupsToMerge.push_back( nodes );
        }
      }
      break;
    }
    default: ;
    }
    // SMESH_MeshEditor::TListOfListOfNodes::const_iterator nll = nodeGroupsToMerge.begin();
    // for ( ; nll != nodeGroupsToMerge.end(); ++nll )
    // {
    //   cout << "Merge ";
    //   const std::list< const SMDS_MeshNode* >& nl = *nll;
    //   std::list< const SMDS_MeshNode* >::const_iterator nIt = nl.begin();
    //   for ( ; nIt != nl.end(); ++nIt )
    //     cout << (*nIt) << " ";
    //   cout << endl;
    // }
    // cout << endl;
  }

  //================================================================================
  /*!
   * \brief A temporary mesh used to compute mesh on a proxy FACE
   */
  //================================================================================

  struct TmpMesh: public SMESH_Mesh
  {
    typedef std::map<const SMDS_MeshNode*, const SMDS_MeshNode*, TIDCompare > TN2NMap;
    TN2NMap     _tmp2origNN;
    TopoDS_Face _proxyFace;

    TmpMesh()
    {
      _myMeshDS = new SMESHDS_Mesh( _id, true );
    }
    //--------------------------------------------------------------------------------
    /*!
     * \brief Creates a FACE bound by viscous layers and mesh each its EDGE with 1 segment
     */
    //--------------------------------------------------------------------------------

    const TopoDS_Face& makeProxyFace( SMESH_ProxyMesh::Ptr& viscousMesh,
                                      const TopoDS_Face&    origFace)
    {
      // get data of nodes on inner boundary of viscous layers
      SMESH_Mesh* origMesh = viscousMesh->GetMesh();
      TError err;
      TSideVector wireVec = StdMeshers_FaceSide::GetFaceWires(origFace, *origMesh,
                                                              /*skipMediumNodes = */true,
                                                              err, viscousMesh );
      if ( err && err->IsKO() )
        throw *err.get(); // it should be caught at SMESH_subMesh

      // proxy nodes and corresponding tmp VERTEXes
      std::vector<const SMDS_MeshNode*> origNodes;
      std::vector<TopoDS_Vertex>        tmpVertex;

      // create a proxy FACE
      TopoDS_Shape origFaceCopy = origFace.EmptyCopied();
      BRepBuilderAPI_MakeFace newFace( TopoDS::Face( origFaceCopy ));
      for ( size_t iW = 0; iW != wireVec.size(); ++iW )
      {
        StdMeshers_FaceSidePtr& wireData = wireVec[iW];
        const UVPtStructVec& wirePoints = wireData->GetUVPtStruct();
        if ( wirePoints.size() < 3 )
          continue;

        BRepBuilderAPI_MakePolygon wire;
        for ( size_t iN = 1; iN < wirePoints.size(); ++iN )
        {
          wire.Add( SMESH_TNodeXYZ( wirePoints[ iN ].node ));
          origNodes.push_back( wirePoints[ iN ].node );
          tmpVertex.push_back( wire.LastVertex() );
        }
        tmpVertex[0] = wire.FirstVertex();
        wire.Close();
        if ( !wire.IsDone() )
          throw SALOME_Exception("BLSURFPlugin_BLSURF: BRepBuilderAPI_MakePolygon failed");
        newFace.Add( wire );
      }
      _proxyFace = newFace;

      // set a new shape to mesh
      TopoDS_Compound auxCompoundToMesh;
      BRep_Builder shapeBuilder;
      shapeBuilder.MakeCompound( auxCompoundToMesh );
      shapeBuilder.Add( auxCompoundToMesh, _proxyFace );
      shapeBuilder.Add( auxCompoundToMesh, origMesh->GetShapeToMesh() );

      ShapeToMesh( auxCompoundToMesh );

      //TopExp_Explorer fExp( auxCompoundToMesh, TopAbs_FACE );
      //_proxyFace = TopoDS::Face( fExp.Current() );


      // Make input mesh for BLSURF: segments on EDGE's of newFace

      // make nodes and fill in _tmp2origNN
      //
      SMESHDS_Mesh* tmpMeshDS = GetMeshDS();
      for ( size_t i = 0; i < origNodes.size(); ++i )
      {
        GetSubMesh( tmpVertex[i] )->ComputeStateEngine( SMESH_subMesh::COMPUTE );
        if ( const SMDS_MeshNode* tmpN = SMESH_Algo::VertexNode( tmpVertex[i], tmpMeshDS ))
          _tmp2origNN.insert( _tmp2origNN.end(), make_pair( tmpN, origNodes[i] ));
        else
          throw SALOME_Exception("BLSURFPlugin_BLSURF: a proxy vertex not meshed");
      }

      // make segments
      TopoDS_Vertex v1, v2;
      for ( TopExp_Explorer edge( _proxyFace, TopAbs_EDGE ); edge.More(); edge.Next() )
      {
        const TopoDS_Edge& E = TopoDS::Edge( edge.Current() );
        TopExp::Vertices( E, v1, v2 );
        const SMDS_MeshNode* n1 = SMESH_Algo::VertexNode( v1, tmpMeshDS );
        const SMDS_MeshNode* n2 = SMESH_Algo::VertexNode( v2, tmpMeshDS );

        if ( SMDS_MeshElement* seg = tmpMeshDS->AddEdge( n1, n2 ))
          tmpMeshDS->SetMeshElementOnShape( seg, E );
      }

      return _proxyFace;
    }

    //--------------------------------------------------------------------------------
    /*!
     * \brief Fill in the origMesh with faces computed by BLSURF in this tmp mesh
     */
    //--------------------------------------------------------------------------------

    void FillInOrigMesh( SMESH_Mesh&        origMesh,
                         const TopoDS_Face& origFace )
    {
      SMESH_MesherHelper helper( origMesh );
      helper.SetSubShape( origFace );
      helper.SetElementsOnShape( true );

      // iterate over tmp faces and copy them in origMesh
      const SMDS_MeshNode* nodes[27];
      const SMDS_MeshNode* nullNode = 0;
      double xyz[3];
      SMDS_FaceIteratorPtr fIt = GetMeshDS()->facesIterator(/*idInceasingOrder=*/true);
      while ( fIt->more() )
      {
        const SMDS_MeshElement* f = fIt->next();
        SMDS_ElemIteratorPtr nIt = f->nodesIterator();
        int nbN = 0;
        for ( ; nIt->more(); ++nbN )
        {
          const SMDS_MeshNode* n = static_cast<const SMDS_MeshNode*>( nIt->next() );
          TN2NMap::iterator n2nIt = 
            _tmp2origNN.insert( _tmp2origNN.end(), make_pair( n, nullNode ));
          if ( !n2nIt->second ) {
            n->GetXYZ( xyz );
            gp_XY uv = helper.GetNodeUV( _proxyFace, n );
            n2nIt->second = helper.AddNode( xyz[0], xyz[1], xyz[2], uv.X(), uv.Y() );
          }
          nodes[ nbN ] = n2nIt->second;
        }
        switch( nbN ) {
        case 3: helper.AddFace( nodes[0], nodes[1], nodes[2] ); break;
        // case 6: helper.AddFace( nodes[0], nodes[1], nodes[2],
        //                         nodes[3], nodes[4], nodes[5]); break;
        case 4: helper.AddFace( nodes[0], nodes[1], nodes[2], nodes[3] ); break;
        // case 9: helper.AddFace( nodes[0], nodes[1], nodes[2], nodes[3],
        //                         nodes[4], nodes[5], nodes[6], nodes[7], nodes[8]); break;
        // case 8: helper.AddFace( nodes[0], nodes[1], nodes[2], nodes[3],
        //                         nodes[4], nodes[5], nodes[6], nodes[7]); break;
        }
      }
    }
  };


} // namespace

status_t curv_fun(real t, real *uv, real *dt, real *dtt, void *user_data);
status_t surf_fun(real *uv, real *xyz, real*du, real *dv,
                  real *duu, real *duv, real *dvv, void *user_data);
status_t message_cb(message_t *msg, void *user_data);
status_t interrupt_cb(integer *interrupt_status, void *user_data);

//=============================================================================
/*!
 *
 */
//=============================================================================

bool BLSURFPlugin_BLSURF::Compute(SMESH_Mesh& aMesh, const TopoDS_Shape& aShape) {

  MESSAGE("BLSURFPlugin_BLSURF::Compute");

  // Fix problem with locales
  Kernel_Utils::Localizer aLocalizer;

  if ( !compute( aMesh, aShape ))
    return false;

  if ( _haveViscousLayers )
  {
    // Compute viscous layers

    TopTools_MapOfShape map;
    for (TopExp_Explorer face_iter(aShape,TopAbs_FACE);face_iter.More();face_iter.Next())
    {
      const TopoDS_Face& F = TopoDS::Face(face_iter.Current());
      if ( !map.Add( F )) continue;
      SMESH_ProxyMesh::Ptr viscousMesh = StdMeshers_ViscousLayers2D::Compute( aMesh, F );
      if ( !viscousMesh )
        return false; // error in StdMeshers_ViscousLayers2D::Compute()

      // Compute BLSURF mesh on viscous layers

      if ( viscousMesh->NbProxySubMeshes() > 0 )
      {
        TmpMesh tmpMesh;
        const TopoDS_Face& proxyFace = tmpMesh.makeProxyFace( viscousMesh, F );
        if ( !compute( tmpMesh, proxyFace ))
          return false;
        tmpMesh.FillInOrigMesh( aMesh, F );
      }
    }

    // Re-compute BLSURF mesh on the rest faces if the mesh was cleared

    for (TopExp_Explorer face_iter(aShape,TopAbs_FACE);face_iter.More();face_iter.Next())
    {
      const TopoDS_Face& F = TopoDS::Face(face_iter.Current());
      SMESH_subMesh* fSM = aMesh.GetSubMesh( F );
      if ( fSM->IsMeshComputed() ) continue;

      if ( !compute( aMesh, aShape ))
        return false;
      break;
    }
  }
  return true;
}

//=============================================================================
/*!
 *
 */
//=============================================================================

bool BLSURFPlugin_BLSURF::compute(SMESH_Mesh&         aMesh,
                                  const TopoDS_Shape& aShape)
{
  /* create a distene context (generic object) */
  status_t status = STATUS_ERROR;

  SMESHDS_Mesh* meshDS = aMesh.GetMeshDS();
  SMESH_MesherHelper helper( aMesh );
  // do not call helper.IsQuadraticSubMesh() because sub-meshes
  // may be cleaned and helper.myTLinkNodeMap gets invalid in such a case
  bool haveQuadraticSubMesh = SMESH_MesherHelper( aMesh ).IsQuadraticSubMesh( aShape );
  bool quadraticSubMeshAndViscousLayer = false;
  bool needMerge = false;
  typedef set< SMESHDS_SubMesh*, ShapeTypeCompare > TSubMeshSet;
  TSubMeshSet edgeSubmeshes;
  TSubMeshSet& mergeSubmeshes = edgeSubmeshes;

  TopTools_IndexedMapOfShape fmap;
  TopTools_IndexedMapOfShape emap;
  TopTools_IndexedMapOfShape pmap;

  // Issue 0019864. On DebianSarge, FE signals do not obey to OSD::SetSignal(false)
#ifndef WNT
  feclearexcept( FE_ALL_EXCEPT );
  int oldFEFlags = fedisableexcept( FE_ALL_EXCEPT );
#endif

  context_t *ctx =  context_new();

  /* Set the message callback in the working context */
  context_set_message_callback(ctx, message_cb, &_comment);
#ifdef WITH_SMESH_CANCEL_COMPUTE
  _compute_canceled = false;
  context_set_interrupt_callback(ctx, interrupt_cb, this);
#else
  context_set_interrupt_callback(ctx, interrupt_cb, NULL);
#endif

  /* create the CAD object we will work on. It is associated to the context ctx. */
  cad_t *c     = cad_new(ctx);
  dcad_t *dcad = dcad_new(c);

  FacesWithSizeMap.Clear();
  FaceId2SizeMap.clear();
  FaceId2ClassAttractor.clear();
  FaceIndex2ClassAttractor.clear();
  EdgesWithSizeMap.Clear();
  EdgeId2SizeMap.clear();
  VerticesWithSizeMap.Clear();
  VertexId2SizeMap.clear();

  /* Now fill the CAD object with data from your CAD
   * environement. This is the most complex part of a successfull
   * integration.
   */

  // PreCAD
  // If user requests it, send the CAD through Distene preprocessor : PreCAD
  cad_t *cleanc = NULL; // preprocessed cad
  precad_session_t *pcs = precad_session_new(ctx);
  precad_data_set_cad(pcs, c);

  cadsurf_session_t *css = cadsurf_session_new(ctx);

  // an object that correctly deletes all cadsurf objects at destruction
  BLSURF_Cleaner cleaner( ctx,css,c,dcad );

  MESSAGE("BEGIN SetParameters");
  bool use_precad = false;
  SetParameters(
                // #if BLSURF_VERSION_LONG >= "3.1.1"
                //     c,
                // #endif
                _hypothesis, css, pcs, aMesh, &use_precad);
  MESSAGE("END SetParameters");

  haveQuadraticSubMesh = haveQuadraticSubMesh || (_hypothesis != NULL && _hypothesis->GetQuadraticMesh());
  helper.SetIsQuadratic( haveQuadraticSubMesh );

  // To remove as soon as quadratic mesh is allowed - BEGIN
  // GDD: Viscous layer is not allowed with quadratic mesh
  if (_haveViscousLayers && haveQuadraticSubMesh ) {
    quadraticSubMeshAndViscousLayer = true;
    _haveViscousLayers = !haveQuadraticSubMesh;
    _comment += "Warning: Viscous layer is not possible with a quadratic mesh, it is ignored.";
    error(COMPERR_WARNING, _comment);
  }
  // To remove as soon as quadratic mesh is allowed - END

  // needed to prevent the opencascade memory managmement from freeing things
  vector<Handle(Geom2d_Curve)> curves;
  vector<Handle(Geom_Surface)> surfaces;

  fmap.Clear();
  emap.Clear();
  pmap.Clear();
  FaceId2PythonSmp.clear();
  EdgeId2PythonSmp.clear();
  VertexId2PythonSmp.clear();

  /****************************************************************************************
                                          FACES
  *****************************************************************************************/
  int iface = 0;
  string bad_end = "return";
  int faceKey = -1;
  TopTools_IndexedMapOfShape _map;
  TopExp::MapShapes(aShape,TopAbs_VERTEX,_map);
  int ienf = _map.Extent();

  assert(Py_IsInitialized());
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();

  string theSizeMapStr;

  for (TopExp_Explorer face_iter(aShape,TopAbs_FACE);face_iter.More();face_iter.Next())
  {
    TopoDS_Face f = TopoDS::Face(face_iter.Current());

    // make INTERNAL face oriented FORWARD (issue 0020993)
    if (f.Orientation() != TopAbs_FORWARD && f.Orientation() != TopAbs_REVERSED )
      f.Orientation(TopAbs_FORWARD);

    if (fmap.FindIndex(f) > 0)
      continue;
    iface = fmap.Add(f);

    surfaces.push_back(BRep_Tool::Surface(f));

    /* create an object representing the face for cadsurf */
    /* where face_id is an integer identifying the face.
     * surf_function is the function that defines the surface
     * (For this face, it will be called by cadsurf with your_face_object_ptr
     * as last parameter.
     */
    cad_face_t *fce = cad_face_new(c, iface, surf_fun, surfaces.back());

    /* by default a face has no tag (color).
       The following call sets it to the same value as the face_id : */
    cad_face_set_tag(fce, iface);

    /* Set face orientation (optional if you want a well oriented output mesh)*/
    if(f.Orientation() != TopAbs_FORWARD)
      cad_face_set_orientation(fce, CAD_ORIENTATION_REVERSED);
    else
      cad_face_set_orientation(fce, CAD_ORIENTATION_FORWARD);

    if (HasSizeMapOnFace && !use_precad)
    {
      // -----------------
      // Classic size map
      // -----------------
      faceKey = FacesWithSizeMap.FindIndex(f);


      if (FaceId2SizeMap.find(faceKey)!=FaceId2SizeMap.end()) {
        MESSAGE("A size map is defined on face :"<<faceKey)
          theSizeMapStr = FaceId2SizeMap[faceKey];
        // check if function ends with "return"
        if (theSizeMapStr.find(bad_end) == (theSizeMapStr.size()-bad_end.size()-1))
          continue;
        // Expr To Python function, verification is performed at validation in GUI
        PyObject * obj = NULL;
        obj= PyRun_String(theSizeMapStr.c_str(), Py_file_input, main_dict, NULL);
        Py_DECREF(obj);
        PyObject * func = NULL;
        func = PyObject_GetAttrString(main_mod, "f");
        FaceId2PythonSmp[iface]=func;
        FaceId2SizeMap.erase(faceKey);
      }

      // Specific size map = Attractor
      std::map<int,std::vector<double> >::iterator attractor_iter = FaceId2AttractorCoords.begin();

      for (; attractor_iter != FaceId2AttractorCoords.end(); ++attractor_iter) {
        if (attractor_iter->first == faceKey) {
          MESSAGE("Face indice: " << iface);
          MESSAGE("Adding attractor");

          double xyzCoords[3]  = {attractor_iter->second[2],
                                  attractor_iter->second[3],
                                  attractor_iter->second[4]};

          MESSAGE("Check position of vertex =(" << xyzCoords[0] << "," << xyzCoords[1] << "," << xyzCoords[2] << ")");
          gp_Pnt P(xyzCoords[0],xyzCoords[1],xyzCoords[2]);
          BRepClass_FaceClassifier scl(f,P,1e-7);
          // OCC 6.3sp6 : scl.Perform() is bugged. The function was rewritten
          // BRepClass_FaceClassifierPerform(&scl,f,P,1e-7);
          // OCC 6.5.2: scl.Perform() is not bugged anymore
          scl.Perform(f, P, 1e-7);
          TopAbs_State result = scl.State();
          MESSAGE("Position of point on face: "<<result);
          if ( result == TopAbs_OUT )
            MESSAGE("Point is out of face: node is not created");
          if ( result == TopAbs_UNKNOWN )
            MESSAGE("Point position on face is unknown: node is not created");
          if ( result == TopAbs_ON )
            MESSAGE("Point is on border of face: node is not created");
          if ( result == TopAbs_IN )
          {
            // Point is inside face and not on border
            MESSAGE("Point is in face: node is created");
            double uvCoords[2] = {attractor_iter->second[0],attractor_iter->second[1]};
            ienf++;
            MESSAGE("Add cad point on (u,v)=(" << uvCoords[0] << "," << uvCoords[1] << ") with id = " << ienf);
            cad_point_t* point_p = cad_point_new(fce, ienf, uvCoords);
            cad_point_set_tag(point_p, ienf);
          }
          FaceId2AttractorCoords.erase(faceKey);
        }
      }

      // -----------------
      // Class Attractors
      // -----------------
      std::map<int,BLSURFPlugin_Attractor* >::iterator clAttractor_iter = FaceId2ClassAttractor.find(faceKey);
      if (clAttractor_iter != FaceId2ClassAttractor.end()){
        MESSAGE("Face indice: " << iface);
        MESSAGE("Adding attractor");
        FaceIndex2ClassAttractor[iface]=clAttractor_iter->second;
        FaceId2ClassAttractor.erase(clAttractor_iter);
      }
    } // if (HasSizeMapOnFace && !use_precad)

      // ------------------
      // Enforced Vertices
      // ------------------
    faceKey = FacesWithEnforcedVertices.FindIndex(f);
    std::map<int,BLSURFPlugin_Hypothesis::TEnfVertexCoordsList >::const_iterator evmIt = FaceId2EnforcedVertexCoords.find(faceKey);
    if (evmIt != FaceId2EnforcedVertexCoords.end()) {
      MESSAGE("Some enforced vertices are defined");
      BLSURFPlugin_Hypothesis::TEnfVertexCoordsList evl;
      MESSAGE("Face indice: " << iface);
      MESSAGE("Adding enforced vertices");
      evl = evmIt->second;
      MESSAGE("Number of vertices to add: "<< evl.size());
      BLSURFPlugin_Hypothesis::TEnfVertexCoordsList::const_iterator evlIt = evl.begin();
      for (; evlIt != evl.end(); ++evlIt) {
        BLSURFPlugin_Hypothesis::TEnfVertexCoords xyzCoords;
        xyzCoords.push_back(evlIt->at(2));
        xyzCoords.push_back(evlIt->at(3));
        xyzCoords.push_back(evlIt->at(4));
        MESSAGE("Check position of vertex =(" << xyzCoords[0] << "," << xyzCoords[1] << "," << xyzCoords[2] << ")");
        gp_Pnt P(xyzCoords[0],xyzCoords[1],xyzCoords[2]);
        BRepClass_FaceClassifier scl(f,P,1e-7);
        // OCC 6.3sp6 : scl.Perform() is bugged. The function was rewritten
        // BRepClass_FaceClassifierPerform(&scl,f,P,1e-7);
        // OCC 6.5.2: scl.Perform() is not bugged anymore
        scl.Perform(f, P, 1e-7);
        TopAbs_State result = scl.State();
        MESSAGE("Position of point on face: "<<result);
        if ( result == TopAbs_OUT ) {
          MESSAGE("Point is out of face: node is not created");
          if (EnfVertexCoords2ProjVertex.find(xyzCoords) != EnfVertexCoords2ProjVertex.end()) {
            EnfVertexCoords2ProjVertex.erase(xyzCoords);
            EnfVertexCoords2EnfVertexList.erase(xyzCoords);
          }
        }
        if ( result == TopAbs_UNKNOWN ) {
          MESSAGE("Point position on face is unknown: node is not created");
          if (EnfVertexCoords2ProjVertex.find(xyzCoords) != EnfVertexCoords2ProjVertex.end()) {
            EnfVertexCoords2ProjVertex.erase(xyzCoords);
            EnfVertexCoords2EnfVertexList.erase(xyzCoords);
          }
        }
        if ( result == TopAbs_ON ) {
          MESSAGE("Point is on border of face: node is not created");
          if (EnfVertexCoords2ProjVertex.find(xyzCoords) != EnfVertexCoords2ProjVertex.end()) {
            EnfVertexCoords2ProjVertex.erase(xyzCoords);
            EnfVertexCoords2EnfVertexList.erase(xyzCoords);
          }
        }
        if ( result == TopAbs_IN )
        {
          // Point is inside face and not on border
          MESSAGE("Point is in face: node is created");
          double uvCoords[2]   = {evlIt->at(0),evlIt->at(1)};
          ienf++;
          MESSAGE("Add cad point on (u,v)=(" << uvCoords[0] << "," << uvCoords[1] << ") with id = " << ienf);
          cad_point_t* point_p = cad_point_new(fce, ienf, uvCoords);
          int tag = 0;
          std::map< BLSURFPlugin_Hypothesis::TEnfVertexCoords, BLSURFPlugin_Hypothesis::TEnfVertexList >::const_iterator enfCoordsIt = EnfVertexCoords2EnfVertexList.find(xyzCoords);
          if (enfCoordsIt != EnfVertexCoords2EnfVertexList.end() &&
              !enfCoordsIt->second.empty() )
          {
            // to merge nodes of an INTERNAL vertex belonging to several faces
            TopoDS_Vertex     v = (*enfCoordsIt->second.begin())->vertex;
            if ( v.IsNull() ) v = (*enfCoordsIt->second.rbegin())->vertex;
            if ( !v.IsNull() ) {
              tag = pmap.Add( v );
              SMESH_subMesh* vSM = aMesh.GetSubMesh( v );
              vSM->ComputeStateEngine( SMESH_subMesh::COMPUTE );
              mergeSubmeshes.insert( vSM->GetSubMeshDS() );
              //if ( tag != pmap.Extent() )
              needMerge = true;
            }
          }
          if ( tag == 0 ) tag = ienf;
          cad_point_set_tag(point_p, tag);
        }
      }
      FaceId2EnforcedVertexCoords.erase(faceKey);

    }

    /****************************************************************************************
                                           EDGES
                        now create the edges associated to this face
    *****************************************************************************************/
    int edgeKey = -1;
    for (TopExp_Explorer edge_iter(f,TopAbs_EDGE);edge_iter.More();edge_iter.Next())
    {
      TopoDS_Edge e = TopoDS::Edge(edge_iter.Current());
      int ic = emap.FindIndex(e);
      if (ic <= 0)
        ic = emap.Add(e);

      double tmin,tmax;
      curves.push_back(BRep_Tool::CurveOnSurface(e, f, tmin, tmax));

      if (HasSizeMapOnEdge){
        edgeKey = EdgesWithSizeMap.FindIndex(e);
        if (EdgeId2SizeMap.find(edgeKey)!=EdgeId2SizeMap.end()) {
          MESSAGE("Sizemap defined on edge with index " << edgeKey);
          theSizeMapStr = EdgeId2SizeMap[edgeKey];
          if (theSizeMapStr.find(bad_end) == (theSizeMapStr.size()-bad_end.size()-1))
            continue;
          // Expr To Python function, verification is performed at validation in GUI
          PyObject * obj = NULL;
          obj= PyRun_String(theSizeMapStr.c_str(), Py_file_input, main_dict, NULL);
          Py_DECREF(obj);
          PyObject * func = NULL;
          func = PyObject_GetAttrString(main_mod, "f");
          EdgeId2PythonSmp[ic]=func;
          EdgeId2SizeMap.erase(edgeKey);
        }
      }
      /* data of nodes existing on the edge */
      StdMeshers_FaceSidePtr nodeData;
      SMESH_subMesh* sm = aMesh.GetSubMesh( e );
      if ( !sm->IsEmpty() )
      {
        SMESH_subMeshIteratorPtr subsmIt = sm->getDependsOnIterator( /*includeSelf=*/true,
                                                                     /*complexFirst=*/false);
        while ( subsmIt->more() )
          edgeSubmeshes.insert( subsmIt->next()->GetSubMeshDS() );

        nodeData.reset( new StdMeshers_FaceSide( f, e, &aMesh, /*isForwrd = */true,
                                                 /*ignoreMedium=*/haveQuadraticSubMesh));
        if ( nodeData->MissVertexNode() )
          return error(COMPERR_BAD_INPUT_MESH,"No node on vertex");

        const std::vector<UVPtStruct>& nodeDataVec = nodeData->GetUVPtStruct();
        if ( !nodeDataVec.empty() )
        {
          if ( Abs( nodeDataVec[0].param - tmin ) > Abs( nodeDataVec.back().param - tmin ))
          {
            nodeData->Reverse();
            nodeData->GetUVPtStruct(); // nodeData recomputes nodeDataVec
          }
          // tmin and tmax can change in case of viscous layer on an adjacent edge
          tmin = nodeDataVec.front().param;
          tmax = nodeDataVec.back().param;
        }
        else
        {
          cout << "---------------- Invalid nodeData" << endl;
          nodeData.reset();
        }
      }

      /* attach the edge to the current cadsurf face */
      cad_edge_t *edg = cad_edge_new(fce, ic, tmin, tmax, curv_fun, curves.back());

      /* by default an edge has no tag (color).
         The following call sets it to the same value as the edge_id : */
      cad_edge_set_tag(edg, ic);

      /* by default, an edge does not necessalry appear in the resulting mesh,
         unless the following property is set :
      */
      cad_edge_set_property(edg, EDGE_PROPERTY_SOFT_REQUIRED);

      /* by default an edge is a boundary edge */
      if (e.Orientation() == TopAbs_INTERNAL)
        cad_edge_set_property(edg, EDGE_PROPERTY_INTERNAL);

      // pass existing nodes of sub-meshes to BLSURF
      if ( nodeData )
      {
        const std::vector<UVPtStruct>& nodeDataVec = nodeData->GetUVPtStruct();
        const int                      nbNodes     = nodeDataVec.size();

        dcad_edge_discretization_t *dedge;
        dcad_get_edge_discretization(dcad, edg, &dedge);
        dcad_edge_discretization_set_vertex_count( dedge, nbNodes );

        // cout << endl << " EDGE " << ic << endl;
        // cout << "tmin = "<<tmin << ", tmax = "<< tmax << endl;
        for ( int iN = 0; iN < nbNodes; ++iN )
        {
          const UVPtStruct& nData = nodeDataVec[ iN ];
          double t                = nData.param;
          real uv[2]              = { nData.u, nData.v };
          SMESH_TNodeXYZ nXYZ( nData.node );
          // cout << "\tt = " << t
          //      << "\t uv = ( " << uv[0] << ","<< uv[1] << " ) "
          //      << "\t u = " << nData.param
          //      << "\t ID = " << nData.node->GetID() << endl;
          dcad_edge_discretization_set_vertex_coordinates( dedge, iN+1, t, uv, nXYZ._xyz );
        }
        dcad_edge_discretization_set_property(dedge, DISTENE_DCAD_PROPERTY_REQUIRED);
      }

      /****************************************************************************************
                                      VERTICES
      *****************************************************************************************/

      int npts = 0;
      int ip1, ip2, *ip;
      gp_Pnt2d e0 = curves.back()->Value(tmin);
      gp_Pnt ee0 = surfaces.back()->Value(e0.X(), e0.Y());
      Standard_Real d1=0,d2=0;

      int vertexKey = -1;
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

        if (HasSizeMapOnVertex){
          vertexKey = VerticesWithSizeMap.FindIndex(v);
          if (VertexId2SizeMap.find(vertexKey)!=VertexId2SizeMap.end()){
            theSizeMapStr = VertexId2SizeMap[vertexKey];
            //MESSAGE("VertexId2SizeMap[faceKey]: " << VertexId2SizeMap[vertexKey]);
            if (theSizeMapStr.find(bad_end) == (theSizeMapStr.size()-bad_end.size()-1))
              continue;
            // Expr To Python function, verification is performed at validation in GUI
            PyObject * obj = NULL;
            obj= PyRun_String(theSizeMapStr.c_str(), Py_file_input, main_dict, NULL);
            Py_DECREF(obj);
            PyObject * func = NULL;
            func = PyObject_GetAttrString(main_mod, "f");
            VertexId2PythonSmp[*ip]=func;
            VertexId2SizeMap.erase(vertexKey);   // do not erase if using a vector
          }
        }
      }
      if (npts != 2) {
        // should not happen
        MESSAGE("An edge does not have 2 extremities.");
      } else {
        if (d1 < d2) {
          // This defines the curves extremity connectivity
          cad_edge_set_extremities(edg, ip1, ip2);
          /* set the tag (color) to the same value as the extremity id : */
          cad_edge_set_extremities_tag(edg, ip1, ip2);
        }
        else {
          cad_edge_set_extremities(edg, ip2, ip1);
          cad_edge_set_extremities_tag(edg, ip2, ip1);
        }
      }
    } // for edge
  } //for face

  // Clear mesh from already meshed edges if possible else
  // remember that merge is needed
  TSubMeshSet::iterator smIt = edgeSubmeshes.begin();
  for ( ; smIt != edgeSubmeshes.end(); ++smIt ) // loop on already meshed EDGEs
  {
    SMESHDS_SubMesh* smDS = *smIt;
    if ( !smDS ) continue;
    SMDS_NodeIteratorPtr nIt = smDS->GetNodes();
    if ( nIt->more() )
    {
      const SMDS_MeshNode* n = nIt->next();
      if ( n->NbInverseElements( SMDSAbs_Face ) > 0 )
      {
        needMerge = true;
        // add existing medium nodes to helper
        if ( aMesh.NbEdges( ORDER_QUADRATIC ) > 0 )
        {
          SMDS_ElemIteratorPtr edgeIt = smDS->GetElements();
          while ( edgeIt->more() )
            helper.AddTLinks( static_cast<const SMDS_MeshEdge*>(edgeIt->next()));
        }
        continue;
      }
    }
    {
      SMDS_ElemIteratorPtr eIt = smDS->GetElements();
      while ( eIt->more() ) meshDS->RemoveFreeElement( eIt->next(), smDS );
      SMDS_NodeIteratorPtr nIt = smDS->GetNodes();
      while ( nIt->more() ) meshDS->RemoveFreeNode( nIt->next(), smDS );
    }
  }


  if (use_precad) {
    /* Now launch the PreCAD process */
    status = precad_process(pcs);
    if(status != STATUS_OK){
      cout << "PreCAD processing failed with error code " << status << "\n";
    }
    else {
      // retrieve the pre-processed CAD object
      cleanc = precad_new_cad(pcs);
      if(!cleanc){
        cout << "Unable to retrieve PreCAD result \n";
      }
      cout << "PreCAD processing successfull \n";

      // #if BLSURF_VERSION_LONG >= "3.1.1"
      //       /* We can now get the updated sizemaps (if any) */
      // //       if(geo_sizemap_e)
      // //         clean_geo_sizemap_e = precad_new_sizemap(pcs, geo_sizemap_e);
      // // 
      // //       if(geo_sizemap_f)
      // //         clean_geo_sizemap_f = precad_new_sizemap(pcs, geo_sizemap_f);
      //
      //       if(iso_sizemap_p)
      //         clean_iso_sizemap_p = precad_new_sizemap(pcs, iso_sizemap_p);
      //
      //       if(iso_sizemap_e)
      //         clean_iso_sizemap_e = precad_new_sizemap(pcs, iso_sizemap_e);
      //
      //       if(iso_sizemap_f)
      //         clean_iso_sizemap_f = precad_new_sizemap(pcs, iso_sizemap_f);
      // #endif
    }
    // Now we can delete the PreCAD session
    precad_session_delete(pcs);
  }

  cadsurf_data_set_dcad(css, dcad);
  if (cleanc) {
    // Give the pre-processed CAD object to the current BLSurf session
    cadsurf_data_set_cad(css, cleanc);
  }
  else {
    // Use the original one
    cadsurf_data_set_cad(css, c);
  }

  std::cout << std::endl;
  std::cout << "Beginning of Surface Mesh generation" << std::endl;
  std::cout << std::endl;

  try {
    OCC_CATCH_SIGNALS;

    status = cadsurf_compute_mesh(css);

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
      _comment = "Exception in cadsurf_compute_mesh()";
  }
  if ( status != STATUS_OK) {
    // There was an error while meshing
    error(_comment);
  }

  PyGILState_Release(gstate);

  std::cout << std::endl;
  std::cout << "End of Surface Mesh generation" << std::endl;
  std::cout << std::endl;

  mesh_t *msh = NULL;
  cadsurf_data_get_mesh(css, &msh);
  if(!msh){
    /* release the mesh object */
    cadsurf_data_regain_mesh(css, msh);
    return error(_comment);
  }

  std::string GMFFileName = BLSURFPlugin_Hypothesis::GetDefaultGMFFile();
  if (_hypothesis)
    GMFFileName = _hypothesis->GetGMFFile();
  if (GMFFileName != "") {
    //     bool GMFFileMode = _hypothesis->GetGMFFileMode();
    bool asciiFound = (GMFFileName.find(".mesh",GMFFileName.length()-5) != std::string::npos);
    bool binaryFound = (GMFFileName.find(".meshb",GMFFileName.length()-6) != std::string::npos);
    if (!asciiFound && !binaryFound)
      GMFFileName.append(".mesh");
    mesh_write_mesh(msh, GMFFileName.c_str());
  }

  /* retrieve mesh data (see meshgems/mesh.h) */
  integer nv, ne, nt, nq, vtx[4], tag;
  integer *evedg, *evtri, *evquad, type;
  real xyz[3];

  mesh_get_vertex_count(msh, &nv);
  mesh_get_edge_count(msh, &ne);
  mesh_get_triangle_count(msh, &nt);
  mesh_get_quadrangle_count(msh, &nq);

  evedg  = (integer *)mesh_calloc_generic_buffer(msh);
  evtri  = (integer *)mesh_calloc_generic_buffer(msh);
  evquad = (integer *)mesh_calloc_generic_buffer(msh);

  SMDS_MeshNode** nodes = new SMDS_MeshNode*[nv+1];
  bool* tags = new bool[nv+1];

  /* enumerated vertices */
  for(int iv=1;iv<=nv;iv++) {
    mesh_get_vertex_coordinates(msh, iv, xyz);
    mesh_get_vertex_tag(msh, iv, &tag);
    // Issue 0020656. Use vertex coordinates
    if ( tag > 0 && tag <= pmap.Extent() ) {
      TopoDS_Vertex v = TopoDS::Vertex(pmap(tag));
      double tol = BRep_Tool::Tolerance( v );
      gp_Pnt p = BRep_Tool::Pnt( v );
      if ( p.IsEqual( gp_Pnt( xyz[0], xyz[1], xyz[2]), 2*tol))
        xyz[0] = p.X(), xyz[1] = p.Y(), xyz[2] = p.Z();
      else
        tag = 0; // enforced or attracted vertex
    }
    nodes[iv] = meshDS->AddNode(xyz[0], xyz[1], xyz[2]);

    // Create group of enforced vertices if requested
    BLSURFPlugin_Hypothesis::TEnfVertexCoords projVertex;
    projVertex.clear();
    projVertex.push_back((double)xyz[0]);
    projVertex.push_back((double)xyz[1]);
    projVertex.push_back((double)xyz[2]);
    std::map< BLSURFPlugin_Hypothesis::TEnfVertexCoords, BLSURFPlugin_Hypothesis::TEnfVertexList >::const_iterator enfCoordsIt = EnfVertexCoords2EnfVertexList.find(projVertex);
    if (enfCoordsIt != EnfVertexCoords2EnfVertexList.end()) {
      MESSAGE("Found enforced vertex @ " << xyz[0] << ", " << xyz[1] << ", " << xyz[2]);
      BLSURFPlugin_Hypothesis::TEnfVertexList::const_iterator enfListIt = enfCoordsIt->second.begin();
      BLSURFPlugin_Hypothesis::TEnfVertex *currentEnfVertex;
      for (; enfListIt != enfCoordsIt->second.end(); ++enfListIt) {
        currentEnfVertex = (*enfListIt);
        if (currentEnfVertex->grpName != "") {
          bool groupDone = false;
          SMESH_Mesh::GroupIteratorPtr grIt = aMesh.GetGroups();
          MESSAGE("currentEnfVertex->grpName: " << currentEnfVertex->grpName);
          MESSAGE("Parsing the groups of the mesh");
          while (grIt->more()) {
            SMESH_Group * group = grIt->next();
            if ( !group ) continue;
            MESSAGE("Group: " << group->GetName());
            SMESHDS_GroupBase* groupDS = group->GetGroupDS();
            if ( !groupDS ) continue;
            MESSAGE("group->SMDSGroup().GetType(): " << (groupDS->GetType()));
            MESSAGE("group->SMDSGroup().GetType()==SMDSAbs_Node: " << (groupDS->GetType()==SMDSAbs_Node));
            MESSAGE("currentEnfVertex->grpName.compare(group->GetStoreName())==0: " << (currentEnfVertex->grpName.compare(group->GetName())==0));
            if ( groupDS->GetType()==SMDSAbs_Node && currentEnfVertex->grpName.compare(group->GetName())==0) {
              SMESHDS_Group* aGroupDS = static_cast<SMESHDS_Group*>( groupDS );
              aGroupDS->SMDSGroup().Add(nodes[iv]);
              MESSAGE("Node ID: " << nodes[iv]->GetID());
              // How can I inform the hypothesis ?
              //                 _hypothesis->AddEnfVertexNodeID(currentEnfVertex->grpName,nodes[iv]->GetID());
              groupDone = true;
              MESSAGE("Successfully added enforced vertex to existing group " << currentEnfVertex->grpName);
              break;
            }
          }
          if (!groupDone)
          {
            int groupId;
            SMESH_Group* aGroup = aMesh.AddGroup(SMDSAbs_Node, currentEnfVertex->grpName.c_str(), groupId);
            aGroup->SetName( currentEnfVertex->grpName.c_str() );
            SMESHDS_Group* aGroupDS = static_cast<SMESHDS_Group*>( aGroup->GetGroupDS() );
            aGroupDS->SMDSGroup().Add(nodes[iv]);
            MESSAGE("Successfully created enforced vertex group " << currentEnfVertex->grpName);
            groupDone = true;
          }
          if (!groupDone)
            throw SALOME_Exception(LOCALIZED("An enforced vertex node was not added to a group"));
        }
        else
          MESSAGE("Group name is empty: '"<<currentEnfVertex->grpName<<"' => group is not created");
      }
    }

    // internal points are tagged to zero
    if(tag > 0 && tag <= pmap.Extent() ){
      meshDS->SetNodeOnVertex(nodes[iv], TopoDS::Vertex(pmap(tag)));
      tags[iv] = false;
    } else {
      tags[iv] = true;
    }
  }

  /* enumerate edges */
  for(int it=1;it<=ne;it++) {
    SMDS_MeshEdge* edg;
    mesh_get_edge_vertices(msh, it, vtx);
    mesh_get_edge_extra_vertices(msh, it, &type, evedg);
    mesh_get_edge_tag(msh, it, &tag);
    if (tags[vtx[0]]) {
      Set_NodeOnEdge(meshDS, nodes[vtx[0]], emap(tag));
      tags[vtx[0]] = false;
    };
    if (tags[vtx[1]]) {
      Set_NodeOnEdge(meshDS, nodes[vtx[1]], emap(tag));
      tags[vtx[1]] = false;
    };
    if (type == MESHGEMS_MESH_ELEMENT_TYPE_EDGE3) {
      // QUADRATIC EDGE
      if (tags[evedg[0]]) {
        Set_NodeOnEdge(meshDS, nodes[evedg[0]], emap(tag));
        tags[evedg[0]] = false;
      }
      edg = meshDS->AddEdge(nodes[vtx[0]], nodes[vtx[1]], nodes[evedg[0]]);
    }
    else {
      edg = helper.AddEdge(nodes[vtx[0]], nodes[vtx[1]]);
    }
    meshDS->SetMeshElementOnShape(edg, TopoDS::Edge(emap(tag)));
  }

  /* enumerate triangles */
  for(int it=1;it<=nt;it++) {
    SMDS_MeshFace* tri;
    mesh_get_triangle_vertices(msh, it, vtx);
    mesh_get_triangle_extra_vertices(msh, it, &type, evtri);
    mesh_get_triangle_tag(msh, it, &tag);
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
    if (type == MESHGEMS_MESH_ELEMENT_TYPE_TRIA6) {
      // QUADRATIC TRIANGLE
      if (tags[evtri[0]]) {
        meshDS->SetNodeOnFace(nodes[evtri[0]], TopoDS::Face(fmap(tag)));
        tags[evtri[0]] = false;
      }
      if (tags[evtri[1]]) {
        meshDS->SetNodeOnFace(nodes[evtri[1]], TopoDS::Face(fmap(tag)));
        tags[evtri[1]] = false;
      }
      if (tags[evtri[2]]) {
        meshDS->SetNodeOnFace(nodes[evtri[2]], TopoDS::Face(fmap(tag)));
        tags[evtri[2]] = false;
      }
      tri = meshDS->AddFace(nodes[vtx[0]], nodes[vtx[1]], nodes[vtx[2]],
                            nodes[evtri[0]], nodes[evtri[1]], nodes[evtri[2]]);
    }
    else {
      tri = helper.AddFace(nodes[vtx[0]], nodes[vtx[1]], nodes[vtx[2]]);
    }
    meshDS->SetMeshElementOnShape(tri, TopoDS::Face(fmap(tag)));
  }

  /* enumerate quadrangles */
  for(int it=1;it<=nq;it++) {
    SMDS_MeshFace* quad;
    mesh_get_quadrangle_vertices(msh, it, vtx);
    mesh_get_quadrangle_extra_vertices(msh, it, &type, evquad);
    mesh_get_quadrangle_tag(msh, it, &tag);
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
    if (type == MESHGEMS_MESH_ELEMENT_TYPE_QUAD9) {
      // QUADRATIC QUADRANGLE
      std::cout << "This is a quadratic quadrangle" << std::endl;
      if (tags[evquad[0]]) {
        meshDS->SetNodeOnFace(nodes[evquad[0]], TopoDS::Face(fmap(tag)));
        tags[evquad[0]] = false;
      }
      if (tags[evquad[1]]) {
        meshDS->SetNodeOnFace(nodes[evquad[1]], TopoDS::Face(fmap(tag)));
        tags[evquad[1]] = false;
      }
      if (tags[evquad[2]]) {
        meshDS->SetNodeOnFace(nodes[evquad[2]], TopoDS::Face(fmap(tag)));
        tags[evquad[2]] = false;
      }
      if (tags[evquad[3]]) {
        meshDS->SetNodeOnFace(nodes[evquad[3]], TopoDS::Face(fmap(tag)));
        tags[evquad[3]] = false;
      }
      if (tags[evquad[4]]) {
        meshDS->SetNodeOnFace(nodes[evquad[4]], TopoDS::Face(fmap(tag)));
        tags[evquad[4]] = false;
      }
      quad = meshDS->AddFace(nodes[vtx[0]], nodes[vtx[1]], nodes[vtx[2]], nodes[vtx[3]],
                             nodes[evquad[0]], nodes[evquad[1]], nodes[evquad[2]], nodes[evquad[3]],
                             nodes[evquad[4]]);
    }
    else {
      quad = helper.AddFace(nodes[vtx[0]], nodes[vtx[1]], nodes[vtx[2]], nodes[vtx[3]]);
    }
    meshDS->SetMeshElementOnShape(quad, TopoDS::Face(fmap(tag)));
  }

  /* release the mesh object, the rest is released by cleaner */
  cadsurf_data_regain_mesh(css, msh);

  delete [] nodes;
  delete [] tags;

  if ( needMerge ) // sew mesh computed by BLSURF with pre-existing mesh
  {
    SMESH_MeshEditor editor( &aMesh );
    SMESH_MeshEditor::TListOfListOfNodes nodeGroupsToMerge;
    TIDSortedElemSet segementsOnEdge;
    TIDSortedNodeSet nodesOnEdge;
    TSubMeshSet::iterator smIt;
    SMESHDS_SubMesh* smDS;
    typedef SMDS_StdIterator< const SMDS_MeshNode*, SMDS_NodeIteratorPtr > TNodeIterator;
    double tol;

    // merge nodes on EDGE's with ones computed by BLSURF
    for ( smIt = mergeSubmeshes.begin(); smIt != mergeSubmeshes.end(); ++smIt )
    {
      if (! (smDS = *smIt) ) continue;
      getNodeGroupsToMerge( smDS, meshDS->IndexToShape((*smIt)->GetID()), nodeGroupsToMerge );

      SMDS_ElemIteratorPtr segIt = smDS->GetElements();
      while ( segIt->more() )
        segementsOnEdge.insert( segIt->next() );
    }
    // merge nodes
    editor.MergeNodes( nodeGroupsToMerge );

    // merge segments
    SMESH_MeshEditor::TListOfListOfElementsID equalSegments;
    editor.FindEqualElements( segementsOnEdge, equalSegments );
    editor.MergeElements( equalSegments );

    // remove excess segments created on the boundary of viscous layers
    const SMDS_TypeOfPosition onFace = SMDS_TOP_FACE;
    for ( int i = 1; i <= emap.Extent(); ++i )
    {
      if ( SMESHDS_SubMesh* smDS = meshDS->MeshElements( emap( i )))
      {
        SMDS_ElemIteratorPtr segIt = smDS->GetElements();
        while ( segIt->more() )
        {
          const SMDS_MeshElement* seg = segIt->next();
          if ( seg->GetNode(0)->GetPosition()->GetTypeOfPosition() == onFace ||
               seg->GetNode(1)->GetPosition()->GetTypeOfPosition() == onFace )
            meshDS->RemoveFreeElement( seg, smDS );
        }
      }
    }
  }

  // SetIsAlwaysComputed( true ) to sub-meshes of EDGEs w/o mesh
  TopLoc_Location loc; double f,l;
  for (int i = 1; i <= emap.Extent(); i++)
    if ( SMESH_subMesh* sm = aMesh.GetSubMeshContaining( emap( i )))
      sm->SetIsAlwaysComputed( true );
  for (int i = 1; i <= pmap.Extent(); i++)
    if ( SMESH_subMesh* sm = aMesh.GetSubMeshContaining( pmap( i )))
      if ( !sm->IsMeshComputed() )
        sm->SetIsAlwaysComputed( true );

  // Set error to FACE's w/o elements
  for ( int i = 1; i <= fmap.Extent(); ++i )
  {
    SMESH_subMesh* sm = aMesh.GetSubMesh( fmap(i) );
    if ( !sm->GetSubMeshDS() || sm->GetSubMeshDS()->NbElements() == 0 )
      sm->GetComputeError().reset
        ( new SMESH_ComputeError( COMPERR_ALGO_FAILED, _comment, this ));
  }

  // Issue 0019864. On DebianSarge, FE signals do not obey to OSD::SetSignal(false)
#ifndef WNT
  if ( oldFEFlags > 0 )
    feenableexcept( oldFEFlags );
  feclearexcept( FE_ALL_EXCEPT );
#endif

  /*
    std::cout << "FacesWithSizeMap" << std::endl;
    FacesWithSizeMap.Statistics(std::cout);
    std::cout << "EdgesWithSizeMap" << std::endl;
    EdgesWithSizeMap.Statistics(std::cout);
    std::cout << "VerticesWithSizeMap" << std::endl;
    VerticesWithSizeMap.Statistics(std::cout);
    std::cout << "FacesWithEnforcedVertices" << std::endl;
    FacesWithEnforcedVertices.Statistics(std::cout);
  */

  MESSAGE("END OF BLSURFPlugin_BLSURF::Compute()");
  return ( status == STATUS_OK && !quadraticSubMeshAndViscousLayer );
}

//================================================================================
/*!
 * \brief Terminates computation
 */
//================================================================================

#ifdef WITH_SMESH_CANCEL_COMPUTE
void BLSURFPlugin_BLSURF::CancelCompute()
{
  _compute_canceled = true;
}
#endif

//=============================================================================
/*!
 *  SetNodeOnEdge
 */
//=============================================================================

void BLSURFPlugin_BLSURF::Set_NodeOnEdge(SMESHDS_Mesh* meshDS, SMDS_MeshNode* node, const TopoDS_Shape& ed) {
  const TopoDS_Edge edge = TopoDS::Edge(ed);

  gp_Pnt pnt(node->X(), node->Y(), node->Z());

  Standard_Real p0 = 0.0;
  Standard_Real p1 = 1.0;
  TopLoc_Location loc;
  Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, loc, p0, p1);

  if ( !loc.IsIdentity() ) pnt.Transform( loc.Transformation().Inverted() );
  GeomAPI_ProjectPointOnCurve proj(pnt, curve, p0, p1);

  double pa = 0.;
  if ( proj.NbPoints() > 0 )
  {
    pa = (double)proj.LowerDistanceParameter();
    // Issue 0020656. Move node if it is too far from edge
    gp_Pnt curve_pnt = curve->Value( pa );
    double dist2     = pnt.SquareDistance( curve_pnt );
    double tol       = BRep_Tool::Tolerance( edge );
    if ( 1e-14 < dist2 && dist2 <= 1000*tol ) // large enough and within tolerance
    {
      curve_pnt.Transform( loc );
      meshDS->MoveNode( node, curve_pnt.X(), curve_pnt.Y(), curve_pnt.Z() );
    }
  }
//   GProp_GProps LProps;
//   BRepGProp::LinearProperties(ed, LProps);
//   double lg = (double)LProps.Mass();

  meshDS->SetNodeOnEdge(node, edge, pa);
}

/* Curve definition function See cad_curv_t in file meshgems/cad.h for
 * more information.
 * NOTE : if when your CAD systems evaluates second
 * order derivatives it also computes first order derivatives and
 * function evaluation, you can optimize this example by making only
 * one CAD call and filling the necessary uv, dt, dtt arrays.
 */
status_t curv_fun(real t, real *uv, real *dt, real *dtt, void *user_data)
{
  /* t is given. It contains the t (time) 1D parametric coordintaes
     of the point PreCAD/BLSurf is querying on the curve */

  /* user_data identifies the edge PreCAD/BLSurf is querying
   * (see cad_edge_new later in this example) */
  const Geom2d_Curve*pargeo = (const Geom2d_Curve*) user_data;

  if (uv){
   /* BLSurf is querying the function evaluation */
    gp_Pnt2d P;
    P=pargeo->Value(t);
    uv[0]=P.X(); uv[1]=P.Y();
  }

  if(dt) {
   /* query for the first order derivatives */
    gp_Vec2d V1;
    V1=pargeo->DN(t,1);
    dt[0]=V1.X(); dt[1]=V1.Y();
  }

  if(dtt){
    /* query for the second order derivatives */
    gp_Vec2d V2;
    V2=pargeo->DN(t,2);
    dtt[0]=V2.X(); dtt[1]=V2.Y();
  }

  return STATUS_OK;
}

/* Surface definition function.
 * See cad_surf_t in file meshgems/cad.h for more information.
 * NOTE : if when your CAD systems evaluates second order derivatives it also
 * computes first order derivatives and function evaluation, you can optimize
 * this example by making only one CAD call and filling the necessary xyz, du, dv, etc..
 * arrays.
 */
status_t surf_fun(real *uv, real *xyz, real*du, real *dv,
                  real *duu, real *duv, real *dvv, void *user_data)
{
  /* uv[2] is given. It contains the u,v coordinates of the point
   * PreCAD/BLSurf is querying on the surface */

  /* user_data identifies the face PreCAD/BLSurf is querying (see
   * cad_face_new later in this example)*/
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

  return STATUS_OK;
}


status_t size_on_surface(integer face_id, real *uv, real *size, void *user_data)
{
  //MESSAGE("size_on_surface")
  if (FaceId2PythonSmp.count(face_id) != 0){
    //MESSAGE("A size map is used to calculate size on face : "<<face_id)
    PyObject * pyresult = NULL;
    PyObject* new_stderr = NULL;
    assert(Py_IsInitialized());
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    pyresult = PyObject_CallFunction(FaceId2PythonSmp[face_id],(char*)"(f,f)",uv[0],uv[1]);
    real result;
    if ( pyresult != NULL) {
      result = PyFloat_AsDouble(pyresult);
      Py_DECREF(pyresult);
//       *size = result;
    }
    else{
      fflush(stderr);
      string err_description="";
      new_stderr = newPyStdOut(err_description);
      PySys_SetObject((char*)"stderr", new_stderr);
      PyErr_Print();
      PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
      Py_DECREF(new_stderr);
      MESSAGE("Can't evaluate f(" << uv[0] << "," << uv[1] << ")" << " error is " << err_description);
      result = *((real*)user_data);
    }
    *size = result;
    PyGILState_Release(gstate);
  }
  else if (FaceIndex2ClassAttractor.count(face_id) !=0 && !FaceIndex2ClassAttractor[face_id]->Empty()){
//    MESSAGE("attractor used on face :"<<face_id)
    // MESSAGE("List of attractor is not empty")
    // MESSAGE("Attractor empty : "<< FaceIndex2ClassAttractor[face_id]->Empty())
    real result = FaceIndex2ClassAttractor[face_id]->GetSize(uv[0],uv[1]);
    *size = result;
  }
  else {
    // MESSAGE("List of attractor is empty !!!")
    *size = *((real*)user_data);
  }
//   std::cout << "Size_on_surface sur la face " << face_id << " donne une size de: " << *size << std::endl;
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
    pyresult = PyObject_CallFunction(EdgeId2PythonSmp[edge_id],(char*)"(f)",t);
    real result;
    if ( pyresult != NULL) {
      result = PyFloat_AsDouble(pyresult);
      Py_DECREF(pyresult);
//       *size = result;
    }
    else{
      fflush(stderr);
      string err_description="";
      new_stderr = newPyStdOut(err_description);
      PySys_SetObject((char*)"stderr", new_stderr);
      PyErr_Print();
      PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
      Py_DECREF(new_stderr);
      MESSAGE("Can't evaluate f(" << t << ")" << " error is " << err_description);
      result = *((real*)user_data);
    }
    *size = result;
    PyGILState_Release(gstate);
  }
  else {
    *size = *((real*)user_data);
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
    pyresult = PyObject_CallFunction(VertexId2PythonSmp[point_id],(char*)"");
    real result;
    if ( pyresult != NULL) {
      result = PyFloat_AsDouble(pyresult);
      Py_DECREF(pyresult);
//       *size = result;
    }
    else {
      fflush(stderr);
      string err_description="";
      new_stderr = newPyStdOut(err_description);
      PySys_SetObject((char*)"stderr", new_stderr);
      PyErr_Print();
      PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
      Py_DECREF(new_stderr);
      MESSAGE("Can't evaluate f()" << " error is " << err_description);
      result = *((real*)user_data);
    }
    *size = result;
    PyGILState_Release(gstate);
  }
  else {
    *size = *((real*)user_data);
  }
 return STATUS_OK;
}

/*
 * The following function will be called for PreCAD/BLSurf message
 * printing.  See context_set_message_callback (later in this
 * template) for how to set user_data.
 */
status_t message_cb(message_t *msg, void *user_data)
{
  integer errnumber = 0;
  char *desc;
  message_get_number(msg, &errnumber);
  message_get_description(msg, &desc);
  string err( desc );
  if ( errnumber < 0 || err.find("license") != string::npos ) {
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

/* This is the interrupt callback. PreCAD/BLSurf will call this
 * function regularily. See the file meshgems/interrupt.h
 */
status_t interrupt_cb(integer *interrupt_status, void *user_data)
{
  integer you_want_to_continue = 1;
#ifdef WITH_SMESH_CANCEL_COMPUTE
  BLSURFPlugin_BLSURF* tmp = (BLSURFPlugin_BLSURF*)user_data;
  you_want_to_continue = !tmp->computeCanceled();
#endif

  if(you_want_to_continue)
  {
    *interrupt_status = INTERRUPT_CONTINUE;
    return STATUS_OK;
  }
  else /* you want to stop BLSurf */
  {
    *interrupt_status = INTERRUPT_STOP;
    return STATUS_ERROR;
  }
}

//=============================================================================
/*!
 *
 */
//=============================================================================
bool BLSURFPlugin_BLSURF::Evaluate(SMESH_Mesh&         aMesh,
                                   const TopoDS_Shape& aShape,
                                   MapShapeNbElems&    aResMap)
{
  double diagonal       = aMesh.GetShapeDiagonalSize();
  double bbSegmentation = _gen->GetBoundaryBoxSegmentation();
  int    _physicalMesh  = BLSURFPlugin_Hypothesis::GetDefaultPhysicalMesh();
  double _phySize       = BLSURFPlugin_Hypothesis::GetDefaultPhySize(diagonal, bbSegmentation);
  bool   _phySizeRel    = BLSURFPlugin_Hypothesis::GetDefaultPhySizeRel();
  //int    _geometricMesh = BLSURFPlugin_Hypothesis::GetDefaultGeometricMesh();
  double _angleMesh     = BLSURFPlugin_Hypothesis::GetDefaultAngleMesh();
  bool   _quadAllowed   = BLSURFPlugin_Hypothesis::GetDefaultQuadAllowed();
  if(_hypothesis) {
    _physicalMesh  = (int) _hypothesis->GetPhysicalMesh();
    _phySizeRel         = _hypothesis->IsPhySizeRel();
    if ( _hypothesis->GetPhySize() > 0)
      _phySize          = _phySizeRel ? diagonal*_hypothesis->GetPhySize() : _hypothesis->GetPhySize();
    //_geometricMesh = (int) hyp->GetGeometricMesh();
    if (_hypothesis->GetAngleMesh() > 0)
      _angleMesh        = _hypothesis->GetAngleMesh();
    _quadAllowed        = _hypothesis->GetQuadAllowed();
  } else {
    //0020968: EDF1545 SMESH: Problem in the creation of a mesh group on geometry
    // GetDefaultPhySize() sometimes leads to computation failure
    _phySize = aMesh.GetShapeDiagonalSize() / _gen->GetBoundaryBoxSegmentation();
    MESSAGE("BLSURFPlugin_BLSURF::SetParameters using defaults");
  }

  bool IsQuadratic = _quadraticMesh;

  // ----------------
  // evaluate 1D
  // ----------------
  TopTools_DataMapOfShapeInteger EdgesMap;
  double fullLen = 0.0;
  double fullNbSeg = 0;
  for (TopExp_Explorer exp(aShape, TopAbs_EDGE); exp.More(); exp.Next()) {
    TopoDS_Edge E = TopoDS::Edge( exp.Current() );
    if( EdgesMap.IsBound(E) )
      continue;
    SMESH_subMesh *sm = aMesh.GetSubMesh(E);
    double aLen = SMESH_Algo::EdgeLength(E);
    fullLen += aLen;
    int nb1d = 0;
    if(_physicalMesh==1) {
       nb1d = (int)( aLen/_phySize + 1 );
    }
    else {
      // use geometry
      double f,l;
      Handle(Geom_Curve) C = BRep_Tool::Curve(E,f,l);
      double fullAng = 0.0;
      double dp = (l-f)/200;
      gp_Pnt P1,P2,P3;
      C->D0(f,P1);
      C->D0(f+dp,P2);
      gp_Vec V1(P1,P2);
      for(int j=2; j<=200; j++) {
        C->D0(f+dp*j,P3);
        gp_Vec V2(P2,P3);
        fullAng += fabs(V1.Angle(V2));
        V1 = V2;
        P2 = P3;
      }
      nb1d = (int)( fullAng/_angleMesh + 1 );
    }
    fullNbSeg += nb1d;
    std::vector<int> aVec(SMDSEntity_Last);
    for(int i=SMDSEntity_Node; i<SMDSEntity_Last; i++) aVec[i]=0;
    if( IsQuadratic > 0 ) {
      aVec[SMDSEntity_Node] = 2*nb1d - 1;
      aVec[SMDSEntity_Quad_Edge] = nb1d;
    }
    else {
      aVec[SMDSEntity_Node] = nb1d - 1;
      aVec[SMDSEntity_Edge] = nb1d;
    }
    aResMap.insert(std::make_pair(sm,aVec));
    EdgesMap.Bind(E,nb1d);
  }
  double ELen = fullLen/fullNbSeg;
  // ----------------
  // evaluate 2D
  // ----------------
  // try to evaluate as in MEFISTO
  for (TopExp_Explorer exp(aShape, TopAbs_FACE); exp.More(); exp.Next()) {
    TopoDS_Face F = TopoDS::Face( exp.Current() );
    SMESH_subMesh *sm = aMesh.GetSubMesh(F);
    GProp_GProps G;
    BRepGProp::SurfaceProperties(F,G);
    double anArea = G.Mass();
    int nb1d = 0;
    std::vector<int> nb1dVec;
    for (TopExp_Explorer exp1(F,TopAbs_EDGE); exp1.More(); exp1.Next()) {
      int nbSeg = EdgesMap.Find(exp1.Current());
      nb1d += nbSeg;
      nb1dVec.push_back( nbSeg );
    }
    int nbQuad = 0;
    int nbTria = (int) ( anArea/( ELen*ELen*sqrt(3.) / 4 ) );
    int nbNodes = (int) ( ( nbTria*3 - (nb1d-1)*2 ) / 6 + 1 );
    if ( _quadAllowed )
    {
      if ( nb1dVec.size() == 4 ) // quadrangle geom face
      {
        int n1 = nb1dVec[0], n2 = nb1dVec[ nb1dVec[1] == nb1dVec[0] ? 2 : 1 ];
        nbQuad = n1 * n2;
        nbNodes = (n1 + 1) * (n2 + 1);
        nbTria = 0;
      }
      else
      {
        nbTria = nbQuad = nbTria / 3 + 1;
      }
    }
    std::vector<int> aVec(SMDSEntity_Last,0);
    if( IsQuadratic ) {
      int nb1d_in = (nbTria*3 - nb1d) / 2;
      aVec[SMDSEntity_Node] = nbNodes + nb1d_in;
      aVec[SMDSEntity_Quad_Triangle] = nbTria;
      aVec[SMDSEntity_Quad_Quadrangle] = nbQuad;
    }
    else {
      aVec[SMDSEntity_Node] = nbNodes;
      aVec[SMDSEntity_Triangle] = nbTria;
      aVec[SMDSEntity_Quadrangle] = nbQuad;
    }
    aResMap.insert(std::make_pair(sm,aVec));
  }

  // ----------------
  // evaluate 3D
  // ----------------
  GProp_GProps G;
  BRepGProp::VolumeProperties(aShape,G);
  double aVolume = G.Mass();
  double tetrVol = 0.1179*ELen*ELen*ELen;
  int nbVols  = int(aVolume/tetrVol);
  int nb1d_in = int(( nbVols*6 - fullNbSeg ) / 6 );
  std::vector<int> aVec(SMDSEntity_Last);
  for(int i=SMDSEntity_Node; i<SMDSEntity_Last; i++) aVec[i]=0;
  if( IsQuadratic ) {
    aVec[SMDSEntity_Node] = nb1d_in/3 + 1 + nb1d_in;
    aVec[SMDSEntity_Quad_Tetra] = nbVols;
  }
  else {
    aVec[SMDSEntity_Node] = nb1d_in/3 + 1;
    aVec[SMDSEntity_Tetra] = nbVols;
  }
  SMESH_subMesh *sm = aMesh.GetSubMesh(aShape);
  aResMap.insert(std::make_pair(sm,aVec));

  return true;
}
