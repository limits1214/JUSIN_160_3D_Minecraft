#pragma once
#include "EntityObject.h"

NS_BEGIN(Engine)
class CComEntityModel;
class ENGINE_DLL CExperienceOrb : public CEntityObject
{
public:
	typedef struct tagDesc : CEntityObject::DESC
	{

	}DESC;

public:
	DECLARE_DERIVED_TYPE(CExperienceOrb, CEntityObject)

private:
	explicit CExperienceOrb();
	~CExperienceOrb() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

private:
	int m_iFrameIndex = 0;
	int m_iFrameCol{};
	int m_iFrameRow{};
public:
	static UPtr<CExperienceOrb> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END