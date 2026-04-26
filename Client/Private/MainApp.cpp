#include "pch.h"
#include "MainApp.h"

NS_USING(Client)

CMainApp::CMainApp()
{
}

CMainApp::~CMainApp()
{
}

void CMainApp::FixedUpdate(E::_float fTimeDelta)
{
}

void CMainApp::Update(E::_float fTimeDelta)
{
}

HRESULT CMainApp::Render(E::_float fInterpolation)
{
	return S_OK;
}

void CMainApp::FrameStart(E::_float fTimeDelta)
{
}

void CMainApp::FrameEnd(E::_float fTimeDelta)
{
}

HRESULT CMainApp::Initialize()
{
	return S_OK;
}

HRESULT CMainApp::ReadyWSClient()
{
	return S_OK;
}

Engine::UPtr<CMainApp> CMainApp::Create()
{
	return E::ToUPtr(new CMainApp{});
}
