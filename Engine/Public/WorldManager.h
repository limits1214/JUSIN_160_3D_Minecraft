#pragma once
#include "Engine_Defines.h"
#include "FurnaceStorage.h"
#include "ChestStorage.h" 
#include "Timer.h"

NS_BEGIN(Engine)
//class CFurnaceStorage;
//class CChestStorage;
class ENGINE_DLL CWorldManager final: public CEngineBase
{
public:
	enum class WORLD_TYPE
	{
		OVER,
		NETHER,
		THE_END,

		END
	};
	enum class DAY_TYPE
	{
		DAY = 0,
		NIGHT
	};

private:
	explicit CWorldManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CWorldManager() override;

public:
	void UpdateGUI();

public:
	void Update(_float fTimeDelta);

public:
	CFurnaceStorage* GetFurnaceStorage() const { return m_pFurnaceStorage.get(); }
	CChestStorage* GetChestStorage() const { return m_pChestStorage.get(); }
public:
	HRESULT Initialize();

private:
	void UpdateSunShadowCamera(_float fTimeDelta);

private:
	ComPtr<ID3D11Device> m_pDevice{};
	ComPtr<ID3D11DeviceContext> m_pContext{};

private:
	UPtr<CFurnaceStorage> m_pFurnaceStorage{};
	UPtr<CChestStorage> m_pChestStorage{};

private:
	CTimer m_TimerMobSpawn{};

public:
	_float GetElapsedTime() const { return m_fElapsedTime; };
	_float GetDayFactor() const
	{
		// 1. 0.0 -> 1.0으로 무한 반복하는 기초 진행도 계산
		_float fLinearFactor = fmodf(m_fElapsedTime, fDayDuration) / fDayDuration;

		// 2. 0.0 -> 1.0 -> 0.0 왕복 핑퐁 (0: 낮, 1: 밤)
		_float fPingPong = 1.0f - fabsf(fLinearFactor - 0.5f) * 2.0f;

		// 3. 코사인 곡선을 이용해 S자 곡선(Smooth Step 효과)으로 변환
		// fPingPong이 0일 때(낮) cos 결과는 1 -> 최종 1.0
		// fPingPong이 1일 때(밤) cos 결과는 -1 -> 최종 0.0
		_float fSmoothFactor = 0.5f * (1.0f + cosf(fPingPong * XM_PI));

		return fSmoothFactor;
	}

	_float GetSkyRotation() const
	{
		_float fAngle = (fmodf(m_fElapsedTime, fDayDuration) / fDayDuration) * XM_2PI;
		return fAngle;
	}
	
	//float g_fTimeFactor = ; // 0.0f ~ 1.0f
private:
	float fDayDuration = 60.0f * 5.f;
	_float m_fElapsedTime{10.f};

private:
	_float m_fShadowSnapTimer = 0.f;
	_vector m_vShadowEye = XMVectorZero();

public:
	HRESULT WorldRandomMonsterGeneration(_float3 vCenterPos, _float fRadius, uint32_t iCnt);

public:
	void SetPlayer(CHandle h) { m_hPlayer = h; }
private:
	CHandle m_hPlayer{};

public:
	static UPtr<CWorldManager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END