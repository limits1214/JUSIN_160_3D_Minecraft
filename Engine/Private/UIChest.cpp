#include "pch.h"
#include "UIChest.h"
#include "GameInstance.h"
#include "CameraObject.h"
#include "Resources.h"
#include "ComConstantBuffer.h"

NS_USING(Engine)

CUIChest::CUIChest()
{
}


CUIChest::~CUIChest()
{
}

void CUIChest::UpdateGUI()
{
	CUIObject::UpdateGUI();

	if (ImGui::Button("RenderToggle"))
	{
		m_bRender = !m_bRender;
	}
}

HRESULT CUIChest::Initialize(void* pArg)
{
	auto pDesc = static_cast<CUIObject::UIOBJECT_DESC*>(pArg);
	pDesc->fSizeX = 176.f * MC_UI_SCALE;
	pDesc->fSizeY = 166.f * MC_UI_SCALE;

	pDesc->fX = (1280.f * 0.5f);
	pDesc->fY = (720.f * 0.5f);

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

	//GetTransform().AddPosition(XMVectorSet(0.f, 0.f, -0.01f, 0.f));

	{
		//if (FAILED(AddComponentFromProto("PERMANENT", "Prototype_Component_Transform", "Com_OverlayTransform", nullptr, &m_pComOverlayTransform)))
		//{
		//	return E_FAIL;
		//}
		////m_pComOverlayTransform
		//auto tmp = GetTransform().GetPosition();
		//tmp.z -= 0.01f;
		//m_pComOverlayTransform->SetPosition(tmp);
		//m_pComOverlayTransform->SetScale(GetTransform().GetScale());
	}

	return S_OK;
}

void CUIChest::PriorityUpdate(E::_float fTimeDelta)
{
}

void CUIChest::Update(E::_float fTimeDelta)
{
}

void CUIChest::LateUpdate(E::_float fTimeDelta)
{
	if (m_bRender)
	{
		E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::UI, this);
	}

	GetTransform().Update();
}

HRESULT CUIChest::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
	const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_UI");
	const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_UI");

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
		perUI.texIndex = PackTexId(12, 6);
		perUI.texCoord = { 0 / 256.f, 0 / 256.f };
		perUI.uvSize = { 176.f / 256.f, 166.f / 256.f };
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


	return S_OK;
}

E::UPtr<CUIChest> CUIChest::Create()
{
	auto pInstance = E::ToUPtr(new CUIChest{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUIChest");
		return nullptr;
	}
	return  pInstance;
}

E::UPtr<E::CPrototype> CUIChest::Clone(void* pArg)
{
	auto	pInstance = E::ToUPtr(new CUIChest{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIChest");
		return nullptr;
	}

	return pInstance;
}
