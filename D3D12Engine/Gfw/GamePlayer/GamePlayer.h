#pragma once
#include "Gfw/GameObject/GameObject.h"

class CCamera;

// [피직스] 추가, 방향으로 움직이고, 업데이트로 갱신하게 변경
class CGamePlayer : public CGameObject
{
public:
	CGamePlayer(){ }

	void Update(float fElapsedTime) override;
	void ProcessInput(UCHAR* pKeysBuffers, float fElapsedTime);
	void SetCamera(std::shared_ptr<CCamera> pCamera);

	void MoveForward(float fDelta);
	void MoveBackward(float fDelta);
	void MoveLeft(float fDelta);
	void MoveRight(float fDelta);
	void MoveUp(float fDelta);
	void MoveDown(float fDelta);
	void TurnLeft(float fDelta);
	void TurnRight(float fDelta);
	void TurnUp(float fDelta);
	void TurnDown(float fDelta);
private:
	std::shared_ptr<CCamera> m_pCamera = nullptr;
	float m_fMoveSpeed = 10.f;
	float m_fRotateSpeed = 60.f;
};

