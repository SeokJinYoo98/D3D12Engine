#include "Common\pch.h"
#include "GamePlayer.h"
#include "Gfw/Camera/Camera.h"	

void CGamePlayer::Update(float fElapsedTime)
{
	if (m_pCamera) {
		m_pCamera->SetCameraPos(m_pGameTransform->GetPosition());
		m_pCamera->SetCameraLookAt(m_pGameTransform->GetPosPlusForward());
	}
}

void CGamePlayer::ProcessInput(UCHAR* pKeysBuffers, float fElapsedTime)
{
	float fMvSpeed = m_fMoveSpeed * fElapsedTime;
	float fRtSpeed = m_fRotateSpeed * fElapsedTime;

	if (pKeysBuffers[0x57] & 0x80)  // W key
		MoveForward(fMvSpeed);
	if (pKeysBuffers[0x53] & 0x80)  // S key
		MoveBackward(fMvSpeed);
	if (pKeysBuffers[0x41] & 0x80)  // A key
		MoveLeft(fMvSpeed);
	if (pKeysBuffers[0x44] & 0x80)  // D key
		MoveRight(fMvSpeed);
	if (pKeysBuffers[VK_SPACE] & 0x80)  // Space key
		MoveUp(fMvSpeed);
	if (pKeysBuffers[VK_SHIFT] & 0x80)  // Shift key
		MoveDown(fMvSpeed);
	if (pKeysBuffers[VK_LEFT] & 0x80)
		TurnLeft(fRtSpeed);
	if (pKeysBuffers[VK_RIGHT] & 0x80)
		TurnRight(fRtSpeed);
	if (pKeysBuffers[VK_UP] & 0x80)
		TurnUp(fRtSpeed);
	if (pKeysBuffers[VK_DOWN] & 0x80)
		TurnDown(fRtSpeed);
}

void CGamePlayer::SetCamera(std::shared_ptr<CCamera> pCamera)
{
	m_pCamera = pCamera;

	m_pCamera->SetCameraPos(m_pGameTransform->GetPosition());
	m_pCamera->SetCameraLookAt(m_pGameTransform->GetPosPlusForward());
	m_pCamera->SetCameraUp(Direction::UP);
}

void CGamePlayer::MoveForward(float fDelta)
{
	auto deltaMove = Vector3::ScalarProduct(m_pGameTransform->GetForwardVector(), fDelta, false);
	m_pGameTransform->Translate(deltaMove);
}

void CGamePlayer::MoveBackward(float fDelta)
{
	auto deltaMove = Vector3::ScalarProduct(m_pGameTransform->GetBackwardVector(), fDelta, false);
	m_pGameTransform->Translate(deltaMove);
}

void CGamePlayer::MoveLeft(float fDelta)
{
	auto deltaMove = Vector3::ScalarProduct(m_pGameTransform->GetLeftVector(), fDelta, false);
	m_pGameTransform->Translate(deltaMove);
}

void CGamePlayer::MoveRight(float fDelta)
{
	auto deltaMove = Vector3::ScalarProduct(m_pGameTransform->GetRightVector(), fDelta, false);
	m_pGameTransform->Translate(deltaMove);
}

void CGamePlayer::MoveUp(float fDelta)
{
	auto deltaMove = Vector3::ScalarProduct(Direction::UP, fDelta, false);
	m_pGameTransform->Translate(deltaMove);
}

void CGamePlayer::MoveDown(float fDelta)
{
	auto deltaMove = Vector3::ScalarProduct(Direction::DOWN, fDelta, false);
	m_pGameTransform->Translate(deltaMove);
}

void CGamePlayer::TurnLeft(float fDelta)
{
	auto deltaMove = Vector3::ScalarProduct(Direction::AXIS_Y, -fDelta, false);
	m_pGameTransform->Rotate(deltaMove);
}

void CGamePlayer::TurnRight(float fDelta)
{
	auto deltaMove = Vector3::ScalarProduct(Direction::AXIS_Y, +fDelta, false);
	m_pGameTransform->Rotate(deltaMove);
}

void CGamePlayer::TurnUp(float fDelta)
{
	auto deltaMove = Vector3::ScalarProduct(Direction::AXIS_X, -fDelta, false);
	m_pGameTransform->Rotate(deltaMove);
}

void CGamePlayer::TurnDown(float fDelta)
{
	auto deltaMove = Vector3::ScalarProduct(Direction::AXIS_X, +fDelta, false);
	m_pGameTransform->Rotate(deltaMove);
}
