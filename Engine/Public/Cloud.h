#pragma once
#include "GameObject.h"
#include "Timer.h"

NS_BEGIN(Engine)
class CResDynamicBuffer;
class ENGINE_DLL CCloud : public CGameObject
{
public:
	DECLARE_DERIVED_TYPE(CCloud, CGameObject)

private:
	explicit CCloud();
	CCloud(const CCloud& rhs) ;
	~CCloud() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

private:
	_bool m_bRender{ true };


private:
	struct InstanceData
	{
		XMFLOAT4X4 matWorld;
	};


private:
	uint32_t m_iNumElements = 16384;
	SPtr<CResDynamicBuffer> m_pResInstancedBuffer{};

	SPtr<CResCloudVIBuffer> m_pResVIBuffer{  };
	std::vector<XMFLOAT3> m_CloudPositions;

private:
	_float m_fCloudOffset{ 0.0f };
	_float m_fCloudSpeed{ 2.0f };
	std::vector<InstanceData> m_InstanceData;

private:
	std::future<std::vector<CCloud::InstanceData>> m_futCloudCalc{};
	CTimer m_TimerForCloudCalc{};

public:
	static UPtr<CCloud> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END