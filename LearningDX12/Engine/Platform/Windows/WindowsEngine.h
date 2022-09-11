#pragma once

#if defined(_WIN32)
#include "../../Core/Engine.h"

class FWindowsEngine : public FEngine
{
	friend class IRenderingInterface;
public:
	FWindowsEngine();
	~FWindowsEngine();

	virtual int PreInit(FWinMainCommandParameters InParameters);//引擎预初始化
	virtual int Init(FWinMainCommandParameters InParameters);//引擎初始化
	virtual int PostInit();//结束初始化

	virtual void Tick(float DeltaTime);//渲染

	virtual int PreExit();//预退出
	virtual int Exit();//退出
	virtual int PostExit();//退出结束

public:
	ID3D12Resource* GetCurrentSwapBuff() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentSwapBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentDepthStencilView() const;

protected:
	void WaitGPUCommandQueueComplete();

private:
	bool InitWindows(FWinMainCommandParameters InParameters);

	bool InitDirect3D();
	void PostInitDirect3D();

protected:
	ComPtr<IDXGIFactory4> DXGIFactory;//通过Factory创建适配器等图形结构基础对象
	ComPtr<ID3D12Device> D3dDevice; //创建命令分配器 命令列表 命令队列 Fence 资源 管道状态对象 堆 根签名 采样器和许多资源视图
	ComPtr<ID3D12Fence> Fence;//围栏 用于CPU与GPU同步

	ComPtr<ID3D12CommandQueue> CommandQueue;//队列
	ComPtr<ID3D12CommandAllocator> CommandAllocator; //存储 分配器
	ComPtr<ID3D12GraphicsCommandList> GraphicsCommandList;//命令列表

	ComPtr<IDXGISwapChain> SwapChain;

	vector<ComPtr<ID3D12Resource>> SwapChainBuffer; //交换链buffer
	ComPtr<ID3D12Resource> DepthStencilBuffer; //深度/模板 buffer

	//渲染视口
	D3D12_VIEWPORT ViewprotInfo;
	//裁剪框
	D3D12_RECT ViewprotRect;

	UINT64 CurrentFenceIndex;
	int CurrentSwapBuffIndex;

	//描述符对象和堆
	ComPtr<ID3D12DescriptorHeap> RTVHeap;
	ComPtr<ID3D12DescriptorHeap> DSVHeap;

protected:
	HWND MainWindowsHandle;//主windows句柄
	UINT M4XQualityLevels;//MSAA质量级别
	bool bMSAA4XEnabled;//是否开启4xMSAA
	DXGI_FORMAT BackBufferFormat;//后缓冲区格式
	DXGI_FORMAT DepthStencilFormat;//深度模板缓冲格式
	UINT RTVDescriptorSize;//RTV资源描述大小
};
#endif