#include "Chunk.h"
#include <FastNoiseLite.h>

NS_USING(Engine)

std::vector<QUAD> CChunk::GenerateQuad()
{
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    return std::vector<QUAD>();
}
