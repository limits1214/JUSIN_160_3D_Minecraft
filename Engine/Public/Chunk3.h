#pragma once
#include "Engine_Defines.h"
#include "Block3.h"

NS_BEGIN(Engine)
class CResCBuffer;
class CResDynamicVIBuffer;

constexpr static uint32_t VOXEL_CHUNK_X_SIZE3 = 32;
constexpr static uint32_t VOXEL_CHUNK_Z_SIZE3 = 32;
constexpr static uint32_t VOXEL_CHUNK_Y_SIZE3 = 256;

class CChunk3 final : public CEngineBase
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
	BUFFER_STATE GetBufferState() const { return m_eBufferState; }
	MESSING_STATE GetMessingState() const { return m_eMessingState; }
	BLOCKFILLING_STATE GetBlockFillingSate() const { return m_eBlockFillingState; }

	std::atomic <BLOCKFILLING_STATE> m_eBlockFillingState{ BLOCKFILLING_STATE::NON };
	std::atomic <BUFFER_STATE> m_eBufferState{ BUFFER_STATE::NON };
	std::atomic<MESSING_STATE>m_eMessingState{ MESSING_STATE::NON };

	uint64_t GetCoordIdx() const { return m_iChunkCoord; }
	std::tuple<int32_t, int32_t, int32_t> GetCoord() const { return { m_iX, m_iY, m_iZ }; }

	static uint32_t BlockIndexing(uint32_t x, uint32_t y, uint32_t z)  { return y + z * VOXEL_CHUNK_Y_SIZE3 + x * VOXEL_CHUNK_Y_SIZE3 * VOXEL_CHUNK_Z_SIZE3; }
	
public:
	CBlock3& GetBlock(uint32_t x, uint32_t y, uint32_t z) { return m_arrBlocks[BlockIndexing(x, y, z)]; }
	void SetBlock(uint32_t x, uint32_t y, uint32_t z, CBlock3::TYPE eType) { m_arrBlocks[BlockIndexing(x, y, z)].SetType(eType); }
	CBlock3& GetBlock(uint32_t idx) { return m_arrBlocks[idx]; }
	void SetBlock(uint32_t idx, CBlock3::TYPE eType) { m_arrBlocks[idx].SetType(eType); }

	HRESULT BlockFilling();
	HRESULT QuadMessing();
	HRESULT CreateBuffer();
	HRESULT Draw(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx) const;
	_bool GetDead() const { return m_bDead; }
	void SetDead() { m_bDead = true; }

	_bool GetMessingQueued() const { return m_bMessingQueued; }
	void SetMessingQueued(_bool b) { m_bMessingQueued = b; }

private:
	void NiveFaceCulling(std::vector<VOX_QUAD>& quads) const;

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
	SPtr<CResDynamicVIBuffer> m_pResDynamicViBuffer{};
	std::vector<E::VTX_VOXEL> m_vertices{};
	std::vector<uint32_t>  m_indices{};
	SPtr<CResCBuffer> m_pResCBufferPerObject{};

public:
	static UPtr<CChunk3> Create(const DESC& desc);
};
NS_END
