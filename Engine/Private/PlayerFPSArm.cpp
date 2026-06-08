#include "PlayerFPSArm.h"
#include "GameInstance.h"
#include "Resources.h"
#include "ComEntityModel.h"

NS_USING(Engine)

CPlayerFPSArm::CPlayerFPSArm()
{
}

CPlayerFPSArm::~CPlayerFPSArm()
{
}

HRESULT CPlayerFPSArm::Initialize(void* pArg)
{
	auto pDesc = static_cast<DESC*>(pArg);

	//m_viBufferID = pDesc->viBufferId;
	if (FAILED(CEntityObject::Initialize(pArg)))
	{
		return E_FAIL;
	}

	{
		CComEntityModel::DESC componentDesc{};
		componentDesc.pGameObject = this;
		componentDesc.viBufferId = { "MC_ENTITY_VIBuffer", "SteveArm" };
		componentDesc.geometryId = { "MC_ENTITY_GEOMETRY", "SteveArm" };
		auto pProto = CGameInstance::Get().ClonePrototype("PERMANENT", "Prototype_Component_EntityModel", &componentDesc);
		if (pProto == nullptr)
		{
			return E_FAIL;
		}
		m_pComEntityArmModel = AddComponent("Com_EntityArmModel", static_uptr_cast<CComEntityModel>(std::move(pProto)));
	}

	return S_OK;
}

void CPlayerFPSArm::PriorityUpdate(E::_float fTimeDelta)
{
}

void CPlayerFPSArm::Update(E::_float fTimeDelta)
{
}

void CPlayerFPSArm::LateUpdate(E::_float fTimeDelta)
{
	m_pComEntityArmModel->UpdateBoneMatrix(fTimeDelta);
	E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::NONBLEND, this);
	GetTransform().Update();
}

HRESULT CPlayerFPSArm::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
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

	m_pComEntityArmModel->BindBoneMatrix();

	m_pComEntityArmModel->Render(pContext, ctx);
	return S_OK;
}

UPtr<CPlayerFPSArm> CPlayerFPSArm::Create()
{
	auto pInstance = ToUPtr(new CPlayerFPSArm{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Create: CPlayerFPSArm");
		return nullptr;
	}

	return pInstance;
}

UPtr<CPrototype> CPlayerFPSArm::Clone(void* pArg)
{
	auto pInstance = ToUPtr(new CPlayerFPSArm{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned: CPlayerFPSArm");
		return nullptr;
	}

	return pInstance;
}
