#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)
class CFurnaceStorage;
class CChestStorage;
class ENGINE_DLL CWorldManager final: public CEngineBase
{
public:
	enum class WORLD_TYPE
	{
		OVER,
		NETHER,
		THE_END,

		END
	};
private:
	explicit CWorldManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CWorldManager() override;

public:
	void UpdateGUI();

public:
	void Update(_float fTimeDelta);

public:
	CFurnaceStorage* GetFurnaceStorage() const { return m_pFurnaceStorage.get(); }
	CChestStorage* GetChestStorage() const { return m_pChestStorage.get(); }
public:
	HRESULT Initialize();

private:
	ComPtr<ID3D11Device> m_pDevice{};
	ComPtr<ID3D11DeviceContext> m_pContext{};

private:
	UPtr<CFurnaceStorage> m_pFurnaceStorage{};
	UPtr<CChestStorage> m_pChestStorage{};

public:
	static UPtr<CWorldManager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END