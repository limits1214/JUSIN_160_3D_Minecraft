#pragma once
#include "AnimalEntityObject.h"
#include "Timer.h"

NS_BEGIN(Engine)
class CComEntityModel;
class CComAnimator;
class CComConstantBuffer;
class ENGINE_DLL CPigEntity : public CAnimalEntityObject
{
public:
	typedef struct tagDesc : CAnimalEntityObject::DESC
	{

	}DESC;

public:
	DECLARE_DERIVED_TYPE(CPigEntity, CAnimalEntityObject)

private:
	explicit CPigEntity();
	CPigEntity(const CPigEntity& rhs);
	~CPigEntity() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

public:
	void SetPlayer(CHandle h) { m_hPlayer = h; }

private:
	CHandle m_hPlayer{};

protected:
	UPtr<CCollider> m_pCenterCollider{};
	void VelocityUpdate(E::_float fTimeDelta, _fvector vWishDir);
	_float3 m_vVelocity{};
	_float m_fSpeed{5.f};
	_bool m_bOnGround{};
	_float m_fRootRotY{ 0.f };

private:
	CComEntityModel* m_pComEntityModel{};
	CComAnimator* m_pComAnimator{ };
	CComConstantBuffer* m_pComCBufferPerObject{};


public:
	enum class PIG_STATE { IDLE, WANDER, FLEE, LOOK_AT_PLAYER, DIE};

	void TakeDamage(uint32_t iDamage, _vector vAttackerPos);

private:
	void ProcessDropDropItem(_float fTimeDelta);
	void ProcessDestroy(_float fTimeDelta);
	_bool m_bDropDropItem{ false };
	int32_t m_iHeart{ 5 };
private:
	_float m_fDeathTimer = 0.f;
private:
	PIG_STATE m_eCurrentState = PIG_STATE::IDLE;
	_float    m_fStateTimer = 0.f;        // 상태 유지 시간 타이머
	_float    m_fTargetYaw = 0.f;         // 부드러운 회전을 위한 목표 각도

	// 피격 처리용 변수
	_bool     m_bIsHit = false;
	_float    m_fHitTimer = 0.f;

private:
	CTimer m_TimerAmbientSoundPlay{};

public:
	static UPtr<CPigEntity> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END