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
// File    : GeomSelectionTools.cxx
// Authors : Nicolas GEIMER (OCC)
// ---
//


#include "GeomSelectionTools.h"
#include <sstream>

#include <LightApp_SelectionMgr.h> 
#include <SalomeApp_Application.h> 
#include <SUIT_Session.h> 

#include <SALOME_ListIteratorOfListIO.hxx> 
#include <GEOM_Client.hxx>
#include <SMESH_Gen_i.hxx> 
#include <SMESHGUI_Utils.h>
#include <boost/shared_ptr.hpp> 

#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <Handle_Geom_Surface.hxx>
#include <BRepAdaptor_Surface.hxx>


/*!
 * Constructor 
 * @param aStudy pointer to the Study
 *
 */
GeomSelectionTools::GeomSelectionTools(_PTR(Study) aStudy)
{
  myStudy = aStudy;
}

/*!
 * Accessor to the Study used by this GeomSelectionTools object
 * @return The study used by the GeomSelectionTools class
 */
_PTR(Study) GeomSelectionTools::getMyStudy()
{
    return myStudy;
}

/*!
 * Allows to get the selection manager from LightApp 
 * @return A LightApp_SelectionMgr Pointer or 0 if it can't get it.
 */
LightApp_SelectionMgr*  GeomSelectionTools::selectionMgr()
{
  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  if (anApp)
    return dynamic_cast<LightApp_SelectionMgr*>( anApp->selectionMgr() ); 
  else  
    return 0;
}

/*!
 * Return the list of the selected Salome Interactive Object (SALOME_ListIO*)
 * @return the list of the selected Salome Interactive Object
 */
SALOME_ListIO* GeomSelectionTools::getSelectedSalomeObjects()
{
  SALOME_ListIO* selected;
  LightApp_SelectionMgr* aSel = selectionMgr();
  aSel->selectedObjects( *selected, NULL, false );
  return selected;
}

/*!
 * Return the first selected Salome Interactive Object (Handle(Salome_InteractiveObject))
 * @return the first selected Salome Interactive Object
 */
Handle(SALOME_InteractiveObject) GeomSelectionTools::getFirstSelectedSalomeObject()
{
  SALOME_ListIO selected;
  LightApp_SelectionMgr* aSel = selectionMgr();
  aSel->selectedObjects( selected, NULL, false );
  if (!selected.IsEmpty()){  
    SALOME_ListIteratorOfListIO anIt(selected);
    Handle(SALOME_InteractiveObject) anIO;
    anIO = anIt.Value();
    return anIO;
  }
  return NULL;
}

/*!
 * Return the entry of the first selected Object
 * @return the entry of the first selected Object
 */ 
std::string GeomSelectionTools::getFirstSelectedEntry()
{
  Handle(SALOME_InteractiveObject) anIO;
  std::string entry="";
  anIO=GeomSelectionTools::getFirstSelectedSalomeObject();
  return GeomSelectionTools::getEntryOfObject(anIO);
}

/*!
 * Return the entry of a Salome Interactive Object
 * @param anIO the Handle of the Salome Interactive Object
 * @return the entry of the Salome Interactive Object
 */
std::string GeomSelectionTools::getEntryOfObject(Handle(SALOME_InteractiveObject) anIO){
  std::string entry="";
  _PTR(SObject) aSO = myStudy->FindObjectID(anIO->getEntry());
  if (aSO){   
    _PTR(SObject) aRefSObj;
    // If selected object is a reference
    if ( aSO->ReferencedObject( aRefSObj ))
      entry = aRefSObj->GetID();
    // If selected object is a reference is not a reference
    else
      entry= anIO->getEntry();
  }
  return entry;
}
  
/*!
 * Retrieve the name from the entry of the object
 * @param entry the entry of the object
 * @return the name of the object
 */
std::string GeomSelectionTools::getNameFromEntry(std::string entry){
  std::string name = "";
  _PTR(SObject) aSO = myStudy->FindObjectID(entry);
  if (aSO){
    _PTR(SObject) aRefSObj;
    // If selected object is a reference
    if ( aSO->ReferencedObject( aRefSObj ))
      name = aRefSObj->GetName();
    // If selected object is a reference is not a reference
    else
      name = aSO->GetName();
   }
  return name;
}


/*!
 * Retrieve the component type of the first selected object, it manages successfully references.
 * @return the component type of the first selected object
 */
std::string GeomSelectionTools::getFirstSelectedComponentDataType()
{
  Handle(SALOME_InteractiveObject) anIO;
  std::string DataType="";
  anIO=GeomSelectionTools::getFirstSelectedSalomeObject();
  _PTR(SObject) aSO = myStudy->FindObjectID(anIO->getEntry()); 
  if (aSO){
    _PTR(SObject) aRefSObj;
    // If selected object is a reference 
    if ( aSO->ReferencedObject( aRefSObj ))
      DataType= aRefSObj->GetFatherComponent()->ComponentDataType();
    // If selected object is a reference is not a reference 
    else 
      DataType=anIO->getComponentDataType();
 }
 return DataType;
}

/*!
 *  Retrieve the TopoDS shape from the first selected object
 *  @return the TopoDS shape from the first selected object, empty TopoDS Shape if a shape is not selected.
 */
TopoDS_Shape GeomSelectionTools::getFirstSelectedTopoDSShape()
{
 Handle(SALOME_InteractiveObject) anIO;
 anIO=GeomSelectionTools::getFirstSelectedSalomeObject();
 return entryToShape(anIO->getEntry());
}

/*!
 * Retrieve the TopoDS shape from the entry
 * @return the TopoDS shape from the entry, empty TopoDS Shape if the entry does not define a shape.
 */
TopoDS_Shape GeomSelectionTools::entryToShape(std::string entry){
  TopoDS_Shape S = TopoDS_Shape();
   _PTR(SObject) aSO = myStudy->FindObjectID(entry);
  if (aSO){ 
    _PTR(SObject) aRefSObj;
    GEOM::GEOM_Object_var aShape;
    // If selected object is a reference 
    if ( aSO->ReferencedObject( aRefSObj )){
      if (aRefSObj->GetFatherComponent()->ComponentDataType() == "GEOM")  
        aShape=SMESH::SObjectToInterface<GEOM::GEOM_Object>(aRefSObj);
     }
     // If selected object is a reference is not a reference
     else {
       if (  aSO->GetFatherComponent()->ComponentDataType() == "GEOM") 
         aShape = SMESH::SObjectToInterface<GEOM::GEOM_Object>(aSO);
     }
     if ( !aShape->_is_nil() ){
       if (SMESH_Gen_i* gen = SMESH_Gen_i::GetSMESHGen())
          S=gen->GeomObjectToShape( aShape.in() );
    }
  }
  return S;
}

/*!
 * Gives the ShapeType of the first Selected Object, return TopAbs_SHAPE if the first selected object does not define a shape.
 * @return the ShapeType of the first Selected Object, return TopAbs_SHAPE if the first selected object does not define a shape.
 */
TopAbs_ShapeEnum GeomSelectionTools:: getFirstSelectedShapeType()
{
 TopoDS_Shape S=getFirstSelectedTopoDSShape();
 if (!S.IsNull())
   return S.ShapeType();
 else
   return TopAbs_SHAPE;
}

/*!
 *  Print information to std output of the face (if the first selected object is a face)
 *  and return the OCC type of face: Plane, Cylinder,Cone, Sphere, Torus, BezierSurface,BSplineSurface, SurfaceOfRevolution,SurfaceOfExtrusion, OtherSurface
 *  @return the OCC type of face: Plane, Cylinder,Cone, Sphere, Torus, BezierSurface,BSplineSurface, SurfaceOfRevolution,SurfaceOfExtrusion, OtherSurface
 *  return Other_Surface if the selected face is not a face.
 *  Information printed is :
 *  U and V degrees
 *  U and V number of poles
 *  U and V number of knots
 *  U or V is Rational ?
 *  
 */  
GeomAbs_SurfaceType GeomSelectionTools::getFaceInformation()
{
  TopoDS_Shape S=getFirstSelectedTopoDSShape();
  GeomAbs_SurfaceType surf_type=GeomAbs_OtherSurface ;
  if (!S.IsNull() &&  S.ShapeType()==TopAbs_FACE){
    TopoDS_Face f=TopoDS::Face(S);
    Handle(Geom_Surface) surf = BRep_Tool::Surface(f);
    BRepAdaptor_Surface surf_adap=BRepAdaptor_Surface::BRepAdaptor_Surface(f);
    
    /* Global Information */
    cout << "GLOBAL INFORMATION" << endl;
    cout << "******************" << endl;
    stringstream buffer;
    buffer << "Degre U : " <<  surf_adap.UDegree();
   //conversion nécessaire pour affichage
    cout << buffer.str() << endl;
    cout <<  " Degre V : " <<  surf_adap.VDegree() << endl;
    cout <<  " Nb Poles U : " <<  surf_adap.NbUPoles() << endl;
    cout <<  " Nb Poles V : " <<  surf_adap.NbVPoles() << endl;    
    cout <<  " Nb Noeuds U : " <<  surf_adap.NbUKnots() << endl; 
    cout <<  " Nb Noeuds V : " <<  surf_adap.NbVKnots() << endl;
    cout <<  " U Rationnel ? " <<  surf_adap.IsURational() << endl;
    cout <<  " V Rationnel ? " <<  surf_adap.IsVRational() << endl;

    surf_type=surf_adap.GetType();
  }
  return surf_type;
}
