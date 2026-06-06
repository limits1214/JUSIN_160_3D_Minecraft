
#pragma once
#include "UIObject.h"
NS_BEGIN(Engine)
class CComConstantBuffer;
class CComTransform;
class ENGINE_DLL CUIItem final : public E::CUIObject
{
public:
	enum class TYPE
	{
		BLOCK_DIRT,
		BLOCK_COBBLESTONE,
		BLOCK_SAND,
		BLOCK_TNT,
		END
	};

private:
	static CB_PER_UI GetPerUIByType(TYPE eType)
	{
		CB_PER_UI perUI{};
		switch (eType)
		{
		case CUIItem::TYPE::BLOCK_DIRT:
			perUI.texIndex = PackTexId(17, 0);
			perUI.texCoord = { 0 / 300.f, 0 / 300.f };
			perUI.uvSize = { 300.f / 300.f, 300.f / 300.f };
			return perUI;
		case CUIItem::TYPE::BLOCK_COBBLESTONE:
			perUI.texIndex = PackTexId(17, 1);
			perUI.texCoord = { 0 / 300.f, 0 / 300.f };
			perUI.uvSize = { 300.f / 300.f, 300.f / 300.f };
			return perUI;
		case CUIItem::TYPE::BLOCK_SAND:
			perUI.texIndex = PackTexId(17, 2);
			perUI.texCoord = { 0 / 300.f, 0 / 300.f };
			perUI.uvSize = { 300.f / 300.f, 300.f / 300.f };
			return perUI;
		case CUIItem::TYPE::BLOCK_TNT:
			perUI.texIndex = PackTexId(17, 3);
			perUI.texCoord = { 0 / 300.f, 0 / 300.f };
			perUI.uvSize = { 300.f / 300.f, 300.f / 300.f };
			return perUI;
		case CUIItem::TYPE::END:
			return perUI;
		}
		return perUI;
	}
public:
	typedef struct tagDesc : public CUIObject::UIOBJECT_DESC
	{
		TYPE eType;
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
	void SetCnt(uint8_t i) { m_iCnt = std::clamp(i, (uint8_t)1, (uint8_t)64); }
	uint8_t GetCnt() const { return m_iCnt; }
private:
	uint8_t m_iCnt{ 1 };

public:
	void SetDurability(_float f) { m_fDurability = f; }
	_float GetDurability() const { return m_fDurability ; }
private:
	_float m_fDurability{ 1.f };

public:
	_bool GetOnCursor() const { return m_bOnCursor; }
	void SetOnCursor(_bool b) { m_bOnCursor = b; }
private:
	_bool m_bOnCursor{ false };


public:
	TYPE GetItemType() const { return m_eType; }
	void SetItemType(TYPE eType) { m_eType = eType; }
private:
	TYPE m_eType{TYPE::END};
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