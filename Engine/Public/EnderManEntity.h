#pragma once
#include "MonsterEntityObject.h"

NS_BEGIN(Engine)
class CComEntityModel;
class CComAnimator;
class CComConstantBuffer;
class ENGINE_DLL CEnderManEntity : public CMonsterEntityObject
{
public:
	typedef struct tagDesc : CMonsterEntityObject::DESC
	{

	}DESC;

public:
	DECLARE_DERIVED_TYPE(CEnderManEntity, CMonsterEntityObject)

private:
	explicit CEnderManEntity();
	~CEnderManEntity() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

private:
	CComEntityModel* m_pComEntityModel{};
	CComAnimator* m_pComAnimator{ };
	CComConstantBuffer* m_pComCBufferPerObject{};

public:
	static UPtr<CEnderManEntity> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END