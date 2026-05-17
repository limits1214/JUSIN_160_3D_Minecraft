#pragma once

#include "Component.h"
#include "EntityModelBone.h"
#include "EntityModelMesh.h"

NS_BEGIN(Engine)
class CResEnttVIBuffer;
class ENGINE_DLL CComEntityModel : public CComponent
{
public:
	typedef struct tagDesc: CComponent::DESC
	{
		std::pair<StringID, StringID> viBufferId;
		std::pair<StringID, StringID> geometryId;
	}DESC;
public:
	DECLARE_DERIVED_TYPE(CComEntityModel, CComponent)

public:
	virtual void UpdateGUI();

private:
	explicit CComEntityModel();
	~CComEntityModel() override;

public:
	void UpdateBoneMatrix(_float fTimeDelta);
	void BindBoneMatrix() const;
	void ResetBonesChannel();
	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx, uint32_t iMeshIdx = 0);
	const std::vector<CEntityModelBone>& GetBones() const { return m_Bones; }
	CEntityModelBone* GetBone(const std::string& name)
	{
		auto iter = m_BonesLookup.find(name);
		if (iter == m_BonesLookup.end())
		{
			return nullptr;
		}
		return &m_Bones[iter->second];
	}

private:
	HRESULT Initialize(void* pArg) override;

private:
	_float m_fElapsed{ 0 };
	std::vector<CEntityModelBone> m_Bones{};
	std::unordered_map<std::string, size_t> m_BonesLookup{};
	//std::vector<CEntityModelMesh> m_Meshes{};
	SPtr<CResEnttVIBuffer> m_pResEntityViBuffer{};
	CB_PER_BONE m_cbPerBone{};

public:
	static UPtr<CComEntityModel> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END