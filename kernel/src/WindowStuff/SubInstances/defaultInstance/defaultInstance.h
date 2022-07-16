#pragma once

enum InstanceType
{
    Default,
    Terminal,
    DebugTerminal
};

class DefaultInstance
{
    protected:

    public:
        InstanceType instanceType = InstanceType::Default;
        void DefaultFree();
};