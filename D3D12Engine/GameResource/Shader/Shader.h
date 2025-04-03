#pragma once
class CCamera;
class CShader 
{
public:
	CShader() = delete;
	CShader(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile);
	virtual ~CShader() { }

	ID3DBlob* GetpShaderBlob() { return m_pShaderBlob.Get(); }
protected:
	void CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile);
	
protected:
	Microsoft::WRL::ComPtr<ID3DBlob>				m_pShaderBlob;
};

