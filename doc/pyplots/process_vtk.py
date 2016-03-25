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
import sys
import os
from optparse import OptionParser

def process (file_name):
    """
    cette methode prend en entree le nom d'un fichier vtk cree par le
    module HEXABLOCK de Salome, et remplace les "," par des "."
    """

    wr_data = ""
    with open(file_name, 'r') as f:
        read_data = f.read()
        wr_data = read_data.replace(',', '.')
        pass

    with open(file_name, 'w') as f:
        f.write(wr_data)
        pass

    pass

if __name__ == '__main__':

    usage = "usage: %prog file_name"
    parser = OptionParser(usage=usage)
    (options, args) = parser.parse_args()

    if len(args) != 1:
        print usage
        sys.exit(1)
    
    file_name = os.path.join(os.environ['TMP'], args[0])

    print file_name
    process(file_name)
    
    sys.exit() 
