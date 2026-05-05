#pragma once
#include "Engine_Defines.h"
#include "Block.h"
#include "Chunk.h"
#include "IRenderable.h"
#include <FastNoiseLite.h>

NS_BEGIN(Engine)
class CResTexture2DArray;

class CVoxelManager final: public CEngineBase, public IRenderable
{
//public:
//	CVoxelManager(const CVoxelManager& rhs) = delete;
//	CVoxelManager& operator=(const CVoxelManager&) = delete;
public:
	typedef struct tagWolrdCreateDesc
	{
		uint32_t iRenderDistance{ 1 };

	}WORLD_CREATE_DESC;

private:
	explicit CVoxelManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CVoxelManager() override;

public:
	HRESULT Render(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx) override;
	bool HasRenderPass(RENDERPASS ePass) const override { return ePass == RENDERPASS::DEFAULT; };

public:
	void UpdateGUI();
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

public:
	void Update(_float fTimeDelta);
	CChunk* GetChunk(int32_t x, int32_t y, int32_t z) ;
	void StateUpdate(const VOXEL_MANAGER_STATE_UPDATE_DESC& desc);
	void SetChunkLoadCenter(int32_t x, int32_t y, int32_t z);
	void WorldCreate();
	void WorldDestroy();

private:
	HRESULT ChunkLoad(int32_t x, int32_t y, int32_t z);

private:
	HRESULT Initialize();

private:
	std::unordered_map<int64_t, UPtr<CChunk>> m_mapChucnks{};
	int32_t m_iRenderDistance{ 1 };
	int32_t m_iVerticalRenderDistance{0};

	std::vector<int64_t> m_ChunkLoadPending{};
	uint32_t m_iEnqueuedCnt{};


	std::vector<std::future<uint64_t>> m_ChunkLoadFutures{};


public:
	_float GetHeightNoise(_float x, _float z) const
	{
		return  m_NoiseHeight.GetNoise(x, z);
	}

private:
	FastNoiseLite m_NoiseHeight{};
	SPtr<CResTexture2DArray> m_pResBlocksTexutreArray{};
	SPtr<CResSamplerState> m_pResSamplerPointWrap{};

private:
	ComPtr<ID3D11Device> m_pDevice{};
	ComPtr<ID3D11DeviceContext> m_pContext{};
	std::mutex m_Mutex{};

public:
	static UPtr<CVoxelManager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END
