#pragma once
#include "Engine_Defines.h"
#include "Chunk2.h"
#include "IRenderable.h"
#include <FastNoiseLite.h>

NS_BEGIN(Engine)
class CResTexture2DArray;
class CResVertexShader;
class CResPixelShader;
class CResSamplerState;

class CVoxelManager2 final : public CEngineBase, public IRenderable
{
public:
	enum class PROCESS
	{
		CHUNK_IN_RANGE_CREATE,
		CHUNK_OUT_RANGE_RELEASE,
		CHUNK_REBUILD
	};

	enum class PROCESS_CHUNK_IN_RANGE_CREATE_STATE
	{
		NON,
		COLLECT_CANDIDATE,
		BLOCK_FILLING,
		REGIST_NEIGHBOR,
		MESSING,
		BUFFER_CREATE,
	};
	PROCESS_CHUNK_IN_RANGE_CREATE_STATE m_eProcessChunkInRangeCreateState{ PROCESS_CHUNK_IN_RANGE_CREATE_STATE::NON};
	std::future<std::vector<std::pair<uint64_t, UPtr<CChunk2>>>> m_futInRangeChunkCreateCollectCandidate{};
	std::vector<std::future<CChunk2*>> m_futInRangeChunkCreateBlockFillings{};
	//std::future<bool> m_futInRangeChunkCreateRegistNeighbor{};
	std::vector<std::future<CChunk2*>> m_futInRangeChunkCreateMessing{};

	enum class PROCESS_CHUNK_OUT_RANGE_RELEASE_STATE
	{
		NON,
		COLLECT_CANDIDATE,
		UNREGIST_NEIGHBOR,
		MESSING,
		BUFFER_CREATE,
	};
	PROCESS_CHUNK_OUT_RANGE_RELEASE_STATE m_eProcessChunkOutRangeReleaseState{ PROCESS_CHUNK_OUT_RANGE_RELEASE_STATE::NON };
	std::vector<std::future<CChunk2*>> m_futOutRangeChunkReleaseMessing{};

	enum class PROCESS_CHUNK_REBUILD_STATE
	{
		NON,
	};

	typedef struct tagChunkInRangeCreateDesc
	{
		int32_t iCenterX{}, iCenterY{}, iCenterZ{};
	}CHUNK_IN_RANGE_CREATE_DESC;

	typedef struct tagChunkOutRangeReleaseDesc
	{
		int32_t iCenterX{}, iCenterY{}, iCenterZ{};
	}CHUNK_OUT_RANGE_RELEASE_DESC;

	typedef struct tagChunkRebuildDesc
	{

	}CHUNK_REBUILD_DESC;
	std::list<CHUNK_IN_RANGE_CREATE_DESC> m_ChunkInRangeCreateQueue{};
	std::list<CHUNK_OUT_RANGE_RELEASE_DESC> m_ChunkOutRangeReleaseQueue{};
	std::list<CHUNK_REBUILD_DESC> m_ChunkRebuildQueue{};
	std::unordered_set<PROCESS> m_setCurrentProcess{};
	const std::unordered_set<PROCESS>& GetCurrentProcessing() const { return m_setCurrentProcess; };

	HRESULT QueuingChunkInRangeCreate(const CHUNK_IN_RANGE_CREATE_DESC& desc);
	HRESULT QueuingChunkOutRangeRelease(const CHUNK_OUT_RANGE_RELEASE_DESC& desc);
	HRESULT QueuingChunkRebuild(const CHUNK_REBUILD_DESC& desc);

private:
	explicit CVoxelManager2(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CVoxelManager2() override;

public:
	HRESULT Render(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx) override;
	bool HasRenderPass(RENDERPASS ePass) const override { return ePass == RENDERPASS::DEFAULT; };

public:
	void Update(_float fTimeDelta);
	void UpdateGUI();

public:
	uint64_t encodeChunkCoord(int32_t x, int32_t y, int32_t z) const
	{
		// X: 21비트 (±1,048,575 청크 ≈ ±33.5 million 블록)
		// Y: 22비트 (±2,097,151 청크 ≈ ±67 million 블록) → Y축은 보통 더 넓게
		// Z: 21비트

		constexpr int64_t X_OFFSET = 1 << 20;   // 2^20 = 1048576
		constexpr int64_t Y_OFFSET = 1 << 21;
		constexpr int64_t Z_OFFSET = 1 << 20;

		uint64_t ux = static_cast<uint64_t>(x + X_OFFSET) & 0x1FFFFF;   // 21비트
		uint64_t uy = static_cast<uint64_t>(y + Y_OFFSET) & 0x3FFFFF;   // 22비트
		uint64_t uz = static_cast<uint64_t>(z + Z_OFFSET) & 0x1FFFFF;   // 21비트

		return (ux << 43) | (uy << 21) | uz;
	}

	std::tuple<int32_t, int32_t, int32_t> decodeChunkCoord(uint64_t key) const
	{
		constexpr int64_t X_OFFSET = 1 << 20;
		constexpr int64_t Y_OFFSET = 1 << 21;
		constexpr int64_t Z_OFFSET = 1 << 20;

		int32_t x = static_cast<int32_t>((key >> 43) & 0x1FFFFF) - X_OFFSET;
		int32_t y = static_cast<int32_t>((key >> 21) & 0x3FFFFF) - Y_OFFSET;
		int32_t z = static_cast<int32_t>(key & 0x1FFFFF) - Z_OFFSET;

		return { x, y, z };
	}

	CChunk2* GetChunkByChunkCoord(int32_t x, int32_t y, int32_t z)
	{
		auto idx = encodeChunkCoord(x, y, z);
		auto iter = m_mapChucnks.find(idx);
		if (iter == m_mapChucnks.end())
		{
			return nullptr;
		}

		return iter->second.get();
	}

	CChunk2* GetChunkByWorldBlockCoord(int32_t x, int32_t y, int32_t z)
	{
		int32_t cx = (int32_t)floor(x / VOXEL_CHUNK_X_SIZE2);
		int32_t cy = (int32_t)floor(y / VOXEL_CHUNK_Y_SIZE2);
		int32_t cz = (int32_t)floor(z / VOXEL_CHUNK_Z_SIZE2);
		return GetChunkByChunkCoord(cx, cy, cz);
	}


	// DDA(Digital Differential Analyzer)
	struct RaycastResult {
		CChunk2* pChunk{};
		uint32_t iX{}, iY{}, iZ{};// 청크기준
		FACE_DIR eHitFace;
		float fDist{};
	};
	bool RaycastDDA(
		const _float3& rayOrigin,
		const _float3& rayDir,     // normalized
		float fMaxDist,
		RaycastResult& outResult);

private:
	HRESULT Initialize();

private:
	std::unordered_map<uint64_t, UPtr<CChunk2>> m_mapChucnks{};
	int32_t m_iRenderDistance{ 0 };
	int32_t m_iVerticalRenderDistance{ 0 };

public:
	_float GetHeightNoise(_float x, _float z) const { return  m_NoiseHeight.GetNoise(x, z); }
private:
	FastNoiseLite m_NoiseHeight{};

private:
	SPtr<CResTexture2DArray> m_pResBlocksTexutreArray{};
	SPtr<CResSamplerState> m_pResSamplerPointWrap{};
	SPtr<CResPixelShader> m_pResPixelShader{};
	SPtr<CResVertexShader> m_pResVertexShader{};

private:
	ComPtr<ID3D11Device> m_pDevice{};
	ComPtr<ID3D11DeviceContext> m_pContext{};
	std::mutex m_Mutex{};

public:
	static UPtr<CVoxelManager2> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END
