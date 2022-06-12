#pragma once

#include "../Cols.h"

enum commandMode
{
    none,
    enterPassword,
    enterText
};


struct OSUserColData
{
    uint32_t userColor;
    uint32_t defaultTextColor;
    uint32_t arrowColor;
};

struct OSUser
{
    const char* password;
    const char* userName;
    OSUserColData colData;
    uint32_t permLevel;
    commandMode mode;
};


extern OSUser adminUser;
extern OSUser guestUser;

//extern OSUser* currentUser;

OSUser* getUser(const char* name);

void initUsers();