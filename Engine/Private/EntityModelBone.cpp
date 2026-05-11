#include "EntityModelBone.h"
NS_USING(Engine)

CEntityModelBone::CEntityModelBone(const std::string& sName)
	: m_sName{ sName }
{
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
	//std::string m_Name{};
	_float4x4 m_TransformationMatrix = { };
	_float4x4 m_CombinedTransformationMatrix = {};
}
