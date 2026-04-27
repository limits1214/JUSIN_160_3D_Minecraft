#include "pch.h"
#include "UIObject.h"
#include "GameInstance.h"

NS_USING(Engine)

CUIObject::CUIObject()
{
}

CUIObject::~CUIObject()
{
}

HRESULT CUIObject::Initialize(void* pArg)
{
	auto		pDesc = static_cast<UIOBJECT_DESC*>(pArg);

	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;

	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;
	RECT rect;
	GetClientRect(CGameInstance::Get().GetHwnd(), &rect);
	auto clientWidth = rect.right;
	auto clientHeight = rect.bottom;

	GetTransform().SetScale(E::_float3{ m_fSizeX, m_fSizeY, 1.f });
	auto a = m_fX - clientWidth * 0.5f;
	auto b = -m_fY + clientHeight * 0.5f;
	GetTransform().SetPosition(XMVectorSet(m_fX - clientWidth * 0.5f, -m_fY + clientHeight * 0.5f, 0.f, 1.f));

	return S_OK;
}