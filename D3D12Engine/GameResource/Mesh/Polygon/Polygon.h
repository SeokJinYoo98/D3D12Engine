#pragma once
struct Vertex {
	DirectX::XMFLOAT3 Position = { };
	DirectX::XMFLOAT2 UV = { };
	DirectX::XMFLOAT3 Normal = { };
};

struct SubPolygon {
	std::string name = "None";
	UINT	IndexCount = 0;
	UINT	StartIndexLocation = 0;
	INT		BaseVertexLocation = 0;
};

struct Polygons
{
	bool UseNorm = false;
	bool UseUV = false;

	UINT			VertexStride			= 0;
	UINT			VertexCount				= 0;

	DXGI_FORMAT		IndexFormat				= DXGI_FORMAT_R16_UINT;
	UINT			IndexCount				= 0;

	std::vector<Vertex>		Vertices;
	std::vector<uint32_t>	Indices32;
	std::vector<uint16_t>	Indices16;

	std::vector<SubPolygon> SubPolygons;
};

struct DiffusedPolygon 
{
	UINT			VertexStride = 0;
	UINT			VertexCount = 0;

	DXGI_FORMAT		IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT			IndexCount = 0;

	std::vector<float> Vertices;
	std::vector<uint16_t>	Indices16;
};