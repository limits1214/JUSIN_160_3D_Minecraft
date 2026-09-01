#pragma once
#include "UIObject.h"
#include "ItemObject.h"
NS_BEGIN(Engine)
class CComConstantBuffer;
class ENGINE_DLL CUIChest final : public E::CUIObject
{
public:
	enum class SlotType
	{
		INVENTORY_HOTBAR,
		INVENTORY,

		CHEST_INVENTORY,
	};

	struct ChestSlot
	{
		_float2 vOriginPos{};
		SlotType eType{};
		CHandle hItem{};
		size_t typeIdx{};
	};

public:
	DECLARE_DERIVED_TYPE(CUIChest, CUIObject)

private:
	CUIChest();
	~CUIChest() override;

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
	CComConstantBuffer* m_pComCBufferPerObject{};
	CComConstantBuffer* m_pComCBufferPerUI{};

public:
	const std::vector<ChestSlot>& GetChestSlots()const { return m_vecSlot; }
private:
	void InitializeSlot();
	std::vector<ChestSlot> m_vecSlot{};
private:
	size_t m_InventoryIdxs[9 * 3]{};
	size_t m_InventoryHotbarIdxs[9]{};
	size_t m_ChestInventoryIdxs[9 * 3]{};

public:
	void SetInventoryHotbarItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size);
	void SetInventoryItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size);
	void SetChestInventoryItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size);


public:
	void SetUIController(CHandle h) { m_hUIController = h; }
private:
	CHandle m_hUIController{};


public:
	static E::UPtr<CUIChest> Create();
	E::UPtr<E::CPrototype> Clone(void* pArg) override;
};

NS_END