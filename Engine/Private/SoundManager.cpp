#include "pch.h"
#include "SoundManager.h"
#include "fmod.h"
#include "fmod.hpp"

NS_USING(Engine)

CSoundManager::CSoundManager()
{
}

CSoundManager::~CSoundManager()
{
}

HRESULT CSoundManager::Initialize()
{
	// 사운드를 담당하는 대표객체를 생성하는 함수
	FMOD_System_Create(&m_pSystem, FMOD_VERSION);

	FMOD_System_SetDSPBufferSize(m_pSystem, 2048, 4);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	return S_OK;
}

HRESULT CSoundManager::CreateSound(const _string& sPath, FMOD_SOUND** ppSound)
{
	FMOD_RESULT eRes = FMOD_System_CreateSound(
		m_pSystem,
		sPath.c_str(),
		FMOD_CREATESTREAM, 0, ppSound);

	if (eRes != FMOD_OK)
	{
		return E_FAIL;
	}
	return S_OK;
}

UPtr<CSoundManager> CSoundManager::Create()
{
	auto pInstance = UPtr<CSoundManager>(new CSoundManager{});
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CSoundManager");
		return nullptr;
	}
	return pInstance;
}
