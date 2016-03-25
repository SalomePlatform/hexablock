dnl Copyright (C) 2009-2016  CEA/DEN, EDF R&D
dnl
dnl This library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Lesser General Public
dnl License as published by the Free Software Foundation; either
dnl version 2.1 of the License, or (at your option) any later version.
dnl
dnl This library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl Lesser General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public
dnl License along with this library; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
dnl
dnl See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
dnl
#  Check availability of HEXABLOCK module binary distribution
#
#  Author : Marc Tajchman (CEA, 2002)
#------------------------------------------------------------

AC_DEFUN([CHECK_HEXABLOCK],[

AC_CHECKING(for HEXABLOCK)

HEXABLOCK_ok=no

HEXABLOCK_LDFLAGS=""
HEXABLOCK_CXXFLAGS=""

AC_ARG_WITH(hexablock,
	    --with-hexablock=DIR root directory path of HEXABLOCK installation,
	    HEXABLOCK_DIR="$withval",HEXABLOCK_DIR="")

if test "x$HEXABLOCK_DIR" = "x" ; then

# no --with-gui-dir option used

  if test "x$HEXABLOCK_ROOT_DIR" != "x" ; then

    # HEXABLOCK_ROOT_DIR environment variable defined
    HEXABLOCK_DIR=$HEXABLOCK_ROOT_DIR

  else

    # search hexablock binaries in PATH variable
    AC_PATH_PROG(TEMP, libHEXABLOCK.so)
    if test "x$TEMP" != "x" ; then
      HEXABLOCK_BIN_DIR=`dirname $TEMP`
      HEXABLOCK_DIR=`dirname $HEXABLOCK_BIN_DIR`
    fi

  fi
#
fi

if test -f ${HEXABLOCK_DIR}/lib/salome/libHEXABLOCK.so  ; then
  HEXABLOCK_ok=yes
  AC_MSG_RESULT(Using HEXABLOCK distribution in ${HEXABLOCK_DIR})

  if test "x$HEXABLOCK_ROOT_DIR" == "x" ; then
    HEXABLOCK_ROOT_DIR=${HEXABLOCK_DIR}
  fi
  AC_SUBST(HEXABLOCK_ROOT_DIR)

  HEXABLOCK_LDFLAGS=-L${HEXABLOCK_DIR}/lib${LIB_LOCATION_SUFFIX}/salome
  HEXABLOCK_CXXFLAGS=-I${HEXABLOCK_DIR}/include/salome
    
  AC_SUBST(HEXABLOCK_LDFLAGS)
  AC_SUBST(HEXABLOCK_CXXFLAGS)

else
  AC_MSG_WARN("Cannot find compiled HEXABLOCK distribution")
fi
  
AC_MSG_RESULT(for HEXABLOCK: $HEXABLOCK_ok)
 
])dnl
 
