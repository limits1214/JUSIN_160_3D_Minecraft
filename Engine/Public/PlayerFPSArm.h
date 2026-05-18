#pragma once
#include "ItemObject.h"

NS_BEGIN(Engine)
class CComEntityModel;
class ENGINE_DLL CPlayerFPSArm : public CItemObject
{
public:
	typedef struct tagDesc : CItemObject::DESC
	{
		std::pair<StringID, StringID> viBufferId{};
	}DESC;

public:
	DECLARE_DERIVED_TYPE(CDropItem, CItemObject)

private:
	explicit CPlayerFPSArm();
	~CPlayerFPSArm() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

private:
	//std::pair<StringID, StringID> m_viBufferID{};

	CComEntityModel* m_pComEntityArmModel{};
public:
	static UPtr<CPlayerFPSArm> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END