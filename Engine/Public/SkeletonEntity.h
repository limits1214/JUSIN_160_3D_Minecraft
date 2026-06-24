#pragma once
#include "MonsterEntityObject.h"

NS_BEGIN(Engine)
class CComEntityModel;
class CComAnimator;
class CComConstantBuffer;
class CCollider;
class ENGINE_DLL CSkeletonEntity : public CMonsterEntityObject
{
public:
	typedef struct tagDesc : CMonsterEntityObject::DESC
	{

	}DESC;

	enum class SKELETON_STATE
	{
		IDLE, WANDER, CHASE_PLAYER, DIE
	};

	SKELETON_STATE m_eCurrentState{ SKELETON_STATE::IDLE };
	_float    m_fStateTimer = 0.f;

public:
	DECLARE_DERIVED_TYPE(CSkeletonEntity, CMonsterEntityObject)

private:
	explicit CSkeletonEntity();
	CSkeletonEntity(const CSkeletonEntity& rhs);
	~CSkeletonEntity() override;

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
	//_bool m_bColorWhite{ false };
	//_float m_fColorWhiteTimer{};
	//_float m_fSwellingProgress{};

private:
	_bool m_bBowAndArrow{ false };
	_float m_fLifeTime{};
	_float m_fBowWeight{};
	_float m_fArrowShootTimer{};

public:
	void TakeDamage(uint32_t iDamage, _vector vAttackerPos);

private:
	int32_t m_iHeart{ 5 };
	_float m_fDeathTimer{};

private:
	void ProcessDestroy(_float fTimeDelta);

private:
	CComEntityModel* m_pComEntityModel{};
	CComAnimator* m_pComAnimator{ };
	CComConstantBuffer* m_pComCBufferPerObject{};

private:
	CHandle m_hHandHeld{};

public:
	static UPtr<CSkeletonEntity> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END