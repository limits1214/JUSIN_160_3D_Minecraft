#include "GameObjectManager.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Helper.h"

NS_USING(Engine)

CGameObjectManager::CGameObjectManager()
{
}
CGameObjectManager::~CGameObjectManager()
{
}

void CGameObjectManager::UpdateGUI()
{
	ImGui::Begin("GameObject_Manager");

	if (ImGui::TreeNode("FreeSlots"))
	{
		for (const auto& i : m_FreeSlots)
		{
			ImGui::Text("%i", i);
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Slots"))
	{
		for (size_t i = 0; i < m_Objects.size(); ++i)
		{
			std::string occ = m_Objects[i].IsOccupied() ? "" : "[Empty]";
			std::string a = "idx_" + std::to_string(i) + "_gen_" + std::to_string(m_Objects[i].GetGeneration());
			occ += a;
			if (ImGui::TreeNode(occ.c_str()))
			{
				if (m_Objects[i].IsOccupied())
				{
					m_Objects[i].Get()->UpdateGUI();
				}

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Layers"))
	{
		for (const auto& [LayerID, Layer] : m_Layers)
		{
			if (ImGui::TreeNode(LayerID.c_str()))
			{
				for (const auto& handle : Layer)
				{
					if (auto* pObj = CGameInstance::Get().GetGameObjectByHandle(handle))
					{
						std::string s = std::to_string(handle.GetIndex()) + "_" + std::string{ pObj->GetTag() };
						if (ImGui::TreeNode(s.data()))
						{
							pObj->UpdateGUI();

							ImGui::TreePop();
						}
					}
				}

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Tree"))
	{
		for (const auto& rootHandle : m_TreePreparation)
		{
			UpdateGUIDrawTreeNode(rootHandle);
		}
		ImGui::TreePop();
	}

	ImGui::End();
}


void CGameObjectManager::UpdateGUIDrawTreeNode(const CHandle& handle)
{
	if (auto* pObj = GetGameObjectByHandle(handle))
	{
		std::string label =
			std::to_string(handle.GetIndex()) + "_" + std::string{ pObj->GetTag() };

		if (ImGui::TreeNode(label.c_str()))
		{
			pObj->UpdateGUI();

			if (!pObj->GetChildrenHandle().empty())
			{
				if (ImGui::TreeNode("Childrens"))
				{
					for (const auto& childHandle : pObj->GetChildrenHandle())
					{
						UpdateGUIDrawTreeNode(childHandle);
					}

					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}
	}
}

void CGameObjectManager::FrameStart()
{
	if (!m_bTreeReBuild)
	{
		return;
	}

	m_TreePreparation.clear();
	for (const auto& [LayerID, Layer] : m_Layers)
	{
		for (const auto& handle : Layer)
		{
			if (auto* pObj = GetGameObjectByHandle(handle))
			{
				if (!pObj->GetParentObjectHandle().has_value())
				{
					m_TreePreparation.push_back(pObj->GetHandle());
				}
			}
		}
	}

	m_Tree.clear();
	for (const auto& RootHandle : m_TreePreparation)
	{
		CHelper::MyHandleTreeDFS(&RootHandle, [&](const CHandle* handle)
			{
				if (auto* pObj = GetGameObjectByHandle(*handle))
				{
					m_Tree.push_back(pObj);
				}
			});
	}

	// TODO: 플래그 완성되면
	//m_bTreeReBuild = false;
}

void CGameObjectManager::FrameEnd()
{
	for (size_t i = 0; i < m_Objects.size(); ++i)
	{
		if (m_Objects[i].IsOccupied())
		{
			if (m_Objects[i].Get()->GetPendingDestroy())
			{
				m_bTreeReBuild = true;

				m_Objects[i].Reset();
				m_FreeSlots.push_back(i);
			}
		}
	}


}

std::optional<CHandle> CGameObjectManager::AddGameObjectToLayer(const StringID& siProtoGroupTag, const StringID& siPrototypeTag,
	const _string& siLayerTag, void* pArg)
{
	CHandle objectHandle{};
	if (m_FreeSlots.empty())
	{
		size_t idx = m_Objects.size();
		uint32_t gen = 0;

		objectHandle = CHandle{ idx, gen };
		static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg)->handle = objectHandle;
		// TODO: nullcheck
		auto pProto = CGameInstance::Get().ClonePrototype(siProtoGroupTag, siPrototypeTag, pArg);
		auto pGameObject = static_uptr_cast<CGameObject>(std::move(pProto));
		if (!pGameObject)
		{
			return std::nullopt;
		}
		CSlot<CGameObject> slot{ std::move(pGameObject) };

		m_Objects.push_back(std::move(slot));
	}
	else
	{
		size_t emptyIdx = m_FreeSlots.back();
		m_FreeSlots.pop_back();

		size_t idx = emptyIdx;
		uint32_t gen = m_Objects[emptyIdx].GetGeneration();

		objectHandle = CHandle{ idx, gen };
		static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg)->handle = objectHandle;
		// TODO: nullcheck
		auto pProto = CGameInstance::Get().ClonePrototype(siProtoGroupTag, siPrototypeTag, pArg);
		auto pGameObject = static_uptr_cast<CGameObject>(std::move(pProto));
		if (!pGameObject)
		{
			return std::nullopt;
		}
		m_Objects[emptyIdx].Set(std::move(pGameObject));
	}


	auto iter = m_Layers.find(siLayerTag);
	if (iter == m_Layers.end())
	{
		m_Layers.emplace(siLayerTag, std::vector<CHandle>{ objectHandle });
	}
	else
	{
		iter->second.push_back(objectHandle);
	}

	m_bTreeReBuild = true;

	return objectHandle;
}


const std::vector<CHandle>* CGameObjectManager::GetLayer(const _string& siLayerTag) const
{
	auto* pLayer = FindLayer(siLayerTag);
	if (pLayer == nullptr)
	{
		return nullptr;
	}
	return pLayer;
}

void CGameObjectManager::PriorityUpdate(_float fTimeDelta)
{
	for (auto& pObj : m_Tree)
	{
		if (!pObj->GetPendingDestroy())
		{
			pObj->PriorityUpdate(fTimeDelta);
		}
	}
}

void CGameObjectManager::Update(_float fTimeDelta)
{
	for (auto& pObj : m_Tree)
	{
		if (!pObj->GetPendingDestroy())
		{
			pObj->Update(fTimeDelta);
		}
	}
}

void CGameObjectManager::LateUpdate(_float fTimeDelta)
{
	for (auto& pObj : m_Tree)
	{
		if (!pObj->GetPendingDestroy())
		{
			pObj->LateUpdate(fTimeDelta);
		}
	}
}

const std::vector<CHandle>* CGameObjectManager::FindLayer(const _string& siLayerTag) const
{
	auto iter = m_Layers.find(siLayerTag);
	if (iter == m_Layers.end())
	{
		return nullptr;
	}

	return &iter->second;
}

UPtr<CGameObjectManager> CGameObjectManager::Create()
{
	return ToUPtr(new CGameObjectManager{});
}

// 이거 먼저 호출해주어야함
// 왜냐면 매니저가 지워지면서 오브젝트들지워주는데
// 지우는 과정에서 매니저 호출하는데 매니저가 없어서 크래시남
// 그래서 지우기 전에 먼저 이거 호출
void CGameObjectManager::AllReset()
{
	for (auto& pObj : m_Objects)
	{
		if (pObj.IsOccupied())
		{
			pObj.Get()->SetPendingDestroy();
		}
	}
	FrameEnd();

	m_Layers.clear();
	m_TreePreparation.clear();
	m_Tree.clear();
}

void CGameObjectManager::DelLayer(const _string& siLayerTag)
{
	auto iter = m_Layers.find(siLayerTag);
	if (iter != m_Layers.end())
	{
		for (const auto& handle : iter->second)
		{
			if (auto pObj = GetGameObjectByHandle(handle))
			{
				pObj->SetPendingDestroy();
			}
		}
		m_Layers.erase(iter);
		FrameEnd();
	}
}

void CGameObjectManager::Free()
{
	CEngineBase::Free();
}
