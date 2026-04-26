#include "pch.h"
#include "ResPixelShader.h"

NS_USING(Engine)

HRESULT CResPixelShader::Load(const std::any& arg)
{
	m_eState = STATE::LOADING;
	if (FAILED(CompileShader()))
	{
		MSG_BOX("PIXEL SHADER COMPILE FAILED");
		m_eState = STATE::LOADFAIL;
		return E_FAIL;
	}
	if (FAILED(m_pDevice->CreatePixelShader(m_pBlob->GetBufferPointer(),
		m_pBlob->GetBufferSize(), nullptr, &m_pPixelShader)))
	{
		m_eState = STATE::LOADFAIL;
		return E_FAIL;
	}
	m_eState = STATE::LOADED;
	return S_OK;
}

HRESULT CResPixelShader::Unload(const std::any& arg)
{
	return S_OK;
}

CResPixelShader::CResPixelShader(const _string& sPath, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CResShader{ sPath, pDevice, pContext }
{
	m_sEntryPoint = "PSMain";
	m_sTarget = "ps_5_0";
}

CResPixelShader::~CResPixelShader()
{
}

SPtr<CResPixelShader> CResPixelShader::Create(const _string& sPath, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	return ToSPtr(new CResPixelShader{ sPath, pDevice, pContext });
}
