#pragma once
#include "Engine_Defines.h"
#include "IRenderable.h"
#include "Block3.h"
NS_BEGIN(Engine)
class CResDynamicVIBuffer;
class ENGINE_DLL CParticleManager final : public CEngineBase, public IRenderable
{
public:
	enum class PARTICLE_TYPE
	{
		BLOCK_DESTRUCT,
		PARTICLES_ATLAS_SMOKE,
		END
	};

public:
	void AddParticleRenderDestruct(CBlock3 block, _float3 pos, uint32_t iCnt = 1);


	//enum class PARTICLE_TEX_TYPE
	//{
	//	BLOCK_DESTRUCT,
	//	PARTICLES_ATLAS,
	//};

	typedef struct tagAttribute
	{
		_float2 vUv{}; // 시작지점
		_float2 vUvSize{}; // 시작지점으로부터오프셋
		_float2 vSize{};// GS에서  포인트로부터 늘릴 사이즈
		uint32_t iFrameIndex{};
		uint32_t iMaxFrameIndex{};
		_float fFramePlusUnit{};
		uint32_t iTexId{};
		_float3 vPos{};
		_float3 vVelocity{};
		_float3 vAcceleration{};
		_float fLifeTime{};
		_float fAge{};
		_float4 vColor{};
		_bool bAlive{};
	} ATTRIBUTE;

private:
	explicit CParticleManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CParticleManager() override;

public:
	void UpdateGUI();


public:
	HRESULT RenderX(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx) ;
	HRESULT Render(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx) override;
	bool HasRenderPass(RENDERPASS ePass) const override { return ePass == RENDERPASS::DEFAULT; };

public:

public:
	HRESULT AddParticle(PARTICLE_TYPE eType, const ATTRIBUTE& particle);
	void Update(_float fTimeDelta);

private:
	HRESULT Initialize();

private:
	ComPtr<ID3D11Device> m_pDevice{};
	ComPtr<ID3D11DeviceContext> m_pContext{};

private:
	static constexpr uint32_t PARTICLE_MAX_CNT{ 5000 };
	SPtr<CResDynamicVIBuffer> m_pResVIBuffer{};

	std::array<std::vector<ATTRIBUTE>, ETOUI(PARTICLE_TYPE::END)> m_arrParticles{};

	std::array<std::vector<VTX_POINT_PARTICLE>, ETOUI(PARTICLE_TYPE::END)> m_arrVertices{};

	//uint32_t m_iVbOffset{ 0 };
	//const uint32_t m_iVbBatchSize{ 500 };

private:
	//std::vector<ATTRIBUTE> m_vecAttributes{};
	//std::array<VTX_POINT_PARTICLE, PARTICLE_MAX_CNT> m_arrVertices{};

public:
	static UPtr<CParticleManager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END