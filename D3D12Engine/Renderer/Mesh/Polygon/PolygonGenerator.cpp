#include "Common\pch.h"
#include "PolygonGenerator.h"
#include "Common/AssetLoader/ObjModelLoader.h"

std::unique_ptr<Polygons> CPolygonGenerator::GenFromObjModel(const std::string& name)
{
	CObjModelLoader objLoader;
	auto data = objLoader.LoadObjModel(name);

	std::unordered_map<std::string, Vertex> vertexMap;
	std::unordered_map<std::string, std::vector<std::string>> subPolyMap;

	for (const auto& [name, poly] : data->pGroupDatas) {
		for (size_t idx = 0; idx < poly->Indices.size(); ++idx) {
			Vertex newVertex{ };
			std::string vertexName{ };
			vertexName += std::to_string(poly->Indices[idx]);
			newVertex.Position = data->Positions[poly->Indices[idx]];

			if (data->UseUV) {
				++idx;
				vertexName += std::to_string(poly->Indices[idx]);
				newVertex.UV = data->UVs[poly->Indices[idx]];
			}
			if (data->UseNorm) {
				++idx;
				vertexName += std::to_string(poly->Indices[idx]);
				newVertex.Normal = data->Normals[poly->Indices[idx]];
			}
			subPolyMap[name].push_back(vertexName);
			vertexMap[vertexName] = newVertex;
		}
	}

	auto newPoly = std::make_unique<Polygons>();
	newPoly->UseNorm = data->UseNorm;
	newPoly->UseUV = data->UseUV;
	newPoly->IndexFormat = SetIndexFormat(data->UseUint32);

	for (const auto& [name, v] : vertexMap) {
		newPoly->Vertices.emplace_back(v);
	}

	UINT	indexCount = 0;
	UINT	startIndexLocation = 0;
	INT		baseVertexLocation = 0;
	for (const auto& [subPolyName, indices] : subPolyMap) {
		SubPolygon newSubPoly;
		newSubPoly.name = subPolyName;
		newSubPoly.IndexCount = static_cast<UINT>(indices.size());
		newSubPoly.StartIndexLocation = startIndexLocation;
		newSubPoly.BaseVertexLocation = baseVertexLocation; 

		UINT maxVertexIndex = 0;
		for (const auto& index : indices) {
			auto it = vertexMap.find(index);
			if (it == vertexMap.end())
				throw std::runtime_error("Invaild Key");

			auto vertexIndex = std::distance(vertexMap.begin(), it);
			
			if (newPoly->IndexFormat == DXGI_FORMAT_R16_UINT)
				newPoly->Indices16.push_back(static_cast<uint16_t>(vertexIndex));
			else
				newPoly->Indices32.push_back(static_cast<uint32_t>(vertexIndex));

			if (vertexIndex > maxVertexIndex)
				maxVertexIndex = static_cast<UINT>(vertexIndex);
		}

		startIndexLocation = static_cast<UINT>(indices.size());
		baseVertexLocation = maxVertexIndex;

		newPoly->SubPolygons.push_back(newSubPoly);
	}


	newPoly->VertexStride	= static_cast<UINT>(sizeof(Vertex));
	newPoly->VertexCount	= static_cast<UINT>(newPoly->Vertices.size());
	if (newPoly->IndexFormat == DXGI_FORMAT_R16_UINT) 
		newPoly->IndexCount = static_cast<UINT>(newPoly->Indices16.size());
	
	else 
		newPoly->IndexCount = static_cast<UINT>(newPoly->Indices32.size());
	

	if (!newPoly)
		throw std::runtime_error("Invalid Ptr");

	return std::move(newPoly);
}

UINT CPolygonGenerator::CalcVertexStride(bool bNorm, bool bUV)
{
	UINT vertexStride = sizeof(DirectX::XMFLOAT3);
	if (bNorm)
		vertexStride += sizeof(DirectX::XMFLOAT3);
	if (bUV)
		vertexStride += sizeof(DirectX::XMFLOAT2);

	return vertexStride;
}

DXGI_FORMAT CPolygonGenerator::SetIndexFormat(bool useUint32)
{
	if (useUint32) {
		throw std::runtime_error("uint32_t 사용 불가 추후 추가 예정.");
		return DXGI_FORMAT_R32_UINT;
	}
		
	return DXGI_FORMAT_R16_UINT;
}
