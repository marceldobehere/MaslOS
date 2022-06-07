#include "userdata.h"

OSUser adminUser;

OSUser* currentUser = &adminUser;

void initUsers()
{
    {
        adminUser.userName = "admin";
        adminUser.permLevel = 100;
        adminUser.colData.arrowColor = Colors.white;
        adminUser.colData.defaultTextColor = Colors.white;
        adminUser.colData.userColor = Colors.bgreen;
    }
}


