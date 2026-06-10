#pragma once
#include "UIObject.h"
#include "ItemObject.h"
NS_BEGIN(Engine)
class CComConstantBuffer;
class ENGINE_DLL CUIInventory final : public E::CUIObject
{
public:
	enum class SlotType
	{
		INVENTORY_HOTBAR,
		INVENTORY,
		ARMOR_HELMET,
		ARMOR_CHESTPLATE,
		ARMOR_LEGGINGS,
		ARMOR_BOOTS,
		SHIELD,
		CRAFT_LT,
		CRAFT_RT,
		CRAFT_LB,
		CRAFT_RB,
		CRAFT_RESULT,

	};
	struct InventorySlot
	{
		_float2 vOriginPos{};
		SlotType eType{};
		CHandle hItem{};
		size_t typeIdx{};
	};
public:
	DECLARE_DERIVED_TYPE(CUIInventory, CUIObject)

private:
	CUIInventory();
	~CUIInventory() override;

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
	_bool m_bRender{ false };

//public:
//	HRESULT AddItemToInventory(const CItemObject::ItemInfo& info);
//
//private:
//	std::optional<size_t> IsCanAddItem(const CItemObject::ItemInfo& info);

private:
	CComConstantBuffer* m_pComCBufferPerObject{};
	CComConstantBuffer* m_pComCBufferPerUI{};

private:
	void InitializeSlot();

public:
	const std::vector<InventorySlot>& GetInvenSlots()const { return m_vecInventorySlot; }
private:
	std::vector<InventorySlot> m_vecInventorySlot{};
	//std::unordered_map<uint64_t, InventorySlot> m_mapInventorySlots{};
private:
	std::optional<CHandle> m_hOnCursorItem{};

	size_t m_InventoryIdxs[9*3]{};
	size_t m_InventoryHotbarIdxs[9]{};

	size_t m_ArmorHelmetIdx{};
	size_t m_ArmorChestplateIdx{};
	size_t m_ArmorLeggingsIdx{};
	size_t m_ArmorBootsIdx{};
	size_t m_ShiledIdx{};

	size_t m_CraftLTIdx{};
	size_t m_CraftRTIdx{};
	size_t m_CraftLBIdx{};
	size_t m_CraftRBIdx{};
	size_t m_CraftResultIdx{};


public:
	void SetUIController(CHandle h) { m_hUIController = h; }
private:
	CHandle m_hUIController{};

public:
	void SetPlayerHandle(CHandle h) { m_hPlayerHandle = h; }
private:
	CHandle m_hPlayerHandle{};

public:
	void SetInventoryHotbarItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size);
	void SetInventoryItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size);

	void SetInventoryArmorItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size);
	void SetInventoryShieldItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size);
	void SetInventoryCraftingItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size);

public:
	std::optional<CHandle> GetHotbarSlotHandle(size_t hotbarIdx) { return m_vecInventorySlot[m_InventoryHotbarIdxs[std::clamp(hotbarIdx, (size_t)0, (size_t)9)]].hItem; };

private:
	void UpdateCrafting(_float fTimeDelta);
public:
	static E::UPtr<CUIInventory> Create();
	E::UPtr<E::CPrototype> Clone(void* pArg) override;
};

NS_END