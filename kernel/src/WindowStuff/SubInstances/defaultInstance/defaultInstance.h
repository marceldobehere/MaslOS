#pragma once

enum InstanceType
{
    Default,
    Terminal,
    DebugTerminal,
    NewTerminal
};

class DefaultInstance
{
    protected:

    public:
        InstanceType instanceType = InstanceType::Default;
        void DefaultFree();
};