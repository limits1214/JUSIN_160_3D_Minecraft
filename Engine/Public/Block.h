#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

struct Block
{
public:
	enum class TYPE : unsigned char
	{
		AIR = 0,
		GRASS,
		END
	};

	TYPE type{ TYPE::GRASS };
	uint8_t  metadata{}; // 1바이트: 상위 4비트(밝기), 하위 4비트(상태/회전)
};

NS_END