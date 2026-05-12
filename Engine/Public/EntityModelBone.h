#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)
class ENGINE_DLL CEntityModelBone
{
public:
    CEntityModelBone(const std::string& sName);

public:
    int32_t GetParentIndex() const { return m_iParentIndex; }
    void SetParentIndex(uint32_t i) { m_iParentIndex = i; }
    const _float4x4* GetCombinedTransformationMatrix() const { return &m_CombinedTransformationMatrix; }
    void SetPivot(const _float3& pivot) { m_vPivot = pivot; }
    const _float3& GetPivot() const { return m_vPivot; }
    const std::string& GetName() const { return m_sName; }
    const std::string& GetParentName() const { return m_sParentName; }
    void SetParentName(const std::string& name) { m_sParentName = name; }

    void UpdateTransformationMatrix(_fmatrix mat)
    {
        //auto pivot = XMLoadFloat3(&m_vPivot);
        //XMMATRIX boneMatrix =
        //    XMMatrixTranslationFromVector(-pivot)
        //    * mat
        //    * XMMatrixTranslationFromVector(pivot);
        XMStoreFloat4x4(&m_TransformationMatrix, mat);
    }

    void UpdateCombinedMatrix(const _float4x4* pParentCombined)
    {
        //XMVECTOR pivot = XMLoadFloat3(&m_vPivot);

        //XMMATRIX local =
        //    XMMatrixTranslationFromVector(-pivot)
        //    * XMLoadFloat4x4(&m_TransformationMatrix)
        //    * XMMatrixTranslationFromVector(pivot);

        XMMATRIX local =XMLoadFloat4x4(&m_TransformationMatrix);

        if (pParentCombined)
            XMStoreFloat4x4(&m_CombinedTransformationMatrix,
                local * XMLoadFloat4x4(pParentCombined));
        else
            XMStoreFloat4x4(&m_CombinedTransformationMatrix, local);
    }

private:
    std::string m_sName{};
    std::string m_sParentName{};
    _float4x4 m_TransformationMatrix{};
    _float4x4 m_CombinedTransformationMatrix{};
    _float3 m_vPivot{};

    int32_t m_iParentIndex{ -1 };
};
NS_END