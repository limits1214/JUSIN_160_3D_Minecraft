#pragma once
#include "UIObject.h"
NS_BEGIN(Engine)
class CComConstantBuffer;
class ENGINE_DLL CUIInventory final : public E::CUIObject
{
private:
	enum class SlotType
	{
		HOTBAR,
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
		std::optional<CHandle> hItem{};
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

private:
	CComConstantBuffer* m_pComCBufferPerObject{};
	CComConstantBuffer* m_pComCBufferPerUI{};

private:
	void InitializeSlot();
private:
	std::vector<InventorySlot> m_vecInventorySlot{};
	//std::unordered_map<uint64_t, InventorySlot> m_mapInventorySlots{};

private:
	std::optional<CHandle> m_hOnCursorItem{};

	size_t m_InventoryIdxs[9*3]{};
	size_t m_HotbarIdxs[9]{};

public:
	static E::UPtr<CUIInventory> Create();
	E::UPtr<E::CPrototype> Clone(void* pArg) override;
};

NS_END