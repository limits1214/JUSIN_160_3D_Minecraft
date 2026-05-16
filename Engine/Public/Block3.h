#pragma once
#include "Engine_Defines.h"
NS_BEGIN(Engine)
class ENGINE_DLL CBlock3
{
public:
	enum class TYPE : uint8_t
	{
		AIR = 0,
		GRASS,
		END
	};

	// 불투명한가?
	_bool IsOpaque() const
	{
		if (m_eType == TYPE::AIR)
		{
			return false;
		}
		return true;
	}
	TYPE GetType() const { return m_eType; }
	void SetType(TYPE e) { m_eType = e; }

private:
	TYPE m_eType{ TYPE::GRASS };
	uint8_t  m_iFlag{}; // 1바이트: 상위 4비트(밝기), 하위 4비트(상태/회전)
};

NS_END