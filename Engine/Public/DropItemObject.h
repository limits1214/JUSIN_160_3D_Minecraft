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
			case CItemObject::ITEM_TYPE::ITEM_CopperHelmet:
			case CItemObject::ITEM_TYPE::ITEM_IronHelmet:
			case CItemObject::ITEM_TYPE::ITEM_GoldHelmet:
			case CItemObject::ITEM_TYPE::ITEM_DiamondHelmet:
			case CItemObject::ITEM_TYPE::ITEM_NetheriteHelmet:
				return "01_DROPITEM_Helmet";

			case CItemObject::ITEM_TYPE::ITEM_CopperChestplate:
			case CItemObject::ITEM_TYPE::ITEM_IronChestplate:
			case CItemObject::ITEM_TYPE::ITEM_GoldChestplate:
			case CItemObject::ITEM_TYPE::ITEM_DiamondChestplate:
			case CItemObject::ITEM_TYPE::ITEM_NetheriteChestplate:
				return "01_DROPITEM_Chestplate";

			case CItemObject::ITEM_TYPE::ITEM_CopperLeggings:
			case CItemObject::ITEM_TYPE::ITEM_IronLeggings:
			case CItemObject::ITEM_TYPE::ITEM_GoldLeggings:
			case CItemObject::ITEM_TYPE::ITEM_DiamondLeggings:
			case CItemObject::ITEM_TYPE::ITEM_NetheriteLeggings:
				return "01_DROPITEM_Leggings";

			case CItemObject::ITEM_TYPE::ITEM_CopperBoots:
			case CItemObject::ITEM_TYPE::ITEM_IronBoots:
			case CItemObject::ITEM_TYPE::ITEM_GoldBoots:
			case CItemObject::ITEM_TYPE::ITEM_DiamondBoots:
			case CItemObject::ITEM_TYPE::ITEM_NetheriteBoots:
				return "01_DROPITEM_Boots";


			case CItemObject::ITEM_TYPE::ITEM_WoodPickaxe:
			case CItemObject::ITEM_TYPE::ITEM_StonePickaxe:
			case CItemObject::ITEM_TYPE::ITEM_CopperPickaxe:
			case CItemObject::ITEM_TYPE::ITEM_IronPickaxe:
			case CItemObject::ITEM_TYPE::ITEM_GoldPickaxe:
			case CItemObject::ITEM_TYPE::ITEM_DiamondPickaxe:
			case CItemObject::ITEM_TYPE::ITEM_NetheritePickaxe:
				return "01_DROPITEM_WoodPickaxe";

			case CItemObject::ITEM_TYPE::ITEM_WoodAxe:
			case CItemObject::ITEM_TYPE::ITEM_StoneAxe:
			case CItemObject::ITEM_TYPE::ITEM_CopperAxe:
			case CItemObject::ITEM_TYPE::ITEM_IronAxe:
			case CItemObject::ITEM_TYPE::ITEM_GoldAxe:
			case CItemObject::ITEM_TYPE::ITEM_DiamondAxe:
			case CItemObject::ITEM_TYPE::ITEM_NetheriteAxe:
				return "01_DROPITEM_Axe";

			case CItemObject::ITEM_TYPE::ITEM_WoodHoe:
			case CItemObject::ITEM_TYPE::ITEM_StoneHoe:
			case CItemObject::ITEM_TYPE::ITEM_CopperHoe:
			case CItemObject::ITEM_TYPE::ITEM_IronHoe:
			case CItemObject::ITEM_TYPE::ITEM_GoldHoe:
			case CItemObject::ITEM_TYPE::ITEM_DiamondHoe:
			case CItemObject::ITEM_TYPE::ITEM_NetheriteHoe:
				return "01_DROPITEM_Hoe";

			case CItemObject::ITEM_TYPE::ITEM_WoodShovel:
			case CItemObject::ITEM_TYPE::ITEM_StoneShovel:
			case CItemObject::ITEM_TYPE::ITEM_CopperShovel:
			case CItemObject::ITEM_TYPE::ITEM_IronShovel:
			case CItemObject::ITEM_TYPE::ITEM_GoldShovel:
			case CItemObject::ITEM_TYPE::ITEM_DiamondShovel:
			case CItemObject::ITEM_TYPE::ITEM_NetheriteShovel:
				return "01_DROPITEM_Shovel";

			case CItemObject::ITEM_TYPE::ITEM_WoodSword:
			case CItemObject::ITEM_TYPE::ITEM_StoneSword:
			case CItemObject::ITEM_TYPE::ITEM_CopperSword:
			case CItemObject::ITEM_TYPE::ITEM_IronSword:
			case CItemObject::ITEM_TYPE::ITEM_GoldSword:
			case CItemObject::ITEM_TYPE::ITEM_DiamondSword:
			case CItemObject::ITEM_TYPE::ITEM_NetheriteSword:
				return "01_DROPITEM_Sword";

			case CItemObject::ITEM_TYPE::ITEM_Bow_Standby:
				return "01_DROPITEM_Bow_Standby";
			case CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_0:
				return "01_DROPITEM_Bow_Pulling_0";
			case CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_1:
				return "01_DROPITEM_Bow_Pulling_1";
			case CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_2:
				return "01_DROPITEM_Bow_Pulling_2";

			case CItemObject::ITEM_TYPE::ITEM_Arrow:
				return "01_DROPITEM_Arrow";

			case CItemObject::ITEM_TYPE::ITEM_Raw_Iron:
				return "01_DROPITEM_RawIron";
			case CItemObject::ITEM_TYPE::ITEM_Raw_Copper:
				return "01_DROPITEM_RawCopper";
			case CItemObject::ITEM_TYPE::ITEM_Raw_Gold:
				return "01_DROPITEM_RawGold";
			case CItemObject::ITEM_TYPE::ITEM_Iron_Ingot:
			case CItemObject::ITEM_TYPE::ITEM_Copper_Ingot:
			case CItemObject::ITEM_TYPE::ITEM_Gold_Ingot:
			case CItemObject::ITEM_TYPE::ITEM_Netherite_Ingot:
				return "01_DROPITEM_Ingot";
			case CItemObject::ITEM_TYPE::ITEM_Diamond:
				return "01_DROPITEM_Diamond";
			case CItemObject::ITEM_TYPE::ITEM_NetheriteScrap:
				return "01_DROPITEM_NetheriteScrap";

			case CItemObject::ITEM_TYPE::ITEM_Charcoal:
				return "01_DROPITEM_Charcoal";

			case CItemObject::ITEM_TYPE::ITEM_Coal:
				return "01_DROPITEM_Coal";
			case CItemObject::ITEM_TYPE::ITEM_Stick:
				return "01_DROPITEM_Stick";
			case CItemObject::ITEM_TYPE::ITEM_Torch:
				return "01_DROPITEM_Torch";

			case Engine::CItemObject::ITEM_TYPE::ITEM_String:
				return  "01_DROPITEM_String";
			case Engine::CItemObject::ITEM_TYPE::ITEM_Flint:
				return  "01_DROPITEM_Flint";
			case Engine::CItemObject::ITEM_TYPE::ITEM_Feather:
				return  "01_DROPITEM_Feather";

			case Engine::CItemObject::ITEM_TYPE::ITEM_Bucket_Empty:
			case Engine::CItemObject::ITEM_TYPE::ITEM_Bucket_Water:
			case Engine::CItemObject::ITEM_TYPE::ITEM_Bucket_Lava:
				return  "01_DROPITEM_Bucket";

			case Engine::CItemObject::ITEM_TYPE::ITEM_FlintAndSteel:
				return  "01_DROPITEM_FlintAndSteel";

			case Engine::CItemObject::ITEM_TYPE::ITEM_Gunpowder:
				return  "01_DROPITEM_Gunpowder";

			case Engine::CItemObject::ITEM_TYPE::ITEM_Raw_Chicken:
			case Engine::CItemObject::ITEM_TYPE::ITEM_Chicken_Cooked:
				return  "01_DROPITEM_Chicken";

			case Engine::CItemObject::ITEM_TYPE::ITEM_Raw_Beef:
			case Engine::CItemObject::ITEM_TYPE::ITEM_Beef_Cooked:
				return  "01_DROPITEM_Beef";

			case Engine::CItemObject::ITEM_TYPE::ITEM_Raw_Porkchop:
			case Engine::CItemObject::ITEM_TYPE::ITEM_Porkchop_Cooked:
				return  "01_DROPITEM_Porkchop";

			case Engine::CItemObject::ITEM_TYPE::ITEM_Raw_Mutton:
			case Engine::CItemObject::ITEM_TYPE::ITEM_Mutton_Cooked:
				return  "01_DROPITEM_Mutton";

			case Engine::CItemObject::ITEM_TYPE::ITEM_Apple:
				return  "01_DROPITEM_Apple";

			case Engine::CItemObject::ITEM_TYPE::ITEM_Wheat:
				return  "01_DROPITEM_Wheat";

			case Engine::CItemObject::ITEM_TYPE::ITEM_Bread:
				return  "01_DROPITEM_Bread";
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
			case CItemObject::ITEM_TYPE::ITEM_CopperHelmet:
			case CItemObject::ITEM_TYPE::ITEM_IronHelmet:
			case CItemObject::ITEM_TYPE::ITEM_GoldHelmet:
			case CItemObject::ITEM_TYPE::ITEM_DiamondHelmet:
			case CItemObject::ITEM_TYPE::ITEM_NetheriteHelmet:
				return "CopperHelmet";

			case CItemObject::ITEM_TYPE::ITEM_CopperChestplate:
			case CItemObject::ITEM_TYPE::ITEM_IronChestplate:
			case CItemObject::ITEM_TYPE::ITEM_GoldChestplate:
			case CItemObject::ITEM_TYPE::ITEM_DiamondChestplate:
			case CItemObject::ITEM_TYPE::ITEM_NetheriteChestplate:
				return "Chestplate";

			case CItemObject::ITEM_TYPE::ITEM_CopperLeggings:
			case CItemObject::ITEM_TYPE::ITEM_IronLeggings:
			case CItemObject::ITEM_TYPE::ITEM_GoldLeggings:
			case CItemObject::ITEM_TYPE::ITEM_DiamondLeggings:
			case CItemObject::ITEM_TYPE::ITEM_NetheriteLeggings:
				return "Leggings";

			case CItemObject::ITEM_TYPE::ITEM_CopperBoots:
			case CItemObject::ITEM_TYPE::ITEM_IronBoots:
			case CItemObject::ITEM_TYPE::ITEM_GoldBoots:
			case CItemObject::ITEM_TYPE::ITEM_DiamondBoots:
			case CItemObject::ITEM_TYPE::ITEM_NetheriteBoots:
				return "Boots";


			case CItemObject::ITEM_TYPE::ITEM_WoodPickaxe:
			case CItemObject::ITEM_TYPE::ITEM_StonePickaxe:
			case CItemObject::ITEM_TYPE::ITEM_CopperPickaxe:
			case CItemObject::ITEM_TYPE::ITEM_IronPickaxe:
			case CItemObject::ITEM_TYPE::ITEM_GoldPickaxe:
			case CItemObject::ITEM_TYPE::ITEM_DiamondPickaxe:
			case CItemObject::ITEM_TYPE::ITEM_NetheritePickaxe:
				return "WoodPickaxe";

			case CItemObject::ITEM_TYPE::ITEM_WoodAxe:
			case CItemObject::ITEM_TYPE::ITEM_StoneAxe:
			case CItemObject::ITEM_TYPE::ITEM_CopperAxe:
			case CItemObject::ITEM_TYPE::ITEM_IronAxe:
			case CItemObject::ITEM_TYPE::ITEM_GoldAxe:
			case CItemObject::ITEM_TYPE::ITEM_DiamondAxe:
			case CItemObject::ITEM_TYPE::ITEM_NetheriteAxe:
				return "Axe";

			case CItemObject::ITEM_TYPE::ITEM_WoodHoe:
			case CItemObject::ITEM_TYPE::ITEM_StoneHoe:
			case CItemObject::ITEM_TYPE::ITEM_CopperHoe:
			case CItemObject::ITEM_TYPE::ITEM_IronHoe:
			case CItemObject::ITEM_TYPE::ITEM_GoldHoe:
			case CItemObject::ITEM_TYPE::ITEM_DiamondHoe:
			case CItemObject::ITEM_TYPE::ITEM_NetheriteHoe:
				return "Hoe";

			case CItemObject::ITEM_TYPE::ITEM_WoodShovel:
			case CItemObject::ITEM_TYPE::ITEM_StoneShovel:
			case CItemObject::ITEM_TYPE::ITEM_CopperShovel:
			case CItemObject::ITEM_TYPE::ITEM_IronShovel:
			case CItemObject::ITEM_TYPE::ITEM_GoldShovel:
			case CItemObject::ITEM_TYPE::ITEM_DiamondShovel:
			case CItemObject::ITEM_TYPE::ITEM_NetheriteShovel:
				return "Shovel";

			case CItemObject::ITEM_TYPE::ITEM_WoodSword:
			case CItemObject::ITEM_TYPE::ITEM_StoneSword:
			case CItemObject::ITEM_TYPE::ITEM_CopperSword:
			case CItemObject::ITEM_TYPE::ITEM_IronSword:
			case CItemObject::ITEM_TYPE::ITEM_GoldSword:
			case CItemObject::ITEM_TYPE::ITEM_DiamondSword:
			case CItemObject::ITEM_TYPE::ITEM_NetheriteSword:
				return "Sword";

			case CItemObject::ITEM_TYPE::ITEM_Bow_Standby:
				return "Bow_Standby";
			case CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_0:
				return "Bow_Pulling_0";
			case CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_1:
				return "Bow_Pulling_1";
			case CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_2:
				return "Bow_Pulling_2";

			case CItemObject::ITEM_TYPE::ITEM_Arrow:
				return "Arrow";

			case CItemObject::ITEM_TYPE::ITEM_Raw_Iron:
				return "RawIron";
			case CItemObject::ITEM_TYPE::ITEM_Raw_Copper:
				return "RawCopper";
			case CItemObject::ITEM_TYPE::ITEM_Raw_Gold:
				return "RawGold";
			case CItemObject::ITEM_TYPE::ITEM_Iron_Ingot:
			case CItemObject::ITEM_TYPE::ITEM_Copper_Ingot:
			case CItemObject::ITEM_TYPE::ITEM_Gold_Ingot:
			case CItemObject::ITEM_TYPE::ITEM_Netherite_Ingot:
				return "Ingot";
			case CItemObject::ITEM_TYPE::ITEM_Diamond:
				return "Diamond";
			case CItemObject::ITEM_TYPE::ITEM_NetheriteScrap:
				return "NetheriteScrap";

			case CItemObject::ITEM_TYPE::ITEM_Charcoal:
				return "Charcoal";

			case CItemObject::ITEM_TYPE::ITEM_Coal:
				return "Coal";
			case CItemObject::ITEM_TYPE::ITEM_Stick:
				return "Stick";
			case CItemObject::ITEM_TYPE::ITEM_Torch:
				return "Torch";

			case Engine::CItemObject::ITEM_TYPE::ITEM_String:
				return  "String";
			case Engine::CItemObject::ITEM_TYPE::ITEM_Flint:
				return  "Flint";
			case Engine::CItemObject::ITEM_TYPE::ITEM_Feather:
				return  "Feather";

			case Engine::CItemObject::ITEM_TYPE::ITEM_Bucket_Empty:
			case Engine::CItemObject::ITEM_TYPE::ITEM_Bucket_Water:
			case Engine::CItemObject::ITEM_TYPE::ITEM_Bucket_Lava:
				return  "Bucket";

			case Engine::CItemObject::ITEM_TYPE::ITEM_FlintAndSteel:
				return  "FlintAndSteel";

			case Engine::CItemObject::ITEM_TYPE::ITEM_Gunpowder:
				return  "Gunpowder";

			case Engine::CItemObject::ITEM_TYPE::ITEM_Raw_Chicken:
			case Engine::CItemObject::ITEM_TYPE::ITEM_Chicken_Cooked:
				return  "Chicken";

			case Engine::CItemObject::ITEM_TYPE::ITEM_Raw_Beef:
			case Engine::CItemObject::ITEM_TYPE::ITEM_Beef_Cooked:
				return  "Beef";

			case Engine::CItemObject::ITEM_TYPE::ITEM_Raw_Porkchop:
			case Engine::CItemObject::ITEM_TYPE::ITEM_Porkchop_Cooked:
				return  "Porkchop";

			case Engine::CItemObject::ITEM_TYPE::ITEM_Raw_Mutton:
			case Engine::CItemObject::ITEM_TYPE::ITEM_Mutton_Cooked:
				return  "Mutton";

			case Engine::CItemObject::ITEM_TYPE::ITEM_Apple:
				return  "Apple";

			case Engine::CItemObject::ITEM_TYPE::ITEM_Wheat:
				return  "Wheat";

			case Engine::CItemObject::ITEM_TYPE::ITEM_Bread:
				return  "Bread";
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
	//UPtr<CCollider> m_pCenterCollider{};

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