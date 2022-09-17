#include "RenderingResourcesUpdate.h"

FRenderingResourcesUpdate::FRenderingResourcesUpdate()
{

}

FRenderingResourcesUpdate::~FRenderingResourcesUpdate()
{
	if (UploadBuffer != nullptr)
	{
		UploadBuffer->Unmap(0, NULL);//取消映射
		UploadBuffer = nullptr;
	}
}

void FRenderingResourcesUpdate::Init(ID3D12Device* InDevice, UINT InElemetSize, UINT InElemetCount)
{
	assert(InDevice);//断言 必须存在驱动

	ElementSize = GetConstantBufferByteSize(InElemetSize);

	CD3DX12_HEAP_PROPERTIES HeapPropertie = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);//上传堆
	CD3DX12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(ElementSize * InElemetCount);//资源描述
	//创建buffer
	ANALYSIS_HRESULT(InDevice->CreateCommittedResource(
		&HeapPropertie,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(&UploadBuffer)));
	//buffer映射
	ANALYSIS_HRESULT(UploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&Data)));
}

void FRenderingResourcesUpdate::Update(int Index, const void* InData)
{
	//通过地址偏移找到首地址 然后将数据拷贝过去
	memcpy(&Data[Index * ElementSize], InData, ElementSize);
}

UINT FRenderingResourcesUpdate::GetConstantBufferByteSize(UINT InTypeSzie)
{
	//缓冲区大小必须为256的倍数
	//(InTypeSzie + 255)& ~255;

	/*if (!(InTypeSzie % 256))
	{
		float NewFloat =  (float) InTypeSzie / 256.f;
		int Num = (NewFloat += 1);
		InTypeSzie = Num * 256;
	}*/
	//~=取反
	//456
	//(456 + 255) & ~255;
	//711 & ~255;
	//0x02c7 & ~0x00ff
	//0x02c7 & 0xff00
	//0x0200
	return (InTypeSzie + 255) & ~255;
}

UINT FRenderingResourcesUpdate::GetConstantBufferByteSize()
{
	return GetConstantBufferByteSize(ElementSize);
}
