#include "pch.h"
#include "ResSamplerState.h"

NS_USING(Engine)

CResSamplerState::CResSamplerState(const _string& sPath, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CResource{ sPath }
	, m_pDevice{pDevice}
	, m_pContext{pContext}
{
}

CResSamplerState::~CResSamplerState()
{
}

HRESULT CResSamplerState::Load(const std::any& arg)
{
	auto desc = std::any_cast<D3D11_SAMPLER_DESC>(&arg);
	if (!desc)
	{
		return E_FAIL;
	}

	if (m_eState == STATE::LOADED)
	{
		return S_OK;
	}

	m_eState = STATE::LOADING;

	if (FAILED(m_pDevice->CreateSamplerState(desc, m_Sampler.GetAddressOf())))
	{
		m_eState = STATE::LOADFAIL;
		MSG_BOX("SamplerState Create Failed");
		return E_FAIL;
	};

	m_eState = STATE::LOADED;
	return S_OK;
}

HRESULT CResSamplerState::Unload(const std::any& arg)
{
	return S_OK;
}

SPtr<CResSamplerState> CResSamplerState::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	return ToSPtr(new CResSamplerState{ "", pDevice, pContext });
}
