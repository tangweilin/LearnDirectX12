#pragma once
#include "../../Core/CoreObject/GuidInterface.h"
#include "../../Core/Engine.h"
#if defined(_WIN32)
#include "../../Platform/Windows/WindowsEngine.h"
#else
#endif


//渲染内容的接口类
class IRenderingInterface :public IGuidInterface
{
	friend class CWindowsEngine;

public:
	IRenderingInterface();
	virtual ~IRenderingInterface();

	virtual void Init();

	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);
	virtual void PostDraw(float DeltaTime);
protected:
	ComPtr<ID3D12Device> GetD3dDevice();
	ComPtr<ID3D12GraphicsCommandList> GetGraphicsCommandList();
	ComPtr<ID3D12CommandAllocator> GetCommandAllocator();

protected:
	//资源类型Buffer创建
	ComPtr<ID3D12Resource> ConstructDefaultBuffer(ComPtr<ID3D12Resource>& OutTmpBuffer, const void* InData, UINT64 InDataSize);

#if defined(_WIN32)
	CWindowsEngine* GetEngine();
#else
	CEngine* GetEngine();
#endif

	

private:
	static vector<IRenderingInterface*> RenderingInterface;
};
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

	ID3D12Resource* GetBuffer() { return UploadBuffer.Get(); } //获得Buffer
private:
	ComPtr<ID3D12Resource> UploadBuffer;//上传Buffer
	UINT ElementSize;//元素大小
	BYTE* Data;//实际数据
};