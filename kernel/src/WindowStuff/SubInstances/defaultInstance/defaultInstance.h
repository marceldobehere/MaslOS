#pragma once

enum InstanceType
{
    Default,
    Terminal,
    DebugTerminal,
    NewTerminal,
    Connect4
};

class DefaultInstance
{
    protected:

    public:
        InstanceType instanceType = InstanceType::Default;
        void DefaultFree();
};