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

class CVoxelManager3 final : public CEngineBase, public IRenderable
{
public:
	typedef struct tagInRangeChunkCreateDesc
	{
		int32_t iCenterX{}, iCenterY{}, iCenterZ{};
	}IN_RANGE_CHUNK_CREATE_DESC;

	typedef struct tagOutRangeChunkReleaseDesc
	{
		int32_t iCenterX{}, iCenterY{}, iCenterZ{};
	}OUT_RANGE_CHUNK_RELEASE_DESC;

	typedef struct tagChunkEditDesc
	{
		int32_t iWorldBlockX{};
		int32_t iWorldBlockY{};
		int32_t iWorldBlockZ{};
		CBlock3 block{};
	}CHUNK_EDIT_DESC;

	std::unordered_map<uint64_t, std::unordered_map<uint32_t, CBlock3>> m_EditShadow{};

	typedef struct tagRaycastResult {
		CChunk3* pChunk{};
		uint32_t iX{}, iY{}, iZ{};// 청크기준
		FACE_DIR eHitFace;
		float fDist{};
	} BLOCK_RAY_RESULT;
public:
	std::optional< CBlock3> GetBlock(int32_t wbx, int32_t wby, int32_t wbz) const;
	std::optional< CBlock3> GetBlockByChunkCoord(uint64_t chunkIdx ,int32_t cbx, int32_t cby, int32_t cbz) const;
	void SetBlock(int32_t wbx, int32_t wby, int32_t wbz, CBlock3 block);
	_bool BlockRaycast(const _float3& rayOrigin,
		const _float3& rayDir,     // normalized
		float fMaxDist,
		BLOCK_RAY_RESULT& outResult) const;
	HRESULT QueuingInRangeChunkCreate(const IN_RANGE_CHUNK_CREATE_DESC& desc);
	HRESULT QueuingOutRangeChunkRelease(const OUT_RANGE_CHUNK_RELEASE_DESC& desc);

private:
	std::list<IN_RANGE_CHUNK_CREATE_DESC> m_queueInRangeChunkCreate{};
	std::list<OUT_RANGE_CHUNK_RELEASE_DESC> m_queueOutRangeChunkRelease{};
	//std::list<CHUNK_EDIT_DESC> m_queueChunkEdit{};

	std::list<std::vector<uint64_t>> m_queuedQuadMessingChunks{};

	std::list<std::future<CHUNK_EDIT_DESC>> m_queueFutEdit{};
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
	HRESULT QueueingQuadMessing(std::vector<uint64_t> targetCoords);
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
	std::mutex m_Mutex{};

public:
	static UPtr<CVoxelManager3> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END
