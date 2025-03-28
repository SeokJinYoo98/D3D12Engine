#pragma once
#include "Common/GameTransform/GameTransform.h"
struct Light {
	DirectX::XMFLOAT4X4 xmf4x4Transform;
	DirectX::XMFLOAT3	xmf3Color;
	float				fIntensity;
};

class CGameLight {
public:
	CGameLight() {
		m_gameTransform = { };
		m_xmf3Color = { 1.f, 1.f, 1.f };
		m_fIntensity = 1.f;
	}
	Light GetLightData() {
		Light data;
		data.xmf4x4Transform	= Matrix4x4::Transpose(m_gameTransform.GetTransform());
		data.xmf3Color			= m_xmf3Color;
		data.fIntensity			= m_fIntensity;
		return data;
	}
	CGameTransform		m_gameTransform;	// 위치, 방향
protected:
	DirectX::XMFLOAT3	m_xmf3Color;		// 컬러
	float				m_fIntensity;		// 강도
};

class CGameSpotLight {

};
