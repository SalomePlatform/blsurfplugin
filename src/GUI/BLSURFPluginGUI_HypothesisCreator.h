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
// ---
//
#ifndef BLSURFPLUGINGUI_HypothesisCreator_H
#define BLSURFPLUGINGUI_HypothesisCreator_H

#ifdef WIN32
  #ifdef BLSURFPLUGIN_GUI_EXPORTS
    #define BLSURFPLUGIN_GUI_EXPORT __declspec( dllexport )
  #else
    #define BLSURFPLUGIN_GUI_EXPORT __declspec( dllimport )
  #endif
#else
  #define BLSURFPLUGIN_GUI_EXPORT
#endif

#include <SMESHGUI_Hypotheses.h>
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(BLSURFPlugin_Algorithm)

class QGroupBox;
class QtxDoubleSpinBox;
class QComboBox;
class QCheckBox;
class QLineEdit;
class QTableWidget;
class QSpinBox;
class QMenu;
class QAction;

typedef struct
{
  int     myTopology, myVerbosity;
  int     myPhysicalMesh, myGeometricMesh;
  double  myAngleMeshS, myAngleMeshC, myGradation;
  QString myPhySize, myGeoMin, myGeoMax, myPhyMin, myPhyMax;
  bool    myAllowQuadrangles, myDecimesh;
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
  void             onPhysicalMeshChanged();
  void             onGeometricMeshChanged();
  void             onAddOption();
  void             onDeleteOption();
  void             onOptionChosenInPopup( QAction* );

private:
  bool             readParamsFromHypo( BlsurfHypothesisData& ) const;
  QString          readParamsFromWidgets( BlsurfHypothesisData& ) const;
  bool             storeParamsToHypo( const BlsurfHypothesisData& ) const;

private:
  QWidget*            myStdGroup;
  QLineEdit*          myName;
  QComboBox*          myPhysicalMesh;
  QLineEdit*          myPhySize;
  QLineEdit*          myPhyMin;
  QLineEdit*          myPhyMax;
  QComboBox*          myGeometricMesh;
  QtxDoubleSpinBox*   myAngleMeshS;
  QtxDoubleSpinBox*   myAngleMeshC;
  QLineEdit*          myGeoMin;
  QLineEdit*          myGeoMax;
  QtxDoubleSpinBox*   myGradation;
  QCheckBox*          myAllowQuadrangles;
  QCheckBox*          myDecimesh;

  QWidget*            myAdvGroup;
  QComboBox*          myTopology;
  QSpinBox*           myVerbosity;
  QTableWidget*       myOptionTable;

  BLSURFPlugin::string_array_var myOptions;
};

#endif // BLSURFPLUGINGUI_HypothesisCreator_H
