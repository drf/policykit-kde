/*
  Copyright (c) 2007,2008 TUBITAK/UEKAE

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Please read the COPYING file.
*/

#include <iostream>
#include <qstring.h>

#include "debug.h"

using namespace std;

void Debug::printDebug(const QString &msg)
{
    cerr << DEBUGCOLOR << "DEBUG: " << msg << DEFAULTCOLOR << endl;
}

void Debug::printWarning(const QString &msg)
{
    cerr << WARNINGCOLOR << "WARNING: " << msg << DEFAULTCOLOR << endl;
}

void Debug::printError(const QString &msg)
{
    cerr << ERRORCOLOR << "ERROR: " << msg << DEFAULTCOLOR << endl;
}
