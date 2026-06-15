#include "pch.h"
#include "UIItem.h"
#include "GameInstance.h"
#include "CameraObject.h"
#include "Resources.h"
#include "ComConstantBuffer.h"
#include "ComTransform.h"

NS_USING(Engine)

CUIItem::CUIItem()
{
}


CUIItem::~CUIItem()
{
}

void CUIItem::UpdateGUI()
{
	CUIObject::UpdateGUI();

	if (ImGui::Button("RenderToggle"))
	{
		m_bRender = !m_bRender;
	}
	
	if (m_ItemInfo)
	{
		if (ImGui::DragFloat("Durability", &m_ItemInfo->fDurability, 0.01f, 0.f, 1.f))
		{

		}
	}
	
}

HRESULT CUIItem::Initialize(void* pArg)
{
	auto pDesc = static_cast<DESC*>(pArg);
	pDesc->fSizeX = 16.f * MC_UI_SCALE;
	pDesc->fSizeY = 16.f * MC_UI_SCALE;

	//m_eType = pDesc->eType;
	//m_ItemInfo = pDesc->itemInfo;
	//m_PerUI = GetPerUIByType(m_eType);

	//pDesc->fX = (1280.f * 0.5f);
	//pDesc->fY = (720.f * 0.5f);


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

	GetTransform().AddPosition(XMVectorSet(0.f, 0.f, -0.01f, 0.f));

	{
		if (FAILED(AddComponentFromProto("PERMANENT", "Prototype_Component_Transform", "Com_DurabilityBgTransform", nullptr, &m_pComDurabilityBgTransform)))
		{
			return E_FAIL;
		}
		auto tmp = GetTransform().GetPosition();
		tmp.z -= 0.01f;
		m_pComDurabilityBgTransform->SetPosition(tmp);
		auto x = 16.f * MC_UI_SCALE;
		auto y = 1.f * MC_UI_SCALE;

		m_pComDurabilityBgTransform->SetScale(_float3{x,y, 1.f});
	}
	{
		if (FAILED(AddComponentFromProto("PERMANENT", "Prototype_Component_Transform", "Com_DurabilityGageTransform", nullptr, &m_pComDurabilityGageTransform)))
		{
			return E_FAIL;
		}
		auto tmp = GetTransform().GetPosition();
		tmp.z -= 0.02f;

		m_pComDurabilityGageTransform->SetPosition(tmp);
		auto x = 16.f * MC_UI_SCALE * 1.f;
		auto y = 1.f * MC_UI_SCALE;
		m_pComDurabilityGageTransform->SetScale(_float3{ x,y, 1.f });
	}

	//if (m_ItemInfo)
	//{
	//	if (m_ItemInfo->block)
	//	{
	//		m_PerUI = GetPerUIByType(m_ItemInfo->block.value().GetType());
	//	}
	//	else
	//	{
	//		m_PerUI = GetPerUIByType(m_ItemInfo->eItemType);
	//	}
	//}
	
	

	return S_OK;
}

void CUIItem::PriorityUpdate(E::_float fTimeDelta)
{
}

void CUIItem::Update(E::_float fTimeDelta)
{
	//if (m_bOnCursor)
	//{
	//	POINT mousePos;
	//	GetCursorPos(&mousePos);
	//	ScreenToClient(CGameInstance::Get().GetHwnd(), &mousePos);

	//	m_fX = mousePos.x;
	//	m_fY = mousePos.y;
	//	CalcUICoord();
	//}
}

void CUIItem::LateUpdate(E::_float fTimeDelta)
{
	if (!m_ItemInfo)return;

	if (m_bRender)
	{
		E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::UI, this);
	}

	GetTransform().Update();

	auto itemPos = GetTransform().GetPosition();
	itemPos.z -= 0.01f;
	m_pComDurabilityBgTransform->SetPosition(itemPos);
	m_pComDurabilityBgTransform->Update();

	//itemPos.z -= 0.01f;
	//m_pComDurabilityGageTransform->SetPosition(itemPos);

	auto gageScale = m_pComDurabilityBgTransform->GetScale();
	gageScale.x = 16.f * MC_UI_SCALE * m_ItemInfo->fDurability;
	m_pComDurabilityGageTransform->SetScale(gageScale);

	float fullWidth = 16.f * MC_UI_SCALE;
	gageScale.x = fullWidth * m_ItemInfo->fDurability;
	m_pComDurabilityGageTransform->SetScale(gageScale);

	float offsetX = (fullWidth - gageScale.x) * 0.5f;
	auto gagePos = itemPos;
	gagePos.x -= offsetX;
	gagePos.z -= 0.01f;
	m_pComDurabilityGageTransform->SetPosition(gagePos);

	m_pComDurabilityGageTransform->Update();
}

HRESULT CUIItem::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
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
		{
			if (FAILED(m_pComCBufferPerUI->MapDiscard(pContext, &m_PerUI, sizeof(m_PerUI))))
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
	}

	// cnt
	{
		if (m_ItemInfo)
		{
			if (m_ItemInfo->iCnt >= 1)
			{
				E::CGameInstance::Get().FontAddLateDraw(RENDERGROUP::UI, "NeoDGM_20px", std::to_wstring(m_ItemInfo->iCnt), { m_fX , m_fY });
			}
		}
	}

	// durability
	{
		if(m_ItemInfo)
		{
			if (m_ItemInfo->fDurability < 1.f)
			{
				{
					{
						E::CB_PER_UI perUI{};
						perUI.texIndex = PackTexId(12, 0);
						perUI.texCoord = { 1.f / 256.f, 144.f / 256.f };
						perUI.uvSize = { 1.f / 256.f, 1.f / 256.f };

						if (FAILED(m_pComCBufferPerUI->MapDiscard(pContext, &perUI, sizeof(perUI))))
						{
							return E_FAIL;
						}
						pContext->VSSetConstantBuffers(7, 1, m_pComCBufferPerUI->GetAdressOfBuffer());
						pContext->PSSetConstantBuffers(7, 1, m_pComCBufferPerUI->GetAdressOfBuffer());
					}
					{
						E::CB_PER_OBJECT cbPerObject{};
						cbPerObject.matWorld = *m_pComDurabilityBgTransform->GetCombinedWorldMatrix();
						XMStoreFloat4x4(&cbPerObject.matWVP, m_pComDurabilityBgTransform->GetLoadedCombinedWorldMatrix() * ctx.matViewProj);
						if (FAILED(m_pComCBufferPerObject->MapDiscard(pContext, &cbPerObject, sizeof(cbPerObject))))
						{
							return E_FAIL;
						}
						pContext->VSSetConstantBuffers(0, 1, m_pComCBufferPerObject->GetAdressOfBuffer());
						pContext->PSSetConstantBuffers(0, 1, m_pComCBufferPerObject->GetAdressOfBuffer());
					}

					pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
				}

				{
					{
						E::CB_PER_UI perUI{};
						perUI.texIndex = PackTexId(12, 0);
						perUI.texCoord = { 0.f / 256.f, 144.f / 256.f };
						perUI.uvSize = { 1.f / 256.f, 1.f / 256.f };

						if (FAILED(m_pComCBufferPerUI->MapDiscard(pContext, &perUI, sizeof(perUI))))
						{
							return E_FAIL;
						}
						pContext->VSSetConstantBuffers(7, 1, m_pComCBufferPerUI->GetAdressOfBuffer());
						pContext->PSSetConstantBuffers(7, 1, m_pComCBufferPerUI->GetAdressOfBuffer());
					}
					{
						E::CB_PER_OBJECT cbPerObject{};
						cbPerObject.matWorld = *m_pComDurabilityGageTransform->GetCombinedWorldMatrix();
						XMStoreFloat4x4(&cbPerObject.matWVP, m_pComDurabilityGageTransform->GetLoadedCombinedWorldMatrix() * ctx.matViewProj);
						if (FAILED(m_pComCBufferPerObject->MapDiscard(pContext, &cbPerObject, sizeof(cbPerObject))))
						{
							return E_FAIL;
						}
						pContext->VSSetConstantBuffers(0, 1, m_pComCBufferPerObject->GetAdressOfBuffer());
						pContext->PSSetConstantBuffers(0, 1, m_pComCBufferPerObject->GetAdressOfBuffer());
					}

					pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
				}
			}
		}
		
		

	}
	return S_OK;
}

E::UPtr<CUIItem> CUIItem::Create()
{
	auto pInstance = E::ToUPtr(new CUIItem{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUICraftingTable");
		return nullptr;
	}
	return  pInstance;
}

E::UPtr<E::CPrototype> CUIItem::Clone(void* pArg)
{
	auto	pInstance = E::ToUPtr(new CUIItem{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIItem");
		return nullptr;
	}

	return pInstance;
}
