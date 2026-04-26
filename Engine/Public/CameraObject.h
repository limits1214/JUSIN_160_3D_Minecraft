#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCameraObject : public CGameObject
{
public:
	DECLARE_DERIVED_TYPE(CCameraObject, CGameObject)

public:
	enum class PROJ { PERSPECTIVE, ORTHOGRAPHIC, END };
	typedef struct tagCameraDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vEye{};
		_float3 vAt{};
		_float3 vUp{ 0.f, 1.f, 0.f };
		_float fFovY{};
		_float fAspect{};
		_float fNear{};
		_float fFar{};
		CCameraObject::PROJ eProj{ CCameraObject::PROJ::END };
		_float fWidth{};
		_float fHeight{};
	}CAMERA_DESC;


protected:
	explicit CCameraObject();
	explicit CCameraObject(const CCameraObject& Prototype);
	~CCameraObject() override;

public:
	_matrix GetView() const { return XMLoadFloat4x4(&m_matView); }
	_matrix GetProj() const { return XMLoadFloat4x4(&m_matProj); }
	const _float4* GetFrustumFarCorner() const { return m_FrustumFarCorner; }

public:
	HRESULT Initialize(void* pArg) override;
	//void PriorityUpdate(E::_float fTimeDelta) override;
	//void Update(E::_float fTimeDelta) override;
	//void LateUpdate(E::_float fTimeDelta) override;
	//HRESULT Render(ComPtr<ID3D11Device>& ppDevice, ComPtr<ID3D11DeviceContext>& ppContext) override;

protected:
	HRESULT UpdateViewMatrix();
	HRESULT UpdateProjMatrix();

protected:
	CAMERA_DESC m_cameraDesc{};
	_float4x4 m_matView{};
	_float4x4 m_matProj{};

	_float4 m_FrustumFarCorner[4]{};
};

NS_END