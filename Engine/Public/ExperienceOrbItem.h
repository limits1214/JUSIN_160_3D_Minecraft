#pragma once
#include "ItemObject.h"

NS_BEGIN(Engine)
class CComEntityModel;
class ENGINE_DLL CExperienceOrbItem : public CItemObject
{
public:
	typedef struct tagDesc : CItemObject::DESC
	{

	}DESC;

public:
	DECLARE_DERIVED_TYPE(CExperienceOrbItem, CItemObject)

private:
	explicit CExperienceOrbItem();
	~CExperienceOrbItem() override;

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
	static UPtr<CExperienceOrbItem> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END