#include "ExperienceOrbItem.h"
#include "GameInstance.h"
#include "Resources.h"
#include "CameraObject.h"
NS_USING(Engine)

CExperienceOrbItem::CExperienceOrbItem()
{
}

CExperienceOrbItem::~CExperienceOrbItem()
{
}

HRESULT CExperienceOrbItem::Initialize(void* pArg)
{
	if (FAILED(CItemObject::Initialize(pArg)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CExperienceOrbItem::PriorityUpdate(E::_float fTimeDelta)
{
}

void CExperienceOrbItem::Update(E::_float fTimeDelta)
{
	static float fTemp = 0;
	fTemp += fTimeDelta;

	int frameIndex = fTemp / 0.1;
	//int col = frameIndex % 4;
	//int row = frameIndex / 4;
	m_iFrameCol = frameIndex % 4;
	m_iFrameRow = frameIndex / 4;
}

void CExperienceOrbItem::LateUpdate(E::_float fTimeDelta)
{
	E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::NONBLEND, this);
	GetTransform().Update();

}

HRESULT CExperienceOrbItem::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
	
	{
		auto pResCBuf = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerQuadItemAnim");
		D3D11_MAPPED_SUBRESOURCE mappedSubResource;
		if (SUCCEEDED(pContext->Map(pResCBuf->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
		{
			E::CB_PER_QUADITEM_ANIM cbPerQuadItemAnim{};
			//cbPerQuadItemAnim.uvOffset =
			//int frameIndex = 0;
			//int col = frameIndex % 4;
			//int row = frameIndex / 4;

			float frameSize = 1.f / 4.f;  // 0.25f

			// cbuffer에 넘길 값
			cbPerQuadItemAnim.uvOffset = { m_iFrameCol * frameSize, m_iFrameRow * frameSize };
			cbPerQuadItemAnim.uvScale = { frameSize, frameSize };  // 0.25f, 0.25f
			memcpy(mappedSubResource.pData, &cbPerQuadItemAnim, sizeof(cbPerQuadItemAnim));
			pContext->Unmap(pResCBuf->GetCBuffer().Get(), 0);
		}
		pContext->VSSetConstantBuffers(5, 1, pResCBuf->GetCBuffer().GetAddressOf());
	}
	const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Item");
	const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Item");
	const auto& viBuffer = E::CGameInstance::Get().GetResourceFirst<E::CResQuadItemVIBuffer>("MC_ITEM_VIBuffer", "ExperienceOrb");

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
		auto pCbPerObject =
			E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(
				TAG_RES_GRP_PERMANENT_BUFFER,
				"CB_PerObject");

		D3D11_MAPPED_SUBRESOURCE mappedSubResource{};

		if (SUCCEEDED(pContext->Map(
			pCbPerObject->GetCBuffer().Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedSubResource)))
		{
			E::CB_PER_OBJECT cbPerObject{};

			XMVECTOR vPos =
				GetTransform().GetLoadedPostion();

			_float3 scale =
				GetTransform().GetScale();

			auto pGameCam = CGameInstance::Get().GetCameraObject("GAME");

			// 카메라 월드축 추출
			XMMATRIX matInvView = pGameCam->GetTransform().GetLoadedWorldMatrix();

			XMVECTOR vRight =
				XMVector3Normalize(matInvView.r[0]);

			XMVECTOR vUp =
				XMVector3Normalize(matInvView.r[1]);

			XMVECTOR vLook =
				XMVector3Normalize(matInvView.r[2]);

			// 스케일 적용
			vRight *= scale.x;
			vUp *= scale.y;
			vLook *= scale.z;

			// Billboard World
			XMMATRIX matWorld = XMMatrixIdentity();

			matWorld.r[0] = XMVectorSetW(vRight, 0.f);
			matWorld.r[1] = XMVectorSetW(vUp, 0.f);
			matWorld.r[2] = XMVectorSetW(vLook, 0.f);
			matWorld.r[3] = XMVectorSetW(vPos, 1.f);

			XMMATRIX matWVP =
				matWorld * ctx.matViewProj;

			XMStoreFloat4x4(
				&cbPerObject.matWorld,
				matWorld);

			XMStoreFloat4x4(
				&cbPerObject.matWVP,
				matWVP);

			memcpy(
				mappedSubResource.pData,
				&cbPerObject,
				sizeof(cbPerObject));

			pContext->Unmap(
				pCbPerObject->GetCBuffer().Get(),
				0);
		}

		pContext->VSSetConstantBuffers(
			0,
			1,
			pCbPerObject->GetCBuffer().GetAddressOf());

		pContext->PSSetConstantBuffers(
			0,
			1,
			pCbPerObject->GetCBuffer().GetAddressOf());
	}

	{
		const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP);
		pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
	}

	pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
	return S_OK;
}

UPtr<CExperienceOrbItem> CExperienceOrbItem::Create()
{
	auto pInstance = ToUPtr(new CExperienceOrbItem{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Create: CExperienceOrbItem");
		return nullptr;
	}

	return pInstance;
}

UPtr<CPrototype> CExperienceOrbItem::Clone(void* pArg)
{
	auto pInstance = ToUPtr(new CExperienceOrbItem{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned: CExperienceOrbItem");
		return nullptr;
	}

	return pInstance;
}
