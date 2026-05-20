#include "pch.h"
#include "UIExperienceBarGage.h"
#include "GameInstance.h"
#include "CameraObject.h"
#include "Resources.h"
NS_USING(Engine)

CUIExperienceBarGage::CUIExperienceBarGage()
{
}


CUIExperienceBarGage::~CUIExperienceBarGage()
{
}

void CUIExperienceBarGage::UpdateGUI()
{
	CUIObject::UpdateGUI();

	if (ImGui::Button("RenderToggle"))
	{
		m_bRender = !m_bRender;
	}

	if (ImGui::DragFloat("Gage", &m_fGage, 0.01f, 0.f, 1.f))
	{
		m_bCalcCoord = true;
	}
}

HRESULT CUIExperienceBarGage::Initialize(void* pArg)
{
	auto pDesc = static_cast<CUIObject::UIOBJECT_DESC*>(pArg);
	pDesc->fSizeX = 182.f * MC_UI_SCALE * m_fGage;
	pDesc->fSizeY = 5.f * MC_UI_SCALE;

	pDesc->fX = (1280.f * 0.5f) - (182.f * 0.5f * MC_UI_SCALE) + (pDesc->fSizeX * 0.5f);
	pDesc->fY = 720.f - pDesc->fSizeY * 0.5f - ((24.f + 1.f) * MC_UI_SCALE);
	if (FAILED(CUIObject::Initialize(pArg)))
		return E_FAIL;

	GetTransform().AddPosition(XMVectorSet(0.f, 0.f, -0.02f, 0.f));

	return S_OK;
}

void CUIExperienceBarGage::PriorityUpdate(E::_float fTimeDelta)
{
}

void CUIExperienceBarGage::Update(E::_float fTimeDelta)
{
	//m_fX = (1280.f * 0.5f) - (182.f * 0.5f * MC_UI_SCALE) + (182.f * MC_UI_SCALE * m_fGage * 0.5f);
	//pDesc->fSizeX = 182.f * MC_UI_SCALE * m_fGage;
	//pDesc->fSizeY = 5.f * MC_UI_SCALE;

	//pDesc->fX = (1280.f * 0.5f) - (182.f * 0.5f * MC_UI_SCALE) + (pDesc->fSizeX * 0.5f);

	m_fSizeX = 182.f * MC_UI_SCALE * m_fGage;

	m_fX = (1280.f * 0.5f) - (182.f * 0.5f * MC_UI_SCALE) + (m_fSizeX * 0.5f);
}

void CUIExperienceBarGage::LateUpdate(E::_float fTimeDelta)
{
	if (m_bRender)
	{
		E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::UI, this);
	}
	if (m_bCalcCoord)
	{
		m_bCalcCoord = false;
		CalcUICoord();
	}
	GetTransform().Update();
}

HRESULT CUIExperienceBarGage::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
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
		auto pCbPerObject = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerObject");
		D3D11_MAPPED_SUBRESOURCE mappedSubResource;
		if (SUCCEEDED(pContext->Map(pCbPerObject->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
		{

			E::CB_PER_OBJECT cbPerObject{};
			cbPerObject.matWorld = *GetTransform().GetCombinedWorldMatrix();
			XMStoreFloat4x4(&cbPerObject.matWVP, GetTransform().GetLoadedCombinedWorldMatrix() * ctx.matViewProj);

			memcpy(mappedSubResource.pData, &cbPerObject, sizeof(cbPerObject));
			pContext->Unmap(pCbPerObject->GetCBuffer().Get(), 0);
		}
		pContext->VSSetConstantBuffers(0, 1, pCbPerObject->GetCBuffer().GetAddressOf());
		pContext->PSSetConstantBuffers(0, 1, pCbPerObject->GetCBuffer().GetAddressOf());
	}
	{

		const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP);
		pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
	}



	// draw 2: Gage
	{

		{
			auto pCb = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerUI");
			D3D11_MAPPED_SUBRESOURCE mappedSubResource;
			if (SUCCEEDED(pContext->Map(pCb->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
			{

				E::CB_PER_UI perUI{};

				float fGage = 1.f;

				perUI.texIndex = PackTexId(12, 0);
				perUI.texCoord = { 0.f , 69.f / 256.f };
				perUI.uvSize = { 182.f / 256.f * m_fGage, 5.f / 256.f };

				memcpy(mappedSubResource.pData, &perUI, sizeof(perUI));
				pContext->Unmap(pCb->GetCBuffer().Get(), 0);
			}
			pContext->VSSetConstantBuffers(7, 1, pCb->GetCBuffer().GetAddressOf());
			pContext->PSSetConstantBuffers(7, 1, pCb->GetCBuffer().GetAddressOf());
		}
		pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
	}
	return S_OK;
}

E::UPtr<CUIExperienceBarGage> CUIExperienceBarGage::Create()
{
	auto pInstance = E::ToUPtr(new CUIExperienceBarGage{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUIExperienceBarGage");
		return nullptr;
	}
	return  pInstance;
}

E::UPtr<E::CPrototype> CUIExperienceBarGage::Clone(void* pArg)
{
	auto	pInstance = E::ToUPtr(new CUIExperienceBarGage{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIExperienceBarGage");
		return nullptr;
	}

	return pInstance;
}
