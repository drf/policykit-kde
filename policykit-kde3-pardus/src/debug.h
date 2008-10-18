/*
  Copyright (c) 2007,2008 TUBITAK/UEKAE

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Please read the COPYING file.
*/

#define DEBUGCOLOR "\033[32m"
#define WARNINGCOLOR "\033[33m"
#define ERRORCOLOR "\033[01;31m"
#define DEFAULTCOLOR "\033[0m"

class Debug
{
    public:
        static void printDebug(const QString &msg);
        static void printWarning(const QString &msg);
        static void printError(const QString &msg);
};
