#include "pch.h"

#include "CameraManager.h"
#include "GameInstance.h"
#include "CameraObject.h"
NS_USING(Engine)

CCameraManager::CCameraManager()
{
}

CCameraManager::~CCameraManager()
{
}

void CCameraManager::UpdateGUI()
{
	ImGui::Begin("CCameraManager");

	//for (const auto& [GroupID, handle] : m_ActiveCameras)
	//{
	//	if (ImGui::TreeNode(GroupID.GetDbgStr()))
	//	{
	//		auto pObj = CGameInstance::Get().GetGameObjectByHandle(handle);
	//		if (pObj)
	//		{
	//			pObj->UpdateGUI();
	//		}

	//		ImGui::TreePop();
	//	}
	//}

	std::string activeGameCamera{"GAME: "};
	if (m_ActiveGameCamera.has_value())
	{
		if (auto c = GetGameCamera(m_ActiveGameCamera->first))
		{
			activeGameCamera += m_ActiveGameCamera->first.GetDbgStr();
		}
	}
	ImGui::Text(activeGameCamera.c_str());

	std::string activeUICamera{ "UI: " };
	if (m_ActiveUICamera.has_value())
	{
		if (auto c = GetUICamera(m_ActiveUICamera->first))
		{
			activeUICamera += m_ActiveUICamera->first.GetDbgStr();
		}
	}
	ImGui::Text(activeUICamera.c_str());

	if (ImGui::TreeNode("RegisteredGameCamera"))
	{
		for (const auto& camHandle : m_GameCameras)
		{
			ImGui::PushID(camHandle.first.GetDbgStr());

			if (ImGui::TreeNode(camHandle.first.GetDbgStr()))
			{
				auto pObj = CGameInstance::Get().GetGameObjectByHandle(camHandle.second);

				if (pObj)
				{
					pObj->UpdateGUI();
				}

				ImGui::TreePop();
			}

			if (ImGui::Button("Active"))
			{
				SetActiveGameCamera(camHandle.first);
			}

			ImGui::PopID();
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("RegisteredUICamera"))
	{
		for (const auto& camHandle : m_UICameras)
		{
			ImGui::PushID(camHandle.first.GetDbgStr());

			if (ImGui::TreeNode(camHandle.first.GetDbgStr()))
			{
				auto pObj = CGameInstance::Get().GetGameObjectByHandle(camHandle.second);

				if (pObj)
				{
					pObj->UpdateGUI();
				}

				ImGui::TreePop();
			}

			if (ImGui::Button("Active"))
			{
				SetActiveUICamera(camHandle.first);
			}

			ImGui::PopID();
		}

		ImGui::TreePop();
	}



	ImGui::End();
}

//const CCameraObject* CCameraManager::GetCameraObject(const StringID& GroupID) const
//{
//	auto iter = m_ActiveCameras.find(GroupID);
//	if (iter == m_ActiveCameras.end())
//	{
//		return nullptr;
//	}
//
//	auto pObj = CGameInstance::Get().GetGameObjectByHandle(iter->second);
//	if (!pObj)
//	{
//		return nullptr;
//	}
//	
//	if (!pObj->IsA(CCameraObject::StaticType))
//	{
//		return nullptr;
//	}
//
//	return static_cast<CCameraObject*>(pObj);
//}
//
//HRESULT CCameraManager::SetCameraObject(const StringID& GroupID, const CHandle& handle)
//{
//	auto pObj = CGameInstance::Get().GetGameObjectByHandle(handle);
//	if (!pObj)
//	{
//		return E_FAIL;
//	}
//
//	if (!pObj->IsA(CCameraObject::StaticType))
//	{
//		return E_FAIL;
//	}
//
//	auto iter = m_ActiveCameras.find(GroupID);
//	if (iter != m_ActiveCameras.end())
//	{
//		m_ActiveCameras.erase(iter);
//	}
//	m_ActiveCameras.emplace(GroupID, handle);
//
//	return S_OK;
//}

const CCameraObject* CCameraManager::GetActiveGameCamera() const
{
	if (!m_ActiveGameCamera.has_value())
	{
		return nullptr;
	}

	auto pObj = CGameInstance::Get().GetGameObjectByHandle(m_ActiveGameCamera->second);
	if (!pObj)
	{
		return nullptr;
	}

	if (!pObj->IsA(CCameraObject::StaticType))
	{
		return nullptr;
	}

	return static_cast<CCameraObject*>(pObj);
}

HRESULT CCameraManager::SetActiveGameCamera(const StringID& CameraID)
{
	auto iter = m_GameCameras.find(CameraID);
	if (iter == m_GameCameras.end())
	{
		return E_FAIL;
	}
	auto pObj = CGameInstance::Get().GetGameObjectByHandle(iter->second);
	if (!pObj)
	{
		return E_FAIL;
	}

	if (!pObj->IsA(CCameraObject::StaticType))
	{
		return E_FAIL;
	}

	m_ActiveGameCamera = *iter;

	return S_OK;
}

const CCameraObject* CCameraManager::GetActiveUICamera() const
{
	if (!m_ActiveUICamera.has_value())
	{
		return nullptr;
	}

	auto pObj = CGameInstance::Get().GetGameObjectByHandle(m_ActiveUICamera->second);
	if (!pObj)
	{
		return nullptr;
	}

	if (!pObj->IsA(CCameraObject::StaticType))
	{
		return nullptr;
	}

	return static_cast<CCameraObject*>(pObj);
}

HRESULT CCameraManager::SetActiveUICamera(const StringID& CameraID)
{
	auto iter = m_UICameras.find(CameraID);
	if (iter == m_UICameras.end())
	{
		return E_FAIL;
	}
	auto pObj = CGameInstance::Get().GetGameObjectByHandle(iter->second);
	if (!pObj)
	{
		return E_FAIL;
	}

	if (!pObj->IsA(CCameraObject::StaticType))
	{
		return E_FAIL;
	}

	m_ActiveUICamera = *iter;

	return S_OK;
}

const CCameraObject* CCameraManager::GetActiveGameCamera(const StringID& CameraID) const
{
	auto tmp = GetActiveGameCamera();
	if (tmp != GetGameCamera(CameraID))
	{
		return nullptr;
	}
	return tmp;
}

const CCameraObject* CCameraManager::GetActiveUICamera(const StringID& CameraID) const
{
	auto tmp = GetActiveUICamera();
	if (tmp != GetUICamera(CameraID))
	{
		return nullptr;
	}
	return tmp;
}

const CCameraObject* CCameraManager::GetGameCamera(const StringID& CameraID) const
{
	auto iter = m_GameCameras.find(CameraID);
	if (iter == m_GameCameras.end())
	{
		return nullptr;
	}
	auto pObj = CGameInstance::Get().GetGameObjectByHandle(iter->second);
	if (!pObj)
	{
		return nullptr;
	}

	if (!pObj->IsA(CCameraObject::StaticType))
	{
		return nullptr;
	}

	return static_cast<CCameraObject*>(pObj);
}

const CCameraObject* CCameraManager::GetUICamera(const StringID& CameraID) const
{
	auto iter = m_UICameras.find(CameraID);
	if (iter == m_UICameras.end())
	{
		return nullptr;
	}
	auto pObj = CGameInstance::Get().GetGameObjectByHandle(iter->second);
	if (!pObj)
	{
		return nullptr;
	}

	if (!pObj->IsA(CCameraObject::StaticType))
	{
		return nullptr;
	}

	return static_cast<CCameraObject*>(pObj);
}

HRESULT CCameraManager::RegistGameCamera(const StringID& CameraID, const CHandle& handle)
{
	auto pObj = CGameInstance::Get().GetGameObjectByHandle(handle);
	if (!pObj)
	{
		return E_FAIL;
	}

	if (!pObj->IsA(CCameraObject::StaticType))
	{
		return E_FAIL;
	}

	auto iter = m_GameCameras.find(CameraID);
	if (iter != m_GameCameras.end())
	{
		m_GameCameras.erase(iter);
	}
	m_GameCameras.emplace(CameraID, handle);
	return S_OK;
}

HRESULT CCameraManager::RegistUICamera(const StringID& CameraID, const CHandle& handle)
{
	auto pObj = CGameInstance::Get().GetGameObjectByHandle(handle);
	if (!pObj)
	{
		return E_FAIL;
	}

	if (!pObj->IsA(CCameraObject::StaticType))
	{
		return E_FAIL;
	}

	auto iter = m_UICameras.find(CameraID);
	if (iter != m_UICameras.end())
	{
		m_UICameras.erase(iter);
	}
	m_UICameras.emplace(CameraID, handle);
	return S_OK;
}

UPtr<CCameraManager> CCameraManager::Create()
{
	return ToUPtr(new CCameraManager{});
}
