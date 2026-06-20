#pragma once
#include "EntityObject.h"

NS_BEGIN(Engine)
class CComEntityModel;
class CComAnimator;
class CComConstantBuffer;
class ENGINE_DLL CArmorEntity : public CEntityObject
{
public:
	enum class ARMOR_TYPE
	{
		HELMET,
		CHESTPLATE,
		LEGGINGS,
		BOOTS,
		END
	};

	enum class ARMOR_MADE
	{
		IRON,
		COPPER,
		GOLD,
		DIAMOND,
		NETHERITE,
		END
	};
public:
	typedef struct tagDesc : CEntityObject::DESC
	{
		ARMOR_TYPE eArmorType{ ARMOR_TYPE::END };
		ARMOR_MADE eArmorMade{ ARMOR_MADE::END} ;
	}DESC;

private:
	_string_view GetArmorGeoByType(ARMOR_TYPE eType)
	{
		switch (eType)
		{
		case ARMOR_TYPE::HELMET:
			return "PlayerArmorHelmet";
		case ARMOR_TYPE::CHESTPLATE:
			return "PlayerArmorChestplate";
		case ARMOR_TYPE::LEGGINGS:
			return "PlayerArmorLeggings";
		case ARMOR_TYPE::BOOTS:
			return "PlayerArmorBoots";
		}
	}

	_string GetVIBufferByTypeAndMade(ARMOR_TYPE eType, ARMOR_MADE eMade)
	{
		
		_string_view currType = GetArmorGeoByType(eType);
		switch (eMade)
		{
		case ARMOR_MADE::IRON:
			return std::format("{}{}", currType, "_Iron");
		case ARMOR_MADE::COPPER:
			return std::format("{}{}", currType, "_Copper");
		case ARMOR_MADE::GOLD:
			return std::format("{}{}", currType, "_Gold");;
		case ARMOR_MADE::DIAMOND:
			return std::format("{}{}", currType, "_Diamond");;
		case ARMOR_MADE::NETHERITE:
			return std::format("{}{}", currType, "_Netherite");;
		}
	}

public:
	DECLARE_DERIVED_TYPE(CArmorEntity, CEntityObject)

private:
	explicit CArmorEntity();
	~CArmorEntity() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

private:
	CComEntityModel* m_pComEntityModel{};
	CComAnimator* m_pComAnimator{ };
	CComConstantBuffer* m_pComCBufferPerObject{};

private:
	ARMOR_TYPE m_eArmorType{ ARMOR_TYPE::END };
	ARMOR_MADE m_eArmorMade{ ARMOR_MADE::END };

public:
	static UPtr<CArmorEntity> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END