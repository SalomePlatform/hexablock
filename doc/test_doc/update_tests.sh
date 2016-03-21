#!/bin/sh
# Copyright (C) 2009-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

# Comm : Update python procedures

start=`dirname $0`
target=`cd $start/../../src/TEST_PY/documentation ; pwd`
start=`cd $start ; pwd`

cd $start
for rep in *
    do
    if ( [ -d $start/$rep ] ) then
       mkdir -p $target/$rep
       for fic in $rep/*.py 
           do
           if ( [ ! -f  $target/$fic ] ) then
                 echo " ____________________________ Add File : $fic"
              cp  $fic  $target/$rep/
           else
              diff=`diff -q $fic $target/$rep/`
              if ( [ "$diff" ] ) then
                 echo " ---------------------------- Update   : $fic"
                 cp  $fic  $target/$rep/
              else
                 echo " . . . . . . . . . . . Identical files : $fic"
              fi
           fi
           done
       fi
    done

