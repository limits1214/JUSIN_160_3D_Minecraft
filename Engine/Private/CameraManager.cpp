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

	for (const auto& [GroupID, handle] : m_ActiveCameras)
	{
		if (ImGui::TreeNode(GroupID.GetDbgStr()))
		{
			auto pObj = CGameInstance::Get().GetGameObjectByHandle(handle);
			if (pObj)
			{
				pObj->UpdateGUI();
			}

			ImGui::TreePop();
		}
	}

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
	//auto a = pObj->GetType();
	//auto b = pObj->GetTypeStr();
	//if (pObj->GetType() != CCameraObject::StaticType)
	//{
	//	return nullptr;
	//}

	return static_cast<CCameraObject*>(pObj);
}

HRESULT CCameraManager::SetCameraObject(const StringID& GroupID, const CHandle& handle)
{
	auto pObj = CGameInstance::Get().GetGameObjectByHandle(handle);
	if (!pObj)
	{
		return E_FAIL;
	}

	if (!dynamic_cast<CCameraObject*>(pObj))
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

UPtr<CCameraManager> CCameraManager::Create()
{
	return ToUPtr(new CCameraManager{});
}
