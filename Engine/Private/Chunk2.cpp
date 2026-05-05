#include "pch.h"
#include "Chunk2.h"
#include "Resources.h"

NS_USING(Engine)

CChunk2::CChunk2()
{
}

CChunk2::~CChunk2()
{
}

HRESULT CChunk2::Initialize(const DESC& desc)
{
	m_iX = desc.iX;
	m_iY = desc.iY;
	m_iZ = desc.iZ;
	m_iChunkCoord = desc.iChunkCoord;

	m_pResCBufferPerObject = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, TAG_RES_CBUFFER_OBJECT);

	return S_OK;
}


UPtr<CChunk2> CChunk2::Create(const DESC& desc)
{
	auto pInstnace = ToUPtr(new CChunk2{});
	if (FAILED(pInstnace->Initialize(desc)))
	{
		return nullptr;
	}
	return pInstnace;
}

void CChunk2::Free()
{
	CEngineBase::Free();
}
