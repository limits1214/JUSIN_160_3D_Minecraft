#pragma once
#include "ItemObject.h"

NS_BEGIN(Engine)
class CCollider;
class ENGINE_DLL CDropItem : public CItemObject
{
public:
	typedef struct tagDesc : CItemObject::DESC
	{
		std::pair<StringID, StringID> viBufferId{};
	}DESC;

public:
	DECLARE_DERIVED_TYPE(CDropItem, CItemObject)

private:
	explicit CDropItem();
	CDropItem(const CDropItem& rhs);
	~CDropItem() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

private:
	std::pair<StringID, StringID> m_viBufferID{};

private:
	UPtr<CCollider> m_pCenterCollider{};
	CComTransform* m_pComAnimateTransform{};

public:
	void SetGravity(_bool b) { m_bGravity = b; }
	_bool GetGravity() const { return m_bGravity; }
private:
	_bool m_bGravity{ true };


private:
	_bool m_bAnimation{ true };
	void AnimateTransformUpdate(E::_float fTimeDelta);
	float m_fBobTime = 0.f;   // 둥실 타이머
	float m_fRotAngle = 0.f;   // 회전 각도 (라디안)

	static constexpr float BOB_SPEED = 2.0f;   // 위아래 속도
	static constexpr float BOB_AMPLITUDE = 0.1f;   // 위아래 폭 (픽셀 단위)
	static constexpr float ROT_SPEED = 90.0f;   // 회전 속도 (라디안/초)

private:
	void VelocityUpdate(E::_float fTimeDelta);

public:
	void SetVelocity(_float3 v) { m_vVelocity = v; };
private:
	_float m_fSpeed{ 5.f };
	_float3 m_vVelocity{};
	bool     m_bOnGround{ false };

public:
	static UPtr<CDropItem> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END