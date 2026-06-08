#pragma once
#include "ItemObject.h"

NS_BEGIN(Engine)
class CCollider;
class CResDynamicBuffer;
class ENGINE_DLL CDropItemObject : public CItemObject
{
public:
	typedef struct tagDesc : CItemObject::DESC
	{
		std::pair<StringID, StringID> viBufferId{};
	}DESC;
public:
	struct InstancedDropItemObjectDesc
	{
		CItemObject::ItemInfo itemInfo{};
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

public:
	static _string GetDropItemLayer(const CItemObject::ItemInfo& info)
	{
		if (!info.block)
		{
			switch (info.eItemType)
			{
			case CItemObject::ITEM_TYPE::ITEM_CooperHelmet:
				return "01_DROPITEM_Helmet";
			case CItemObject::ITEM_TYPE::ITEM_CooperPickaxe:
				return "01_DROPITEM_Picaxe";
			}
		}
		return "01_DROPBLOCK_CUBE";
	}

	static StringID GetVIBufferName(const CItemObject::ItemInfo& info)
	{
		if (!info.block)
		{
			switch (info.eItemType)
			{
			case CItemObject::ITEM_TYPE::ITEM_CooperHelmet:
				return "CopperHelmet";
			case CItemObject::ITEM_TYPE::ITEM_WoodPickaxe:
			case CItemObject::ITEM_TYPE::ITEM_CooperPickaxe:
				return "WoodPickaxe";
			}
		}
		return "CubeItemDirt";
	}

struct CollHint
{
	std::list<InstancedDropItemObjectDesc>::iterator iter;
};
public:
	DECLARE_DERIVED_TYPE(CDropItemObject, CItemObject)

protected:
	explicit CDropItemObject();
	CDropItemObject(const CDropItemObject& rhs);
	~CDropItemObject() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

protected:
	std::pair<StringID, StringID> m_viBufferID{};

protected:
	UPtr<CCollider> m_pCenterCollider{};

protected:
	void SetGravity(_bool b) { m_bGravity = b; }
	_bool GetGravity() const { return m_bGravity; }
protected:
	_bool m_bGravity{ true };


protected:
	_bool m_bAnimation{ true };
	void AnimateTransformUpdate(InstancedDropItemObjectDesc& item, E::_float fTimeDelta);


	static constexpr float BOB_SPEED = 2.0f;   // 위아래 속도
	static constexpr float BOB_AMPLITUDE = 0.1f;   // 위아래 폭 (픽셀 단위)
	static constexpr float ROT_SPEED = 90.0f;   // 회전 속도 (라디안/초)

protected:
	void VelocityUpdate(InstancedDropItemObjectDesc& item, E::_float fTimeDelta);

protected:
	_float m_fSpeed{ 5.f };

public:
	std::list<InstancedDropItemObjectDesc>& GetDropItemObjects() { return m_vecDropItemObjects; }
	void AddDropItemObject(const CItemObject::ItemInfo& itemInfo, _float3 vPos, _float3 vVelocity, const std::vector<uint32_t>& vecTexindex);
	
protected:
	std::list<InstancedDropItemObjectDesc>  m_vecDropItemObjects;
};

NS_END