#pragma once
#include "../../Core/CoreObject/GuidInterface.h"
#include "../../Core/Engine.h"
#if defined(_WIN32)
#include "../../Platform/Windows/WindowsEngine.h"
#else
#endif
//提供渲染内容的接口
class IRenderingInterface
{
	friend class CDirectXRenderingEngine;
public:
	IRenderingInterface();
	virtual ~IRenderingInterface();

	virtual void Init();

	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);
	virtual void PostDraw(float DeltaTime);
protected:
	//资源类型Buffer创建
	ComPtr<ID3D12Resource> ConstructDefaultBuffer(ComPtr<ID3D12Resource>& OutTmpBuffer, const void* InData, UINT64 InDataSize);
protected:
	ComPtr<ID3D12Device> GetD3dDevice(); //获得D3D驱动
	ComPtr<ID3D12GraphicsCommandList> GetGraphicsCommandList();//获得命令队列
	ComPtr<ID3D12CommandAllocator> GetCommandAllocator();//获得命令分配器
#if defined(_WIN32)
	CWindowsEngine* GetEngine();//获得DX引擎
#else
	CEngine* GetEngine();
#endif
};