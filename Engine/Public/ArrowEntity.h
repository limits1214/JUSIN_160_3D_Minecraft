#pragma once
#include "EntityObject.h"

NS_BEGIN(Engine)
class CComEntityModel;
class CComAnimator;
class CComConstantBuffer;
class ENGINE_DLL CArrowEntity : public CEntityObject
{
public:
	typedef struct tagDesc : CEntityObject::DESC
	{
	}DESC;

public:
	DECLARE_DERIVED_TYPE(CArrowEntity, CEntityObject)

private:
	explicit CArrowEntity();
	~CArrowEntity() override;

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
	void SetRender(_bool b) { m_bRender = b; }
private:
	_bool m_bRender{ true };

public:
	static UPtr<CArrowEntity> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END