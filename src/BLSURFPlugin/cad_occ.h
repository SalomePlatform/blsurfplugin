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
#ifndef __BL_SURF__
#define __BL_SURF__

#include <list>
#include <vector>
#include <string>

#include <Handle_Geom_Surface.hxx>
#include <Handle_Geom2d_Curve.hxx>
#include <Handle_Geom_Curve.hxx>
#include <Handle_AIS_InteractiveObject.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

#include <blshare.h>

class BL_SURFACE;
class BL_CURVE_SIZER;
class BL_SURFACE_SIZER;
class BL_POINT_SIZER;

class BL_EDGE 
{
  public :
    Handle(Geom2d_Curve) parametric_geometry;
    Handle(Geom_Curve) space_geometry;
    TopoDS_Edge topology;
    BL_SURFACE *boss;
    BL_CURVE_SIZER *sizer;
    bool splitted_closed;
    int splitted_part;
    vector<double> P1,P2,MP;
    string name;
    int i_P1,i_P2;
    int typc,refc;
    
    double parametric_min,parametric_max;
    double space_min,space_max;

    BL_EDGE(TopoDS_Edge&,BL_SURFACE*,Handle(Geom2d_Curve),double,double);
    ~BL_EDGE();

    bool verification();

    void method_curv0(const double&, double*);
    void method_curv1(const double&, double*);
    void method_curv2(const double&, double*);
};

class BL_POINT {
  public :
    BL_POINT_SIZER *sizer;
    BL_SURFACE *boss;
    double u,v;

    BL_POINT() { u=v=0.; boss=NULL; sizer=NULL; }
    BL_POINT(BL_SURFACE *b, double du, double dv) { u=du; v=dv; boss=b; sizer=NULL; }
    BL_POINT(double du, double dv) { u=du; v=dv; boss=NULL; sizer=NULL; }
    ~BL_POINT() { }

    BL_POINT& operator=(BL_POINT p) { u=p.u; v=p.v; return(*this); }
    bool operator==(BL_POINT p) { return((u==p.u)&&(v==p.v)); }
};

class BL_SURFACE {
  public :
    Handle(Geom_Surface) geometry;
    TopoDS_Face topology;
    bool to_delete;

    BL_SURFACE_SIZER *sizer;
    vector<BL_EDGE*> edges;
    vector<BL_POINT*> points;

    BL_SURFACE(TopoDS_Face&);
    ~BL_SURFACE();
 
    bool build();
    bool verification();

    void method_surf0(double const*, double*);
    void method_surf1(double const*, double*, double*);
    void method_surf2(double const*, double*, double*, double*);

    bool add(TopoDS_Edge&);
    void add(double u, double v);
};

class BL_SURF 
{
  protected:

    // void build_blsurf_data_file();
    void init_tsmo(blw_ *blw);
    int  bls_main(blw_ *blw);

    // void set_reference_points();
    void build_surfaces(TopoDS_Shape&);
    // void build_from_occ_references(TopoDS_Shape&);

    void analyse_identical_faces();

  public:
    static BL_SURF *active_blsurf;
    TopoDS_Shape the_object;

    vector<BL_EDGE*> all_edges;
    vector<BL_SURFACE*> surfaces;

    vector<BL_POINT> internal_points;
    vector<TopoDS_Face> internal_face_points;

    vector<TopoDS_Edge> internal_edges;
    vector<TopoDS_Face> internal_faces;

    vector<TopoDS_Face> conforming_source_face;
    vector<TopoDS_Face> conforming_dest_face;

    vector< vector<TopoDS_Face>* > faces_to_merge;
    vector< vector<int>* > vertices_tags;
    vector< vector<int>* > edges_tags;

    // string blsurf_fname;
    // bool mesh_2D, auto_merge, use_occ_refs;

    // enum WHAT_MESH { SURFACE_MESH=0, VOLUME_MESH=1 };
    // enum MESH_ORDER { LINEAR=0, QUADRATIC=1 };

    blw_ blw;
    // int auto_connectivity;
    // int Verbosity;
    // int hphy_flag;
    // int hinterpol_flag,hmean_flag;
    // int eps_glue_flag,eps_ends_flag,eps_collapse_flag;
    // double eps_glue,eps_ends,eps_collapse,shockmax;
    // double angle_mesh,angle_smo;
    // WHAT_MESH WhatMesh;
    // MESH_ORDER MeshOrder;
    // int ComponentToMesh;
    
    // double Pmin,Pmax;
    // double Gmin,Gmax;
    // vector<int> Poptions;  // Goptions

    // vector<TopoDS_Shape> remaining_faces;

    BL_SURF() {
      // Verbosity=25;
      // hphy_flag=0; 
      // hinterpol_flag=0;
      // hmean_flag=0;
      // auto_connectivity=1;   // PL valait 0
      // use_occ_refs=false;
      // Poptions.resize(1); Poptions[0]=0; 
      // WhatMesh=SURFACE_MESH;
      // MeshOrder=LINEAR;
      // mesh_2D=false;
      // eps_glue=eps_ends=eps_collapse=1e-8;
      // eps_glue_flag=eps_ends_flag=eps_collapse_flag=0;
      // shockmax=1.e100;
      // blsurf_fname="";
      // auto_merge=false;
      // ComponentToMesh=0;
      // angle_mesh=8.; angle_smo=1.;
    }
   ~BL_SURF() { }

    bool init(const TopoDS_Shape&);
    // int vertices(double*&);
    void get_blw(blw_*&);
    // int number_of_patches();
    // bool all_triangles();
    // void face_mesh(int is, int*& F, int*& FRef, int*& iglops, int& nF, int& ndom, int& surforient);
    bool end();

    bool run(TopoDS_Shape&);
    bool run();
    void reset();
};

#endif

// PL Gmin, Gmax, hgeo_flag, Goptions ont deja ete elimines
// PL hphy_flag, Poptions, ... sont a eliminer aussi
// bls.Gmin=0.01;
// bls.Gmax=1000;
// bls.hgeo_flag=1;
// bls.Goptions[0]=1;
