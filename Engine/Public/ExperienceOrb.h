#pragma once
#include "EntityObject.h"

NS_BEGIN(Engine)
class CComConstantBuffer;
class ENGINE_DLL CExperienceOrb : public CEntityObject
{
public:
	struct InstancedExpOrbDesc
	{
		_float3 vPos{};
		_float3 vVelocity{};
		bool    bOnGround{ false };
		//_float  fBobYOffset{};
		//_float  fBobYRot{};
		_float  fBobTime{};
		uint32_t iType{};
		_float fExp{};
		SPtr<CCollider> boxCollider{};
		_float4x4 matWorld{};
	};
	struct CollHint
	{
		std::list<InstancedExpOrbDesc>::iterator iter;
	};
public:
	typedef struct tagDesc : CEntityObject::DESC
	{
		CHandle hPlayer{};
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
	void AddOrb(const _float3& vPos, const _float3& vVelocity, uint32_t iType, _float fExp);

private:
	//void VelocityUpdate(InstancedExpOrbDesc& item, E::_float fTimeDelta);

	void VelocityUpdate(InstancedExpOrbDesc& item, E::_float fTimeDelta, _bool bMagnet, XMVECTOR vToPlayer);

private:
	_float m_fSpeed{ 5.f };

public:
	std::list<InstancedExpOrbDesc>& GetExpOrbObjects() { return m_listOrbs; }
private:
	CComConstantBuffer* m_pComCBufferPerObject{};
	std::list<InstancedExpOrbDesc> m_listOrbs{};
	std::vector<VTX_EXP_ORB_INSTANCED_DATA> m_vecInstancedData{};
	uint32_t m_iNumElements{ 500 };
	SPtr<CResDynamicBuffer> m_pResInstancedBuffer{};


public:
	void SetPlayerHandle(CHandle h) { m_hPlayer = h; }
private:
	CHandle m_hPlayer{};

public:
	static UPtr<CExperienceOrb> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END