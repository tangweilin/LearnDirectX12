#pragma once
#include "../../EngineMinimal.h"

//�����������洢�ṹ
class FRenderingResourcesUpdate :public enable_shared_from_this<FRenderingResourcesUpdate>
{
public:
	FRenderingResourcesUpdate();
	~FRenderingResourcesUpdate();

	void Init(ID3D12Device* InDevice, UINT InElemetSize, UINT InElemetCount);//��ʼ��������Դ

	void Update(int Index, const void* InData);//��������

	UINT GetConstantBufferByteSize(UINT InTypeSzie);//��ó��������ֽ�Size
	UINT GetConstantBufferByteSize();

	ID3D12Resource* GetBuffer() { return UploadBuffer.Get(); }//���Buffer
private:
	ComPtr<ID3D12Resource> UploadBuffer;//�ϴ�Buffer
	UINT ElementSize;//Ԫ�ش�С
	BYTE* Data;//ʵ������
};