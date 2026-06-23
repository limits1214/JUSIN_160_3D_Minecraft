#pragma once
#include "MonsterEntityObject.h"

NS_BEGIN(Engine)
class CComEntityModel;
class CComAnimator;
class CComConstantBuffer;
class ENGINE_DLL CZombieEntity : public CMonsterEntityObject
{
public:
	typedef struct tagDesc : CMonsterEntityObject::DESC
	{

	}DESC;

private:
	enum class ZOMBIE_STATE
	{
		IDLE, WANDER, CHASE_PLAYER, DIE
	};

	ZOMBIE_STATE m_eCurrentState{ ZOMBIE_STATE::IDLE };
	_float    m_fStateTimer = 0.f;

public:
	DECLARE_DERIVED_TYPE(CZombieEntity, CMonsterEntityObject)

private:
	explicit CZombieEntity();
	CZombieEntity(const CZombieEntity& rhs);
	~CZombieEntity() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

protected:
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

	_float m_fIdleLookTimer = 0.0f;

	_float m_fLifeTime{};
	_float m_fAttackTime{};
	_float m_fAttackDelay{};
	_bool m_bAttack{false};

private:
	_bool     m_bIsHit = false;
	_float    m_fHitTimer = 0.f;

public:
	void SetPlayer(CHandle h) { m_hPlayer = h; }
private:
	CHandle m_hPlayer{};

private:
	_float m_fWalkWeight{};
	_float m_fWalkTime{};

public:
	void TakeDamage(uint32_t iDamage);
private:
	int32_t m_iHeart{ 5 };
	_float m_fDeathTimer{};

private:
	void ProcessDestroy(_float fTimeDelta);

private:
	CComEntityModel* m_pComEntityModel{};
	CComAnimator* m_pComAnimator{ };
	CComConstantBuffer* m_pComCBufferPerObject{};

public:
	static UPtr<CZombieEntity> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END