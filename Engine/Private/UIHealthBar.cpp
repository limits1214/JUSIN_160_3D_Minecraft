#include "pch.h"
#include "UIHealthBar.h"
#include "GameInstance.h"
#include "CameraObject.h"
#include "Resources.h"
#include "UIHealthBarIcon.h"
NS_USING(Engine)

CUIHealthBar::CUIHealthBar()
{
}


CUIHealthBar::~CUIHealthBar()
{
}

void CUIHealthBar::UpdateGUI()
{
	CUIObject::UpdateGUI();

	if (ImGui::Button("RenderToggle"))
	{
		m_bRender = !m_bRender;
	}

	ImGui::DragInt("HalfHealthCnt", &m_iCurrentHalfHealthCnt, 1, 0, 20);


	//if (ImGui::Button("TEST"))
	//{
	//	if (auto pLayer = CGameInstance::Get().GetGameObjectLayer("80_UIx"))
	//	{
	//		if (!pLayer->empty())
	//		{
	//			if (auto pObj = CGameInstance::Get().GetGameObjectByHandle(pLayer->front()))
	//			{
	//				auto copy = GetChildrenNode();
	//				//80_UIHealthIcon
	//				for (auto& icon : copy)
	//				{
	//					icon->SetParentNode(pObj);
	//				}
	//			}
	//		}
	//	}
	//}
}

HRESULT CUIHealthBar::Initialize(void* pArg)
{
	auto pDesc = static_cast<CUIObject::UIOBJECT_DESC*>(pArg);
	pDesc->fSizeX = 9.f * MC_UI_SCALE;
	pDesc->fSizeY = 9.f * MC_UI_SCALE;

	pDesc->fX = (1280.f * 0.5f) - (91.f * MC_UI_SCALE) + (pDesc->fSizeY * 0.5f) + (pDesc->fSizeY * 0.f);
	pDesc->fY = 720.f - pDesc->fSizeY * 0.5f - ((24.f + 8.f) * MC_UI_SCALE);
	if (FAILED(CUIObject::Initialize(pArg)))
		return E_FAIL;



	GetTransform().AddPosition(XMVectorSet(0.f, 0.f, -0.01f, 0.f));

	return S_OK;
}

void CUIHealthBar::PriorityUpdate(E::_float fTimeDelta)
{
}

void CUIHealthBar::Update(E::_float fTimeDelta)
{
	auto imaxHealthCnt = GetChildrenNode().size();
	auto iFullHealthCnt = m_iCurrentHalfHealthCnt / 2;
	auto iHalfHealthCnt = m_iCurrentHalfHealthCnt % 2;

	for (int32_t i = 0; i < imaxHealthCnt; ++i)
	{
		if (auto* icon = Cast<CUIHealthBarIcon>(GetChildrenNode()[i]))
		{
			if (i < iFullHealthCnt)
			{
				icon->SetIconType(CUIHealthBarIcon::HEALTH_ICON_TYPE::FULL);
			}
			else
			{
				if (i == iFullHealthCnt && iHalfHealthCnt > 0)
				{
					icon->SetIconType(CUIHealthBarIcon::HEALTH_ICON_TYPE::HALF);
				}
				else
				{
					icon->SetIconType(CUIHealthBarIcon::HEALTH_ICON_TYPE::EMPTY);
				}
			}
			
		}
	}
}

void CUIHealthBar::LateUpdate(E::_float fTimeDelta)
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

HRESULT CUIHealthBar::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
	
	return S_OK;
}

E::UPtr<CUIHealthBar> CUIHealthBar::Create()
{
	auto pInstance = E::ToUPtr(new CUIHealthBar{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUIHealthBar");
		return nullptr;
	}
	return  pInstance;
}

E::UPtr<E::CPrototype> CUIHealthBar::Clone(void* pArg)
{
	auto	pInstance = E::ToUPtr(new CUIHealthBar{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIHealthBar");
		return nullptr;
	}

	return pInstance;
}
