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

#include <stdlib.h>

#include "HEXABLOCKGUI_Exception.hxx"

using namespace HEXABLOCK::GUI;

//#define _DEVDEBUG_

#ifdef _DEVDEBUG_
#include <execinfo.h>
#endif

Exception::Exception(const std::string& what):_what(what)
{
#ifdef _DEVDEBUG_
  void *array[20];
  size_t size=10;
  char **strings;
  size_t i;

  size = backtrace (array, 10);
  strings = backtrace_symbols (array, size);

  _what=_what+'\n';
  for (i = 0; i < size; i++)
     _what=_what+strings[i]+'\n';

  free (strings);
#endif
}

const char *Exception::what( void ) const noexcept
{
  return _what.c_str();
}

Exception::~Exception() noexcept
{
}
