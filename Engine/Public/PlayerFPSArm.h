#pragma once
#include "EntityObject.h"

NS_BEGIN(Engine)
class CComEntityModel;
class ENGINE_DLL CPlayerFPSArm : public CEntityObject
{
public:
	typedef struct tagDesc : CEntityObject::DESC
	{
		std::pair<StringID, StringID> viBufferId{};
	}DESC;

public:
	DECLARE_DERIVED_TYPE(CDropItem, CEntityObject)

private:
	explicit CPlayerFPSArm();
	~CPlayerFPSArm() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

public:
	void SetRender(_bool b) { m_bRender = b; }
private:
	_bool m_bRender{true};
private:

public:
	void SetLight(uint8_t i) { m_iLight = i; }
private:
	uint8_t m_iLight{ 0xFF };
	//std::pair<StringID, StringID> m_viBufferID{};

	CComEntityModel* m_pComEntityArmModel{};
public:
	static UPtr<CPlayerFPSArm> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END