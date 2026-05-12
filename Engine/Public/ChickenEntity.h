#pragma once
#include "AnimalEntityObject.h"

NS_BEGIN(Engine)
class CComEntityModel;
class ENGINE_DLL CChickenEntity : public CAnimalEntityObject
{
public:
	typedef struct tagDesc : CAnimalEntityObject::DESC
	{

	}DESC;

public:
	DECLARE_DERIVED_TYPE(CChickenEntity, CAnimalEntityObject)

private:
	explicit CChickenEntity();
	~CChickenEntity() override;

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
	static UPtr<CChickenEntity> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END