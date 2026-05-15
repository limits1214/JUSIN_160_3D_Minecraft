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
    const _float4x4* GeTransformationMatrix() const { return &m_TransformationMatrix; }

    void SetPivot(const _float3& pivot) { m_vPivot = pivot; }
    const _float3& GetPivot() const { return m_vPivot; }
    void SetLocalPivot(const _float3& pivot) { m_vLocalPivot = pivot; }
    const _float3& GetLocalPivot() const { return m_vLocalPivot; }
    const std::string& GetName() const { return m_sName; }
    const std::string& GetParentName() const { return m_sParentName; }
    void SetParentName(const std::string& name) { m_sParentName = name; }

    const _float4x4* GetOffsetMatrix() const { return &m_OffsetMatrix; }
    void SetOffsetMatrix(const _float4x4& o) { m_OffsetMatrix = o; }

    void UpdateTransformationMatrix(_fmatrix localTransform)   // 키프레임에서 온 Local TRS
    {
        // LocalPivot = Bind Pose에서의 부모 기준 Local Position
        
        XMStoreFloat4x4(&m_TransformationMatrix, localTransform );
    }
    void UpdateCombinedMatrix(const _float4x4* pParentCombined)
    {
        XMMATRIX OffsetFromParentPivot = XMMatrixTranslationFromVector(XMLoadFloat3(&m_vLocalPivot));
        XMMATRIX local = XMLoadFloat4x4(&m_TransformationMatrix) * OffsetFromParentPivot;

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
    _float3 m_vLocalPivot{};

    _float4x4 m_OffsetMatrix{};

    int32_t m_iParentIndex{ -1 };
};
NS_END