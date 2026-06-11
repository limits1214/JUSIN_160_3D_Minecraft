#include "pch.h"
#include "UIBlastFurnace.h"
#include "GameInstance.h"
#include "CameraObject.h"
#include "Resources.h"
#include "ComConstantBuffer.h"

#include "UIItem.h"
#include "UIController.h"

NS_USING(Engine)

CUIBlastFurnace::CUIBlastFurnace()
{
}


CUIBlastFurnace::~CUIBlastFurnace()
{
}

void CUIBlastFurnace::UpdateGUI()
{
	CUIObject::UpdateGUI();

	if (ImGui::Button("RenderToggle"))
	{
		m_bRender = !m_bRender;
	}

	if (ImGui::DragFloat("Progress", &m_fProcess, 0.1f, 0.f, 1.f))
	{

	}

	if (ImGui::DragFloat("Fuel", &m_fFuel, 0.1f, 0.f, 1.f))
	{

	}
}

HRESULT CUIBlastFurnace::Initialize(void* pArg)
{
	auto pDesc = static_cast<CUIObject::UIOBJECT_DESC*>(pArg);
	pDesc->fSizeX = 176.f * MC_UI_SCALE;
	pDesc->fSizeY = 166.f * MC_UI_SCALE;

	pDesc->fX = (1280.f * 0.5f);
	pDesc->fY = (720.f * 0.5f);

	if (FAILED(CUIObject::Initialize(pArg)))
		return E_FAIL;
	CComTransform::DESC ProgressTransformDese{};
	if (FAILED(AddComponentFromProto("PERMANENT", "Prototype_Component_Transform", "ComProgressTransform", &ProgressTransformDese, &m_pComProgressTransform)))
	{
		return E_FAIL;
	};

	if (FAILED(AddComponentFromProto("PERMANENT", "Prototype_Component_Transform", "ComFuelTransform", &ProgressTransformDese, &m_pComFuelTransform)))
	{
		return E_FAIL;
	};

	auto itemPos = GetTransform().GetPosition();
	itemPos.z -= 0.01f;
	//itemPos.x += 4.f;
	//itemPos.y += 80.f;
	m_pComFuelTransform->SetPosition(itemPos);
	m_pComFuelTransform->SetScale(_float3{ 14.f * MC_UI_SCALE, 14.f * MC_UI_SCALE, 1.f });
	m_pComFuelTransform->Update();

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

void CUIBlastFurnace::PriorityUpdate(E::_float fTimeDelta)
{
}

void CUIBlastFurnace::Update(E::_float fTimeDelta)
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

void CUIBlastFurnace::LateUpdate(E::_float fTimeDelta)
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

	{
		float fullWidth = 24.f * MC_UI_SCALE;
		float scaledWidth = fullWidth * m_fProcess;

		auto basePos = GetTransform().GetPosition();
		basePos.x += -fullWidth * 0.5f + scaledWidth * 0.5f;
		basePos.z -= 0.01f;

		basePos.x += 4.f;
		basePos.y += 80.f;

		m_pComProgressTransform->SetPosition(basePos);
		m_pComProgressTransform->SetScale(_float3{ scaledWidth, 16.f * MC_UI_SCALE, 1.f });
		m_pComProgressTransform->Update();
	}

	{
		float fullHeight = 14.f * MC_UI_SCALE;
		float scaledHeight = fullHeight * m_fFuel;  
		auto basePos = GetTransform().GetPosition();
		basePos.y += -fullHeight * 0.5f + scaledHeight * 0.5f;  // 아래쪽 끝 고정
		basePos.z -= 0.01f;

		basePos.x -= 48.f;
		basePos.y += 78.f;
		m_pComFuelTransform->SetPosition(basePos);
		m_pComFuelTransform->SetScale(_float3{ 14.f * MC_UI_SCALE, scaledHeight, 1.f });  // scaledHeight
		m_pComFuelTransform->Update();
	}
}

void CUIBlastFurnace::InitializeSlot()
{
	size_t typeIdx{};



	{
		m_IngredientIdx = m_vecSlot.size();
		auto posX = 64.f;
		auto posY = 25.f;
		FurnaceSlot ingredient{};
		ingredient.eType = SlotType::INGREDIENT;
		ingredient.vOriginPos.x = (m_fX - m_fSizeX * 0.5f) + posX * MC_UI_SCALE;
		ingredient.vOriginPos.y = (m_fY - m_fSizeY * 0.5f) + posY * MC_UI_SCALE;
		{
			E::CUIItem::DESC Desc{};
			Desc.fX = ingredient.vOriginPos.x;
			Desc.fY = ingredient.vOriginPos.y;
			Desc.sObjectTag = "UIItem";
			if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIItem",
				"80_UI", &Desc))
			{
				ingredient.hItem = handle.value();
			}
		}
		ingredient.typeIdx = typeIdx++;
		m_vecSlot.push_back(ingredient);
	}
	typeIdx = 0;

	{
		m_FuelIdx = m_vecSlot.size();
		auto posX = 64.f;
		auto posY = 61.f;
		FurnaceSlot fuel{};
		fuel.eType = SlotType::FUEL;
		fuel.vOriginPos.x = (m_fX - m_fSizeX * 0.5f) + posX * MC_UI_SCALE;
		fuel.vOriginPos.y = (m_fY - m_fSizeY * 0.5f) + posY * MC_UI_SCALE;
		{
			E::CUIItem::DESC Desc{};
			Desc.fX = fuel.vOriginPos.x;
			Desc.fY = fuel.vOriginPos.y;
			Desc.sObjectTag = "UIItem";
			if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIItem",
				"80_UI", &Desc))
			{
				fuel.hItem = handle.value();
			}
		}
		fuel.typeIdx = typeIdx++;
		m_vecSlot.push_back(fuel);
	}
	typeIdx = 0;

	{
		m_ResultIdx = m_vecSlot.size();
		auto posX = 124.f;
		auto posY = 43.f;
		FurnaceSlot result{};
		result.eType = SlotType::RESULT;
		result.vOriginPos.x = (m_fX - m_fSizeX * 0.5f) + posX * MC_UI_SCALE;
		result.vOriginPos.y = (m_fY - m_fSizeY * 0.5f) + posY * MC_UI_SCALE;
		{
			E::CUIItem::DESC Desc{};
			Desc.fX = result.vOriginPos.x;
			Desc.fY = result.vOriginPos.y;
			Desc.sObjectTag = "UIItem";
			if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIItem",
				"80_UI", &Desc))
			{
				result.hItem = handle.value();
			}
		}
		result.typeIdx = typeIdx++;
		m_vecSlot.push_back(result);
	}
	typeIdx = 0;

	for (uint32_t i = 0; i < 3; ++i)
	{
		for (uint32_t j = 0; j < 9; ++j)
		{
			m_InventoryIdxs[9 * i + j] = m_vecSlot.size();

			auto posX = 16.f + (j * 18.f);
			auto posY = 92.f + (i * 18.f);

			FurnaceSlot InvenSlot{};
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

		FurnaceSlot hotbarSlot{};
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

HRESULT CUIBlastFurnace::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
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
		perUI.texIndex = PackTexId(12, 4);
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


	if (m_fProcess > 0.f)
	{
		{
			{
				E::CB_PER_UI perUI{};
				perUI.texIndex = PackTexId(12, 0);
				perUI.texCoord = { 48.f / 256.f, 112.f / 256.f };
				perUI.uvSize = { 24.f / 256.f * m_fProcess, 16.f / 256.f };
				if (FAILED(m_pComCBufferPerUI->MapDiscard(pContext, &perUI, sizeof(perUI))))
				{
					return E_FAIL;
				}
				pContext->VSSetConstantBuffers(7, 1, m_pComCBufferPerUI->GetAdressOfBuffer());
				pContext->PSSetConstantBuffers(7, 1, m_pComCBufferPerUI->GetAdressOfBuffer());
			}
			{
				E::CB_PER_OBJECT cbPerObject{};
				cbPerObject.matWorld = *m_pComProgressTransform->GetCombinedWorldMatrix();
				XMStoreFloat4x4(&cbPerObject.matWVP, m_pComProgressTransform->GetLoadedCombinedWorldMatrix() * ctx.matViewProj);
				if (FAILED(m_pComCBufferPerObject->MapDiscard(pContext, &cbPerObject, sizeof(cbPerObject))))
				{
					return E_FAIL;
				}
				pContext->VSSetConstantBuffers(0, 1, m_pComCBufferPerObject->GetAdressOfBuffer());
				pContext->PSSetConstantBuffers(0, 1, m_pComCBufferPerObject->GetAdressOfBuffer());
			}
		}
		pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
	}

	if (m_fFuel > 0.f)
	{
		{

			E::CB_PER_UI perUI{};
			perUI.texIndex = PackTexId(12, 0);
			float fullUVH = 14.f / 256.f;
			perUI.texCoord = { 48.f / 256.f, 144.f / 256.f + fullUVH * (1.f - m_fFuel) };  // 아래쪽 끝 고정
			perUI.uvSize = { 14.f / 256.f, fullUVH * m_fFuel };
			//perUI.texCoord = { 48.f / 256.f, 144.f / 256.f };
			//perUI.uvSize = { 14.f / 256.f , 14.f / 256.f * m_fProcess };
			if (FAILED(m_pComCBufferPerUI->MapDiscard(pContext, &perUI, sizeof(perUI))))
			{
				return E_FAIL;
			}
			pContext->VSSetConstantBuffers(7, 1, m_pComCBufferPerUI->GetAdressOfBuffer());
			pContext->PSSetConstantBuffers(7, 1, m_pComCBufferPerUI->GetAdressOfBuffer());
		}
		{
			E::CB_PER_OBJECT cbPerObject{};
			cbPerObject.matWorld = *m_pComFuelTransform->GetCombinedWorldMatrix();
			XMStoreFloat4x4(&cbPerObject.matWVP, m_pComFuelTransform->GetLoadedCombinedWorldMatrix() * ctx.matViewProj);
			if (FAILED(m_pComCBufferPerObject->MapDiscard(pContext, &cbPerObject, sizeof(cbPerObject))))
			{
				return E_FAIL;
			}
			pContext->VSSetConstantBuffers(0, 1, m_pComCBufferPerObject->GetAdressOfBuffer());
			pContext->PSSetConstantBuffers(0, 1, m_pComCBufferPerObject->GetAdressOfBuffer());
		}
		pContext->DrawIndexed(viBuffer->GetNumIndices(), 0, 0);
	}

	return S_OK;
}

E::UPtr<CUIBlastFurnace> CUIBlastFurnace::Create()
{
	auto pInstance = E::ToUPtr(new CUIBlastFurnace{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUIBlastFurnace");
		return nullptr;
	}
	return  pInstance;
}

E::UPtr<E::CPrototype> CUIBlastFurnace::Clone(void* pArg)
{
	auto	pInstance = E::ToUPtr(new CUIBlastFurnace{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIBlastFurnace");
		return nullptr;
	}

	return pInstance;
}

void CUIBlastFurnace::SetInventoryHotbarItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size)
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

void CUIBlastFurnace::SetInventoryItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size)
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

void CUIBlastFurnace::SetIngredientItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size)
{
	if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(m_vecSlot[m_IngredientIdx].hItem))
	{
		if (pArr[0])
		{
			pObj->SetItemInfo(pArr[0]);
		}
		else
		{
			pObj->SetItemInfo(std::nullopt);
		}
	}
}

void CUIBlastFurnace::SetFuelItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size)
{
	if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(m_vecSlot[m_FuelIdx].hItem))
	{
		if (pArr[0])
		{
			pObj->SetItemInfo(pArr[0]);
		}
		else
		{
			pObj->SetItemInfo(std::nullopt);
		}
	}
}

void CUIBlastFurnace::SetResultItemData(std::optional<CItemObject::ItemInfo>* pArr, size_t size)
{
	if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(m_vecSlot[m_ResultIdx].hItem))
	{
		if (pArr[0])
		{
			pObj->SetItemInfo(pArr[0]);
		}
		else
		{
			pObj->SetItemInfo(std::nullopt);
		}
	}
}
