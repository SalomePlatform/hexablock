// Copyright (C) 2009-2016  CEA/DEN, EDF R&D
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

#include "hexa_base.hxx"
#include "HEXABLOCKGUI_Model.hxx"

//#define _DEVDEBUG_
#include "HEXABLOCKGUI_Trace.hxx"



HEXABLOCKGUI_Model::HEXABLOCKGUI_Model(CAM_Module* theModule)
  : SalomeApp_DataModel(theModule)
{
  DEBTRACE("HEXABLOCKGUI_Model::HEXABLOCKGUI_Model");
  std::cout <<  "HEXABLOCKGUI_Model::HEXABLOCKGUI_Model" << std::endl;
}

HEXABLOCKGUI_Model::~HEXABLOCKGUI_Model()
{
  DEBTRACE("HEXABLOCKGUI_Model:: ~HEXABLOCKGUI_Model");
}
  
bool HEXABLOCKGUI_Model::open(const QString& fileName, CAM_Study* study, QStringList listOfFiles)
{
  DEBTRACE("HEXABLOCKGUI_Model::open");
  return SalomeApp_DataModel::open(fileName, study, listOfFiles);
}

bool HEXABLOCKGUI_Model::save(QStringList& listOfFiles)
{
  DEBTRACE("HEXABLOCKGUI_Model::save");
  std::cout <<  "HEXABLOCKGUI_Model::save" << std::endl;
  return SalomeApp_DataModel::save(listOfFiles);
}

bool HEXABLOCKGUI_Model::saveAs(const QString& fileName, CAM_Study* study, QStringList& listOfFiles)
{
  DEBTRACE("HEXABLOCKGUI_Model::saveAs");
  std::cout <<  "HEXABLOCKGUI_Model::saveAs" << std::endl;
  return SalomeApp_DataModel::saveAs(fileName, study, listOfFiles);
}
