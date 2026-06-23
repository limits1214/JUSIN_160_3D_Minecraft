#pragma once
#include "MonsterEntityObject.h"

NS_BEGIN(Engine)
class CComEntityModel;
class CComAnimator;
class CComConstantBuffer;
class ENGINE_DLL CCreeperEntity : public CMonsterEntityObject
{
public:
	typedef struct tagDesc : CMonsterEntityObject::DESC
	{

	}DESC;

private:
	enum class CREEPER_STATE
	{
		IDLE, WANDER, CHASE_PLAYER, DIE
	};

	CREEPER_STATE m_eCurrentState{ CREEPER_STATE::IDLE };
	_float    m_fStateTimer = 0.f;
public:
	DECLARE_DERIVED_TYPE(CCreeperEntity, CMonsterEntityObject)

private:
	explicit CCreeperEntity();
	CCreeperEntity(const CCreeperEntity& rhs);
	~CCreeperEntity() override;

public:
	void SetPlayer(CHandle h) { m_hPlayer = h; }
private:
	CHandle m_hPlayer{};

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

private:
	CComEntityModel* m_pComEntityModel{};
	CComAnimator* m_pComAnimator{ };
	CComConstantBuffer* m_pComCBufferPerObject{};

private:
	UPtr<CCollider> m_pCenterCollider{};
	void VelocityUpdate(E::_float fTimeDelta, _fvector vWishDir);
	_float3 m_vVelocity{};
	_float m_fSpeed{ 5.f };
	_bool m_bOnGround{};
	_float m_fRootRotRadY{ 0.f };
	_float m_fHeadRotRadX = 0.f;
	_float m_fHeadRotRadY = 0.f;
	_float m_fTargetYaw = 0.f;
	_float m_fTargetPitch = 0.f;

	float m_fIdleLookTimer = 0.0f;

private:
	_bool     m_bIsHit = false;
	_float    m_fHitTimer = 0.f;

private:
	_float m_fWalkWeight{};
	_float m_fWalkTime{};
	//_float m_fIgniteTimer{};
	_bool m_bColorWhite{ false };
	_float m_fColorWhiteTimer{};
	_float m_fSwellingProgress{};


private:
	void ExplodeUpdate(_float fTimeDelta);

public:
	void TakeDamage(uint32_t iDamage);
private:
	uint32_t m_iHeart{5};
	_float m_fDeathTimer{};

private:
	void ProcessDestroy(_float fTimeDelta);
public:
	static UPtr<CCreeperEntity> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END