// Copyright (C) 2007-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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


//////////////////////////////////////////
// BLSURFPluginGUI_AdvWidget
//////////////////////////////////////////

BLSURFPluginGUI_AdvWidget::BLSURFPluginGUI_AdvWidget( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );
  myOptionTable->horizontalHeader()->hideSection( OPTION_ID_COLUMN );
}

BLSURFPluginGUI_AdvWidget::~BLSURFPluginGUI_AdvWidget()
{
}

void BLSURFPluginGUI_AdvWidget::onChooseGMFFile() {
  QString fileName = QFileDialog::getSaveFileName(0, tr("BLSURF_GMF_FILE_DIALOG"), myGMFFileName->text(), tr("BLSURF_GMF_FILE_FORMAT"));
  std::cout << "fileName: " << fileName.toStdString() << std::endl;
  if (!fileName.endsWith(".mesh") && !fileName.endsWith(".meshb"))
    fileName.append(".mesh");
  myGMFFileName->setText(fileName);
}


