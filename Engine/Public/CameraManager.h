#pragma once
#include "Engine_Defines.h"
#include "Handle.h"
NS_BEGIN(Engine)

class CCameraObject;

class CCameraManager final : public CEngineBase
{
private:
	CCameraManager();
	~CCameraManager() override;

public:
	void UpdateGUI();

public:
	const CCameraObject* GetCameraObject(const StringID& GroupID) const;
	HRESULT SetCameraObject(const StringID& GroupID, const CHandle& handle);

public:
	std::unordered_map<StringID, CHandle> m_ActiveCameras{};

public:
	static UPtr<CCameraManager> Create();
};

NS_END