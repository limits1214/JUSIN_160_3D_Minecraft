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


private:
	CChunk2();
	~CChunk2() override;

public:
	
public:

private:
	HRESULT Initialize(const DESC& desc);
private:
	int32_t m_iX{}, m_iY{}, m_iZ{};
	uint64_t m_iChunkCoord{};

private:
	std::array<CBlock2, VOXEL_CHUNK_X_SIZE2 * VOXEL_CHUNK_Z_SIZE2 * VOXEL_CHUNK_Y_SIZE2> m_arrBlocks{};






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
