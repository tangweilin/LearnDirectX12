#pragma once
#include "../../EngineMinimal.h"

//常量缓冲区存储结构
class FRenderingResourcesUpdate :public enable_shared_from_this<FRenderingResourcesUpdate>
{
public:
	FRenderingResourcesUpdate();
	~FRenderingResourcesUpdate();

	void Init(ID3D12Device* InDevice, UINT InElemetSize, UINT InElemetCount);//初始化创建资源

	void Update(int Index, const void* InData);//更新数据

	UINT GetConstantBufferByteSize(UINT InTypeSzie);//获得常量缓冲字节Size
	UINT GetConstantBufferByteSize();

	ID3D12Resource* GetBuffer() { return UploadBuffer.Get(); }//获得Buffer
private:
	ComPtr<ID3D12Resource> UploadBuffer;//上传Buffer
	UINT ElementSize;//元素大小
	BYTE* Data;//实际数据
};