#pragma once
#include "PlayerEntityObject.h"
#include "PlayerCamera.h"

#include "Item.h"

NS_BEGIN(Engine)
class CComEntityModel;
class CDestroyStage;
class CUIController;
class ENGINE_DLL CPlayerEntity : public CPlayerEntityObject
{
public:
	enum class CAMERA_TYPE
	{
		FPS,
		TPS,
		TPS_BACK,
	};

	enum class MODE_TYPE
	{
		GOD,
		GRAVITY,
	};

	enum class MOVE_STATE
	{
		CROUCH
	};

public:
	typedef struct tagDesc : CPlayerEntityObject::DESC
	{
		//CHandle hFurnaceStorage{};
	}DESC;

public:
	DECLARE_DERIVED_TYPE(CPlayerEntity, CPlayerEntityObject)

private:
	explicit CPlayerEntity();
	CPlayerEntity(const CPlayerEntity& rhs);
	~CPlayerEntity() override;

public:
	void UpdateGUI() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;

	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;
	HRESULT RenderDefault(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx);
	HRESULT RenderShadow(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx);
	HRESULT RenderPlayerInvenUI(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx);

	void AddRenderPassPlayerInvenUIPass();

private:
	_float3 m_vHeadRotation = { 0.f, 0.f, 0.f };
	CAMERA_TYPE m_eCameraType{ CAMERA_TYPE ::FPS };
	MODE_TYPE m_eModeType{ MODE_TYPE::GOD };
private:
	CComEntityModel* m_pComEntityModel{};

	CComEntityModel* m_pComInventoryPlayerEntityModel{};

public:
	void SetDestroyState(CHandle h) { m_hDestroyStage = h; }
private:
	void ProcessDestroyStage(float fTimeDelta);
	//void DestroyStageEndAfterProcess(const CItemObject::ItemInfo& info, const XMINT3& wbLocatoin);
	//void DestroyStageEndItemConverter(CItemObject::ItemInfo& info);
	CDestroyStage* GetDestroyStage() const;
private:
	std::optional<std::pair<XMINT3, uint8_t>> m_DestoryStageRaycastTarget{};
	_bool m_bDestoryStageStart{ false };
	CHandle m_hDestroyStage{};

public:
	void ProcessLeftClick(float fTimeDelta);
	void ProcessRightClick(float fTimeDelta);

public:
	void SetUIController(CHandle h){m_hUIController = h; }
private:
	CUIController* GetUIController() const;
	void ProcessUI(float fTimeDelta);
	void ProcessUIHotbar(float fTimeDelta);
	void ProcessUIInventory(float fTimeDelta);
	void ProcessUIInventoryCrafting(float fTimeDelta);
	void ProcessUICraftingTable(float fTimeDelta);
	void ProcessUICraftingTableCrafting(float fTimeDelta);
	void ProcessUIFurnace(float fTimeDelta);
	void ProcessUIChest(float fTimeDelta);
	void ProcessThrowItem(float fTimeDelta);
	void ProcessHandHeldItem(float fTimeDelta);
private:
	CHandle m_hUIController{};

public:
	void SetRightItemHandle(std::optional<CHandle> h) { m_hRightItem = h; }
private:
	std::optional<CHandle> m_hRightItem{};


private:
	void ProcessActionUpdate(_float fTimeDelta);

private:
	void ProcessItemColliding(_float fTimeDelta);
	void ProcessExpOrbColliding(_float fTimeDelta);

private:
	void PlayerCameraTrace(_float fTimeDelta);
	void PlayerMove(_float fTimeDelta);
	//void PlayerMoveX(_float fTimeDelta);
private:
	float m_fPlayerSpeed = 5.f;

private:
	float m_fRootRotRadY = 0.f;   // 리셋에 영향 안 받는 영구 상태
	float m_fHeadRotRadX = 0.f;
	float m_fHeadRotRadY = 0.f;

private:


	bool m_bAttacking = false;
	float m_fAttackTime = 0.0f;
	float m_fAttackDuration = 0.6f;

	void StartAttack()
	{
		m_bAttacking = true;
		m_fAttackTime = 0.0f;
		m_fAttackDuration = 0.6f;  // 공격 지속 시간
	}


private:
	CHandle m_hPlayerFPSArm{};

private:
	_bool m_bControl{ false };
	_bool m_bKeyPressingW{ false };
	_bool m_bKeyPressingA{ false };
	_bool m_bKeyPressingS{ false };
	_bool m_bKeyPressingD{ false };
	_bool m_bKeyPressingQ{ false };
	_bool m_bKeyPressingE{ false };
	_bool m_bKeyPressingShift{ false };
	_bool m_bKeyPressingSpace{ false };
	_bool m_bNumKeyPressing[10]{};

	_bool m_bKeyDownE{ false };
	_bool m_bKeyDownQ{ false };

	int32_t m_iMouseMoveX{ 0 };
	int32_t m_iMouseMoveY{ 0 };
	int32_t m_iMouseMoveZ{ 0 };
	_bool m_bMousePressingLeft{ false };
	_bool m_bMousePressingRight{ false };
	_bool m_bMouseDownLeft{ false };
	_bool m_bMouseDownRight{ false };

private:
	UPtr<CCollider> m_pMeleeAttackCollider{};

private:
	UPtr<CCollider> m_pCenterCollider{};

private:
	CCameraObject* m_pActivePlayerCamera{};
	CCameraObject* m_pPlayerCamera{};
	_bool m_bPlayerCameraLookBack{false};

private:
	_float3 m_vVelocity{};
	bool     m_bOnGround{ false };

private:
	void ReadyPlayerItem();

	HRESULT ProcessItemGain(const CItemObject::ItemInfo& info);
	HRESULT ProcessExpOrbGain(_float fGage);
	
	std::array<std::optional<CItemObject::ItemInfo>, 4> m_ItemArrArmor{};
	std::optional<CItemObject::ItemInfo> m_ItemShiled{};
	std::array<std::optional<CItemObject::ItemInfo>, 9*3> m_ItemArrInventory{};
	std::array<std::optional<CItemObject::ItemInfo>, 9> m_ItemArrHotbar{};
	std::array<std::optional<CItemObject::ItemInfo>, 5> m_ItemArrInvenCrafting{};

	std::optional <CItemObject::SRecipe > m_ItemInvenCraftingOriginRecipe{};

	std::array<std::optional<CItemObject::ItemInfo>, 10> m_ItemArrCraftingTableCrafting{};

	std::optional <CItemObject::SRecipe > m_ItemCraftingTableCraftingOriginRecipe{};




	CHandle m_hInventoryUIItemOnCursor{};
	void ProcessUIInventoryItemOnCursor(_float fTimeDelta);

	CHandle m_hCraftingTableUIItemOnCursor{};
	void ProcessUICraftingItemOnCursor(_float fTimeDelta);

	CHandle m_hFurnaceUIItemOnCursor{};
	void ProcessUIFurnaceOnCursor(_float fTimeDelta);

	CHandle m_hChestUIItemOnCursor{};
	void ProcessUIChestOnCursor(_float fTimeDelta);

private:
	//CHandle m_hFurnaceStorage{};
	std::optional<XMINT3> m_openFurnaceLocation{};

	std::optional<XMINT3> m_openChestLocation{};

private:
	//void SpawnDropItemObject(const CItemObject::ItemInfo& info, _float3 pos, _float3 vel);

private:
	void ProcessUIStatus(_float fTimeDelta);
	void ProcessUIStatusHealth(_float fTimeDelta);
	void ProcessUIStatusHunger(_float fTimeDelta);
	void ProcessUIStatusArmor(_float fTimeDelta);
	void ProcessUIStatusLevel(_float fTimeDelta);
	void ProcessUIStatusBreath(_float fTimeDelta);
	void ProcessHungerTimer(_float fTimeDelta);
	int32_t m_iHalfHealth{ 20 };// max: 20
	int32_t m_iHalfHunger{ 20 };// max: 20
	_float m_fHungerTimer{};
	int32_t m_iHalfArmor{ 0 };// max: 20
	int32_t m_iRealHealfArmor{ 0 };
	int32_t m_iLevel{ 0 };
	_float m_fExperienceGage{ 0.f };
	int32_t m_iBreath{ 10 }; // max: 10

private:
	void ProcessArmorEntities(_float fTimeDelta);
	std::array<CHandle, 4> m_hArmorEntities{};

	void ProcessPlayerOpenInvenArmorEntities(_float fTimeDelta);
	std::array<CHandle, 4> m_hPlayerOpenInvenArmorEntities{};

private:
	void ProcessPlayerOpenInvenAction(_float fTimeDelta);
	void ProcessPlayerCameraAction(_float fTimeDelta);


public:
	static UPtr<CPlayerEntity> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END