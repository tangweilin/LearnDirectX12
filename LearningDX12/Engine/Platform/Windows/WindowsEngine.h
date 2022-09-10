#pragma once

#if defined(_WIN32)
#include "../../Core/Engine.h"

class FWindowsEngine : public FEngine
{
public:
	FWindowsEngine();
	~FWindowsEngine();

	virtual int PreInit(FWinMainCommandParameters InParameters);//引擎预初始化
	virtual int Init();//引擎初始化
	virtual int PostInit();//结束初始化

	virtual void Tick();//渲染

	virtual int PreExit();//预退出
	virtual int Exit();//退出
	virtual int PostExit();//退出结束

private:
	bool InitWindows(FWinMainCommandParameters InParameters);

	bool InitDirect3D();

protected:
	ComPtr<IDXGIFactory4> DXGIFactory;//通过Factory创建适配器等图形结构基础对象
	ComPtr<ID3D12Device> D3dDevice; //创建命令分配器 命令列表 命令队列 Fence 资源 管道状态对象 堆 根签名 采样器和许多资源视图
	ComPtr<ID3D12Fence> Fence;//围栏 用于CPU与GPU同步

	ComPtr<ID3D12CommandQueue> CommandQueue;//队列
	ComPtr<ID3D12CommandAllocator> CommandAllocator; //存储 分配器
	ComPtr<ID3D12GraphicsCommandList> GraphicsCommandList;//命令列表

	ComPtr<IDXGISwapChain> SwapChain;

	//描述符对象和堆
	ComPtr<ID3D12DescriptorHeap> RTVHeap;
	ComPtr<ID3D12DescriptorHeap> DSVHeap;

	vector<ComPtr<ID3D12Resource>> SwapChainBuffer;
	ComPtr<ID3D12Resource> DepthStencilBuffer;

	//和屏幕的视口有关
	D3D12_VIEWPORT ViewprotInfo;
	D3D12_RECT ViewprotRect;

	UINT64 CurrentFenceIndex;
	int CurrentSwapBuffIndex;

protected:
	HWND MainWindowsHandle;//主windows句柄
	UINT M4XQualityLevels;//MSAA质量级别
	bool bMSAA4XEnabled;//是否开启4xMSAA
	DXGI_FORMAT BackBufferFormat;//后台缓冲
	DXGI_FORMAT DepthStencilFormat;//深度缓冲
	UINT RTVDescriptorSize;//RenderTarget资源描述大小
};
#endif