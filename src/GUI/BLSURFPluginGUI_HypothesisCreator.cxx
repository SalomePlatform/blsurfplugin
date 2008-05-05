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
#include <qtabbar.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qcursor.h>
#include <qpopupmenu.h>
#include <qtable.h>
#include <qapplication.h>

#include <set>

#define WITH_SIZE_BOUNDARIES

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

enum {
  STD_TAB = 0,
  ADV_TAB,
  OPTION_ID_COLUMN = 0,
  OPTION_NAME_COLUMN,
  OPTION_VALUE_COLUMN,
  NB_COLUMNS
};

BLSURFPluginGUI_HypothesisCreator::BLSURFPluginGUI_HypothesisCreator( const QString& theHypType )
: SMESHGUI_GenericHypothesisCreator( theHypType )
{
  myPopup = 0;
}

BLSURFPluginGUI_HypothesisCreator::~BLSURFPluginGUI_HypothesisCreator()
{
  if ( myPopup )
    delete myPopup;
}

namespace {
  inline bool isDouble( const QString& theText, const bool emptyOK=false ) {
    QString str = theText.stripWhiteSpace();
    bool isOk = true;
    if ( !str.isEmpty() )
      str.toDouble(&isOk);
    else
      isOk = emptyOK;
    return isOk;
  }
}

bool BLSURFPluginGUI_HypothesisCreator::checkParams() const
{
  if ( !isDouble( myPhyMin->text(), true )) {
    myPhyMin->selectAll();
    return false;
  }
  if ( !isDouble( myPhyMax->text(), true )) {
    myPhyMax->selectAll();
    return false;
  }
  if ( !isDouble( myGeoMin->text(), true )) {
    myGeoMin->selectAll();
    return false;
  }
  if ( !isDouble( myGeoMin->text(), true )) {
    myGeoMin->selectAll();
    return false;
  }

  return true;
}

QFrame* BLSURFPluginGUI_HypothesisCreator::buildFrame()
{
  QFrame* fr = new QFrame( 0, "myframe" );
  QVBoxLayout* lay = new QVBoxLayout( fr, 5, 0 );

  // tab
  QTabBar* tab = new QTabBar( fr, "QTabBar");
  tab->setShape( QTabBar::RoundedAbove );
  tab->insertTab( new QTab( tr( "SMESH_ARGUMENTS")), STD_TAB);
  tab->insertTab( new QTab( tr( "GHS3D_ADV_ARGS")), ADV_TAB);
  lay->addWidget( tab );

  // basic parameters

  myStdGroup = new QGroupBox( 2, Qt::Horizontal, fr, "myStdGroup" );
  myStdGroup->layout()->setSpacing( 6 );
  myStdGroup->layout()->setMargin( 11 );
  lay->addWidget( myStdGroup );

  myName = 0;
  if( isCreation() ) {
    new QLabel( tr( "SMESH_NAME" ), myStdGroup );
    myName = new QLineEdit( myStdGroup );
  }

  new QLabel( tr( "BLSURF_PHY_MESH" ), myStdGroup );
  myPhysicalMesh = new QtxComboBox( myStdGroup );
  QStringList physicalTypes;
  physicalTypes.append( QObject::tr( "BLSURF_DEFAULT_USER" ) );
  physicalTypes.append( QObject::tr( "BLSURF_CUSTOM_USER" ) );
  myPhysicalMesh->insertStringList( physicalTypes );

  new QLabel( tr( "BLSURF_HPHYDEF" ), myStdGroup );
  myPhySize = new QtxDblSpinBox( myStdGroup );
  myPhySize->setMinValue( 1e-02 );
  myPhySize->setMaxValue( 1e+02 );
  myPhySize->setLineStep( 1 );

#ifdef WITH_SIZE_BOUNDARIES
  new QLabel( tr( "BLSURF_HPHYMIN" ), myStdGroup );
  myPhyMin = new QLineEdit( myStdGroup );
//   myPhyMin = new QtxDblSpinBox( myStdGroup );
//   myPhyMin->setMinValue( 1e-06 );
//   myPhyMin->setMaxValue( 1e+04 );
//   myPhyMin->setLineStep( 1 );

  new QLabel( tr( "BLSURF_HPHYMAX" ), myStdGroup );
  myPhyMax = new QLineEdit( myStdGroup );
//   myPhyMax = new QtxDblSpinBox( myStdGroup );
//   myPhyMax->setMinValue( 1e-04 );
//   myPhyMax->setMaxValue( 1e+06 );
//   myPhyMax->setLineStep( 1 );
#endif

  new QLabel( tr( "BLSURF_GEOM_MESH" ), myStdGroup );
  myGeometricMesh = new QtxComboBox( myStdGroup );
  QStringList types;
  types.append( QObject::tr( "BLSURF_DEFAULT_GEOM" ) );
  types.append( QObject::tr( "BLSURF_CUSTOM_GEOM" ) );
  myGeometricMesh->insertStringList( types );

  new QLabel( tr( "BLSURF_ANGLE_MESH_S" ), myStdGroup );
  myAngleMeshS = new QtxDblSpinBox( myStdGroup );
  myAngleMeshS->setMinValue( 0 );
  myAngleMeshS->setMaxValue( 16 );
  myAngleMeshS->setLineStep( 0.5 );
  
  new QLabel( tr( "BLSURF_ANGLE_MESH_C" ), myStdGroup );
  myAngleMeshC = new QtxDblSpinBox( myStdGroup );
  myAngleMeshC->setMinValue( 0 );
  myAngleMeshC->setMaxValue( 16 );
  myAngleMeshC->setLineStep( 0.5 );
  
  new QLabel( tr( "BLSURF_GRADATION" ), myStdGroup );
  myGradation = new QtxDblSpinBox( myStdGroup );
  myGradation->setMinValue( 1.1 );
  myGradation->setMaxValue( 2.5 );
  myGradation->setLineStep( 0.1 );

#ifdef WITH_SIZE_BOUNDARIES
  new QLabel( tr( "BLSURF_HGEOMIN" ), myStdGroup );
  myGeoMin = new QLineEdit( myStdGroup );
//   myGeoMin = new QtxDblSpinBox( myStdGroup );
//   myGeoMin->setMinValue( 1e-06 );
//   myGeoMin->setMaxValue( 1e+04 );
//   myGeoMin->setLineStep( 1 );

  new QLabel( tr( "BLSURF_HGEOMAX" ), myStdGroup );
  myGeoMax = new QLineEdit( myStdGroup );
//   myGeoMax = new QtxDblSpinBox( myStdGroup );
//   myGeoMax->setMinValue( 1e-04 );
//   myGeoMax->setMaxValue( 1e+06 );
//   myGeoMax->setLineStep( 1 );
#endif

  myAllowQuadrangles = new QCheckBox( tr( "BLSURF_ALLOW_QUADRANGLES" ), myStdGroup );
  myStdGroup->addSpace(0);

  myDecimesh = new QCheckBox( tr( "BLSURF_DECIMESH" ), myStdGroup );
  myStdGroup->addSpace(0);



  // advanced parameters

  myAdvGroup = new QGroupBox( 3, Qt::Horizontal, fr, "myAdvGroup" );
  lay->addWidget( myAdvGroup );
  QFrame* anAdvFrame = new QFrame(myAdvGroup, "anAdvFrame");
  QGridLayout* anAdvLayout = new QGridLayout(anAdvFrame, 5, 3);
  anAdvLayout->setSpacing(6);
  anAdvLayout->setAutoAdd(false);
  
  QLabel* aTopologyLabel = new QLabel( tr( "BLSURF_TOPOLOGY" ), anAdvFrame );
  myTopology = new QtxComboBox( anAdvFrame );
  QStringList topologyTypes;
  topologyTypes.append( QObject::tr( "BLSURF_TOPOLOGY_CAD" ) );
  topologyTypes.append( QObject::tr( "BLSURF_TOPOLOGY_PROCESS" ) );
  topologyTypes.append( QObject::tr( "BLSURF_TOPOLOGY_PROCESS2" ) );
  myTopology->insertStringList( topologyTypes );

  QLabel* aVarbosityLabel = new QLabel( tr( "BLSURF_VERBOSITY" ), anAdvFrame );
  myVerbosity = new QSpinBox( anAdvFrame );
  myVerbosity->setMinValue( 0 );
  myVerbosity->setMaxValue( 100 );
  myVerbosity->setLineStep( 5 );

  QButton* addBtn = new QPushButton(tr("ADD_OPTION"), anAdvFrame, "addBtn");
  QButton* rmBtn = new QPushButton(tr("REMOVE_OPTION"), anAdvFrame, "rmBtn");

  myOptionTable = new QTable( 0, NB_COLUMNS, anAdvFrame, "myOptionTable");
  myOptionTable->hideColumn( OPTION_ID_COLUMN );
  myOptionTable->horizontalHeader()->setLabel( OPTION_NAME_COLUMN, tr("OPTION_NAME_COLUMN"));
  myOptionTable->horizontalHeader()->setLabel( OPTION_VALUE_COLUMN, tr("OPTION_VALUE_COLUMN"));
  myOptionTable->setColumnReadOnly( OPTION_NAME_COLUMN, TRUE );
  myOptionTable->setColumnReadOnly( OPTION_VALUE_COLUMN, FALSE );
  myOptionTable->setLeftMargin( 0 );

  anAdvLayout->addWidget(aTopologyLabel, 0, 0);
  anAdvLayout->addWidget(myTopology,     0, 1);

  anAdvLayout->addWidget(aVarbosityLabel, 1, 0);
  anAdvLayout->addWidget(myVerbosity,     1, 1);

  anAdvLayout->addMultiCellWidget( myOptionTable, 2,4, 0,1 );
  anAdvLayout->addWidget( addBtn,         2, 2);
  anAdvLayout->addWidget( rmBtn,          3, 2);

  anAdvLayout->setRowStretch( 4, 1 );
  anAdvLayout->setColStretch( 9, 1 );

  onTabSelected( STD_TAB );

  connect( tab,             SIGNAL( selected ( int ) ), this, SLOT( onTabSelected(int) ));
  connect( myGeometricMesh, SIGNAL( activated( int ) ), this, SLOT( onGeometricMeshChanged() ));
  connect( myPhysicalMesh,  SIGNAL( activated( int ) ), this, SLOT( onPhysicalMeshChanged() ));
  connect( addBtn,          SIGNAL( clicked()),         this, SLOT( onAddOption() ));
  connect( rmBtn,           SIGNAL( clicked()),         this, SLOT( onDeleteOption() ));

  return fr;
}

void BLSURFPluginGUI_HypothesisCreator::retrieveParams() const
{
  BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;

  BlsurfHypothesisData data;
  that->readParamsFromHypo( data );

  if( myName ) {
    myName->setText( data.myName );
    QFontMetrics metrics( myName->font() );
    myName->setMinimumWidth( metrics.width( data.myName )+5 );
  }
  myTopology        ->setCurrentItem( data.myTopology );
  myPhysicalMesh    ->setCurrentItem( data.myPhysicalMesh );
  myPhySize         ->setValue      ( data.myPhySize );
#ifdef WITH_SIZE_BOUNDARIES
  myPhyMin          ->setText       ( data.myPhyMin );
  myPhyMax          ->setText       ( data.myPhyMax );
  myGeoMin          ->setText       ( data.myGeoMin );
  myGeoMax          ->setText       ( data.myGeoMax );
//   myPhyMin          ->setValue      ( data.myPhyMin );
//   myPhyMax          ->setValue      ( data.myPhyMax );
//   myGeoMin          ->setValue      ( data.myGeoMin );
//   myGeoMax          ->setValue      ( data.myGeoMax );
#endif
  myGeometricMesh   ->setCurrentItem( data.myGeometricMesh );
  myAngleMeshS      ->setValue      ( data.myAngleMeshS);
  myAngleMeshC      ->setValue      ( data.myAngleMeshC);
  myGradation       ->setValue      ( data.myGradation);
  myAllowQuadrangles->setChecked    ( data.myAllowQuadrangles );
  myDecimesh        ->setChecked    ( data.myDecimesh );
  myVerbosity       ->setValue      ( data.myVerbosity);

  if ( myOptions.operator->() ) {
    BLSURFPluginGUI_HypothesisCreator* that = const_cast<BLSURFPluginGUI_HypothesisCreator*>(this);
    for ( int i = 0, nb = myOptions->length(); i < nb; ++i ) {
      QString option = that->myOptions[i].in();
      QStringList name_value = QStringList::split( ":", option );
      if ( name_value.size() > 1 ) {
        QString idStr = QString("%1").arg( i );
        int row = myOptionTable->numRows();
        myOptionTable->insertRows( row );
        myOptionTable->setText( row, OPTION_ID_COLUMN, idStr);
        myOptionTable->setText( row, OPTION_NAME_COLUMN, name_value[0]);
        myOptionTable->setText( row, OPTION_VALUE_COLUMN, name_value[1] );
      }
    } 
  }
  myOptionTable->adjustColumn( OPTION_NAME_COLUMN );
  // update widgets

  bool isPhysicalCustom = (myPhysicalMesh->currentItem() == PhysicalUserDefined);
  myPhySize->setEnabled(isPhysicalCustom);
  myPhyMax->setEnabled(isPhysicalCustom);
  myPhyMin->setEnabled(isPhysicalCustom);

  bool isCustom = (myGeometricMesh->currentItem() == UserDefined);
  myAngleMeshS->setEnabled(isCustom);
  myAngleMeshC->setEnabled(isCustom);
  myGradation->setEnabled(isCustom);
  myGeoMax->setEnabled(isCustom);
  myGeoMin->setEnabled(isCustom);
}

QString BLSURFPluginGUI_HypothesisCreator::storeParams() const
{
  BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;

  BlsurfHypothesisData data;
  QString guiHyp = that->readParamsFromWidgets( data );
  that->storeParamsToHypo( data );

  return guiHyp;
}

bool BLSURFPluginGUI_HypothesisCreator::readParamsFromHypo( BlsurfHypothesisData& h_data ) const
{
  BLSURFPlugin::BLSURFPlugin_Hypothesis_var h =
    BLSURFPlugin::BLSURFPlugin_Hypothesis::_narrow( initParamsHypothesis() );

  HypothesisData* data = SMESH::GetHypothesisData( hypType() );
  h_data.myName = isCreation() && data ? hypName() : "";

  h_data.myTopology         = (int) h->GetTopology();
  h_data.myPhysicalMesh     = (int) h->GetPhysicalMesh();
  h_data.myPhySize          = h->GetPhySize();
  h_data.myGeometricMesh    = (int) h->GetGeometricMesh();
  h_data.myAngleMeshS       = h->GetAngleMeshS();
  h_data.myAngleMeshC       = h->GetAngleMeshC();
  h_data.myGradation        = h->GetGradation();
  h_data.myAllowQuadrangles = h->GetQuadAllowed();
  h_data.myDecimesh         = h->GetDecimesh();
  h_data.myVerbosity        = h->GetVerbosity();

#ifdef WITH_SIZE_BOUNDARIES
  double PhyMin = h->GetPhyMin();
  double PhyMax = h->GetPhyMax();
  double GeoMin = h->GetGeoMin();
  double GeoMax = h->GetGeoMax();
  if ( PhyMin > 0 )
  h_data.myPhyMin = PhyMin > 0 ? QString::number( h->GetPhyMin() ) : QString("");
  h_data.myPhyMax = PhyMax > 0 ? QString::number( h->GetPhyMax() ) : QString("");
  h_data.myGeoMin = GeoMin > 0 ? QString::number( h->GetGeoMin() ) : QString("");
  h_data.myGeoMax = GeoMax > 0 ? QString::number( h->GetGeoMax() ) : QString("");
//   h_data.myPhyMin           = h->GetPhyMin();
//   h_data.myPhyMax           = h->GetPhyMax();
//   h_data.myGeoMin           = h->GetGeoMin();
//   h_data.myGeoMax           = h->GetGeoMax();
#endif

  BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;
  that->myOptions = h->GetOptionValues();

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
    if ( h->GetVerbosity() != h_data.myVerbosity )
      h->SetVerbosity( h_data.myVerbosity );

    if( (int) h_data.myPhysicalMesh == PhysicalUserDefined ) {
      if ( h->GetPhySize() != h_data.myPhySize )
        h->SetPhySize( h_data.myPhySize );
  }
    if( (int) h_data.myGeometricMesh == UserDefined ) {
      if ( h->GetAngleMeshS() != h_data.myAngleMeshS )
        h->SetAngleMeshS( h_data.myAngleMeshS );
      if ( h->GetAngleMeshC() != h_data.myAngleMeshC )
        h->SetAngleMeshC( h_data.myAngleMeshC );
    }
#ifdef WITH_SIZE_BOUNDARIES
    if ( !isDouble( h_data.myPhyMin ))
      h->SetPhyMin( -1 );
    else if ( h->GetPhyMin() != h_data.myPhyMin.toDouble() )
      h->SetPhyMin( h_data.myPhyMin.toDouble() );
    if ( !isDouble( h_data.myPhyMax ))
      h->SetPhyMax( -1 );
    else if ( h->GetPhyMax() != h_data.myPhyMax.toDouble() )
      h->SetPhyMax( h_data.myPhyMax.toDouble() );
    if ( !isDouble( h_data.myGeoMin ))
      h->SetGeoMin( -1 );
    else if ( h->GetGeoMin() != h_data.myGeoMin.toDouble() )
      h->SetGeoMin( h_data.myGeoMin.toDouble() );
    if ( !isDouble( h_data.myGeoMax ))
      h->SetGeoMax( -1 );
    else if ( h->GetGeoMax() != h_data.myGeoMax.toDouble() )
      h->SetGeoMax( h_data.myGeoMax.toDouble() );
//       if ( h->GetPhyMin() != h_data.myPhyMin )
//         h->SetPhyMin( h_data.myPhyMin );
//       if ( h->GetPhyMax() != h_data.myPhyMax )
//         h->SetPhyMax( h_data.myPhyMax );
//       if ( h->GetGeoMin() != h_data.myGeoMin )
//         h->SetGeoMin( h_data.myGeoMin );
//       if ( h->GetGeoMax() != h_data.myGeoMax )
//         h->SetGeoMax( h_data.myGeoMax );
#endif

    h->SetOptionValues( myOptions );
  }
  catch(const SALOME::SALOME_Exception& ex)
  {
    SalomeApp_Tools::QtCatchCorbaException(ex);
    ok = false;
  }
  return ok;
}

QString BLSURFPluginGUI_HypothesisCreator::readParamsFromWidgets( BlsurfHypothesisData& h_data ) const
{
  h_data.myName             = myName ? myName->text() : "";
  h_data.myTopology         = myTopology->currentItem();
  h_data.myPhysicalMesh     = myPhysicalMesh->currentItem();
  h_data.myPhySize          = myPhySize->value();
#ifdef WITH_SIZE_BOUNDARIES
  h_data.myPhyMin           = myPhyMin->text();
  h_data.myPhyMax           = myPhyMax->text();
  h_data.myGeoMin           = myGeoMin->text();
  h_data.myGeoMax           = myGeoMax->text();
//   h_data.myPhyMin           = myPhyMin->value();
//   h_data.myPhyMax           = myPhyMax->value();
//   h_data.myGeoMin           = myGeoMin->value();
//   h_data.myGeoMax           = myGeoMax->value();
#endif
  h_data.myGeometricMesh    = myGeometricMesh->currentItem();
  h_data.myAngleMeshS       = myAngleMeshS->value();
  h_data.myAngleMeshC       = myAngleMeshC->value();
  h_data.myGradation        = myGradation->value();
  h_data.myAllowQuadrangles = myAllowQuadrangles->isChecked();
  h_data.myDecimesh         = myDecimesh->isChecked();
  h_data.myVerbosity        = myVerbosity->value();

  QString guiHyp;
  guiHyp += tr("BLSURF_TOPOLOGY") + " = " + QString::number( h_data.myTopology ) + "; ";
  guiHyp += tr("BLSURF_PHY_MESH") + " = " + QString::number( h_data.myPhysicalMesh ) + "; ";
  guiHyp += tr("BLSURF_HPHYDEF") + " = " + QString::number( h_data.myPhySize ) + "; ";
  guiHyp += tr("BLSURF_GEOM_MESH") + " = " + QString::number( h_data.myGeometricMesh ) + "; ";
  guiHyp += tr("BLSURF_ANGLE_MESH_S") + " = " + QString::number( h_data.myAngleMeshS ) + "; ";
  guiHyp += tr("BLSURF_GRADATION") + " = " + QString::number( h_data.myGradation ) + "; ";
  guiHyp += tr("BLSURF_ALLOW_QUADRANGLES") + " = " + QString(h_data.myAllowQuadrangles ? "yes" : "no") + "; ";
  guiHyp += tr("BLSURF_DECIMESH") + " = " + QString(h_data.myDecimesh ? "yes" : "no") + "; ";
#ifdef WITH_SIZE_BOUNDARIES
  if ( isDouble( h_data.myPhyMin )) guiHyp += "hphymin = " + h_data.myPhyMin + "; ";
  if ( isDouble( h_data.myPhyMax )) guiHyp += "hphymax = " + h_data.myPhyMax + "; ";
  if ( isDouble( h_data.myGeoMin )) guiHyp += "hgeomin = " + h_data.myGeoMin + "; ";
  if ( isDouble( h_data.myGeoMax )) guiHyp += "hgeomax = " + h_data.myGeoMax + "; ";
#endif

  BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;
  int row = 0, nbRows = myOptionTable->numRows();
  for ( ; row < nbRows; ++row )
  {
    int id = myOptionTable->text( row, OPTION_ID_COLUMN ).toInt();
    if ( id >= 0 && id < myOptions->length() )
    {
      QString name = myOptionTable->text( row, OPTION_NAME_COLUMN );
      QString value = myOptionTable->text( row, OPTION_VALUE_COLUMN );
      if ( !value )
        value = "";
      that->myOptions[ id ] = ( name + ":" + value).latin1();
      if ( value != "" )
        guiHyp += name + " = " + value + "; ";
    }
  }

  cout << "guiHyp : " << guiHyp << endl;

  return guiHyp;
}

void BLSURFPluginGUI_HypothesisCreator::onPhysicalMeshChanged() {
  bool isCustom = (myPhysicalMesh->currentItem() == PhysicalUserDefined);
  myPhySize->setEnabled(isCustom);
  myPhyMax->setEnabled(isCustom);
  myPhyMin->setEnabled(isCustom);

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
  myAngleMeshC->setEnabled(isCustom);
  myGradation->setEnabled(isCustom);
  myGeoMax->setEnabled(isCustom);
  myGeoMin->setEnabled(isCustom);

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
    myAngleMeshC->setValue( aAngleMeshS );
    myGradation->setValue( aGradation );
    //  hphy_flag = 0 and hgeo_flag = 0 is not allowed (spec)
    if ( myPhysicalMesh->currentItem() == DefaultSize ) {
      myPhysicalMesh->setCurrentItem( PhysicalUserDefined );
      onPhysicalMeshChanged();
    }
  }
}

void BLSURFPluginGUI_HypothesisCreator::onTabSelected(int tab)
{
  if ( tab == STD_TAB ) {
    myAdvGroup->hide();
    myStdGroup->show();
  }
  else {
    myStdGroup->hide();
    myAdvGroup->show();
  }
  qApp->processEvents();
  dlg()->adjustSize();
}

void BLSURFPluginGUI_HypothesisCreator::onAddOption()
{
  if (!myPopup) {
    // fill popup with option names
    myPopup = new QPopupMenu();
    if ( myOptions.operator->() ) {
      for ( int i = 0, nb = myOptions->length(); i < nb; ++i ) {
        QString name_value = myOptions[i].in();
        QString name = QStringList::split( ":", name_value )[0];
        myPopup->insertItem( name, i );
      }
    }
    connect( myPopup, SIGNAL( activated( int ) ), SLOT( onOptionChosenInPopup( int ) ) );
  }
  if ( myPopup && myPopup->text(0) )
    myPopup->exec( QCursor::pos() );
}

void BLSURFPluginGUI_HypothesisCreator::onOptionChosenInPopup(int i)
{
  QString idStr = QString("%1").arg( i );
  QString option = myOptions[i].in();
  QString optionName = QStringList::split( ":", option )[0];

  // look for a row with optionName
  int row = 0, nbRows = myOptionTable->numRows();
  for ( ; row < nbRows; ++row )
    if ( myOptionTable->text( row, OPTION_ID_COLUMN ) == idStr )
      break;
  // add a row if not found
  if ( row == nbRows )
    myOptionTable->insertRows( nbRows );

  myOptionTable->setText( row, OPTION_ID_COLUMN, idStr);
  myOptionTable->setText( row, OPTION_NAME_COLUMN, optionName);
  myOptionTable->editCell( row, OPTION_VALUE_COLUMN );
  myOptionTable->adjustColumn( OPTION_NAME_COLUMN );
}
    
void BLSURFPluginGUI_HypothesisCreator::onDeleteOption()
{
  // clear option values and remember selected row
  std::set<int> selectedRows;
  int row = 0, nbRows = myOptionTable->numRows();
  for ( ; row < nbRows; ++row ) {
    if ( myOptionTable->isRowSelected( row )) {
      selectedRows.insert( row );
      int id = myOptionTable->text( row, OPTION_ID_COLUMN ).toInt();
      if ( id >= 0 && id < myOptions->length() )
        myOptions[ id ] = myOptionTable->text( row, OPTION_NAME_COLUMN );
    }
  }
  // remove selected rows
  std::set<int>::reverse_iterator r = selectedRows.rbegin();
  for ( ; r != selectedRows.rend(); ++r )
    myOptionTable->removeRow ( *r );
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
