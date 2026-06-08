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
	struct ItemInfo
	{
		ITEM_TYPE eItemType{ CItemObject::ITEM_TYPE::END };
		std::optional<CBlock3> block{};
		_float fDurability{ 1.f };
		uint8_t iCnt{};
	};

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
public:
	typedef struct tagDesc : GAMEOBJECT_DESC
	{

	}DESC;
public:
	DECLARE_DERIVED_TYPE(CItemObject, CGameObject)

public:
	ItemInfo& GetItemInfoRef() { return m_ItemInfo; }

private:
	ItemInfo m_ItemInfo{};

protected:
	explicit CItemObject();
	~CItemObject() override;

public:
	HRESULT Initialize(void* pArg) override;
};

NS_END