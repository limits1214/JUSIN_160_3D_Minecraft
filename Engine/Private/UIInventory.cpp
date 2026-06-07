#include "pch.h"
#include "UIInventory.h"
#include "GameInstance.h"
#include "CameraObject.h"
#include "Resources.h"
#include "ComConstantBuffer.h"

#include "UIItem.h"

NS_USING(Engine)

CUIInventory::CUIInventory()
{
}


CUIInventory::~CUIInventory()
{
}

void CUIInventory::UpdateGUI()
{
	CUIObject::UpdateGUI();

	if (ImGui::Button("RenderToggle"))
	{
		m_bRender = !m_bRender;
	}
}

HRESULT CUIInventory::Initialize(void* pArg)
{
	auto pDesc = static_cast<CUIObject::UIOBJECT_DESC*>(pArg);
	pDesc->fSizeX = 176.f * MC_UI_SCALE;
	pDesc->fSizeY = 166.f * MC_UI_SCALE;

	pDesc->fX = (1280.f * 0.5f) ;
	pDesc->fY = (720.f * 0.5f) ;

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

void CUIInventory::PriorityUpdate(E::_float fTimeDelta)
{
}

void CUIInventory::Update(E::_float fTimeDelta)
{
	if (m_hOnCursorItem)
	{
		if (auto pItem = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(m_hOnCursorItem.value()))
		{
			POINT mousePos;
			GetCursorPos(&mousePos);
			ScreenToClient(CGameInstance::Get().GetHwnd(), &mousePos);

			pItem->SetOrigin(_float2{ (float)mousePos.x, (float)mousePos.y });





			if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB)
				|| CGameInstance::Get().MouseDown(MOUSEKEYSTATE::RB))
			{
				for (auto& slot : m_vecInventorySlot)
				{
					//if (slot.hItem.has_value()) continue;

					auto itemOrigin = pItem->GetOrigin();
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
						if (slot.hItem.has_value())
						{
							if (auto pSlotItem = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(slot.hItem.value()))
							{
								if (pSlotItem->GetItemType() == pItem->GetItemType())
								{
									if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB))
									{
										auto isUnCountable = pSlotItem->GetCnt() == 0;
										if (isUnCountable)
										{
											std::swap(m_hOnCursorItem, slot.hItem);
										}
										else
										{
											auto isMaxOver = pItem->GetCnt() + pSlotItem->GetCnt() >= 64;
											if (isMaxOver)
											{
												std::swap(m_hOnCursorItem, slot.hItem);
											}
											else
											{
												pSlotItem->SetCnt(pItem->GetCnt() + pSlotItem->GetCnt());
												pItem->SetPendingDestroyCascade();
												m_hOnCursorItem = std::nullopt;
											}
										}
									}
									else
									{
										auto isUnCountable = pSlotItem->GetCnt() == 0;
										if (isUnCountable)
										{
											std::swap(m_hOnCursorItem, slot.hItem);
										}
										else
										{
											auto isCanAdd = pSlotItem->GetCnt() < 64;
											if (isCanAdd)
											{
												pSlotItem->SetCnt(pSlotItem->GetCnt() + 1);

												if (pItem->GetCnt() > 1)
												{
													pItem->SetCnt(pItem->GetCnt() - 1);
												}
												else
												{
													pItem->SetPendingDestroyCascade();
													m_hOnCursorItem = std::nullopt;
												}
											}
											else
											{
												std::swap(m_hOnCursorItem, slot.hItem);
											}
										}
									}
								}
								else
								{
									std::swap(m_hOnCursorItem, slot.hItem);
									int x = 0;
								}
							}
						}
						else
						{
							if (pItem->GetCnt() <= 1)
							{
								slot.hItem = m_hOnCursorItem;
								m_hOnCursorItem = std::nullopt;
							}
							else
							{
								if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB))
								{
									slot.hItem = m_hOnCursorItem;
									m_hOnCursorItem = std::nullopt;
								}
								else
								{
									pItem->SetCnt(pItem->GetCnt() - 1);
									
									{
										E::CUIItem::DESC Desc{};
										Desc.fX = itemOrigin.x;
										Desc.fY = itemOrigin.y;
										Desc.sObjectTag = "UIItem";
										Desc.eType = pItem->GetItemType();
										if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIItem",
											"80_UI", &Desc))
										{
											slot.hItem = handle;
											if (auto pItem = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(handle.value()))
											{
												pItem->SetOrigin(slotOrigin);
												pItem->SetCnt(1);
											}
										}
									}
								}
							}
						}

						pItem->SetOrigin(slotOrigin);
						break;
					}
				}
			}


			
		}
	}
	else
	{
		if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB)
			|| CGameInstance::Get().MouseDown(MOUSEKEYSTATE::RB))
		{
			POINT mousePos;
			GetCursorPos(&mousePos);
			ScreenToClient(CGameInstance::Get().GetHwnd(), &mousePos);

			for (auto& slot : m_vecInventorySlot)
			{
				if (!slot.hItem.has_value()) continue;

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
					if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB))
					{
						m_hOnCursorItem = slot.hItem;
						slot.hItem = std::nullopt;
					}
					else
					{
						if (auto pItem = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(slot.hItem.value()))
						{
							auto currItemCnt = pItem->GetCnt();
							if (currItemCnt <= 1)
							{
								m_hOnCursorItem = slot.hItem;
								slot.hItem = std::nullopt;
							}
							else
							{
								auto dvd2 = currItemCnt / 2;
								auto remain = currItemCnt - dvd2;
								pItem->SetCnt(remain);

								{
									E::CUIItem::DESC Desc{};
									Desc.fX = itemOrigin.x;
									Desc.fY = itemOrigin.y;
									Desc.sObjectTag = "UIItem";
									Desc.eType = pItem->GetItemType();
									if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIItem",
										"80_UI", &Desc))
									{
										if (auto pItem = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(handle.value()))
										{
											pItem->SetCnt(dvd2);
										}
										m_hOnCursorItem = handle;
									}
								}
							}
						}
					}
					
					break;
				}
			}
		}
	}
}

void CUIInventory::LateUpdate(E::_float fTimeDelta)
{
	if (m_bRender)
	{
		E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::UI, this);
	}

	GetTransform().Update();
}

HRESULT CUIInventory::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
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
		perUI.texIndex = PackTexId(12, 2);
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

void CUIInventory::InitializeSlot()
{
	{
		auto posX = 16.f;
		auto posY = 16.f;
		InventorySlot armorHelmet{};
		armorHelmet.eType = SlotType::ARMOR_HELMET;
		armorHelmet.vOriginPos.x = (m_fX - m_fSizeX * 0.5f) + posX * MC_UI_SCALE;
		armorHelmet.vOriginPos.y = (m_fY - m_fSizeY * 0.5f) + posY * MC_UI_SCALE;
		m_vecInventorySlot.push_back(armorHelmet);
	}

	{
		auto posX = 16.f;
		auto posY = 34.f;
		InventorySlot armorChestplate{};
		armorChestplate.eType = SlotType::ARMOR_CHESTPLATE;
		armorChestplate.vOriginPos.x = (m_fX - m_fSizeX * 0.5f) + posX * MC_UI_SCALE;
		armorChestplate.vOriginPos.y = (m_fY - m_fSizeY * 0.5f) + posY * MC_UI_SCALE;
		m_vecInventorySlot.push_back(armorChestplate);
	}

	{
		auto posX = 16.f;
		auto posY = 52.f;
		InventorySlot armorLeggins{};
		armorLeggins.eType = SlotType::ARMOR_LEGGINGS;
		armorLeggins.vOriginPos.x = (m_fX - m_fSizeX * 0.5f) + posX * MC_UI_SCALE;
		armorLeggins.vOriginPos.y = (m_fY - m_fSizeY * 0.5f) + posY * MC_UI_SCALE;
		m_vecInventorySlot.push_back(armorLeggins);
	}

	{
		auto posX = 16.f;
		auto posY = 70.f;
		InventorySlot armorBoots{};
		armorBoots.eType = SlotType::ARMOR_BOOTS;
		armorBoots.vOriginPos.x = (m_fX - m_fSizeX * 0.5f) + posX * MC_UI_SCALE;
		armorBoots.vOriginPos.y = (m_fY - m_fSizeY * 0.5f) + posY * MC_UI_SCALE;
		m_vecInventorySlot.push_back(armorBoots);
	}

	{
		auto posX = 85.f;
		auto posY = 70.f;
		InventorySlot shiled{};
		shiled.eType = SlotType::SHIELD;
		shiled.vOriginPos.x = (m_fX - m_fSizeX * 0.5f) + posX * MC_UI_SCALE;
		shiled.vOriginPos.y = (m_fY - m_fSizeY * 0.5f) + posY * MC_UI_SCALE;
		m_vecInventorySlot.push_back(shiled);
	}

	{
		auto posX = 106.f;
		auto posY = 26.f;
		InventorySlot craftLt{};
		craftLt.eType = SlotType::CRAFT_LT;
		craftLt.vOriginPos.x = (m_fX - m_fSizeX * 0.5f) + posX * MC_UI_SCALE;
		craftLt.vOriginPos.y = (m_fY - m_fSizeY * 0.5f) + posY * MC_UI_SCALE;
		m_vecInventorySlot.push_back(craftLt);
	}

	{
		auto posX = 124.f;
		auto posY = 26.f;
		InventorySlot craftRt{};
		craftRt.eType = SlotType::CRAFT_RT;
		craftRt.vOriginPos.x = (m_fX - m_fSizeX * 0.5f) + posX * MC_UI_SCALE;
		craftRt.vOriginPos.y = (m_fY - m_fSizeY * 0.5f) + posY * MC_UI_SCALE;
		m_vecInventorySlot.push_back(craftRt);
	}

	{
		auto posX = 106.f;
		auto posY = 44.f;
		InventorySlot craftLb{};
		craftLb.eType = SlotType::CRAFT_LB;
		craftLb.vOriginPos.x = (m_fX - m_fSizeX * 0.5f) + posX * MC_UI_SCALE;
		craftLb.vOriginPos.y = (m_fY - m_fSizeY * 0.5f) + posY * MC_UI_SCALE;
		m_vecInventorySlot.push_back(craftLb);
	}

	{
		auto posX = 124.f;
		auto posY = 44.f;
		InventorySlot craftRb{};
		craftRb.eType = SlotType::CRAFT_RB;
		craftRb.vOriginPos.x = (m_fX - m_fSizeX * 0.5f) + posX * MC_UI_SCALE;
		craftRb.vOriginPos.y = (m_fY - m_fSizeY * 0.5f) + posY * MC_UI_SCALE;
		m_vecInventorySlot.push_back(craftRb);
	}

	{
		auto posX = 162.f;
		auto posY = 36.f;
		InventorySlot craftResult{};
		craftResult.eType = SlotType::CRAFT_RESULT;
		craftResult.vOriginPos.x = (m_fX - m_fSizeX * 0.5f) + posX * MC_UI_SCALE;
		craftResult.vOriginPos.y = (m_fY - m_fSizeY * 0.5f) + posY * MC_UI_SCALE;
		m_vecInventorySlot.push_back(craftResult);
	}

	for (uint32_t i = 0; i < 9; ++i)
	{
		for (uint32_t j = 0; j < 3; ++j)
		{
			m_InventoryIdxs[9 * j + i] = m_vecInventorySlot.size();

			auto posX = 16.f + (i * 18.f);
			auto posY = 92.f + (j * 18.f);

			InventorySlot InvenSlot{};
			InvenSlot.eType = SlotType::INVENTORY;
			InvenSlot.vOriginPos.x = (m_fX - m_fSizeX * 0.5f) + posX * MC_UI_SCALE;
			InvenSlot.vOriginPos.y = (m_fY - m_fSizeY * 0.5f) + posY * MC_UI_SCALE;
			m_vecInventorySlot.push_back(InvenSlot);
		}
	}

	
	for (uint32_t i = 0; i < 9; ++i)
	{
		m_HotbarIdxs[i] = m_vecInventorySlot.size();
		auto posX = 16.f + (i * 18.f);
		auto posY = 150.f ;

		InventorySlot hotbarSlot{};
		hotbarSlot.eType = SlotType::HOTBAR;
		hotbarSlot.vOriginPos.x = (m_fX - m_fSizeX * 0.5f) + posX * MC_UI_SCALE;
		hotbarSlot.vOriginPos.y = (m_fY - m_fSizeY * 0.5f) + posY * MC_UI_SCALE;
		m_vecInventorySlot.push_back(hotbarSlot);
	}

	
	{
		E::CUIItem::DESC Desc{};
		Desc.fX = m_vecInventorySlot[m_HotbarIdxs[0]].vOriginPos.x;
		Desc.fY = m_vecInventorySlot[m_HotbarIdxs[0]].vOriginPos.y;
		Desc.eType = CUIItem::TYPE::BLOCK_TNT;
		Desc.sObjectTag = "UIItem";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIItem",
			"80_UI", &Desc))
		{
			if (auto pItem = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(handle.value()))
			{
				pItem->SetCnt(18);
			}
			m_vecInventorySlot[m_HotbarIdxs[0]].hItem = handle;
			//m_hOnCursorItem = handle;
		}
	}

	{
		E::CUIItem::DESC Desc{};
		Desc.fX = m_vecInventorySlot[m_HotbarIdxs[1]].vOriginPos.x;
		Desc.fY = m_vecInventorySlot[m_HotbarIdxs[1]].vOriginPos.y;
		Desc.eType = CUIItem::TYPE::BLOCK_DIRT;
		Desc.sObjectTag = "UIItem";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIItem",
			"80_UI", &Desc))
		{
			if (auto pItem = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(handle.value()))
			{
				pItem->SetCnt(18);
			}
			m_vecInventorySlot[m_HotbarIdxs[1]].hItem = handle;
			//m_hOnCursorItem = handle;
		}
	}

	{
		E::CUIItem::DESC Desc{};
		Desc.fX = m_vecInventorySlot[m_HotbarIdxs[2]].vOriginPos.x;
		Desc.fY = m_vecInventorySlot[m_HotbarIdxs[2]].vOriginPos.y;
		Desc.eType = CUIItem::TYPE::BLOCK_COBBLESTONE;
		Desc.sObjectTag = "UIItem";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIItem",
			"80_UI", &Desc))
		{
			if (auto pItem = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(handle.value()))
			{
				pItem->SetCnt(60);
			}
			m_vecInventorySlot[m_HotbarIdxs[2]].hItem = handle;
			//m_hOnCursorItem = handle;
		}
	}

	{
		E::CUIItem::DESC Desc{};
		Desc.fX = m_vecInventorySlot[m_HotbarIdxs[3]].vOriginPos.x;
		Desc.fY = m_vecInventorySlot[m_HotbarIdxs[3]].vOriginPos.y;
		Desc.eType = CUIItem::TYPE::BLOCK_COBBLESTONE;
		Desc.sObjectTag = "UIItem";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIItem",
			"80_UI", &Desc))
		{
			if (auto pItem = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(handle.value()))
			{
				pItem->SetCnt(31);
			}
			m_vecInventorySlot[m_HotbarIdxs[3]].hItem = handle;
			//m_hOnCursorItem = handle;
		}
	}

	{
		E::CUIItem::DESC Desc{};
		Desc.fX = m_vecInventorySlot[m_HotbarIdxs[4]].vOriginPos.x;
		Desc.fY = m_vecInventorySlot[m_HotbarIdxs[4]].vOriginPos.y;
		Desc.eType = CUIItem::TYPE::ITEM_WoodPickaxe;
		Desc.sObjectTag = "UIItem";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIItem",
			"80_UI", &Desc))
		{
			if (auto pItem = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(handle.value()))
			{
				pItem->SetCnt(0);
			}
			m_vecInventorySlot[m_HotbarIdxs[4]].hItem = handle;
			//m_hOnCursorItem = handle;
		}
	}
	{
		E::CUIItem::DESC Desc{};
		Desc.fX = m_vecInventorySlot[m_HotbarIdxs[5]].vOriginPos.x;
		Desc.fY = m_vecInventorySlot[m_HotbarIdxs[5]].vOriginPos.y;
		Desc.eType = CUIItem::TYPE::ITEM_CooperHelmet;
		Desc.sObjectTag = "UIItem";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIItem",
			"80_UI", &Desc))
		{
			if (auto pItem = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(handle.value()))
			{
				pItem->SetCnt(0);
			}
			m_vecInventorySlot[m_HotbarIdxs[5]].hItem = handle;
			//m_hOnCursorItem = handle;
		}
	}
}

E::UPtr<CUIInventory> CUIInventory::Create()
{
	auto pInstance = E::ToUPtr(new CUIInventory{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUIInventory");
		return nullptr;
	}
	return  pInstance;
}

E::UPtr<E::CPrototype> CUIInventory::Clone(void* pArg)
{
	auto	pInstance = E::ToUPtr(new CUIInventory{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIInventory");
		return nullptr;
	}

	return pInstance;
}
