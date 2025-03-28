#include "Common\pch.h"
#include "GameComponent.h"

CStaticMeshComponent::CStaticMeshComponent()
{
}

DirectX::XMFLOAT4X4 CStaticMeshComponent::GetTransform()
{
	return m_gameTransform.GetTransform();
}

const DirectX::XMFLOAT4& CStaticMeshComponent::GetColor() const
{
	return m_xmf4Color;
}
