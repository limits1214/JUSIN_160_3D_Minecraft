#pragma once

#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Client)

class CLevelOverWorld final : public E::CLevel
{
private:
	explicit CLevelOverWorld();
	~CLevelOverWorld() override;

public:
	HRESULT Initialize() override;
	void Update(E::_float fTimeDelta) override;
	HRESULT Render() override;
	void UpdateGUI() override;
	void FrameStart(E::_float fTimeDelta) override;

public:
	static Engine::UPtr<CLevelOverWorld> Create();
};

NS_END