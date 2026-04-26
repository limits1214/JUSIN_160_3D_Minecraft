#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL ISerializer
{
public:
    virtual ~ISerializer() = default;

    virtual _string Serialize() const = 0;
    virtual void Deserialize(_string_view data) = 0;
};

NS_END