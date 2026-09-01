#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class CResDynamicVIBuffer;
class ENGINE_DLL CDestroyStage : public CGameObject
{
public:
	struct DestroyQuad
	{
		VTX_DESTROY_STAGE v[4]; // 로컬 쿼드 4개 버텍스
	};
public:
	DECLARE_DERIVED_TYPE(CDestroyStage, CGameObject)

private:
	explicit CDestroyStage();
	~CDestroyStage() override;

public:
	HRESULT InitializePrototype(void* pArg = nullptr) override;
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;
	void SetQuads(const std::vector<DestroyQuad>& quads)
	{
		m_quads = quads;
		m_bDirty = true; // 다음 렌더때 VB 업데이트
	}
private:
	void UpdateVertexBuffer(ID3D11DeviceContext* pContext);

private:
	SPtr<CResDynamicVIBuffer> m_pVIBuffer{};
	uint32_t m_currentIndexCount{};

public:
	void SetRender(_bool b) { m_bRender = b; }
	void SetLight(uint32_t light) { m_iLight = light; };
	void SetFrameIndex(uint32_t i) { m_iFrameIndex = std::clamp(i, (uint32_t)0, (uint32_t)9); }
	uint32_t GetFrameIndex() const { return m_iFrameIndex; }

	void MakeCubeQuads(_float3 vOri, _float3 vExt);
private:
	_float3 m_LastOri{};
	_float3 m_LastExt{};
private:
	_bool m_bRender{ false };
	_bool m_bDirty{ true };

private:
	std::vector<DestroyQuad> m_quads{};
	_float m_fElapsed{ 0 };
	uint32_t m_iFrameIndex{ 0 };
	uint32_t m_iLight{ 0xFF };
public:
	static UPtr<CDestroyStage> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END