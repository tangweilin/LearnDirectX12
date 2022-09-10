#include "WindowsEngine.h"
#include "../../Debug/EngineDebug.h"
#include "../../Config/EngineRenderConfig.h"

#if defined(_WIN32)
#include "WindowsMessageProcessing.h"

FWindowsEngine::FWindowsEngine()
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
}

int FWindowsEngine::PreInit(FWinMainCommandParameters InParameters)
{
	//��ʼ����־ϵͳ
	const char LogPath[] = "../log";
	init_log_system(LogPath);
	Engine_Log("Log System Init.")

	//��������

	

	if (InitWindows(InParameters))
	{

	}

	
	Engine_Log("Engine Pre Initialization Complete.")
	return 0;
}

int FWindowsEngine::Init()
{
	Engine_Log("Engine Initialization Complete.")
	return 0;
}

int FWindowsEngine::PostInit()
{
	Engine_Log("Engine Post Initialization Complete.")
	return 0;
}

void FWindowsEngine::Tick()
{

}

int FWindowsEngine::PreExit()
{

	Engine_Log("Engine Pre Exit Complete.")
	return 0;
}

int FWindowsEngine::Exit()
{
	Engine_Log("Engine Exit Complete.")
	return 0;
}

int FWindowsEngine::PostExit()
{
	FEngineRenderConfig::Destroy();//�ͷ�Config
	Engine_Log("Engine Post Exit Complete.")
	return 0;
}
bool FWindowsEngine::InitWindows(FWinMainCommandParameters InParameters)
{
	//ע�ᴰ��
	WNDCLASSEX WindowsClass;
	WindowsClass.cbSize = sizeof(WNDCLASSEX); //�ö���ʵ��ռ�ö���ڴ�
	WindowsClass.cbClsExtra = 0; //�Ƿ���Ҫ����ռ�
	WindowsClass.cbWndExtra = 0; //�Ƿ���Ҫ�����ڴ�
	WindowsClass.hbrBackground = nullptr; //����������Ǿ���GDI����
	WindowsClass.hCursor = LoadCursor(NULL, IDC_ARROW); //����һ����ͷ���
	WindowsClass.hIcon = nullptr; //Ӧ�ó�����ڴ�������ʾ��ͼ��
	WindowsClass.hIconSm = NULL; //Ӧ�ó�����ʾ�����Ͻǵ�ͼ��
	WindowsClass.hInstance = InParameters.HInstance; //����ʵ��
	WindowsClass.lpszClassName = L"DirectX12Engine"; //��������
	WindowsClass.lpszMenuName = nullptr; //�˵�����
	WindowsClass.style = CS_VREDRAW | CS_HREDRAW; //��ô���ƴ���  ��ֱorˮƽ
	WindowsClass.lpfnWndProc = EngineWindowProc; //��Ϣ������

	ATOM RegisterAtom = RegisterClassEx(&WindowsClass); //ע�ᴰ��

	if (!RegisterAtom)
	{
		Engine_Log_Error("Register Windows Class Fail.")
		MessageBox(NULL, L"Register Windows Class Fail", L"Error", MB_OK);//����ʧ����ʾ��Ϣ
	}

	RECT Rect = { 0,0, FEngineRenderConfig::GetRenderConfig()->ScreenWidth, FEngineRenderConfig::GetRenderConfig()->ScreenHeight };//left top right bottom

	AdjustWindowRect(&Rect,WS_OVERLAPPEDWINDOW,NULL);//���ڴ�С ����Style  �˵�ΪNULL

	int WindowWidth = Rect.right - Rect.left;
	int WindowHeight = Rect.bottom - Rect.top;

	MainWindowsHandle= CreateWindowEx(
	NULL,//���ڶ�����
	L"DirectX12Engine",//��������
	L"DIREXTX12 ENGINE",//��ʾ�ڱ�����������
	WS_OVERLAPPEDWINDOW,//���ڷ��
		100,100,//��������
		WindowWidth,WindowHeight,//���ڵĿ��
		NULL,//�����ھ��
		nullptr,//�˵����
		InParameters.HInstance,//����ʵ��
		NULL//������� Ĭ��ΪNULL
	);
	if (!MainWindowsHandle)
	{
		Engine_Log_Error("CreateWindow Failed.")
		MessageBox(0, L"CreateWindow Failed .", L"Error", 0);//���ڴ���ʧ����ʾ
		return false;
	}
	
	//��ʾ����
	ShowWindow(MainWindowsHandle, SW_SHOW);
	//ˢ�´���
	UpdateWindow(MainWindowsHandle);

	Engine_Log("InitWindows Complete.")
}
bool FWindowsEngine::InitDirect3D()
{
	//HRESULT
	//S_OK				0x00000000
	//E_UNEXPECTED		0x8000FFFF �����ʧ��
	//E_NOTIMPL			0x80004001 δʵ��
	//E_OUTOFMEMORY		0x8007000E δ�ܷ���������ڴ�
	//E_INVALIDARG		0x80070057 һ������������Ч 
	//E_NOINTERFACE		0x80004002 ��֧�ִ˽ӿ�
	//E_POINTER			0x80004003 ��Чָ��
	//E_HANDLE			0x80070006 ��Ч���
	//E_ABORT			0x80004004 ������ֹ
	//E_FAIL			0x80004005 ����
	//E_ACCESSDENIED	0x80070005 һ��ķ��ʱ��ܾ�����
	ANALYSIS_HRESULT(CreateDXGIFactory1(IID_PPV_ARGS(&DXGIFactory)));

	//ʹ��Ĭ�ϵ�������  ���FEATURE_LevelΪ11.0  
	HRESULT D3dDeviceResult = D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3dDevice));

	if (FAILED(D3dDeviceResult))
	{
		//warp ���ͨ��Ӳ������������ʧ�� �������������
		ComPtr<IDXGIAdapter> WARPAdapter;//����������
		ANALYSIS_HRESULT(DXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&WARPAdapter)));//ö��������
		ANALYSIS_HRESULT(D3D12CreateDevice(WARPAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3dDevice)));//����Device
	}
	//����Χ��
	//D3D12_FENCE_FLAG_NONE 
	//D3D11_FENCE_FLAG_SHARED
	//D3D11_FENCE_FLAG_SHARED_CROSS_ADAPTER
	/*
	Fence->SetEventOnCompletion
	ִ������
	�ύ����
	Queue->Signal
	wait
	*/
	ANALYSIS_HRESULT(D3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));

	//�����������
	//INT Priority �������ȼ�
	//D3D12_COMMAND_QUEUE_PRIORITY
	//D3D12_COMMAND_QUEUE_PRIORITY_NORMAL
	//D3D12_COMMAND_QUEUE_PRIORITY_HIGH
	//NodeMask ָʾ���������Ӧ���ĸ�GPU�ڵ���ִ��
	D3D12_COMMAND_QUEUE_DESC QueueDesc = {};//���������ṹ��
	QueueDesc.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;//��������Ϊֱ������ GPUֱ�ӵ�������
	QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
	ANALYSIS_HRESULT(D3dDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&CommandQueue)));//��������

	ID3D12CommandAllocator Allocator();//�ȴ����ڴ������ ����������������б����
	ANALYSIS_HRESULT(D3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(CommandAllocator.GetAddressOf())));

	//���������б�
	ANALYSIS_HRESULT(D3dDevice->CreateCommandList(
		0, //Ĭ��һ��Gpu 
		D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,//��������Ϊֱ������ GPUֱ�ӵ�������
		CommandAllocator.Get(),//��Commandlist������Allocator 
		NULL,//ID3D12PipelineState
		IID_PPV_ARGS(GraphicsCommandList.GetAddressOf())));

	GraphicsCommandList->Close();//�ر��б�

	//���ý�����
	SwapChain.Reset();
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;//����������

	SwapChainDesc.BufferDesc.Width = FEngineRenderConfig::GetRenderConfig()->ScreenWidth;//buffer��
	SwapChainDesc.BufferDesc.Height = FEngineRenderConfig::GetRenderConfig()->ScreenHeight;//buffer��
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = FEngineRenderConfig::GetRenderConfig()->RefreshRate;//֡��
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;//��ĸ
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;//��դɨ�跽ʽ
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;//
	SwapChainDesc.BufferCount = FEngineRenderConfig::GetRenderConfig()->SwapChainCount;//����������
	//DXGI_USAGE_BACK_BUFFER ��̨������
	//DXGI_USAGE_READ_ONLY  ֻ��
	//DXGI_USAGE_SHADER_INPUT ��Ϊshader������
	//DXGI_USAGE_SHARED  ����
	//DXGI_USAGE_UNORDERED_ACCESS  �������
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//ʹ�ñ������Դ��Ϊ�����ȾĿ�ꡣ
	SwapChainDesc.OutputWindow = MainWindowsHandle;//ָ��windows���
	SwapChainDesc.Windowed = true;//�Դ������� falseΪȫ��
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;//����������������ʱ��������buffer DISCARDΪ����Buffer
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;//IDXGISwapChain::ResizeTarget
	SwapChainDesc.BufferDesc.Format = BackBufferFormat;//�����ʽ

	//MSAA
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS QualityLevels;//MSAA��������
	QualityLevels.Format = BackBufferFormat;
	QualityLevels.SampleCount = 4;//��������
	QualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS::D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	QualityLevels.NumQualityLevels = 0;

	//���MSAA֧��
	ANALYSIS_HRESULT(D3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&QualityLevels,
		sizeof(QualityLevels)));
	//�õ�MSAA��������
	M4XQualityLevels = QualityLevels.NumQualityLevels;

	//���ز���MSAA���������
	SwapChainDesc.SampleDesc.Count = bMSAA4XEnabled ? 4 : 1;//�����Ƿ���4xMSAA���ò�����
	SwapChainDesc.SampleDesc.Quality = bMSAA4XEnabled ? (M4XQualityLevels - 1) : 0;//���������ȼ�

	ANALYSIS_HRESULT(DXGIFactory->CreateSwapChain(
		CommandQueue.Get(),
		&SwapChainDesc, SwapChain.GetAddressOf()));//��������а󶨲�����������
	return false;
}
#endif