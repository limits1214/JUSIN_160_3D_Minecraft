#include "pch.h"
#include "BaseApp.h"
#include "Level.h"

NS_USING(Engine)

CBaseApp::CBaseApp()
{
}

CBaseApp::~CBaseApp()
{
}

HRESULT CBaseApp::Loop()
{
	return S_OK;
}

void CBaseApp::UpdateGUI()
{
}

void CBaseApp::FixedUpdate(_float fTimeDelta)
{
}

void CBaseApp::Update(_float fTimeDelta)
{
}

HRESULT CBaseApp::Render(_float fInterpolation)
{
	return S_OK;
}

void CBaseApp::FrameStart(_float fTimeDelta)
{
}

void CBaseApp::FrameEnd(_float fTimeDelta)
{
}

HRESULT CBaseApp::Initialize(const ENGINE_DESC& engineDesc)
{
	return S_OK;
}

HRESULT CBaseApp::StartLevel(UPtr<CLevel> pStartLevel)
{
	return S_OK;
}
