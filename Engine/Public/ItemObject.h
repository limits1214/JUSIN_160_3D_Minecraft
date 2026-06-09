#pragma once
#include "GameObject.h"
#include "Block3.h"
//#include "UIItem.h"

NS_BEGIN(Engine)
class ENGINE_DLL CItemObject : public CGameObject
{
public:
	enum class ITEM_TYPE
	{
		ITEM_WoodPickaxe,
		ITEM_CooperPickaxe,
		ITEM_CooperHelmet,
		END
	};

	static _bool IsCountableItem(CItemObject::ITEM_TYPE eType)
	{
		switch (eType)
		{
		case CItemObject::ITEM_TYPE::ITEM_WoodPickaxe:
		case CItemObject::ITEM_TYPE::ITEM_CooperPickaxe:
		case CItemObject::ITEM_TYPE::ITEM_CooperHelmet:
			return false;
		}
		return true;
	}

	static uint32_t GetPackedTexIdByType(ITEM_TYPE eType)
	{
		switch (eType)
		{
		case Engine::CItemObject::ITEM_TYPE::ITEM_WoodPickaxe:
			return  PackTexId(6, 0);
		case Engine::CItemObject::ITEM_TYPE::ITEM_CooperPickaxe:
			return  PackTexId(6, 7);
		case Engine::CItemObject::ITEM_TYPE::ITEM_CooperHelmet:
			return  PackTexId(6, 6);
		case Engine::CItemObject::ITEM_TYPE::END:
			break;
		}
		return 0;
	}

	struct ItemInfo
	{
		ITEM_TYPE eItemType{ CItemObject::ITEM_TYPE::END };
		std::optional<CBlock3> block{};
		_float fDurability{ 1.f };
		uint8_t iCnt{};
	};

public:
	typedef struct tagDesc : GAMEOBJECT_DESC
	{
		ItemInfo info{};
	}DESC;
public:
	DECLARE_DERIVED_TYPE(CItemObject, CGameObject)

public:
	ItemInfo* GetItemInfo()
	{
		if (!m_ItemInfo.has_value())
			return nullptr;

		return &m_ItemInfo.value();
	}
	void SetItemInfo(std::optional<ItemInfo> info) { m_ItemInfo = info; }

private:
	std::optional<ItemInfo> m_ItemInfo{};

protected:
	explicit CItemObject();
	~CItemObject() override;

public:
	HRESULT Initialize(void* pArg) override;
};

NS_END