#pragma once

enum InstanceType
{
    Default,
    Terminal
};

class DefaultInstance
{
    protected:

    public:
        InstanceType instanceType = InstanceType::Default;
};