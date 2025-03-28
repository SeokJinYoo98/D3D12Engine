#include "Common\pch.h"
#include "Renderer/Shader/Shader.h"

void CShader::CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile)
{
	UINT nComPileFlags{ };
#if defined(_DEBUG)
	nComPileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	Microsoft::WRL::ComPtr<ID3DBlob> pErrorBlob;

	HRESULT hResult = ::D3DCompileFromFile(
		pszFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		pszShaderName, pszShaderProfile,
		nComPileFlags, 0, m_pShaderBlob.GetAddressOf(), pErrorBlob.GetAddressOf()
	);

	if (FAILED(hResult))
	{
		if (pErrorBlob)
		{
			::OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			std::cout << (char*)pErrorBlob->GetBufferPointer() << std::endl;
		}

		throw std::runtime_error("Shader compilation failed.");
	}
}

CShader::CShader(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile)
{
	CompileShaderFromFile(pszFileName, pszShaderName, pszShaderProfile);
}
