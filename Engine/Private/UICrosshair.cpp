#include "pch.h"
#include "UICrossHair.h"
#include "GameInstance.h"
#include "CameraObject.h"
#include "Resources.h"
NS_USING(Engine)

CUICrosshair::CUICrosshair()
{
}


CUICrosshair::~CUICrosshair()
{
}

void CUICrosshair::UpdateGUI()
{
	CUIObject::UpdateGUI();

	if (ImGui::Button("RenderToggle"))
	{
		m_bRender = !m_bRender;
	}
}

HRESULT CUICrosshair::Initialize(void* pArg)
{
	if (FAILED(CUIObject::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUICrosshair::PriorityUpdate(E::_float fTimeDelta)
{
}

void CUICrosshair::Update(E::_float fTimeDelta)
{
}

void CUICrosshair::LateUpdate(E::_float fTimeDelta)
{
	if (m_bRender)
	{
		E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::UI, this);
	}
	
	GetTransform().Update();
}

HRESULT CUICrosshair::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
	const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Crosshair");
	const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Crosshair");
	
	pContext->IASetInputLayout(nullptr);
	pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
	pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);

	//ID3D11Buffer* vertexBuffers[] = {
	//		viBuffer->GetVertexBuffer().Get()
	//};
	//uint32_t strides[] = {
	//	viBuffer->GetVertexStride()
	//};
	//uint32_t offsets[] = {
	//	0
	//};
	//pContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
	//pContext->IASetIndexBuffer(viBuffer->GetIndexBuffer().Get(), viBuffer->GetIndexFormat(), 0);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	{
		const auto& rasterizer = CGameInstance::Get().GetResourceFirst<CResRasterizerState>(
			TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_RS_SOLID_NOCULL);
		pContext->RSSetState(rasterizer->GetRasterizerState().Get());
	}

	pContext->Draw(3, 0);

	return S_OK;
}

E::UPtr<CUICrosshair> CUICrosshair::Create()
{
	auto pInstance = E::ToUPtr(new CUICrosshair{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUICrosshair");
		return nullptr;
	}
	return  pInstance;
}

E::UPtr<E::CPrototype> CUICrosshair::Clone(void* pArg)
{
	auto	pInstance = E::ToUPtr(new CUICrosshair{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUICrosshair");
		return nullptr;
	}

	return pInstance;
}
