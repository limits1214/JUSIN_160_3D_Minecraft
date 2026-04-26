#include "pch.h"
#include "GameInstance.h"
#include "SoundManager.h"
#include "DInputManager.h"
#include "GraphicDevice.h"
#include "LevelManager.h"
#include "Level.h"
#include "ImguiManager.h"
#include "TimeProvider.h"
#include "PrototypeManager.h"

#include "GameObject.h"
#include "CameraManager.h"
#include "ColliderManager.h"
#include "Collider.h"

#include "Renderer.h"

#include "FlyCamera.h"

NS_USING(Engine)

CGameInstance::CGameInstance()
{
}

CGameInstance::~CGameInstance()
{
}

HRESULT CGameInstance::InitializeEngine(const ENGINE_DESC& EngineDesc, ComPtr<ID3D11Device>& ppDevice, ComPtr<ID3D11DeviceContext>& ppContext)
{
	m_hWnd = EngineDesc.hWnd;

	m_pGraphicDevice = CGraphicDevice::Create(ppDevice, ppContext);
	if (m_pGraphicDevice == nullptr)
	{
		return E_FAIL;
	}

	m_pResourceManager = CResourceManager::Create(ppDevice.Get(), ppContext.Get());
	if (m_pResourceManager == nullptr)
	{
		return E_FAIL;
	}

	if (FAILED(m_pGraphicDevice->ReadyDevice(EngineDesc.hWnd, EngineDesc.eWinMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY)))
	{
		return E_FAIL;
	}

	m_pImguiManager = CImguiManager::Create(EngineDesc.hWnd, ppDevice.Get(), ppContext.Get());
	if (m_pImguiManager == nullptr)
	{
		return E_FAIL;
	}

	m_pLevelManager = CLevelManager::Create();
	if (m_pLevelManager == nullptr)
	{
		return E_FAIL;
	}

	m_pDInputManager = CDInputManager::Create(EngineDesc.hInstance, EngineDesc.hWnd);
	if (m_pDInputManager == nullptr)
	{
		return E_FAIL;
	}

	m_pTimeProvider = CTimeProvider::Create();
	if (m_pTimeProvider == nullptr)
	{
		return E_FAIL;
	}

	m_pWorkerManager = CWorkerManager::Create();
	if (m_pWorkerManager == nullptr)
	{
		return E_FAIL;
	}

	m_pPrototypeManager = CPrototypeManager::Create(ppDevice.Get(), ppContext.Get());
	if (m_pPrototypeManager == nullptr)
	{
		return E_FAIL;
	}

	m_pGameObjectManager = CGameObjectManager::Create();
	if (m_pGameObjectManager == nullptr)
	{
		return E_FAIL;
	}

	m_pRenderer = CRenderer::Create(ppDevice.Get(), ppContext.Get());
	if (m_pRenderer == nullptr)
	{
		return E_FAIL;
	}

	m_pCameraManager = CCameraManager::Create();
	if (m_pCameraManager == nullptr)
	{
		return E_FAIL;
	}

	m_pColliderManager = CColliderManager::Create(ppDevice.Get(), ppContext.Get());
	if (m_pColliderManager == nullptr)
	{
		return E_FAIL;
	}

	if (FAILED(InitializeResources()))
	{
		return E_FAIL;
	}

	if (FAILED(InitializePrototype()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CGameInstance::UpdateEngine(_float fTimeDelta)
{
	m_pDInputManager->Update_InputDev();


	m_pGameObjectManager->PriorityUpdate(fTimeDelta);
	m_pGameObjectManager->Update(fTimeDelta);
	m_pGameObjectManager->LateUpdate(fTimeDelta);

	m_pLevelManager->Update(fTimeDelta);


	AddRenderObject(RENDERGROUP::COLLIDER, m_pColliderManager.get());
}

HRESULT CGameInstance::Draw()
{
	if (FAILED(m_pRenderer->Draw()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pLevelManager->Render()))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CGameInstance::UpdateGUI()
{
	m_pPrototypeManager->UpdateGUI();

	m_pGameObjectManager->UpdateGUI();

	m_pWorkerManager->UpdateGUI();

	m_pResourceManager->UpdateGUI();

	m_pCameraManager->UpdateGUI();

	m_pLevelManager->UpdateGUI();

	m_pColliderManager->UpdateGUI();
}

void CGameInstance::ClearResource(uint32_t iClearLevelIndex)
{
}

void CGameInstance::FrameStart(_float fTimeDelta)
{
	m_pLevelManager->FrameStart(fTimeDelta);
	m_pGameObjectManager->FrameStart();
	m_pColliderManager->FrameStart();
}

void CGameInstance::FrameEnd(_float fTimeDelta)
{
	m_pGameObjectManager->FrameEnd();
	m_pLevelManager->FrameEnd(fTimeDelta);

	m_pColliderManager->FrameEnd();
}

void CGameInstance::Release_Engine()
{
	m_pSoundManager.reset();
	m_pImguiManager.reset();
	m_pDInputManager.reset();
	m_pPrototypeManager.reset();
	m_pGameObjectManager->AllReset();
	m_pGameObjectManager.reset();
	m_pLevelManager.reset();
	m_pColliderManager.reset();
	m_pWorkerManager.reset();
	m_pResourceManager.reset();
	m_pRenderer.reset();

	m_pGraphicDevice.reset();
}

HRESULT CGameInstance::InitializeResources()
{
	return S_OK;
}


HRESULT CGameInstance::InitializePrototype()
{
	if (AddPrototype("PERMANENT", "Prototype_Component_Transform", CTransform::Create()))
	{
		return E_FAIL;
	}

	if (AddPrototype("CAMERAS", "Prototype_GameObject_FlyCamera", CFlyCamera::Create()))
	{
		return E_FAIL;
	}
	return S_OK;
}




#pragma region RESOURCE_MANAGER
SPtr<CResource> CGameInstance::AddResource(const StringID& sGroupTag, const StringID& sResTag, _string_id eAssetType, const _string& sPath, void* pArg)
{
	return m_pResourceManager->AddResource(sGroupTag, sResTag, eAssetType, sPath, pArg);
}
SPtr<CResource> CGameInstance::AddResource(const StringID& sGroupTag, const StringID& sResTag, SPtr<CResource> pAsset)
{
	return m_pResourceManager->AddResource(sGroupTag, sResTag, pAsset);
}
const std::vector<SPtr<CResource>>* CGameInstance::GetResource(const StringID& sGroupTag, const StringID& sResTag) const
{
	return m_pResourceManager->GetResource(sGroupTag, sResTag);
}
const std::unordered_map<StringID, std::vector<SPtr<CResource>>>* CGameInstance::GetResource(const StringID& sGroupTag) const
{
	return m_pResourceManager->GetResource(sGroupTag);
}
HRESULT CGameInstance::LoadResource(const StringID& sGroupTag)
{
	return m_pResourceManager->LoadResource(sGroupTag);
}
HRESULT CGameInstance::LoadResource(const StringID& sGroupTag, const StringID& sResTag)
{
	return m_pResourceManager->LoadResource(sGroupTag, sResTag);
}
HRESULT CGameInstance::UnLoadResource(const StringID& sGroupTag)
{
	return m_pResourceManager->UnLoadResource(sGroupTag);
}
HRESULT CGameInstance::UnLoadResource(const StringID& sGroupTag, const StringID& sResTag)
{
	return m_pResourceManager->UnLoadResource(sGroupTag, sResTag);
}
void CGameInstance::DelResource(const StringID& sGroupTag)
{
	m_pResourceManager->DelResource(sGroupTag);
}
void CGameInstance::DelResource(const StringID& sGroupTag, const StringID& sResTag)
{
	m_pResourceManager->DelResource(sGroupTag, sResTag);
}
#pragma endregion

#pragma region SOUND_MANAGER
HRESULT CGameInstance::CreateSound(const _string& sPath, FMOD_SOUND** ppSound)
{
	return m_pSoundManager->CreateSound(sPath, ppSound);
}


#pragma region DINPUT_MANAGER
_bool CGameInstance::KeyPressing(_ubyte byKeyID) const
{
	return m_pDInputManager->KeyPressing(byKeyID);
}
_bool CGameInstance::KeyUp(_ubyte byKeyID) const
{
	return m_pDInputManager->KeyUp(byKeyID);
}
_bool CGameInstance::KeyDown(_ubyte byKeyID) const
{
	return m_pDInputManager->KeyDown(byKeyID);
}
int32_t CGameInstance::MouseMove(MOUSEMOVESTATE eMouseState) const
{
	return m_pDInputManager->MouseMove(eMouseState);
}
_bool CGameInstance::MousePressing(MOUSEKEYSTATE eMouseState) const
{
	return  m_pDInputManager->MousePressing(eMouseState);
}
_bool CGameInstance::MouseUp(MOUSEKEYSTATE eMouseState) const
{
	return  m_pDInputManager->MouseUp(eMouseState);
}
_bool CGameInstance::MouseDown(MOUSEKEYSTATE eMouseState) const
{
	return  m_pDInputManager->MouseDown(eMouseState);
}

#pragma endregion




#pragma region GRAPHIC_DEVICE
ComPtr<ID3D11Device> CGameInstance::GetGraphicDevice() const
{
	return m_pGraphicDevice->GetDevice();
}
ComPtr<ID3D11DeviceContext> CGameInstance::GetGraphicDeviceContext() const
{
	return  m_pGraphicDevice->GetContext();
}
ComPtr<ID3D11RenderTargetView> CGameInstance::GetBackBufferRTV() const
{
	return m_pGraphicDevice->GetBackBufferRTV();
}
HRESULT CGameInstance::ClearBackBufferView(const _float4* pClearColor)
{
	return m_pGraphicDevice->ClearBackBufferView(pClearColor);
}

HRESULT CGameInstance::ClearDepthStencilView()
{
	return m_pGraphicDevice->ClearDepthStencilView();
}

HRESULT CGameInstance::Present()
{
	return m_pGraphicDevice->Present();
}
#pragma endregion


#pragma region LEVEL_MANAGER
HRESULT CGameInstance::ChangeLevel(UPtr<CLevel> pNewLevel)
{
	return m_pLevelManager->ChangeLevel(std::move(pNewLevel));
}
void CGameInstance::RegisterLevelChangeFunc(const _string& ID, _Func func)
{
	m_pLevelManager->RegisterLevelChangeFunc(ID, func);
}
#pragma endregion


#pragma region IMGUI_MANAGER
void CGameInstance::ImguiNewFrame()
{
	m_pImguiManager->Update_Imgui();
}

void CGameInstance::ImguiEndFrameAndRender()
{
	m_pImguiManager->Render_Imgui();
}

_bool CGameInstance::ImguiWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return m_pImguiManager->WinProc(hWnd, message, wParam, lParam);
}

_bool CGameInstance::ImguiGetActive() const
{
	return m_pImguiManager->Get_Active();
}

void CGameInstance::ImguiSetActive(_bool bActive)
{
	m_pImguiManager->Set_Active(bActive);
}
#pragma endregion


#pragma region WORKER_MANAGER
void CGameInstance::WorkerEnqueue(_string_view svTaskName, _Func func)
{
	m_pWorkerManager->Enqueue(svTaskName, func);
}

#pragma endregion


#pragma region TIME_PROVIDER
inline _float CGameInstance::UpdateTimeProvider()
{
	return m_pTimeProvider->UpdateTimeProvider();
}
#pragma endregion


#pragma region PROTOTYPE_MANAGER
HRESULT CGameInstance::AddPrototype(const StringID& svGroupTag, const StringID& svPrototypetag, UPtr<CPrototype> pPrototype)
{
	return m_pPrototypeManager->AddPrototype(svGroupTag, svPrototypetag, std::move(pPrototype));
}
UPtr<CPrototype> CGameInstance::ClonePrototype(const StringID& svGroupTag, const StringID& svPrototypetag, void* pArg)
{
	return m_pPrototypeManager->ClonePrototype(svGroupTag, svPrototypetag, pArg);
}
#pragma endregion


#pragma region OBJECT_MANAGER
void CGameInstance::GameObjectAllReset()
{
	m_pGameObjectManager->AllReset();
}
std::optional<CHandle> CGameInstance::AddGameObjectToLayer(const StringID& iPrototypeLevelIndex, const StringID& svPrototypeTag, const _string& svLayerTag, void* pArg)
{
	//return m_pObjectManager->AddGameObjectToLayer(iPrototypeLevelIndex, svPrototypeTag, iLayerLevelIndex, svLayerTag, pArg);
	return m_pGameObjectManager->AddGameObjectToLayer(iPrototypeLevelIndex, svPrototypeTag, svLayerTag, pArg);
}
inline CGameObject* CGameInstance::GetGameObjectByHandle(const CHandle& handle)
{
	return m_pGameObjectManager->GetGameObjectByHandle(handle);
}
const std::vector<CHandle>* CGameInstance::GetGameObjectLayer(const _string& siLayerTag) const
{
	return m_pGameObjectManager->GetLayer(siLayerTag);
}
void CGameInstance::DelGameObjectLayer(const _string& siLayerTag)
{
	return m_pGameObjectManager->DelLayer(siLayerTag);
}
#pragma endregion


#pragma region CAMERA_MANAGER
const CCameraObject* CGameInstance::GetCameraObject(const StringID& GroupID) const
{
	return m_pCameraManager->GetCameraObject(GroupID);
}
HRESULT CGameInstance::SetCameraObject(const StringID& GroupID, const CHandle& handle)
{
	return m_pCameraManager->SetCameraObject(GroupID, handle);
}

#pragma endregion



#pragma region COLLIDER_MANAGER
void CGameInstance::AddColliderGroup(const StringID& groupTag, const CCollider* pCollider)
{
	m_pColliderManager->AddColliderGroup(groupTag, pCollider);
}
const std::vector<const CCollider*>* CGameInstance::GetColliderGroup(const StringID& groupTag) const
{
	return m_pColliderManager->GetColliderGroup(groupTag);
}
_bool CGameInstance::IntersectColl(const CCollider* pColl1, const CCollider* pColl2)
{
	return m_pColliderManager->IntersectColl(pColl1, pColl2);
}
const std::unordered_map<StringID, std::vector<const CCollider*>>* CGameInstance::GetColliders() const
{
	return m_pColliderManager->GetColliders();
}
#pragma endregion

#pragma region RENDERER
HRESULT CGameInstance::AddRenderObject(RENDERGROUP eRenderGroup, IRenderable* pRenderObject)
{
	return m_pRenderer->AddRenderObject(eRenderGroup, pRenderObject);
}
#pragma endregion