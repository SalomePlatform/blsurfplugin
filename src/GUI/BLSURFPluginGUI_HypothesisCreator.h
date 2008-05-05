//  BLSURFPlugin GUI: GUI for plugged-in mesher BLSURFPlugin
//
//  Copyright (C) 2003  CEA
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
//  See http://www.salome-platform.org or email : webmaster.salome@opencascade.org
//
//
//
//  File   : BLSURFPluginGUI_HypothesisCreator.h
// Authors : Francis KLOSS (OCC) & Patrick LAUG (INRIA) & Lioka RAZAFINDRAZAKA (CEA)
//           & Aurelien ALLEAUME (DISTENE)
//  Module : BLSURFPlugin
//  $Header: 

#ifndef BLSURFPLUGINGUI_HypothesisCreator_HeaderFile
#define BLSURFPLUGINGUI_HypothesisCreator_HeaderFile

#include <SMESHGUI_Hypotheses.h>
#include CORBA_SERVER_HEADER(BLSURFPlugin_Algorithm)

class QtxDblSpinBox;
class QtxComboBox;
class QCheckBox;
class QLineEdit;
class QTable;
class QSpinBox;
class QPopupMenu;

typedef struct
{
  int     myTopology, myVerbosity;
  int     myPhysicalMesh, myGeometricMesh;
  double  myPhySize, myAngleMeshS, myAngleMeshC, myGradation;
  QString myGeoMin, myGeoMax, myPhyMin, myPhyMax;
//   double  myGeoMin, myGeoMax, myPhyMin, myPhyMax;
  bool    myAllowQuadrangles, myDecimesh;
  QString myName;

} BlsurfHypothesisData;

/*!
 * \brief Class for creation of BLSURF hypotheses
*/

class BLSURFPlugin_Hypothesis;

class BLSURFPluginGUI_HypothesisCreator : public SMESHGUI_GenericHypothesisCreator
{
  Q_OBJECT

public:
  BLSURFPluginGUI_HypothesisCreator( const QString& );
  virtual ~BLSURFPluginGUI_HypothesisCreator();

  virtual bool     checkParams() const;
  virtual QString  helpPage() const;

protected:
  virtual QFrame*  buildFrame    ();
  virtual void     retrieveParams() const;
  virtual QString  storeParams   () const;
  
  virtual QString  caption() const;
  virtual QPixmap  icon() const;
  virtual QString  type() const;

protected slots:
  void onPhysicalMeshChanged();
  void onGeometricMeshChanged();
  void onTabSelected(int);
  void onAddOption();
  void onDeleteOption();
  void onOptionChosenInPopup(int);

private:
  bool    readParamsFromHypo( BlsurfHypothesisData& ) const;
  QString readParamsFromWidgets( BlsurfHypothesisData& ) const;
  bool    storeParamsToHypo( const BlsurfHypothesisData& ) const;

private:

  QGroupBox*       myStdGroup;
  QLineEdit*       myName;
  QtxComboBox*     myPhysicalMesh;
  QtxDblSpinBox*   myPhySize;
//   QtxDblSpinBox*   myPhyMin;
//   QtxDblSpinBox*   myPhyMax;
  QLineEdit*       myPhyMin;
  QLineEdit*       myPhyMax;
  QtxComboBox*     myGeometricMesh;
  QtxDblSpinBox*   myAngleMeshS;
  QtxDblSpinBox*   myAngleMeshC;
  QLineEdit*       myGeoMin;
  QLineEdit*       myGeoMax;
//   QtxDblSpinBox*   myGeoMin;
//   QtxDblSpinBox*   myGeoMax;
  QtxDblSpinBox*   myGradation;
  QCheckBox*       myAllowQuadrangles;
  QCheckBox*       myDecimesh;

  QGroupBox*       myAdvGroup;
  QtxComboBox*     myTopology;
  QSpinBox*        myVerbosity;
  QTable*          myOptionTable;
  QPopupMenu*      myPopup;

  BLSURFPlugin::string_array_var myOptions;
};

#endif
