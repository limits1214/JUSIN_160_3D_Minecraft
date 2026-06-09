#include "pch.h"
#include "UITextBg.h"
#include "GameInstance.h"
#include "CameraObject.h"
#include "Resources.h"
#include "UIHungerBarIcon.h"
#include "ComConstantBuffer.h"
NS_USING(Engine)

CUITextBg::CUITextBg()
{
}


CUITextBg::~CUITextBg()
{
}

void CUITextBg::UpdateGUI()
{
	CUIObject::UpdateGUI();

	if (ImGui::Button("RenderToggle"))
	{
		m_bRender = !m_bRender;
	}

}

HRESULT CUITextBg::Initialize(void* pArg)
{
	auto pDesc = static_cast<CUIObject::UIOBJECT_DESC*>(pArg);
	pDesc->fSizeX = 24.f * MC_UI_SCALE * 3.f;
	pDesc->fSizeY = 24.f * MC_UI_SCALE;

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

	GetTransform().AddPosition(XMVectorSet(0.f, 0.f, -0.02f, 0.f));

	SetText(L"ASDF");

	return S_OK;
}

void CUITextBg::PriorityUpdate(E::_float fTimeDelta)
{
}

void CUITextBg::Update(E::_float fTimeDelta)
{
	if (m_bRender)
	{
		POINT mousePos;
		GetCursorPos(&mousePos);
		ScreenToClient(CGameInstance::Get().GetHwnd(), &mousePos);

		SetOrigin(_float2{ (float)mousePos.x, (float)mousePos.y });
	}

}

void CUITextBg::LateUpdate(E::_float fTimeDelta)
{
	if (m_bRender)
	{
		E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::UI_TOOLTIP, this);
	}

	GetTransform().Update();

}

HRESULT CUITextBg::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
	const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_UI");
	const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_UI");

	pContext->IASetInputLayout(nullptr);
	pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
	pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);

	const auto& viBuffer = E::CGameInstance::Get().GetResourceFirst<E::CResQuadTexBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "VIBuffer_QuadTex");

	pContext->IASetInputLayout(vs->GetInputLayout().Get());
	pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
	pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);

	ID3D11Buffer* vertexBuffers[] = {
			viBuffer->GetVertexBuffer().Get()
	};
	uint32_t strides[] = {
		viBuffer->GetVertexStride()
	};
	uint32_t offsets[] = {
		0
	};
	pContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
	pContext->IASetIndexBuffer(viBuffer->GetIndexBuffer().Get(), viBuffer->GetIndexFormat(), 0);
	pContext->IASetPrimitiveTopology(viBuffer->GetPrimitiveType());
	{
		const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP);
		pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
	}



	{
		E::CB_PER_UI perUI{};
		perUI.texIndex = PackTexId(12, 0);
		//perUI.texCoord = { 0.f / 256.f, 112.f / 256.f };
		//perUI.uvSize = { 24.f / 256.f, 24.f / 256.f };

		perUI.texCoord = { 0.f / 256.f, 112.f / 256.f };
		perUI.uvSize = { 24.f / 256.f, 24.f / 256.f };
		perUI.borderUV = { 4.f / 256.f,  4.f / 256.f };  // atlas에서 border 4px
		perUI.borderPx = { 4.f * MC_UI_SCALE, 4.f * MC_UI_SCALE };
		perUI.rectSizePx = { m_fSizeX, m_fSizeY };
		
		
		{
			float textX = m_fX - m_fSizeX * 0.5f + m_paddingX;
			float textY = m_fY - m_fSizeY * 0.5f + m_paddingY;
			E::CGameInstance::Get().FontAddLateDraw(RENDERGROUP::UI_TOOLTIP, "NeoDGM_20px", m_text.c_str(), { textX, textY });

			//perUI.rectSizePx = { m_fSizeX, m_fSizeY };

			//// 텍스트를 박스 좌상단 기준 패딩만큼 오프셋
			//float textX = m_fX - m_fSizeX * 0.5f + m_paddingX;
			//float textY = m_fY - m_fSizeY * 0.5f + m_paddingY;
			//E::CGameInstance::Get().FontAddLateDraw("NeoDGM_20px", m_text.c_str(), { textX, textY });
		}

		if (FAILED(m_pComCBufferPerUI->MapDiscard(pContext, &perUI, sizeof(perUI))))
		{
			return E_FAIL;
		}
		pContext->VSSetConstantBuffers(7, 1, m_pComCBufferPerUI->GetAdressOfBuffer());
		pContext->PSSetConstantBuffers(7, 1, m_pComCBufferPerUI->GetAdressOfBuffer());
	}
	{
		E::CB_PER_OBJECT cbPerObject{};
		cbPerObject.matWorld = *GetTransform().GetCombinedWorldMatrix();
		XMStoreFloat4x4(&cbPerObject.matWVP, GetTransform().GetLoadedCombinedWorldMatrix() * ctx.matViewProj);
		if (FAILED(m_pComCBufferPerObject->MapDiscard(pContext, &cbPerObject, sizeof(cbPerObject))))
		{
			return E_FAIL;
		}
		pContext->VSSetConstantBuffers(0, 1, m_pComCBufferPerObject->GetAdressOfBuffer());
		pContext->PSSetConstantBuffers(0, 1, m_pComCBufferPerObject->GetAdressOfBuffer());
	}

	pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);



	
	/*E::CGameInstance::Get().FontAddLateDraw("NeoDGM_20px", L"ASDF", {m_fX , m_fY});*/
	return S_OK;
}

E::UPtr<CUITextBg> CUITextBg::Create()
{
	auto pInstance = E::ToUPtr(new CUITextBg{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUITextBg");
		return nullptr;
	}
	return  pInstance;
}

E::UPtr<E::CPrototype> CUITextBg::Clone(void* pArg)
{
	auto	pInstance = E::ToUPtr(new CUITextBg{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUITextBg");
		return nullptr;
	}

	return pInstance;
}
