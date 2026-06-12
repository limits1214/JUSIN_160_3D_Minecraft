#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class ENGINE_DLL CSkybox : public CGameObject
{
public:
	DECLARE_DERIVED_TYPE(CSkybox, CGameObject)

private:
	explicit CSkybox();
	~CSkybox() override;

public:
	HRESULT InitializePrototype(void* pArg = nullptr) override;
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

private:
	_bool m_bRender{ true };

public:
	static UPtr<CSkybox> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END