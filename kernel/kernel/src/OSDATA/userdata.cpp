#include "userdata.h"
#include "../cStdLib/cstrTools.h"

OSUser adminUser;
OSUser guestUser;

//OSUser* currentUser = &adminUser;

void initUsers()
{
    {
        adminUser.userName = "admin";
        adminUser.password = "password";
        adminUser.permLevel = 100;
        adminUser.colData.arrowColor = Colors.white;
        adminUser.colData.defaultTextColor = Colors.white;
        adminUser.colData.userColor = Colors.bgreen;
    }

    {
        guestUser.userName = "guest";
        guestUser.password = "";
        guestUser.permLevel = 0;
        guestUser.colData.arrowColor = Colors.white;
        guestUser.colData.defaultTextColor = Colors.white;
        guestUser.colData.userColor = Colors.yellow;
    }
}


OSUser* getUser(const char* name)
{
    if (StrEquals(name, "admin"))
        return &adminUser;

    if (StrEquals(name, "guest"))
        return &guestUser;

    return 0;
}


