
#pragma once

#include "UIObject.h"

#include "ItemObject.h"

NS_BEGIN(Engine)
class CComConstantBuffer;
class CComTransform;
class ENGINE_DLL CUIItem final : public E::CUIObject
{
private:
	static CB_PER_UI GetPerUIByType(CBlock3::TYPE eType)
	{
		CB_PER_UI perUI{};
		perUI.texCoord = { 0 / 300.f, 0 / 300.f };
		perUI.uvSize = { 300.f / 300.f, 300.f / 300.f };
		switch (eType)
		{
		case CBlock3::TYPE::DIRT:
			perUI.texIndex = PackTexId(17, 0);
			return perUI;
		case CBlock3::TYPE::COBBLESTONE:
			perUI.texIndex = PackTexId(17, 1);
			return perUI;
		case CBlock3::TYPE::SAND:
			perUI.texIndex = PackTexId(17, 2);
			return perUI;
		case CBlock3::TYPE::TNT:
			perUI.texIndex = PackTexId(17, 3);
			return perUI;
		case CBlock3::TYPE::LOG_ACACIA:
			perUI.texIndex = PackTexId(17, 4);
			return perUI;
		case CBlock3::TYPE::PLANK_ACACIA:
			perUI.texIndex = PackTexId(17, 5);
			return perUI;
		case CBlock3::TYPE::LOG_BIRCH:
			perUI.texIndex = PackTexId(17, 6);
			return perUI;
		case CBlock3::TYPE::PLANK_BIRCH:
			perUI.texIndex = PackTexId(17, 7);
			return perUI;
		case CBlock3::TYPE::LOG_CHERRY:
			perUI.texIndex = PackTexId(17, 8);
			return perUI;
		case CBlock3::TYPE::PLANK_CHERRY:
			perUI.texIndex = PackTexId(17, 9);
			return perUI;
		case CBlock3::TYPE::LOG_OAK:
			perUI.texIndex = PackTexId(17, 10);
			return perUI;
		case CBlock3::TYPE::PLANK_OAK:
			perUI.texIndex = PackTexId(17, 11);
			return perUI;
		case CBlock3::TYPE::OBSIDIAN:
			perUI.texIndex = PackTexId(17, 12);
			return perUI;
		case CBlock3::TYPE::GRASS:
			perUI.texIndex = PackTexId(17, 13);
			return perUI;
		case CBlock3::TYPE::CRAFTING_TABLE:
			perUI.texIndex = PackTexId(17, 14);
			return perUI;
		case CBlock3::TYPE::CHEST:
			perUI.texIndex = PackTexId(17, 15);
			return perUI;
		case CBlock3::TYPE::FURNACE:
			perUI.texIndex = PackTexId(17, 16);
			return perUI;
		case CBlock3::TYPE::END:
			return perUI;
		}
		return perUI;
	}
	static CB_PER_UI GetPerUIByType(CItemObject::ITEM_TYPE eType)
	{
		CB_PER_UI perUI{};
		perUI.texIndex = CItemObject::GetPackedTexIdByType(eType);
		perUI.texCoord = { 0 / 16.f, 0 / 16.f };
		perUI.uvSize = { 16.f / 16.f, 16.f / 16.f };
		switch (eType)
		{
		case CItemObject::ITEM_TYPE::END:
			return perUI;
		}
		return perUI;
	}
public:
	typedef struct tagDesc : public CUIObject::UIOBJECT_DESC
	{
		//CItemObject::ITEM_TYPE eType;
		CItemObject::ItemInfo itemInfo{};
	}DESC;
public:
	DECLARE_DERIVED_TYPE(CUIItem, CUIObject)

private:
	CUIItem();
	~CUIItem() override;

public:
	void UpdateGUI() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;
	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

public:
	void SetRender(_bool b) { m_bRender = b; }
	_bool GetRender() const { return m_bRender; }

private:
	_bool m_bRender{ true };

public:
	void SetIsOnCursor() { m_bOnCursor = true; }
private:
	_bool m_bOnCursor{ false };

public:
	void SetCnt(uint8_t i) { m_iCnt = std::clamp(i, (uint8_t)1, (uint8_t)64); }
	uint8_t GetCnt() const { return m_iCnt; }
private:
	uint8_t m_iCnt{ 1 };

//public:
//	void SetDurability(_float f) { m_fDurability = f; }
//	_float GetDurability() const { return m_fDurability ; }
//private:
//	_float m_fDurability{ 1.f };

//public:
//	_bool GetOnCursor() const { return m_bOnCursor; }
//	void SetOnCursor(_bool b) { m_bOnCursor = b; }
//private:
//	_bool m_bOnCursor{ false };

public:
	//CItemObject::ItemInfo GetItemInfo()
	//{
	//	if (m_ItemInfo)
	//	{
	//		return m_ItemInfo.value();
	//	}
	//	return {};
	//}
	CItemObject::ItemInfo* GetItemInfoPtr()
	{
		if (!m_ItemInfo.has_value())
			return nullptr;

		return &m_ItemInfo.value();
	}
	void SetItemInfo(std::optional<CItemObject::ItemInfo> info)
	{
		// TODO: DirtyCheck
		m_ItemInfo = info; 

		RePerUI();
	}
	void RePerUI()
	{
		if (m_ItemInfo)
		{
			if (m_ItemInfo->block)
			{
				m_PerUI = GetPerUIByType(m_ItemInfo->block.value().GetType());
			}
			else
			{
				m_PerUI = GetPerUIByType(m_ItemInfo->eItemType);
			}
		}
	}
	//CItemObject::ItemInfo& GetItemInfoRef() { return m_ItemInfo; }
private:
	std::optional<CItemObject::ItemInfo> m_ItemInfo{};

public:
	//CItemObject::ITEM_TYPE GetItemType() const { return m_eType; }
	//void SetItemType(CItemObject::ITEM_TYPE eType) { m_eType = eType; }
private:
	//CItemObject::ITEM_TYPE m_eType{ CItemObject::ITEM_TYPE::END};
	CB_PER_UI m_PerUI{};

private:
	CComTransform* m_pComDurabilityBgTransform{};
	CComTransform* m_pComDurabilityGageTransform{};
	CComConstantBuffer* m_pComCBufferPerObject{};
	CComConstantBuffer* m_pComCBufferPerUI{};

public:
	static E::UPtr<CUIItem> Create();
	E::UPtr<E::CPrototype> Clone(void* pArg) override;
};

NS_END