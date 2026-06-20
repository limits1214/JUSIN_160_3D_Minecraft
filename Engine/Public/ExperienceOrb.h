#pragma once
#include "EntityObject.h"

NS_BEGIN(Engine)
class CComEntityModel;
class ENGINE_DLL CExperienceOrb : public CEntityObject
{
public:
	struct InstancedExpOrbDesc
	{
		_float3 vPos{};
		_float3 vVelocity{};
		bool    bOnGround{ false };
		_float  fBobYOffset{};
		_float  fBobYRot{};
		_float  fBobTime{};
		uint32_t iType{};
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
	void AddOrb(const _float3& vPos, const _float3& vVelocity, uint32_t iType);

private:
	void VelocityUpdate(InstancedExpOrbDesc& item, E::_float fTimeDelta);


private:
	_float m_fSpeed{ 5.f };

private:
	std::list<InstancedExpOrbDesc> m_listOrbs{};
	std::vector<VTX_EXP_ORB_INSTANCED_DATA> m_vecInstancedData{};
	uint32_t m_iNumElements{ 100 };
	SPtr<CResDynamicBuffer> m_pResInstancedBuffer{};

public:
	static UPtr<CExperienceOrb> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END