# Copyright (C) 2009-2023  CEA/DEN, EDF R&D
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
import sys
import os
from argparse import ArgumentParser

def process (file_name):
    """
    cette methode prend en entree le nom d'un fichier vtk cree par le
    module HEXABLOCK de Salome, et remplace les "," par des "."
    """

    wr_data = ""
    with open(file_name, 'r', encoding='utf8') as f:
        read_data = f.read()
        wr_data = read_data.replace(',', '.')
        pass

    with open(file_name, 'w', encoding='utf8') as f:
        f.write(wr_data)
        pass

    pass

if __name__ == '__main__':

    parser = ArgumentParser()
    parser.add_argument('file_name', help='Nom du fichier vtk dans $TMP')
    args = parser.parse_args()
    
    file_name = os.path.join(os.environ['TMP'], args.file_name)

    print(file_name)
    process(file_name)
    
    sys.exit() 
