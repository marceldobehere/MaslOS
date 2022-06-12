#include "userdata.h"
#include "../cmdParsing/cstrTools.h"

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
        adminUser.mode = commandMode::none;
    }

    {
        guestUser.userName = "guest";
        guestUser.password = "";
        guestUser.permLevel = 0;
        guestUser.colData.arrowColor = Colors.white;
        guestUser.colData.defaultTextColor = Colors.white;
        guestUser.colData.userColor = Colors.yellow;
        guestUser.mode = commandMode::none;
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


