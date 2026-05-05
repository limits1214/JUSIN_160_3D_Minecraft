#pragma once
#include "Engine_Defines.h"
#include "Chunk2.h"
#include "IRenderable.h"
#include <FastNoiseLite.h>

NS_BEGIN(Engine)
class CResTexture2DArray;
class CResVertexShader;
class CResPixelShader;
class CResSamplerState;

class CVoxelManager2 final : public CEngineBase, public IRenderable
{
private:
	explicit CVoxelManager2(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CVoxelManager2() override;

public:
	HRESULT Render(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx) override;
	bool HasRenderPass(RENDERPASS ePass) const override { return ePass == RENDERPASS::DEFAULT; };

public:
	void Update(_float fTimeDelta);
	void UpdateGUI();

public:
	uint64_t encodeChunkCoord(int32_t x, int32_t y, int32_t z) const
	{
		// X: 21비트 (±1,048,575 청크 ≈ ±33.5 million 블록)
		// Y: 22비트 (±2,097,151 청크 ≈ ±67 million 블록) → Y축은 보통 더 넓게
		// Z: 21비트

		constexpr int64_t X_OFFSET = 1 << 20;   // 2^20 = 1048576
		constexpr int64_t Y_OFFSET = 1 << 21;
		constexpr int64_t Z_OFFSET = 1 << 20;

		uint64_t ux = static_cast<uint64_t>(x + X_OFFSET) & 0x1FFFFF;   // 21비트
		uint64_t uy = static_cast<uint64_t>(y + Y_OFFSET) & 0x3FFFFF;   // 22비트
		uint64_t uz = static_cast<uint64_t>(z + Z_OFFSET) & 0x1FFFFF;   // 21비트

		return (ux << 43) | (uy << 21) | uz;
	}

	std::tuple<int32_t, int32_t, int32_t> decodeChunkCoord(uint64_t key) const
	{
		constexpr int64_t X_OFFSET = 1 << 20;
		constexpr int64_t Y_OFFSET = 1 << 21;
		constexpr int64_t Z_OFFSET = 1 << 20;

		int32_t x = static_cast<int32_t>((key >> 43) & 0x1FFFFF) - X_OFFSET;
		int32_t y = static_cast<int32_t>((key >> 21) & 0x3FFFFF) - Y_OFFSET;
		int32_t z = static_cast<int32_t>(key & 0x1FFFFF) - Z_OFFSET;

		return { x, y, z };
	}



private:
	HRESULT Initialize();

private:
	std::unordered_map<int64_t, UPtr<CChunk2>> m_mapChucnks{};
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
	static UPtr<CVoxelManager2> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END
