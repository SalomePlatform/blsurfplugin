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

class QtxDblSpinBox;
class QtxComboBox;
class QCheckBox;
class QLineEdit;

typedef struct
{
  int     myTopology;
  int     myPhysicalMesh, myGeometricMesh;
  double  myPhySize, myAngleMeshS, myGradation;
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

  virtual bool checkParams() const;

protected:
  virtual QFrame*  buildFrame    ();
  virtual void     retrieveParams() const;
  virtual QString  storeParams   () const;
  
  virtual QString  caption() const;
  virtual QPixmap  icon() const;
  virtual QString  type() const;

protected slots:
  virtual void onPhysicalMeshChanged();
  virtual void onGeometricMeshChanged();

private:
  bool readParamsFromHypo( BlsurfHypothesisData& ) const;
  bool readParamsFromWidgets( BlsurfHypothesisData& ) const;
  bool storeParamsToHypo( const BlsurfHypothesisData& ) const;

private:
 QLineEdit*       myName;
 QtxComboBox*     myTopology;
 QtxComboBox*     myPhysicalMesh;
 QtxDblSpinBox*   myPhySize;
 QtxComboBox*     myGeometricMesh;
 QtxDblSpinBox*   myAngleMeshS;
 QtxDblSpinBox*   myGradation;
 QCheckBox*       myAllowQuadrangles;
 QCheckBox*       myDecimesh;

 bool myIs2D;
};

#endif
