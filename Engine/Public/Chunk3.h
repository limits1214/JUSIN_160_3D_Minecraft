#pragma once
#include "Engine_Defines.h"
#include "Block3.h"

NS_BEGIN(Engine)
class CResCBuffer;
class CResDynamicVIBuffer;
class CCollBox;

constexpr static uint32_t VOXEL_CHUNK_X_SIZE3 = 32;
constexpr static uint32_t VOXEL_CHUNK_Z_SIZE3 = 32;
constexpr static uint32_t VOXEL_CHUNK_Y_SIZE3 = 256;

class ENGINE_DLL CChunk3 final : public CEngineBase
{
public:
	typedef struct tagDesc
	{
		int32_t iX{}, iY{}, iZ{};
		uint64_t iChunkCoord{};
	} DESC;

	enum class BLOCKFILLING_STATE
	{
		NON, ING, DONE
	};

	enum class BUFFER_STATE
	{
		NON, ING, DONE
	};

	enum class MESSING_STATE
	{
		NON, ING, DONE
	};
	
	enum class LIGHTING_STATE
	{
		NON, ING, DONE
	};

	BUFFER_STATE GetBufferState() const { return m_eBufferState; }
	MESSING_STATE GetMessingState() const { return m_eMessingState; }
	BLOCKFILLING_STATE GetBlockFillingSate() const { return m_eBlockFillingState; }
	LIGHTING_STATE GetLightingState() const { return m_eLightingState; }
	void SetLightingState(LIGHTING_STATE eState) { m_eLightingState = eState; }

	std::atomic <BLOCKFILLING_STATE> m_eBlockFillingState{ BLOCKFILLING_STATE::NON };
	std::atomic <BUFFER_STATE> m_eBufferState{ BUFFER_STATE::NON };
	std::atomic<MESSING_STATE>m_eMessingState{ MESSING_STATE::NON };
	std::atomic<LIGHTING_STATE> m_eLightingState{ LIGHTING_STATE::NON };

	uint64_t GetCoordIdx() const { return m_iChunkCoord; }
	std::tuple<int32_t, int32_t, int32_t> GetCoord() const { return { m_iX, m_iY, m_iZ }; }

	static uint32_t BlockIndexing(uint32_t x, uint32_t y, uint32_t z)  { return y + z * VOXEL_CHUNK_Y_SIZE3 + x * VOXEL_CHUNK_Y_SIZE3 * VOXEL_CHUNK_Z_SIZE3; }
	static std::tuple<uint32_t, uint32_t, uint32_t> BlockIndexDecoding(uint32_t idx)
	{
		constexpr uint32_t yzStride =
			VOXEL_CHUNK_Y_SIZE3 * VOXEL_CHUNK_Z_SIZE3;

		uint32_t x = static_cast<uint32_t>(idx / yzStride);

		idx %= yzStride;

		uint32_t z = static_cast<uint32_t>(idx / VOXEL_CHUNK_Y_SIZE3);

		uint32_t y = static_cast<uint32_t>(idx % VOXEL_CHUNK_Y_SIZE3);

		return { x, y, z };
	}

public:
	void CollectLightingSeeds(
		std::queue<std::pair<XMINT3, uint8_t>>& skyLightSeedQ,
		std::queue<std::pair<XMINT3, uint8_t>>& blockLightSeedQ
	);
public:
	CBlock3& GetBlock(uint32_t x, uint32_t y, uint32_t z) { return m_arrBlocks[BlockIndexing(x, y, z)]; }
	void SetBlock(uint32_t x, uint32_t y, uint32_t z, CBlock3::TYPE eType) { m_arrBlocks[BlockIndexing(x, y, z)].SetType(eType); }
	CBlock3& GetBlock(uint32_t idx) { return m_arrBlocks[idx]; }
	void SetBlock(uint32_t idx, CBlock3::TYPE eType) { m_arrBlocks[idx].SetType(eType); }
	void SetBlock(uint32_t idx, CBlock3 block) { m_arrBlocks[idx] = block; }


	HRESULT InitialChunkLighting();
	void InitialChunkFloodFillSkyLighting(std::queue<XMINT3>& q);
	void InitialChunkFloodFillBlockLighting(std::queue<XMINT3>& q);

	HRESULT BlockFilling();
	HRESULT QuadMessing();
	HRESULT CreateBuffer();
	HRESULT DrawSolid(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx) const;
	HRESULT DrawWater(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx) const;
	_bool GetDead() const { return m_bDead; }
	void SetDead() { m_bDead = true; }

	_bool GetMessingQueued() const { return m_bMessingQueued; }
	void SetMessingQueued(_bool b) { m_bMessingQueued = b; }

public:
	void Update(_float fTimeDelta);

private:
	void NiveFaceCulling(std::vector<VOX_QUAD>& quads) const;
	_bool IsInsideOpaque(int32_t x, int32_t y, int32_t z,
		CChunk3* pPX, CChunk3* pMX, CChunk3* pPZ, CChunk3* pMZ) const;

	uint8_t CalculateVertexAO(_bool side1, _bool side2, _bool corner) const;
private:
	CChunk3();
	~CChunk3() override;
	
private:
	HRESULT Initialize(const DESC& desc);
private:
	int32_t m_iX{}, m_iY{}, m_iZ{};
	uint64_t m_iChunkCoord{};
	std::atomic<_bool> m_bDead{ false };
	std::atomic<_bool> m_bMessingQueued{ false };

private:
	std::array<CBlock3, VOXEL_CHUNK_X_SIZE3* VOXEL_CHUNK_Z_SIZE3* VOXEL_CHUNK_Y_SIZE3> m_arrBlocks{};

private:
	SPtr<CResDynamicVIBuffer> m_pResSolidDynamicViBuffer{};
	std::vector<E::VTX_VOXEL> m_SolidVertices{};
	std::vector<uint32_t>  m_SolidIndices{};
	SPtr<CResCBuffer> m_pResCBufferPerObject{};

public:
	CCollBox* GetCollBox() const { return m_pCollBox.get(); }
private:
	UPtr<CCollBox> m_pCollBox{};

public:
	static UPtr<CChunk3> Create(const DESC& desc);
};
NS_END
