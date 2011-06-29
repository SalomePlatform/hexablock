//  Copyright (C) 2009-2011  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// Incl : Diagnostics

#ifndef _HEX_DIAGNOSTICS_H
#define _HEX_DIAGNOSTICS_H

#define W_H_NULL_QUAD "new Hexa : La face %s est nulle"
#define W_H_EQ_QUAD "new Hexa : Les faces %s et %s sont confondues"
#define W_H_BAD_EDGE "new Hexa : L'arete commune %s n'existe pas"
#define W_H_BAD_VERTEX "new Hexa : Le sommet commun %s n'existe pas"

#define W_H_NULL_VERTEX "addHexa : Le sommet %s est nul"
#define W_H_EQ_VERTEX   "addHexa : Les sommets %s et %s sont confondus"
#define W_REPL_ASSOC "mergeXXXX : Les elements fusionnes %s et %s sont associes"
#define W_DISCO_ASSOC "disconnextXXXX : L'element deconnecte %s etait associe"


#define W_ASSO_LINE  "associateLine : l'edge de depart (%s) est isole"
#define W_ASSO_LINE2 "associateLine : l'edge nro %s (%s) est isole"
#define W_ASSO_LINE1 "associateLine : le vertex de depart (%s) est isole"

#define W_ASSO_LINE3 "associateLine : le vertex de depart est incorrect"
#define W_ASSO_LINE4 "associateLine : l'edge de depart est incorrect"
#define W_ASSO_LINE5 "associateLine : l'edge nro %s est incorrect"
#define W_ASSO_LINE6 "associateLine : Le contour n'est pas ferme"

#endif
