#pragma once
#include "Engine_Defines.h"
#include "Handle.h"
#include "CameraObject.h"
NS_BEGIN(Engine)

class CCameraManager final : public CEngineBase
{
private:
	CCameraManager();
	~CCameraManager() override;

public:
	void UpdateGUI();

//public:
//	const CCameraObject* GetCameraObject(const StringID& GroupID) const;
//	HRESULT SetCameraObject(const StringID& GroupID, const CHandle& handle);
//
//private:
//	std::unordered_map<StringID, CHandle> m_ActiveCameras{};

public:
	const CCameraObject* GetActiveGameCamera() const;
	HRESULT SetActiveGameCamera(const StringID& CameraID);
	const CCameraObject* GetActiveUICamera() const;
	HRESULT SetActiveUICamera(const StringID& CameraID);

	const CCameraObject* GetActiveGameCamera(const StringID& CameraID) const;
	const CCameraObject* GetActiveUICamera(const StringID& CameraID) const;



	const CCameraObject* GetGameCamera(const StringID& CameraID) const;
	const CCameraObject* GetUICamera(const StringID& CameraID) const;
	HRESULT RegistGameCamera(const StringID& CameraID, const CHandle& handle);
	HRESULT RegistUICamera(const StringID& CameraID, const CHandle& handle);

private:
	std::optional<std::pair<StringID, CHandle>> m_ActiveGameCamera{};
	std::optional<std::pair<StringID, CHandle>> m_ActiveUICamera{};
	//std::unordered_map<StringID, CHandle> m_ActiveGameCamera{};

	std::unordered_map<StringID, CHandle> m_GameCameras{};
	std::unordered_map<StringID, CHandle> m_UICameras{};

public:
	static UPtr<CCameraManager> Create();
};

NS_END