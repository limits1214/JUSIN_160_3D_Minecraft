#pragma once
#include "CameraObject.h"
NS_BEGIN(Engine)
class CCollider;
class ENGINE_DLL CPlayerInvenUICamera final : public CCameraObject
{
public:
	DECLARE_DERIVED_TYPE(CPlayerInvenUICamera, CCameraObject)


public:
	void UpdateGUI() override;

protected:
	explicit CPlayerInvenUICamera();
	explicit CPlayerInvenUICamera(const CPlayerInvenUICamera& Prototype);
	~CPlayerInvenUICamera() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;

private:
	E::UPtr<E::CCollider> m_pCollider{};

public:
	static Engine::UPtr<CPlayerInvenUICamera> Create();
	Engine::UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END
