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
//           Size maps developement: Nicolas GEIMER (OCC) & Gilles DAVID (EURIWARE)
// ---
//
#include "BLSURFPluginGUI_HypothesisCreator.h"

#include <SMESHGUI_Utils.h>
#include <SMESHGUI_HypothesesUtils.h>
#include <SMESHGUI_Dialog.h>

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
#include <QRadioButton>

#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Application.h> 
#include <SALOME_ListIO.hxx>
#include <SALOME_ListIteratorOfListIO.hxx>

#include <GEOM_Client.hxx>
#include <TopoDS_Shape.hxx>
#include <SMESH_Gen_i.hxx>
#include <boost/shared_ptr.hpp> 
#include <structmember.h>

// #include <GeomSelectionTools.h>
#define WITH_SIZE_BOUNDARIES

enum Topology {
    FromCAD,
    Process,
    Process2
  } ;

enum PhysicalMesh
  {
    DefaultSize = 0,
    PhysicalUserDefined,
    SizeMap
  };

enum GeometricMesh
  {
    DefaultGeom = 0,
    UserDefined
  };

enum {
  STD_TAB = 0,
  ADV_TAB,
  SMP_TAB,
  OPTION_ID_COLUMN = 0,
  OPTION_NAME_COLUMN,
  OPTION_VALUE_COLUMN,
  NB_COLUMNS,
  SMP_ENTRY_COLUMN = 0,
  SMP_NAME_COLUMN,
  SMP_SIZEMAP_COLUMN,
  SMP_NB_COLUMNS
};

enum {
  SMP_BTNS = 0,
//   SMP_ATTRACTOR_BTN,
//   SMP_SEPARATOR1,
  SMP_POINT_BTN,
  SMP_EDGE_BTN,
  SMP_SURFACE_BTN,
  SMP_SEPARATOR2,
  SMP_REMOVE_BTN,
};

/**************************************************
 Begin initialization Python structures and objects 
***************************************************/

typedef struct {
  PyObject_HEAD
  int softspace;
  std::string *out;
  } PyStdOut;

static void
PyStdOut_dealloc(PyStdOut *self)
{
  PyObject_Del(self);
}

static PyObject *
PyStdOut_write(PyStdOut *self, PyObject *args)
{
  char *c;
  int l;
  if (!PyArg_ParseTuple(args, "t#:write",&c, &l))
    return NULL;

  //std::cerr << c ;
  *(self->out)=*(self->out)+c;

  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef PyStdOut_methods[] = {
  {"write",  (PyCFunction)PyStdOut_write,  METH_VARARGS,
    PyDoc_STR("write(string) -> None")},
  {NULL,    NULL}   /* sentinel */
};

static PyMemberDef PyStdOut_memberlist[] = {
  {"softspace", T_INT,  offsetof(PyStdOut, softspace), 0,
   "flag indicating that a space needs to be printed; used by print"},
  {NULL} /* Sentinel */
};

static PyTypeObject PyStdOut_Type = {
  /* The ob_type field must be initialized in the module init function
   * to be portable to Windows without using C++. */
  PyObject_HEAD_INIT(NULL)
  0,                            /*ob_size*/
  "PyOut",                      /*tp_name*/
  sizeof(PyStdOut),             /*tp_basicsize*/
  0,                            /*tp_itemsize*/
  /* methods */
  (destructor)PyStdOut_dealloc, /*tp_dealloc*/
  0,                            /*tp_print*/
  0,                            /*tp_getattr*/
  0,                            /*tp_setattr*/
  0,                            /*tp_compare*/
  0,                            /*tp_repr*/
  0,                            /*tp_as_number*/
  0,                            /*tp_as_sequence*/
  0,                            /*tp_as_mapping*/
  0,                            /*tp_hash*/
  0,                            /*tp_call*/
  0,                            /*tp_str*/
  PyObject_GenericGetAttr,      /*tp_getattro*/
  /* softspace is writable:  we must supply tp_setattro */
  PyObject_GenericSetAttr,      /* tp_setattro */
  0,                            /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT,           /*tp_flags*/
  0,                            /*tp_doc*/
  0,                            /*tp_traverse*/
  0,                            /*tp_clear*/
  0,                            /*tp_richcompare*/
  0,                            /*tp_weaklistoffset*/
  0,                            /*tp_iter*/
  0,                            /*tp_iternext*/
  PyStdOut_methods,             /*tp_methods*/
  PyStdOut_memberlist,          /*tp_members*/
  0,                            /*tp_getset*/
  0,                            /*tp_base*/
  0,                            /*tp_dict*/
  0,                            /*tp_descr_get*/
  0,                            /*tp_descr_set*/
  0,                            /*tp_dictoffset*/
  0,                            /*tp_init*/
  0,                            /*tp_alloc*/
  0,                            /*tp_new*/
  0,                            /*tp_free*/
  0,                            /*tp_is_gc*/
};

PyObject * newPyStdOut( std::string& out )
{
  PyStdOut *self;
  self = PyObject_New(PyStdOut, &PyStdOut_Type);
  if (self == NULL)
    return NULL;
  self->softspace = 0;
  self->out=&out;
  return (PyObject*)self;
}

/*************************************************
End initialization Python structures and objects 
**************************************************/

/**
 * \brief {BLSURFPluginGUI_HypothesisCreator constructor}
 * @param theHypType Name of the hypothesis type (here BLSURF_Parameters)
 * 
 * */
BLSURFPluginGUI_HypothesisCreator::BLSURFPluginGUI_HypothesisCreator( const QString& theHypType )
  : SMESHGUI_GenericHypothesisCreator( theHypType )
{
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::BLSURFPluginGUI_HypothesisCreator"); 
  this->mySMPMap.clear();

  GeomToolSelected = NULL;
  GeomToolSelected = getGeomSelectionTool();

  aSel = GeomToolSelected->selectionMgr();

  /* Initialize the Python interpreter */
  if (not Py_IsInitialized())
    throw ("Error: Python interpreter is not initialized");
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();
  
  main_mod = NULL;
  main_mod = PyImport_AddModule("__main__");
  
  main_dict = NULL;
  main_dict = PyModule_GetDict(main_mod);
  
  PyRun_SimpleString("from math import *");
  PyGILState_Release(gstate);

}

BLSURFPluginGUI_HypothesisCreator::~BLSURFPluginGUI_HypothesisCreator()
{
}

/**
 * \brief {Get or create the geom selection tool for active study}
 * */
GeomSelectionTools* BLSURFPluginGUI_HypothesisCreator::getGeomSelectionTool()
{
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::getGeomSelectionTool"); 
  BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;
  _PTR(Study) aStudy = SMESH::GetActiveStudyDocument();
  if (that->GeomToolSelected == NULL or that->GeomToolSelected->getMyStudy() != aStudy) {
    MESSAGE("GeomToolSelected is created");
    that->GeomToolSelected = new GeomSelectionTools(aStudy);
  }
  else
    MESSAGE("GeomToolSelected already exists");
  return that->GeomToolSelected;
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
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::checkParams"); 
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

  // SizeMap
  if ( ok )
  {
    mySizeMapTable->setFocus();
    QApplication::instance()->processEvents();

    BLSURFPlugin::BLSURFPlugin_Hypothesis_var h = BLSURFPlugin::BLSURFPlugin_Hypothesis::_narrow( initParamsHypothesis() );
    BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;

    int row = 0, nbRows = mySizeMapTable->rowCount();
    for ( ; row < nbRows; ++row )
    {
      QString entry   = mySizeMapTable->item( row, SMP_ENTRY_COLUMN )->text();
      QString sizeMap = mySizeMapTable->item( row, SMP_SIZEMAP_COLUMN )->text().trimmed();
      if ( !sizeMap.isEmpty() ) {
        if (that->sizeMapValidationFromRow(row))
        {
          try {
            const char* e = entry.toLatin1().constData();
            const char* s = that->mySMPMap[entry].toLatin1().constData();
            h->SetSizeMapEntry( e, s );
          }
          catch ( const SALOME::SALOME_Exception& ex )
          {
            SUIT_MessageBox::critical( dlg(),
                                       tr("SMESH_ERROR"),
                                       ex.details.text.in() );
            ok = false;
          }
        }
        else {
          ok = false;
        }
      }
    }
  }

  return ok;
}

QFrame* BLSURFPluginGUI_HypothesisCreator::buildFrame()
{
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::buildFrame");
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
  physicalTypes << tr( "BLSURF_DEFAULT_USER" ) << tr( "BLSURF_CUSTOM_USER" ) << tr( "BLSURF_SIZE_MAP");
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

  // Size Maps parameters
  
  mySmpGroup = new QWidget();
  QGridLayout* anSmpLayout = new QGridLayout(mySmpGroup);

  mySizeMapTable = new QTableWidget( 0, SMP_NB_COLUMNS, mySmpGroup );
  anSmpLayout->addWidget(mySizeMapTable, 1, 0, 8, 1);
  QStringList sizeMapHeaders;
  sizeMapHeaders << tr( "SMP_ENTRY_COLUMN" )<< tr( "SMP_NAME_COLUMN" ) << tr( "SMP_SIZEMAP_COLUMN" ); 
  mySizeMapTable->setHorizontalHeaderLabels(sizeMapHeaders);
  mySizeMapTable->horizontalHeader()->hideSection( SMP_ENTRY_COLUMN );
  mySizeMapTable->resizeColumnToContents(SMP_NAME_COLUMN);
  mySizeMapTable->resizeColumnToContents(SMP_SIZEMAP_COLUMN);
  mySizeMapTable->setAlternatingRowColors(true);
  mySizeMapTable->verticalHeader()->hide();

/*
  addAttractorButton = new QPushButton(tr("BLSURF_SM_ATTRACTOR"),mySmpGroup);
  anSmpLayout->addWidget(addAttractorButton, SMP_ATTRACTOR_BTN, 1, 1, 1);

  QFrame *line = new QFrame(mySmpGroup);
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  anSmpLayout->addWidget(line, SMP_SEPARATOR1, 1, 1, 1);
*/
  addSurfaceButton = new QPushButton(tr("BLSURF_SM_SURFACE"),mySmpGroup);
  anSmpLayout->addWidget(addSurfaceButton, SMP_SURFACE_BTN, 1, 1, 1);

  addEdgeButton = new QPushButton(tr("BLSURF_SM_EDGE"),mySmpGroup);
  anSmpLayout->addWidget(addEdgeButton, SMP_EDGE_BTN, 1, 1, 1);

  addPointButton = new QPushButton(tr("BLSURF_SM_POINT"),mySmpGroup);
  anSmpLayout->addWidget(addPointButton, SMP_POINT_BTN, 1, 1, 1);

  QFrame *line2 = new QFrame(mySmpGroup);
  line2->setFrameShape(QFrame::HLine);
  line2->setFrameShadow(QFrame::Sunken);
  anSmpLayout->addWidget(line2, SMP_SEPARATOR2, 1, 1, 1);

  removeButton = new QPushButton(tr("BLSURF_SM_REMOVE"),mySmpGroup);
  anSmpLayout->addWidget(removeButton, SMP_REMOVE_BTN, 1, 1, 1);
  

  // ---
  tab->insertTab( STD_TAB, myStdGroup, tr( "SMESH_ARGUMENTS" ) );
  tab->insertTab( ADV_TAB, myAdvGroup, tr( "BLSURF_ADV_ARGS" ) );
  tab->insertTab( SMP_TAB, mySmpGroup, tr( "BLSURF_SIZE_MAP" ) );

  tab->setCurrentIndex( STD_TAB );

  // ---
  connect( myGeometricMesh, SIGNAL( activated( int ) ), this, SLOT( onGeometricMeshChanged() ) );
  connect( myPhysicalMesh,  SIGNAL( activated( int ) ), this, SLOT( onPhysicalMeshChanged() ) );
  connect( addBtn->menu(),  SIGNAL( aboutToShow() ),    this, SLOT( onAddOption() ) );
  connect( addBtn->menu(),  SIGNAL( triggered( QAction* ) ), this, SLOT( onOptionChosenInPopup( QAction* ) ) );
  connect( rmBtn,           SIGNAL( clicked()),         this, SLOT( onDeleteOption() ) );
  
  connect(addSurfaceButton, SIGNAL(clicked()), this, SLOT(onAddMapOnSurface()));
  connect(addEdgeButton, SIGNAL(clicked()), this, SLOT(onAddMapOnEdge()));
  connect(addPointButton, SIGNAL(clicked()), this, SLOT(onAddMapOnPoint()));
  connect(removeButton, SIGNAL(clicked()), this, SLOT(onRemoveMap()));
  connect(mySizeMapTable, SIGNAL(cellChanged ( int, int  )),this,SLOT (onSetSizeMap(int,int )));

  return fr;
}

void BLSURFPluginGUI_HypothesisCreator::retrieveParams() const
{
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::retrieveParams");
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

  printf("retrieveParams():that->mySMPMap.size()=%d\n",that->mySMPMap.size());
  QMapIterator<QString, QString> i(that->mySMPMap);
  GeomSelectionTools* myGeomToolSelected = that->getGeomSelectionTool();
  while (i.hasNext()) {
    i.next();
    const QString entry = i.key();
    string shapeName = myGeomToolSelected->getNameFromEntry(entry.toStdString());
    const QString sizeMap = i.value();
    int row = mySizeMapTable->rowCount();
    mySizeMapTable->setRowCount( row+1 );
    mySizeMapTable->setItem( row, SMP_ENTRY_COLUMN, new QTableWidgetItem( entry ) );
    mySizeMapTable->item( row, SMP_ENTRY_COLUMN )->setFlags( 0 );
    mySizeMapTable->setItem( row, SMP_NAME_COLUMN, new QTableWidgetItem( QString::fromStdString(shapeName) ) );
    mySizeMapTable->item( row, SMP_NAME_COLUMN )->setFlags( 0 );
    mySizeMapTable->setItem( row, SMP_SIZEMAP_COLUMN, new QTableWidgetItem( sizeMap ) );
    mySizeMapTable->item( row, SMP_SIZEMAP_COLUMN )->setFlags( Qt::ItemIsSelectable |
                                                               Qt::ItemIsEditable   |
                                                               Qt::ItemIsEnabled );
    }
  
  mySizeMapTable->resizeColumnToContents( SMP_NAME_COLUMN );
  mySizeMapTable->resizeColumnToContents(SMP_SIZEMAP_COLUMN);

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
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::readParamsFromHypo");
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

  that->mySMPMap.clear();

  // classic size maps
  BLSURFPlugin::string_array_var mySizeMaps = h->GetSizeMapEntries();
  MESSAGE("mySizeMaps->length() = " << mySizeMaps->length());
  QString fullSizeMaps;
  QStringList fullSizeMapList;
  GeomSelectionTools* myGeomToolSelected = that->getGeomSelectionTool();
  for ( int i = 0;i<mySizeMaps->length(); ++i ) {
    fullSizeMaps =  mySizeMaps[i].in();
    MESSAGE("fullSizeMaps: " << fullSizeMaps.toStdString());
    fullSizeMapList = fullSizeMaps.split( "|", QString::KeepEmptyParts );
    if ( fullSizeMapList.count() > 1 ) {
      string fullSizeMap = fullSizeMapList[1].toStdString();
      int pos = fullSizeMap.find("return")+7;
      QString sizeMap = QString::fromStdString(fullSizeMap.substr(pos, fullSizeMap.size()-pos));
      that->mySMPMap[fullSizeMapList[0]] = sizeMap;
      MESSAGE("mySMPMap[" << fullSizeMapList[0].toStdString() << "] = " << sizeMap.toStdString());
      that->mySMPShapeTypeMap[fullSizeMapList[0]] = myGeomToolSelected->entryToShape(fullSizeMapList[0].toStdString()).ShapeType();
      MESSAGE("mySMPShapeTypeMap[" << fullSizeMapList[0].toStdString() << "] = " << that->mySMPShapeTypeMap[fullSizeMapList[0]]);
    }
  }

  // custom size maps
/*
  BLSURFPlugin::string_array_var myCustomSizeMaps = h->GetCustomSizeMapEntries();
  MESSAGE("myCustomSizeMaps->length() = " << myCustomSizeMaps->length());

  for ( int i = 0;i<myCustomSizeMaps->length(); ++i ) {
    QString fullCustomSizeMaps =  myCustomSizeMaps[i].in();
    QStringList fullCustomSizeMapList = fullCustomSizeMaps.split( "|", QString::KeepEmptyParts );
    if ( fullCustomSizeMapList.count() > 1 ) {
      that->mySMPMap[fullCustomSizeMapList[0]] = fullCustomSizeMapList[1];
      that->mySMPShapeTypeMap[fullCustomSizeMapList[0]] = GeomToolSelected->entryToShape(fullCustomSizeMapList[0].toStdString()).ShapeType();
      MESSAGE("mySMPMap[" << fullCustomSizeMapList[0].toStdString() << "] = " << fullCustomSizeMapList[1].toStdString());
      MESSAGE("mySMPShapeTypeMap[" << fullCustomSizeMapList[0].toStdString() << "] = " << that->mySMPShapeTypeMap[fullCustomSizeMapList[0]]);
    }
  }
*/
  // attractor
  BLSURFPlugin::string_array_var allMyAttractors = h->GetAttractorEntries();
  MESSAGE("myAttractors->length() = " << allMyAttractors->length());

  for ( int i = 0;i<allMyAttractors->length(); ++i ) {
    QString myAttractors =  allMyAttractors[i].in();
    QStringList myAttractorList = myAttractors.split( "|", QString::KeepEmptyParts );
    if ( myAttractorList.count() > 1 ) {
      that->mySMPMap[myAttractorList[0]] = myAttractorList[1];
      that->mySMPShapeTypeMap[myAttractorList[0]] = myGeomToolSelected->entryToShape(myAttractorList[0].toStdString()).ShapeType();
      MESSAGE("mySMPMap[" << myAttractorList[0].toStdString() << "] = " << myAttractorList[1].toStdString());
      MESSAGE("mySMPShapeTypeMap[" << myAttractorList[0].toStdString() << "] = " << that->mySMPShapeTypeMap[myAttractorList[0]]);
    }
  }

  return true;
}

bool BLSURFPluginGUI_HypothesisCreator::storeParamsToHypo( const BlsurfHypothesisData& h_data ) const
{
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::storeParamsToHypo");
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

    if( ((int) h_data.myPhysicalMesh == PhysicalUserDefined)||((int) h_data.myPhysicalMesh == SizeMap) ) {
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

    //printf("storeParamsToHypo():myOptions->length()=%d\n",myOptions->length());
    h->SetOptionValues( myOptions ); // is set in checkParams()

    BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;
    QMapIterator<QString,QString> i(that->mySMPMap);
    // Iterate over each size map
    while (i.hasNext()) {
      i.next();
      const QString entry = i.key();
      const QString sizeMap = i.value();

      if (sizeMap == "__TO_DELETE__") {
        MESSAGE("Delete entry " << entry.toStdString() << " from engine");
        h->UnsetEntry(entry.toLatin1().constData());
      }
      else if (sizeMap.startsWith("ATTRACTOR")) {
        MESSAGE("SetAttractorEntry(" << entry.toStdString() << ")= " << sizeMap.toStdString());
        h->SetAttractorEntry( entry.toLatin1().constData(), sizeMap.toLatin1().constData() );
      }
      else if (sizeMap.startsWith("def")) {
        MESSAGE("SetCustomSizeMapEntry(" << entry.toStdString() << ")= " << sizeMap.toStdString());
//        h->SetCustomSizeMapEntry( entry.toLatin1().constData(), sizeMap.toLatin1().constData() );
      }
      else {
        QString fullSizeMap;
        fullSizeMap = QString("");
        if (that->mySMPShapeTypeMap[entry]  == TopAbs_FACE)
          fullSizeMap = QString("def f(u,v): return ") + sizeMap;
        else if (that->mySMPShapeTypeMap[entry]  == TopAbs_EDGE)
          fullSizeMap = QString("def f(t): return ") + sizeMap;
        else if (that->mySMPShapeTypeMap[entry] == TopAbs_VERTEX)
          fullSizeMap = QString("def f(): return ") + sizeMap;

        MESSAGE("SetSizeMapEntry("<<entry.toStdString()<<") = " <<fullSizeMap.toStdString());
        h->SetSizeMapEntry( entry.toLatin1().constData(), fullSizeMap.toLatin1().constData() );
      }
    }
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
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::readParamsFromWidgets");
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
  
  // SizeMap
  row = 0, nbRows = mySizeMapTable->rowCount();
  for ( ; row < nbRows; ++row )
  {
      QString entry   = mySizeMapTable->item( row, SMP_ENTRY_COLUMN )->text();
      if ( that->mySMPMap.contains(entry) )
        guiHyp += entry + " = " + that->mySMPMap[entry] + "; ";
  }
  
  MESSAGE("guiHyp : " << guiHyp.toLatin1().data());

  return guiHyp;
}

void BLSURFPluginGUI_HypothesisCreator::onPhysicalMeshChanged() {
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::onPhysicalMeshChanged");
  bool isCustom = ((myPhysicalMesh->currentIndex() == PhysicalUserDefined) || (myPhysicalMesh->currentIndex() == SizeMap)) ;
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
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::onGeometricMeshChanged");
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

// **********************
// *** BEGIN SIZE MAP ***
// **********************


void BLSURFPluginGUI_HypothesisCreator::onRemoveMap()
{
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::onRemoveMap()");
  QList<int> selectedRows;
  QList<QTableWidgetItem*> selected = mySizeMapTable->selectedItems();
  QTableWidgetItem* item;
  int row;
  foreach( item, selected ) {
    row = item->row();
    if ( !selectedRows.contains( row ) ) 
      selectedRows.append( row );
  }

  BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;

  qSort( selectedRows );
  QListIterator<int> it( selectedRows );
  it.toBack();
  while ( it.hasPrevious() ) {
      row = it.previous();
      QString entry = mySizeMapTable->item(row,SMP_ENTRY_COLUMN)->text();
      if (that->mySMPMap.contains(entry))
        that->mySMPMap[entry] = "__TO_DELETE__";
      if (that->mySMPShapeTypeMap.contains(entry))
        that->mySMPShapeTypeMap.remove(entry);
      mySizeMapTable->removeRow(row );
  }
  mySizeMapTable->resizeColumnToContents(SMP_NAME_COLUMN);
  mySizeMapTable->resizeColumnToContents(SMP_SIZEMAP_COLUMN);
}

void BLSURFPluginGUI_HypothesisCreator::onSetSizeMap(int row,int col)
{
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::onSetSizeMap("<< row << "," << col << ")");
  if (col == SMP_SIZEMAP_COLUMN) {
    BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;
    QString entry   = that->mySizeMapTable->item(row, SMP_ENTRY_COLUMN)->text();
    QString sizeMap = that->mySizeMapTable->item(row, SMP_SIZEMAP_COLUMN)->text().trimmed();
    MESSAGE("entry: " << entry.toStdString() << ", sizeMap: " << sizeMap.toStdString());
    if (not that->mySMPShapeTypeMap.contains(entry))
      return;
    if (that->mySMPMap.contains(entry))
      if (that->mySMPMap[entry] == sizeMap)
        return;
    QColor* bgColor = new QColor("white");
    QColor* fgColor = new QColor("black");
    if (not sizeMap.isEmpty()) {
      that->mySMPMap[entry] = sizeMap;
      if (not sizeMapValidationFromRow(row)) {
        bgColor->setRgb(255,0,0);
        fgColor->setRgb(255,255,255);
      }
    }
    else {
      MESSAGE("Size map empty: reverse to precedent value" );
      that->mySizeMapTable->item(row, SMP_SIZEMAP_COLUMN)->setText(that->mySMPMap[entry]);
    }
    that->mySizeMapTable->item(row, SMP_NAME_COLUMN)->setBackground(QBrush(*bgColor));
    that->mySizeMapTable->item(row, SMP_SIZEMAP_COLUMN)->setBackground(QBrush(*bgColor));
    that->mySizeMapTable->item(row, SMP_NAME_COLUMN)->setForeground(QBrush(*fgColor));
    that->mySizeMapTable->item(row, SMP_SIZEMAP_COLUMN)->setForeground(QBrush(*fgColor));
    mySizeMapTable->resizeColumnToContents(SMP_SIZEMAP_COLUMN);
  }
}

void BLSURFPluginGUI_HypothesisCreator::onAddMapOnSurface()
{
 insertElementType(TopAbs_FACE);
}

void BLSURFPluginGUI_HypothesisCreator::onAddMapOnEdge()
{
 insertElementType(TopAbs_EDGE);
}

void BLSURFPluginGUI_HypothesisCreator::onAddMapOnPoint()
{
 insertElementType(TopAbs_VERTEX);
}

void BLSURFPluginGUI_HypothesisCreator::insertElementType(TopAbs_ShapeEnum typeShapeAsked)
{
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::insertElementType()");

  BLSURFPlugin::BLSURFPlugin_Hypothesis_var h =
    BLSURFPlugin::BLSURFPlugin_Hypothesis::_narrow( initParamsHypothesis());

  BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;

  TopoDS_Shape S;
  string entry, shapeName;
//  LightApp_SelectionMgr* aSel = GeomToolSelected->selectionMgr();

  SALOME_ListIO ListSelectedObjects;
  aSel->selectedObjects(ListSelectedObjects, NULL, false );
  if (!ListSelectedObjects.IsEmpty())
  {
    SALOME_ListIteratorOfListIO Object_It(ListSelectedObjects);
    GeomSelectionTools* myGeomToolSelected = getGeomSelectionTool();
    for (; Object_It.More(); Object_It.Next()) 
    {
      Handle(SALOME_InteractiveObject) anObject = Object_It.Value();
      entry     = myGeomToolSelected->getEntryOfObject(anObject);
      shapeName = anObject->getName();
      S         = myGeomToolSelected->entryToShape(entry);
      if ((! S.IsNull()) && (S.ShapeType() == typeShapeAsked)) 
      { 
        mySizeMapTable->setFocus();
        QString shapeEntry;
        shapeEntry = QString::fromStdString(entry);
        double phySize = h->GetPhySize();
        std::ostringstream oss;
        oss << phySize;
        QString sizeMap;
        sizeMap  = QString::fromStdString(oss.str());
        if (that->mySMPMap.contains(shapeEntry)) {
          if (that->mySMPMap[shapeEntry] != "__TO_DELETE__") {
            MESSAGE("Size map for shape with name(entry): "<< shapeName << "(" << entry << ")");
            break;
          }
        }
        that->mySMPMap[shapeEntry] = sizeMap;
        that->mySMPShapeTypeMap[shapeEntry] = typeShapeAsked; 
        int row = mySizeMapTable->rowCount() ;
        mySizeMapTable->setRowCount( row+1 );
        mySizeMapTable->setItem( row, SMP_ENTRY_COLUMN, new QTableWidgetItem( shapeEntry ) );
        mySizeMapTable->item( row, SMP_ENTRY_COLUMN )->setFlags( 0 );
        mySizeMapTable->setItem( row, SMP_NAME_COLUMN, new QTableWidgetItem( QString::fromStdString(shapeName) ) );
        mySizeMapTable->item( row, SMP_NAME_COLUMN )->setFlags( 0 );
        mySizeMapTable->setItem( row, SMP_SIZEMAP_COLUMN, new QTableWidgetItem( sizeMap ) );
        mySizeMapTable->item( row, SMP_SIZEMAP_COLUMN )->setFlags( Qt::ItemIsSelectable |Qt::ItemIsEditable   |Qt::ItemIsEnabled );
        mySizeMapTable->resizeColumnToContents( SMP_NAME_COLUMN );
        mySizeMapTable->resizeColumnToContents(SMP_SIZEMAP_COLUMN);
        mySizeMapTable->clearSelection();
        mySizeMapTable->scrollToItem( mySizeMapTable->item( row, SMP_SIZEMAP_COLUMN ) );

        if ( myPhysicalMesh->currentIndex() != SizeMap ) {
          myPhysicalMesh->setCurrentIndex( SizeMap );
          onPhysicalMeshChanged();
        }
      }
    }
  }
}

bool BLSURFPluginGUI_HypothesisCreator::sizeMapsValidation()
{
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::sizeMapsValidation()");
  int row = 0, nbRows = mySizeMapTable->rowCount();
  for ( ; row < nbRows; ++row )
    if (not sizeMapValidationFromRow(row))
      return false;
  return true;
}

bool BLSURFPluginGUI_HypothesisCreator::sizeMapValidationFromRow(int myRow, bool displayError)
{
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::sizeMapValidationFromRow()");
  QString myEntry   = mySizeMapTable->item( myRow, SMP_ENTRY_COLUMN )->text();
  bool res = sizeMapValidationFromEntry(myEntry,displayError);
  mySizeMapTable->setFocus();
  return res; 
}

bool BLSURFPluginGUI_HypothesisCreator::sizeMapValidationFromEntry(QString myEntry, bool displayError)
{
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::sizeMapValidationFromEntry()");

  BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;
     
  if (not that->mySMPMap.contains(myEntry)) {
    MESSAGE("Geometry with entry "<<myEntry.toStdString()<<" was not found.");
    return false;
  }
  if (not that->mySMPShapeTypeMap.contains(myEntry)) {
    MESSAGE("Shape type with entry "<<myEntry.toStdString()<<" was not found.");
    return false;
  }

  string expr;

  if (that->mySMPMap[myEntry].startsWith("def")) {
    MESSAGE("custom function" );
    expr = that->mySMPMap[myEntry].toStdString();
  }
  else if (that->mySMPMap[myEntry].startsWith("ATTRACTOR")) {
    MESSAGE("Attractor" );
//    if ((that->mySMPMap[myEntry].count(QRegExp("ATTRACTOR([0-9])")) != 1))
    if ((that->mySMPMap[myEntry].count('(') != 1) or 
        (that->mySMPMap[myEntry].count(')') != 1) or
        (that->mySMPMap[myEntry].count(';') != 4) or
        (that->mySMPMap[myEntry].size() == 15)){
      if (displayError)
        SUIT_MessageBox::warning( dlg(),"Definition of attractor : Error" ,"An attractor is defined with the following pattern: ATTRACTOR(xa;ya;za;a;b)" );
      return false;
    }
    return true;
  }
  else {
    // case size map is empty
    if (that->mySMPMap[myEntry].isEmpty()) {
      if (displayError)
        SUIT_MessageBox::warning( dlg(),"Definition of size map : Error" , "Size map can't be empty");
      return false;
    }

    if ( that->mySMPShapeTypeMap[myEntry] == TopAbs_FACE)
      expr = "def f(u,v) : return " + that->mySMPMap[myEntry].toStdString();
    else if ( that->mySMPShapeTypeMap[myEntry] == TopAbs_EDGE)
      expr = "def f(t) : return " + that->mySMPMap[myEntry].toStdString();
    else if ( that->mySMPShapeTypeMap[myEntry] == TopAbs_VERTEX)
      expr = "def f() : return " + that->mySMPMap[myEntry].toStdString();
  }
  //assert(Py_IsInitialized());
  if (not Py_IsInitialized())
    throw ("Erreur: Python interpreter is not initialized");
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();
 
  PyObject * obj = NULL;   
  PyObject* new_stderr = NULL;   
  string  err_description="";
  obj= PyRun_String(expr.c_str(), Py_file_input, main_dict, NULL);
  if (obj == NULL){
    fflush(stderr);
    err_description="";
    new_stderr=newPyStdOut(err_description);
    PySys_SetObject("stderr", new_stderr);
    PyErr_Print();
    PySys_SetObject("stderr", PySys_GetObject("__stderr__"));
    Py_DECREF(new_stderr);
    if (displayError)
      SUIT_MessageBox::warning( dlg(),"Definition of Python Function : Error" ,err_description.c_str() );
    PyGILState_Release(gstate);
    return false;
  }
  Py_DECREF(obj);
   
  PyObject * func = NULL;
  func = PyObject_GetAttrString(main_mod, "f");
  if ( func == NULL){
    fflush(stderr);                                                                            
    err_description="";                                                                        
    new_stderr=newPyStdOut(err_description);                                         
    PySys_SetObject("stderr", new_stderr);                                                     
    PyErr_Print();                                                                             
    PySys_SetObject("stderr", PySys_GetObject("__stderr__"));                                  
    Py_DECREF(new_stderr);
    if (displayError)
      SUIT_MessageBox::warning( dlg(),"Python Error" ,err_description.c_str() );
    PyGILState_Release(gstate);
    return false;
  }

  PyGILState_Release(gstate);

  MESSAGE("SizeMap expression "<<expr<<" is valid");

  return true;
}

/*
void BLSURFPluginGUI_HypothesisCreator::OnEditMapFunction(QModelIndex* index) {
  int myRow = index->row();
  int myColumn = index->column();
  
  if (myColumn == 2){
     if (!myEditor) {
         myEditor = new BLSURFPluginGUI_MapFunctionEditor(sizeMapModel->item(myRow,0)->text());
	 connect(myEditor, SIGNAL(FunctionEntered(QString)), this, SLOT(FunctionLightValidation(QString)));
     }
     myEditor->exec();
//      myEditor->show();
//      myEditor->raise();
//      myEditor->activateWindow();
     

//     BLSURFPluginGUI_MapFunctionEditor* myEditor = new BLSURFPluginGUI_MapFunctionEditor(sizeMapModel->item(myRow,0)->text());
//     myEditor->exec();
     QString myFunction = myEditor->GetFunctionText();
     // FIN RECUPERATION FONCTION
     
     if (! myFunction.isEmpty()) {
     
     // MAJ DE LA MAP
     
     BLSURFPlugin::BLSURFPlugin_Hypothesis_var h = 
       BLSURFPlugin::BLSURFPlugin_Hypothesis::_narrow( initParamsHypothesis());

//     h->SetSizeMapEntry(sizeMapModel->item(myRow,1)->text().toLatin1().constData(),
//                        item->text().toLatin1().constData());
     h->SetSizeMapEntry(sizeMapModel->item(myRow,1)->text().toLatin1().constData(),
                        myFunction.toLatin1().constData());
     // FIN MAJ DE LA MAP
     }
  }
}*/
 
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

LightApp_SelectionMgr* BLSURFPluginGUI_HypothesisCreator::selectionMgr()
{

  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  if( anApp )
    return dynamic_cast<LightApp_SelectionMgr*>( anApp->selectionMgr() );
  else
    return 0;
}
