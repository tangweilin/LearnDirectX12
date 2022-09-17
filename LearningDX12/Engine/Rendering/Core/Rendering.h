#pragma once
#include "../../Core/CoreObject/GuidInterface.h"
#include "../../Core/Engine.h"
#if defined(_WIN32)
#include "../../Platform/Windows/WindowsEngine.h"
#else
#endif
//�ṩ��Ⱦ���ݵĽӿ�
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
	//��Դ����Buffer����
	ComPtr<ID3D12Resource> ConstructDefaultBuffer(ComPtr<ID3D12Resource>& OutTmpBuffer, const void* InData, UINT64 InDataSize);
protected:
	ComPtr<ID3D12Device> GetD3dDevice(); //���D3D����
	ComPtr<ID3D12GraphicsCommandList> GetGraphicsCommandList();//����������
	ComPtr<ID3D12CommandAllocator> GetCommandAllocator();//������������
#if defined(_WIN32)
	CWindowsEngine* GetEngine();//���DX����
#else
	CEngine* GetEngine();
#endif
};