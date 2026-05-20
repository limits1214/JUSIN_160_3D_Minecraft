#include "pch.h"

#include "FontManager.h"



NS_USING(Engine)

CFontManager::CFontManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice { pDevice }
    , m_pContext {pContext}
{
}

CFontManager::~CFontManager()
{
}

void CFontManager::UpdateGUI()
{
}

void CFontManager::Draw(const StringID& fontName, const _tchar* pText, const _float2& vPosition, float fScale, _fvector vColor, _float fRotation, const _float2& vOrigin)
{
    
    if (auto font = CGameInstance::Get().GetResourceFirst<CResFontCustom>("FONT", fontName))
    {
        m_pBatch->Begin();
        font->GetFont()->DrawString(m_pBatch.get(), pText, vPosition, vColor, fRotation, vOrigin, fScale);
        m_pBatch->End();
    }
}

HRESULT CFontManager::Initialize()
{
    //MakeSpriteFont "Neo둥근모 Pro" /FontSize:20 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 NeoDGM_20px.spritefont
    if (auto res = CGameInstance::Get().AddResource("FONT", "NeoDGM_20px", CResFontCustom::Create("./Resources/Font/NeoDGM_20px.spritefont")))
    {
        if (FAILED(res->Load()))
        {
            return E_FAIL;
        }
    }
    m_pBatch = std::make_unique<SpriteBatch>(m_pContext.Get());
	return S_OK;
}

UPtr<CFontManager> CFontManager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    auto pInstance = ToUPtr(new CFontManager{ pDevice, pContext });

    if (FAILED(pInstance->Initialize()))
    {
        return nullptr;
    }

    return pInstance;
}
