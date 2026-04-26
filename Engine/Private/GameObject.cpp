#include "GameObject.h"
#include "Component.h"
#include "GameInstance.h"

#include "Transform.h"
#include "Helper.h"

NS_USING(Engine)

CGameObject::CGameObject()
    : CPrototype{ }
{
}

CGameObject::CGameObject(const CGameObject& Prototype)
    : CPrototype{ Prototype }
{
}

CGameObject::~CGameObject()
{
}

HRESULT CGameObject::Initialize(void* pArg)
{
    auto pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
    m_sTag = pDesc->sObjectTag;
    m_ObjectHandle = pDesc->handle;

    {
        auto pProto = CGameInstance::Get().ClonePrototype("PERMANENT", "Prototype_Component_Transform", pDesc);
        if (pProto == nullptr)
        {
            return E_FAIL;
        }
        AddComponent("Com_Transform", static_uptr_cast<CTransform>(std::move(pProto)));
        m_pComTransform = GetComponent<CTransform>("Com_Transform");
    }

    return S_OK;
}

void CGameObject::PriorityUpdate(_float fTimeDelta)
{
}

void CGameObject::Update(_float fTimeDelta)
{
}

void CGameObject::LateUpdate(_float fTimeDelta)
{
}

HRESULT CGameObject::Render(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx)
{
    return S_OK;
}


void CGameObject::UpdateGUI()
{
    if (ImGui::Button("Destroy"))
    {
        SetPendingDestroy();
    }
    if (ImGui::Button("DestroyCascade"))
    {
        SetPendingDestroyCascade();
    }
    //ImGui::Text("dest: %s", m_bPendingDestroy ? "true" : "false");
    if (ImGui::TreeNode("Components"))
    {
        for (const auto& [comID, pCom] : m_Components)
        {
            if (ImGui::TreeNode(comID.GetDbgStr()))
            {
                pCom->UpdateGUI();

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }
}

HRESULT CGameObject::AddComponent(const StringID& tagComponent, UPtr<CComponent> pComponent)
{
    auto iter = m_ComponentsLookup.find(tagComponent);
    if (iter != m_ComponentsLookup.end())
    {
        return E_FAIL;
    }

    pComponent->SetGameObject(this);
    pComponent->SetGameObjectHandle(m_ObjectHandle);

    auto size = m_Components.size();
    std::pair<StringID, UPtr<CComponent>> a{ tagComponent, std::move(pComponent) };
    m_Components.push_back(std::move(a));
    m_ComponentsLookup.emplace(tagComponent, size);


    return S_OK;
}

HRESULT CGameObject::DelComponent(const StringID& tagComponent)
{
    auto iter = m_ComponentsLookup.find(tagComponent);
    if (iter == m_ComponentsLookup.end())
    {
        return E_FAIL;
    }

    const size_t idx = iter->second;
    const size_t last = m_Components.size() - 1;

    if (idx != last)
    {
        std::swap(m_Components[idx], m_Components[last]);

        // 이동된 요소의 키 인덱스 업데이트
        m_ComponentsLookup[m_Components[idx].first] = idx;
    }

    m_ComponentsLookup.erase(iter);
    m_Components.pop_back();
    return S_OK;
}

void CGameObject::SetParentObjectHandle(std::optional<CHandle> newParentHandle)
{
    _SetParentHandle(newParentHandle);

    if (newParentHandle)
    {
        if (auto pTransform = GetComponent<CTransform>("Com_Transform"))
        {
            pTransform->SetDirty(true);
            auto world = GetComponent<CTransform>("Com_Transform")->GetWorldMatrix();
            pTransform->SetParentWorldMatrix(*world);
        }
    }
    else
    {
        if (auto pTransform = GetComponent<CTransform>("Com_Transform"))
        {
            pTransform->SetDirty(true);
            pTransform->SetParentWorldMatrix(std::nullopt);
        }
    }
}

void CGameObject::_SetParentHandle(const std::optional<CHandle>& newParentHandle)
{
    if (newParentHandle)
    {
        if (m_pParentHandle)
        {
            if (auto* pParent = CGameInstance::Get().GetGameObjectByHandle(m_pParentHandle.value()))
            {
                pParent->EraseChildHandle(GetHandle());
                pParent->m_pChildrenHandles.push_back(GetHandle());
            }
            m_pParentHandle = newParentHandle;
        }
        else
        {
            m_pParentHandle = newParentHandle;
            if (auto* pParent = CGameInstance::Get().GetGameObjectByHandle(m_pParentHandle.value()))
            {
                pParent->m_pChildrenHandles.push_back(GetHandle());
            }
        }
    }
    else
    {
        if (m_pParentHandle)
        {
            if (auto* pParent = CGameInstance::Get().GetGameObjectByHandle(m_pParentHandle.value()))
            {
                pParent->EraseChildHandle(GetHandle());
            }
            m_pParentHandle = std::nullopt;
        }
    }
}

void CGameObject::EraseChildHandle(const CHandle& pChildHandle)
{
    auto iter = std::find(m_pChildrenHandles.begin(), m_pChildrenHandles.end(), pChildHandle);
    if (iter != m_pChildrenHandles.end())
    {
        m_pChildrenHandles.erase(iter);
    }
}

void CGameObject::Free()
{
    {
        SetParentObjectHandle(std::nullopt);
        std::for_each(m_pChildrenHandles.begin(), m_pChildrenHandles.end(),
            [](const CHandle& childHandle)
            {
                if (auto* pChildObj = CGameInstance::Get().GetGameObjectByHandle(childHandle))
                {
                    pChildObj->SetParentObjectHandle(std::nullopt);
                }
            });
    }
    CPrototype::Free();
}

void CGameObject::SetPendingDestroy(_bool b)
{
    m_bPendingDestroy = b;
}

void CGameObject::SetPendingDestroyCascade(_bool b)
{
    CHelper::MyHandleTreeDFS(&m_ObjectHandle, [&](const CHandle* handle)
        {
            if (auto* pObj = CGameInstance::Get().GetGameObjectByHandle(*handle))
            {
                pObj->SetPendingDestroy(b);
            }
        });
}
