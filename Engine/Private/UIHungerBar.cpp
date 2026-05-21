#include "pch.h"
#include "UIHungerBar.h"
#include "GameInstance.h"
#include "CameraObject.h"
#include "Resources.h"
#include "UIHungerBarIcon.h"
NS_USING(Engine)

CUIHungerBar::CUIHungerBar()
{
}


CUIHungerBar::~CUIHungerBar()
{
}

void CUIHungerBar::UpdateGUI()
{
	CUIObject::UpdateGUI();

	if (ImGui::Button("RenderToggle"))
	{
		m_bRender = !m_bRender;
	}

	ImGui::DragInt("HalfHungerCnt", &m_iCurrentHalfHungerCnt, 1, 0, 20);
}

HRESULT CUIHungerBar::Initialize(void* pArg)
{
	auto pDesc = static_cast<CUIObject::UIOBJECT_DESC*>(pArg);
	pDesc->fSizeX = 9.f * MC_UI_SCALE;
	pDesc->fSizeY = 9.f * MC_UI_SCALE;

	pDesc->fX = (1280.f * 0.5f) + (1.f * MC_UI_SCALE) + (pDesc->fSizeY * 0.5f) + (pDesc->fSizeY * 0.f);
	pDesc->fY = 720.f - pDesc->fSizeY * 0.5f - ((24.f + 8.f) * MC_UI_SCALE);
	if (FAILED(CUIObject::Initialize(pArg)))
		return E_FAIL;

	GetTransform().AddPosition(XMVectorSet(0.f, 0.f, -0.01f, 0.f));

	return S_OK;
}

void CUIHungerBar::PriorityUpdate(E::_float fTimeDelta)
{
}

void CUIHungerBar::Update(E::_float fTimeDelta)
{
	auto imaxHealthCnt = GetChildrenNode().size();
	auto iFullHealthCnt = (imaxHealthCnt * 2 - m_iCurrentHalfHungerCnt) / 2;
	auto iHalfHealthCnt = (imaxHealthCnt * 2 - m_iCurrentHalfHungerCnt) % 2;

	for (uint32_t i = 0; i < imaxHealthCnt; ++i)
	{
		if (auto* icon = Cast<CUIHungerBarIcon>(GetChildrenNode()[i]))
		{
			if (i < iFullHealthCnt)
			{
				icon->SetIconType(CUIHungerBarIcon::HUNGER_ICON_TYPE::EMPTY);
			}
			else
			{
				if (i == iFullHealthCnt && iHalfHealthCnt > 0)
				{
					icon->SetIconType(CUIHungerBarIcon::HUNGER_ICON_TYPE::HALF);
				}
				else
				{
					icon->SetIconType(CUIHungerBarIcon::HUNGER_ICON_TYPE::FULL);
				}
			}

		}
	}
}

void CUIHungerBar::LateUpdate(E::_float fTimeDelta)
{
	if (m_bRender)
	{
		//E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::UI, this);
	}

	GetTransform().Update();


	for (uint32_t i = 0; i < GetChildrenNode().size(); ++i)
	{
		GetChildrenNode()[i]->GetTransform().SetPosition(GetTransform().GetLoadedPostion());
		GetChildrenNode()[i]->GetTransform().AddPosition(XMVectorSet((9.f * i * MC_UI_SCALE), 0.f, 0.f, 0.f));


		auto tmp = GetChildrenNode()[i]->GetTransform().GetPosition();
		tmp.z -= 0.01f;
		GetChildrenNode()[i]->GetComponent<CComTransform>("Com_OverlayTransform")->SetPosition(tmp);
	}
}

HRESULT CUIHungerBar::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
	
	return S_OK;
}

E::UPtr<CUIHungerBar> CUIHungerBar::Create()
{
	auto pInstance = E::ToUPtr(new CUIHungerBar{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUIHungerBar");
		return nullptr;
	}
	return  pInstance;
}

E::UPtr<E::CPrototype> CUIHungerBar::Clone(void* pArg)
{
	auto	pInstance = E::ToUPtr(new CUIHungerBar{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIHungerBar");
		return nullptr;
	}

	return pInstance;
}
