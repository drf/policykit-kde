/*
  Copyright (c) 2007,2008 TUBITAK/UEKAE

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Please read the COPYING file.
*/

#ifndef POLICYKITKDE_H
#define POLICYKITKDE_H

#define POLICYKITKDE_BUSNAME "org.freedesktop.PolicyKit.AuthenticationAgent"
#define POLICYKITKDE_INTERFACENAME "org.freedesktop.PolicyKit.AuthenticationAgent"
#define POLICYKITKDE_OBJECTNAME "/"

struct PolicyService;

class PolicyKitKDE
{

public:
    PolicyKitKDE();
    ~PolicyKitKDE();

private:
    PolicyService *service;
};

#endif
