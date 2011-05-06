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
