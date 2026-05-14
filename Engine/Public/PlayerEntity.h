#pragma once
#include "PlayerEntityObject.h"
#include "PlayerCamera.h"

NS_BEGIN(Engine)
class CComEntityModel;
class ENGINE_DLL CPlayerEntity : public CPlayerEntityObject
{
public:
	enum class CAMERA_TYPE
	{
		FPS,
		TPS,
		TPS_BACK,
	};

public:
	typedef struct tagDesc : CPlayerEntityObject::DESC
	{

	}DESC;

public:
	DECLARE_DERIVED_TYPE(CPlayerEntity, CPlayerEntityObject)

private:
	explicit CPlayerEntity();
	~CPlayerEntity() override;

public:
	void UpdateGUI() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

private:
	//CPlayerCamera* m_pPlayerCamera{};
	_float3 m_vHeadRotation = { 0.f, 0.f, 0.f };
	CAMERA_TYPE m_eCameraType{ CAMERA_TYPE ::TPS };
private:
	CComEntityModel* m_pComEntityModel{};


	bool m_bAttacking = false;
	float m_fAttackTime = 0.0f;
	float m_fAttackDuration = 0.6f;

	void StartAttack()
	{
		m_bAttacking = true;
		m_fAttackTime = 0.0f;
		m_fAttackDuration = 0.6f;  // 공격 지속 시간
	}

	// LateUpdate에서
	void UpdateAttackAnimation(float fTimeDelta);
public:
	static UPtr<CPlayerEntity> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END