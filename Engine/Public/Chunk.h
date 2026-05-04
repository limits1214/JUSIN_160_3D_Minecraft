#pragma once
#include "Engine_Defines.h"
#include "Block.h"

NS_BEGIN(Engine)

constexpr static uint32_t VOXEL_CHUNK_X_SIZE = 32;
constexpr static uint32_t VOXEL_CHUNK_Z_SIZE = 32;
constexpr static uint32_t VOXEL_CHUNK_Y_SIZE = 32;

class CChunk final: public CEngineBase
{
public:
	typedef struct tagDesc
	{
		int32_t iX{}, iY{}, iZ{};
		uint64_t iChunkCoord{};
	} DESC;

private:
	CChunk();
	~CChunk() override;

public:
	_bool GetLoaded() const { return m_bLoaded; }
	std::vector<VOX_QUAD> GenerateQuad();
	uint32_t BlockIndexing(uint32_t x, uint32_t y, uint32_t z) const
	{
		return y + z * VOXEL_CHUNK_Y_SIZE + x *VOXEL_CHUNK_Y_SIZE * VOXEL_CHUNK_Z_SIZE;
	}
	Block& GetBlock(uint32_t x, uint32_t y, uint32_t z)
	{
		return m_arrBlocks[BlockIndexing(x, y, z)];
	}
private:
	void BlockGenerate();
	void FaceCulling(std::vector<VOX_QUAD>& quads);
	void GreedyCulling(std::vector<VOX_QUAD>& quads);
	void NoCulling(std::vector<VOX_QUAD>& quads);
public:
	HRESULT BufferLoad(std::mutex& m_Mutex);
	void BindBuffer(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx);
	_bool QuadCalc();
	HRESULT MapBuffer(ID3D11DeviceContext* pContext);

private:
	HRESULT Initialize(const DESC& desc);
	
private:
	//x,z,y
	_string m_sResName{};
	std::array<Block, VOXEL_CHUNK_X_SIZE* VOXEL_CHUNK_Z_SIZE* VOXEL_CHUNK_Y_SIZE> m_arrBlocks{};
	SPtr<CResDynamicVIBuffer> m_pResDynamicViBuffer{};
	int32_t m_iX{}, m_iY{}, m_iZ{};
	uint64_t m_iChunkCoord{};

	std::vector<VOX_QUAD> m_quads{};
	std::vector<E::VTX_VOXEL> m_vertices{};
	std::vector<uint32_t>  m_indices{};
	uint32_t m_iNumIndices{};

	_bool m_bLoaded{ false };

private:
	SPtr<CResCBuffer> m_pResCBufferPerObject{};

public:
	static UPtr<CChunk> Create(const DESC& desc);

public:
	void Free() override;
};
NS_END
