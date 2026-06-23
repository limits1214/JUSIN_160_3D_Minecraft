#pragma once
#include "CameraObject.h"
NS_BEGIN(Engine)
class CCollider;
class ENGINE_DLL CPlayerCamera final : public CCameraObject
{
public:
	DECLARE_DERIVED_TYPE(CPlayerCamera, CCameraObject)


public:
	void UpdateGUI() override;

protected:
	explicit CPlayerCamera();
	explicit CPlayerCamera(const CPlayerCamera& Prototype);
	~CPlayerCamera() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;

	void TriggerCameraShake(_float fDuration, _float fIntensity)
	{
		m_fShakeTimer = fDuration;
		m_fShakeDuration = fDuration;
		m_fShakeIntensity = fIntensity;
	}
private:
	_float m_fShakeTimer = 0.f;     // 현재 남은 흔들림 시간
	_float m_fShakeDuration = 0.f;  // 전체 흔들림 시간 (감쇠 비율 계산용)
	_float m_fShakeIntensity = 0.f; // 흔들림의 최대 강도

//private:
//	void MouseFix() const;
//private:
//	_bool m_bFix{ false };

private:
	E::UPtr<E::CCollider> m_pCollider{};
	uint32_t m_iColliderIntersect{};

public:
	static Engine::UPtr<CPlayerCamera> Create();
	Engine::UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END
