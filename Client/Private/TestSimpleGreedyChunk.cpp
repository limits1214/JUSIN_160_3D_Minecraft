#include "TestSimpleGreedyChunk.h"
NS_USING(Client)

CTestSimpleGreedyChunk::CTestSimpleGreedyChunk()
{

}
CTestSimpleGreedyChunk::~CTestSimpleGreedyChunk()
{
}

HRESULT CTestSimpleGreedyChunk::Initialize(void* pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
	{
		return E_FAIL;
	}
    return S_OK;
}

void CTestSimpleGreedyChunk::PriorityUpdate(E::_float fTimeDelta)
{
	int x = 0;
}

void CTestSimpleGreedyChunk::Update(E::_float fTimeDelta)
{
}

void CTestSimpleGreedyChunk::LateUpdate(E::_float fTimeDelta)
{
}

HRESULT CTestSimpleGreedyChunk::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    return S_OK;
}

E::UPtr<CTestSimpleGreedyChunk> CTestSimpleGreedyChunk::Create()
{
	auto pInstance = E::ToUPtr(new CTestSimpleGreedyChunk{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CTestSimpleGreedyChunk");
		return nullptr;
	}
	return  pInstance;
}

E::UPtr<E::CPrototype> CTestSimpleGreedyChunk::Clone(void* pArg)
{
	auto	pInstance = E::ToUPtr(new CTestSimpleGreedyChunk{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTestSimpleGreedyChunk");
		return nullptr;
	}
	return pInstance;
}
