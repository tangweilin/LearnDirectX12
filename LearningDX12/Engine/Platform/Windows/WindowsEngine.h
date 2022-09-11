#pragma once

#if defined(_WIN32)
#include "../../Core/Engine.h"

class FWindowsEngine : public FEngine
{
	friend class IRenderingInterface;
public:
	FWindowsEngine();
	~FWindowsEngine();

	virtual int PreInit(FWinMainCommandParameters InParameters);//����Ԥ��ʼ��
	virtual int Init(FWinMainCommandParameters InParameters);//�����ʼ��
	virtual int PostInit();//������ʼ��

	virtual void Tick(float DeltaTime);//��Ⱦ

	virtual int PreExit();//Ԥ�˳�
	virtual int Exit();//�˳�
	virtual int PostExit();//�˳�����

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
	ComPtr<IDXGIFactory4> DXGIFactory;//ͨ��Factory������������ͼ�νṹ��������
	ComPtr<ID3D12Device> D3dDevice; //������������� �����б� ������� Fence ��Դ �ܵ�״̬���� �� ��ǩ�� �������������Դ��ͼ
	ComPtr<ID3D12Fence> Fence;//Χ�� ����CPU��GPUͬ��

	ComPtr<ID3D12CommandQueue> CommandQueue;//����
	ComPtr<ID3D12CommandAllocator> CommandAllocator; //�洢 ������
	ComPtr<ID3D12GraphicsCommandList> GraphicsCommandList;//�����б�

	ComPtr<IDXGISwapChain> SwapChain;

	vector<ComPtr<ID3D12Resource>> SwapChainBuffer; //������buffer
	ComPtr<ID3D12Resource> DepthStencilBuffer; //���/ģ�� buffer

	//��Ⱦ�ӿ�
	D3D12_VIEWPORT ViewprotInfo;
	//�ü���
	D3D12_RECT ViewprotRect;

	UINT64 CurrentFenceIndex;
	int CurrentSwapBuffIndex;

	//����������Ͷ�
	ComPtr<ID3D12DescriptorHeap> RTVHeap;
	ComPtr<ID3D12DescriptorHeap> DSVHeap;

protected:
	HWND MainWindowsHandle;//��windows���
	UINT M4XQualityLevels;//MSAA��������
	bool bMSAA4XEnabled;//�Ƿ���4xMSAA
	DXGI_FORMAT BackBufferFormat;//�󻺳�����ʽ
	DXGI_FORMAT DepthStencilFormat;//���ģ�建���ʽ
	UINT RTVDescriptorSize;//RTV��Դ������С
};
#endif