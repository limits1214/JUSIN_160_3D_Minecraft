#pragma once
#include "AnimalEntityObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCowEntity : public CAnimalEntityObject
{
public:
	typedef struct tagDesc : CAnimalEntityObject::DESC
	{

	}DESC;

public:
	DECLARE_DERIVED_TYPE(CCowEntity, CAnimalEntityObject)

private:
	explicit CCowEntity();
	~CCowEntity() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

private:
	_float4x4 m_boneMatrices[64]{};
	std::vector<EntityBone> m_Bones{};

public:
	static UPtr<CCowEntity> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END