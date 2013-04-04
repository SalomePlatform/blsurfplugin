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
	BLSURFPlugin_BLSURF.hxx	\
	BLSURFPlugin_BLSURF_i.hxx	\
	BLSURFPlugin_Hypothesis.hxx	\
	BLSURFPlugin_Hypothesis_i.hxx   \
	BLSURFPlugin_Attractor.hxx

# Libraries targets
lib_LTLIBRARIES = libBLSURFEngine.la

dist_libBLSURFEngine_la_SOURCES =	\
	BLSURFPlugin_BLSURF.cxx		\
	BLSURFPlugin_BLSURF_i.cxx	\
	BLSURFPlugin_Hypothesis.cxx	\
	BLSURFPlugin_Hypothesis_i.cxx	\
	BLSURFPlugin_i.cxx              \
	BLSURFPlugin_Attractor.cxx

libBLSURFEngine_la_CPPFLAGS =		\
	$(QT_INCLUDES)          	\
	$(PYTHON_INCLUDES)      	\
	$(KERNEL_CXXFLAGS)		\
	$(GUI_CXXFLAGS)			\
	$(MED_CXXFLAGS)			\
	$(GEOM_CXXFLAGS)		\
	$(CAS_CPPFLAGS)			\
	$(VTK_INCLUDES)			\
	$(MESHGEMS_CADSURF_INCLUDES)	\
	$(SMESH_CXXFLAGS)		\
	$(CORBA_CXXFLAGS)		\
	$(CORBA_INCLUDES)		\
	$(BOOST_CPPFLAGS)		\
	-I$(top_builddir)/idl

#Qt uniquement necessaire pour le getActiveStudyDocument de SMeshGuiUtils.h

libBLSURFEngine_la_LDFLAGS  =			\
	../../idl/libSalomeIDLBLSURFPLUGIN.la	\
	$(PYTHON_LIBS) \
	$(CAS_KERNEL) -lTKBRep -lTKGeomBase -lTKGeomAlgo -lTKTopAlgo -lTKLCAF -lTKXSBase -lTKG2d -lTKG3d -lTKShHealing \
	$(MESHGEMS_CADSURF_LIBS) \
	$(SMESH_LDFLAGS) -lSMESHimpl -lSMESHEngine -lStdMeshers -lStdMeshersEngine -lSMDS -lSMESHDS \
	$(GEOM_LDFLAGS) -lGEOMbasic \
	$(MED_LDFLAGS) -lSalomeIDLMED \
	$(KERNEL_LDFLAGS) -lOpUtil -lSalomeGenericObj -lSalomeNS -lSALOMELocalTrace -lSALOMEBasics \
	$(BOOST_LIB_REGEX)

# Scripts to be installed.
#dist_salomescript_DATA= BLSURFPluginDC.py
mypkgpythondir = $(salomepythondir)/salome/BLSURFPlugin
mypkgpython_PYTHON = \
	__init__.py \
	BLSURFPluginBuilder.py
