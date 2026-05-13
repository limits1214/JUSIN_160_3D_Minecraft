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

private:
	void MouseFix() const;
private:
	_bool m_bFix{ false };

private:
	E::UPtr<E::CCollider> m_pCollider{};
	uint32_t m_iColliderIntersect{};

public:
	static Engine::UPtr<CPlayerCamera> Create();
	Engine::UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END
