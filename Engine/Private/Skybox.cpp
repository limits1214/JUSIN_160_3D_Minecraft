#include "Skybox.h"
#include "GameInstance.h"
#include "Resources.h"
NS_USING(Engine)

CSkybox::CSkybox()
{
}

CSkybox::~CSkybox()
{
}

HRESULT CSkybox::Initialize(void* pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
	{
		return E_FAIL;
	}
    return S_OK;
}

void CSkybox::PriorityUpdate(E::_float fTimeDelta)
{
}

void CSkybox::Update(E::_float fTimeDelta)
{
}

void CSkybox::LateUpdate(E::_float fTimeDelta)
{
	CGameInstance::Get().AddRenderObject(RENDERGROUP::SKYBOX, this);
}

HRESULT CSkybox::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
	const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Skybox");
	const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Skybox");

	pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
	pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);
	
	auto ds = CGameInstance::Get().GetResourceFirst<CResDepthStencilState>(TAG_RES_GRP_PERMANENT_STATE, "DS_Skybox");

	pContext->OMSetDepthStencilState(ds->GetDepthStencilState().Get(), 0);

	UINT stride = 0;
	UINT offset = 0;
	ID3D11Buffer* nullBuffer = nullptr;
	pContext->IASetVertexBuffers(0, 1, &nullBuffer, &stride, &offset);
	pContext->IASetInputLayout(nullptr);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_RS_SOLID_NOCULL

	auto rs = CGameInstance::Get().GetResourceFirst<CResRasterizerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_RS_SOLID_NOCULL);
	pContext->RSSetState(rs->GetRasterizerState().Get());

	{
		const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP_NOMIP);
		pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
	}


	// 셰이더 상수 버퍼 업데이트 후...
	// 5. 정점 버퍼 없이 딱 3개의 정점만 그리라고 명령합니다. (셰이더가 사각형을 만들어냄)
	pContext->Draw(3, 0);
	pContext->OMSetDepthStencilState(nullptr, 0);
    return S_OK;
}

UPtr<CSkybox> CSkybox::Create()
{
	auto pInstance = ToUPtr(new CSkybox{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Create: CSkybox");
		return nullptr;
	}

	return pInstance;
}

UPtr<CPrototype> CSkybox::Clone(void* pArg)
{
	auto pInstance = ToUPtr(new CSkybox{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned: CSkybox");
		return nullptr;
	}

	return pInstance;
}
