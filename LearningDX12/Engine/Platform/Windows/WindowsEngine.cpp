#include "WindowsEngine.h"
#include "../../Debug/EngineDebug.h"
#include "../../Config/EngineRenderConfig.h"
#include "../../Rendering/Core/Rendering.h"
#include "../../Mesh/BoxMesh.h"
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
        SwapChainBuffer.push_back(ComPtr<ID3D12Resource>());//交换链buffer初始化
    }
}

int FWindowsEngine::PreInit(FWinMainCommandParameters InParameters)
{
    //初始化日志系统
    const char LogPath[] = "../log";
    init_log_system(LogPath);
    Engine_Log("Log System Init.")

    //处理命令

    
    Engine_Log("Engine Pre Initialization Complete.")
    return 0;
}

int FWindowsEngine::Init(FWinMainCommandParameters InParameters)
{

    InitWindows(InParameters);

    InitDirect3D();

    PostInitDirect3D();

    Engine_Log("Engine Initialization Complete.")
    return 0;
}

int FWindowsEngine::PostInit()
{
   
    Engine_Log("Engine Post Initialization Complete.")


    ANALYSIS_HRESULT(GraphicsCommandList->Reset(CommandAllocator.Get(), NULL));
    {
        //构建Mesh
        FBoxMesh* Box = FBoxMesh::CreateMesh();

    }

    ANALYSIS_HRESULT(GraphicsCommandList->Close());

    ID3D12CommandList* CommandList[] = { GraphicsCommandList.Get() };
    CommandQueue->ExecuteCommandLists(_countof(CommandList), CommandList);

    WaitGPUCommandQueueComplete();

    return 0;
}

void FWindowsEngine::Tick(float DeltaTime)
{
    //重置命令分配器内存，为下一帧做准备
    ANALYSIS_HRESULT(CommandAllocator->Reset());

    //重置命令列表
    ANALYSIS_HRESULT(GraphicsCommandList->Reset(CommandAllocator.Get(), NULL));

    //判定并等待完成渲染目标的资源是否完成了从Present（提交）状态切换到Render Target（渲染目标）状态了
    CD3DX12_RESOURCE_BARRIER ResourceBarrierPresent = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentSwapBuff(),
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    GraphicsCommandList->ResourceBarrier(1, &ResourceBarrierPresent);

    //需要每帧执行
    //绑定矩形框
    GraphicsCommandList->RSSetViewports(1, &ViewprotInfo);
    GraphicsCommandList->RSSetScissorRects(1, &ViewprotRect);

    //清除RTV 画布
    GraphicsCommandList->ClearRenderTargetView(GetCurrentSwapBufferView(),
        DirectX::Colors::Black,
        0, nullptr);

    //清除深度模板缓冲区
    GraphicsCommandList->ClearDepthStencilView(GetCurrentDepthStencilView(),
        D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
        1.f, 0, 0, NULL);

    //将RTV与DSV绑定至输出的合并阶段 
    D3D12_CPU_DESCRIPTOR_HANDLE SwapBufferView = GetCurrentSwapBufferView();
    D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView = GetCurrentDepthStencilView();
    GraphicsCommandList->OMSetRenderTargets(1, &SwapBufferView,
        true, &DepthStencilView);
    
    //接口渲染
    for (auto& Tmp : IRenderingInterface::RenderingInterface)
    {
        Tmp->Draw(DeltaTime);
    }


    //判定并等待完成渲染目标的资源是否完成了从Render Target（渲染目标）状态 切换到 Present（提交）状态了
    CD3DX12_RESOURCE_BARRIER ResourceBarrierPresentRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentSwapBuff(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    GraphicsCommandList->ResourceBarrier(1, &ResourceBarrierPresentRenderTarget);

    //命令录入完成
    ANALYSIS_HRESULT(GraphicsCommandList->Close());

    //提交并执行命令
    ID3D12CommandList* CommandList[] = { GraphicsCommandList.Get() };
    CommandQueue->ExecuteCommandLists(_countof(CommandList), CommandList);

    //交换两个buff缓冲区
    ANALYSIS_HRESULT(SwapChain->Present(0, 0));
    
    CurrentSwapBuffIndex = !(bool)CurrentSwapBuffIndex;

    //CPU与GPU同步
    WaitGPUCommandQueueComplete();
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
    FEngineRenderConfig::Destroy();//释放Config
    Engine_Log("Engine Post Exit Complete.")
    return 0;
}
//获得当前的交换链buffer
ID3D12Resource* FWindowsEngine::GetCurrentSwapBuff() const
{
    return SwapChainBuffer[CurrentSwapBuffIndex].Get();
}

//获得当前的RTV
D3D12_CPU_DESCRIPTOR_HANDLE FWindowsEngine::GetCurrentSwapBufferView() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        RTVHeap->GetCPUDescriptorHandleForHeapStart(),
        CurrentSwapBuffIndex, RTVDescriptorSize);
}
//获得当前的DSV
D3D12_CPU_DESCRIPTOR_HANDLE FWindowsEngine::GetCurrentDepthStencilView() const
{
    return DSVHeap->GetCPUDescriptorHandleForHeapStart();
}

void FWindowsEngine::WaitGPUCommandQueueComplete()
{
    CurrentFenceIndex++;

    //向GUP设置新的隔离点 等待GPU处理完信号
    ANALYSIS_HRESULT(CommandQueue->Signal(Fence.Get(), CurrentFenceIndex));

    if (Fence->GetCompletedValue() < CurrentFenceIndex)
    {
        //创建或打开一个事件内核对象,并返回该内核对象的句柄.
        //SECURITY_ATTRIBUTES
        //CREATE_EVENT_INITIAL_SET  0x00000002
        //CREATE_EVENT_MANUAL_RESET 0x00000001
        //ResetEvents
        HANDLE EventEX = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);

        //GPU完成后会通知我们的Handle
        ANALYSIS_HRESULT(Fence->SetEventOnCompletion(CurrentFenceIndex, EventEX));

        //等待GPU,阻塞主线程
        WaitForSingleObject(EventEX, INFINITE);
        CloseHandle(EventEX);
    }
}
bool FWindowsEngine::InitWindows(FWinMainCommandParameters InParameters)
{
    //注册窗口
    WNDCLASSEX WindowsClass;
    WindowsClass.cbSize = sizeof(WNDCLASSEX); //该对象实际占用多大内存
    WindowsClass.cbClsExtra = 0; //是否需要额外空间
    WindowsClass.cbWndExtra = 0; //是否需要额外内存
    WindowsClass.hbrBackground = nullptr; //如果有设置那就是GDI擦除
    WindowsClass.hCursor = LoadCursor(NULL, IDC_ARROW); //设置一个箭头光标
    WindowsClass.hIcon = nullptr; //应用程序放在磁盘上显示的图标
    WindowsClass.hIconSm = NULL; //应用程序显示在左上角的图标
    WindowsClass.hInstance = InParameters.HInstance; //窗口实例
    WindowsClass.lpszClassName = L"DirectX12Engine"; //窗口名称
    WindowsClass.lpszMenuName = nullptr; //菜单名称
    WindowsClass.style = CS_VREDRAW | CS_HREDRAW; //怎么绘制窗口  垂直or水平
    WindowsClass.lpfnWndProc = EngineWindowProc; //消息处理函数

    ATOM RegisterAtom = RegisterClassEx(&WindowsClass); //注册窗口

    if (!RegisterAtom)
    {
        Engine_Log_Error("Register Windows Class Fail.")
        MessageBox(NULL, L"Register Windows Class Fail", L"Error", MB_OK);//创建失败提示消息
    }

    RECT Rect = { 0,0, FEngineRenderConfig::GetRenderConfig()->ScreenWidth, FEngineRenderConfig::GetRenderConfig()->ScreenHeight };//left top right bottom

    AdjustWindowRect(&Rect,WS_OVERLAPPEDWINDOW,NULL);//窗口大小 窗口Style  菜单为NULL

    int WindowWidth = Rect.right - Rect.left;
    int WindowHeight = Rect.bottom - Rect.top;

    MainWindowsHandle= CreateWindowEx(
    NULL,//窗口额外风格
    L"DirectX12Engine",//窗口名称
    L"DIREXTX12 ENGINE",//显示在标题栏的名称
    WS_OVERLAPPEDWINDOW,//窗口风格
        100,100,//窗口坐标
        WindowWidth,WindowHeight,//窗口的宽高
        NULL,//副窗口句柄
        nullptr,//菜单句柄
        InParameters.HInstance,//窗口实例
        NULL//额外参数 默认为NULL
    );
    if (!MainWindowsHandle)
    {
        Engine_Log_Error("CreateWindow Failed.")
        MessageBox(0, L"CreateWindow Failed .", L"Error", 0);//窗口创建失败提示
        return false;
    }
    
    //显示窗口
    ShowWindow(MainWindowsHandle, SW_SHOW);
    //刷新窗口
    UpdateWindow(MainWindowsHandle);

    Engine_Log("InitWindows Complete.")
}
bool FWindowsEngine::InitDirect3D()
{
    //D3D12Debug
    ComPtr<ID3D12Debug> D3D12Debug;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&D3D12Debug))))
    {
        D3D12Debug->EnableDebugLayer();//开启D3D的Debug 如果出现问题后会在OutPut上出现Log信息
    }

    //HRESULT
    //S_OK				0x00000000
    //E_UNEXPECTED		0x8000FFFF 意外的失败
    //E_NOTIMPL			0x80004001 未实现
    //E_OUTOFMEMORY		0x8007000E 未能分配所需的内存
    //E_INVALIDARG		0x80070057 一个或多个参数无效 
    //E_NOINTERFACE		0x80004002 不支持此接口
    //E_POINTER			0x80004003 无效指针
    //E_HANDLE			0x80070006 无效句柄
    //E_ABORT			0x80004004 操作终止
    //E_FAIL			0x80004005 错误
    //E_ACCESSDENIED	0x80070005 一般的访问被拒绝错误
    ANALYSIS_HRESULT(CreateDXGIFactory1(IID_PPV_ARGS(&DXGIFactory)));

    //使用默认的适配器  最低FEATURE_Level为11.0  
    HRESULT D3dDeviceResult = D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3dDevice));

    if (FAILED(D3dDeviceResult))
    {
        //warp 如果通过硬件适配器创建失败 创建软件适配器
        ComPtr<IDXGIAdapter> WARPAdapter;//创建适配器
        ANALYSIS_HRESULT(DXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&WARPAdapter)));//枚举适配器
        ANALYSIS_HRESULT(D3D12CreateDevice(WARPAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3dDevice)));//创建Device
    }
    //创建围栏
    //D3D12_FENCE_FLAG_NONE 
    //D3D11_FENCE_FLAG_SHARED
    //D3D11_FENCE_FLAG_SHARED_CROSS_ADAPTER
    /*
    Fence->SetEventOnCompletion
    执行命令
    提交呈现
    Queue->Signal
    wait
    */
    ANALYSIS_HRESULT(D3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));

    //创建命令队列
    //INT Priority 队列优先级
    //D3D12_COMMAND_QUEUE_PRIORITY
    //D3D12_COMMAND_QUEUE_PRIORITY_NORMAL
    //D3D12_COMMAND_QUEUE_PRIORITY_HIGH
    //NodeMask 指示该命令队列应在哪个GPU节点上执行
    D3D12_COMMAND_QUEUE_DESC QueueDesc = {};//队列描述结构体
    QueueDesc.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;//命令类型为直接类型 GPU直接调用命令
    QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
    ANALYSIS_HRESULT(D3dDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&CommandQueue)));//创建队列

    ID3D12CommandAllocator Allocator();//先创建内存分配器 避免后续创建命令列表崩溃
    ANALYSIS_HRESULT(D3dDevice->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(CommandAllocator.GetAddressOf())));

    //创建命令列表
    ANALYSIS_HRESULT(D3dDevice->CreateCommandList(
        0, //默认一个Gpu 
        D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,//命令类型为直接类型 GPU直接调用命令
        CommandAllocator.Get(),//将Commandlist关联到Allocator 
        NULL,//ID3D12PipelineState
        IID_PPV_ARGS(GraphicsCommandList.GetAddressOf())));


    ANALYSIS_HRESULT(GraphicsCommandList->Close());//关闭列表


    //重置交换链
    SwapChain.Reset();
    DXGI_SWAP_CHAIN_DESC SwapChainDesc;//交换链描述

    SwapChainDesc.BufferDesc.Width = FEngineRenderConfig::GetRenderConfig()->ScreenWidth;//buffer宽
    SwapChainDesc.BufferDesc.Height = FEngineRenderConfig::GetRenderConfig()->ScreenHeight;//buffer高
    SwapChainDesc.BufferDesc.RefreshRate.Numerator = FEngineRenderConfig::GetRenderConfig()->RefreshRate;//帧率
    SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;//分母
    SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;//光栅扫描方式
    SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;//
    SwapChainDesc.BufferCount = FEngineRenderConfig::GetRenderConfig()->SwapChainCount;//交换链数量
    //DXGI_USAGE_BACK_BUFFER 后台缓冲区
    //DXGI_USAGE_READ_ONLY  只读
    //DXGI_USAGE_SHADER_INPUT 作为shader的输入
    //DXGI_USAGE_SHARED  共享
    //DXGI_USAGE_UNORDERED_ACCESS  无序访问
    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//使用表面或资源作为输出渲染目标。
    SwapChainDesc.OutputWindow = MainWindowsHandle;//指定windows句柄
    SwapChainDesc.Windowed = true;//以窗口运行 false为全屏
    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;//当交换链发生交换时怎样处置buffer DISCARD为丢弃Buffer
    SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;//IDXGISwapChain::ResizeTarget
    SwapChainDesc.BufferDesc.Format = BackBufferFormat;//纹理格式 默认构造为R8G8B8A8

    //MSAA
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS QualityLevels;//MSAA质量级别
    QualityLevels.Format = BackBufferFormat;//纹理格式
    QualityLevels.SampleCount = 4;//采样次数
    QualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS::D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    QualityLevels.NumQualityLevels = 0;

    //检查MSAA支持
    ANALYSIS_HRESULT(D3dDevice->CheckFeatureSupport(
        D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
        &QualityLevels,
        sizeof(QualityLevels)));
    //拿到MSAA质量级别
    M4XQualityLevels = QualityLevels.NumQualityLevels;

    //多重采样MSAA抗锯齿设置
    SwapChainDesc.SampleDesc.Count = bMSAA4XEnabled ? 4 : 1;//根据是否开启4xMSAA设置采样数
    SwapChainDesc.SampleDesc.Quality = bMSAA4XEnabled ? (M4XQualityLevels - 1) : 0;//设置质量等级

    ANALYSIS_HRESULT( DXGIFactory->CreateSwapChain(
        CommandQueue.Get(),
        &SwapChainDesc, SwapChain.GetAddressOf()));//将命令队列绑定并创建交换链

    //资源描述符
    //D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV	//CBV常量缓冲区视图 SRV着色器资源视图 UAV无序访问视图
    //D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER		//采样器视图
    //D3D12_DESCRIPTOR_HEAP_TYPE_RTV			//渲染目标的视图资源
    //D3D12_DESCRIPTOR_HEAP_TYPE_DSV			//深度/模板的视图资源
    D3D12_DESCRIPTOR_HEAP_DESC RTVDescriptorHeapDesc;	//RTV
    RTVDescriptorHeapDesc.NumDescriptors = FEngineRenderConfig::GetRenderConfig()->SwapChainCount;//根据交换链数量设定描述符数量
    RTVDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    RTVDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    RTVDescriptorHeapDesc.NodeMask = 0;
    //从资源描述堆栈创建描述符
    ANALYSIS_HRESULT(D3dDevice->CreateDescriptorHeap(
        &RTVDescriptorHeapDesc,
        IID_PPV_ARGS(RTVHeap.GetAddressOf())));

    //DSV
    D3D12_DESCRIPTOR_HEAP_DESC DSVDescriptorHeapDesc;
    DSVDescriptorHeapDesc.NumDescriptors = 1; //深度/模板缓冲只需要一个
    DSVDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    DSVDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    DSVDescriptorHeapDesc.NodeMask = 0;
    //从资源描述堆栈创建描述符
    ANALYSIS_HRESULT(D3dDevice->CreateDescriptorHeap(
        &DSVDescriptorHeapDesc,
        IID_PPV_ARGS(DSVHeap.GetAddressOf())));

    return false;
}
void FWindowsEngine::PostInitDirect3D()
{
    //同步
    WaitGPUCommandQueueComplete();

    ANALYSIS_HRESULT(GraphicsCommandList->Reset(CommandAllocator.Get(), NULL));
    for (int i = 0; i < FEngineRenderConfig::GetRenderConfig()->SwapChainCount; i++)
    {
        SwapChainBuffer[i].Reset();//初始化重置交换链
    }
    DepthStencilBuffer.Reset();

    //根据交换链的数量和大小重置buff的size
    SwapChain->ResizeBuffers(
        FEngineRenderConfig::GetRenderConfig()->SwapChainCount,
        FEngineRenderConfig::GetRenderConfig()->ScreenWidth,
        FEngineRenderConfig::GetRenderConfig()->ScreenHeight,
        BackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

    //拿到每个RTV描述的大小
    RTVDescriptorSize = D3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    //拿到堆栈handle
    CD3DX12_CPU_DESCRIPTOR_HANDLE HeapHandle(RTVHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < FEngineRenderConfig::GetRenderConfig()->SwapChainCount; i++)
    {
        SwapChain->GetBuffer(i, IID_PPV_ARGS(&SwapChainBuffer[i]));//获得交换链的buffer
        D3dDevice->CreateRenderTargetView(SwapChainBuffer[i].Get(), nullptr, HeapHandle);//创建RTV
        HeapHandle.Offset(1, RTVDescriptorSize);//堆栈句柄偏移
    }
    //资源描述
    D3D12_RESOURCE_DESC ResourceDesc;
    ResourceDesc.Width = FEngineRenderConfig::GetRenderConfig()->ScreenWidth;
    ResourceDesc.Height = FEngineRenderConfig::GetRenderConfig()->ScreenHeight;
    ResourceDesc.Alignment = 0;
    ResourceDesc.MipLevels = 1;
    ResourceDesc.DepthOrArraySize = 1; //3d指定的是depth 1d或2d指定的是ArraySize
    ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    ResourceDesc.SampleDesc.Count = bMSAA4XEnabled ? 4 : 1;
    ResourceDesc.SampleDesc.Quality = bMSAA4XEnabled ? (M4XQualityLevels - 1) : 0;
    ResourceDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//depth stencil标记
    ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    //清除描述
    D3D12_CLEAR_VALUE ClearValue;
    ClearValue.DepthStencil.Depth = 1.f;//清除时设置Depth为 1
    ClearValue.DepthStencil.Stencil = 0;//清除时设置stencil为 0
    ClearValue.Format = DepthStencilFormat;

    CD3DX12_HEAP_PROPERTIES Properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);//默认堆
    //为DSV分配实例

    D3dDevice->CreateCommittedResource(
        &Properties,
        D3D12_HEAP_FLAG_NONE, &ResourceDesc,
        D3D12_RESOURCE_STATE_COMMON, &ClearValue,
        IID_PPV_ARGS(DepthStencilBuffer.GetAddressOf()));
    //DSV描述
    D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc;
    DSVDesc.Format = DepthStencilFormat;
    DSVDesc.Texture2D.MipSlice = 0;
    DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    DSVDesc.Flags = D3D12_DSV_FLAG_NONE;
    //创建DSV
    D3dDevice->CreateDepthStencilView(DepthStencilBuffer.Get(), &DSVDesc, DSVHeap->GetCPUDescriptorHandleForHeapStart());

    //ResourceBarrier 同步调用
    CD3DX12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilBuffer.Get(),
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_DEPTH_WRITE);

    GraphicsCommandList->ResourceBarrier(1, &Barrier);

    GraphicsCommandList->Close();//关闭命令列表

    ID3D12CommandList* CommandList[] = { GraphicsCommandList.Get() };
    CommandQueue->ExecuteCommandLists(_countof(CommandList), CommandList);//执行命令

    //这些会覆盖原先windows画布
    //描述视口尺寸
    ViewprotInfo.TopLeftX = 0;
    ViewprotInfo.TopLeftY = 0;
    ViewprotInfo.Width = FEngineRenderConfig::GetRenderConfig()->ScreenWidth;
    ViewprotInfo.Height = FEngineRenderConfig::GetRenderConfig()->ScreenHeight;
    ViewprotInfo.MinDepth = 0.f;
    ViewprotInfo.MaxDepth = 1.f;

    //裁剪矩形
    ViewprotRect.left = 0;
    ViewprotRect.top = 0;
    ViewprotRect.right = FEngineRenderConfig::GetRenderConfig()->ScreenWidth;
    ViewprotRect.bottom = FEngineRenderConfig::GetRenderConfig()->ScreenHeight;

    WaitGPUCommandQueueComplete();
}
#endif