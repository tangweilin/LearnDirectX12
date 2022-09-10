#pragma once

#if defined(_WIN32)
#include "../../Core/Engine.h"

class FWindowsEngine : public FEngine
{
public:
	FWindowsEngine();
	~FWindowsEngine();

	virtual int PreInit(FWinMainCommandParameters InParameters);//����Ԥ��ʼ��
	virtual int Init();//�����ʼ��
	virtual int PostInit();//������ʼ��

	virtual void Tick();//��Ⱦ

	virtual int PreExit();//Ԥ�˳�
	virtual int Exit();//�˳�
	virtual int PostExit();//�˳�����

private:
	bool InitWindows(FWinMainCommandParameters InParameters);

	bool InitDirect3D();

protected:
	ComPtr<IDXGIFactory4> DXGIFactory;//ͨ��Factory������������ͼ�νṹ��������
	ComPtr<ID3D12Device> D3dDevice; //������������� �����б� ������� Fence ��Դ �ܵ�״̬���� �� ��ǩ�� �������������Դ��ͼ
	ComPtr<ID3D12Fence> Fence;//Χ�� ����CPU��GPUͬ��

	ComPtr<ID3D12CommandQueue> CommandQueue;//����
	ComPtr<ID3D12CommandAllocator> CommandAllocator; //�洢 ������
	ComPtr<ID3D12GraphicsCommandList> GraphicsCommandList;//�����б�

	ComPtr<IDXGISwapChain> SwapChain;

	//����������Ͷ�
	ComPtr<ID3D12DescriptorHeap> RTVHeap;
	ComPtr<ID3D12DescriptorHeap> DSVHeap;

	vector<ComPtr<ID3D12Resource>> SwapChainBuffer;
	ComPtr<ID3D12Resource> DepthStencilBuffer;

	//����Ļ���ӿ��й�
	D3D12_VIEWPORT ViewprotInfo;
	D3D12_RECT ViewprotRect;

	UINT64 CurrentFenceIndex;
	int CurrentSwapBuffIndex;

protected:
	HWND MainWindowsHandle;//��windows���
	UINT M4XQualityLevels;//MSAA��������
	bool bMSAA4XEnabled;//�Ƿ���4xMSAA
	DXGI_FORMAT BackBufferFormat;//��̨����
	DXGI_FORMAT DepthStencilFormat;//��Ȼ���
	UINT RTVDescriptorSize;//RenderTarget��Դ������С
};
#endif