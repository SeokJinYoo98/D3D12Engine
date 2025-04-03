#include "Common\pch.h"
#include "GameComponent.h"

CStaticMeshComponent::CStaticMeshComponent()
{
}

DirectX::XMFLOAT4X4 CStaticMeshComponent::GetTransform()
{
	return m_gameTransform.GetTransform();
}
