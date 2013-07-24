# Copyright (C) 2007-2013  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

# ---
# File   : Makefile.am
# Author : Vadim SANDLER, Open CASCADE S.A.S (vadim.sandler@opencascade.com)
# ---
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 
salomeinclude_HEADERS =	\
	BLSURFPluginGUI_Dlg.h	\
	BLSURFPluginGUI_HypothesisCreator.h

# Libraries targets
lib_LTLIBRARIES = libBLSURFPluginGUI.la

dist_libBLSURFPluginGUI_la_SOURCES = \
	BLSURFPluginGUI.cxx \
	BLSURFPluginGUI_StdWidget.cxx \
	BLSURFPluginGUI_AdvWidget.cxx \
	BLSURFPluginGUI_HypothesisCreator.cxx

MOC_FILES = \
	BLSURFPluginGUI_HypothesisCreator_moc.cxx	\
	BLSURFPluginGUI_Dlg_moc.cxx

UIC_FILES =	\
	ui_BLSURFPluginGUI_StdWidget_QTD.h	\
	ui_BLSURFPluginGUI_AdvWidget_QTD.h

BUILT_SOURCES = $(UIC_FILES)

nodist_libBLSURFPluginGUI_la_SOURCES = \
	$(MOC_FILES)

# additionnal information to compil and link file

libBLSURFPluginGUI_la_CPPFLAGS = 	\
	$(QT_INCLUDES)			\
	$(CAS_CPPFLAGS)			\
	$(PYTHON_INCLUDES)		\
	$(KERNEL_CXXFLAGS)		\
	$(GUI_CXXFLAGS)			\
	$(GEOM_CXXFLAGS)		\
	$(SMESH_CXXFLAGS)		\
	$(VTK_INCLUDES)	\
	$(BOOST_CPPFLAGS)		\
	$(CORBA_CXXFLAGS)		\
	$(CORBA_INCLUDES)		\
	$(MESHGEMS_CADSURF_INCLUDES)	\
	-I$(srcdir)/../BLSURFPlugin	\
	-I$(top_builddir)/idl

libBLSURFPluginGUI_la_LDFLAGS =			\
	$(QT_LIBS) $(QT_MT_LIBS)\
	../BLSURFPlugin/libBLSURFEngine.la	\
	../../idl/libSalomeIDLBLSURFPLUGIN.la \
	$(GUI_LDFLAGS) -lqtx -lSalomeApp -lsuit -lSalomeObject -lLightApp	\
	$(GEOM_LDFLAGS) -lGEOM \
	${SMESH_LDFLAGS} -lSMESH  -lSMESHEngine -lGeomSelectionTools -lStdMeshersGUI -lSMESHFiltersSelection \
	$(CAS_KERNEL) $(MESHGEMS_CADSURF_LIBS)

# resources files
nodist_salomeres_DATA =		\
	BLSURFPlugin_images.qm	\
	BLSURFPlugin_msg_en.qm  \
	BLSURFPlugin_msg_fr.qm  \
	BLSURFPlugin_msg_ja.qm
