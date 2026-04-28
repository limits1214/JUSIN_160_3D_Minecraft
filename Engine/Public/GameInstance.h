#pragma once
#include "Engine_Defines.h"
#include "ResourceManager.h"
#include "WorkerManager.h"
#include "GameObjectManager.h"

struct FMOD_SOUND;

NS_BEGIN(Engine)
class CTimeProvider;
class CSoundManager;
class CDInputManager;
class CGraphicDevice;
class CLevelManager;
class CLevel;
class CImguiManager;
class CPrototypeManager;
class CPrototype;
class CCameraManager;
class CColliderManager;
class CCollider;
class CRenderer;

class ENGINE_DLL CGameInstance final : public Singleton<CGameInstance>
{
	friend Singleton<CGameInstance>;
private:
	CGameInstance();
	~CGameInstance();

public:
	HRESULT InitializeEngine(const ENGINE_DESC& EngineDesc, ComPtr<ID3D11Device>& ppDevice, ComPtr<ID3D11DeviceContext>& ppContext);
	void UpdateEngine(_float fTimeDelta);
	HRESULT Draw();
	void UpdateGUI();
	void ClearResource(uint32_t iClearLevelIndex);
	void FrameStart(_float fTimeDelta);
	void FrameEnd(_float fTimeDelta);

public:
	void Release_Engine();

private:
	HRESULT InitializeResources();
	HRESULT InitializePrototype();

public:
	HWND GetHwnd() const { return m_hWnd; }
private:
	HWND m_hWnd;


#pragma region RESOURCE_MANAGER
public:
	SPtr<CResource> AddResource(const StringID& sGroupTag, const StringID& sResTag, _string_id eAssetType, const _string& sPath, void* pArg = nullptr);
	SPtr<CResource> AddResource(const StringID& sGroupTag, const StringID& sResTag, SPtr<CResource> pAsset);
	template<typename T>
	SPtr<T> AddResourceT(const StringID& sGroupTag, const StringID& sResTag, const _string& sPath, void* pArg = nullptr)
	{
		return m_pResourceManager->AddResourceT<T>(sGroupTag, sResTag, sPath, pArg);
	}
	template<typename T>
	SPtr<T> AddResourceT(const StringID& sGroupTag, const StringID& sResTag, SPtr<T> pAsset)
	{
		return m_pResourceManager->AddResourceT<T>(sGroupTag, sResTag, pAsset);
	}
	template<typename T>
	SPtr<T> GetResourceFirst(const StringID& sGroupTag, const StringID& sResTag) const
	{
		return m_pResourceManager->GetResourceFirst<T>(sGroupTag, sResTag);
	}
	const std::vector<SPtr<CResource>>* GetResource(const StringID& sGroupTag, const StringID& sResTag) const;
	const std::unordered_map<StringID, std::vector<SPtr<CResource>>>* GetResource(const StringID& sGroupTag) const;
	HRESULT LoadResource(const StringID& sGroupTag);
	HRESULT LoadResource(const StringID& sGroupTag, const StringID& sResTag);
	HRESULT UnLoadResource(const StringID& sGroupTag);
	HRESULT UnLoadResource(const StringID& sGroupTag, const StringID& sResTag);
	void DelResource(const StringID& sGroupTag);
	void DelResource(const StringID& sGroupTag, const StringID& sResTag);
#pragma endregion

#pragma region SOUND_MANAGER
public:
	HRESULT CreateSound(const _string& sPath, FMOD_SOUND** ppSound);
#pragma endregion

#pragma region DINPUT_MANAGER
public:
	_bool KeyPressing(_ubyte byKeyID) const;
	_bool KeyUp(_ubyte byKeyID) const;
	_bool KeyDown(_ubyte byKeyID) const;
	int32_t	MouseMove(MOUSEMOVESTATE eMouseState) const;
	_bool MousePressing(MOUSEKEYSTATE eMouseState) const;
	_bool MouseUp(MOUSEKEYSTATE eMouseState) const;
	_bool MouseDown(MOUSEKEYSTATE eMouseState) const;
#pragma endregion

#pragma region GRAPHIC_DEVICE
public:
	ComPtr<ID3D11Device> GetGraphicDevice() const;
	ComPtr<ID3D11DeviceContext> GetGraphicDeviceContext()const;
	ComPtr<ID3D11RenderTargetView> GetBackBufferRTV() const;
	HRESULT ClearBackBufferView(const _float4* pClearColor);
	HRESULT ClearDepthStencilView();
	HRESULT Present();
#pragma endregion

#pragma region LEVEL_MANAGER
public:
	HRESULT ChangeLevel(UPtr<CLevel> pNewLevel);
	void RegisterLevelChangeFunc(const _string& ID, _Func func);
#pragma endregion


#pragma region IMGUI_MANAGER
public:
	void ImguiNewFrame();
	void ImguiEndFrameAndRender();
	_bool ImguiWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	_bool ImguiGetActive() const;
	void ImguiSetActive(_bool bActive);
#pragma endregion

#pragma region WORKER_MANAGER
public:
	void WorkerEnqueue(_string_view svTaskName, _Func func);
	template<typename Func, typename... Args>
	auto WorkerEnqueueWithFuture(_string_view svTaskName, Func&& f, Args&&... args)
		-> std::future<std::invoke_result_t<Func, Args...>>
	{
		return m_pWorkerManager->WorkerEnqueueWithFuture(
			svTaskName,
			std::forward<Func>(f),
			std::forward<Args>(args)...
		);
	}
#pragma endregion

#pragma region TIME_PROVIDER
public:
	inline _float UpdateTimeProvider();
#pragma endregion

#pragma region PROTOTYPE_MANAGER
public:
	HRESULT AddPrototype(const StringID& svGroupTag, const StringID& svPrototypetag, UPtr<CPrototype> pPrototype);
	UPtr<CPrototype> ClonePrototype(const StringID& svGroupTag, const StringID& svPrototypetag, void* pArg = nullptr);
#pragma endregion


#pragma region GAMEOBJECT_MANAGER
public:
	void GameObjectAllReset();
	std::optional<CHandle> AddGameObjectToLayer(const StringID& iPrototypeLevelIndex, const StringID& svPrototypeTag, const _string& svLayerTag, void* pArg = nullptr);
	inline CGameObject* GetGameObjectByHandle(const CHandle& handle);
	const std::vector<CHandle>* GetGameObjectLayer(const _string& siLayerTag) const;
	void DelGameObjectLayer(const _string& siLayerTag);

	template<typename T>
	T* GetGameObjectByHandleT(const CHandle& handle)
	{
		return m_pGameObjectManager->GetGameObjectByHandleT<T>(handle);
	}
	template<typename T>
	const T* GetGameObjectByHandleT(const CHandle& handle) const
	{
		return static_cast<const CGameObjectManager*>(m_pGameObjectManager.get())->GetGameObjectByHandleT<T>(handle);
	}
#pragma endregion


#pragma region CAMERA_MANAGER
public:
	const CCameraObject* GetCameraObject(const StringID& GroupID) const;
	HRESULT SetCameraObject(const StringID& GroupID, const CHandle& handle);
#pragma endregion


#pragma region COLLIDER_MANAGER
public:
	void AddColliderGroup(const StringID& groupTag, const CCollider*);
	const std::vector<const CCollider*>* GetColliderGroup(const StringID& groupTag) const;
	_bool IntersectColl(const CCollider* pColl1, const CCollider* pColl2);
	const std::unordered_map<StringID, std::vector<const CCollider*>>* GetColliders() const;
	//const CCollider* GetColliderGroupFirst(const StringID& groupTag) const;
#pragma endregion

#pragma region RENDERER
public:
	HRESULT AddRenderObject(RENDERGROUP eRenderGroup, IRenderable* pRenderObject);
#pragma endregion

private:
	UPtr<CGraphicDevice> m_pGraphicDevice{};
	UPtr<CImguiManager> m_pImguiManager{};
	UPtr<CResourceManager> m_pResourceManager{};
	UPtr<CSoundManager> m_pSoundManager{};
	UPtr<CDInputManager> m_pDInputManager{};
	UPtr<CLevelManager> m_pLevelManager{};
	UPtr<CWorkerManager> m_pWorkerManager{};
	UPtr<CTimeProvider> m_pTimeProvider{};
	UPtr<CPrototypeManager> m_pPrototypeManager{};
	UPtr<CGameObjectManager> m_pGameObjectManager{};
	UPtr<CCameraManager> m_pCameraManager{};
	UPtr<CColliderManager> m_pColliderManager{};
	UPtr<CRenderer> m_pRenderer{};
};

NS_END
