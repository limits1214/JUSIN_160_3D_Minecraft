#pragma once
#include "EntityObject.h"

NS_BEGIN(Engine)
class CComEntityModel;
class CComAnimator;
class CComConstantBuffer;
class CCollider;
class ENGINE_DLL CArrowEntity : public CEntityObject
{
public:
	typedef struct tagDesc : CEntityObject::DESC
	{
	}DESC;

public:
	DECLARE_DERIVED_TYPE(CArrowEntity, CEntityObject)

private:
	explicit CArrowEntity();
	CArrowEntity(const CArrowEntity& rhs);
	~CArrowEntity() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

private:
	void ProcessArrowDamage(_float fTimeDelta);

private:
	CComEntityModel* m_pComEntityModel{};
	CComAnimator* m_pComAnimator{ };
	CComConstantBuffer* m_pComCBufferPerObject{};

private:
	UPtr<CCollider> m_pHeadCollider{};

private:
	_float m_fElapsed{};

public:
	void SetRender(_bool b) { m_bRender = b; }
private:
	_bool m_bRender{ true };

private:
	void UpdateArrowLiftTime(_float fTimeDelta);
	//void UpdateArrowVelocity(_float fTimeDelta);
	void UpdateArrowVelocity2(_float fTimeDelta);
	//void VelocityUpdate(E::_float fTimeDelta, _fvector vWishDir);
	bool m_bStuck = false;
	XMFLOAT3 m_vStuckDir;
	XMFLOAT3 m_vLookDir = { 0.f, 0.f, 1.f };
private:
	_float m_fSpeed{ 5.f };
	_bool m_bOnGround{ false };
	_float m_fArrowLiftTime{ 15.f };

private:
	_float3 m_vStartPos{};

private:
	_bool m_bArrowBomb{ false };

public:
	// 화살 발사 초기화 함수 (발사 위치, 방향, 속도 지정)
	void Shoot(const DirectX::XMVECTOR& vStartPos, const DirectX::XMVECTOR& vDirection, E::_float fSpeed, _bool bArrowBomb = false);

private:
	// 물리 이동을 위한 변수들
	DirectX::XMFLOAT3 m_vVelocity{ 0.f, 0.f, 11.f }; // 현재 화살의 속도 벡터
	E::_float m_fGravity{ 9.8f };                   // 중력값 (마인크래프트 느낌에 맞춰 조절 필요)
	E::_float m_fDrag{ 0.99f };                     // 공기 저항 (매 초마다 속도 감소율)

public:
	static UPtr<CArrowEntity> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END