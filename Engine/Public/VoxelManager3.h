#pragma once
#include "Engine_Defines.h"
#include "Chunk3.h"
#include "IRenderable.h"
#include <FastNoiseLite.h>

NS_BEGIN(Engine)
class CResTexture2DArray;
class CResVertexShader;
class CResPixelShader;
class CResSamplerState;

class ENGINE_DLL CVoxelManager3 final : public CEngineBase, public IRenderable
{
public:
	typedef struct tagInRangeChunkCreateDesc
	{
		int32_t iCenterX{}, iCenterY{}, iCenterZ{};
		//_bool bIncrementCreate{ false };

	}IN_RANGE_CHUNK_CREATE_DESC;

	typedef struct tagOutRangeChunkReleaseDesc
	{
		int32_t iCenterX{}, iCenterY{}, iCenterZ{};
	}OUT_RANGE_CHUNK_RELEASE_DESC;

	//typedef struct tagChunkEditDesc
	//{
	//	int32_t iWorldBlockX{};
	//	int32_t iWorldBlockY{};
	//	int32_t iWorldBlockZ{};
	//	CBlock3 block{};
	//}CHUNK_EDIT_DESC;

	std::unordered_map<uint64_t, std::unordered_map<uint32_t, CBlock3>> m_EditShadow{};

	typedef struct tagRaycastResult {
		//CChunk3* pChunk{};
		int32_t iWorldBlockX{}, iWorldBlockY{}, iWorldBlockZ{};
		int32_t iChunkX{}, iChunkY{}, iChunkZ{};
		uint32_t iChunkBlockX{}, iChunkBlockY{}, iChunkBlockZ{};
		std::optional<CBlock3> block{};
		FACE_DIR eHitFace;
		float fDist{};
	} BLOCK_RAY_RESULT;
public:
	std::optional< CBlock3> GetBlock(int32_t wbx, int32_t wby, int32_t wbz) const;
	std::optional< CBlock3> GetBlockByChunkCoord(uint64_t chunkIdx ,int32_t cbx, int32_t cby, int32_t cbz) const;
	void SetBlock(int32_t wbx, int32_t wby, int32_t wbz, CBlock3 block);
	void SetBlocks(std::vector<std::tuple<int32_t, int32_t, int32_t, CBlock3>>);
	_bool BlockRaycast(const _float3& rayOrigin,
		const _float3& rayDir,     // normalized
		float fMaxDist,
		BLOCK_RAY_RESULT& outResult) const;

	bool VoxelAABBOverlap(const _float3& pos, const _float3& halfExtents) const
	{
		// AABB가 차지하는 블록 범위
		int xMin = (int)floorf(pos.x - halfExtents.x);
		int xMax = (int)floorf(pos.x + halfExtents.x);
		int yMin = (int)floorf(pos.y - halfExtents.y);
		int yMax = (int)floorf(pos.y + halfExtents.y);
		int zMin = (int)floorf(pos.z - halfExtents.z);
		int zMax = (int)floorf(pos.z + halfExtents.z);

		for (int x = xMin; x <= xMax; x++)
			for (int y = yMin; y <= yMax; y++)
				for (int z = zMin; z <= zMax; z++)
				{
					auto block = GetBlock(x, y, z);
					if (block && block->GetType() != CBlock3::TYPE::AIR)
						return true;
				}
		return false;
	}


	struct AABB_OVERLAP_RESULT
	{
		bool        bOverlap = false;
		CBlock3     block{};
		int         x{}, y{}, z{};  // 충돌한 블록 좌표
	};

	AABB_OVERLAP_RESULT VoxelAABBOverlapResult(const _float3& pos, const _float3& halfExtents) const
	{
		int xMin = (int)floorf(pos.x - halfExtents.x);
		int xMax = (int)floorf(pos.x + halfExtents.x);
		int yMin = (int)floorf(pos.y - halfExtents.y);
		int yMax = (int)floorf(pos.y + halfExtents.y);
		int zMin = (int)floorf(pos.z - halfExtents.z);
		int zMax = (int)floorf(pos.z + halfExtents.z);

		for (int x = xMin; x <= xMax; x++)
			for (int y = yMin; y <= yMax; y++)
				for (int z = zMin; z <= zMax; z++)
				{
					auto block = GetBlock(x, y, z);
					if (!block) continue;

					switch (block->GetType())
					{
					case CBlock3::TYPE::AIR:
					//case CBlock3::TYPE::WATER:   // 물은 통과
					//case CBlock3::TYPE::LAVA:    // 용암도 통과 (데미지는 별도)
						continue;

					default:  // 일반 솔리드 블록
						return { true, *block, x, y, z };
					}
				}
		return { false };
	}

	bool VoxelAABBContainsType(const _float3& pos, const _float3& halfExtents, CBlock3::TYPE type) const
	{
		int xMin = (int)floorf(pos.x - halfExtents.x);
		int xMax = (int)floorf(pos.x + halfExtents.x);
		int yMin = (int)floorf(pos.y - halfExtents.y);
		int yMax = (int)floorf(pos.y + halfExtents.y);
		int zMin = (int)floorf(pos.z - halfExtents.z);
		int zMax = (int)floorf(pos.z + halfExtents.z);

		for (int x = xMin; x <= xMax; x++)
			for (int y = yMin; y <= yMax; y++)
				for (int z = zMin; z <= zMax; z++)
				{
					auto block = GetBlock(x, y, z);
					if (block && block->GetType() == type)
						return true;
				}
		return false;
	}
public:
	HRESULT QueuingInRangeChunkCreate(const IN_RANGE_CHUNK_CREATE_DESC& desc);
	HRESULT QueuingOutRangeChunkRelease(const OUT_RANGE_CHUNK_RELEASE_DESC& desc);

private:
	std::list<IN_RANGE_CHUNK_CREATE_DESC> m_queueInRangeChunkCreate{};
	std::list<OUT_RANGE_CHUNK_RELEASE_DESC> m_queueOutRangeChunkRelease{};
	//std::list<CHUNK_EDIT_DESC> m_queueChunkEdit{};

	std::list<std::vector<uint64_t>> m_queuedQuadMessingChunks{};

	//std::list<std::future<CHUNK_EDIT_DESC>> m_queueFutEdit{};
	std::list<std::vector<std::future<uint64_t>>> m_queueFutBlockFilling{};
	std::list<std::future<std::vector<uint64_t>>> m_queueFutQuadMessing{};


private:
	explicit CVoxelManager3(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CVoxelManager3() override;

public:
	HRESULT Render(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx) override;
	bool HasRenderPass(RENDERPASS ePass) const override { return ePass == RENDERPASS::DEFAULT; };

public:
	void Update(_float fTimeDelta);
	void UpdateGUI();

public:
	static uint64_t encodeChunkCoord(int32_t x, int32_t y, int32_t z) ;
	static std::tuple<int32_t, int32_t, int32_t> decodeChunkCoord(uint64_t key) ;

//private:
	CChunk3* GetChunkByChunkCoord(int32_t x, int32_t y, int32_t z) const;
	CChunk3* GetChunkByWorldBlockCoord(int32_t x, int32_t y, int32_t z) const;

private:
	HRESULT StartProcessInRangeChunkCreate(const IN_RANGE_CHUNK_CREATE_DESC& createDesc);
	HRESULT StartProcessOutRangeChunkRelease(const OUT_RANGE_CHUNK_RELEASE_DESC& releaseDesc);
	HRESULT UpdateCheckBlockFillingFutures();
	HRESULT UpdateCheckQuduedQuadMessingChunk();
	HRESULT UpdateCheckQuadMessingEndFutures();
	HRESULT UpdateCheckBlockEdit();

	//HRESULT AdjChunkReMessing(uint64_t targetIdx);
	HRESULT QueueingQuadMessing(std::vector<uint64_t> targetCoords, _bool bPushFront = false);
private:
	HRESULT Initialize();

private:
	std::unordered_map<uint64_t, UPtr<CChunk3>> m_mapChunks{};
	int32_t m_iRenderDistance{ 1 };
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
	//std::mutex m_Mutex{};
	std::atomic<_bool> m_bCreating{ false };

private:
	_bool m_bDbgBlockPicking{ false };
	_bool m_bDbgPicking{ false };

public:
	static UPtr<CVoxelManager3> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END
