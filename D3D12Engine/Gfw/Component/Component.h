#pragma once
class CComponent
{
};

class CTransformComponent : public CComponent{
};
class CMovementComponent : public CComponent {
	friend class CTransformComponent;
};
class CMeshGeometry;
class CMatrial;

class CMeshComponent {
	
};
