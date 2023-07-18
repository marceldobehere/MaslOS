#pragma once
#include "../../../osData/userData.h"
#include "../defaultInstance/defaultInstance.h"
#include "../../Window/window.h"



class CustomInstance : public DefaultInstance
{
    public:
    CustomInstance(InstanceType type);
};