#pragma once
#include "Engine_Defines.h"
#include "Block.h"
#include "Chunk.h"
#include "IRenderable.h"
#include <FastNoiseLite.h>

NS_BEGIN(Engine)


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
	uint64_t  encodeChunkCoord(int32_t x, int32_t z) const
	{
		return static_cast<uint64_t>(static_cast<uint32_t>(x)) << 32 | static_cast<uint32_t>(z);
	}
	std::pair<int32_t, int32_t> decodeChunkCoord(uint64_t chunkCoord) const
	{
		int32_t x = static_cast<int32_t>(chunkCoord >> 32);
		int32_t z = static_cast<int32_t>(chunkCoord & 0xFFFFFFFF);
		return { x, z };
	}

public:
	void Update(_float fTimeDelta);
	void StateUpdate(const VOXEL_MANAGER_STATE_UPDATE_DESC& desc);
	void SetChunkLoadCenter(int32_t x, int32_t z);
	void WorldCreate();
	void WorldDestroy();

private:
	HRESULT ChunkLoad(int32_t x, int32_t z);

private:
	HRESULT Initialize();

private:
	std::unordered_map<int64_t, UPtr<CChunk>> m_mapChucnks{};
	int32_t m_iRenderDistance{ 1 };

	std::vector<int64_t> m_ChunkLoadPending{};
	uint32_t m_iEnqueuedCnt{};


public:
	_float GetHeightNoise(_float x, _float z) const
	{
		return  m_NoiseHeight.GetNoise((float)x, (float)z);
	}

private:
	FastNoiseLite m_NoiseHeight{};

private:
	ComPtr<ID3D11Device> m_pDevice{};
	ComPtr<ID3D11DeviceContext> m_pContext{};
	std::mutex m_Mutex{};

public:
	static UPtr<CVoxelManager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END
