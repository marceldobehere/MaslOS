#pragma once

enum InstanceType
{
    Default,
    Terminal,
    DebugTerminal,
    NewTerminal,
    Connect4,
    GUI,
    CRASH,
    WARNING,
    TESTO_PGM
};

class DefaultInstance
{
    protected:

    public:
        InstanceType instanceType = InstanceType::Default;
        void DefaultFree();
};