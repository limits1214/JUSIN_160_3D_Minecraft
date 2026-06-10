#pragma once
#include "UIObject.h"
NS_BEGIN(Engine)
class CComConstantBuffer;
class ENGINE_DLL CUICraftingTable final : public E::CUIObject
{
public:
	enum class SlotType
	{
		INVENTORY_HOTBAR,
		INVENTORY,
		
		CRAFT_LT,
		CRAFT_MT,
		CRAFT_RT,
		CRAFT_LM,
		CRAFT_MM,
		CRAFT_RM,
		CRAFT_LB,
		CRAFT_MB,
		CRAFT_RB,

		CRAFT_RESULT,

	};
	struct CraftingTableSlot
	{
		_float2 vOriginPos{};
		SlotType eType{};
		CHandle hItem{};
		size_t typeIdx{};
	};
public:
	DECLARE_DERIVED_TYPE(CUICraftingTable, CUIObject)

private:
	CUICraftingTable();
	~CUICraftingTable() override;

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

private:
	void InitializeSlot();

public:
	const std::vector<CraftingTableSlot>& GetCraftingTableSlots()const { return m_vecSlot; }
private:
	std::vector<CraftingTableSlot> m_vecSlot{};
private:

	size_t m_InventoryIdxs[9 * 3]{};
	size_t m_InventoryHotbarIdxs[9]{};

	size_t m_CraftLTIdx{};
	size_t m_CraftMTIdx{};
	size_t m_CraftRTIdx{};
	size_t m_CraftLMIdx{};
	size_t m_CraftMMIdx{};
	size_t m_CraftRMIdx{};
	size_t m_CraftLBIdx{};
	size_t m_CraftMBIdx{};
	size_t m_CraftRBIdx{};
	size_t m_CraftResultIdx{};

private:
	CComConstantBuffer* m_pComCBufferPerObject{};
	CComConstantBuffer* m_pComCBufferPerUI{};

public:
	void SetInventoryHotbarItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size);
	void SetInventoryItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size);
	void SetCraftingTableCraftingItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size);
public:
	static E::UPtr<CUICraftingTable> Create();
	E::UPtr<E::CPrototype> Clone(void* pArg) override;
};

NS_END