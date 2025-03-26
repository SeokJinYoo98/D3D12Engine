#include "Common\pch.h"
#include "ObjModelLoader.h"

#include <fstream>
#include <sstream>

std::unique_ptr<CObjModelLoader::ObjModelData> CObjModelLoader::LoadObjModel(const std::string& fileName)
{

	std::string filePath = "assets/Models/ObjFile/" + fileName + ".obj";

	std::ifstream inFile(filePath);
	if (!inFile)
		throw std::runtime_error("Invalid FilePath:" + filePath);

	std::string line;
	std::string currGroupName = "None";

	auto newData = std::make_unique<CObjModelLoader::ObjModelData>();

	while (std::getline(inFile, line))
	{
		std::istringstream iss(line);
		std::string prefix;
		iss >> prefix;
		if (prefix == "g" || prefix == "o")
		{
			std::string groupName;
			std::getline(iss >> std::ws, groupName);
			if (groupName != "off" && groupName != currGroupName) {
				newData->pGroupDatas[groupName] = std::make_unique<PolyData>();
				currGroupName = groupName;
			}
		}
		else if (prefix == "v")
		{
			float vx, vy, vz;
			iss >> vx >> vy >> vz;
			newData->Positions.emplace_back(vx, vy, vz);

		}
		else if (prefix == "vt")
		{
			float u, v;
			iss >> u >> v;
			newData->UVs.emplace_back(u, v);
			if (!newData->UseUV) newData->UseUV = true;
		}
		else if (prefix == "vn")
		{
			float vnx, vny, vnz;
			iss >> vnx >> vny >> vnz;
			newData->Normals.emplace_back(vnx, vny, vnz);
			if (!newData->UseNorm) newData->UseNorm = true;
		}
		else if (prefix == "f")
		{
			std::string faceGroup;
			std::getline(iss, faceGroup);
			int slashCnt = static_cast<int>(std::count(faceGroup.begin(), faceGroup.end(), '/'));
			if (6 < slashCnt) {
				// 추후 분할
				throw std::runtime_error("Rect Base File");
			}
				
			
			std::replace(faceGroup.begin(), faceGroup.end(), '/', ' ');

			std::istringstream tokenStream(faceGroup);
			uint32_t indexValue;

			while (tokenStream >> indexValue) {
				--indexValue;
				if (newData->MaxIndexNumber < indexValue)
					newData->MaxIndexNumber = indexValue;
				newData->pGroupDatas[currGroupName]->Indices.push_back(indexValue);
			}
		}
	}
	if (newData->MaxIndexNumber >= (std::numeric_limits<uint16_t>::max)())
		newData->UseUint32 = true;

	//PrintData(newData.get());
	return std::move(newData);
}

void CObjModelLoader::PrintData(CObjModelLoader::ObjModelData* data)
{
}
