#pragma once
#include "Engine_Defines.h"
#include "Block3.h"

NS_BEGIN(Engine)
class CResCBuffer;
class CResDynamicVIBuffer;
class CCollBox;

constexpr static int32_t VOXEL_CHUNK_X_SIZE3 = 32;
constexpr static int32_t VOXEL_CHUNK_Z_SIZE3 = 32;
constexpr static int32_t VOXEL_CHUNK_Y_SIZE3 = 256;

class ENGINE_DLL CChunk3 final : public CEngineBase
{
public:
	typedef struct tagDesc
	{
		int32_t iX{}, iY{}, iZ{};
		uint64_t iChunkCoord{};
	} DESC;

	enum class BLOCKFILLING_STATE { NON, ING, DONE };

	enum class BUFFER_STATE { NON, ING, DONE };

	enum class MESSING_STATE { NON, ING, DONE };
	
	enum class LIGHTING_STATE { NON, ING, DONE };

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
	static inline std::tuple<uint32_t, uint32_t, uint32_t> BlockIndexDecoding(uint32_t idx);

public:
	CBlock3& GetBlock(uint32_t x, uint32_t y, uint32_t z) { return m_arrBlocks[BlockIndexing(x, y, z)]; }
	void SetBlock(uint32_t x, uint32_t y, uint32_t z, CBlock3::TYPE eType) { m_arrBlocks[BlockIndexing(x, y, z)].SetType(eType); }
	CBlock3& GetBlock(uint32_t idx) { return m_arrBlocks[idx]; }
	void SetBlock(uint32_t idx, CBlock3::TYPE eType) { m_arrBlocks[idx].SetType(eType); }
	void SetBlock(uint32_t idx, CBlock3 block) { m_arrBlocks[idx] = block; }

public:
	HRESULT BlockFilling();
	HRESULT QuadMessing();
	HRESULT CreateBuffer();
	HRESULT DrawSolid(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx) const;
	HRESULT DrawAlphaTest(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx) const;
	HRESULT DrawWater(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx) const;
	_bool GetDead() const { return m_bDead; }
	void SetDead() { m_bDead = true; }

	_bool GetMessingQueued() const { return m_bMessingQueued; }
	void SetMessingQueued(_bool b) { m_bMessingQueued = b; }

public:
	void Update(_float fTimeDelta);

private:
	std::optional<CBlock3> GetBlockAt(int32_t x, int32_t y, int32_t z, std::vector<CChunk3*>& vecAdjChunks) const;

public:
	static std::vector<CChunk3*> MakeAdjChunks(int32_t cx, int32_t cy, int32_t cz);

private:
	std::vector<CChunk3*> MakeAdjChunks() const ;

	static bool IsFaceExposed(CBlock3 curBlock, FACE_DIR eDir, std::optional<CBlock3> optNextBlock);
private:
	struct QuadBuckets {
		std::vector<VOX_QUAD> solid{};
		std::vector<VOX_QUAD> alphaTest{};
		std::vector<VOX_QUAD> water{};
	};
	void NiveFaceCulling(QuadBuckets& quadBuckets) const;
	//void NiveFaceCulling(std::vector<VOX_QUAD>& solidQuads, std::vector<VOX_QUAD>& alphaTestQuads, std::vector<VOX_QUAD>& waterQuads) const;

	uint8_t CalculateVertexAO(_bool side1, _bool side2, _bool corner) const;

private:
	void SpawnTree(int32_t baseI, int32_t baseK, int32_t baseJ, CBlock3::TYPE eLogType, CBlock3::TYPE eLeafType);
	void BuildCrossMesh(float fx, float fy, float fz, CBlock3::TYPE curType, std::vector<VOX_QUAD>& alphaTestQuads) const;
	void BuildTorchMesh(float fx, float fy, float fz, CBlock3::TYPE curType, std::vector<VOX_QUAD>& alphaTestQuads) const;
	void BuildSlapMesh(float fx, float fy, float fz, CBlock3::TYPE curType, std::vector<VOX_QUAD>& alphaTestQuads) const;
	void BuildStairMesh(float fx, float fy, float fz, CBlock3::TYPE curType, std::vector<VOX_QUAD>& alphaTestQuads) const;
	void BuildWaterMesh(int x, int y, int z, std::vector<VOX_QUAD>& waterQuads, std::vector<CChunk3*>& vecAdjChunks)const;
	void BuildLavaMesh(int x, int y, int z, std::vector<VOX_QUAD>& waterQuads, std::vector<CChunk3*>& vecAdjChunks)const;

	//uint8_t GetCornerWaterLevel(int cx, int cy, int cz, std::vector<CChunk3*>& vecAdjChunks) const;
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
	void QuadsToVerticies(std::vector<VOX_QUAD>& quads, std::vector<E::VTX_VOXEL>& vertices, std::vector<uint32_t>& indices);
	HRESULT CreateBuffer(SPtr<CResDynamicVIBuffer>& pResBuffer, std::vector<E::VTX_VOXEL>& vertices, std::vector<uint32_t>& indices);
	SPtr<CResDynamicVIBuffer> m_pResSolidDynamicViBuffer{};
	std::vector<E::VTX_VOXEL> m_SolidVertices{};
	std::vector<uint32_t>  m_SolidIndices{};
	SPtr<CResDynamicVIBuffer> m_pResAlphaTestDynamicViBuffer{};
	std::vector<E::VTX_VOXEL> m_AlphaTestVertices{};
	std::vector<uint32_t>  m_AlphaTestIndices{};
	SPtr<CResDynamicVIBuffer> m_pResWaterDynamicViBuffer{};
	std::vector<E::VTX_VOXEL> m_WaterVertices{};
	std::vector<uint32_t>  m_WaterIndices{};

	SPtr<CResCBuffer> m_pResCBufferPerObject{};

public:
	uint32_t GetBufferVersion() const { return m_iBufferVersion; }
private:
	std::atomic<uint32_t> m_iBufferVersion{ 0 };

public:
	CCollBox* GetCollBox() const { return m_pCollBox.get(); }
private:
	UPtr<CCollBox> m_pCollBox{};

public:
	static UPtr<CChunk3> Create(const DESC& desc);
};
NS_END

NS_BEGIN(Engine)

inline std::tuple<uint32_t, uint32_t, uint32_t> CChunk3::BlockIndexDecoding(uint32_t idx)
{
	constexpr uint32_t yzStride =
		VOXEL_CHUNK_Y_SIZE3 * VOXEL_CHUNK_Z_SIZE3;

	uint32_t x = static_cast<uint32_t>(idx / yzStride);

	idx %= yzStride;

	uint32_t z = static_cast<uint32_t>(idx / VOXEL_CHUNK_Y_SIZE3);

	uint32_t y = static_cast<uint32_t>(idx % VOXEL_CHUNK_Y_SIZE3);

	return { x, y, z };
}

NS_END