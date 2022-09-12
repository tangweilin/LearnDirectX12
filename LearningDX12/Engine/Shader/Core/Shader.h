#pragma once
#include "../../EngineMinimal.h"

class FShader
{
public:
	LPVOID GetBufferPointer();//shader的指针
	SIZE_T GetBufferSize();//shader的size

	void BuildShaders(const wstring& InFileName, const string& InEntryFunName, const string& InShadersVersion);//编译shader接口
private:
	ComPtr<ID3DBlob> ShaderCode;//shader code
};