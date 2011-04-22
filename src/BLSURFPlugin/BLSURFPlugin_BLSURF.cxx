//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
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
#include "BLSURFPlugin_Attractor.hxx"

extern "C"{
#include <distene/api.h>
#include <distene/blsurf.h>
}

#include <structmember.h>


#include <Basics_Utils.hxx>

#include <SMESH_Gen.hxx>
#include <SMESH_Mesh.hxx>
#include <SMESH_ControlsDef.hxx>
#include <SMDSAbs_ElementType.hxx>
#include "SMESHDS_Group.hxx"
#include "SMESH_Group.hxx"

#include <utilities.h>

#include <limits>
#include <list>
#include <vector>
#include <set>
#include <cstdlib>

// OPENCASCADE includes
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <NCollection_Map.hxx>

#include <Geom_Surface.hxx>
#include <Handle_Geom_Surface.hxx>
#include <Geom2d_Curve.hxx>
#include <Handle_Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Handle_Geom_Curve.hxx>
#include <Handle_AIS_InteractiveObject.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepTools.hxx>

#include <TopTools_DataMapOfShapeInteger.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>

#ifndef WNT
#include <fenv.h>
#endif

#include <Standard_ErrorHandler.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
// #include <BRepClass_FaceClassifier.hxx>
#include <TopTools_MapOfShape.hxx>

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
  _compatibleHypothesis.push_back("BLSURF_Parameters");
  _requireDescretBoundary = false;
  _onlyUnaryInput = false;
  _hypothesis = NULL;

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
  EnfVertexCoords2EnfVertexList[s_coords].insert(enfVertex);

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
void createAttractorOnFace(TopoDS_Shape GeomShape, std::string AttractorFunction)
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
  attractorFunctionStream << _smp_phy_size << "-(" << _smp_phy_size <<"-" << a << ")";
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
//   myAttractor.SetParameters(a, _smp_phy_size, b, d);
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

void BLSURFPlugin_BLSURF::SetParameters(const BLSURFPlugin_Hypothesis* hyp,
                                        blsurf_session_t *             bls,
                                        SMESH_Mesh&                   mesh)
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
    //0020968: EDF1545 SMESH: Problem in the creation of a mesh group on geometry
    // GetDefaultPhySize() sometimes leads to computation failure
    _phySize = mesh.GetShapeDiagonalSize() / _gen->GetBoundaryBoxSegmentation();
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
        MESSAGE("blsurf_set_sizeMap(): " << smIt->first << " = " << smIt->second);
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
    const BLSURFPlugin_Hypothesis::TSizeMap attractors = BLSURFPlugin_Hypothesis::GetAttractorEntries(hyp);
    BLSURFPlugin_Hypothesis::TSizeMap::const_iterator atIt = attractors.begin();
    for ( ; atIt != attractors.end(); ++atIt ) {
      if ( !atIt->second.empty() ) {
        MESSAGE("blsurf_set_attractor(): " << atIt->first << " = " << atIt->second);
        GeomShape = entryToShape(atIt->first);
        GeomType  = GeomShape.ShapeType();
        // Group Management
        if (GeomType == TopAbs_COMPOUND){
          for (TopoDS_Iterator it (GeomShape); it.More(); it.Next()){
            if (it.Value().ShapeType() == TopAbs_FACE){
              HasSizeMapOnFace = true;
              createAttractorOnFace(it.Value(), atIt->second);
            }
          }
        }
                
        if (GeomType == TopAbs_FACE){
          HasSizeMapOnFace = true;
          createAttractorOnFace(GeomShape, atIt->second);
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
       // MESSAGE("blsurf_set_attractor(): " << AtIt->first << " = " << AtIt->second);
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
  blsurf_set_param(bls, "relax_size",        _decimesh ? "0": to_string(_geometricMesh).c_str());
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
status_t message_cb(message_t *msg, void *user_data);
status_t interrupt_cb(integer *interrupt_status, void *user_data);

//=============================================================================
/*!
 *
 */
//=============================================================================

bool BLSURFPlugin_BLSURF::Compute(SMESH_Mesh& aMesh, const TopoDS_Shape& aShape) {

  MESSAGE("BLSURFPlugin_BLSURF::Compute");

//   if (aShape.ShapeType() == TopAbs_COMPOUND) {
//     MESSAGE("  the shape is a COMPOUND");
//   }
//   else {
//     MESSAGE("  the shape is UNKNOWN");
//   };

  // Fix problem with locales
  Kernel_Utils::Localizer aLocalizer;

  /* create a distene context (generic object) */
  status_t status = STATUS_ERROR;
  
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
  cad_t *c = cad_new(ctx);

  blsurf_session_t *bls = blsurf_session_new(ctx);

  FacesWithSizeMap.Clear();
  FaceId2SizeMap.clear();
  FaceId2ClassAttractor.clear();
  FaceIndex2ClassAttractor.clear();
  EdgesWithSizeMap.Clear();
  EdgeId2SizeMap.clear();
  VerticesWithSizeMap.Clear();
  VertexId2SizeMap.clear();

  MESSAGE("BEGIN SetParameters");
  SetParameters(_hypothesis, bls, aMesh);
  MESSAGE("END SetParameters");

  /* Now fill the CAD object with data from your CAD
   * environement. This is the most complex part of a successfull
   * integration.
   */

  // needed to prevent the opencascade memory managmement from freeing things
  vector<Handle(Geom2d_Curve)> curves;
  vector<Handle(Geom_Surface)> surfaces;

  surfaces.resize(0);
  curves.resize(0);
  
  TopTools_IndexedMapOfShape fmap;
  TopTools_IndexedMapOfShape emap;
  TopTools_IndexedMapOfShape pmap;
  
  fmap.Clear();
  FaceId2PythonSmp.clear();
  emap.Clear();
  EdgeId2PythonSmp.clear();
  pmap.Clear();
  VertexId2PythonSmp.clear();

  assert(Py_IsInitialized());
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();

  string theSizeMapStr;
  
  /****************************************************************************************
                                  FACES
  *****************************************************************************************/
  int iface = 0;
  string bad_end = "return";
  int faceKey = -1;
  int ienf = 0;
  BLSURFPlugin_Attractor myAttractor;
  for (TopExp_Explorer face_iter(aShape,TopAbs_FACE);face_iter.More();face_iter.Next()) {
    TopoDS_Face f=TopoDS::Face(face_iter.Current());

    // make INTERNAL face oriented FORWARD (issue 0020993)
    if (f.Orientation() != TopAbs_FORWARD && f.Orientation() != TopAbs_REVERSED )
      f.Orientation(TopAbs_FORWARD);
    
    if (fmap.FindIndex(f) > 0)
      continue;

    fmap.Add(f);
    iface++;
    surfaces.push_back(BRep_Tool::Surface(f));
    
    /* create an object representing the face for blsurf */
    /* where face_id is an integer identifying the face.
     * surf_function is the function that defines the surface
     * (For this face, it will be called by blsurf with your_face_object_ptr
     * as last parameter.
     */
    cad_face_t *fce = cad_face_new(c, iface, surf_fun, surfaces.back());
    
    /* by default a face has no tag (color). The following call sets it to the same value as the face_id : */
    cad_face_set_tag(fce, iface);
      
    /* Set face orientation (optional if you want a well oriented output mesh)*/
    if(f.Orientation() != TopAbs_FORWARD){
      cad_face_set_orientation(fce, CAD_ORIENTATION_REVERSED);
    } else {
      cad_face_set_orientation(fce, CAD_ORIENTATION_FORWARD);
    }
    
    if (HasSizeMapOnFace){
//       MESSAGE("A size map is defined on a face")
//       std::cout << "A size map is defined on a face" << std::endl;
      // Classic size map
      faceKey = FacesWithSizeMap.FindIndex(f);
      
      
      if (FaceId2SizeMap.find(faceKey)!=FaceId2SizeMap.end()){
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
      int iatt=0;
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
          // scl.Perform() is bugged. The function was rewritten
//          scl.Perform();
          BRepClass_FaceClassifierPerform(&scl,f,P,1e-7);
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
            double uvCoords[2]   = {attractor_iter->second[0],attractor_iter->second[1]};
            iatt++;
            MESSAGE("Add cad point on (u,v)=(" << uvCoords[0] << "," << uvCoords[1] << ") with id = " << iatt);
            cad_point_t* point_p = cad_point_new(fce, iatt, uvCoords);
            cad_point_set_tag(point_p, iatt);
          }
          FaceId2AttractorCoords.erase(faceKey);
        }
      }
      
      // Class Attractors
      std::map<int,BLSURFPlugin_Attractor* >::iterator clAttractor_iter = FaceId2ClassAttractor.find(faceKey);
      if (clAttractor_iter != FaceId2ClassAttractor.end()){
          MESSAGE("Face indice: " << iface);
          MESSAGE("Adding attractor");
          FaceIndex2ClassAttractor[iface]=clAttractor_iter->second;
          FaceId2ClassAttractor.erase(clAttractor_iter);
        }
      }     
      
      // Enforced Vertices
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
          // scl.Perform() is bugged. The function was rewritten
//          scl.Perform();
          BRepClass_FaceClassifierPerform(&scl,f,P,1e-7);
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
            cad_point_set_tag(point_p, ienf);
          }
        }
        FaceId2EnforcedVertexCoords.erase(faceKey);
      }
//       else
//         std::cout << "No enforced vertex defined" << std::endl;
//     }
    
    
    /****************************************************************************************
                                    EDGES
                   now create the edges associated to this face
    *****************************************************************************************/
    int edgeKey = -1;
    for (TopExp_Explorer edge_iter(f,TopAbs_EDGE);edge_iter.More();edge_iter.Next()) {
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
      
      /* attach the edge to the current blsurf face */
      cad_edge_t *edg = cad_edge_new(fce, ic, tmin, tmax, curv_fun, curves.back());
      
      /* by default an edge has no tag (color). The following call sets it to the same value as the edge_id : */
      cad_edge_set_tag(edg, ic);
      
      /* by default, an edge does not necessalry appear in the resulting mesh,
     unless the following property is set :
      */
      cad_edge_set_property(edg, EDGE_PROPERTY_SOFT_REQUIRED);
      
      /* by default an edge is a boundary edge */
      if (e.Orientation() == TopAbs_INTERNAL)
        cad_edge_set_property(edg, EDGE_PROPERTY_INTERNAL);

      int npts = 0;
      int ip1, ip2, *ip;
      gp_Pnt2d e0 = curves.back()->Value(tmin);
      gp_Pnt ee0 = surfaces.back()->Value(e0.X(), e0.Y());
      Standard_Real d1=0,d2=0;
      
      
      /****************************************************************************************
                                      VERTICES
      *****************************************************************************************/
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
    // There was an error while meshing
    blsurf_session_delete(bls);
    cad_delete(c);
    context_delete(ctx);

    return error(_comment);
  }

  std::cout << std::endl;
  std::cout << "End of Surface Mesh generation" << std::endl;
  std::cout << std::endl;

  mesh_t *msh = NULL;
  blsurf_data_get_mesh(bls, &msh);
  if(!msh){
    blsurf_session_delete(bls);
    cad_delete(c);
    context_delete(ctx);

    return error(_comment);
    //return false;
  }

  /* retrieve mesh data (see distene/mesh.h) */
  integer nv, ne, nt, nq, vtx[4], tag;
  real xyz[3];

  mesh_get_vertex_count(msh, &nv);
  mesh_get_edge_count(msh, &ne);
  mesh_get_triangle_count(msh, &nt);
  mesh_get_quadrangle_count(msh, &nq);


  SMESHDS_Mesh* meshDS = aMesh.GetMeshDS();
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
    if(_hypothesis) {
      BLSURFPlugin_Hypothesis::TEnfVertexCoords projVertex;
      projVertex.clear();
      projVertex.push_back((double)xyz[0]);
      projVertex.push_back((double)xyz[1]);
      projVertex.push_back((double)xyz[2]);
      std::map< BLSURFPlugin_Hypothesis::TEnfVertexCoords, BLSURFPlugin_Hypothesis::TEnfVertexList >::const_iterator enfCoordsIt = EnfVertexCoords2EnfVertexList.find(projVertex);
      if (enfCoordsIt != EnfVertexCoords2EnfVertexList.end()) {
        MESSAGE("Found enforced vertex @ " << xyz[0] << ", " << xyz[1] << ", " << xyz[2])
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
              throw SALOME_Exception(LOCALIZED("A enforced vertex node was not added to a group"));
          }
          else
            MESSAGE("Group name is empty: '"<<currentEnfVertex->grpName<<"' => group is not created");
        }
      }
    }


    // internal point are tagged to zero
    if(tag > 0 && tag <= pmap.Extent() ){
      meshDS->SetNodeOnVertex(nodes[iv], TopoDS::Vertex(pmap(tag)));
      tags[iv] = false;
    } else {
      tags[iv] = true;
    }
  }

  /* enumerate edges */
  for(int it=1;it<=ne;it++) {
    mesh_get_edge_vertices(msh, it, vtx);
    SMDS_MeshEdge* edg = meshDS->AddEdge(nodes[vtx[0]], nodes[vtx[1]]);
    mesh_get_edge_tag(msh, it, &tag);

    if (tags[vtx[0]]) {
      Set_NodeOnEdge(meshDS, nodes[vtx[0]], emap(tag));
      tags[vtx[0]] = false;
    };
    if (tags[vtx[1]]) {
      Set_NodeOnEdge(meshDS, nodes[vtx[1]], emap(tag));
      tags[vtx[1]] = false;
    };
    meshDS->SetMeshElementOnShape(edg, TopoDS::Edge(emap(tag)));

  }

  /* enumerate triangles */
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

  /* enumerate quadrangles */
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

  // SetIsAlwaysComputed( true ) to sub-meshes of degenerated EDGEs
  TopLoc_Location loc; double f,l;
  for (int i = 1; i <= emap.Extent(); i++)
    if ( BRep_Tool::Curve(TopoDS::Edge( emap( i )), loc, f,l).IsNull() )
      if ( SMESH_subMesh* sm = aMesh.GetSubMeshContaining( emap( i )))
        sm->SetIsAlwaysComputed( true );

  delete [] nodes;

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
  
  return true;
}

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
    double dist2 = pnt.SquareDistance( curve_pnt );
    double tol = BRep_Tool::Tolerance( edge );
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

/* Curve definition function See cad_curv_t in file distene/cad.h for
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
 * See cad_surf_t in file distene/cad.h for more information.
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
  //MESSAGE("size_on_surface")
  if (FaceId2PythonSmp.count(face_id) != 0){
    //MESSAGE("A size map is used to calculate size on face : "<<face_id)
    PyObject * pyresult = NULL;
    PyObject* new_stderr = NULL;
    assert(Py_IsInitialized());
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    pyresult = PyObject_CallFunction(FaceId2PythonSmp[face_id],(char*)"(f,f)",uv[0],uv[1]);
    double result;
    if ( pyresult == NULL){
      fflush(stderr);
      string err_description="";
      new_stderr = newPyStdOut(err_description);
      PySys_SetObject((char*)"stderr", new_stderr);
      PyErr_Print();
      PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
      Py_DECREF(new_stderr);
      MESSAGE("Can't evaluate f(" << uv[0] << "," << uv[1] << ")" << " error is " << err_description);
      result = *((double*)user_data);
      }
    else {
      result = PyFloat_AsDouble(pyresult);
      Py_DECREF(pyresult);
    }
   // MESSAGE("f(" << uv[0] << "," << uv[1] << ")" << " = " << result);
    *size = result;
    PyGILState_Release(gstate);
  }
  else if (FaceIndex2ClassAttractor.count(face_id) !=0 && !FaceIndex2ClassAttractor[face_id]->Empty()){
//    MESSAGE("attractor used on face :"<<face_id)
    // MESSAGE("List of attractor is not empty")
    // MESSAGE("Attractor empty : "<< FaceIndex2ClassAttractor[face_id]->Empty())
    double result = FaceIndex2ClassAttractor[face_id]->GetSize(uv[0],uv[1]);
    *size = result;
   // MESSAGE("f(" << uv[0] << "," << uv[1] << ")" << " = " << result);
  }
  else {
    // MESSAGE("List of attractor is empty !!!")
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
    pyresult = PyObject_CallFunction(EdgeId2PythonSmp[edge_id],(char*)"(f)",t);
    double result;
    if ( pyresult == NULL){
      fflush(stderr);
      string err_description="";
      new_stderr = newPyStdOut(err_description);
      PySys_SetObject((char*)"stderr", new_stderr);
      PyErr_Print();
      PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
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
    pyresult = PyObject_CallFunction(VertexId2PythonSmp[point_id],(char*)"");
    double result;
    if ( pyresult == NULL){
      fflush(stderr);
      string err_description="";
      new_stderr = newPyStdOut(err_description);
      PySys_SetObject((char*)"stderr", new_stderr);
      PyErr_Print();
      PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
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

/* This is the interrupt callback. PreCAD/BLSurf will call this
 * function regularily. See the file distene/interrupt.h
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
  int    _physicalMesh  = BLSURFPlugin_Hypothesis::GetDefaultPhysicalMesh();
  double _phySize       = BLSURFPlugin_Hypothesis::GetDefaultPhySize();
  //int    _geometricMesh = BLSURFPlugin_Hypothesis::GetDefaultGeometricMesh();
  //double _angleMeshS    = BLSURFPlugin_Hypothesis::GetDefaultAngleMeshS();
  double _angleMeshC    = BLSURFPlugin_Hypothesis::GetDefaultAngleMeshC();
  bool   _quadAllowed   = BLSURFPlugin_Hypothesis::GetDefaultQuadAllowed();
  if(_hypothesis) {
    _physicalMesh  = (int) _hypothesis->GetPhysicalMesh();
    _phySize       = _hypothesis->GetPhySize();
    //_geometricMesh = (int) hyp->GetGeometricMesh();
    //_angleMeshS    = hyp->GetAngleMeshS();
    _angleMeshC    = _hypothesis->GetAngleMeshC();
    _quadAllowed   = _hypothesis->GetQuadAllowed();
  }

  bool IsQuadratic = false;

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
      nb1d = (int)( fullAng/_angleMeshC + 1 );
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

//=============================================================================
/*!
 *  Rewritting of the BRepClass_FaceClassifier::Perform function which is bugged (CAS 6.3sp6)
 *  Following line was added:
 *        myExtrem.Perform(P);
 */
//=============================================================================
void  BLSURFPlugin_BLSURF::BRepClass_FaceClassifierPerform(BRepClass_FaceClassifier* fc,
                    const TopoDS_Face& face, 
                    const gp_Pnt& P, 
                    const Standard_Real Tol)
{
  //-- Voir BRepExtrema_ExtPF.cxx 
  BRepAdaptor_Surface Surf(face);
  Standard_Real U1, U2, V1, V2;
  BRepTools::UVBounds(face, U1, U2, V1, V2);
  Extrema_ExtPS myExtrem;
  myExtrem.Initialize(Surf, U1, U2, V1, V2, Tol, Tol);
  myExtrem.Perform(P);
  //----------------------------------------------------------
  //-- On cherche le point le plus proche , PUIS 
  //-- On le classifie. 
  Standard_Integer nbv    = 0; // xpu
  Standard_Real MaxDist   =  RealLast();
  Standard_Integer indice = 0;
  if(myExtrem.IsDone()) {
    nbv = myExtrem.NbExt();
    for (Standard_Integer i = 1; i <= nbv; i++) {
      Standard_Real d = myExtrem.Value(i);
      d = Abs(d);
      if(d <= MaxDist) { 
    MaxDist = d;
    indice = i;
      }
    }
  }
  if(indice) { 
    gp_Pnt2d Puv;
    Standard_Real U1,U2;
    myExtrem.Point(indice).Parameter(U1, U2);
    Puv.SetCoord(U1, U2);
    fc->Perform(face, Puv, Tol);
  }
  else { 
    fc->Perform(face, gp_Pnt2d(U1-1.0,V1 - 1.0), Tol); //-- NYI etc BUG PAS BEAU En attendant l acces a rejected
    //-- le resultat est TopAbs_OUT;
  }
}

