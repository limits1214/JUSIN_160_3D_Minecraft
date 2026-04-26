#include "pch.h"
#include "ResGeometryShader.h"

NS_USING(Engine)

HRESULT CResGeometryShader::Load(const std::any& arg)
{
	m_eState = STATE::LOADING;
	if (FAILED(CompileShader()))
	{
		MSG_BOX("GEOMETRY SHADER COMPILE FAILED");
		m_eState = STATE::LOADFAIL;
		return E_FAIL;
	}
	if (FAILED(m_pDevice->CreateGeometryShader(m_pBlob->GetBufferPointer(),
		m_pBlob->GetBufferSize(), nullptr, &m_pGeometryShader)))
	{
		m_eState = STATE::LOADFAIL;
		return E_FAIL;
	}
	m_eState = STATE::LOADED;
	return S_OK;
}

HRESULT CResGeometryShader::Unload(const std::any& arg)
{
	return S_OK;
}

CResGeometryShader::CResGeometryShader(const _string& sPath, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CResShader{ sPath, pDevice, pContext }
{
	m_sEntryPoint = "GSMain";
	m_sTarget = "gs_5_0";
}

CResGeometryShader::~CResGeometryShader()
{
}

SPtr<CResGeometryShader> CResGeometryShader::Create(const _string& sPath, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	return ToSPtr(new CResGeometryShader{ sPath, pDevice, pContext });
}
