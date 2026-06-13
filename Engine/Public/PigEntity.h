#pragma once
#include "AnimalEntityObject.h"

NS_BEGIN(Engine)
class CComEntityModel;
class CComAnimator;
class CComConstantBuffer;
class ENGINE_DLL CPigEntity : public CAnimalEntityObject
{
public:
	typedef struct tagDesc : CAnimalEntityObject::DESC
	{

	}DESC;

public:
	DECLARE_DERIVED_TYPE(CPigEntity, CAnimalEntityObject)

private:
	explicit CPigEntity();
	~CPigEntity() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

public:
	void SetPlayer(CHandle h) { m_hPlayer = h; }

private:
	CHandle m_hPlayer{};

private:
	CComEntityModel* m_pComEntityModel{};
	CComAnimator* m_pComAnimator{ };
	CComConstantBuffer* m_pComCBufferPerObject{};

public:
	static UPtr<CPigEntity> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END