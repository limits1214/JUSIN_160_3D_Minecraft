#include "UIDeathScreen.h"
#include "ComConstantBuffer.h"
#include "GameInstance.h"
#include "Resources.h"

#include "PlayerEntity.h"
NS_USING(Engine)

CUIDeathScreen::CUIDeathScreen()
{
}

CUIDeathScreen::~CUIDeathScreen()
{
}

void CUIDeathScreen::UpdateGUI()
{
}

HRESULT CUIDeathScreen::Initialize(void* pArg)
{
	auto pDesc = static_cast<CUIObject::UIOBJECT_DESC*>(pArg);
	//pDesc->fSizeX = 24.f * MC_UI_SCALE * 3.f;
	//pDesc->fSizeY = 24.f * MC_UI_SCALE;

	//pDesc->fX = (1280.f * 0.5f) + (1.f * MC_UI_SCALE) + (pDesc->fSizeY * 0.5f) + (pDesc->fSizeY * 0.f);
	//pDesc->fY = 720.f - pDesc->fSizeY * 0.5f - ((24.f + 8.f) * MC_UI_SCALE);
	if (FAILED(CUIObject::Initialize(pArg)))
		return E_FAIL;

	CComConstantBuffer::DESC Desc{};
	Desc.cBufferId = { TAG_RES_GRP_PERMANENT_BUFFER, TAG_RES_CBUFFER_OBJECT };
	if (FAILED(AddComponentFromProto("PERMANENT", "Prototype_Component_ConstantBuffer", "ComCBufferPerObject", &Desc, &m_pComCBufferPerObject)))
	{
		return E_FAIL;
	};

	Desc.cBufferId = { TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerUI" };
	if (FAILED(AddComponentFromProto("PERMANENT", "Prototype_Component_ConstantBuffer", "ComCBufferPerUI", &Desc, &m_pComCBufferPerUI)))
	{
		return E_FAIL;
	};

	//GetTransform().AddPosition(XMVectorSet(0.f, 0.f, -0.02f, 0.f));

    return S_OK;
}

void CUIDeathScreen::PriorityUpdate(E::_float fTimeDelta)
{
}

void CUIDeathScreen::Update(E::_float fTimeDelta)
{
	
}

void CUIDeathScreen::LateUpdate(E::_float fTimeDelta)
{
	E::CGameInstance::Get().RendererSetFilterRed(m_bRender);
	if (m_bRender)
	{
		E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::UI_TEXT_AFTER_FILTERRED, this);
	}

	GetTransform().Update();

	if (m_bRender)
	{
		if (CGameInstance::Get().KeyDown(DIK_R))
		{
			if (auto pPlayer = CGameInstance::Get().GetGameObjectByHandleT<CPlayerEntity>(m_hPlayer))
			{
				pPlayer->ReSpawnFromDeath();
			}
		}
	}
}

HRESULT CUIDeathScreen::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
	{
		auto size = E::CGameInstance::Get().FontMeasureString("NeoDGM_20px", L"사망하셨습니다.", 1.f);
		auto x = (1280.f * 0.5f) - (size.x * 0.5f);
		auto y = (720 * 0.5f) - (size.y * 0.5f);
		E::CGameInstance::Get().FontAddLateDraw(RENDERGROUP::UI_TEXT_AFTER_FILTERRED, "NeoDGM_20px", L"사망하셨습니다.", { x, y });
	}

	{
		auto size = E::CGameInstance::Get().FontMeasureString("NeoDGM_20px", L"Score: 01101001", 1.f);
		auto x = (1280.f * 0.5f) - (size.x * 0.5f);
		auto y = (720 * 0.5f) - (size.y * 0.5f) + 20.f;
		E::CGameInstance::Get().FontAddLateDraw(RENDERGROUP::UI_TEXT_AFTER_FILTERRED, "NeoDGM_20px", L"Score: 01101001", { x, y });
	}

	{
		auto size = E::CGameInstance::Get().FontMeasureString("NeoDGM_20px", L"R을 눌러 부활하세요", 1.f);
		auto x = (1280.f * 0.5f) - (size.x * 0.5f);
		auto y = (720 * 0.5f) - (size.y * 0.5f) + 40.f;
		E::CGameInstance::Get().FontAddLateDraw(RENDERGROUP::UI_TEXT_AFTER_FILTERRED, "NeoDGM_20px", L"R을 눌러 부활하세요", { x, y });
	}


	//CGameInstance::Get().FontAddLateDraw(RENDERGROUP::UI, "NeoDGM_20px", L"사망하셨습니다.", {});

	//CGameInstance::Get().FontAddLateDraw(RENDERGROUP::UI, "NeoDGM_20px", L"R 을눌러 부활하세요", {});
    return S_OK;
}

E::UPtr<CUIDeathScreen> CUIDeathScreen::Create()
{
	auto pInstance = E::ToUPtr(new CUIDeathScreen{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUIDeathScreen");
		return nullptr;
	}
	return  pInstance;
}


E::UPtr<E::CPrototype> CUIDeathScreen::Clone(void* pArg)
{
	auto	pInstance = E::ToUPtr(new CUIDeathScreen{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIDeathScreen");
		return nullptr;
	}

	return pInstance;
}
