#pragma once
#include "Engine_Defines.h"

struct FMOD_SYSTEM;
struct FMOD_SOUND;

NS_BEGIN(Engine)

class CSoundManager final : public CEngineBase
{
private:
	CSoundManager();
	~CSoundManager();

public:
	HRESULT Initialize();

	HRESULT CreateSound(const _string& sPath, FMOD_SOUND** ppSound);

private:
	FMOD_SYSTEM* m_pSystem{};

public:
	static UPtr<CSoundManager> Create();
};

NS_END
