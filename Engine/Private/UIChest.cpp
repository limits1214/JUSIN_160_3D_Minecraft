#include "pch.h"
#include "UIChest.h"
#include "GameInstance.h"
#include "CameraObject.h"
#include "Resources.h"
#include "ComConstantBuffer.h"
#include "UIItem.h"
#include "UIController.h"
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

	InitializeSlot();
	return S_OK;
}

void CUIChest::PriorityUpdate(E::_float fTimeDelta)
{
}

void CUIChest::Update(E::_float fTimeDelta)
{
	if (!m_bRender)return;

	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(CGameInstance::Get().GetHwnd(), &mousePos);

	_bool bIntersected{ false };
	for (const auto& slot : m_vecSlot)
	{
		auto itemOrigin = _float2{ (float)mousePos.x, (float)mousePos.y };
		auto slotOrigin = slot.vOriginPos;

		auto slotMinX = slotOrigin.x - 8 * MC_UI_SCALE;
		auto slotMinY = slotOrigin.y - 8 * MC_UI_SCALE;
		auto slotMaxX = slotOrigin.x + 8 * MC_UI_SCALE;
		auto slotMaxY = slotOrigin.y + 8 * MC_UI_SCALE;


		if (itemOrigin.x > slotMinX
			&& itemOrigin.x < slotMaxX
			&& itemOrigin.y > slotMinY
			&& itemOrigin.y < slotMaxY)
		{
			bIntersected = true;
			if (auto pItem = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(slot.hItem))
			{
				if (auto pInfo = pItem->GetItemInfoPtr())
				{
					if (auto pUIController = CGameInstance::Get().GetGameObjectByHandleT<CUIController>(m_hUIController))
					{

						pUIController->GetTextBg()->SetRender(true);
						pUIController->GetTextBg()->SetText(CItemObject::GetItemName(*pInfo));
					}
				}
				else
				{
					if (auto pUIController = CGameInstance::Get().GetGameObjectByHandleT<CUIController>(m_hUIController))
					{
						pUIController->GetTextBg()->SetRender(false);
					}
				}

			}
		}
	}

	if (!bIntersected)
	{
		if (auto pUIController = CGameInstance::Get().GetGameObjectByHandleT<CUIController>(m_hUIController))
		{
			pUIController->GetTextBg()->SetRender(false);
		}
	}
}

void CUIChest::LateUpdate(E::_float fTimeDelta)
{
	if (m_bRender)
		E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::UI, this);
	for (auto& slot : m_vecSlot)
	{
		if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(slot.hItem))
		{
			pObj->SetRender(m_bRender);
		}
	}
	GetTransform().Update();
}


void CUIChest::InitializeSlot()
{
	size_t typeIdx{};

	for (uint32_t i = 0; i < 3; ++i)
	{
		for (uint32_t j = 0; j < 9; ++j)
		{
			m_ChestInventoryIdxs[9 * i + j] = m_vecSlot.size();

			auto posX = 16.f + (j * 18.f);
			auto posY = 26.f + (i * 18.f);

			ChestSlot InvenSlot{};
			InvenSlot.eType = SlotType::CHEST_INVENTORY;
			InvenSlot.vOriginPos.x = (m_fX - m_fSizeX * 0.5f) + posX * MC_UI_SCALE;
			InvenSlot.vOriginPos.y = (m_fY - m_fSizeY * 0.5f) + posY * MC_UI_SCALE;
			{
				E::CUIItem::DESC Desc{};
				Desc.fX = InvenSlot.vOriginPos.x;
				Desc.fY = InvenSlot.vOriginPos.y;
				Desc.sObjectTag = "UIItem";
				if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIItem",
					"80_UI", &Desc))
				{
					InvenSlot.hItem = handle.value();
				}
			}
			InvenSlot.typeIdx = typeIdx++;
			m_vecSlot.push_back(InvenSlot);
		}
	}
	typeIdx = 0;


	for (uint32_t i = 0; i < 3; ++i)
	{
		for (uint32_t j = 0; j < 9; ++j)
		{
			m_InventoryIdxs[9 * i + j] = m_vecSlot.size();

			auto posX = 16.f + (j * 18.f);
			auto posY = 92.f + (i * 18.f);

			ChestSlot InvenSlot{};
			InvenSlot.eType = SlotType::INVENTORY;
			InvenSlot.vOriginPos.x = (m_fX - m_fSizeX * 0.5f) + posX * MC_UI_SCALE;
			InvenSlot.vOriginPos.y = (m_fY - m_fSizeY * 0.5f) + posY * MC_UI_SCALE;
			{
				E::CUIItem::DESC Desc{};
				Desc.fX = InvenSlot.vOriginPos.x;
				Desc.fY = InvenSlot.vOriginPos.y;
				Desc.sObjectTag = "UIItem";
				if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIItem",
					"80_UI", &Desc))
				{
					InvenSlot.hItem = handle.value();
				}
			}
			InvenSlot.typeIdx = typeIdx++;
			m_vecSlot.push_back(InvenSlot);
		}
	}
	typeIdx = 0;


	for (uint32_t i = 0; i < 9; ++i)
	{
		m_InventoryHotbarIdxs[i] = m_vecSlot.size();
		auto posX = 16.f + (i * 18.f);
		auto posY = 150.f;

		ChestSlot hotbarSlot{};
		hotbarSlot.eType = SlotType::INVENTORY_HOTBAR;
		hotbarSlot.vOriginPos.x = (m_fX - m_fSizeX * 0.5f) + posX * MC_UI_SCALE;
		hotbarSlot.vOriginPos.y = (m_fY - m_fSizeY * 0.5f) + posY * MC_UI_SCALE;
		{
			E::CUIItem::DESC Desc{};
			Desc.fX = hotbarSlot.vOriginPos.x;
			Desc.fY = hotbarSlot.vOriginPos.y;
			Desc.sObjectTag = "UIItem";
			if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIItem",
				"80_UI", &Desc))
			{
				hotbarSlot.hItem = handle.value();
			}
		}
		hotbarSlot.typeIdx = typeIdx++;
		m_vecSlot.push_back(hotbarSlot);
	}
	typeIdx = 0;
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
		//pContext->GSSetConstantBuffers
		//pContext->HS
		//pContext->DS
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


void CUIChest::SetInventoryHotbarItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size)
{
	for (uint32_t i = 0; i < size; ++i)
	{
		if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(m_vecSlot[m_InventoryHotbarIdxs[i]].hItem))
		{
			if (pArr[i])
			{
				pObj->SetItemInfo(pArr[i]);
			}
			else
			{
				pObj->SetItemInfo(std::nullopt);
			}
		}
	}
}

void CUIChest::SetInventoryItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size)
{
	for (uint32_t i = 0; i < size; ++i)
	{
		if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(m_vecSlot[m_InventoryIdxs[i]].hItem))
		{
			if (pArr[i])
			{
				pObj->SetItemInfo(pArr[i]);
			}
			else
			{
				pObj->SetItemInfo(std::nullopt);
			}
		}
	}
}

void CUIChest::SetChestInventoryItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size)
{
	for (uint32_t i = 0; i < size; ++i)
	{
		if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(m_vecSlot[m_ChestInventoryIdxs[i]].hItem))
		{
			if (pArr[i])
			{
				pObj->SetItemInfo(pArr[i]);
			}
			else
			{
				pObj->SetItemInfo(std::nullopt);
			}
		}
	}
}