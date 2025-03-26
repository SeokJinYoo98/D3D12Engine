#pragma once
class CObjModelLoader
{
	// [TODO] : 오브젝트 로더 생성하기.
public:
	struct PolyData {
		std::vector<uint32_t>				Indices;
	};
	struct ObjModelData {
		std::unordered_map<std::string, std::unique_ptr<PolyData>> pGroupDatas;
		std::vector<DirectX::XMFLOAT3>		Positions;
		std::vector<DirectX::XMFLOAT3>		Normals;
		std::vector<DirectX::XMFLOAT2>		UVs;
		uint32_t							MaxIndexNumber = 0;
		bool								UseNorm		= false;
		bool								UseUV		= false;
		bool								UseUint32	= false;
	};

	std::unique_ptr<CObjModelLoader::ObjModelData> LoadObjModel(const std::string& fileName);

private:
	void PrintData(CObjModelLoader::ObjModelData* data);
};

