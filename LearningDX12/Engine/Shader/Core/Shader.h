#pragma once
#include "../../EngineMinimal.h"

class FShader
{
public:
	LPVOID GetBufferPointer();
	SIZE_T GetBufferSize();

	void BuildShaders(const wstring& InFileName, const string& InEntryFunName, const string& InShadersVersion);//±àÒëshader½Ó¿Ú
private:
	ComPtr<ID3DBlob> ShaderCode;//shader code
};