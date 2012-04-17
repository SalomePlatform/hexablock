
// C++ : Outils communs aux tests unitaires

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
//  See http://www.salome-platform.org/ 
//  or email : webmaster.salome@opencascade.com


#include "test_unit.hxx"

#include <string>
#include <cstdlib>
#include <unistd.h>

static int nbr_vtk = 0;
static cpchar case_name = "hexa";
static Hex::Document*   docu = NULL;

static string workspace = "/tmp/test_hexablock";

// ======================================================== call_system 
void call_system (const string&  command)
{
   system (command.c_str());
}
// ======================================================== goto_workspace
void goto_workspace ()
{
   string rmdir = "rm -rf ";
   string mkdir = "mkdir -p ";

   call_system (rmdir + workspace);
   call_system (mkdir + workspace);

   chdir (workspace.c_str());
}
// ======================================================== free_workspace
void free_workspace ()
{
   string chmod = "chmod 777 ";

   call_system (chmod + workspace);
   call_system (chmod + workspace + "/*");
}
