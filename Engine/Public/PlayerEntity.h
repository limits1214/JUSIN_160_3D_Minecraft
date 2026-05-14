#pragma once
#include "PlayerEntityObject.h"

NS_BEGIN(Engine)
class CComEntityModel;
class ENGINE_DLL CPlayerEntity : public CPlayerEntityObject
{
public:
	typedef struct tagDesc : CPlayerEntityObject::DESC
	{

	}DESC;

public:
	DECLARE_DERIVED_TYPE(CPlayerEntity, CPlayerEntityObject)

private:
	explicit CPlayerEntity();
	~CPlayerEntity() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

private:
private:
	CComEntityModel* m_pComEntityModel{};
public:
	static UPtr<CPlayerEntity> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END