
#pragma once
#include "ItemObject.h"

NS_BEGIN(Engine)
class CCollider;
class CResDynamicBuffer;
class ENGINE_DLL CDropBlock : public CItemObject
{
public:
	typedef struct tagDesc : CItemObject::DESC
	{
		std::pair<StringID, StringID> viBufferId{};
	}DESC;
public:
	struct InstancedDropBlockDesc
	{
		_float3 vPos{};
		_float3 vVelocity{};
		bool    bOnGround{ false };
		_float  fBobYOffset{};
		_float  fBobYRot{};
		_float  fBobTime{};
		std::vector<uint32_t>texIndexs{};

		SPtr<CCollider> boxCollider{};

		_float4x4 matWorld{};
	};

	struct CollHint
	{
		std::list<InstancedDropBlockDesc>::iterator iter;
	};

public:
	DECLARE_DERIVED_TYPE(CDropBlock, CItemObject)

private:
	explicit CDropBlock();
	CDropBlock(const CDropBlock& rhs);
	~CDropBlock() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

private:
	std::pair<StringID, StringID> m_viBufferID{};

private:
	//UPtr<CCollider> m_pCenterCollider{};

public:
	void SetGravity(_bool b) { m_bGravity = b; }
	_bool GetGravity() const { return m_bGravity; }
private:
	_bool m_bGravity{ true };


private:
	_bool m_bAnimation{ true };
	void AnimateTransformUpdate(InstancedDropBlockDesc& item, E::_float fTimeDelta);


	static constexpr float BOB_SPEED = 2.0f;   // 위아래 속도
	static constexpr float BOB_AMPLITUDE = 0.1f;   // 위아래 폭 (픽셀 단위)
	static constexpr float ROT_SPEED = 90.0f;   // 회전 속도 (라디안/초)

private:
	void VelocityUpdate(InstancedDropBlockDesc& item, E::_float fTimeDelta);

private:
	_float m_fSpeed{ 5.f };

public:
	std::list<InstancedDropBlockDesc>& GetDropItems() { return m_vecDropItems; }
	void AddDropItem(_float3 vPos, _float3 vVelocity, const std::vector<uint32_t>& vecTexindex);
private:
	std::list<InstancedDropBlockDesc>  m_vecDropItems;
	std::vector<VTX_DROP_BLOCK_INSTANCED_DATA> m_vecInstancedData{};
	uint32_t m_iNumElements{ 1000 };
	//uint32_t m_iElementStride{ sizeof(_float4x4) };
	SPtr<CResDynamicBuffer> m_pResInstancedBuffer{};

public:
	static UPtr<CDropBlock> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END