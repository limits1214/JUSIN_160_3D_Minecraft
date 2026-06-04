#include "pch.h"
#include "UIHotbar.h"
#include "GameInstance.h"
#include "CameraObject.h"
#include "Resources.h"
#include "UIHotbarSelect.h"
NS_USING(Engine)

CUIHotBar::CUIHotBar()
{
}


CUIHotBar::~CUIHotBar()
{
}

void CUIHotBar::UpdateGUI()
{
	CUIObject::UpdateGUI();

	if (ImGui::Button("RenderToggle"))
	{
		m_bRender = !m_bRender;
	}
}

HRESULT CUIHotBar::Initialize(void* pArg)
{
	auto pDesc = static_cast<CUIObject::UIOBJECT_DESC*>(pArg);
	pDesc->fSizeX = 182.f * MC_UI_SCALE;
	pDesc->fSizeY = 22.f * MC_UI_SCALE;

	pDesc->fX = 1280.f * 0.5f;
	pDesc->fY = 720.f - pDesc->fSizeY * 0.5f - MC_UI_SCALE;
	if (FAILED(CUIObject::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUIHotBar::PriorityUpdate(E::_float fTimeDelta)
{
}

void CUIHotBar::Update(E::_float fTimeDelta)
{
}

void CUIHotBar::LateUpdate(E::_float fTimeDelta)
{
	if (m_bRender)
	{
		E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::UI, this);
	}

	GetTransform().Update();
}

HRESULT CUIHotBar::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
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
		auto pCb = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerUI");
		D3D11_MAPPED_SUBRESOURCE mappedSubResource;
		if (SUCCEEDED(pContext->Map(pCb->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
		{

			E::CB_PER_UI perUI{};

			perUI.texIndex = PackTexId(12, 1);
			perUI.texCoord = { 0.f, 0.f };
			perUI.uvSize = { 182.f / 256.f, 22.f / 256.f };

			memcpy(mappedSubResource.pData, &perUI, sizeof(perUI));
			pContext->Unmap(pCb->GetCBuffer().Get(), 0);
		}
		pContext->VSSetConstantBuffers(7, 1, pCb->GetCBuffer().GetAddressOf());
		pContext->PSSetConstantBuffers(7, 1, pCb->GetCBuffer().GetAddressOf());
	}
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

	pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
	return S_OK;
}

CUIHotBarSelect* CUIHotBar::GetHotBarSelect() const
{
	return CGameInstance::Get().GetGameObjectByHandleT<CUIHotBarSelect>(m_hHotbarSelect);
}

E::UPtr<CUIHotBar> CUIHotBar::Create()
{
	auto pInstance = E::ToUPtr(new CUIHotBar{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUIHotBar");
		return nullptr;
	}
	return  pInstance;
}

E::UPtr<E::CPrototype> CUIHotBar::Clone(void* pArg)
{
	auto	pInstance = E::ToUPtr(new CUIHotBar{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIHotBar");
		return nullptr;
	}

	return pInstance;
}
