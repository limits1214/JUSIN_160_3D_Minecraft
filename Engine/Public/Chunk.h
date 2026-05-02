#pragma once
#include "Engine_Defines.h"
#include "Block.h"

NS_BEGIN(Engine)

constexpr static uint32_t VOXEL_CHUNK_X_SIZE = 16;
constexpr static uint32_t VOXEL_CHUNK_Y_SIZE = 16;
constexpr static uint32_t VOXEL_CHUNK_Z_SIZE = 256;

class CChunk
{
public:
	struct ChunkCoord {
		int x, y, z;
		bool operator==(const ChunkCoord& other) const {
			return x == other.x && y == other.y && z == other.z;
		}
	};
	struct ChunkCoordHasher {
		size_t operator()(const Engine::CChunk::ChunkCoord& c) const noexcept {
			return (static_cast<size_t>(c.x) << 40) |
				((static_cast<size_t>(c.y) & 0xFFFFF) << 20) |
				(static_cast<size_t>(c.z) & 0xFFFFF);
		}
	};
	//key = (x << 40) | (y << 20) | z;
public:
	//const CBlock& GetBlock(uint32_t x, uint32_t y, uint32_t z) const;
	std::vector<QUAD> GenerateQuad();
	
private:
	//x,z,y
	std::array<Block, VOXEL_CHUNK_X_SIZE* VOXEL_CHUNK_Z_SIZE* VOXEL_CHUNK_Y_SIZE> m_arrBlocks{};
};
NS_END
