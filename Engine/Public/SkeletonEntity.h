#pragma once
#include "MonsterEntityObject.h"

NS_BEGIN(Engine)
class CComEntityModel;
class ENGINE_DLL CSkeletonEntity : public CMonsterEntityObject
{
public:
	typedef struct tagDesc : CMonsterEntityObject::DESC
	{

	}DESC;

public:
	DECLARE_DERIVED_TYPE(CSkeletonEntity, CMonsterEntityObject)

private:
	explicit CSkeletonEntity();
	~CSkeletonEntity() override;

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
	static UPtr<CSkeletonEntity> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END