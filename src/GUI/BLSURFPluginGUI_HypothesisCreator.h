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
// File    : BLSURFPluginGUI_HypothesisCreator.h
// Authors : Francis KLOSS (OCC) & Patrick LAUG (INRIA) & Lioka RAZAFINDRAZAKA (CEA)
//           & Aurelien ALLEAUME (DISTENE)
//           Size maps developement: Nicolas GEIMER (OCC) & Gilles DAVID (EURIWARE)
// ---
//
#ifndef BLSURFPLUGINGUI_HypothesisCreator_H
#define BLSURFPLUGINGUI_HypothesisCreator_H

#ifdef WIN32
  #if defined BLSURFPLUGIN_GUI_EXPORTS || defined BLSURFPluginGUI_EXPORTS
    #define BLSURFPLUGIN_GUI_EXPORT __declspec( dllexport )
  #else
    #define BLSURFPLUGIN_GUI_EXPORT __declspec( dllimport )
  #endif
#else
  #define BLSURFPLUGIN_GUI_EXPORT
#endif

#ifdef WNT
// E.A. : On windows with python 2.6, there is a conflict
// E.A. : between pymath.h and Standard_math.h which define
// E.A. : some same symbols : acosh, asinh, ...
#include <Standard_math.hxx>
#include <pymath.h>
#endif

#include <Python.h>

#include <QItemDelegate>

#include <SMESHGUI_Hypotheses.h>
#include "StdMeshersGUI_ObjectReferenceParamWdg.h"
#include <SALOMEconfig.h>
#include <cstring>
#include <map>
#include <set>
#include <vector>
#include <TopAbs_ShapeEnum.hxx>
#include <GeomSelectionTools.h>
#include <GEOM_Client.hxx>
#include CORBA_SERVER_HEADER(BLSURFPlugin_Algorithm)

class QGroupBox;
class QComboBox;
class QCheckBox;
class QLineEdit;
class QTableWidget;
class QTreeWidget;
class QModelIndex;
class QSpinBox;
class QDoubleSpinBox;
class QMenu;
class QAction;
class QTreeWidgetItem;
class QTableWidgetItem;
class QObject;

class SMESHGUI_SpinBox;
class LightApp_SelectionMgr;
// class DlgBlSurfHyp_Enforced;

// Name
typedef std::string TEnfName;
// Entry
typedef std::string TEntry;
// List of entries
typedef std::set<TEntry> TEntryList;
// Enforced vertex = 3 coordinates
typedef std::vector<double> TEnfVertexCoords;
// List of enforced vertices
typedef std::set< TEnfVertexCoords > TEnfVertexCoordsList;
// Enforced vertex
struct TEnfVertex{
  TEnfName name;
  TEntry geomEntry;
  TEnfVertexCoords coords;
  TEnfName grpName;
};


struct CompareEnfVertices
{
  bool operator () (const TEnfVertex* e1, const TEnfVertex* e2) const {
    if (e1 && e2) {
      if (e1->coords.size() && e2->coords.size())
        return (e1->coords < e2->coords);
      else
        return (e1->geomEntry < e2->geomEntry);
    }
    return false;
  }
};

// List of enforced vertices
typedef std::set< TEnfVertex*, CompareEnfVertices > TEnfVertexList;

// Map Face Entry / List of enforced vertices
typedef std::map< TEntry, TEnfVertexList > TFaceEntryEnfVertexListMap;

typedef struct
{
  int     myTopology, myVerbosity;
  int     myPhysicalMesh, myGeometricMesh;
  double  myAngleMeshS, myAngleMeshC, myGradation;
  double  myPhySize, myGeoMin, myGeoMax, myPhyMin,myPhyMax;
  bool    myAllowQuadrangles, myDecimesh,mySmpsurface,mySmpedge,mySmppoint,myEnforcedVertex;
  bool    myPreCADMergeEdges, myPreCADRemoveNanoEdges, myPreCADDiscardInput;
  double  myPreCADEpsNano;
//   bool    myGMFFileMode;
  std::string myGMFFileName;
  TEnfVertexList enfVertexList;
  TFaceEntryEnfVertexListMap faceEntryEnfVertexListMap;
  /* TODO GROUPS
  TGroupNameEnfVertexListMap groupNameEnfVertexListMap;
  */
  QString myName;
} BlsurfHypothesisData;



// class BLSURFPluginGUI_ObjectReferenceParamWdg: public StdMeshersGUI_ObjectReferenceParamWdg
// {
//   Q_OBJECT
// public:
//   BLSURFPluginGUI_ObjectReferenceParamWdg( SUIT_SelectionFilter* filter, 
//                                          QWidget*              parent,
//                                          bool                  multiSelection=false);
//   BLSURFPluginGUI_ObjectReferenceParamWdg( MeshObjectType objType,
//                                          QWidget*       parent,
//                                          bool           multiSelection=false);
//   ~BLSURFPluginGUI_ObjectReferenceParamWdg();
// 
// private:
//   void init();
//   
// public slots:
//   void activateSelectionOnly();
//   void deactivateSelectionOnly();
//   void setActivationStatus(bool status);
// };

/*!
 * \brief Class for creation of BLSURF hypotheses
*/
class BLSURFPLUGIN_GUI_EXPORT BLSURFPluginGUI_HypothesisCreator : public SMESHGUI_GenericHypothesisCreator
{
  Q_OBJECT

public:
  BLSURFPluginGUI_HypothesisCreator( const QString& );
  virtual ~BLSURFPluginGUI_HypothesisCreator();

  virtual bool        checkParams() const;
  virtual QString     helpPage() const;
  //void                insertElementType( TopAbs_ShapeEnum );
  void                insertElement( GEOM::GEOM_Object_var, bool modify = false );
  void                insertAttractor(GEOM::GEOM_Object_var, GEOM::GEOM_Object_var, bool modify = false);
  int                 findRowFromEntry(QString entry);
  CORBA::Object_var   entryToObject(QString entry);
  static LightApp_SelectionMgr* selectionMgr();

protected:
  virtual QFrame*     buildFrame    ();
  virtual void        retrieveParams() const;
  virtual QString     storeParams   () const;

  virtual QString     caption() const;
  virtual QPixmap     icon() const;
  virtual QString     type() const;

protected slots:
  void                onPhysicalMeshChanged();
  void                onGeometricMeshChanged();
  void                onAddOption();
  void                onDeleteOption();
  void                onChooseGMFFile();
  void                onOptionChosenInPopup( QAction* );
  void                onTopologyChanged( int );
  void                onMapGeomContentModified();
  void                onSmpItemClicked( QTreeWidgetItem *, int );
  void                onSmpTabChanged(int);
  void                onAttractorClicked(int);
  void                onConstSizeClicked(int);
  void                onAddMap();
  void                onRemoveMap();
  void                onModifyMap();
  void                onSetSizeMap(QTreeWidgetItem *, int);

  void                addEnforcedVertex(std::string theFaceEntry, std::string theFaceName, double x=0, double y=0, double z=0, 
                                        std::string vertexName = "", std::string geomEntry = "", std::string groupName = "");
  void                onAddEnforcedVertices();
  void                onRemoveEnforcedVertex();
  void                synchronizeCoords();
  void                updateEnforcedVertexValues(QTreeWidgetItem* , int );
  void                onSelectEnforcedVertex();
  void                deactivateSelection(QWidget*, QWidget*);
  void                clearEnforcedVertexWidgets();

private:
  bool                readParamsFromHypo( BlsurfHypothesisData& ) const;
  QString             readParamsFromWidgets( BlsurfHypothesisData& ) const;
  bool                storeParamsToHypo( const BlsurfHypothesisData& ) const;
  bool                sizeMapsValidation();
  bool                sizeMapValidationFromRow(int,bool displayError = true);
  bool                sizeMapValidationFromEntry(QString,bool displayError = true);
  GeomSelectionTools* getGeomSelectionTool();
  GEOM::GEOM_Gen_var  getGeomEngine();

private:
  QWidget*            myStdGroup;
  QLineEdit*          myName;
  QComboBox*          myPhysicalMesh;
  SMESHGUI_SpinBox*   myPhySize;
  SMESHGUI_SpinBox*   myPhyMin;
  SMESHGUI_SpinBox*   myPhyMax;
  QComboBox*          myGeometricMesh;
  SMESHGUI_SpinBox*   myAngleMeshS;
  SMESHGUI_SpinBox*   myAngleMeshC;
  SMESHGUI_SpinBox*   myGeoMin;
  SMESHGUI_SpinBox*   myGeoMax;
  SMESHGUI_SpinBox*   myGradation;
  QCheckBox*          myAllowQuadrangles;
  QCheckBox*          myDecimesh;

  QWidget*            myAdvGroup;
  QComboBox*          myTopology;
  QGroupBox*          myPreCADGroupBox;
  QCheckBox*          myPreCADMergeEdges;
  QCheckBox*          myPreCADRemoveNanoEdges;
  QCheckBox*          myPreCADDiscardInput;
  SMESHGUI_SpinBox*   myPreCADEpsNano;
  QSpinBox*           myVerbosity;
  QTableWidget*       myOptionTable;
  QLineEdit*          myGMFFileName;
//   QCheckBox*          myGMFFileMode;

  // Sizemap widgets
  QWidget             *mySmpGroup;
  QTreeWidget         *mySizeMapTable;
  QPushButton         *addMapButton;
  QPushButton         *removeMapButton;
  QPushButton         *modifyMapButton;
  QTabWidget          *smpTab; 
  QWidget             *myAttractorGroup;
  QWidget             *mySmpStdGroup;
  QCheckBox           *myAttractorCheck;
  QCheckBox           *myConstSizeCheck;
  QGroupBox           *myDistanceGroup;
//   QGroupBox           *myParamsGroup;
//   QWidget             *myParamsGroup;
  SMESHGUI_SpinBox    *myAttSizeSpin;
  SMESHGUI_SpinBox    *myAttDistSpin;
  SMESHGUI_SpinBox    *myAttDistSpin2;
  SMESHGUI_SpinBox    *mySmpSizeSpin;
  QLabel              *myAttDistLabel;
  QLabel              *myAttDistLabel2;
  QLabel              *myAttSizeLabel;
  // Selection widgets for size maps
  StdMeshersGUI_ObjectReferenceParamWdg *myGeomSelWdg1;
  StdMeshersGUI_ObjectReferenceParamWdg *myGeomSelWdg2;
  StdMeshersGUI_ObjectReferenceParamWdg *myAttSelWdg;
  StdMeshersGUI_ObjectReferenceParamWdg *myDistSelWdg;
  GEOM::GEOM_Object_var                  mySMapObject;
  GEOM::GEOM_Object_var                  myAttObject;
  GEOM::GEOM_Object_var                  myDistObject;
  
  
  
  
  QWidget*            myEnfGroup;
//    TODO FACE AND VERTEX SELECTION
  StdMeshersGUI_ObjectReferenceParamWdg *myEnfFaceWdg;
  GEOM::GEOM_Object_var myEnfFace;
  StdMeshersGUI_ObjectReferenceParamWdg *myEnfVertexWdg;
  GEOM::GEOM_Object_var myEnfVertex;

//   DlgBlSurfHyp_Enforced* myEnforcedVertexWidget;
  QTreeWidget*        myEnforcedTreeWidget;
  SMESHGUI_SpinBox*   myXCoord;
  SMESHGUI_SpinBox*   myYCoord;
  SMESHGUI_SpinBox*   myZCoord;

  QLineEdit*          myGroupName;
//   QGroupBox*          makeGroupsCheck;
  QCheckBox*          myGlobalGroupName;

  QPushButton*        addVertexButton;
  QPushButton*        removeVertexButton;

  // map =  entry , size map
  QMap<QString, QString>          mySMPMap;           // Map <face entry, size>
  QMap<QString, QString>          myATTMap;           // Map <face entry, att. entry>
  QMap<QString, double>           myDistMap;          // Map <entry,distance with constant size> 
  QMap<QString, double>           myAttDistMap;       // Map <entry, influence distance> 
  QMap<QString, TopAbs_ShapeEnum> mySMPShapeTypeMap;
  GeomSelectionTools*             GeomToolSelected;
  LightApp_SelectionMgr*          aSel;

  BLSURFPlugin::string_array_var myOptions, myPreCADOptions;

  PyObject *          main_mod;
  PyObject *          main_dict;
};


class EnforcedTreeWidgetDelegate : public QItemDelegate
{
    Q_OBJECT

public:
  EnforcedTreeWidgetDelegate(QObject *parent = 0);

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const;

  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  void setModelData(QWidget *editor, QAbstractItemModel *model,
                    const QModelIndex &index) const;

  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                    const QModelIndex &index) const;

  bool vertexExists(QAbstractItemModel *model, const QModelIndex &index, QString value) const;
};

#endif // BLSURFPLUGINGUI_HypothesisCreator_H
