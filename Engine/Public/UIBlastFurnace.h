
#pragma once
#include "UIObject.h"
#include "ItemObject.h"
NS_BEGIN(Engine)
class CComConstantBuffer;
class ENGINE_DLL CUIBlastFurnace final : public E::CUIObject
{
public:
	enum class SlotType
	{
		INVENTORY_HOTBAR,
		INVENTORY,

		INGREDIENT,
		FUEL,
		RESULT,
	};

	struct FurnaceSlot
	{
		_float2 vOriginPos{};
		SlotType eType{};
		CHandle hItem{};
		size_t typeIdx{};
	};

public:
	DECLARE_DERIVED_TYPE(CUIBlastFurnace, CUIObject)

private:
	CUIBlastFurnace();
	~CUIBlastFurnace() override;

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
	CComTransform* m_pComProgressTransform{};
	CComTransform* m_pComFuelTransform{};
	CComConstantBuffer* m_pComCBufferPerObject{};
	CComConstantBuffer* m_pComCBufferPerUI{};

public:
	void SetProcess(_float f) { m_fProcess = f; };
	void SetFuelProgress(_float f) { m_fFuel = f; }
private:
	_float m_fProcess{0.f};
	_float m_fFuel{ 0.f };

public:
	const std::vector<FurnaceSlot>& GetFurnaceSlots()const { return m_vecSlot; }
private:
	void InitializeSlot();
	std::vector<FurnaceSlot> m_vecSlot{};
private:
	size_t m_InventoryIdxs[9 * 3]{};
	size_t m_InventoryHotbarIdxs[9]{};
	size_t m_FuelIdx{};
	size_t m_IngredientIdx{};
	size_t m_ResultIdx{};

public:
	void SetInventoryHotbarItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size);
	void SetInventoryItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size);
	void SetIngredientItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size);
	void SetFuelItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size);
	void SetResultItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size);


public:
	void SetUIController(CHandle h) { m_hUIController = h; }
private:
	CHandle m_hUIController{};

public:
	static E::UPtr<CUIBlastFurnace> Create();
	E::UPtr<E::CPrototype> Clone(void* pArg) override;
};

NS_END