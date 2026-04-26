#pragma once
#include "Component.h"
NS_BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	DECLARE_DERIVED_TYPE(CTransform, CComponent)

public:
	virtual void UpdateGUI();

private:
	explicit CTransform();
	explicit CTransform(const CTransform& rhs);
	~CTransform() override;

public:
	void Update();

public:
	const _float3& GetPosition() const { return m_vPos; }
	const _float4& GetQuaternion() const { return m_vQuat; }
	const _float3& GetScale() const { return m_vScale; }

	_vector GetLoadedPostion() const { return XMLoadFloat3(&m_vPos); }
	_vector GetLoadedQuaternion() const { return XMLoadFloat4(&m_vQuat); }
	_vector GetLoadedScale() const { return XMLoadFloat3(&m_vScale); }

	void SetPosition(const _float3& vPos) { m_vPos = vPos; m_bDirty = true; }
	void SetPosition(_vector vPos) { XMStoreFloat3(&m_vPos, vPos); m_bDirty = true; }
	void SetScale(const _float3& vScale) { m_vScale = vScale; m_bDirty = true; }
	void SetScale(_vector vScale) { XMStoreFloat3(&m_vScale, vScale); m_bDirty = true; }
	void SetQuaternion(const _float4& vQuat) { SetQuaternion(XMLoadFloat4(&vQuat)); m_bDirty = true; }
	void SetQuaternion(_vector vQuat) { XMStoreFloat4(&m_vQuat, XMQuaternionNormalize(vQuat)); m_bDirty = true; }

	_vector GetState(STATE eState) const
	{
		switch (eState)
		{
		case STATE::RIGHT: // 기본 X축(1,0,0)을 현재 쿼터니언으로 회전
			return XMVector3Rotate(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMLoadFloat4(&m_vQuat));
		case STATE::UP:    // 기본 Y축(0,1,0)을 현재 쿼터니언으로 회전
			return XMVector3Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMLoadFloat4(&m_vQuat));
		case STATE::LOOK:  // 기본 Z축(0,0,1)을 현재 쿼터니언으로 회전
			return XMVector3Rotate(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMLoadFloat4(&m_vQuat));
		case STATE::POSITION:
			return XMLoadFloat3(&m_vPos);
		}
		return XMVectorSet(0.f, 0.f, 0.f, 0.f);
	}

	//{ return XMLoadFloat4(reinterpret_cast<const _float4*>(&GetWorldMatrix()->m[ETOUI(eState)][0])); }

	void AddRotation(_vector vAxis, _float fAngleDegree)
	{
		_vector vNormalizedAxis = XMVector3Normalize(vAxis);
		_vector qPrevRot = XMLoadFloat4(&m_vQuat);
		_vector qDeltaRot = XMQuaternionRotationAxis(vNormalizedAxis, XMConvertToRadians(fAngleDegree));
		_vector qNewRot = XMQuaternionMultiply(qPrevRot, qDeltaRot);
		XMStoreFloat4(&m_vQuat, XMQuaternionNormalize(qNewRot));

		m_bDirty = true;
	}

	void SetRotation(_vector vAxis, float fAngleDegree)
	{
		_vector qNewRot = XMQuaternionRotationAxis(vAxis, XMConvertToRadians(fAngleDegree));
		XMStoreFloat4(&m_vQuat, qNewRot);

		m_bDirty = true;
	}

	const _float3& GetRotationEuler() const { return m_vEuler; }

	void SetRotationEuler(const _float3& vRotationAmount)
	{
		m_vEuler = vRotationAmount;
		_vector qDelta = XMQuaternionRotationRollPitchYaw(
			XMConvertToRadians(vRotationAmount.x),
			XMConvertToRadians(vRotationAmount.y),
			XMConvertToRadians(vRotationAmount.z)
		);
		XMStoreFloat4(&m_vQuat, XMQuaternionNormalize(qDelta));

		m_bDirty = true;
	}


	void AddRotationEuler(const _float3& vRotationAmount)
	{
		_vector qPrev = XMLoadFloat4(&m_vQuat);
		_vector qDelta = XMQuaternionRotationRollPitchYaw(
			XMConvertToRadians(vRotationAmount.x),
			XMConvertToRadians(vRotationAmount.y),
			XMConvertToRadians(vRotationAmount.z)
		);
		_vector qNew = XMQuaternionMultiply(qPrev, qDelta);
		XMStoreFloat4(&m_vQuat, XMQuaternionNormalize(qNew));

		m_bDirty = true;
	}

	void UpdateEulerFromQuat()
	{
		SimpleMath::Quaternion q = XMLoadFloat4(&m_vQuat);
		SimpleMath::Vector3 euler = q.ToEuler();
		m_vEuler.x = XMConvertToDegrees(euler.x);
		m_vEuler.y = XMConvertToDegrees(euler.y);
		m_vEuler.z = XMConvertToDegrees(euler.z);
	}

public:
	void GoStraight(_float fDist);
	void GoBackward(_float fDist);
	void GoLeft(_float fDist);
	void GoRight(_float fDist);

	void LookAt(_fvector vAt, _fvector vWorldUp = { 0.f, 1.f, 0.f, 0.f });
	void Chase(_fvector vGoal, _float fDist, _float fLimit = 0.f);

public:
	const _float4x4* GetWorldMatrix() const { return &m_WorldMatrix; }
	_matrix GetLoadedWorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }


private:
	_float3 m_vPos{};
	_float4 m_vQuat{ 0.f,0.f,0.f,1.f };
	_float3 m_vScale{ 1.f,1.f,1.f };

	_float3 m_vEuler{};

	_bool m_bParentScaleNormalize{ false };

	_bool m_bDirty{ true };
public:
	void SetDirty(_bool b) { m_bDirty = b; }

public:
	void SetParentWorldMatrix(const std::optional<_float4x4>& mat) { m_ParentWorldMatrix = mat; };
private:
	std::optional<_float4x4> m_ParentWorldMatrix{};

private:
	_float4x4 m_WorldMatrix{};

public:
	static UPtr<CTransform> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END
