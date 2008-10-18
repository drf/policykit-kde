/*
  Copyright (c) 2007,2008 TUBITAK/UEKAE

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Please read the COPYING file.
*/

#include <qstring.h>

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kuniqueapplication.h>
#include <klocale.h>

#include "authdialog.h"
#include "policykitkde.h"
#include "debug.h"

int main (int argc, char *argv[])
{
    KAboutData aboutData( "policykit-kde", "PolicyKit-kde", "0.2",
                          "PolicyKit-kde", KAboutData::License_GPL_V2,
                          "(c) 2007,2008 TUBITAK - UEKAE");
    aboutData.addAuthor( "Gökçen Eraslan", I18N_NOOP( "Author" ), "gokcen@pardus.org.tr" );
    KCmdLineArgs::init( argc, argv, &aboutData );

    //set options
    KCmdLineOptions options[]= {
                                {"no-exit", I18N_NOOP("Do not exit automatically. This is used for debugging purposes."), 0},
                                KCmdLineLastOption
    };

    KCmdLineArgs::addCmdLineOptions(options);
    KUniqueApplication::addCmdLineOptions();

    if (!KUniqueApplication::start())
    {
        Debug::printError("PolicyKit-kde is already running, exiting");
        return 0;
    }

    KUniqueApplication app(true, true, true);

    try {
        PolicyKitKDE *pkKDE = new PolicyKitKDE();
    }
    catch (QString exc)
    {
        return 0;
    }

    return app.exec();
}
