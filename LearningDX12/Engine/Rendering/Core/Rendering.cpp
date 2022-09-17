#include "Rendering.h"
#include "../../Debug/EngineDebug.h"
#include "../../Platform/Windows/WindowsEngine.h"
#include "../../Rendering/Enigne/DirectX/Core/DirectXRenderingEngine.h"

IRenderingInterface::IRenderingInterface()
{

}

IRenderingInterface::~IRenderingInterface()
{

}

void IRenderingInterface::Init()
{
}

void IRenderingInterface::PreDraw(float DeltaTime)
{
	//重置我们的命令列表
	ANALYSIS_HRESULT(GetGraphicsCommandList()->Reset(GetCommandAllocator().Get(), NULL));
}

void IRenderingInterface::Draw(float DeltaTime)
{

}

void IRenderingInterface::PostDraw(float DeltaTime)
{
}

ComPtr<ID3D12Resource> IRenderingInterface::ConstructDefaultBuffer(
	ComPtr<ID3D12Resource>& OutTmpBuffer,
	const void* InData, UINT64 InDataSize)
{
	//声明一个Buffer
	ComPtr<ID3D12Resource> Buffer;

	CD3DX12_RESOURCE_DESC BufferResourceDESC = CD3DX12_RESOURCE_DESC::Buffer(InDataSize);//创建通用资源描述
	CD3DX12_HEAP_PROPERTIES BufferProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);//创建一个默认Buffer堆栈
	//为Buffer创建一个Resource
	ANALYSIS_HRESULT(GetD3dDevice()->CreateCommittedResource(
		&BufferProperties,
		D3D12_HEAP_FLAG_NONE,
		&BufferResourceDESC,
		D3D12_RESOURCE_STATE_COMMON,
		NULL, IID_PPV_ARGS(Buffer.GetAddressOf())));
	//对传递过来的Buffer创建上传缓冲
	CD3DX12_HEAP_PROPERTIES UpdateBufferProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	ANALYSIS_HRESULT(GetD3dDevice()->CreateCommittedResource(
		&UpdateBufferProperties,
		D3D12_HEAP_FLAG_NONE,
		&BufferResourceDESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL, IID_PPV_ARGS(OutTmpBuffer.GetAddressOf())));
	//资源描述
	D3D12_SUBRESOURCE_DATA SubResourceData = {};
	SubResourceData.pData = InData;//资源指针
	SubResourceData.RowPitch = InDataSize;//资源行间距 物理大小
	SubResourceData.SlicePitch = SubResourceData.RowPitch;//资源深度间距 物理大小

	//标记资源为复制目标
	CD3DX12_RESOURCE_BARRIER CopyDestBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Buffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST);

	GetGraphicsCommandList()->ResourceBarrier(1, &CopyDestBarrier);

	//更新子资源，应该填充所有子资源数组
	UpdateSubresources<1>(
		GetGraphicsCommandList().Get(),
		Buffer.Get(),
		OutTmpBuffer.Get(),
		0,//0 -> D3D12_REQ_SUBRESOURCES  资源的索引
		0,//0 -> D3D12_REQ_SUBRESOURCES  资源中的子资源数量
		1,//资源的尺寸size
		&SubResourceData);
	//把buffer从Copy设置为常规可读
	CD3DX12_RESOURCE_BARRIER ReadDestBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Buffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_GENERIC_READ);

	return Buffer;
}

ComPtr<ID3D12Device> IRenderingInterface::GetD3dDevice()
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		if (InEngine->GetRenderingEngine())
		{
			return InEngine->GetRenderingEngine()->D3dDevice;
		}
	}

	return NULL;
}

ComPtr<ID3D12GraphicsCommandList> IRenderingInterface::GetGraphicsCommandList()
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		if (InEngine->GetRenderingEngine())
		{
			return InEngine->GetRenderingEngine()->GraphicsCommandList;
		}
	}

	return NULL;
}

ComPtr<ID3D12CommandAllocator> IRenderingInterface::GetCommandAllocator()
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		if (InEngine->GetRenderingEngine())
		{
			return InEngine->GetRenderingEngine()->CommandAllocator;
		}
	}

	return NULL;
}

#if defined(_WIN32)
CWindowsEngine* IRenderingInterface::GetEngine()
{
	return dynamic_cast<CWindowsEngine*>(Engine);
}
#else
CEngine* IRenderingInterface::GetEngine()
{
	return Engine;
}
#endif
