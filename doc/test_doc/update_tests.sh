#!/bin/sh
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

