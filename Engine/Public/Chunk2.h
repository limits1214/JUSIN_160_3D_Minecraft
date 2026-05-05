#pragma once
#include "Engine_Defines.h"
#include "Block2.h"

NS_BEGIN(Engine)
class CResCBuffer;
class CResDynamicVIBuffer;

constexpr static uint32_t VOXEL_CHUNK_X_SIZE2 = 32;
constexpr static uint32_t VOXEL_CHUNK_Z_SIZE2 = 32;
constexpr static uint32_t VOXEL_CHUNK_Y_SIZE2 = 256;

class CChunk2 final : public CEngineBase
{
public:
	typedef struct tagDesc
	{
		int32_t iX{}, iY{}, iZ{};
		uint64_t iChunkCoord{};
	} DESC;

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
	BUFFER_STATE m_eBufferState{ BUFFER_STATE::NON };
	MESSING_STATE m_eMessingState{ MESSING_STATE::NON };

	uint64_t GetCoordIdx() const { return m_iChunkCoord; }
	std::tuple<uint32_t, uint32_t, uint32_t> GetCoord() const { return { m_iX, m_iY, m_iZ }; }
private:
	CChunk2();
	~CChunk2() override;

public:
	uint32_t BlockIndexing(uint32_t x, uint32_t y, uint32_t z) const { return y + z * VOXEL_CHUNK_Y_SIZE2 + x * VOXEL_CHUNK_Y_SIZE2 * VOXEL_CHUNK_Z_SIZE2; }
public:
	HRESULT BlockFilling();
	HRESULT Messing();
	HRESULT GenBuffer();
	HRESULT Draw(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx);
private:
	void FaceCulling(std::vector<VOX_QUAD>& quads);
	HRESULT Initialize(const DESC& desc);
private:
	int32_t m_iX{}, m_iY{}, m_iZ{};
	uint64_t m_iChunkCoord{};

private:
	std::array<CBlock2, VOXEL_CHUNK_X_SIZE2 * VOXEL_CHUNK_Z_SIZE2 * VOXEL_CHUNK_Y_SIZE2> m_arrBlocks{};


public:
	HRESULT SetNeighborChunk(CChunk2* pChunk, FACE_DIR eDir)
	{
		m_arrNeighborChunks[ETOUI(eDir)] = pChunk;
		return S_OK;
	}
private:
	std::array<CChunk2*, ETOUI(FACE_DIR::END)> m_arrNeighborChunks{};



private:
	SPtr<CResDynamicVIBuffer> m_pResDynamicViBuffer{};
	std::vector<E::VTX_VOXEL> m_vertices{};
	std::vector<uint32_t>  m_indices{};
	SPtr<CResCBuffer> m_pResCBufferPerObject{};

public:
	static UPtr<CChunk2> Create(const DESC& desc);

public:
	void Free() override;
};
NS_END
