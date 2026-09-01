
#pragma once
#include "UIObject.h"
NS_BEGIN(Engine)
class CComConstantBuffer;
class ENGINE_DLL CUITextBg final : public E::CUIObject
{
public:
	DECLARE_DERIVED_TYPE(CUITextBg, CUIObject)

private:
	CUITextBg();
	~CUITextBg() override;

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

private:
	_bool m_bRender{ false };

	CComConstantBuffer* m_pComCBufferPerObject{};
	CComConstantBuffer* m_pComCBufferPerUI{};

private:
	float m_paddingX{ 8.f * MC_UI_SCALE };
	float m_paddingY{ 4.f * MC_UI_SCALE };
	std::wstring m_text{};
	float m_textWidth{};
	float m_textHeight{};

public:
	void SetText(std::wstring_view text, float scale = 1.f)
	{
		m_text = text;

		auto size = E::CGameInstance::Get().FontMeasureString("NeoDGM_15px", m_text.c_str(), scale);
		m_textWidth = size.x;
		m_textHeight = size.y;

		m_fSizeX = m_textWidth + m_paddingX * 2.f;
		m_fSizeY = m_textHeight + m_paddingY * 2.f;

		CalcUICoord();
	}

	//void SetText(std::wstring_view text, float textWidth, float textHeight)
	//{
	//	m_text = text;
	//	m_textWidth = textWidth;
	//	m_textHeight = textHeight;

	//	m_fSizeX = textWidth + m_paddingX * 2.f;
	//	m_fSizeY = textHeight + m_paddingY * 2.f;

	//	// 트랜스폼 스케일 갱신
	//	//GetTransform().SetScale(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	//	CalcUICoord();
	//}

public:
	static E::UPtr<CUITextBg> Create();
	E::UPtr<E::CPrototype> Clone(void* pArg) override;
};

NS_END