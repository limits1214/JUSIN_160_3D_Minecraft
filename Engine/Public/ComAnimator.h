#pragma once

#include "Component.h"
#include "EntityModelBone.h"
#include "EntityModelMesh.h"

NS_BEGIN(Engine)
class CComEntityModel;
class ENGINE_DLL CComAnimator : public CComponent
{
public:
	typedef struct tagDesc : CComponent::DESC
	{
		CComEntityModel* pComEntityModel{};
	}DESC;
public:
	DECLARE_DERIVED_TYPE(CComAnimator, CComponent)

private:
	explicit CComAnimator();
	~CComAnimator() override;

public:
	void QuadrupedWalk(_float fTimeDelta);
	void HeadLookAt(_float fTimeDelta, _float3 vPos);

	void SetSpiderDefaultLegPose(_float fTimeDelta);
	void SetEnderManBasePose(_float fTimeDelta);

	void HumanoidBob(_float fTimeDelta);
private:
	HRESULT Initialize(void* pArg) override;

private:
	_float m_fElapsed{};

private:
	CComEntityModel* m_pComEntityModel{};

private:
	// 머리의 현재 회전 상태를 기억할 쿼터니언 (처음에는 회전 없음(Identity)으로 초기화)
	_float4 m_vCurrentHeadRotQuat{ 0.f, 0.f, 0.f, 1.f };

	// 머리가 회전하는 속도 상수 (숫자가 클수록 빠르고 절도 있게 반응합니다)
	_float  m_fHeadTurnSpeed{ 8.0f };

public:
	static UPtr<CComAnimator> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END