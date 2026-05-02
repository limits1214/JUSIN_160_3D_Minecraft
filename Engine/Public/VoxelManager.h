#pragma once
#include "Engine_Defines.h"
#include "Block.h"
#include "Chunk.h"

NS_BEGIN(Engine)


class CVoxelManager final: public CEngineBase
{
public:

private:
	explicit CVoxelManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CVoxelManager() override;

public:
	void UpdateGUI();

private:

	std::unordered_map<CChunk::ChunkCoord, CChunk, CChunk::ChunkCoordHasher> chunkMap;


private:
	ComPtr<ID3D11Device> m_pDevice{};
	ComPtr<ID3D11DeviceContext> m_pContext{};

public:
	static UPtr<CVoxelManager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END
