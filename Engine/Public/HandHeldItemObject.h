#pragma once
#include "ItemObject.h"
#include "DropItem.h"

NS_BEGIN(Engine)
class ENGINE_DLL CHandHeldItemObject : public CItemObject
{
public:
	typedef struct tagDesc : CItemObject::DESC
	{
		std::pair<StringID, StringID> viBufferId{};
		//CItemObject::ItemInfo itemInfo{};
	}DESC;
public:
	DECLARE_DERIVED_TYPE(CHandHeldItemObject, CItemObject)

public:
	//static _string GetHandHeldItemLayer(const CItemObject::ItemInfo& info)
	//{
	//	if (!info.block)
	//	{
	//		switch (info.eItemType)
	//		{
	//		case CItemObject::ITEM_TYPE::ITEM_CopperHelmet:
	//			return "01_HANDHELD_Helmet";
	//		case CItemObject::ITEM_TYPE::ITEM_CopperPickaxe:
	//		case CItemObject::ITEM_TYPE::ITEM_WoodPickaxe:
	//			return "01_HANDHELD_Picaxe";

	//		case CItemObject::ITEM_TYPE::ITEM_Coal:
	//			return "01_HANDHELD_Coal";
	//		case CItemObject::ITEM_TYPE::ITEM_Stick:
	//			return "01_HANDHELD_Stick";
	//		case CItemObject::ITEM_TYPE::ITEM_Torch:
	//			return "01_HANDHELD_Torch";
	//		}
	//	}
	//	return "01_HANDHELD_CUBE";
	//}

protected:
	explicit CHandHeldItemObject();
	~CHandHeldItemObject() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;


public:
	//void SetItemInfo(const CItemObject::ItemInfo& info)
	//{
	//	//m_ItemInfo = info;
	//	m_viBufferID = { "MC_ITEM_VIBuffer",  CDropItemObject::GetVIBufferName(m_ItemInfo) };
	//}
	void SetVIBufferID(std::pair<StringID, StringID> id)
	{
		m_viBufferID = id;
	}
	void SetRender(_bool b) { m_bRender = b; }
	//void SetPlayerHandle(CHandle h) { m_hPlayerHandle = h; }
	void SetLight(uint8_t l) { m_iLight = l; }
protected:
	//CItemObject::ItemInfo m_ItemInfo{};
	_bool m_bRender{ false };
	//CHandle m_hPlayerHandle{};
	uint8_t m_iLight{0xFF};


protected:
	std::pair<StringID, StringID> m_viBufferID{};

//public:
//	static UPtr<CHandHeldItemObject> Create();
//	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END