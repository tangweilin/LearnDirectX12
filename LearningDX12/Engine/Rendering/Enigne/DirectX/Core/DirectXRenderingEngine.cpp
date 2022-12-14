#include "DirectXRenderingEngine.h"

#include "../../../../Debug/EngineDebug.h"
#include "../../../../Config/EngineRenderConfig.h"
#include "../../../../Rendering/Core/Rendering.h"
#include "../../../../Mesh/BoxMesh.h"
#include "../../../../Mesh/SphereMesh.h"
#include "../../../../Mesh/CylinderMesh.h"
#include "../../../../Mesh/ConeMesh.h"
#include "../../../../Mesh/PlaneMesh.h"
#include "../../../../Mesh/CustomMesh.h"
#include "../../../../Core/CoreObject/CoreMinimalObject.h"
#include "../../../../Core/World.h"
#include "../../../../Mesh/Core/MeshManager.h"

#if defined(_WIN32)
#include "../../../../Core/WinMainCommandParameters.h"

//class FVector
//{
//	unsigned char r;//255 ->[0,1]
//	unsigned char g;//255
//	unsigned char b;//255
//	unsigned char a;//255
//};

CDirectXRenderingEngine::CDirectXRenderingEngine()
	:CurrentFenceIndex(0)
	, M4XQualityLevels(0)
	, bMSAA4XEnabled(false)
	, BackBufferFormat(DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM)
	, DepthStencilFormat(DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT)
	, CurrentSwapBuffIndex(0)
{
	for (int i = 0; i < FEngineRenderConfig::GetRenderConfig()->SwapChainCount; i++)
	{
		SwapChainBuffer.push_back(ComPtr<ID3D12Resource>());
	}

	bTick = false;

	MeshManager = new CMeshManager();//??ʼ??Mesh
}

CDirectXRenderingEngine::~CDirectXRenderingEngine()
{
	delete MeshManager;
}

int CDirectXRenderingEngine::PreInit(FWinMainCommandParameters InParameters)
{
	Engine_Log("DirectXRenderingEngine pre initialization complete.");
	return 0;
}

int CDirectXRenderingEngine::Init(FWinMainCommandParameters InParameters)
{
	InitDirect3D();

	PostInitDirect3D();

	MeshManager->Init();

	Engine_Log("DirectXRenderingEngine initialization complete.");

	return 0;
}

int CDirectXRenderingEngine::PostInit()
{
	Engine_Log("Engine post initialization complete.");

	ANALYSIS_HRESULT(GraphicsCommandList->Reset(CommandAllocator.Get(), NULL));
	{
		//????Mesh
	//	CBoxMesh* Box = CBoxMesh::CreateMesh();
		//MeshManage->CreateBoxMesh(4.f,3.f,1.5f);
		//MeshManage->CreatePlaneMesh(4.f, 3.f, 20, 20);
			string MeshObjPath = "../LearningDX12/Mesh/pig_head.obj";
			MeshManager->CreateMesh(MeshObjPath);
		//	CSphereMesh* SphereMesh = CSphereMesh::CreateMesh(2.f, 20, 20);
		//	CCylinderMesh* CylinderMesh = CCylinderMesh::CreateMesh(1.f,1.f,5.f,20,20);
		//	CConeMesh* ConeMesh = CConeMesh::CreateMesh(1.f, 5.f, 20, 20);
	}

	ANALYSIS_HRESULT(GraphicsCommandList->Close());//?ر?????????

	ID3D12CommandList* CommandList[] = { GraphicsCommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CommandList), CommandList);

	WaitGPUCommandQueueComplete();

	return 0;
}

void CDirectXRenderingEngine::Tick(float DeltaTime)
{
	//???????????????ڴ棬Ϊ??һ֡??׼??
	ANALYSIS_HRESULT(CommandAllocator->Reset());

	MeshManager->PreDraw(DeltaTime);

	//?ж????ȴ???????ȾĿ??????Դ?Ƿ??????˴?Present???ύ??״̬?л???Render Target????ȾĿ?꣩״̬??
	CD3DX12_RESOURCE_BARRIER ResourceBarrierPresent = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentSwapBuff(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	GraphicsCommandList->ResourceBarrier(1, &ResourceBarrierPresent);

	//??Ҫÿִ֡??
	//?󶨾??ο?
	GraphicsCommandList->RSSetViewports(1, &ViewprotInfo);
	GraphicsCommandList->RSSetScissorRects(1, &ViewprotRect);

	//????RTV ????
	GraphicsCommandList->ClearRenderTargetView(GetCurrentSwapBufferView(),
		DirectX::Colors::Black,
		0, nullptr);

	//????????ģ?建????
	GraphicsCommandList->ClearDepthStencilView(GetCurrentDepthStencilView(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.f, 0, 0, NULL);

	//??RTV??DSV???????????ĺϲ??׶? 
	D3D12_CPU_DESCRIPTOR_HANDLE SwapBufferView = GetCurrentSwapBufferView();
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView = GetCurrentDepthStencilView();
	GraphicsCommandList->OMSetRenderTargets(1, &SwapBufferView,
		true, &DepthStencilView);

	//??Mesh
	MeshManager->Draw(DeltaTime);
	MeshManager->PostDraw(DeltaTime);

	//?ж????ȴ???????ȾĿ??????Դ?Ƿ??????˴?Render Target????ȾĿ?꣩״̬ ?л??? Present???ύ??״̬??
	CD3DX12_RESOURCE_BARRIER ResourceBarrierPresentRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentSwapBuff(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	GraphicsCommandList->ResourceBarrier(1, &ResourceBarrierPresentRenderTarget);

	//????¼??????
	ANALYSIS_HRESULT(GraphicsCommandList->Close());

	//?ύ??ִ??????
	ID3D12CommandList* CommandList[] = { GraphicsCommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CommandList), CommandList);

	//????????buff??????
	ANALYSIS_HRESULT(SwapChain->Present(0, 0));
	CurrentSwapBuffIndex = !(bool)CurrentSwapBuffIndex;

	//CPU??GPUͬ??
	WaitGPUCommandQueueComplete();
}

int CDirectXRenderingEngine::PreExit()
{


	Engine_Log("Engine post exit complete.");
	return 0;
}

int CDirectXRenderingEngine::Exit()
{

	Engine_Log("Engine exit complete.");
	return 0;
}

int CDirectXRenderingEngine::PostExit()
{
	FEngineRenderConfig::Destroy();//?ͷ?Config

	Engine_Log("Engine post exit complete.");
	return 0;
}
//???õ?ǰ?Ľ?????buffer
ID3D12Resource* CDirectXRenderingEngine::GetCurrentSwapBuff() const
{
	return SwapChainBuffer[CurrentSwapBuffIndex].Get();
}
//???õ?ǰ??RTV
D3D12_CPU_DESCRIPTOR_HANDLE CDirectXRenderingEngine::GetCurrentSwapBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		RTVHeap->GetCPUDescriptorHandleForHeapStart(),
		CurrentSwapBuffIndex, RTVDescriptorSize);
}
//???õ?ǰ??DSV
D3D12_CPU_DESCRIPTOR_HANDLE CDirectXRenderingEngine::GetCurrentDepthStencilView() const
{
	return DSVHeap->GetCPUDescriptorHandleForHeapStart();
}

UINT CDirectXRenderingEngine::GetDXGISampleCount() const
{
	return bMSAA4XEnabled ? 4 : 1;;
}

UINT CDirectXRenderingEngine::GetDXGISampleQuality() const
{
	return bMSAA4XEnabled ? (M4XQualityLevels - 1) : 0;
}

void CDirectXRenderingEngine::WaitGPUCommandQueueComplete()
{
	CurrentFenceIndex++;

	//??GUP?????µĸ????? ?ȴ?GPU???????ź?
	ANALYSIS_HRESULT(CommandQueue->Signal(Fence.Get(), CurrentFenceIndex));

	if (Fence->GetCompletedValue() < CurrentFenceIndex)
	{
		//??????????һ???¼??ں˶???,?????ظ??ں˶????ľ???.
		//SECURITY_ATTRIBUTES
		//CREATE_EVENT_INITIAL_SET  0x00000002
		//CREATE_EVENT_MANUAL_RESET 0x00000001
		//ResetEvents
		HANDLE EventEX = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);

		//GPU???ɺ???֪ͨ???ǵ?Handle
		ANALYSIS_HRESULT(Fence->SetEventOnCompletion(CurrentFenceIndex, EventEX));

		//?ȴ?GPU,???????߳?
		WaitForSingleObject(EventEX, INFINITE);
		CloseHandle(EventEX);
	}
}

bool CDirectXRenderingEngine::InitDirect3D()
{
	//D3D12Debug
	ComPtr<ID3D12Debug> D3D12Debug;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&D3D12Debug))))
	{
		D3D12Debug->EnableDebugLayer();
	}

	//HRESULT
	//S_OK				0x00000000
	//E_UNEXPECTED		0x8000FFFF ??????ʧ??
	//E_NOTIMPL			0x80004001 δʵ??
	//E_OUTOFMEMORY		0x8007000E δ?ܷ??????????ڴ?
	//E_INVALIDARG		0x80070057 һ??????????????Ч 
	//E_NOINTERFACE		0x80004002 ??֧?ִ˽ӿ?
	//E_POINTER			0x80004003 ??Чָ??
	//E_HANDLE			0x80070006 ??Ч????
	//E_ABORT			0x80004004 ??????ֹ
	//E_FAIL			0x80004005 ????
	//E_ACCESSDENIED	0x80070005 һ???ķ??ʱ??ܾ?????
	ANALYSIS_HRESULT(CreateDXGIFactory1(IID_PPV_ARGS(&DXGIFactory)));

	/*
	D3D_FEATURE_LEVEL_9_1  Ŀ?깦?ܼ???֧??Direct3D 9.1???? shader model 2.
	D3D_FEATURE_LEVEL_9_2  Ŀ?깦?ܼ???֧??Direct3D 9.2???? shader model 2.
	D3D_FEATURE_LEVEL_9_3  Ŀ?깦?ܼ???֧??Direct3D 9.3???? shader model 3.
	D3D_FEATURE_LEVEL_10_0 Ŀ?깦?ܼ???֧??Direct3D 10.0???? shader model 4.
	D3D_FEATURE_LEVEL_10_1 Ŀ?깦?ܼ???֧??Direct3D 10.1???? shader model 4.
	D3D_FEATURE_LEVEL_11_0 Ŀ?깦?ܼ???֧??Direct3D 11.0???? shader model 5.
	*/
	//ʹ??Ĭ?ϵ???????  ????FEATURE_LevelΪ11.0  
	HRESULT D3dDeviceResult = D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3dDevice));
	if (FAILED(D3dDeviceResult))
	{
		//warp ????ͨ??Ӳ????????????ʧ?? ??????????????
		ComPtr<IDXGIAdapter> WARPAdapter;//??????????
		ANALYSIS_HRESULT(DXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&WARPAdapter)));//ö????????
		ANALYSIS_HRESULT(D3D12CreateDevice(WARPAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3dDevice)));//????Device
	}

	//????Χ??
    //D3D12_FENCE_FLAG_NONE 
    //D3D11_FENCE_FLAG_SHARED
    //D3D11_FENCE_FLAG_SHARED_CROSS_ADAPTER
    /*
    Fence->SetEventOnCompletion
    ִ??????
    ?ύ????
    Queue->Signal
    wait
    */
	ANALYSIS_HRESULT(D3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));

	//????????????
	//INT Priority ???????ȼ?
	//D3D12_COMMAND_QUEUE_PRIORITY
	//D3D12_COMMAND_QUEUE_PRIORITY_NORMAL
	//D3D12_COMMAND_QUEUE_PRIORITY_HIGH
	//NodeMask ָʾ??????????Ӧ???ĸ?GPU?ڵ???ִ??
	D3D12_COMMAND_QUEUE_DESC QueueDesc = {};//?????????ṹ??
	QueueDesc.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;//????????Ϊֱ?????? GPUֱ?ӵ???????
	QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
	ANALYSIS_HRESULT(D3dDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&CommandQueue)));//????????

//	ID3D12CommandAllocator Allocator();//?ȴ????ڴ??????? ?????????????????б?????
	ANALYSIS_HRESULT(D3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(CommandAllocator.GetAddressOf())));

	//?????????б?
	ANALYSIS_HRESULT(D3dDevice->CreateCommandList(
		0, //Ĭ??һ??Gpu  
		D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,//????????Ϊֱ?????? GPUֱ?ӵ???????
		CommandAllocator.Get(),//??Commandlist??????Allocator
		NULL,//ID3D12PipelineState
		IID_PPV_ARGS(GraphicsCommandList.GetAddressOf())));

	ANALYSIS_HRESULT(GraphicsCommandList->Close());//?ر??б?

	//MSAA
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS QualityLevels;//MSAA????????
	QualityLevels.Format = BackBufferFormat;//??????ʽ
	QualityLevels.SampleCount = 4;//????????
	QualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS::D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	QualityLevels.NumQualityLevels = 0;

	//????MSAA֧??
	ANALYSIS_HRESULT(D3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&QualityLevels,
		sizeof(QualityLevels)));

	M4XQualityLevels = QualityLevels.NumQualityLevels;

	//???ý?????
	SwapChain.Reset();
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;//??????????

	SwapChainDesc.BufferDesc.Width = FEngineRenderConfig::GetRenderConfig()->ScreenWidth;//buffer??
	SwapChainDesc.BufferDesc.Height = FEngineRenderConfig::GetRenderConfig()->ScreenHeight;//buffer??
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = FEngineRenderConfig::GetRenderConfig()->RefreshRate;//֡??
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;//??ĸ
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;//??դɨ?跽ʽ
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainDesc.BufferCount = FEngineRenderConfig::GetRenderConfig()->SwapChainCount;//??????????
	//DXGI_USAGE_BACK_BUFFER ??̨??????
	//DXGI_USAGE_READ_ONLY  ֻ??
	//DXGI_USAGE_SHADER_INPUT ??Ϊshader??????
	//DXGI_USAGE_SHARED  ????
	//DXGI_USAGE_UNORDERED_ACCESS  ????????
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//ʹ?ñ???????Դ??Ϊ??????ȾĿ?ꡣ
	SwapChainDesc.OutputWindow = MainWindowsHandle;//ָ??windows????
	SwapChainDesc.Windowed = true;//?Դ??????? falseΪȫ??
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;//????????????????ʱ????????buffer DISCARDΪ????Buffer
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;//IDXGISwapChain::ResizeTarget
	SwapChainDesc.BufferDesc.Format = BackBufferFormat;//??????ʽ Ĭ?Ϲ???ΪR8G8B8A8

	//???ز???MSAA??????????
	SwapChainDesc.SampleDesc.Count = GetDXGISampleCount();//?????Ƿ?????4xMSAA???ò?????
	SwapChainDesc.SampleDesc.Quality = GetDXGISampleQuality();//?????????ȼ?
	ANALYSIS_HRESULT(DXGIFactory->CreateSwapChain(
		CommandQueue.Get(),
		&SwapChainDesc, SwapChain.GetAddressOf()));//?????????а󶨲???????????

	//??Դ??????
	//D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV	//CBV????????????ͼ SRV??ɫ????Դ??ͼ UAV??????????ͼ
	//D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER		//????????ͼ
	//D3D12_DESCRIPTOR_HEAP_TYPE_RTV			//??ȾĿ??????ͼ??Դ
	//D3D12_DESCRIPTOR_HEAP_TYPE_DSV			//????/ģ??????ͼ??Դ
	D3D12_DESCRIPTOR_HEAP_DESC RTVDescriptorHeapDesc;//RTV
	RTVDescriptorHeapDesc.NumDescriptors = FEngineRenderConfig::GetRenderConfig()->SwapChainCount;//???ݽ??????????趨??????????
	RTVDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	RTVDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	RTVDescriptorHeapDesc.NodeMask = 0;
	//????Դ??????ջ??????????
	ANALYSIS_HRESULT(D3dDevice->CreateDescriptorHeap(
		&RTVDescriptorHeapDesc,
		IID_PPV_ARGS(RTVHeap.GetAddressOf())));

	//DSV
	D3D12_DESCRIPTOR_HEAP_DESC DSVDescriptorHeapDesc;
	DSVDescriptorHeapDesc.NumDescriptors = 1;//????/ģ?建??ֻ??Ҫһ??
	DSVDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DSVDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DSVDescriptorHeapDesc.NodeMask = 0;
	//????Դ??????ջ??????????
	ANALYSIS_HRESULT(D3dDevice->CreateDescriptorHeap(
		&DSVDescriptorHeapDesc,
		IID_PPV_ARGS(DSVHeap.GetAddressOf())));

	return false;
}

void CDirectXRenderingEngine::PostInitDirect3D()
{
	//ͬ??
	WaitGPUCommandQueueComplete();

	ANALYSIS_HRESULT(GraphicsCommandList->Reset(CommandAllocator.Get(), NULL));

	for (int i = 0; i < FEngineRenderConfig::GetRenderConfig()->SwapChainCount; i++)
	{
		SwapChainBuffer[i].Reset();//??ʼ?????ý?????
	}
	DepthStencilBuffer.Reset();

	//???ݽ????????????ʹ?С????buff??size
	SwapChain->ResizeBuffers(
		FEngineRenderConfig::GetRenderConfig()->SwapChainCount,
		FEngineRenderConfig::GetRenderConfig()->ScreenWidth,
		FEngineRenderConfig::GetRenderConfig()->ScreenHeight,
		BackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	//?õ?ÿ??RTV?????Ĵ?С
	RTVDescriptorSize = D3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//?õ???ջhandle
	CD3DX12_CPU_DESCRIPTOR_HANDLE HeapHandle(RTVHeap->GetCPUDescriptorHandleForHeapStart());
	for (int i = 0; i < FEngineRenderConfig::GetRenderConfig()->SwapChainCount; i++)
	{
		SwapChain->GetBuffer(i, IID_PPV_ARGS(&SwapChainBuffer[i]));//???ý???????buffer
		D3dDevice->CreateRenderTargetView(SwapChainBuffer[i].Get(), nullptr, HeapHandle);//????RTV
		HeapHandle.Offset(1, RTVDescriptorSize);//??ջ????ƫ??
	}
	//??Դ????
	D3D12_RESOURCE_DESC ResourceDesc;
	ResourceDesc.Width = FEngineRenderConfig::GetRenderConfig()->ScreenWidth;
	ResourceDesc.Height = FEngineRenderConfig::GetRenderConfig()->ScreenHeight;
	ResourceDesc.Alignment = 0;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.DepthOrArraySize = 1;//3dָ??????depth 1d??2dָ??????ArraySize
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	ResourceDesc.SampleDesc.Count = bMSAA4XEnabled ? 4 : 1;
	ResourceDesc.SampleDesc.Quality = bMSAA4XEnabled ? (M4XQualityLevels - 1) : 0;
	ResourceDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	//????????
	D3D12_CLEAR_VALUE ClearValue;
	ClearValue.DepthStencil.Depth = 1.f;//????ʱ????DepthΪ 1
	ClearValue.DepthStencil.Stencil = 0;//????ʱ????stencilΪ 0
	ClearValue.Format = DepthStencilFormat;

	CD3DX12_HEAP_PROPERTIES Properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);//Ĭ?϶?

	//ΪDSV????ʵ??
	D3dDevice->CreateCommittedResource(
		&Properties,
		D3D12_HEAP_FLAG_NONE, &ResourceDesc,
		D3D12_RESOURCE_STATE_COMMON, &ClearValue,
		IID_PPV_ARGS(DepthStencilBuffer.GetAddressOf()));
	//DSV????
	D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	DSVDesc.Format = DepthStencilFormat;
	DSVDesc.Texture2D.MipSlice = 0;
	DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DSVDesc.Flags = D3D12_DSV_FLAG_NONE;
	//????DSV
	D3dDevice->CreateDepthStencilView(DepthStencilBuffer.Get(), &DSVDesc, DSVHeap->GetCPUDescriptorHandleForHeapStart());
	//ResourceBarrier ͬ??????
	CD3DX12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_DEPTH_WRITE);

	GraphicsCommandList->ResourceBarrier(1, &Barrier);

	GraphicsCommandList->Close();//?ر??????б?

	ID3D12CommandList* CommandList[] = { GraphicsCommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CommandList), CommandList);

	//??Щ?Ḳ??ԭ??windows????
	//?????ӿڳߴ?
	ViewprotInfo.TopLeftX = 0;
	ViewprotInfo.TopLeftY = 0;
	ViewprotInfo.Width = FEngineRenderConfig::GetRenderConfig()->ScreenWidth;
	ViewprotInfo.Height = FEngineRenderConfig::GetRenderConfig()->ScreenHeight;
	ViewprotInfo.MinDepth = 0.f;
	ViewprotInfo.MaxDepth = 1.f;

	//?ü?????
	ViewprotRect.left = 0;
	ViewprotRect.top = 0;
	ViewprotRect.right = FEngineRenderConfig::GetRenderConfig()->ScreenWidth;
	ViewprotRect.bottom = FEngineRenderConfig::GetRenderConfig()->ScreenHeight;

	WaitGPUCommandQueueComplete();
}

#endif