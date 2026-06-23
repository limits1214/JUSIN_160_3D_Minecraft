#pragma once
#include "VoxelObject.h"


NS_BEGIN(Engine)
class CResVIBuffer;
class CResDynamicBuffer;
class ENGINE_DLL CActivatedTNT : public CVoxelObject
{
public:
	static void ExplodeAndDamageColliding(_float3 vPos, _float fExplodeRadisu, uint32_t iExplodeSmokeParticleCnt);
public:
	typedef struct tagDesc : CVoxelObject::DESC
	{
		//std::pair<StringID, StringID> viBufferId{};
	}DESC;

private:
	struct INSTANCE_DATA
	{
		_float4x4 matWorld;
		_float4 vColor{1.f, 1.f, 1.f, 1.f};
	};

public:
	struct SActivatedTNTData
	{
		_float3 vPos;
		_float3 vVelocity;
		_float fElapsedTime{};
		_float fColorTimer{};
		_bool bOnGround{ false };
		_bool fColorwhite{false};
		_float fTargetFuseTime{4.f};
	};
public:
	DECLARE_DERIVED_TYPE(CActivatedTNT, CVoxelObject)

public:
	CActivatedTNT(const CActivatedTNT& rhs);

protected:
	explicit CActivatedTNT();
	~CActivatedTNT() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;


public:
	HRESULT AddBlock( SActivatedTNTData& data)
	{
		data.fTargetFuseTime = 3.5f + (rand() % 100 / 100.f);
		m_vecActivatedTNT.push_back(data);
		return S_OK;
	}

private:
	void VelocityUpdate(_float fTimeDelta, SActivatedTNTData& data);

private:
	std::list<SActivatedTNTData>  m_vecActivatedTNT;
	std::vector<INSTANCE_DATA>      m_vecInstancedBlockTransform;
	//std::vector<TSettledBlockData>   m_vecSettledBlocks;

private:
	uint32_t m_iNumElements{ 1000 };
	uint32_t m_iElementStride{ sizeof(INSTANCE_DATA) };
	//std::pair<StringID, StringID> m_viBufferID{};
	SPtr<CResVIBuffer> m_pResVIBuffer{};
	SPtr<CResDynamicBuffer> m_pResInstancedBuffer{};

public:
	static UPtr<CActivatedTNT> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END