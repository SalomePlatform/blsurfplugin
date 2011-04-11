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
// File    : BLSURFPlugin_Attractor.cxx
// Authors : Renaud Nédélec (OCC)
// ---
// 
// The idea of the algorithm used to calculate the distance on a 
// non-euclidian parametric surface has been found in the ref. below:
//
// Ref:"Accurate Anisotropic Fast Marching for Diffusion-Based Geodesic Tractography"
// S. Jbabdi, P. Bellec, R. Toro, Daunizeau, M. Pélégrini-Issac, and H. Benali1
//

#include "BLSURFPlugin_Attractor.hxx"
#include <utilities.h>
#include <algorithm>
#include <cmath>

// cascade include
#include "ShapeAnalysis.hxx"
#include "ShapeConstruct_ProjectCurveOnSurface.hxx"
#include <Precision.hxx>

BLSURFPlugin_Attractor::BLSURFPlugin_Attractor ()
  : _face(),
  _attractorShape(),
  _attEntry(),
  _step(0),
  _gridU(0),
  _gridV(0),
  _vectU(),
  _vectV(),
  _DMap(),
  _known(),
  _trial(),
  _u1 (0.),
  _u2 (0.),
  _v1 (0.),
  _v2 (0.),
  _startSize(-1),
  _endSize(-1),
  _actionRadius(-1),
  _constantRadius(-1),
  _type(-1),
  _isMapBuilt(false),
  _isEmpty(true){ MESSAGE("construction of a void attractor"); }

BLSURFPlugin_Attractor::BLSURFPlugin_Attractor (const TopoDS_Face& Face, const TopoDS_Shape& Attractor, const std::string& attEntry, double Step) // TODO Step is now unused -> remove it if testing is OK
  : _face(),
  _attractorShape(),
  _attEntry(attEntry),
  _step(),
  _gridU(),
  _gridV(),
  _vectU(),
  _vectV(),
  _DMap(),
  _known(),
  _trial(),
  _u1 (0.),
  _u2 (0.),
  _v1 (0.),
  _v2 (0.),
  _startSize(-1),
  _endSize(-1),
  _actionRadius(-1),
  _constantRadius(-1),
  _type(0),
  _isMapBuilt(false),
  _isEmpty(false)
{
  _face = Face;
  _attractorShape = Attractor;
  
  init();
}

bool BLSURFPlugin_Attractor::init(){ 
  Standard_Real u0,v0;
  int i,j,i0,j0 ;
  _known.clear();
  _trial.clear();
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(_face);
  
  // Calculation of the bounds of the face
  ShapeAnalysis::GetFaceUVBounds(_face,_u1,_u2,_v1,_v2);
  MESSAGE("u1 = "<<_u1<<" ,u2  = "<<_u2);
  MESSAGE("v1 = "<<_v1<<" ,v2  = "<<_v2);
//   _gridU = floor (_u2 - _u1) / _step;
//   _gridV = floor (_v2 - _v1) / _step;
  // TEST
  _gridU = 300;
  _gridV = 300;
  _step = std::min((_u2-_u1)/_gridU,(_v2-_v1)/_gridV);

  for (i=0; i<=_gridU; i++){
    _vectU.push_back(_u1+i*(_u2-_u1)/_gridU) ;
  }
  for (j=0; j<=_gridV; j++){
    _vectV.push_back(_v1+j*(_v2-_v1)/_gridV) ;
  }
  
  // Initialization of _DMap and _known
  std::vector<double> temp(_gridV+1,std::numeric_limits<double>::infinity());  // Set distance of all "far" points to Infinity 
  for (i=0; i<=_gridU; i++){
    _DMap.push_back(temp);
  }
  std::vector<bool> temp2(_gridV+1,false);
  for (i=0; i<=_gridU; i++){
    _known.push_back(temp2);
  }

  // Determination of the starting points
  if (_attractorShape.ShapeType() == TopAbs_VERTEX){ 
    Trial_Pnt TPnt(3,0); 
    gp_Pnt P = BRep_Tool::Pnt(TopoDS::Vertex(_attractorShape));
    GeomAPI_ProjectPointOnSurf projector( P, aSurf );
    projector.LowerDistanceParameters(u0,v0);
    MESSAGE("u0 = "<<u0<<" ,v0  = "<<v0);
    i0 = floor ( (u0 - _u1) * _gridU / (_u2 - _u1) + 0.5 );
    j0 = floor ( (v0 - _v1) * _gridV / (_v2 - _v1) + 0.5 );
    TPnt[0]=0.;                                                                // Set the distance of the starting point to 0.
    TPnt[1]=i0;
    TPnt[2]=j0;
    _DMap[i0][j0] = 0.;
    _trial.insert(TPnt);         // Move starting point to _trial
  }
  else if (_attractorShape.ShapeType() == TopAbs_EDGE){
    const TopoDS_Edge& anEdge = TopoDS::Edge(_attractorShape);
    edgeInit(aSurf, anEdge);
  }
  else if (_attractorShape.ShapeType() == TopAbs_WIRE){
    TopExp_Explorer anExp(_attractorShape, TopAbs_EDGE);
    for(; anExp.More(); anExp.Next()){
      const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());
      edgeInit(aSurf, anEdge);
    }
  }
    
}

void BLSURFPlugin_Attractor::edgeInit(Handle(Geom_Surface) theSurf, const TopoDS_Edge& anEdge){
  gp_Pnt2d P2;
  double first;
  double last;
  int i,j,i0,j0;
  Trial_Pnt TPnt(3,0);
  Handle(Geom2d_Curve) aCurve2d; 
  Handle(Geom_Curve) aCurve3d = BRep_Tool::Curve (anEdge, first, last);
  ShapeConstruct_ProjectCurveOnSurface curveProjector;
  curveProjector.Init(theSurf, Precision::Confusion());
  curveProjector.PerformAdvanced (aCurve3d, first, last, aCurve2d);
  //int N = 20 * (last - first) / _step;  // If the edge is a circle : 4>Pi so the number of points on the edge should be good -> 5 for ellipses
  int N = 1200;
  MESSAGE("Initialisation des points de départ")
  for (i=0; i<=N; i++){
    P2 = aCurve2d->Value(first + i * (last-first) / N);
    i0 = floor( (P2.X() - _u1) * _gridU / (_u2 - _u1) + 0.5 );
    j0 = floor( (P2.Y() - _v1) * _gridV / (_v2 - _v1) + 0.5 );
//     MESSAGE("i0 = "<<i0<<" , j0 = "<<j0)
    TPnt[0] = 0.;
    TPnt[1] = i0;
    TPnt[2] = j0;
    _DMap[i0][j0] = 0.;
    _trial.insert(TPnt);
  }
  MESSAGE("_trial.size() = "<<_trial.size())
}  


void BLSURFPlugin_Attractor::SetParameters(double Start_Size, double End_Size, double Action_Radius, double Constant_Radius){
  MESSAGE("BLSURFPlugin_Attractor::SetParameters")
  _startSize = Start_Size;
  _endSize = End_Size;
  _actionRadius = Action_Radius;
  _constantRadius = Constant_Radius;
}

double BLSURFPlugin_Attractor::_distance(double u, double v){
  
//   // calcul des coins du carre
//   double stepU = (_u2 - _u1) / _gridU ;
//   double stepV = (_v2 - _v1) / _gridV ;
//   
//   int i_sup =  floor ( fabs(u - _u1) / stepU ) + 1;
//   int j_sup =  floor ( fabs(v - _v1) / stepV ) + 1;
//   i_sup = std::min( i_sup, _gridU);
//   j_sup = std::min( j_sup, _gridV);
//   
//   
// //   int i_inf = floor ( (u - _u1) * _mapGrid / (_u2 - _u1) );
// //   int j_inf = floor ( (v - _v1) * _mapGrid / (_v2 - _v1) );
//   int i_inf = i_sup - 1;
//   int j_inf = j_sup - 1;
//   
//   double u_sup = _vectU[i_sup];
//   double v_sup = _vectV[j_sup];
//   
//   double u_inf = _vectU[i_inf];
//   double v_inf = _vectV[j_inf];
// //   
// // //   MESSAGE("i_inf , i_sup, j_inf, j_sup = "<<i_inf<<" , "<<i_sup<<" , "<<j_inf<<" , "<<j_sup)
// // //   MESSAGE("u = "<<u<<", _u1 ="<<_u1)
// // //   MESSAGE("(u - _u1) / stepU = "<< (u - _u1) / stepU)
//   double d1 = _DMap[i_sup][j_sup]; 
//   double d2 = _DMap[i_sup][j_inf];
//   double d3 = _DMap[i_inf][j_sup];
//   double d4 = _DMap[i_inf][j_inf];
// //   
// //   double d = 0.25 * (d1 + d2 + d3 + d4);
// //   //double d = d1;
// //  //
// // //   if (fabs(v_inf-v_sup) < 1e-14 || fabs(u_inf-u_sup) < 1e-14 ){
// // //     MESSAGE("division par zero v_inf-v_sup = "<< fabs(v_inf-v_sup)<<" , u_inf-u_sup"<<fabs(u_inf-u_sup))
// // //     MESSAGE("v_inf = "<< v_inf<<" , v_sup"<<v_sup)
// // //     MESSAGE("u_inf = "<< u_inf<<" , u_sup"<<u_sup)
// // //   }
//   double P_inf =   d4 * ( 1 + (u_inf - u) / stepU + (v_inf - v) / stepV )
//                  + d3 * ( (v - v_inf) / stepV )
//                  + d2 * ( (u - u_inf) / stepU ) ;
// 		 
//   double P_sup =   d1 * ( 1 + (u - u_sup) / stepU + (v - v_sup) / stepV )
//                  + d3 * ( (u_sup - u) / stepU )
//                  + d2 * ( (v_sup - v) / stepV ) ;
//   
//   // calcul de la distance (interpolation lineaire)
//   bool P_switch = (u+v > u_sup+v_sup);
//   double d;
//   if (P_switch){
//     d = P_inf;
//   }
//   else {
//     d = P_sup;
//   }
//   
//   return d; 
  
  //   BLSURF seems to perform a linear interpolation so it's sufficient to give it a non-continuous distance map
  int i = floor ( (u - _u1) * _gridU / (_u2 - _u1) + 0.5 );
  int j = floor ( (v - _v1) * _gridV / (_v2 - _v1) + 0.5 );
  
  return _DMap[i][j];
}


double BLSURFPlugin_Attractor::GetSize(double u, double v){   
  double myDist = 0.5 * (_distance(u,v) - _constantRadius + fabs(_distance(u,v) - _constantRadius));
  if (myDist<0){
    MESSAGE("Warning myDist<0 : myDist= "<<myDist)
  }
  switch(_type)
  {
    case TYPE_EXP:
      if (fabs(_actionRadius) < 1e-12){ // TODO definir eps et decommenter
	if (myDist < 1e-12){
	  return _startSize;
	}
	else {
	  return _endSize;
	}
      }
      else{
	return _endSize - (_endSize - _startSize) * exp(- myDist * myDist / (_actionRadius * _actionRadius) );
      }
      break;
    case TYPE_LIN:
      return _startSize + ( 0.5 * (_distance(u,v) - _constantRadius + abs(_distance(u,v) - _constantRadius)) ) ;
      break;
  }
}


void BLSURFPlugin_Attractor::BuildMap(){ 
  
  MESSAGE("building the map");
  int i, j, k, n;  
  int count = 0;
  int ip, jp, kp, np;
  int i0, j0;
  gp_Pnt P;
  gp_Vec D1U,D1V;
  double Guu, Gvv, Guv;         // Components of the local metric tensor
  double du, dv;
  double D_Ref = 0.;
  double Dist = 0.;
  bool Dist_changed;
  IJ_Pnt Current_Pnt(2,0);
  Trial_Pnt TPnt(3,0);
  TTrialSet::iterator min;
  TTrialSet::iterator found;
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(_face);
  
  // While there are points in "Trial" (representing a kind of advancing front), loop on them -----------------------------------------------------------
  while (_trial.size() > 0 ){
    min = _trial.begin();                        // Get trial point with min distance from start
    i0 = (*min)[1];
    j0 = (*min)[2];
    _known[i0][j0] = true;                       // Move it to "Known"
//     MESSAGE("_DMap["<<i0<<"]["<<j0<<"] = "<<_DMap[i0][j0])
    _trial.erase(min);                           // Remove it from "Trial"
    // Loop on neighbours of the trial min --------------------------------------------------------------------------------------------------------------
    for (i=i0 - 1 ; i <= i0 + 1 ; i++){ 
      if (!aSurf->IsUPeriodic()){                          // Periodic conditions in U	
        if (i > _gridU ){
          break; }
        else if (i < 0){
          i++; }
      }
      ip = (i + _gridU + 1) % (_gridU+1);                  // We get a periodic index :
      for (j=j0 - 1 ; j <= j0 + 1 ; j++){                  //    ip=modulo(i,N+2) so that  i=-1->ip=N; i=0 -> ip=0 ; ... ; i=N+1 -> ip=0;  
        if (!aSurf->IsVPeriodic()){                        // Periodic conditions in V . 
          if (j > _gridV ){
            break; }
          else if (j < 0){
            j++;
          }
        }
        jp = (j + _gridV + 1) % (_gridV+1);
      
        if (!_known[ip][jp]){                              // If the distance is not known yet
          aSurf->D1(_vectU[ip],_vectV[jp],P,D1U,D1V);      // Calculate the metric at (i,j)
          // G(i,j)  =  | ||dS/du||**2          *     | 
          //            | <dS/du,dS/dv>  ||dS/dv||**2 |
          Guu = D1U.X()*D1U.X() +  D1U.Y()*D1U.Y() + D1U.Z()*D1U.Z();    // Guu = ||dS/du||**2    
          Gvv = D1V.X()*D1V.X() +  D1V.Y()*D1V.Y() + D1V.Z()*D1V.Z();    // Gvv = ||dS/dv||**2           
          Guv = D1U.X()*D1V.X() +  D1U.Y()*D1V.Y() + D1U.Z()*D1V.Z();    // Guv = Gvu = < dS/du,dS/dv > 
          D_Ref = _DMap[ip][jp];                           // Set a ref. distance of the point to its value in _DMap 
          TPnt[0] = D_Ref;                                 // (may be infinite or uncertain)
          TPnt[1] = ip;
          TPnt[2] = jp;
          Dist_changed = false;
          // Loop on neighbours to calculate the min distance from them ---------------------------------------------------------------------------------
          for (k=i - 1 ; k <= i + 1 ; k++){
            if (!aSurf->IsUPeriodic()){                              // Periodic conditions in U  
              if(k > _gridU ){
                break;
              }
              else if (k < 0){
                k++; }
            }
            kp = (k + _gridU + 1) % (_gridU+1);                      // periodic index
            for (n=j - 1 ; n <= j + 1 ; n++){ 
              if (!aSurf->IsVPeriodic()){                            // Periodic conditions in V 
                if(n > _gridV){	  
                  break;
                }
                else if (n < 0){
                  n++; }
              }
              np = (n + _gridV + 1) % (_gridV+1);                    
              if (_known[kp][np]){                                   // If the distance of the neighbour is known
                                                                     // Calculate the distance from (k,n)
                du = (k-i) * (_u2 - _u1) / _gridU;
                dv = (n-j) * (_v2 - _v1) / _gridV;
                Dist = _DMap[kp][np] + sqrt( Guu * du*du + 2*Guv * du*dv + Gvv * dv*dv );   // ds**2 = du'Gdu + 2*du'Gdv + dv'Gdv  (G is always symetrical)
                if (Dist < D_Ref) {                                  // If smaller than ref. distance  ->  update ref. distance
                  D_Ref = Dist;
                  Dist_changed = true;
                }
              }
            }
          } // End of the loop on neighbours --------------------------------------------------------------------------------------------------------------
          if (Dist_changed) {                              // If distance has been updated, update _trial 
            found=_trial.find(TPnt);
            if (found != _trial.end()){
              _trial.erase(found);                         // Erase the point if it was already in _trial
            }
            TPnt[0] = D_Ref;
            TPnt[1] = ip;
            TPnt[2] = jp;
            _DMap[ip][jp] = D_Ref;                         // Set it distance to the minimum distance found during the loop above
            _trial.insert(TPnt);                           // Insert it (or reinsert it) in _trial
          }
        } // end if (!_known[ip][jp])
      } // for
    } // for
  } // while (_trial)
  _known.clear();
  _trial.clear();
  _isMapBuilt = true;
  MESSAGE("_gridU = "<<_gridU<<" , _gridV = "<<_gridV)  
} // end of BuildMap()



