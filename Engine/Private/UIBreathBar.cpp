#include "pch.h"
#include "UIBreathBar.h"
#include "GameInstance.h"
#include "CameraObject.h"
#include "Resources.h"
NS_USING(Engine)

CUIBreathBar::CUIBreathBar()
{
}


CUIBreathBar::~CUIBreathBar()
{
}

void CUIBreathBar::UpdateGUI()
{
	CUIObject::UpdateGUI();

	if (ImGui::Button("RenderToggle"))
	{
		m_bRender = !m_bRender;
	}
}

HRESULT CUIBreathBar::Initialize(void* pArg)
{
	auto pDesc = static_cast<CUIObject::UIOBJECT_DESC*>(pArg);
	pDesc->fSizeX = 9.f * MC_UI_SCALE;
	pDesc->fSizeY = 9.f * MC_UI_SCALE;

	pDesc->fX = (1280.f * 0.5f) + (1.f * MC_UI_SCALE) + (pDesc->fSizeY * 0.5f) + (pDesc->fSizeY * 0.f);
	pDesc->fY = 720.f - pDesc->fSizeY * 0.5f - ((24.f + 8.f + 11.f) * MC_UI_SCALE);
	if (FAILED(CUIObject::Initialize(pArg)))
		return E_FAIL;

	GetTransform().AddPosition(XMVectorSet(0.f, 0.f, -0.01f, 0.f));

	return S_OK;
}

void CUIBreathBar::PriorityUpdate(E::_float fTimeDelta)
{
}

void CUIBreathBar::Update(E::_float fTimeDelta)
{
}

void CUIBreathBar::LateUpdate(E::_float fTimeDelta)
{
	if (m_bRender)
	{
		//E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::UI, this);
	}

	GetTransform().Update();


	for (uint32_t i = 0; i < GetChildrenNode().size(); ++i)
	{
		GetChildrenNode()[i]->GetTransform().SetPosition(GetTransform().GetLoadedPostion());
		GetChildrenNode()[i]->GetTransform().AddPosition(XMVectorSet((9.f * i), 0.f, 0.f, 0.f));
	}
}

HRESULT CUIBreathBar::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
	
	return S_OK;
}

E::UPtr<CUIBreathBar> CUIBreathBar::Create()
{
	auto pInstance = E::ToUPtr(new CUIBreathBar{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUIBreathBar");
		return nullptr;
	}
	return  pInstance;
}

E::UPtr<E::CPrototype> CUIBreathBar::Clone(void* pArg)
{
	auto	pInstance = E::ToUPtr(new CUIBreathBar{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIBreathBar");
		return nullptr;
	}

	return pInstance;
}
