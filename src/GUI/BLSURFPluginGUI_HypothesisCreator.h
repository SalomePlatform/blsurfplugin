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
#include <SALOMEconfig.h>
#include <cstring>
#include <map>
#include <set>
#include <vector>
#include <TopAbs_ShapeEnum.hxx>
#include <GeomSelectionTools.h>
#include CORBA_SERVER_HEADER(BLSURFPlugin_Algorithm)

class QGroupBox;
class QComboBox;
class QCheckBox;
class QLineEdit;
class QTableWidget;
class QTreeWidget;
class QModelIndex;
class QSpinBox;
class QMenu;
class QAction;
class QTreeWidgetItem;

class SalomeApp_DoubleSpinBox;
class LightApp_SelectionMgr;

typedef struct
{
  int     myTopology, myVerbosity;
  int     myPhysicalMesh, myGeometricMesh;
  double  myAngleMeshS, myAngleMeshC, myGradation;
  QString myPhySize, myGeoMin, myGeoMax, myPhyMin,myPhyMax;
  bool    myAllowQuadrangles, myDecimesh,mySmpsurface,mySmpedge,mySmppoint,myEnforcedVertex;
  std::map<std::string, std::set<std::vector<double> > > enfVertMap;
  QString myName;
} BlsurfHypothesisData;


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
  void                insertElementType( TopAbs_ShapeEnum );
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
  void                onOptionChosenInPopup( QAction* );
//  void                onAddAttractor();
  void                onAddMapOnSurface();
  void                onAddMapOnEdge();
  void                onAddMapOnPoint();
  void                onRemoveMap();
  void                onSetSizeMap(int,int);

  void                addEnforcedVertex(std::string, std::string, double, double, double);
  void                onAddEnforcedVertices();
  void                onRemoveEnforcedVertex();
  void                synchronizeCoords();
  void                update(QTreeWidgetItem* , int );

private:
  bool                readParamsFromHypo( BlsurfHypothesisData& ) const;
  QString             readParamsFromWidgets( BlsurfHypothesisData& ) const;
  bool                storeParamsToHypo( const BlsurfHypothesisData& ) const;
  bool                sizeMapsValidation();
  bool                sizeMapValidationFromRow(int,bool displayError = true);
  bool                sizeMapValidationFromEntry(QString,bool displayError = true);
  GeomSelectionTools* getGeomSelectionTool();

private:
  QWidget*            myStdGroup;
  QLineEdit*          myName;
  QComboBox*          myPhysicalMesh;
  QLineEdit*          myPhySize;
  QLineEdit*          myPhyMin;
  QLineEdit*          myPhyMax;
  QComboBox*          myGeometricMesh;
  SalomeApp_DoubleSpinBox*   myAngleMeshS;
  SalomeApp_DoubleSpinBox*   myAngleMeshC;
  QLineEdit*          myGeoMin;
  QLineEdit*          myGeoMax;
  SalomeApp_DoubleSpinBox*   myGradation;
  QCheckBox*          myAllowQuadrangles;
  QCheckBox*          myDecimesh;

  QWidget*            myAdvGroup;
  QComboBox*          myTopology;
  QSpinBox*           myVerbosity;
  QTableWidget*       myOptionTable;

  QWidget             *mySmpGroup;
  QTableWidget        *mySizeMapTable;
  QPushButton         *addAttractorButton;
  QPushButton         *addSurfaceButton;
  QPushButton         *addEdgeButton;
  QPushButton         *addPointButton;
  QPushButton         *removeButton;

  QWidget*            myEnfGroup;
  QTreeWidget*        myEnforcedTreeWidget;
  QLineEdit*          myXCoord;
  QLineEdit*          myYCoord;
  QLineEdit*          myZCoord;
  QPushButton*        addVertexButton;
  QPushButton*        removeVertexButton;

  // map =  entry , size map
  QMap<QString, QString>          mySMPMap;
  QMap<QString, TopAbs_ShapeEnum> mySMPShapeTypeMap;
  GeomSelectionTools*             GeomToolSelected;
  LightApp_SelectionMgr*          aSel;

  BLSURFPlugin::string_array_var myOptions;

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
