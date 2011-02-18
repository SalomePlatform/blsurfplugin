//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
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
// #include <DlgBlSurfHyp_Enforced.h>

#include "GeometryGUI.h"

#include <SMESHGUI_Utils.h>
#include <SMESHGUI_HypothesesUtils.h>
#include <SMESHGUI_Dialog.h>
#include "SMESHGUI_SpinBox.h"
#include "SMESH_NumberFilter.hxx"

#include <SUIT_Session.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Tools.h>

#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
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

#include <QStandardItemModel>
#include <QStandardItem>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QModelIndexList>

#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Application.h>
#include <SALOME_ListIO.hxx>
#include <SALOME_ListIteratorOfListIO.hxx>
#include "SALOME_LifeCycleCORBA.hxx"

#include <TopoDS_Shape.hxx>
#include <SMESH_Gen_i.hxx>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <structmember.h>
#include <stdexcept>

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
  ENF_TAB,
  OPTION_ID_COLUMN = 0,
  OPTION_NAME_COLUMN,
  OPTION_VALUE_COLUMN,
  NB_COLUMNS,
  SMP_ENTRY_COLUMN = 0,
  SMP_NAME_COLUMN,
  SMP_SIZEMAP_COLUMN,
  SMP_NB_COLUMNS,
// Enforced vertices array columns
  ENF_VER_NAME_COLUMN = 0,
  ENF_VER_FACE_ENTRY_COLUMN,
  ENF_VER_X_COLUMN,
  ENF_VER_Y_COLUMN,
  ENF_VER_Z_COLUMN,
  ENF_VER_ENTRY_COLUMN,
  ENF_VER_GROUP_COLUMN,
  ENF_VER_NB_COLUMNS
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
  SMP_NB_LINES
};

// Enforced vertices inputs
enum {
  ENF_VER_FACE = 0,
  ENF_VER_VERTEX,
  ENF_VER_X_COORD,
  ENF_VER_Y_COORD,
  ENF_VER_Z_COORD,
  ENF_VER_GROUP,
  ENF_VER_VERTEX_BTN,
  ENF_VER_REMOVE_BTN,
//   ENF_VER_SEPARATOR,
  ENF_VER_GROUP_CHECK,
  ENF_VER_NB_LINES
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
  {(char*)"softspace", T_INT,  offsetof(PyStdOut, softspace), 0,
   (char*)"flag indicating that a space needs to be printed; used by print"},
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


//
// BEGIN EnforcedTreeWidgetDelegate
//

EnforcedTreeWidgetDelegate::EnforcedTreeWidgetDelegate(QObject *parent)
  : QItemDelegate(parent)
{
}

QWidget *EnforcedTreeWidgetDelegate::createEditor(QWidget *parent,
                                              const QStyleOptionViewItem & option ,
                                              const QModelIndex & index ) const
{
  QModelIndex father = index.parent();
  QString entry = father.child(index.row(), ENF_VER_ENTRY_COLUMN).data(Qt::EditRole).toString();
  
  if (index.column() == ENF_VER_X_COLUMN || \
      index.column() == ENF_VER_Y_COLUMN || \
      index.column() == ENF_VER_Z_COLUMN)
  {
    SMESHGUI_SpinBox *editor = new SMESHGUI_SpinBox(parent);
    editor->RangeStepAndValidator(COORD_MIN, COORD_MAX, 10.0, "length_precision");
    editor->setReadOnly(!entry.isEmpty());
    editor->setDisabled(!entry.isEmpty());
    return editor;
  }
  else
  {
    QLineEdit *editor = new QLineEdit(parent);
    if (index.column() != ENF_VER_GROUP_COLUMN) {
      editor->setReadOnly(!entry.isEmpty());
      editor->setDisabled(!entry.isEmpty());
    }
    return editor;
  }

}

void EnforcedTreeWidgetDelegate::setEditorData(QWidget *editor,
                                           const QModelIndex &index) const
{
  QString value = index.model()->data(index, Qt::EditRole).toString();

  if (index.column() == ENF_VER_X_COLUMN ||
      index.column() == ENF_VER_Y_COLUMN ||
      index.column() == ENF_VER_Z_COLUMN)
  {
    SMESHGUI_SpinBox *lineEdit = static_cast<SMESHGUI_SpinBox*>(editor);
    lineEdit->setText(value);
  }
  else {
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    lineEdit->setText(value);
  }
}

void EnforcedTreeWidgetDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                          const QModelIndex &index) const
{
  QModelIndex parent = index.parent();
  QString entry = parent.child(index.row(), ENF_VER_ENTRY_COLUMN).data(Qt::EditRole).toString();
  if (index.column() == ENF_VER_X_COLUMN || index.column() == ENF_VER_Y_COLUMN || index.column() == ENF_VER_Z_COLUMN) {
    SMESHGUI_SpinBox *lineEdit = static_cast<SMESHGUI_SpinBox*>(editor);
    if (entry.isEmpty() && !vertexExists(model, index, lineEdit->GetString()))
      model->setData(index, lineEdit->GetValue(), Qt::EditRole);
  } else if (index.column() == ENF_VER_NAME_COLUMN) {
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    QString value = lineEdit->text();
    if (entry.isEmpty() && !vertexExists(model, index, value))
      model->setData(index, value, Qt::EditRole);
  } else if (index.column() == ENF_VER_ENTRY_COLUMN) {
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    QString value = lineEdit->text();
    if (! vertexExists(model, index, value))
      model->setData(index, value, Qt::EditRole);
  } else if (index.column() == ENF_VER_GROUP_COLUMN) {
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    model->setData(index, lineEdit->text(), Qt::EditRole);
  }
}

void EnforcedTreeWidgetDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
  editor->setGeometry(option.rect);
}

bool EnforcedTreeWidgetDelegate::vertexExists(QAbstractItemModel *model,
    const QModelIndex &index, QString value) const
{
  bool exists = false;
  QModelIndex parent = index.parent();
  int row = index.row();
  int col = index.column();

  if (parent.isValid() && !value.isEmpty()) {
    if (col == ENF_VER_X_COLUMN || col == ENF_VER_Y_COLUMN || col == ENF_VER_Z_COLUMN) {
      double x, y, z;
      if (col == ENF_VER_X_COLUMN) {
        x = value.toDouble();
        y = parent.child(row, ENF_VER_Y_COLUMN).data(Qt::EditRole).toDouble();
        z = parent.child(row, ENF_VER_Z_COLUMN).data(Qt::EditRole).toDouble();
      }
      if (col == ENF_VER_Y_COLUMN) {
        y = value.toDouble();
        x = parent.child(row, ENF_VER_X_COLUMN).data(Qt::EditRole).toDouble();
        z = parent.child(row, ENF_VER_Z_COLUMN).data(Qt::EditRole).toDouble();
      }
      if (col == ENF_VER_Z_COLUMN) {
        z = value.toDouble();
        x = parent.child(row, ENF_VER_X_COLUMN).data(Qt::EditRole).toDouble();
        y = parent.child(row, ENF_VER_Y_COLUMN).data(Qt::EditRole).toDouble();
      }
      int nbChildren = model->rowCount(parent);
      for (int i = 0 ; i < nbChildren ; i++) {
        if (i != row) {
          double childX = parent.child(i, ENF_VER_X_COLUMN).data(Qt::EditRole).toDouble();
          double childY = parent.child(i, ENF_VER_Y_COLUMN).data(Qt::EditRole).toDouble();
          double childZ = parent.child(i, ENF_VER_Z_COLUMN).data(Qt::EditRole).toDouble();
          if ((childX == x) && (childY == y) && (childZ == z)) {
            exists = true;
            break;
          }
        }
      }
    }
//     else if (col == ENF_VER_ENTRY_COLUMN) {
//       // Compare entries if no coords (= selected geom)
//       xString = parent.child(row, ENF_VER_X_COLUMN).data(Qt::EditRole).toString();
//       if (xString.isEmpty) {
//         int nbChildren = model->rowCount(parent);
//         for (int i = 0 ; i < nbChildren ; i++) {
//           if (i != row) {
//             QString childName = parent.child(i, ENF_VER_ENTRY_COLUMN).data(Qt::EditRole).toString();
//             if (childName == value) {
//               exists = true;
//               break;
//             }
//           }
//         }
//       }
//     }
    else if (col == ENF_VER_NAME_COLUMN) {
      int nbChildren = model->rowCount(parent);
      for (int i = 0 ; i < nbChildren ; i++) {
        if (i != row) {
          QString childName = parent.child(i, ENF_VER_NAME_COLUMN).data(Qt::EditRole).toString();
          if (childName == value) {
            exists = true;
            break;
          }
        }
      }
    }
  }

  return exists;
}

//
// END EnforcedTreeWidgetDelegate
//

//
// BEGIN BLSURFPluginGUI_ObjectReferenceParamWdg
//
//================================================================================

// BLSURFPluginGUI_ObjectReferenceParamWdg::BLSURFPluginGUI_ObjectReferenceParamWdg
// ( SUIT_SelectionFilter* f, QWidget* parent, bool multiSelection)
//   : StdMeshersGUI_ObjectReferenceParamWdg(f, parent, multiSelection)
// {
//   init();
// }
// 
// 
// BLSURFPluginGUI_ObjectReferenceParamWdg::BLSURFPluginGUI_ObjectReferenceParamWdg
// ( MeshObjectType objType, QWidget* parent, bool multiSelection )
//   : StdMeshersGUI_ObjectReferenceParamWdg( objType, parent, multiSelection )
// {
//   init();
// }
// 
// BLSURFPluginGUI_ObjectReferenceParamWdg::~BLSURFPluginGUI_ObjectReferenceParamWdg()
// {
//   if ( myFilter )
//   {
//     mySelectionMgr->removeFilter( myFilter );
//     delete myFilter;
//   }
// }
// 
// void BLSURFPluginGUI_ObjectReferenceParamWdg::init()
// {
//   StdMeshersGUI_ObjectReferenceParamWdg::init();
//   disconnect( mySelButton, SIGNAL(clicked()), SLOT(activateSelection()));
//   connect( mySelButton, SIGNAL(toggled(bool)), SLOT(setActivationStatus(bool)));
// }
// 
// void BLSURFPluginGUI_ObjectReferenceParamWdg::setActivationStatus(bool status)
// {
//   if (status)
//     activateSelection();
//   else
//     deactivateSelection();
// }
// 
// void BLSURFPluginGUI_ObjectReferenceParamWdg::activateSelectionOnly()
// {
//   if ( !mySelectionActivated && mySelectionMgr )
//   {
//     mySelectionActivated = true;
//     mySelectionMgr->clearFilters();
//     if ( SVTK_ViewWindow* aViewWindow = SMESH::GetViewWindow( mySMESHGUI ))
//       aViewWindow->SetSelectionMode(ActorSelection);
//     if ( myFilter )
//       mySelectionMgr->installFilter( myFilter );
//     connect(mySelectionMgr, SIGNAL(currentSelectionChanged()), SLOT(onSelectionDone()));
//   }
//   emit selectionActivated();
// }
// 
// void BLSURFPluginGUI_ObjectReferenceParamWdg::deactivateSelectionOnly()
// {
//   mySelectionActivated = false;
//   disconnect(mySelectionMgr, 0, this, 0 );
//   mySelectionMgr->removeFilter( myFilter );
// }
// 
//
// END BLSURFPluginGUI_ObjectReferenceParamWdg
//

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
  if (! Py_IsInitialized())
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
  BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;
  _PTR(Study) aStudy = SMESH::GetActiveStudyDocument();
  if (that->GeomToolSelected == NULL || that->GeomToolSelected->getMyStudy() != aStudy) {
    that->GeomToolSelected = new GeomSelectionTools(aStudy);
  }
  return that->GeomToolSelected;
}

GEOM::GEOM_Gen_var BLSURFPluginGUI_HypothesisCreator::getGeomEngine()
{
  return GeometryGUI::GetGeomGen();
}


bool BLSURFPluginGUI_HypothesisCreator::checkParams() const
{
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::checkParams");
  bool ok = true;

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

  // SizeMap and attractors
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

  // Enforced vertices
  // TODO

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

  myName = 0;
  if( isCreation() )
    myName = new QLineEdit( myStdGroup );

  myGradation = new SMESHGUI_SpinBox( myStdGroup );
  myGradation->RangeStepAndValidator(1.1, 2.5, 0.1, "length_precision");

  myPhysicalMesh = new QComboBox( myStdGroup );
  QStringList physicalTypes;
  physicalTypes << tr( "BLSURF_DEFAULT_USER" ) << tr( "BLSURF_CUSTOM_USER" ) << tr( "BLSURF_SIZE_MAP");
  myPhysicalMesh->addItems( physicalTypes );

  myPhySize = new SMESHGUI_SpinBox( myStdGroup );
  myPhySize->RangeStepAndValidator(0, COORD_MAX, 10.0, "length_precision");

#ifdef WITH_SIZE_BOUNDARIES
  myPhyMin = new SMESHGUI_SpinBox( myStdGroup );
  myPhyMin->RangeStepAndValidator(0, COORD_MAX, 10.0, "length_precision");
  myPhyMin->setText("");
  myPhyMax = new SMESHGUI_SpinBox( myStdGroup );
  myPhyMax->RangeStepAndValidator(0, COORD_MAX, 10.0, "length_precision");
  myPhyMax->setText("");
#endif

  myGeometricMesh = new QComboBox( myStdGroup );
  QStringList types;
  types << tr( "BLSURF_DEFAULT_GEOM" ) << tr( "BLSURF_CUSTOM_GEOM" );
  myGeometricMesh->addItems( types );

  myAngleMeshS = new SMESHGUI_SpinBox( myStdGroup );
  myAngleMeshS->RangeStepAndValidator(0, 16, 0.5, "angular_precision");

  myAngleMeshC = new SMESHGUI_SpinBox( myStdGroup );
  myAngleMeshC->RangeStepAndValidator(0, 16, 0.5, "angular_precision");

#ifdef WITH_SIZE_BOUNDARIES
  myGeoMin = new SMESHGUI_SpinBox( myStdGroup );
  myGeoMin->RangeStepAndValidator(0, COORD_MAX, 10.0, "length_precision");
  myGeoMin->setText("");
  myGeoMax = new SMESHGUI_SpinBox( myStdGroup );
  myGeoMax->RangeStepAndValidator(0, COORD_MAX, 10.0, "length_precision");
  myGeoMax->setText("");
#endif
  myAllowQuadrangles = new QCheckBox( tr( "BLSURF_ALLOW_QUADRANGLES" ), myStdGroup );
  myDecimesh = new QCheckBox( tr( "BLSURF_DECIMESH" ), myStdGroup );

  // ADD WIDGETS (STANDARD TAB)
  int row = 0;
  if( isCreation() ) {
    aStdLayout->addWidget( new QLabel( tr( "SMESH_NAME" ), myStdGroup ),        row, 0, 1, 1 );
    aStdLayout->addWidget( myName,                                              row++, 1, 1, 1 );
  }
  aStdLayout->addWidget( new QLabel( tr( "BLSURF_GRADATION" ), myStdGroup ),    row, 0, 1, 1 );
  aStdLayout->addWidget( myGradation,                                           row++, 1, 1, 1 );
  aStdLayout->addWidget( new QLabel( tr( "BLSURF_PHY_MESH" ), myStdGroup ),     row, 0, 1, 1 );
  aStdLayout->addWidget( myPhysicalMesh,                                        row++, 1, 1, 1 );
  aStdLayout->addWidget( new QLabel( tr( "BLSURF_HPHYDEF" ), myStdGroup),       row, 0, 1, 1 );
  aStdLayout->addWidget( myPhySize,                                             row++, 1, 1, 1 );
#ifdef WITH_SIZE_BOUNDARIES
  aStdLayout->addWidget( new QLabel( tr( "BLSURF_HPHYMIN" ), myStdGroup ),      row, 0, 1, 1 );
  aStdLayout->addWidget( myPhyMin,                                              row++, 1, 1, 1 );
  aStdLayout->addWidget( new QLabel( tr( "BLSURF_HPHYMAX" ), myStdGroup ),      row, 0, 1, 1 );
  aStdLayout->addWidget( myPhyMax,                                              row++, 1, 1, 1 );
#endif
  aStdLayout->addWidget( new QLabel( tr( "BLSURF_GEOM_MESH" ), myStdGroup ),    row, 0, 1, 1 );
  aStdLayout->addWidget( myGeometricMesh,                                       row++, 1, 1, 1 );
  aStdLayout->addWidget( new QLabel( tr( "BLSURF_ANGLE_MESH_S" ), myStdGroup ), row, 0, 1, 1 );
  aStdLayout->addWidget( myAngleMeshS,                                          row++, 1, 1, 1 );
  aStdLayout->addWidget( new QLabel( tr( "BLSURF_ANGLE_MESH_C" ), myStdGroup ), row, 0, 1, 1 );
  aStdLayout->addWidget( myAngleMeshC,                                          row++, 1, 1, 1 );
#ifdef WITH_SIZE_BOUNDARIES
  aStdLayout->addWidget( new QLabel( tr( "BLSURF_HGEOMIN" ), myStdGroup ),      row, 0, 1, 1 );
  aStdLayout->addWidget( myGeoMin,                                              row++, 1, 1, 1 );
  aStdLayout->addWidget( new QLabel( tr( "BLSURF_HGEOMAX" ), myStdGroup ),      row, 0, 1, 1 );
  aStdLayout->addWidget( myGeoMax,                                              row++, 1, 1, 1 );
#endif
  aStdLayout->addWidget( myAllowQuadrangles,                                    row++, 0, 1, 2 );
  aStdLayout->addWidget( myDecimesh,                                            row++, 0, 1, 2 );

  // advanced parameters
  myAdvGroup = new QWidget();
  QGridLayout* anAdvLayout = new QGridLayout( myAdvGroup );
  anAdvLayout->setSpacing( 6 );
  anAdvLayout->setMargin( 11 );
  anAdvLayout->setRowStretch( 4, 5 );
  anAdvLayout->setColumnStretch( 1, 5 );

  myTopology = new QComboBox( myAdvGroup );
  QStringList topologyTypes;
  topologyTypes << tr( "BLSURF_TOPOLOGY_CAD" ) << tr( "BLSURF_TOPOLOGY_PROCESS" ) << tr( "BLSURF_TOPOLOGY_PROCESS2" );
  myTopology->addItems( topologyTypes );

  myVerbosity = new QSpinBox( myAdvGroup );
  myVerbosity->setMinimum( 0 );
  myVerbosity->setMaximum( 100 );
  myVerbosity->setSingleStep( 5 );

  myOptionTable = new QTableWidget( 0, NB_COLUMNS, myAdvGroup );
  QStringList headers;
  headers << tr( "OPTION_ID_COLUMN" ) << tr( "OPTION_NAME_COLUMN" ) << tr( "OPTION_VALUE_COLUMN" );
  myOptionTable->setHorizontalHeaderLabels( headers );
  myOptionTable->horizontalHeader()->hideSection( OPTION_ID_COLUMN );
  //myOptionTable->setColumnReadOnly( OPTION_NAME_COLUMN, TRUE );//////
  //myOptionTable->setColumnReadOnly( OPTION_VALUE_COLUMN, FALSE );/////
  myOptionTable->verticalHeader()->hide();
  //myOptionTable->setSelectionBehavior( QAbstractItemView::SelectRows );

  QPushButton* addBtn = new QPushButton( tr( "ADD_OPTION"),  myAdvGroup );
  addBtn->setMenu( new QMenu() );
  QPushButton* rmBtn = new QPushButton( tr( "REMOVE_OPTION"), myAdvGroup );


  // ADD WIDGETS (ADVANCED TAB)
  anAdvLayout->addWidget( new QLabel( tr( "BLSURF_TOPOLOGY" ), myAdvGroup ),  0, 0, 1, 1 );
  anAdvLayout->addWidget( myTopology,                                         0, 1, 1, 1 );
  anAdvLayout->addWidget( new QLabel( tr( "BLSURF_VERBOSITY" ), myAdvGroup ), 1, 0, 1, 1 );
  anAdvLayout->addWidget( myVerbosity,                                        1, 1, 1, 1 );
  anAdvLayout->addWidget( myOptionTable,                                      2, 0, 3, 2 );
  anAdvLayout->addWidget( addBtn,                                             2, 2, 1, 1 );
  anAdvLayout->addWidget( rmBtn,                                              3, 2, 1, 1 );


  // Size Maps parameters

  mySmpGroup = new QWidget();
  QGridLayout* anSmpLayout = new QGridLayout(mySmpGroup);

  mySizeMapTable = new QTableWidget( 0, SMP_NB_COLUMNS, mySmpGroup );
  QStringList sizeMapHeaders;
  sizeMapHeaders << tr( "SMP_ENTRY_COLUMN" )<< tr( "SMP_NAME_COLUMN" ) << tr( "SMP_SIZEMAP_COLUMN" );
  mySizeMapTable->setHorizontalHeaderLabels(sizeMapHeaders);
  mySizeMapTable->horizontalHeader()->hideSection( SMP_ENTRY_COLUMN );
  mySizeMapTable->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  mySizeMapTable->resizeColumnToContents(SMP_NAME_COLUMN);
  mySizeMapTable->resizeColumnToContents(SMP_SIZEMAP_COLUMN);
  mySizeMapTable->setAlternatingRowColors(true);
  mySizeMapTable->verticalHeader()->hide();
/*
  addAttractorButton = new QPushButton(tr("BLSURF_SM_ATTRACTOR"),mySmpGroup);
  QFrame *line = new QFrame(mySmpGroup);
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
*/
  addSurfaceButton = new QPushButton(tr("BLSURF_SM_SURFACE"),mySmpGroup);
  addEdgeButton = new QPushButton(tr("BLSURF_SM_EDGE"),mySmpGroup);
  addPointButton = new QPushButton(tr("BLSURF_SM_POINT"),mySmpGroup);
  QFrame *line2 = new QFrame(mySmpGroup);
  line2->setFrameShape(QFrame::HLine);
  line2->setFrameShadow(QFrame::Sunken);
  removeButton = new QPushButton(tr("BLSURF_SM_REMOVE"),mySmpGroup);


  // ADD WIDGETS (SIZEMAP TAB)
  anSmpLayout->addWidget(mySizeMapTable,     SMP_POINT_BTN, 0, SMP_NB_LINES+1, 1);
//   anSmpLayout->addWidget(addAttractorButton, SMP_ATTRACTOR_BTN, 1, 1, 1);
//   anSmpLayout->addWidget(line,               SMP_SEPARATOR1, 1, 1, 1);
  anSmpLayout->addWidget(addPointButton,     SMP_POINT_BTN, 1, 1, 1);
  anSmpLayout->addWidget(addEdgeButton,      SMP_EDGE_BTN, 1, 1, 1);
  anSmpLayout->addWidget(addSurfaceButton,   SMP_SURFACE_BTN, 1, 1, 1);
  anSmpLayout->addWidget(line2,              SMP_SEPARATOR2, 1, 1, 1);
  anSmpLayout->addWidget(removeButton,       SMP_REMOVE_BTN, 1, 1, 1);


  // Enforced vertices parameters
  myEnfGroup = new QWidget();
  QGridLayout* anEnfLayout = new QGridLayout(myEnfGroup);
//
//   myEnforcedVertexWidget = new DlgBlSurfHyp_Enforced(myEnfGroup);
//   anEnfLayout->addWidget(myEnforcedVertexWidget);
//   MESSAGE("Creating DlgBlSurfHyp_Enforced widget instance");
//   myEnforcedVertexWidget = new DlgBlSurfHyp_Enforced();

  myEnforcedTreeWidget = new QTreeWidget(myEnfGroup);
  myEnforcedTreeWidget->setColumnCount( ENF_VER_NB_COLUMNS );
  myEnforcedTreeWidget->setSortingEnabled(true);
  QStringList enforcedHeaders;
  enforcedHeaders << tr("BLSURF_ENF_VER_NAME_COLUMN") << tr("BLSURF_ENF_VER_FACE_ENTRY_COLUMN")
                  << tr("BLSURF_ENF_VER_X_COLUMN")<< tr("BLSURF_ENF_VER_Y_COLUMN") << tr("BLSURF_ENF_VER_Z_COLUMN")
                  << tr("BLSURF_ENF_VER_ENTRY_COLUMN") << tr( "BLSURF_ENF_VER_GROUP_COLUMN" );

  myEnforcedTreeWidget->setHeaderLabels(enforcedHeaders);
  myEnforcedTreeWidget->setAlternatingRowColors(true);
  myEnforcedTreeWidget->setUniformRowHeights(true);
  myEnforcedTreeWidget->setAnimated(true);
  myEnforcedTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
  myEnforcedTreeWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
  for (int column = 0; column < ENF_VER_NB_COLUMNS; ++column) {
    myEnforcedTreeWidget->header()->setResizeMode(column,QHeaderView::Interactive);
    myEnforcedTreeWidget->resizeColumnToContents(column);
  }
  myEnforcedTreeWidget->hideColumn(ENF_VER_FACE_ENTRY_COLUMN);
  myEnforcedTreeWidget->hideColumn(ENF_VER_ENTRY_COLUMN);
  myEnforcedTreeWidget->setItemDelegate(new EnforcedTreeWidgetDelegate());
  
// FACE AND VERTEX SELECTION
  TColStd_MapOfInteger shapeTypes1, shapeTypes2;
  shapeTypes1.Add( TopAbs_FACE );
  shapeTypes1.Add( TopAbs_COMPOUND );
  shapeTypes2.Add( TopAbs_VERTEX );
  shapeTypes2.Add( TopAbs_COMPOUND );

  SMESH_NumberFilter* faceFilter = new SMESH_NumberFilter("GEOM", TopAbs_FACE, 0, shapeTypes1);
  myEnfFaceWdg = new StdMeshersGUI_ObjectReferenceParamWdg( faceFilter, 0, /*multiSel=*/true);
  myEnfFaceWdg->SetDefaultText("Select Faces", "QLineEdit { color: grey }");

  SMESH_NumberFilter* vertexFilter = new SMESH_NumberFilter("GEOM", TopAbs_SHAPE, 1, shapeTypes2);
  myEnfVertexWdg = new StdMeshersGUI_ObjectReferenceParamWdg( vertexFilter, 0, /*multiSel=*/true);
  myEnfVertexWdg->SetDefaultText("Select Vertices", "QLineEdit { color: grey }");

  myEnfVertexWdg->AvoidSimultaneousSelection(myEnfFaceWdg);

  QLabel* myXCoordLabel = new QLabel( tr( "BLSURF_ENF_VER_X_LABEL" ), myEnfGroup );
  myXCoord = new SMESHGUI_SpinBox(myEnfGroup);
  myXCoord->RangeStepAndValidator(COORD_MIN, COORD_MAX, 10.0, "length_precision");

  QLabel* myYCoordLabel = new QLabel( tr( "BLSURF_ENF_VER_Y_LABEL" ), myEnfGroup );
  myYCoord = new SMESHGUI_SpinBox(myEnfGroup);
  myYCoord->RangeStepAndValidator(COORD_MIN, COORD_MAX, 10.0, "length_precision");

  QLabel* myZCoordLabel = new QLabel( tr( "BLSURF_ENF_VER_Z_LABEL" ), myEnfGroup );
  myZCoord = new SMESHGUI_SpinBox(myEnfGroup);
  myZCoord->RangeStepAndValidator(COORD_MIN, COORD_MAX, 10.0, "length_precision");

  QLabel* myGroupNameLabel = new QLabel( tr( "BLSURF_ENF_VER_GROUP_LABEL" ), myEnfGroup );
  myGroupName = new QLineEdit(myEnfGroup);

  addVertexButton = new QPushButton(tr("BLSURF_ENF_VER_VERTEX"),myEnfGroup);
//   QFrame *line = new QFrame(myEnfGroup);
//   line->setFrameShape(QFrame::HLine);
//   line->setFrameShadow(QFrame::Sunken);
  removeVertexButton = new QPushButton(tr("BLSURF_ENF_VER_REMOVE"),myEnfGroup);

  // CheckBox for groups generation
  makeGroupsCheck = new QGroupBox(tr("BLSURF_ENF_VER_GROUPS"), myEnfGroup);
  makeGroupsCheck->setCheckable(true);
  makeGroupsCheck->setChecked(false);
  QGridLayout* aGroupLayout = new QGridLayout(makeGroupsCheck);
  myGlobalGroupName = new QLineEdit(makeGroupsCheck);
  aGroupLayout->addWidget(myGlobalGroupName);

  anEnfLayout->addWidget(myEnforcedTreeWidget,     0, 0, ENF_VER_NB_LINES+1, 1);
  QGridLayout* anEnfLayout2 = new QGridLayout(myEnfGroup);
//  FACE AND VERTEX SELECTION
  anEnfLayout2->addWidget(myEnfFaceWdg,             ENF_VER_FACE, 0, 1, 2);
  anEnfLayout2->addWidget(myEnfVertexWdg,           ENF_VER_VERTEX, 0, 1, 2);

  anEnfLayout2->addWidget(myXCoordLabel,            ENF_VER_X_COORD, 0, 1, 1);
  anEnfLayout2->addWidget(myXCoord,                 ENF_VER_X_COORD, 1, 1, 1);
  anEnfLayout2->addWidget(myYCoordLabel,            ENF_VER_Y_COORD, 0, 1, 1);
  anEnfLayout2->addWidget(myYCoord,                 ENF_VER_Y_COORD, 1, 1, 1);
  anEnfLayout2->addWidget(myZCoordLabel,            ENF_VER_Z_COORD, 0, 1, 1);
  anEnfLayout2->addWidget(myZCoord,                 ENF_VER_Z_COORD, 1, 1, 1);
  anEnfLayout2->addWidget(myGroupNameLabel,         ENF_VER_GROUP, 0, 1, 1);
  anEnfLayout2->addWidget(myGroupName,              ENF_VER_GROUP, 1, 1, 1);
  anEnfLayout2->addWidget(addVertexButton,          ENF_VER_VERTEX_BTN, 0, 1, 2);
  anEnfLayout2->addWidget(removeVertexButton,       ENF_VER_REMOVE_BTN, 0, 1, 2);
//   anEnfLayout->addWidget(line,                     ENF_VER_SEPARATOR, 0, 1, 2);
  anEnfLayout2->addWidget(makeGroupsCheck,          ENF_VER_GROUP_CHECK, 0, 1, 2);
  anEnfLayout->addLayout(anEnfLayout2, 0,1,ENF_VER_NB_LINES+1,2);

  // ---
  tab->insertTab( STD_TAB, myStdGroup, tr( "SMESH_ARGUMENTS" ) );
  tab->insertTab( ADV_TAB, myAdvGroup, tr( "BLSURF_ADV_ARGS" ) );
  tab->insertTab( SMP_TAB, mySmpGroup, tr( "BLSURF_SIZE_MAP" ) );
  tab->insertTab( ENF_TAB, myEnfGroup, tr( "BLSURF_ENF_VER" ) );

  tab->setCurrentIndex( STD_TAB );

  // ---
  connect( myGeometricMesh,    SIGNAL( activated( int ) ),            this,         SLOT( onGeometricMeshChanged() ) );
  connect( myPhysicalMesh,     SIGNAL( activated( int ) ),            this,         SLOT( onPhysicalMeshChanged() ) );
  connect( addBtn->menu(),     SIGNAL( aboutToShow() ),               this,         SLOT( onAddOption() ) );
  connect( addBtn->menu(),     SIGNAL( triggered( QAction* ) ),       this,         SLOT( onOptionChosenInPopup( QAction* ) ) );
  connect( rmBtn,              SIGNAL( clicked()),                    this,         SLOT( onDeleteOption() ) );

  connect( addSurfaceButton,   SIGNAL( clicked()),                    this,         SLOT( onAddMapOnSurface() ) );
  connect( addEdgeButton,      SIGNAL( clicked()),                    this,         SLOT( onAddMapOnEdge() ) );
  connect( addPointButton,     SIGNAL( clicked()),                    this,         SLOT( onAddMapOnPoint() ) );
  connect( removeButton,       SIGNAL( clicked()),                    this,         SLOT( onRemoveMap() ) );
  connect( mySizeMapTable,     SIGNAL( cellChanged ( int, int  )),    this,         SLOT( onSetSizeMap(int,int ) ) );

  connect( myEnforcedTreeWidget,SIGNAL( itemClicked(QTreeWidgetItem *, int)), this, SLOT( synchronizeCoords() ) );
  connect( myEnforcedTreeWidget,SIGNAL( itemChanged(QTreeWidgetItem *, int)), this, SLOT( updateEnforcedVertexValues(QTreeWidgetItem *, int) ) );
  connect( myEnforcedTreeWidget,SIGNAL( itemSelectionChanged() ),     this,         SLOT( synchronizeCoords() ) );
  connect( addVertexButton,    SIGNAL( clicked()),                    this,         SLOT( onAddEnforcedVertices() ) );
  connect( removeVertexButton, SIGNAL( clicked()),                    this,         SLOT( onRemoveEnforcedVertex() ) );
  connect( myEnfVertexWdg,     SIGNAL( contentModified()),            this,         SLOT( onSelectEnforcedVertex() ) );
//   connect( myEnfVertexWdg,     SIGNAL( selectionActivated()),         this,         SLOT( onVertexSelectionActivated() ) );
//   connect( myEnfFaceWdg,       SIGNAL( selectionActivated()),         this,         SLOT( onFaceSelectionActivated() ) );
//   connect(QApplication::instance(), SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(deactivateSelection(QWidget*, QWidget*)));

  return fr;
}

/** BLSURFPluginGUI_HypothesisCreator::deactivateSelection(QWidget*, QWidget*)
This method stop the selection of the widgets StdMeshersGUI_ObjectReferenceParamWdg
*/
void BLSURFPluginGUI_HypothesisCreator::deactivateSelection(QWidget* old, QWidget* now)
{
  if ((now == myXCoord) || (now == myYCoord) || (now == myZCoord)
      || (now = myGroupName) || (now = makeGroupsCheck) || (now = myEnforcedTreeWidget)) {
    BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;
    that->getGeomSelectionTool()->selectionMgr()->clearFilters();
    myEnfFaceWdg->deactivateSelection();
    myEnfVertexWdg->deactivateSelection();
  }
}

/** 
 * This method resets the content of the X, Y, Z and GroupNAme widgets;
**/
void BLSURFPluginGUI_HypothesisCreator::clearEnforcedVertexWidgets()
{
  myXCoord->setCleared(true);
  myYCoord->setCleared(true);
  myZCoord->setCleared(true);
  myXCoord->setText("");
  myYCoord->setText("");
  myZCoord->setText("");
  myGroupName->setText("");
}

/** BLSURFPluginGUI_HypothesisCreator::updateEnforcedVertexValues(item, column)
This method updates the tooltip of a modified item. The QLineEdit widgets content
is synchronized with the coordinates of the enforced vertex clicked in the tree widget.
*/
void BLSURFPluginGUI_HypothesisCreator::updateEnforcedVertexValues(QTreeWidgetItem* item, int column) {
//   MESSAGE("BLSURFPluginGUI_HypothesisCreator::updateEnforcedVertexValues");
  QVariant vertexName = item->data(ENF_VER_NAME_COLUMN, Qt::EditRole);
  QVariant x = item->data(ENF_VER_X_COLUMN, Qt::EditRole);
  QVariant y = item->data(ENF_VER_X_COLUMN, Qt::EditRole);
  QVariant z = item->data(ENF_VER_X_COLUMN, Qt::EditRole);
  QVariant entry = item->data(ENF_VER_ENTRY_COLUMN, Qt::EditRole);
  QString groupName = item->data(ENF_VER_GROUP_COLUMN, Qt::EditRole).toString();
  QTreeWidgetItem* parent = item->parent();
  
  clearEnforcedVertexWidgets();
  
  if (parent && (!x.isNull() || !entry.isNull())) {
      QString shapeName = parent->data(ENF_VER_NAME_COLUMN, Qt::EditRole).toString();
      QString toolTip = shapeName + QString(": ") + vertexName.toString();
      if (entry.isNull()) {
        toolTip += QString("(") + x.toString();
        toolTip += QString(", ") + y.toString();
        toolTip += QString(", ") + z.toString();
        toolTip += QString(")");
      }
      
      if (!groupName.isEmpty())
        toolTip += QString(" [") + groupName + QString("]");

      item->setToolTip(ENF_VER_NAME_COLUMN,toolTip);

    if (!x.isNull()) {
      myXCoord->SetValue(x.toDouble());
      myYCoord->SetValue(y.toDouble());
      myZCoord->SetValue(z.toDouble());
    }
    
    if (!groupName.isEmpty())
      myGroupName->setText(groupName);
  }
}

void BLSURFPluginGUI_HypothesisCreator::onSelectEnforcedVertex() {
  int nbSelEnfVertex = myEnfVertexWdg->NbObjects();
  clearEnforcedVertexWidgets();
  if (nbSelEnfVertex == 1)
  {
    if ( CORBA::is_nil( getGeomEngine() ) && !GeometryGUI::InitGeomGen() )
    return ;

    myEnfVertex = myEnfVertexWdg->GetObject< GEOM::GEOM_Object >(nbSelEnfVertex-1);
    if (myEnfVertex->GetShapeType() == GEOM::VERTEX) {
      BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;
      GEOM::GEOM_IMeasureOperations_var measureOp = getGeomEngine()->GetIMeasureOperations( that->getGeomSelectionTool()->getMyStudy()->StudyId() );
      if (CORBA::is_nil(measureOp))
        return;
      
      CORBA::Double x,y,z;
      measureOp->PointCoordinates (myEnfVertex, x, y, z);
      if ( measureOp->IsDone() )
      {
        myXCoord->SetValue(x);
        myYCoord->SetValue(y);
        myZCoord->SetValue(z);
      }
    }
  }
}

/** BLSURFPluginGUI_HypothesisCreator::synchronizeCoords()
This method synchronizes the QLineEdit/SMESHGUI_SpinBox widgets content with the coordinates
of the enforced vertex clicked in the tree widget.
*/
void BLSURFPluginGUI_HypothesisCreator::synchronizeCoords() {
  clearEnforcedVertexWidgets();
  QList<QTreeWidgetItem *> items = myEnforcedTreeWidget->selectedItems();
  if (! items.isEmpty() && items.size() == 1) {
    QTreeWidgetItem *item = items[0];
//     for (int i=0 ; i < items.size() ; i++) {
//       item = items[i];
      QVariant x = item->data(ENF_VER_X_COLUMN, Qt::EditRole);
      QVariant y = item->data(ENF_VER_Y_COLUMN, Qt::EditRole);
      QVariant z = item->data(ENF_VER_Z_COLUMN, Qt::EditRole);
      QVariant entry = item->data(ENF_VER_ENTRY_COLUMN, Qt::EditRole);
      QVariant group = item->data(ENF_VER_GROUP_COLUMN, Qt::EditRole);
      if (!x.isNull()/* && entry.isNull()*/) {
        myXCoord->SetValue(x.toDouble());
        myYCoord->SetValue(y.toDouble());
        myZCoord->SetValue(z.toDouble());
//         break;
      }
      if (!group.isNull() && (!x.isNull() || !entry.isNull()))
        myGroupName->setText(group.toString());
//     }
  }
}

/** BLSURFPluginGUI_HypothesisCreator::addEnforcedVertex(entry, shapeName, x, y, z)
This method adds an enforced vertex (x,y,z) to shapeName in the tree widget.
*/
void BLSURFPluginGUI_HypothesisCreator::addEnforcedVertex(std::string theFaceEntry, std::string theFaceName,
    double x, double y, double z, std::string vertexName, std::string geomEntry, std::string groupName) {
  // Find theFaceEntry item
  QList<QTreeWidgetItem* > theItemList = myEnforcedTreeWidget->findItems(QString(theFaceEntry.c_str()),Qt::MatchExactly,ENF_VER_FACE_ENTRY_COLUMN);
  QTreeWidgetItem* theItem;
  if (theItemList.empty()) {
    theItem = new QTreeWidgetItem();
    theItem->setData(ENF_VER_FACE_ENTRY_COLUMN, Qt::EditRole, QVariant(theFaceEntry.c_str()));
    theItem->setData(ENF_VER_NAME_COLUMN, Qt::EditRole, QVariant(theFaceName.c_str()));
    theItem->setToolTip(ENF_VER_NAME_COLUMN,QString(theFaceEntry.c_str()));
    myEnforcedTreeWidget->addTopLevelItem(theItem);
  }
  else {
    theItem = theItemList[0];
  }

//   MESSAGE("theItemName is " << theItem->text(ENF_VER_NAME_COLUMN).toStdString());
  bool okToCreate = true;

  const int nbVert = theItem->childCount();
//   MESSAGE("Number of child rows: " << nbVert);
  if (nbVert >0) {
    double childValueX,childValueY,childValueZ;
    QString childEntry, childGroupName;
    QTreeWidgetItem* child;
    for (int row = 0;row<nbVert;row++) {
      child = theItem->child(row);
      childGroupName = child->data(ENF_VER_GROUP_COLUMN,Qt::EditRole).toString();
      childEntry = child->data(ENF_VER_ENTRY_COLUMN,Qt::EditRole).toString();
      childValueX = child->data(ENF_VER_X_COLUMN,Qt::EditRole).toDouble();
      childValueY = child->data(ENF_VER_Y_COLUMN,Qt::EditRole).toDouble();
      childValueZ = child->data(ENF_VER_Z_COLUMN,Qt::EditRole).toDouble();
      if (((childValueX == x) && (childValueY == y) && (childValueZ == z)) || ( (childEntry.toStdString() != "") && (childEntry.toStdString() == geomEntry))) {
        // update group name
        if (childGroupName.toStdString() != groupName) {
          MESSAGE("Group is updated from \"" << childGroupName.toStdString() << "\" to \"" << groupName << "\"");
          child->setData(ENF_VER_GROUP_COLUMN, Qt::EditRole, QVariant(groupName.c_str()));
        }
        okToCreate = false;
        break;
      } // if
    } // for
  } // if
  if (!okToCreate) {
    if (geomEntry.empty()) {
      MESSAGE("In " << theFaceName << " vertex with coords " << x << ", " << y << ", " << z << " already exist: dont create again");
    }
    else {
      MESSAGE("In " << theFaceName << " vertex with entry " << geomEntry << " already exist: dont create again");
    }
    return;
  }
    
  if (geomEntry.empty()) {
    MESSAGE("In " << theFaceName << " vertex with coords " << x << ", " << y << ", " << z<< " is created");
  }
  else {
    MESSAGE("In " << theFaceName << " vertex with geom entry " << geomEntry << " is created");
  }

  QTreeWidgetItem *vertexItem = new QTreeWidgetItem( theItem);
  vertexItem->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
  QPixmap iconSelect (SUIT_Session::session()->resourceMgr()->loadPixmap("SMESH", tr("ICON_SELECT")));
  QSize iconSize = iconSelect.size()*0.7;
  
  int vertexIndex=myEnforcedTreeWidget->indexOfTopLevelItem(theItem);
  QString myVertexName;
  int indexRef = -1;
  while(indexRef != vertexIndex) {
    indexRef = vertexIndex;
    if (vertexName.empty())
      myVertexName = QString("Vertex #%1").arg(vertexIndex);
    else
      myVertexName = QString(vertexName.c_str());

    for (int row = 0;row<nbVert;row++) {
      QString name = theItem->child(row)->data(ENF_VER_NAME_COLUMN,Qt::EditRole).toString();
      if (myVertexName == name) {
        vertexIndex++;
        break;
      }
    }
  }
  vertexItem->setData( ENF_VER_NAME_COLUMN, Qt::EditRole, myVertexName );
  if (geomEntry.empty()) {
    vertexItem->setData( ENF_VER_X_COLUMN, Qt::EditRole, QVariant(x) );
    vertexItem->setData( ENF_VER_Y_COLUMN, Qt::EditRole, QVariant(y) );
    vertexItem->setData( ENF_VER_Z_COLUMN, Qt::EditRole, QVariant(z) );
  }
  else {
    vertexItem->setIcon(ENF_VER_NAME_COLUMN, QIcon(iconSelect.scaled(iconSize,Qt::KeepAspectRatio,Qt::SmoothTransformation)));
    vertexItem->setData( ENF_VER_ENTRY_COLUMN, Qt::EditRole, QString(geomEntry.c_str()) );
  }
  if (groupName != "")
    vertexItem->setData( ENF_VER_GROUP_COLUMN, Qt::EditRole, QVariant(groupName.c_str()));

  QString toolTip = QString(theFaceName.c_str())+QString(": ")+myVertexName;
  if (geomEntry.empty()) {
    toolTip += QString(" (%1, ").arg(x);
    toolTip += QString("%1, ").arg(y);
    toolTip += QString("%1)").arg(z);
  }
  if (groupName != "")
    toolTip += QString(" [%1]").arg(groupName.c_str());
  
  vertexItem->setToolTip(ENF_VER_NAME_COLUMN,toolTip);
  theItem->setExpanded(true);
  myEnforcedTreeWidget->setCurrentItem(vertexItem,ENF_VER_NAME_COLUMN);
}

/** BLSURFPluginGUI_HypothesisCreator::onAddEnforcedVertices()
This method is called when a item is added into the enforced vertices tree widget
*/
void BLSURFPluginGUI_HypothesisCreator::onAddEnforcedVertices() {
//   MESSAGE("BLSURFPluginGUI_HypothesisCreator::onAddEnforcedVertices");

  BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;

  that->getGeomSelectionTool()->selectionMgr()->clearFilters();
  myEnfFaceWdg->deactivateSelection();
  myEnfVertexWdg->deactivateSelection();

  for (int column = 0; column < myEnforcedTreeWidget->columnCount(); ++column)
    myEnforcedTreeWidget->resizeColumnToContents(column);

  // Vertex selection
  int selEnfFace   = myEnfFaceWdg->NbObjects();
  int selEnfVertex = myEnfVertexWdg->NbObjects();
  bool coordsEmpty = (myXCoord->text().isEmpty()) || (myYCoord->text().isEmpty()) || (myZCoord->text().isEmpty());

  if (selEnfFace == 0)
    return;

  if ((selEnfVertex == 0) && coordsEmpty)
    return;

  string entry, shapeName;

  for (int i = 0 ; i < selEnfFace ; i++) {
    myEnfFace = myEnfFaceWdg->GetObject< GEOM::GEOM_Object >(i);
    entry = myEnfFace->GetStudyEntry();
    shapeName = myEnfFace->GetName();

    std::string groupName = myGroupName->text().toStdString();
    if (makeGroupsCheck->isChecked())
      groupName = myGlobalGroupName->text().toStdString();

    if (boost::trim_copy(groupName).empty())
      groupName = "";

    if (selEnfVertex <= 1)
    {
      double x,y,z;
      x = myXCoord->GetValue();
      y = myYCoord->GetValue();
      z = myZCoord->GetValue();
      if (selEnfVertex == 1) {
        myEnfVertex = myEnfVertexWdg->GetObject< GEOM::GEOM_Object >();
        addEnforcedVertex(entry, shapeName, x, y, z, myEnfVertex->GetName(),myEnfVertex->GetStudyEntry(), groupName);
      }
      else
        addEnforcedVertex(entry, shapeName, x, y, z, "", "", groupName);
    }
    else
    {
      if ( CORBA::is_nil(getGeomEngine()))
        return;

      GEOM::GEOM_IMeasureOperations_var measureOp = getGeomEngine()->GetIMeasureOperations( that->getGeomSelectionTool()->getMyStudy()->StudyId() );
      if (CORBA::is_nil(measureOp))
        return;

      CORBA::Double x,y,z;
      x = y = z = 0.;
      for (int j = 0 ; j < selEnfVertex ; j++)
      {
        myEnfVertex = myEnfVertexWdg->GetObject< GEOM::GEOM_Object >(j);
        if (myEnfVertex->GetShapeType() == GEOM::VERTEX) {
          measureOp->PointCoordinates (myEnfVertex, x, y, z);
          if ( measureOp->IsDone() )
            addEnforcedVertex(entry, shapeName, x, y, z, myEnfVertex->GetName(),myEnfVertex->GetStudyEntry(), groupName);
        } else if (myEnfVertex->GetShapeType() == GEOM::COMPOUND) {
            addEnforcedVertex(entry, shapeName, 0, 0, 0, myEnfVertex->GetName(),myEnfVertex->GetStudyEntry(), groupName);
        }
      }
    }
  }

  myEnfFaceWdg->SetObject(GEOM::GEOM_Object::_nil());
  myEnfVertexWdg->SetObject(GEOM::GEOM_Object::_nil());
  
  for (int column = 0; column < myEnforcedTreeWidget->columnCount(); ++column)
    myEnforcedTreeWidget->resizeColumnToContents(column);

  if ( myPhysicalMesh->currentIndex() != SizeMap ) {
    myPhysicalMesh->setCurrentIndex( SizeMap );
    onPhysicalMeshChanged();
  }
}

/** BLSURFPluginGUI_HypothesisCreator::onRemoveEnforcedVertex()
This method is called when a item is removed from the enforced vertices tree widget
*/
void BLSURFPluginGUI_HypothesisCreator::onRemoveEnforcedVertex() {
//   MESSAGE("BLSURFPluginGUI_HypothesisCreator::onRemoveEnforcedVertex");
  QList<QTreeWidgetItem *> selectedItems = myEnforcedTreeWidget->selectedItems();
  QList<QTreeWidgetItem *> selectedVertices;
  QSet<QTreeWidgetItem *> selectedEntries;
  QTreeWidgetItem* item;

  foreach( item, selectedItems ) {
    QVariant value = item->data(ENF_VER_X_COLUMN, Qt::EditRole);
    if (! value.isNull())
      selectedVertices.append(item);
    else {
      value = item->data(ENF_VER_ENTRY_COLUMN, Qt::EditRole);
      if (! value.isNull())
        selectedVertices.append(item);
      else
        selectedEntries.insert(item);
    }
  }

  foreach(item,selectedVertices) {
    QTreeWidgetItem* parent = item->parent();
//     MESSAGE("From geometry "<< parent->text(ENF_VER_NAME_COLUMN).toStdString()<<" remove " << item->text(ENF_VER_NAME_COLUMN).toStdString());
    parent->removeChild(item);
    delete item;
    if (parent->childCount() == 0) {
      if (selectedEntries.contains(parent))
        selectedEntries.remove(parent);
      delete parent;
    }
  }

  foreach(item,selectedEntries) {
//     MESSAGE("Remove " << item->text(ENF_VER_NAME_COLUMN).toStdString());
    delete item;
  }

  myEnforcedTreeWidget->selectionModel()->clearSelection();
}

/** BLSURFPluginGUI_HypothesisCreator::retrieveParams()
This method updates the GUI widgets with the hypothesis data
*/
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
  myPhySize->SetValue( data.myPhySize );
#ifdef WITH_SIZE_BOUNDARIES
  MESSAGE("data.myPhyMin: "<<data.myPhyMin)
  if (data.myPhyMin < 0)
    myPhyMin->setText("");
  else
    myPhyMin->SetValue( data.myPhyMin );
  MESSAGE("data.myPhyMax: "<<data.myPhyMax)
  if (data.myPhyMax < 0)
    myPhyMax->setText("");
  else
    myPhyMax->SetValue( data.myPhyMax );
  MESSAGE("data.myGeoMin: "<<data.myGeoMin)
  if (data.myGeoMin < 0)
    myGeoMin->setText("");
  else
    myGeoMin->SetValue( data.myGeoMin );
  MESSAGE("data.myGeoMax: "<<data.myGeoMax)
  if (data.myGeoMax < 0)
    myGeoMax->setText("");
  else
    myGeoMax->SetValue( data.myGeoMax );
#endif
  myGeometricMesh->setCurrentIndex( data.myGeometricMesh );
//   myAngleMeshS->setValue( data.myAngleMeshS );
//   myAngleMeshC->setValue( data.myAngleMeshC );
//   myGradation->setValue( data.myGradation );
  myAngleMeshS->SetValue( data.myAngleMeshS );
  myAngleMeshC->SetValue( data.myAngleMeshC );
  myGradation->SetValue( data.myGradation );
  myAllowQuadrangles->setChecked( data.myAllowQuadrangles );
  myDecimesh->setChecked( data.myDecimesh );
  myVerbosity->setValue( data.myVerbosity );

  if ( myOptions.operator->() ) {
//     MESSAGE("retrieveParams():myOptions->length() = " << myOptions->length());
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

  // Sizemaps
//   MESSAGE("retrieveParams():that->mySMPMap.size() = " << that->mySMPMap.size());
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

  // Enforced vertices
  MESSAGE("retrieveParams(): data.entryCoordsListMap.size() = " << data.faceEntryEnfVertexListMap.size());
  TFaceEntryEnfVertexListMap::const_iterator evmIt = data.faceEntryEnfVertexListMap.begin();
  for ( ; evmIt != data.faceEntryEnfVertexListMap.end() ; ++evmIt) {
    TEntry entry = (*evmIt).first;
    std::string shapeName = myGeomToolSelected->getNameFromEntry(entry);
    MESSAGE("Face entry: " << entry);
    MESSAGE("Face name: " << shapeName);

    TEnfVertexList evs = (*evmIt).second;
//     try  {
//       evs = (*evmIt).second;
//     }
//     catch(...) {
//       MESSAGE("evs = (*evmIt).second: FAIL");
//       break;
//     }

    TEnfVertexList::const_iterator evsIt = evs.begin();
    TEnfVertex *enfVertex;
    for ( ; evsIt != evs.end() ; ++evsIt) {
      enfVertex = (*evsIt);
      MESSAGE("Name: " << enfVertex->name);
      double x, y, z = 0;
      if (enfVertex->coords.size()) {
        x = enfVertex->coords[0];
        y = enfVertex->coords[1];
        z = enfVertex->coords[2];
      }
      that->addEnforcedVertex(entry, shapeName, x, y, z, enfVertex->name, enfVertex->geomEntry, enfVertex->grpName);
    }
  }
  
  for (int column = 0; column < myEnforcedTreeWidget->columnCount(); ++column)
    myEnforcedTreeWidget->resizeColumnToContents(column);

  // update widgets
  that->onPhysicalMeshChanged();
  that->onGeometricMeshChanged();
}

/** BLSURFPluginGUI_HypothesisCreator::storeParams()
This method updates the hypothesis data with the GUI widgets content.
*/
QString BLSURFPluginGUI_HypothesisCreator::storeParams() const
{
  BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;

  BlsurfHypothesisData data;
  QString guiHyp = that->readParamsFromWidgets( data );
  that->storeParamsToHypo( data );

  return guiHyp;
}

/** BLSURFPluginGUI_HypothesisCreator::readParamsFromHypo(h_data)
Updates the hypothesis data from hypothesis values.
*/
bool BLSURFPluginGUI_HypothesisCreator::readParamsFromHypo( BlsurfHypothesisData& h_data ) const
{
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::readParamsFromHypo");
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
//   if ( PhyMin > 0 )
//   h_data.myPhyMin = PhyMin > 0 ? QString::number( h->GetPhyMin() ) : QString("");
//   h_data.myPhyMax = PhyMax > 0 ? QString::number( h->GetPhyMax() ) : QString("");
//   h_data.myGeoMin = GeoMin > 0 ? QString::number( h->GetGeoMin() ) : QString("");
//   h_data.myGeoMax = GeoMax > 0 ? QString::number( h->GetGeoMax() ) : QString("");
  h_data.myPhyMin = PhyMin > 0 ? PhyMin : -1.0;
  h_data.myPhyMax = PhyMax > 0 ? PhyMax : -1.0;
  h_data.myGeoMin = GeoMin > 0 ? GeoMin : -1.0;
  h_data.myGeoMax = GeoMax > 0 ? GeoMax : -1.0;
#endif

  BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;
  that->myOptions = h->GetOptionValues();

  that->mySMPMap.clear();

  // classic size maps
  BLSURFPlugin::string_array_var mySizeMaps = h->GetSizeMapEntries();
//   MESSAGE("mySizeMaps->length() = " << mySizeMaps->length());
  QString fullSizeMaps;
  QStringList fullSizeMapList;
  GeomSelectionTools* myGeomToolSelected = that->getGeomSelectionTool();
  for ( int i = 0;i<mySizeMaps->length(); ++i ) {
    fullSizeMaps =  mySizeMaps[i].in();
//     MESSAGE("fullSizeMaps: " << fullSizeMaps.toStdString());
    fullSizeMapList = fullSizeMaps.split( "|", QString::KeepEmptyParts );
    if ( fullSizeMapList.count() > 1 ) {
      string fullSizeMap = fullSizeMapList[1].toStdString();
      int pos = fullSizeMap.find("return")+7;
//       MESSAGE("pos:" << pos);
      QString sizeMap;
      try {
        sizeMap = QString::fromStdString(fullSizeMap.substr(pos, fullSizeMap.size()-pos));
      }
      catch (...) {
        continue;
      }
      that->mySMPMap[fullSizeMapList[0]] = sizeMap;
//       MESSAGE("mySMPMap[" << fullSizeMapList[0].toStdString() << "] = " << sizeMap.toStdString());
      that->mySMPShapeTypeMap[fullSizeMapList[0]] = myGeomToolSelected->entryToShapeType(fullSizeMapList[0].toStdString());
//       MESSAGE("mySMPShapeTypeMap[" << fullSizeMapList[0].toStdString() << "] = " << that->mySMPShapeTypeMap[fullSizeMapList[0]]);
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
      that->mySMPShapeTypeMap[fullCustomSizeMapList[0]] = GeomToolSelected->entryToShapeType(fullCustomSizeMapList[0].toStdString());
      MESSAGE("mySMPMap[" << fullCustomSizeMapList[0].toStdString() << "] = " << fullCustomSizeMapList[1].toStdString());
      MESSAGE("mySMPShapeTypeMap[" << fullCustomSizeMapList[0].toStdString() << "] = " << that->mySMPShapeTypeMap[fullCustomSizeMapList[0]]);
    }
  }
*/
  // attractor
  BLSURFPlugin::string_array_var allMyAttractors = h->GetAttractorEntries();
//   MESSAGE("myAttractors->length() = " << allMyAttractors->length());

  for ( int i = 0;i<allMyAttractors->length(); ++i ) {
    QString myAttractors =  allMyAttractors[i].in();
    QStringList myAttractorList = myAttractors.split( "|", QString::KeepEmptyParts );
    if ( myAttractorList.count() > 1 ) {
      that->mySMPMap[myAttractorList[0]] = myAttractorList[1];
      that->mySMPShapeTypeMap[myAttractorList[0]] = myGeomToolSelected->entryToShapeType(myAttractorList[0].toStdString());
//       MESSAGE("mySMPMap[" << myAttractorList[0].toStdString() << "] = " << myAttractorList[1].toStdString());
//       MESSAGE("mySMPShapeTypeMap[" << myAttractorList[0].toStdString() << "] = " << that->mySMPShapeTypeMap[myAttractorList[0]]);
    }
  }

  // Enforced vertices
  h_data.enfVertexList.clear();
  h_data.faceEntryEnfVertexListMap.clear();
  /* TODO GROUPS
  h_data.groupNameEnfVertexListMap.clear();
  */

  BLSURFPlugin::TFaceEntryEnfVertexListMap_var faceEntryEnfVertexListMap = h->GetAllEnforcedVerticesByFace();
  MESSAGE("faceEntryEnfVertexListMap->length() = " << faceEntryEnfVertexListMap->length());

  for ( int i = 0;i<faceEntryEnfVertexListMap->length(); ++i ) {
    std::string entry =  faceEntryEnfVertexListMap[i].faceEntry.in();
//     BLSURFPlugin::TEnfVertexList vertexList = faceEntryEnfVertexListMap[i].enfVertexList.in();
    BLSURFPlugin::TEnfVertexList vertexList = faceEntryEnfVertexListMap[i].enfVertexList;
//     BLSURFPlugin::TEnfVertexList_var vertexList = h->GetEnforcedVerticesEntry(entry.c_str());

//     TEnfVertexList& enfVertexList = h_data.faceEntryEnfVertexListMap[entry];

    for (int j=0 ; j<vertexList.length(); ++j) {
      TEnfVertex *enfVertex = new TEnfVertex();
      
      enfVertex->name = CORBA::string_dup(vertexList[j].name.in());
      enfVertex->geomEntry = CORBA::string_dup(vertexList[j].geomEntry.in());
      enfVertex->grpName = CORBA::string_dup(vertexList[j].grpName.in());
      for (int k=0 ; k< vertexList[j].coords.length();k++)
        enfVertex->coords.push_back(vertexList[j].coords[k]);

      h_data.faceEntryEnfVertexListMap[entry].insert(enfVertex);

      /* TODO GROUPS
      if (groupName != "") {
        h_data.groupNameEnfVertexListMap[groupName].insert(ev);
      }
      */
    }
//     h_data.enfVertMap[entry] = evs;
//     h_data.entryCoordsListMap[entry] = coordsList;

    if (h_data.faceEntryEnfVertexListMap[entry].size() == 0) {
      h_data.faceEntryEnfVertexListMap.erase(entry);
    }
  }

  return true;
}

/** BLSURFPluginGUI_HypothesisCreator::storeParamsToHypo(h_data)
Saves the hypothesis data to hypothesis values.
*/
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
    if (h_data.myPhyMin > 0)
      h->SetPhyMin( h_data.myPhyMin );
    if (h_data.myPhyMax > 0)
      h->SetPhyMax( h_data.myPhyMax );
    if (h_data.myGeoMin > 0)
      h->SetGeoMin( h_data.myGeoMin );
    if (h_data.myGeoMax > 0)
      h->SetGeoMax( h_data.myGeoMax );
#endif

    h->SetOptionValues( myOptions ); // is set in checkParams()

    BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;
    QMapIterator<QString,QString> i(that->mySMPMap);
    // Iterate over each size map
    while (i.hasNext()) {
      i.next();
      const QString entry = i.key();
      const QString sizeMap = i.value();

      if (sizeMap == "__TO_DELETE__") {
//         MESSAGE("Delete entry " << entry.toStdString() << " from engine");
        h->UnsetEntry(entry.toLatin1().constData());
      }
      else if (sizeMap.startsWith("ATTRACTOR")) {
//         MESSAGE("SetAttractorEntry(" << entry.toStdString() << ")= " << sizeMap.toStdString());
        h->SetAttractorEntry( entry.toLatin1().constData(), sizeMap.toLatin1().constData() );
      }
      else if (sizeMap.startsWith("def")) {
//         MESSAGE("SetCustomSizeMapEntry(" << entry.toStdString() << ")= " << sizeMap.toStdString());
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

//         MESSAGE("SetSizeMapEntry("<<entry.toStdString()<<") = " <<fullSizeMap.toStdString());
        h->SetSizeMapEntry( entry.toLatin1().constData(), fullSizeMap.toLatin1().constData() );
      }
    }

    // Enforced vertices
    bool ret;
    int hypNbVertex;
    double x, y, z = 0;
    std::string enfName;
    /* TODO GROUPS
    std::string groupName = "";
    */

    TFaceEntryEnfVertexListMap::const_iterator evmIt = h_data.faceEntryEnfVertexListMap.begin();

    BLSURFPlugin::TFaceEntryEnfVertexListMap_var allEnforcedVerticesByFace = h->GetAllEnforcedVerticesByFace();
    hypNbVertex =  allEnforcedVerticesByFace->length();
    

    // All enforced vertices were deleted
    if (evmIt == h_data.faceEntryEnfVertexListMap.end() && hypNbVertex!=0) {
      h->ClearAllEnforcedVertices();
      ok = true;
    }
    else {
      // All enforced vertices for a specific entry were deleted
      for (int i=0 ; i<hypNbVertex; i++) {
        BLSURFPlugin::TFaceEntryEnfVertexListMapElement el = allEnforcedVerticesByFace[i];
        TEnfName entry = el.faceEntry.in();
        if (h_data.faceEntryEnfVertexListMap.find(entry) == h_data.faceEntryEnfVertexListMap.end()) {
          MESSAGE("Remove all enforced vertices for entry " << entry);
          h->UnsetEnforcedVerticesEntry(entry.c_str());
        }
      }

      // One or several enforced vertices are added or removed for a specific entry
      TEnfVertexList enfVertexFromHyp;
      BLSURFPlugin::TEnfVertexList_var hypEnfVertexList;
      TEnfName faceEntry;
      TEnfVertexList evs;
      TEnfVertexList::const_iterator evsIt, enfVertexToRemove;
      for ( ; evmIt != h_data.faceEntryEnfVertexListMap.end() ; ++evmIt) {
        faceEntry = evmIt->first;
        evs = evmIt->second;
        MESSAGE("Number of enforced vertices for face entry " << faceEntry << ": " << evs.size());
        evsIt = evs.begin();

        hypEnfVertexList = h->GetEnforcedVerticesEntry(faceEntry.c_str());
        hypNbVertex = hypEnfVertexList->length();
        MESSAGE("Number of enforced vertices from hypothesis: " << hypNbVertex);
        enfVertexFromHyp.clear();
        for (int i =0 ; i<hypNbVertex ; i++) {
          TEnfVertex *_enfVertex = new TEnfVertex();
          _enfVertex->name = string("");
          _enfVertex->name = CORBA::string_dup(hypEnfVertexList[i].name.in());
          _enfVertex->geomEntry = string("");
          _enfVertex->geomEntry = CORBA::string_dup(hypEnfVertexList[i].geomEntry.in());
          _enfVertex->grpName = string("");
          _enfVertex->grpName = CORBA::string_dup(hypEnfVertexList[i].grpName.in());
          for (int j=0 ; j< hypEnfVertexList[i].coords.length() ; j++)
            _enfVertex->coords.push_back(hypEnfVertexList[i].coords[j]);
          enfVertexFromHyp.insert(_enfVertex);
          MESSAGE("From hyp: enf vertex " << _enfVertex->name);
//           MESSAGE("From hyp: enf. vertex at " << _coords[0]<<", "<<_coords[1]<<", "<<_coords[2]);
        }

//         TEnfVertex *enfVertex;
        for ( ; evsIt != evs.end() ; ++evsIt) {
          x =y =z = 0;
//           enfVertex = (*evsIt);
          if ((*evsIt)->coords.size()) {
            x = (*evsIt)->coords[0];
            y = (*evsIt)->coords[1];
            z = (*evsIt)->coords[2];
          }
          ret = h->SetEnforcedVertexEntry( faceEntry.c_str(), x, y, z, (*evsIt)->name.c_str(), (*evsIt)->geomEntry.c_str(), (*evsIt)->grpName.c_str());
          enfVertexFromHyp.erase((*evsIt));
          
//           for (enfVertexToRemove = enfVertexFromHyp.begin() ; enfVertexToRemove != enfVertexFromHyp.end() ; ++enfVertexToRemove) {
//             if (!TEnfVertexGUICmp(&enfVertex, &(*enfVertexToRemove))) {
//               MESSAGE("Enf vertex  " << enfVertex.name << " must not be deleted")
//               enfVertexFromHyp.erase(enfVertexToRemove);
//             }
//           }
        }

        // Remove old vertices
        enfVertexToRemove = enfVertexFromHyp.begin();
        for ( ; enfVertexToRemove!=enfVertexFromHyp.end() ; ++enfVertexToRemove) {
//           enfVertex = (*enfVertexToRemove);
          if (h_data.enfVertexList.find((*enfVertexToRemove)) != h_data.enfVertexList.end()) {
          
//           for ( evsIt=h_data.enfVertexList.begin();evsIt!=h_data.enfVertexList.end();evsIt++) {
//             if (TEnfVertexGUICmp(&(*evsIt),&enfVertex)) {
              MESSAGE("Remove enf vertex " << (*enfVertexToRemove)->name);
              if ((*enfVertexToRemove)->coords.size()) {
                x = (*enfVertexToRemove)->coords[0];
                y = (*enfVertexToRemove)->coords[1];
                z = (*enfVertexToRemove)->coords[2];
              }
              ret = h->UnsetEnforcedVertexEntry(faceEntry.c_str(), x, y, z, (*enfVertexToRemove)->geomEntry.c_str());
              if (ret)
                MESSAGE("SUCCESS");
//             } // if
//           } // for
          } // if
        } // for
      } // for
    } // else
  } // try
  catch(const std::exception& ex) {
    std::cout << "Exception: " << ex.what() << std::endl;
    throw ex;
  }
//   catch(const SALOME::SALOME_Exception& ex)
//   {
//     throw ex;
// //     SalomeApp_Tools::QtCatchCorbaException(ex);
// //     ok = false;
//   }
  return ok;
}

/** BLSURFPluginGUI_HypothesisCreator::readParamsFromWidgets(h_data)
Stores the widgets content to the hypothesis data.
*/
QString BLSURFPluginGUI_HypothesisCreator::readParamsFromWidgets( BlsurfHypothesisData& h_data ) const
{
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::readParamsFromWidgets");
  h_data.myName             = myName ? myName->text() : "";
  h_data.myTopology         = myTopology->currentIndex();
  h_data.myPhysicalMesh     = myPhysicalMesh->currentIndex();
  h_data.myPhySize          = myPhySize->GetValue();
#ifdef WITH_SIZE_BOUNDARIES
  h_data.myPhyMin           = myPhyMin->GetValue();
  h_data.myPhyMax           = myPhyMax->GetValue();
  h_data.myGeoMin           = myGeoMin->GetValue();
  h_data.myGeoMax           = myGeoMax->GetValue();
#endif
  h_data.myGeometricMesh    = myGeometricMesh->currentIndex();
  h_data.myAngleMeshS       = myAngleMeshS->GetValue();
  h_data.myAngleMeshC       = myAngleMeshC->GetValue();
  h_data.myGradation        = myGradation->GetValue();
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
  guiHyp += "hphymin = " + QString::number( h_data.myPhyMin ) + "; ";
  guiHyp += "hphymax = " + QString::number( h_data.myPhyMax ) + "; ";
  guiHyp += "hgeomin = " + QString::number( h_data.myGeoMin ) + "; ";
  guiHyp += "hgeomax = " + QString::number( h_data.myGeoMax ) + "; ";
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
        guiHyp += "SetSizeMapEntry(" + entry + ", " + that->mySMPMap[entry] + "); ";
  }

  // Enforced vertices
  h_data.enfVertexList.clear();
  h_data.faceEntryEnfVertexListMap.clear();

  int nbEnforcedShapes = myEnforcedTreeWidget->topLevelItemCount();
  int nbEnforcedVertices = 0;
  std::string groupName = "";
//   MESSAGE("Nb of enforced shapes: " << nbEnforcedShapes);
  for (int i=0 ; i<nbEnforcedShapes ; i++) {
    QTreeWidgetItem* shapeItem = myEnforcedTreeWidget->topLevelItem(i);
    if (shapeItem) {
      std::string faceEntry = shapeItem->data(ENF_VER_FACE_ENTRY_COLUMN,Qt::EditRole).toString().toStdString();
      nbEnforcedVertices = shapeItem->childCount();
      if (nbEnforcedVertices >0) {
        double childValueX,childValueY,childValueZ;
        std::string childName, vertexEntry;
        QTreeWidgetItem* child;
        TEnfVertexList evs;
        evs.clear();
        for (row = 0;row<nbEnforcedVertices;row++) {
          child = shapeItem->child(row);
          childName   = child->data(ENF_VER_NAME_COLUMN,Qt::EditRole).toString().toStdString();
          childValueX = child->data(ENF_VER_X_COLUMN,Qt::EditRole).toDouble();
          childValueY = child->data(ENF_VER_Y_COLUMN,Qt::EditRole).toDouble();
          childValueZ = child->data(ENF_VER_Z_COLUMN,Qt::EditRole).toDouble();
          vertexEntry = child->data(ENF_VER_ENTRY_COLUMN,Qt::EditRole).toString().toStdString();
          if (makeGroupsCheck->isChecked())
            groupName = myGlobalGroupName->text().toStdString();
          else
            groupName = child->data(ENF_VER_GROUP_COLUMN,Qt::EditRole).toString().toStdString();

          TEnfVertex *enfVertex = new TEnfVertex();
          enfVertex->name = childName;
          if (vertexEntry.empty()) {
            enfVertex->coords.push_back(childValueX);
            enfVertex->coords.push_back(childValueY);
            enfVertex->coords.push_back(childValueZ);
          }
          else
            enfVertex->geomEntry = vertexEntry;
          enfVertex->grpName = groupName;
//           TEnfVertexList::iterator it = h_data.enfVertexList.find(enfVertex);
//           if (it == h_data.enfVertexList.end())
          h_data.enfVertexList.insert(enfVertex);
          evs.insert(enfVertex);
          /* TODO GROUPS
          if (groupName != "")
            h_data.groupNameEnfVertexListMap[groupName].insert(vertex);
          */
        }
        h_data.faceEntryEnfVertexListMap[faceEntry] = evs;
      }
    }
  }

  MESSAGE("guiHyp : " << guiHyp.toLatin1().data());
  return guiHyp;
}

void BLSURFPluginGUI_HypothesisCreator::onPhysicalMeshChanged() {
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::onPhysicalMeshChanged");
  bool isPhysicalUserDefined = (myPhysicalMesh->currentIndex() == PhysicalUserDefined);
  bool isSizeMap = (myPhysicalMesh->currentIndex() == SizeMap);
  bool isCustom = (isPhysicalUserDefined || isSizeMap) ;
  bool geomIsCustom = (myGeometricMesh->currentIndex() == UserDefined);

  myGradation->setEnabled(!isPhysicalUserDefined || geomIsCustom);
  myPhySize->setEnabled(isCustom);
  myPhyMax->setEnabled(isCustom);
  myPhyMin->setEnabled(isCustom);

  if ( !myGradation->isEnabled())
    myGradation->SetValue( 1.1 );

  if ( !isCustom ) {
    if ( myGeometricMesh->currentIndex() == DefaultGeom ) {
      myGeometricMesh->setCurrentIndex( UserDefined );
      onGeometricMeshChanged();
    }
  }
}

void BLSURFPluginGUI_HypothesisCreator::onGeometricMeshChanged() {
  MESSAGE("BLSURFPluginGUI_HypothesisCreator::onGeometricMeshChanged");
  bool isCustom = (myGeometricMesh->currentIndex() == UserDefined);
  bool phyIsSizemap = (myPhysicalMesh->currentIndex() == SizeMap);

  myAngleMeshS->setEnabled(isCustom);
  myAngleMeshC->setEnabled(isCustom);
  myGradation->setEnabled(isCustom || phyIsSizemap);
  myGeoMax->setEnabled(isCustom);
  myGeoMin->setEnabled(isCustom);

  if ( !myGradation->isEnabled())
    myGradation->SetValue( 1.1 );

  if ( ! isCustom ) {
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
//     MESSAGE("entry: " << entry.toStdString() << ", sizeMap: " << sizeMap.toStdString());
    if (! that->mySMPShapeTypeMap.contains(entry))
      return;
    if (that->mySMPMap.contains(entry))
      if (that->mySMPMap[entry] == sizeMap)
        return;
    QColor* bgColor = new QColor("white");
    QColor* fgColor = new QColor("black");
    if (! sizeMap.isEmpty()) {
      that->mySMPMap[entry] = sizeMap;
      if (! sizeMapValidationFromRow(row)) {
        bgColor->setRgb(255,0,0);
        fgColor->setRgb(255,255,255);
      }
    }
    else {
//       MESSAGE("Size map empty: reverse to precedent value" );
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
//   MESSAGE("BLSURFPluginGUI_HypothesisCreator::insertElementType()");

  BLSURFPlugin::BLSURFPlugin_Hypothesis_var h =
    BLSURFPlugin::BLSURFPlugin_Hypothesis::_narrow( initParamsHypothesis());

  BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;
  GeomSelectionTools* myGeomToolSelected = that->getGeomSelectionTool();

  LightApp_SelectionMgr* mySel = myGeomToolSelected->selectionMgr();

  TopAbs_ShapeEnum shapeType;
  string entry, shapeName;
  SALOME_ListIO ListSelectedObjects;
  mySel->selectedObjects(ListSelectedObjects, NULL, false );
  if (!ListSelectedObjects.IsEmpty())
  {
    SALOME_ListIteratorOfListIO Object_It(ListSelectedObjects);
    for (; Object_It.More(); Object_It.Next())
    {
      Handle(SALOME_InteractiveObject) anObject = Object_It.Value();
      entry     = myGeomToolSelected->getEntryOfObject(anObject);
      shapeName = anObject->getName();
      shapeType         = myGeomToolSelected->entryToShapeType(entry);
//       MESSAGE("Object Name = " << shapeName << "& Type is " << anObject->getComponentDataType() << " & ShapeType is " << shapeType);
      if (shapeType == typeShapeAsked)
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
//             MESSAGE("Size map for shape with name(entry): "<< shapeName << "(" << entry << ")");
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
//   MESSAGE("BLSURFPluginGUI_HypothesisCreator::sizeMapsValidation()");
  int row = 0, nbRows = mySizeMapTable->rowCount();
  for ( ; row < nbRows; ++row )
    if (! sizeMapValidationFromRow(row))
      return false;
  return true;
}

bool BLSURFPluginGUI_HypothesisCreator::sizeMapValidationFromRow(int myRow, bool displayError)
{
//   MESSAGE("BLSURFPluginGUI_HypothesisCreator::sizeMapValidationFromRow()");
  QString myEntry   = mySizeMapTable->item( myRow, SMP_ENTRY_COLUMN )->text();
  bool res = sizeMapValidationFromEntry(myEntry,displayError);
  mySizeMapTable->setFocus();
  return res;
}

bool BLSURFPluginGUI_HypothesisCreator::sizeMapValidationFromEntry(QString myEntry, bool displayError)
{
//   MESSAGE("BLSURFPluginGUI_HypothesisCreator::sizeMapValidationFromEntry()");

  BLSURFPluginGUI_HypothesisCreator* that = (BLSURFPluginGUI_HypothesisCreator*)this;

  if (! that->mySMPMap.contains(myEntry)) {
//     MESSAGE("Geometry with entry "<<myEntry.toStdString()<<" was not found.");
    return false;
  }
  if (! that->mySMPShapeTypeMap.contains(myEntry)) {
//     MESSAGE("Shape type with entry "<<myEntry.toStdString()<<" was not found.");
    return false;
  }

  string expr;

  if (that->mySMPMap[myEntry].startsWith("def")) {
//     MESSAGE("custom function" );
    expr = that->mySMPMap[myEntry].toStdString();
  }
  else if (that->mySMPMap[myEntry].startsWith("ATTRACTOR")) {
//     MESSAGE("Attractor" );
    if ((that->mySMPMap[myEntry].count(QRegExp("^ATTRACTOR\\((?:(-?0(\\.\\d*)*|-?[1-9]+\\d*(\\.\\d*)*|-?\\.(\\d)+);){5}(True|False)\\)$")) != 1)) {
      if (displayError)
        SUIT_MessageBox::warning( dlg(),"Definition of attractor : Error" ,"An attractor is defined with the following pattern: ATTRACTOR(xa;ya;za;a;b;True|False[;d])" );
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
    else {
      if ( that->mySMPShapeTypeMap[myEntry] == TopAbs_FACE)
        expr = "def f(u,v) : return " + that->mySMPMap[myEntry].toStdString();
      else if ( that->mySMPShapeTypeMap[myEntry] == TopAbs_EDGE)
        expr = "def f(t) : return " + that->mySMPMap[myEntry].toStdString();
      else if ( that->mySMPShapeTypeMap[myEntry] == TopAbs_VERTEX)
        expr = "def f() : return " + that->mySMPMap[myEntry].toStdString();
    }
  }
  //assert(Py_IsInitialized());
  if (! Py_IsInitialized())
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
    PySys_SetObject((char*)"stderr", new_stderr);
    PyErr_Print();
    PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
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
    PySys_SetObject((char*)"stderr", new_stderr);
    PyErr_Print();
    PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
    Py_DECREF(new_stderr);
    if (displayError)
      SUIT_MessageBox::warning( dlg(),"Python Error" ,err_description.c_str() );
    PyGILState_Release(gstate);
    return false;
  }

  PyGILState_Release(gstate);

//   MESSAGE("SizeMap expression "<<expr<<" is valid");

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


