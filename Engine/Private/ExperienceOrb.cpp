#include "ExperienceOrb.h"
#include "GameInstance.h"
#include "Resources.h"
#include "CameraObject.h"
#include "CollBox.h"
NS_USING(Engine)

CExperienceOrb::CExperienceOrb()
{
}

CExperienceOrb::~CExperienceOrb()
{
}

HRESULT CExperienceOrb::Initialize(void* pArg)
{
	if (FAILED(CEntityObject::Initialize(pArg)))
	{
		return E_FAIL;
	}

	if (auto res = CResDynamicBuffer::Create())
	{
		CResDynamicBuffer::DESC Desc{};
		Desc.desc = {
			.ByteWidth = ((uint32_t)sizeof(VTX_EXP_ORB_INSTANCED_DATA) * m_iNumElements),
			.Usage = D3D11_USAGE_DYNAMIC,
			.BindFlags = D3D11_BIND_VERTEX_BUFFER,
			.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
			.MiscFlags = 0,
			.StructureByteStride = 0,
		};

		if (FAILED(res->Load(Desc)))
		{
			return E_FAIL;
		}
		m_pResInstancedBuffer = res;
	}

	return S_OK;
}

void CExperienceOrb::PriorityUpdate(E::_float fTimeDelta)
{
}

void CExperienceOrb::Update(E::_float fTimeDelta)
{
	m_vecInstancedData.clear();

	for (auto& item : m_listOrbs)
	{
		_bool bGravity{ true };
		if (bGravity)    VelocityUpdate(item, fTimeDelta);
		//if (m_bAnimation)  AnimateTransformUpdate(item, fTimeDelta);

		// 최종 월드행렬
	   // _matrix matRot = XMMatrixRotationY(XMConvertToDegrees(item.fBobYRot));
		_matrix matRot = XMMatrixRotationY(item.fBobYRot);
		_matrix matBob = XMMatrixTranslation(0.f, item.fBobYOffset, 0.f);
		_matrix matWorld = XMMatrixTranslation(item.vPos.x, item.vPos.y, item.vPos.z);
		XMStoreFloat4x4(&item.matWorld, matRot * matBob * matWorld);

		item.boxCollider->Transform(matWorld);
		E::CGameInstance::Get().AddColliderGroup("Coll_DropItemObject", item.boxCollider.get());






		VTX_EXP_ORB_INSTANCED_DATA inst{};

		int32_t blockX = static_cast<int32_t>(std::floor(item.vPos.x));
		int32_t blockY = static_cast<int32_t>(std::floor(item.vPos.y));
		int32_t blockZ = static_cast<int32_t>(std::floor(item.vPos.z));
		if (auto optCurrBlock = E::CGameInstance::Get().GetVoxelBlock(blockX, blockY, blockZ))
		{
			inst.light = optCurrBlock->GetLight();
		}


		inst.matWorld = item.matWorld;
		//inst.texIndex = item.texIndexs.front();
		m_vecInstancedData.push_back(inst);

	}

	//static float fTemp = 0;
	//fTemp += fTimeDelta;

	//int frameIndex = int(fTemp / 0.1f);
	////int col = frameIndex % 4;
	////int row = frameIndex / 4;
	//m_iFrameCol = frameIndex % 4;
	//m_iFrameRow = frameIndex / 4;
}

void CExperienceOrb::LateUpdate(E::_float fTimeDelta)
{
	E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::NONBLEND, this);
	GetTransform().Update();

}

HRESULT CExperienceOrb::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
	//
	//{
	//	auto pResCBuf = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerQuadItemAnim");
	//	D3D11_MAPPED_SUBRESOURCE mappedSubResource;
	//	if (SUCCEEDED(pContext->Map(pResCBuf->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
	//	{
	//		E::CB_PER_QUADITEM_ANIM cbPerQuadItemAnim{};
	//		//cbPerQuadItemAnim.uvOffset =
	//		//int frameIndex = 0;
	//		//int col = frameIndex % 4;
	//		//int row = frameIndex / 4;

	//		float frameSize = 1.f / 4.f;  // 0.25f

	//		// cbuffer에 넘길 값
	//		cbPerQuadItemAnim.uvOffset = { m_iFrameCol * frameSize, m_iFrameRow * frameSize };
	//		cbPerQuadItemAnim.uvScale = { frameSize, frameSize };  // 0.25f, 0.25f
	//		memcpy(mappedSubResource.pData, &cbPerQuadItemAnim, sizeof(cbPerQuadItemAnim));
	//		pContext->Unmap(pResCBuf->GetCBuffer().Get(), 0);
	//	}
	//	pContext->VSSetConstantBuffers(5, 1, pResCBuf->GetCBuffer().GetAddressOf());
	//}
	//const auto& vs = E::CGameInstance::Get().GetResourceFirst<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_DropItem");
	//const auto& ps = E::CGameInstance::Get().GetResourceFirst<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_DropItem");
	//const auto& viBuffer = E::CGameInstance::Get().GetResourceFirst<E::CResQuadItemVIBuffer>("MC_ITEM_VIBuffer", "ExperienceOrb");

	//pContext->IASetInputLayout(vs->GetInputLayout().Get());
	//pContext->VSSetShader(vs->GetVertexShader().Get(), nullptr, 0);
	//pContext->PSSetShader(ps->GetPixelShader().Get(), nullptr, 0);

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
	//pContext->IASetPrimitiveTopology(viBuffer->GetPrimitiveType());

	//{
	//	auto pCbPerObject =
	//		E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(
	//			TAG_RES_GRP_PERMANENT_BUFFER,
	//			"CB_PerObject");

	//	D3D11_MAPPED_SUBRESOURCE mappedSubResource{};

	//	if (SUCCEEDED(pContext->Map(
	//		pCbPerObject->GetCBuffer().Get(),
	//		0,
	//		D3D11_MAP_WRITE_DISCARD,
	//		0,
	//		&mappedSubResource)))
	//	{
	//		E::CB_PER_OBJECT cbPerObject{};

	//		XMVECTOR vPos =
	//			GetTransform().GetLoadedPostion();

	//		_float3 scale =
	//			GetTransform().GetScale();

	//		auto pGameCam = CGameInstance::Get().GetActiveGameCamera();

	//		// 카메라 월드축 추출
	//		XMMATRIX matInvView = pGameCam->GetTransform().GetLoadedWorldMatrix();

	//		XMVECTOR vRight =
	//			XMVector3Normalize(matInvView.r[0]);

	//		XMVECTOR vUp =
	//			XMVector3Normalize(matInvView.r[1]);

	//		XMVECTOR vLook =
	//			XMVector3Normalize(matInvView.r[2]);

	//		// 스케일 적용
	//		vRight *= scale.x;
	//		vUp *= scale.y;
	//		vLook *= scale.z;

	//		// Billboard World
	//		XMMATRIX matWorld = XMMatrixIdentity();

	//		matWorld.r[0] = XMVectorSetW(vRight, 0.f);
	//		matWorld.r[1] = XMVectorSetW(vUp, 0.f);
	//		matWorld.r[2] = XMVectorSetW(vLook, 0.f);
	//		matWorld.r[3] = XMVectorSetW(vPos, 1.f);

	//		XMMATRIX matWVP =
	//			matWorld * ctx.matViewProj;

	//		XMStoreFloat4x4(
	//			&cbPerObject.matWorld,
	//			matWorld);

	//		XMStoreFloat4x4(
	//			&cbPerObject.matWVP,
	//			matWVP);

	//		memcpy(
	//			mappedSubResource.pData,
	//			&cbPerObject,
	//			sizeof(cbPerObject));

	//		pContext->Unmap(
	//			pCbPerObject->GetCBuffer().Get(),
	//			0);
	//	}

	//	pContext->VSSetConstantBuffers(
	//		0,
	//		1,
	//		pCbPerObject->GetCBuffer().GetAddressOf());

	//	pContext->PSSetConstantBuffers(
	//		0,
	//		1,
	//		pCbPerObject->GetCBuffer().GetAddressOf());
	//}

	//{
	//	const auto& sampler = E::CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP);
	//	pContext->PSSetSamplers(0, 1, sampler->GetSamplerState().GetAddressOf());
	//}

	//pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
	return S_OK;
}

void CExperienceOrb::AddOrb(const _float3& vPos, const _float3& vVelocity, uint32_t iType)
{
	InstancedExpOrbDesc Desc{};
	Desc.vPos = vPos;
	Desc.vVelocity = vVelocity;
	Desc.iType = iType;
	Desc.boxCollider = CCollBox::Create({ 0.f, 0.f, 0.f }, { 0.25f, 0.25f, 0.25f });
	Desc.boxCollider->SetInnerPointer(this);
	Desc.boxCollider->SetInnerHint("CDropBlock");
	m_listOrbs.push_back(Desc);
	CollHint hint{};
	hint.iter = std::prev(m_listOrbs.end());
	Desc.boxCollider->SetInnerHint2(&hint, sizeof(hint));
}

void CExperienceOrb::VelocityUpdate(InstancedExpOrbDesc& item, E::_float fTimeDelta)
{
	XMVECTOR vVel = XMLoadFloat3(&item.vVelocity);
	XMVECTOR vWishDir = XMVectorZero();

	// 가속
	float fCurrSpeed = XMVectorGetX(XMVector3Dot(XMVectorSetY(vVel, 0.f), vWishDir));
	float fAddSpeed = m_fSpeed - fCurrSpeed;
	if (fAddSpeed > 0.f)
	{
		float fAccelSpeed = std::min(18.f * m_fSpeed * fTimeDelta, fAddSpeed);
		vVel += vWishDir * fAccelSpeed;
	}

	// 수평 속도 제한
	XMVECTOR vHoriz = XMVectorSetY(vVel, 0.f);
	float fHorizSpeed = XMVectorGetX(XMVector3Length(vHoriz));
	if (fHorizSpeed > m_fSpeed)
	{
		vHoriz = XMVector3Normalize(vHoriz) * m_fSpeed;
		vVel = XMVectorSetY(vHoriz, XMVectorGetY(vVel));
	}

	// 마찰
	if (item.bOnGround)
	{
		float fSpeed = XMVectorGetX(XMVector3Length(XMVectorSetY(vVel, 0.f)));
		if (fSpeed > 0.f)
		{
			float fNewSpeed = std::max(fSpeed - fSpeed * 15.f * fTimeDelta, 0.f);
			float vy = XMVectorGetY(vVel);
			vVel = XMVectorSetY(vVel * (fNewSpeed / fSpeed), vy);
		}
	}

	// 중력
	if (!item.bOnGround)
		vVel = XMVectorSetY(vVel, XMVectorGetY(vVel) - 20.f * fTimeDelta);

	// AABB 충돌
	const XMFLOAT3 halfExtents = { 0.25f, 0.25f, 0.25f };
	XMFLOAT3 c = item.vPos;

	// Y
	float velY = XMVectorGetY(vVel);
	float prevY = c.y;
	c.y += velY * fTimeDelta;
	if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
	{
		if (velY < 0.f)
		{
			c.y = floorf(c.y - halfExtents.y) + 1.f + halfExtents.y + 0.001f;
			item.bOnGround = true;
		}
		else c.y = prevY;
		vVel = XMVectorSetY(vVel, 0.f);
	}
	else item.bOnGround = false;

	// X
	float px = c.x;
	c.x += XMVectorGetX(vVel) * fTimeDelta;
	if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
	{
		c.x = px;
		vVel = XMVectorSetX(vVel, 0.f);
	}

	// Z
	float pz = c.z;
	c.z += XMVectorGetZ(vVel) * fTimeDelta;
	if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
	{
		c.z = pz;
		vVel = XMVectorSetZ(vVel, 0.f);
	}

	item.vPos = c;
	XMStoreFloat3(&item.vVelocity, vVel);
}

UPtr<CExperienceOrb> CExperienceOrb::Create()
{
	auto pInstance = ToUPtr(new CExperienceOrb{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Create: CExperienceOrb");
		return nullptr;
	}

	return pInstance;
}

UPtr<CPrototype> CExperienceOrb::Clone(void* pArg)
{
	auto pInstance = ToUPtr(new CExperienceOrb{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned: CExperienceOrb");
		return nullptr;
	}

	return pInstance;
}
