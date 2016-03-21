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

# Comm : Generateur de Makefile

commande=$0
liste_args="$*"

# =============================================== al_aide
al_aide()
{
proc=`basename $commande`
cat << END_HELP
Syntaxe :
    $    $proc [ options ] <fichiers_source>
Effet :
    Genere un Makefile tenant compte des fichiers sources passes en argument
    et des includes appeles par ces fichiers.
Avec :
    <fichiers_sources> liste des fichiers *.c *.cc *.cpp *.cxx et *.C
                       pris en compte pour constituer le Makefile.
                      Si des fichiers *.h sont presents, c'est pour generer des
                       fichiers qt (utilitaire moc)
    Options:
         Make=<fic>  : Nom du makefile a generer
         opt=val      : Option passee
         <autres>     : options de compilation
                        (par exemple le repertoire des includes)
END_HELP
exit
}
# =============================================== ana_arguments
ana_arguments()
{
liste_src=
liste_rad=
liste_qrc=
liste_bnf=
liste_moc=
for arg in $liste_args
    do
    case $arg in
         -setx )
             set -x
             ;;
         -h | --help  )
             al_aide
             ;;
         Make=* | make=* )
             make_file=`echo $arg | cut -f2 -d=`
             ;;
         * )
             arg_gcc="$arg_gcc $arg"
             ;;
    esac
    done

for arg in *.cxx
    do
    case $arg in

         \*.* ) :
             ;;

         *.cpp | *.cc | *.cxx | *.C | *.c )
             rad=`echo $arg | cut -f1 -d.`
             liste_src="$liste_src $arg"
             liste_rad="$liste_rad $rad"
             ;;
         * ) :
             ;;
    esac
    done
}
# =============================================== put_includes
put_includes()
{
   if ( [ -z "$3" ] ) then
      return
      fi

   obj=$repobj/$1
   shift 2

   for mot in $*
       do
       case $mot in
           \\* ) : ;;

           */Qt/* ) : ;;
           */VTK* ) : ;;

           * ) echo $obj $mot
              ;;
       esac
       done
}
# =============================================== init_par_defaut
init_par_defaut()
{
os=`uname`
if ( [ $os = Linux ] ) then
   opt_pic=-fPIC
   fi

hdebut=`date '+%Hh %Mmn %Ss'`
make_file=Makefile
make_file=makefile.ozz
antislash=\\
liste_src=
liste_rad=
arg_gcc=
arg_plus=
make_body=OK
make_ref=OK

repobj='$(REPOBJ)'
replib='$(REPLIB)'
repexe='$(REPEXE)'
opt_user='$(OPT)'
compile='$(CPIL)'
archive='$(ARCH)'
link='$(LINK)'

val_repobj=.
cmd_cpil="g++ -c -g $opt_pic -Wall -DNO_CASCADE -Inokas --pedantic"
cmd_arch='ar rcvu $(BIBLI) *.o'
cmd_link='g++ -g'
uf=`pwd`
uf=`basename "$uf"`

ext=
namof_lib=lib$uf.a

comm=`basename $commande`
reptools=`dirname $commande`
                   ###  Definition des includes par $ozz_opt_cpp

if ( [ -n "$OZZ_FLAGS" -a -f "$OZZ_FLAGS" ] ) then
   . $OZZ_FLAGS
fi
}
# =============================================== add_entete
add_entete()
{
today=`date "+%d/%m/%y a %Hh%M"`
echo
echo  "# Makefile OZZ genere le $today"
echo  "# Par la commande : $comm "
echo  "# Role de ce makefile : "
echo  "#  - Compiler les fichiers-source defraichis avec la commande \$CPIL"
echo  "#  - Les archiver sur le fichier archive \$REPLIB/\$BIBLI"
echo  "#  - Linker le programme principal (test unitaire) passe en argument"
echo
echo ".SUFFIXES:"         ## Pour ne pas etre emm....
echo "OPT    = "          ## Options de compilation de l'appelant
echo "REPOBJ = ."
echo "REPLIB = ."
echo "REPEXE = ./work"
echo "BIBLI  = $namof_lib"
echo "ARCH   = $cmd_arch"
echo "CPIL   = $cmd_cpil"
echo "LINK   = $cmd_link"
echo "main   = test_hexa"
echo

precedente="OBJECTS  ="

for module in $liste_rad
    do
    echo "$precedente \\"
    precedente="      $repobj/$module.o"
    done

cat << FIN
$precedente

all  : saut link

saut :
	@ echo ; echo ; echo ; echo ; echo ; echo ; echo ; echo ; echo ; echo

link     : $repexe/\$(main).exe

$repexe/\$(main).cpp  :
	./make.main \$(main)

$repexe/\$(main).exe  : $repexe/\$(main).cpp  \$(REPLIB)/\$(BIBLI)
	\$(LINK) $repexe/\$(main).cpp  \$(REPLIB)/\$(BIBLI) -o $repexe/\$(main).exe

\$(REPLIB)/\$(BIBLI) : \$(OBJECTS)
	$archive

clean :
	rm -rf *.exe \$(REPLIB)/\$(BIBLI) \$(REPOBJ)/*.o *.bak *~

cleanobj :
	rm -rf \$(REPLIB)/\$(BIBLI) \$(REPOBJ)/*.o *.bak *~

FIN
}
# =============================================== add_sources
add_sources()
{
echo
echo  "                     ### Compilations"
echo
for arg in $liste_src
    do
    radical=`echo $arg | cut -f1 -d.`
    echo "$repobj/$radical.o : $arg"
    echo "	$compile $opt_user $arg"
    done
}
# =============================================== Begin
init_par_defaut
echo
echo " ... Generation de $make_file qui met a jour $namof_lib ..."

ana_arguments
./make_dummycad

rm -rf $make_file
add_entete  >> $make_file
add_sources >> $make_file

makedepend -f $make_file -Y -I. $liste_src 2> /dev/null

hfin=`date '+%Hh %Mmn %Ss'`
echo " ... Fin de la generation : $hfin"
echo " ... Elle avait debute a  : $hdebut"
