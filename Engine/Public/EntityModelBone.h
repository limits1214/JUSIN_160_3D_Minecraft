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

public:
    _float3* GetTranslatoin()   { return &m_Translation; }
    _float3* GetRotation() { return &m_Rotation; }
    _float3* GetScale() { return &m_Scale; }

    void SetTranslation(const _float3& trs) { m_Translation = trs; }
    void SetRotation(const _float3& rot) { m_Rotation = rot; }
    void SetScale(const _float3& scale) { m_Scale = scale; }

    void AddTranslation(const _float3& trs) {
        m_Translation.x += trs.x; 
        m_Translation.y += trs.y;
        m_Translation.z += trs.z;
    }

    void AddRotation(const _float3& rot) {
        m_Rotation.x += rot.x;
        m_Rotation.y += rot.y;
        m_Rotation.z += rot.z;
    }

    void AddScale(const _float3& scale) {
        m_Scale.x *= scale.x;
        m_Scale.y *= scale.y;
        m_Scale.z *= scale.z;
    }

    void BuildTransformMatrix()
    {
        auto t = XMMatrixTranslation(m_Translation.x, m_Translation.y, m_Translation.z);
        auto r = XMMatrixRotationX(m_Rotation.x) * XMMatrixRotationY(m_Rotation.y) * XMMatrixRotationZ(m_Rotation.z);
        auto s = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
        auto rs = s * r * t;
        XMStoreFloat4x4(&m_TransformationMatrix, s * r * t);
    }

private:
    _float3 m_Translation{};
    _float3 m_Rotation{};
    _float3 m_Scale{ 1.f, 1.f, 1.f };

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