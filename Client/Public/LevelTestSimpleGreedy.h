#pragma once
#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Client)

class CLevelTestSimpleGreedy final : public Engine::CLevel
{
private:
	explicit CLevelTestSimpleGreedy();
	~CLevelTestSimpleGreedy() override;

public:
	HRESULT Initialize() override;
	void Update(E::_float fTimeDelta) override;
	HRESULT Render() override;
	void UpdateGUI() override;
	void FrameStart(E::_float fTimeDelta) override;

public:
	static Engine::UPtr<CLevelTestSimpleGreedy> Create();

private:
	void Free() override;
};

NS_END