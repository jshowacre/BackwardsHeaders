#include "interface.h"

#pragma once
class IGameConsole : public IBaseInterface
{
public:
    virtual void Show() = 0;
    virtual void Init() = 0;
    virtual void Hide() = 0;
    virtual void Clear() = 0;
    virtual bool IsShown() = 0;

    // ???
    virtual void UnknownA() = 0;
    virtual void UnknownB() = 0;
};

#define GAMECONSOLE_INTERFACE_VERSION "GameConsole004"