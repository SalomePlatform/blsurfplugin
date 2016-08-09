// Copyright (C) 2007-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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
// File    : BLSURFPluginGUI_Dlg.cxx
// Authors : Gilles DAVID (OCC)
// ---
//

#include "BLSURFPluginGUI_Dlg.h"

#include <QFileDialog>

#include <iostream>

namespace
{
  enum { EDITABLE_ROLE = Qt::UserRole + 1, PARAM_NAME,
         NAME_COL = 0, VALUE_COL };

  class ItemDelegate: public QItemDelegate {
  public:
    ItemDelegate(QObject* parent=0): QItemDelegate(parent) {}
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &o, const QModelIndex &index) const
    {
      bool editable = index.data( EDITABLE_ROLE ).toInt();
      return editable ? QItemDelegate::createEditor( parent, o, index ) : 0;
    }
  };
}

//////////////////////////////////////////
// BLSURFPluginGUI_AdvWidget
//////////////////////////////////////////

BLSURFPluginGUI_AdvWidget::BLSURFPluginGUI_AdvWidget( QWidget* parent, Qt::WindowFlags f )
  : QWidget( parent, f )
{
  setupUi( this );
  myOptionTable->topLevelItem( 2 )->setHidden( true ); // hide CUSTOM options
  myOptionTable->header()->setSectionResizeMode( QHeaderView::ResizeToContents );
  myOptionTable->setItemDelegate( new ItemDelegate( myOptionTable ) );

  connect( myOptionTable, SIGNAL( itemChanged(QTreeWidgetItem *, int)), SLOT( itemChanged(QTreeWidgetItem *, int )));
}

BLSURFPluginGUI_AdvWidget::~BLSURFPluginGUI_AdvWidget()
{
}

void BLSURFPluginGUI_AdvWidget::onChooseGMFFile()
{
  QString fileName = QFileDialog::getSaveFileName(0, tr("BLSURF_GMF_FILE_DIALOG"), myGMFFileName->text(), tr("BLSURF_GMF_FILE_FORMAT"));
  std::cout << "fileName: " << fileName.toStdString() << std::endl;
  if (!fileName.endsWith(".mesh") && !fileName.endsWith(".meshb"))
    fileName.append(".mesh");
  myGMFFileName->setText(fileName);
}

void BLSURFPluginGUI_AdvWidget::AddOption( int iTable, const char* option )
{
  if ( iTable < 0 || iTable > 2 ) return;

  QTreeWidgetItem * table = myOptionTable->topLevelItem( iTable );
  table->setExpanded( true );

  QTreeWidgetItem * row = new QTreeWidgetItem( table );
  row->setData( NAME_COL, EDITABLE_ROLE, int( iTable == 2 && !option ));
  row->setFlags( row->flags() | Qt::ItemIsEditable );

  QString name, value;
  bool isDefault = false;
  if ( option )
  {
    QStringList name_value_type = QString(option).split( ":", QString::KeepEmptyParts );
    if ( name_value_type.size() > 0 )
      name = name_value_type[0];
    if ( name_value_type.size() > 1 )
      value = name_value_type[1];
    if ( name_value_type.size() > 2 )
      isDefault = !name_value_type[2].toInt();
  }
  row->setText( 0, tr( name.toLatin1().constData() ));
  row->setText( 1, tr( value.toLatin1().constData() ));
  row->setCheckState( 0, isDefault ? Qt::Unchecked : Qt::Checked);
  row->setData( NAME_COL, PARAM_NAME, name );

  if ( iTable == 2 )
  {
    table->setHidden( false );
    if ( !option )
    {
      myOptionTable->scrollToItem( row );
      myOptionTable->editItem( row, NAME_COL );
    }
  }
}

void BLSURFPluginGUI_AdvWidget::GetOptionAndValue( QTreeWidgetItem * tblRow,
                                                   QString&          option,
                                                   QString&          value,
                                                   bool&             isDefault)
{
  option    = tblRow->data( NAME_COL, PARAM_NAME ).toString();
  value     = tblRow->text( VALUE_COL );
  isDefault = ! tblRow->checkState( NAME_COL );
}


void BLSURFPluginGUI_AdvWidget::itemChanged(QTreeWidgetItem * tblRow, int column)
{
  if ( tblRow )
  {
    myOptionTable->blockSignals( true );

    tblRow->setData( VALUE_COL, EDITABLE_ROLE, int( tblRow->checkState( NAME_COL )));

    int c = tblRow->checkState( NAME_COL ) ? 0 : 150;
    tblRow->setForeground( VALUE_COL, QBrush( QColor( c, c, c )));

    if ( column == NAME_COL && tblRow->data( NAME_COL, EDITABLE_ROLE ).toInt() ) // custom table
    {
      tblRow->setData( NAME_COL, PARAM_NAME, tblRow->text( NAME_COL ));
    }

    myOptionTable->blockSignals( false );
  }
}
