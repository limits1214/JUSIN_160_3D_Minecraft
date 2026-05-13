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

	ImGui::End();
}

const CCameraObject* CCameraManager::GetCameraObject(const StringID& GroupID) const
{
	auto iter = m_ActiveCameras.find(GroupID);
	if (iter == m_ActiveCameras.end())
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

HRESULT CCameraManager::SetCameraObject(const StringID& GroupID, const CHandle& handle)
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

	auto iter = m_ActiveCameras.find(GroupID);
	if (iter != m_ActiveCameras.end())
	{
		m_ActiveCameras.erase(iter);
	}
	m_ActiveCameras.emplace(GroupID, handle);

	return S_OK;
}

const CCameraObject* CCameraManager::GetActiveGameCamera() const
{
	if (!m_ActiveGameCamera.has_value())
	{
		return nullptr;
	}

	auto pObj = CGameInstance::Get().GetGameObjectByHandle(m_ActiveGameCamera.value());
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

	m_ActiveGameCamera = iter->second;

	return S_OK;
}

const CCameraObject* CCameraManager::GetActiveUICamera() const
{
	if (!m_ActiveUICamera.has_value())
	{
		return nullptr;
	}

	auto pObj = CGameInstance::Get().GetGameObjectByHandle(m_ActiveUICamera.value());
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

	m_ActiveUICamera = iter->second;

	return S_OK;
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
