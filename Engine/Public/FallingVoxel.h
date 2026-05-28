#pragma once
#include "VoxelObject.h"

NS_BEGIN(Engine)
class CResVIBuffer;
class CResDynamicBuffer;
class ENGINE_DLL CFallingVoxel : public CVoxelObject
{
public:
	typedef struct tagDesc : CVoxelObject::DESC
	{
		//std::pair<StringID, StringID> viBufferId{};
	}DESC;

private:
	struct INSTANCE_DATA
	{
		_float4x4 matWorld;
	};
public:
	DECLARE_DERIVED_TYPE(CFallingVoxel, CVoxelObject)

public:
	CFallingVoxel(const CFallingVoxel& rhs);

protected:
	explicit CFallingVoxel();
	~CFallingVoxel() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;
	struct TFallingBlockData
	{
		DirectX::XMFLOAT3 vPos;      // 현재 실시간 부드러운 위치 (float)
		DirectX::XMFLOAT3 vVelocity; // Y축 낙하 속도
		//CBlock3 block{};
		//uint32_t          texIndex;  // 이 모래(혹은 자갈)의 원래 텍스처 인덱스
	};

	struct TSettledBlockData
	{
		DirectX::XMINT3  iTargetPos; // 정착된 최종 정수 좌표 (X, Y, Z)
		DirectX::XMFLOAT3 vRenderPos; // 렌더링에 유지할 float3 위치 (딱 떨어지는 정수 위치)
		bool bDelete{ false };
		uint32_t chunkBufferVersion{  };
	};

public:
	HRESULT AddBlock(const TFallingBlockData& data)
	{
		m_vecFallingBlocks.push_back(data);
		return S_OK;
	}

private:
	std::vector<TFallingBlockData>  m_vecFallingBlocks;         
	std::vector<INSTANCE_DATA>      m_vecInstancedBlockTransform;
	std::vector<TSettledBlockData>   m_vecSettledBlocks;

private:
	uint32_t m_iNumElements{ 1000 };
	uint32_t m_iElementStride{ sizeof(_float4x4) };
	//std::pair<StringID, StringID> m_viBufferID{};
	SPtr<CResVIBuffer> m_pResVIBuffer{};
	SPtr<CResDynamicBuffer> m_pResInstancedBuffer{};

public:
	static UPtr<CFallingVoxel> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END