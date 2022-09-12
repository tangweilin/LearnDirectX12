#pragma once
#include "../../EngineMinimal.h"

class FShader
{
public:
	LPVOID GetBufferPointer();//shader��ָ��
	SIZE_T GetBufferSize();//shader��size

	void BuildShaders(const wstring& InFileName, const string& InEntryFunName, const string& InShadersVersion);//����shader�ӿ�
private:
	ComPtr<ID3DBlob> ShaderCode;//shader code
};