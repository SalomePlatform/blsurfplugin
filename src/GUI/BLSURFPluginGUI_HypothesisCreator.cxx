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
// File    : BLSURFPluginGUI_HypothesisCreator.cxx
// Authors : Francis KLOSS (OCC) & Patrick LAUG (INRIA) & Lioka RAZAFINDRAZAKA (CEA)
//           & Aurelien ALLEAUME (DISTENE)
// ---
//
#include "BLSURFPluginGUI_HypothesisCreator.h"

#include <SMESHGUI_Utils.h>
#include <SMESHGUI_HypothesesUtils.h>

#include <SUIT_Session.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Tools.h>
#include <QtxDoubleSpinBox.h>

#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QFrame>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QTabWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QMenu>
#include <QTableWidget>
#include <QHeaderView>
#include <QApplication>

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
}

BLSURFPluginGUI_HypothesisCreator::~BLSURFPluginGUI_HypothesisCreator()
{
}

namespace {
  inline bool isDouble( const QString& theText, const bool emptyOK=false ) {
    QString str = theText.trimmed();
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
  bool ok = true;
  if ( !isDouble( myPhySize->text(), false )) {
    if ( myPhySize->text().isEmpty() )
      myPhySize->setText(tr("OBLIGATORY_VALUE"));
    myPhySize->selectAll();
    ok = false;
  }
  if ( !isDouble( myPhyMin->text(), true )) {
    myPhyMin->selectAll();
    ok = false;
  }
  if ( !isDouble( myPhyMax->text(), true )) {
    myPhyMax->selectAll();
    ok = false;
  }
  if ( !isDouble( myGeoMin->text(), true )) {
    myGeoMin->selectAll();
    ok = false;
  }
  if ( !isDouble( myGeoMin->text(), true )) {
    myGeoMin->selectAll();
    ok = false;
  }
  if ( ok )
  {
    myOptionTable->setFocus();
    QApplication::instance()->processEvents();

    BLSURFPlugin::BLSURFPlugin_Hypothesis_var h =
      BLSURFPlugin::BLSURFPlugin_Hypothesis::_narrow( initParamsHypothesis() );

    int row = 0, nbRows = myOptionTable->rowCount();
    for ( ; row < nbRows; ++row )
    {
      QString name  = myOptionTable->item( row, OPTION_NAME_COLUMN )->text();
      QString value = myOptionTable->item( row, OPTION_VALUE_COLUMN )->text().trimmed();
      if ( !value.isEmpty() ) {
        try {
          h->SetOptionValue( name.toLatin1().constData(), value.toLatin1().constData() );
        }
        catch ( const SALOME::SALOME_Exception& ex )
        {
          SUIT_MessageBox::critical( dlg(),
				     tr("SMESH_ERROR"),
				     ex.details.text.in() );
          ok = false;
        }
      }
    }
    h->SetOptionValues( myOptions ); // restore values
  }

  return ok;
}

QFrame* BLSURFPluginGUI_HypothesisCreator::buildFrame()
{
  QFrame* fr = new QFrame( 0 );
  QVBoxLayout* lay = new QVBoxLayout( fr );
  lay->setMargin( 5 );
  lay->setSpacing( 0 );

  // tab
  QTabWidget* tab = new QTabWidget( fr );
  tab->setTabShape( QTabWidget::Rounded );
  tab->setTabPosition( QTabWidget::North );
  lay->addWidget( tab );

  // basic parameters
  myStdGroup = new QWidget();
  QGridLayout* aStdLayout = new QGridLayout( myStdGroup );
  aStdLayout->setSpacing( 6 );
  aStdLayout->setMargin( 11 );

  int row = 0;
  myName = 0;
  if( isCreation() ) {
    aStdLayout->addWidget( new QLabel( tr( "SMESH_NAME" ), myStdGroup ), row, 0, 1, 1 );
    myName = new QLineEdit( myStdGroup );
    aStdLayout->addWidget( myName, row++, 1, 1, 1 );
  }

  aStdLayout->addWidget( new QLabel( tr( "BLSURF_PHY_MESH" ), myStdGroup ), row, 0, 1, 1 );
  myPhysicalMesh = new QComboBox( myStdGroup );
  aStdLayout->addWidget( myPhysicalMesh, row++, 1, 1, 1 );
  QStringList physicalTypes;
  physicalTypes << tr( "BLSURF_DEFAULT_USER" ) << tr( "BLSURF_CUSTOM_USER" );
  myPhysicalMesh->addItems( physicalTypes );

  aStdLayout->addWidget( new QLabel( tr( "BLSURF_HPHYDEF" ), myStdGroup), row, 0, 1, 1 );
  myPhySize = new QLineEdit( myStdGroup );
  aStdLayout->addWidget( myPhySize, row++, 1, 1, 1 );

#ifdef WITH_SIZE_BOUNDARIES
  aStdLayout->addWidget( new QLabel( tr( "BLSURF_HPHYMIN" ), myStdGroup ), row, 0, 1, 1 );
  myPhyMin = new QLineEdit( myStdGroup );
  aStdLayout->addWidget( myPhyMin, row++, 1, 1, 1 );

  aStdLayout->addWidget( new QLabel( tr( "BLSURF_HPHYMAX" ), myStdGroup ), row, 0, 1, 1 );
  myPhyMax = new QLineEdit( myStdGroup );
  aStdLayout->addWidget( myPhyMax, row++, 1, 1, 1 );
#endif

  aStdLayout->addWidget( new QLabel( tr( "BLSURF_GEOM_MESH" ), myStdGroup ), row, 0, 1, 1 );
  myGeometricMesh = new QComboBox( myStdGroup );
  aStdLayout->addWidget( myGeometricMesh, row++, 1, 1, 1 );
  QStringList types;
  types << tr( "BLSURF_DEFAULT_GEOM" ) << tr( "BLSURF_CUSTOM_GEOM" );
  myGeometricMesh->addItems( types );

  aStdLayout->addWidget( new QLabel( tr( "BLSURF_ANGLE_MESH_S" ), myStdGroup ), row, 0, 1, 1 );
  myAngleMeshS = new QtxDoubleSpinBox( myStdGroup );
  aStdLayout->addWidget( myAngleMeshS, row++, 1, 1, 1 );
  myAngleMeshS->setMinimum( 0 );
  myAngleMeshS->setMaximum( 16 );
  myAngleMeshS->setSingleStep( 0.5 );
  
  aStdLayout->addWidget( new QLabel( tr( "BLSURF_ANGLE_MESH_C" ), myStdGroup ), row, 0, 1, 1 );
  myAngleMeshC = new QtxDoubleSpinBox( myStdGroup );
  aStdLayout->addWidget( myAngleMeshC, row++, 1, 1, 1 );
  myAngleMeshC->setMinimum( 0 );
  myAngleMeshC->setMaximum( 16 );
  myAngleMeshC->setSingleStep( 0.5 );
  
  aStdLayout->addWidget( new QLabel( tr( "BLSURF_GRADATION" ), myStdGroup ), row, 0, 1, 1 );
  myGradation = new QtxDoubleSpinBox( myStdGroup );
  aStdLayout->addWidget( myGradation, row++, 1, 1, 1 );
  myGradation->setMinimum( 1.1 );
  myGradation->setMaximum( 2.5 );
  myGradation->setSingleStep( 0.1 );

#ifdef WITH_SIZE_BOUNDARIES
  aStdLayout->addWidget( new QLabel( tr( "BLSURF_HGEOMIN" ), myStdGroup ), row, 0, 1, 1 );
  myGeoMin = new QLineEdit( myStdGroup );
  aStdLayout->addWidget( myGeoMin, row++, 1, 1, 1 );

  aStdLayout->addWidget( new QLabel( tr( "BLSURF_HGEOMAX" ), myStdGroup ), row, 0, 1, 1 );
  myGeoMax = new QLineEdit( myStdGroup );
  aStdLayout->addWidget( myGeoMax, row++, 1, 1, 1 );
#endif

  myAllowQuadrangles = new QCheckBox( tr( "BLSURF_ALLOW_QUADRANGLES" ), myStdGroup );
  aStdLayout->addWidget( myAllowQuadrangles, row++, 0, 1, 2 );

  myDecimesh = new QCheckBox( tr( "BLSURF_DECIMESH" ), myStdGroup );
  aStdLayout->addWidget( myDecimesh, row++, 0, 1, 2 );
  
  // advanced parameters
  myAdvGroup = new QWidget();
  QGridLayout* anAdvLayout = new QGridLayout( myAdvGroup );
  anAdvLayout->setSpacing( 6 );
  anAdvLayout->setMargin( 11 );

  anAdvLayout->addWidget( new QLabel( tr( "BLSURF_TOPOLOGY" ), myAdvGroup ), 0, 0, 1, 1 );
  myTopology = new QComboBox( myAdvGroup );
  anAdvLayout->addWidget( myTopology, 0, 1, 1, 1 );
  QStringList topologyTypes;
  topologyTypes << tr( "BLSURF_TOPOLOGY_CAD" ) << tr( "BLSURF_TOPOLOGY_PROCESS" ) << tr( "BLSURF_TOPOLOGY_PROCESS2" );
  myTopology->addItems( topologyTypes );

  anAdvLayout->addWidget( new QLabel( tr( "BLSURF_VERBOSITY" ), myAdvGroup ), 1, 0, 1, 1 );
  myVerbosity = new QSpinBox( myAdvGroup );
  anAdvLayout->addWidget( myVerbosity, 1, 1, 1, 1 );
  myVerbosity->setMinimum( 0 );
  myVerbosity->setMaximum( 100 );
  myVerbosity->setSingleStep( 5 );

  myOptionTable = new QTableWidget( 0, NB_COLUMNS, myAdvGroup );
  anAdvLayout->addWidget( myOptionTable, 2, 0, 3, 2 );
  QStringList headers;
  headers << tr( "OPTION_ID_COLUMN" ) << tr( "OPTION_NAME_COLUMN" ) << tr( "OPTION_VALUE_COLUMN" );
  myOptionTable->setHorizontalHeaderLabels( headers );
  myOptionTable->horizontalHeader()->hideSection( OPTION_ID_COLUMN );
  //myOptionTable->setColumnReadOnly( OPTION_NAME_COLUMN, TRUE );//////
  //myOptionTable->setColumnReadOnly( OPTION_VALUE_COLUMN, FALSE );/////
  myOptionTable->verticalHeader()->hide();
  //myOptionTable->setSelectionBehavior( QAbstractItemView::SelectRows );

  QPushButton* addBtn = new QPushButton( tr( "ADD_OPTION"),  myAdvGroup );
  anAdvLayout->addWidget( addBtn, 2, 2, 1, 1 );
  addBtn->setMenu( new QMenu() );

  QPushButton* rmBtn = new QPushButton( tr( "REMOVE_OPTION"), myAdvGroup );
  anAdvLayout->addWidget( rmBtn, 3, 2, 1, 1 );

  anAdvLayout->setRowStretch( 4, 5 );
  anAdvLayout->setColumnStretch( 1, 5 );

  // ---
  tab->insertTab( STD_TAB, myStdGroup, tr( "SMESH_ARGUMENTS" ) );
  tab->insertTab( ADV_TAB, myAdvGroup, tr( "GHS3D_ADV_ARGS" ) );
  tab->setCurrentIndex( STD_TAB );

  // ---
  connect( myGeometricMesh, SIGNAL( activated( int ) ), this, SLOT( onGeometricMeshChanged() ) );
  connect( myPhysicalMesh,  SIGNAL( activated( int ) ), this, SLOT( onPhysicalMeshChanged() ) );
  connect( addBtn->menu(),  SIGNAL( aboutToShow() ),    this, SLOT( onAddOption() ) );
  connect( addBtn->menu(),  SIGNAL( triggered( QAction* ) ), this, SLOT( onOptionChosenInPopup( QAction* ) ) );
  connect( rmBtn,           SIGNAL( clicked()),         this, SLOT( onDeleteOption() ) );

  return fr;
}

void BLSURFPluginGUI_HypothesisCreator::retrieveParams() const
{
  BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;

  BlsurfHypothesisData data;
  that->readParamsFromHypo( data );

  if ( myName ) {
    myName->setText( data.myName );
    QFontMetrics metrics( myName->font() );
    myName->setMinimumWidth( metrics.width( data.myName )+5 );
  }
  myTopology->setCurrentIndex( data.myTopology );
  myPhysicalMesh->setCurrentIndex( data.myPhysicalMesh );
  myPhySize->setText( data.myPhySize );
#ifdef WITH_SIZE_BOUNDARIES
  myPhyMin->setText( data.myPhyMin );
  myPhyMax->setText( data.myPhyMax );
  myGeoMin->setText( data.myGeoMin );
  myGeoMax->setText( data.myGeoMax );
#endif
  myGeometricMesh->setCurrentIndex( data.myGeometricMesh );
  myAngleMeshS->setValue( data.myAngleMeshS );
  myAngleMeshC->setValue( data.myAngleMeshC );
  myGradation->setValue( data.myGradation );
  myAllowQuadrangles->setChecked( data.myAllowQuadrangles );
  myDecimesh->setChecked( data.myDecimesh );
  myVerbosity->setValue( data.myVerbosity );

  if ( myOptions.operator->() ) {
    printf("retrieveParams():myOptions->length()=%d\n",myOptions->length());
    for ( int i = 0, nb = myOptions->length(); i < nb; ++i ) {
      QString option = that->myOptions[i].in();
      QStringList name_value = option.split( ":", QString::KeepEmptyParts );
      if ( name_value.count() > 1 ) {
        QString idStr = QString("%1").arg( i );
        int row = myOptionTable->rowCount();
	myOptionTable->setRowCount( row+1 );
	myOptionTable->setItem( row, OPTION_ID_COLUMN, new QTableWidgetItem( idStr ) );
	myOptionTable->item( row, OPTION_ID_COLUMN )->setFlags( 0 );
	myOptionTable->setItem( row, OPTION_NAME_COLUMN, new QTableWidgetItem( name_value[0] ) );
	myOptionTable->item( row, OPTION_NAME_COLUMN )->setFlags( 0 );
	myOptionTable->setItem( row, OPTION_VALUE_COLUMN, new QTableWidgetItem( name_value[1] ) );
	myOptionTable->item( row, OPTION_VALUE_COLUMN )->setFlags( Qt::ItemIsSelectable | 
								   Qt::ItemIsEditable   | 
								   Qt::ItemIsEnabled );
      }
    } 
  }
  myOptionTable->resizeColumnToContents( OPTION_NAME_COLUMN );

  // update widgets
  that->onPhysicalMeshChanged();
  that->onGeometricMeshChanged();
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
  h_data.myPhySize          = QString::number( h->GetPhySize() );
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
      SMESH::SetName( SMESH::FindSObject( h ), h_data.myName.toLatin1().constData() );

    if ( h->GetTopology() != h_data.myTopology ) // avoid duplication of DumpPython commands
      h->SetTopology( (int) h_data.myTopology );
    if ( h->GetPhysicalMesh() != h_data.myPhysicalMesh )
      h->SetPhysicalMesh( (int) h_data.myPhysicalMesh );
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
      if ( h->GetPhySize() != h_data.myPhySize.toDouble() )
        h->SetPhySize( h_data.myPhySize.toDouble() );
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
#endif

    printf("storeParamsToHypo():myOptions->length()=%d\n",myOptions->length());
    h->SetOptionValues( myOptions ); // is set in checkParams()
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
  h_data.myTopology         = myTopology->currentIndex();
  h_data.myPhysicalMesh     = myPhysicalMesh->currentIndex();
  h_data.myPhySize          = myPhySize->text();
#ifdef WITH_SIZE_BOUNDARIES
  h_data.myPhyMin           = myPhyMin->text();
  h_data.myPhyMax           = myPhyMax->text();
  h_data.myGeoMin           = myGeoMin->text();
  h_data.myGeoMax           = myGeoMax->text();
#endif
  h_data.myGeometricMesh    = myGeometricMesh->currentIndex();
  h_data.myAngleMeshS       = myAngleMeshS->value();
  h_data.myAngleMeshC       = myAngleMeshC->value();
  h_data.myGradation        = myGradation->value();
  h_data.myAllowQuadrangles = myAllowQuadrangles->isChecked();
  h_data.myDecimesh         = myDecimesh->isChecked();
  h_data.myVerbosity        = myVerbosity->value();

  QString guiHyp;
  guiHyp += tr("BLSURF_TOPOLOGY") + " = " + QString::number( h_data.myTopology ) + "; ";
  guiHyp += tr("BLSURF_PHY_MESH") + " = " + QString::number( h_data.myPhysicalMesh ) + "; ";
  guiHyp += tr("BLSURF_HPHYDEF") + " = " + h_data.myPhySize + "; ";
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
  int row = 0, nbRows = myOptionTable->rowCount();
  for ( ; row < nbRows; ++row )
  {
    int id = myOptionTable->item( row, OPTION_ID_COLUMN )->text().toInt();
    if ( id >= 0 && id < myOptions->length() )
    {
      QString name  = myOptionTable->item( row, OPTION_NAME_COLUMN )->text();
      QString value = myOptionTable->item( row, OPTION_VALUE_COLUMN )->text().trimmed();
      if ( value.isNull() )
        value = "";
      that->myOptions[ id ] = ( name + ":" + value).toLatin1().constData();
      if ( value != "" )
        guiHyp += name + " = " + value + "; ";
    }
  }

  cout << "guiHyp : " << guiHyp.toLatin1().data() << endl;

  return guiHyp;
}

void BLSURFPluginGUI_HypothesisCreator::onPhysicalMeshChanged() {
  bool isCustom = (myPhysicalMesh->currentIndex() == PhysicalUserDefined);
  myPhySize->setEnabled(isCustom);
  myPhyMax->setEnabled(isCustom);
  myPhyMin->setEnabled(isCustom);

  if ( !isCustom ) {
    QString aPhySize = "";
    switch( myPhysicalMesh->currentIndex() ) {
      case DefaultSize:
      default:
        aPhySize = "10";
        break;
      }
    myPhySize->setText( aPhySize );
    if ( !isDouble( myPhyMin->text(), true ))
      myPhyMin->setText("");
    if ( !isDouble( myPhyMax->text(), true ))
      myPhyMax->setText("");
    if ( myGeometricMesh->currentIndex() == DefaultGeom ) {
      myGeometricMesh->setCurrentIndex( UserDefined );
      onGeometricMeshChanged();
    }
  }
}

void BLSURFPluginGUI_HypothesisCreator::onGeometricMeshChanged() {
  bool isCustom = (myGeometricMesh->currentIndex() == UserDefined);
  myAngleMeshS->setEnabled(isCustom);
  myAngleMeshC->setEnabled(isCustom);
  myGradation->setEnabled(isCustom);
  myGeoMax->setEnabled(isCustom);
  myGeoMin->setEnabled(isCustom);

  if ( ! isCustom ) {
    double aAngleMeshS, aGradation;
    switch( myGeometricMesh->currentIndex() ) {
      case DefaultGeom:
      default:
        aAngleMeshS = 8;
        aGradation = 1.1;
        break;
      }
    myAngleMeshS->setValue( aAngleMeshS );
    myAngleMeshC->setValue( aAngleMeshS );
    myGradation->setValue( aGradation );
    if ( !isDouble( myGeoMin->text(), true ))
      myGeoMin->setText("");
    if ( !isDouble( myGeoMax->text(), true ))
      myGeoMax->setText("");
    //  hphy_flag = 0 and hgeo_flag = 0 is not allowed (spec)
    if ( myPhysicalMesh->currentIndex() == DefaultSize ) {
      myPhysicalMesh->setCurrentIndex( PhysicalUserDefined );
      onPhysicalMeshChanged();
    }
  }
}

void BLSURFPluginGUI_HypothesisCreator::onAddOption()
{
  QMenu* menu = (QMenu*)sender();
  // fill popup with option names
  menu->clear();
  if ( myOptions.operator->() ) {
    for ( int i = 0, nb = myOptions->length(); i < nb; ++i ) {
      QString name_value = myOptions[i].in();
      QString name = name_value.split( ":", QString::KeepEmptyParts )[0];
      menu->addAction( name );
    }
  }
}

void BLSURFPluginGUI_HypothesisCreator::onOptionChosenInPopup( QAction* a )
{
  myOptionTable->setFocus();
  QMenu* menu = (QMenu*)( a->parent() );
  
  int idx = menu->actions().indexOf( a );
  QString idStr = QString("%1").arg( idx );
  QString option = myOptions[idx].in();
  QString optionName = option.split( ":", QString::KeepEmptyParts )[0];

  // look for a row with optionName
  int row = 0, nbRows = myOptionTable->rowCount();
  for ( ; row < nbRows; ++row )
    if ( myOptionTable->item( row, OPTION_ID_COLUMN )->text() == idStr )
      break;
  // add a row if not found
  if ( row == nbRows ) {
    myOptionTable->setRowCount( row+1 );
    myOptionTable->setItem( row, OPTION_ID_COLUMN, new QTableWidgetItem( idStr ) );
    myOptionTable->item( row, OPTION_ID_COLUMN )->setFlags( 0 );
    myOptionTable->setItem( row, OPTION_NAME_COLUMN, new QTableWidgetItem( optionName ) );
    myOptionTable->item( row, OPTION_NAME_COLUMN )->setFlags( 0 );
    myOptionTable->setItem( row, OPTION_VALUE_COLUMN, new QTableWidgetItem( "" ) );
    myOptionTable->item( row, OPTION_VALUE_COLUMN )->setFlags( Qt::ItemIsSelectable | 
							       Qt::ItemIsEditable   | 
							       Qt::ItemIsEnabled );
    myOptionTable->resizeColumnToContents( OPTION_NAME_COLUMN );
  }
  myOptionTable->clearSelection();
  myOptionTable->scrollToItem( myOptionTable->item( row, OPTION_VALUE_COLUMN ) );
  //myOptionTable->item( row, OPTION_VALUE_COLUMN )->setSelected( true );
  myOptionTable->setCurrentCell( row, OPTION_VALUE_COLUMN );
  //myOptionTable->openPersistentEditor( myOptionTable->item( row, OPTION_VALUE_COLUMN ) );
}
    
void BLSURFPluginGUI_HypothesisCreator::onDeleteOption()
{
  // clear option values and remember selected row
  QList<int> selectedRows;
  QList<QTableWidgetItem*> selected = myOptionTable->selectedItems(); 
  QTableWidgetItem* item;
  foreach( item, selected ) {
    int row = item->row();
    if ( !selectedRows.contains( row ) ) {
      selectedRows.append( row );
      int id = myOptionTable->item( row, OPTION_ID_COLUMN )->text().toInt();
      if ( id >= 0 && id < myOptions->length() )
        myOptions[ id ] = myOptionTable->item( row, OPTION_NAME_COLUMN )->text().toLatin1().constData();
    }
  }
  qSort( selectedRows );
  QListIterator<int> it( selectedRows );
  it.toBack();
  while ( it.hasPrevious() )
    myOptionTable->removeRow( it.previous() );
}

 
QString BLSURFPluginGUI_HypothesisCreator::caption() const
{
  return tr( "BLSURF_TITLE" );
}

QPixmap BLSURFPluginGUI_HypothesisCreator::icon() const
{
  return SUIT_Session::session()->resourceMgr()->loadPixmap( "BLSURFPlugin", tr( "ICON_DLG_BLSURF_PARAMETERS") );
}

QString BLSURFPluginGUI_HypothesisCreator::type() const
{
  return tr( "BLSURF_HYPOTHESIS" );
}

QString BLSURFPluginGUI_HypothesisCreator::helpPage() const
{
  return "blsurf_hypo_page.html";
}
