
// Copyright (C) 2009-2024  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef _TEST_UNIT_H_
#define _TEST_UNIT_H_

#include "HexTest.hxx"
#include "hexa_base.hxx"

HEXABLOCKTEST_EXPORT void goto_workspace ();
HEXABLOCKTEST_EXPORT void free_workspace ();

HEXABLOCKTEST_EXPORT int test_sphere (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_joint (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_prism (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_revolution9 (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_revolution (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_coude (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_count (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_decoupage (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_gen_xml (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_string_xml (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_relecture (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_spherical (int nbargs, const char* tabargs[]);
HEXABLOCKTEST_EXPORT int test_grille_cyl (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_asso_line (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_cylindrical (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_cylinder (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_xml_cylinder (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_pipe (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_joint2 (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_croix (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_pipes (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_lorraine(int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_disconnect2 (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_disconnect4 (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_disconnect1 (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_disconnect3 (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_disconnect (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_transfo2 (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_transfo (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_copy_document (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_cylindricals (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_hexa (int nbargs, cpchar tabargs[]);
                 // Dans test_quads
HEXABLOCKTEST_EXPORT int test_hexa_quads_5 (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_hexa_quads_ab (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_hexa_quads_ac (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_hexa_quads_ace (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_hexa_quads_acd (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_hexa_quads_abcd (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_hexa_quads_abce (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_cramer (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_hexa_quads_ac1 (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_asso_grid (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_piquage (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_replace (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_hemispheres (int nbargs, cpchar tabargs[]);
HEXABLOCKTEST_EXPORT int test_quads (int nbargs, cpchar tabargs[]);
#endif
