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
//  File   : BLSURFPluginGUI_HypothesisCreator.cxx
// Authors : Francis KLOSS (OCC) & Patrick LAUG (INRIA) & Lioka RAZAFINDRAZAKA (CEA)
//  Module : BLSURFPlugin
//  $Header: 

#include "BLSURFPluginGUI_HypothesisCreator.h"

#include <SMESHGUI_Utils.h>
#include <SMESHGUI_HypothesesUtils.h>

#include CORBA_SERVER_HEADER(BLSURFPlugin_Algorithm)

#include <SUIT_Session.h>

#include <SalomeApp_Tools.h>

#include <QtxDblSpinBox.h>
#include <QtxComboBox.h>

#include <qlabel.h>
#include <qgroupbox.h>
#include <qframe.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpixmap.h>
#include <qfontmetrics.h>

enum Topology {
    FromCAD,
    Process,
    Process2
  };


enum PhysicalMesh
  {
    DefaultSize = 0,
    PhysicalUserDefined
  };

enum GeometricMesh
  {
    DefaultGeom = 0,
    UserDefined
  };

BLSURFPluginGUI_HypothesisCreator::BLSURFPluginGUI_HypothesisCreator( const QString& theHypType )
: SMESHGUI_GenericHypothesisCreator( theHypType )
{
  
}

BLSURFPluginGUI_HypothesisCreator::~BLSURFPluginGUI_HypothesisCreator()
{
}

bool BLSURFPluginGUI_HypothesisCreator::checkParams() const
{
  BlsurfHypothesisData data_old, data_new;
  readParamsFromHypo( data_old );
  readParamsFromWidgets( data_new );
  bool res = storeParamsToHypo( data_new );
  return res;
}

QFrame* BLSURFPluginGUI_HypothesisCreator::buildFrame()
{
  QFrame* fr = new QFrame( 0, "myframe" );
  QVBoxLayout* lay = new QVBoxLayout( fr, 5, 0 );

  QGroupBox* GroupC1 = new QGroupBox( 2, Qt::Horizontal, fr, "GroupC1" );
  lay->addWidget( GroupC1 );
  
  GroupC1->setTitle( tr( "SMESH_ARGUMENTS"  ) );
  GroupC1->layout()->setSpacing( 6 );
  GroupC1->layout()->setMargin( 11 );
  
  myName = 0;
  if( isCreation() ) {
    new QLabel( tr( "SMESH_NAME" ), GroupC1 );
    myName = new QLineEdit( GroupC1 );
  }

  new QLabel( tr( "BLSURF_TOPOLOGY" ), GroupC1 );
  myTopology = new QtxComboBox( GroupC1 );
  QStringList topologyTypes;
  topologyTypes.append( QObject::tr( "BLSURF_TOPOLOGY_CAD" ) );
  topologyTypes.append( QObject::tr( "BLSURF_TOPOLOGY_PROCESS" ) );
  topologyTypes.append( QObject::tr( "BLSURF_TOPOLOGY_PROCESS2" ) );
  myTopology->insertStringList( topologyTypes );

  new QLabel( tr( "BLSURF_PHY_MESH" ), GroupC1 );
  myPhysicalMesh = new QtxComboBox( GroupC1 );
  QStringList physicalTypes;
  physicalTypes.append( QObject::tr( "BLSURF_DEFAULT_USER" ) );
  physicalTypes.append( QObject::tr( "BLSURF_CUSTOM_USER" ) );
  myPhysicalMesh->insertStringList( physicalTypes );

  new QLabel( tr( "BLSURF_HPHYDEF" ), GroupC1 );
  myPhySize = new QtxDblSpinBox( GroupC1 );
  myPhySize->setMinValue( 1e-02 );
  myPhySize->setMaxValue( 1e+02 );
  myPhySize->setLineStep( 1 );

  new QLabel( tr( "BLSURF_GEOM_MESH" ), GroupC1 );
  myGeometricMesh = new QtxComboBox( GroupC1 );
  QStringList types;
  types.append( QObject::tr( "BLSURF_DEFAULT_GEOM" ) );
  types.append( QObject::tr( "BLSURF_CUSTOM_GEOM" ) );
  myGeometricMesh->insertStringList( types );

//   new QLabel( tr( "BLSURF_GROWTH_RATE" ), GroupC1 );
//   myGrowthRate = new QtxDblSpinBox( GroupC1 );
//   myGrowthRate->setMinValue( 0.1 );
//   myGrowthRate->setMaxValue( 10 );
//   myGrowthRate->setLineStep( 0.1 );

  new QLabel( tr( "BLSURF_ANGLE_MESH_S" ), GroupC1 );
  myAngleMeshS = new QtxDblSpinBox( GroupC1 );
  myAngleMeshS->setMinValue( 0 );
  myAngleMeshS->setMaxValue( 16 );
  myAngleMeshS->setLineStep( 0.5 );
  
  new QLabel( tr( "BLSURF_GRADATION" ), GroupC1 );
  myGradation = new QtxDblSpinBox( GroupC1 );
  myGradation->setMinValue( 1.1 );
  myGradation->setMaxValue( 2.5 );
  myGradation->setLineStep( 0.1 );

//   new QLabel( tr( "BLSURF_SEG_PER_RADIUS" ), GroupC1 );
//   myNbSegPerRadius = new QtxDblSpinBox( GroupC1 );
//   myNbSegPerRadius->setMinValue( 0.2 );
//   myNbSegPerRadius->setMaxValue( 5.0 );

  myAllowQuadrangles = new QCheckBox( tr( "BLSURF_ALLOW_QUADRANGLES" ), GroupC1 );
  GroupC1->addSpace(0);
//   myIs2D = true;

  myDecimesh = new QCheckBox( tr( "BLSURF_DECIMESH" ), GroupC1 );
  GroupC1->addSpace(0);

  connect( myGeometricMesh, SIGNAL( activated( int ) ), this, SLOT( onGeometricMeshChanged() ) );
  connect( myPhysicalMesh,  SIGNAL( activated( int ) ), this, SLOT( onPhysicalMeshChanged() ) );

  return fr;
}

void BLSURFPluginGUI_HypothesisCreator::retrieveParams() const
{
  BlsurfHypothesisData data;
  readParamsFromHypo( data );

  if( myName ) {
    myName->setText( data.myName );
    QFontMetrics metrics( myName->font() );
    myName->setMinimumWidth( metrics.width( data.myName )+5 );
  }
  myTopology->setCurrentItem( data.myTopology );
  myPhysicalMesh->setCurrentItem( data.myPhysicalMesh );
  myPhySize->setValue( data.myPhySize );
  myGeometricMesh->setCurrentItem( data.myGeometricMesh );
  myAngleMeshS->setValue( data.myAngleMeshS);
  myGradation->setValue( data.myGradation);
  myAllowQuadrangles->setChecked( data.myAllowQuadrangles );
  myDecimesh->setChecked( data.myDecimesh );

  // update widgets

  bool isPhysicalCustom = (myPhysicalMesh->currentItem() == PhysicalUserDefined);
  myPhySize->setEnabled(isPhysicalCustom);

  bool isCustom = (myGeometricMesh->currentItem() == UserDefined);
  myAngleMeshS->setEnabled(isCustom);
  myGradation->setEnabled(isCustom);
}

QString BLSURFPluginGUI_HypothesisCreator::storeParams() const
{
  BlsurfHypothesisData data;
  readParamsFromWidgets( data );
  storeParamsToHypo( data );

  QString guiHyp;
  guiHyp += tr("BLSURF_TOPOLOGY") + " = " + QString::number( data.myTopology ) + "; ";
  guiHyp += tr("BLSURF_PHY_MESH") + " = " + QString::number( data.myPhysicalMesh ) + "; ";
  guiHyp += tr("BLSURF_HPHYDEF") + " = " + QString::number( data.myPhySize ) + "; ";
  guiHyp += tr("BLSURF_GEOM_MESH") + " = " + QString::number( data.myGeometricMesh ) + "; ";
  guiHyp += tr("BLSURF_ANGLE_MESH_S") + " = " + QString::number( data.myAngleMeshS ) + "; ";
  guiHyp += tr("BLSURF_GRADATION") + " = " + QString::number( data.myGradation ) + "; ";
  guiHyp += tr("BLSURF_ALLOW_QUADRANGLES") + " = " + QString(data.myAllowQuadrangles ? "yes" : "no") + "; ";
  guiHyp += tr("BLSURF_DECIMESH") + " = " + QString(data.myDecimesh ? "yes" : "no") + "; ";

  cout << "guiHyp : " << guiHyp << endl;

  return guiHyp;
}

bool BLSURFPluginGUI_HypothesisCreator::readParamsFromHypo( BlsurfHypothesisData& h_data ) const
{
  BLSURFPlugin::BLSURFPlugin_Hypothesis_var h =
    BLSURFPlugin::BLSURFPlugin_Hypothesis::_narrow( initParamsHypothesis() );

  HypothesisData* data = SMESH::GetHypothesisData( hypType() );
  h_data.myName = isCreation() && data ? hypName() : "";

  h_data.myTopology = (int) h->GetTopology();
  h_data.myPhysicalMesh = (int) h->GetPhysicalMesh();
  h_data.myPhySize = h->GetPhySize();
  h_data.myGeometricMesh = (int) h->GetGeometricMesh();
  h_data.myAngleMeshS = h->GetAngleMeshS();
  h_data.myGradation = h->GetGradation();
  h_data.myAllowQuadrangles = h->GetQuadAllowed();
  h_data.myDecimesh = h->GetDecimesh();

  return true;
}

bool BLSURFPluginGUI_HypothesisCreator::storeParamsToHypo( const BlsurfHypothesisData& h_data ) const
{
  BLSURFPlugin::BLSURFPlugin_Hypothesis_var h =
    BLSURFPlugin::BLSURFPlugin_Hypothesis::_narrow( hypothesis() );

  bool ok = true;
  try
  {
    if( isCreation() )
      SMESH::SetName( SMESH::FindSObject( h ), h_data.myName.latin1() );

    if ( h->GetTopology() != h_data.myTopology ) // avoid duplication of DumpPython commands
      h->SetTopology( (int) h_data.myTopology );
    if ( h->GetPhysicalMesh() != h_data.myPhysicalMesh )
      h->SetPhysicalMesh( (int) h_data.myPhysicalMesh );
    if ( h->GetPhySize() != h_data.myPhySize )
      h->SetPhySize( h_data.myPhySize );
    if ( h->GetGeometricMesh() != (int) h_data.myGeometricMesh )
      h->SetGeometricMesh( (int) h_data.myGeometricMesh );
    if ( h->GetGradation() !=  h_data.myGradation )
      h->SetGradation( h_data.myGradation );
    if ( h->GetQuadAllowed() != h_data.myAllowQuadrangles )
      h->SetQuadAllowed( h_data.myAllowQuadrangles );
    if ( h->GetDecimesh() != h_data.myDecimesh )
      h->SetDecimesh( h_data.myDecimesh );

    if( (int) h_data.myPhysicalMesh == PhysicalUserDefined &&
        h->GetPhySize() != h_data.myPhySize)
      h->SetPhySize( h_data.myPhySize );

    if( (int) h_data.myGeometricMesh == UserDefined &&
        h->GetAngleMeshS() != h_data.myAngleMeshS )
      h->SetAngleMeshS( h_data.myAngleMeshS );
  }
  catch(const SALOME::SALOME_Exception& ex)
  {
    SalomeApp_Tools::QtCatchCorbaException(ex);
    ok = false;
  }
  return ok;
}

bool BLSURFPluginGUI_HypothesisCreator::readParamsFromWidgets( BlsurfHypothesisData& h_data ) const
{
  h_data.myName             = myName ? myName->text() : "";
  h_data.myTopology         = myTopology->currentItem();
  h_data.myPhysicalMesh     = myPhysicalMesh->currentItem();
  h_data.myPhySize          = myPhySize->value();
  h_data.myGeometricMesh    = myGeometricMesh->currentItem();
  h_data.myAngleMeshS       = myAngleMeshS->value();
  h_data.myGradation        = myGradation->value();
  h_data.myAllowQuadrangles = myAllowQuadrangles->isChecked();
  h_data.myDecimesh         = myDecimesh->isChecked();

  return true;
}

void BLSURFPluginGUI_HypothesisCreator::onPhysicalMeshChanged() {
  bool isCustom = (myPhysicalMesh->currentItem() == PhysicalUserDefined);
  myPhySize->setEnabled(isCustom);

  if ( ! isCustom ) {
    double aPhySize;
    switch( myPhysicalMesh->currentItem() ) {
      case DefaultSize:
      default:
        aPhySize = 10;
        break;
      }
    myPhySize->setValue( aPhySize );
    //  hphy_flag = 0 and hgeo_flag = 0 is not allowed (spec)
    if ( myGeometricMesh->currentItem() == DefaultGeom ) {
      myGeometricMesh->setCurrentItem( UserDefined );
      onGeometricMeshChanged();
    }
  }
}

void BLSURFPluginGUI_HypothesisCreator::onGeometricMeshChanged() {
  bool isCustom = (myGeometricMesh->currentItem() == UserDefined);
  myAngleMeshS->setEnabled(isCustom);
  myGradation->setEnabled(isCustom);

  if ( ! isCustom ) {
    double aAngleMeshS, aGradation;
    switch( myGeometricMesh->currentItem() ) {
      case DefaultGeom:
      default:
        aAngleMeshS = 8;
        aGradation = 1.1;
        break;
      }
    myAngleMeshS->setValue( aAngleMeshS );
    myGradation->setValue( aGradation );
    //  hphy_flag = 0 and hgeo_flag = 0 is not allowed (spec)
    if ( myPhysicalMesh->currentItem() == DefaultSize ) {
      myPhysicalMesh->setCurrentItem( PhysicalUserDefined );
      onPhysicalMeshChanged();
    }
  }
}

QString BLSURFPluginGUI_HypothesisCreator::caption() const
{
  return tr( "BLSURF_TITLE" );
}

QPixmap BLSURFPluginGUI_HypothesisCreator::icon() const
{
  QString hypIconName = tr( "ICON_DLG_BLSURF_PARAMETERS");
  return SUIT_Session::session()->resourceMgr()->loadPixmap( "BLSURFPlugin", hypIconName );
}

QString BLSURFPluginGUI_HypothesisCreator::type() const
{
  return tr( "BLSURF_HYPOTHESIS" );
}

QString BLSURFPluginGUI_HypothesisCreator::helpPage() const
{
  return "blsurf_hypo_page.html";
}
