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
	//�������ǵ������б�
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
	//����һ��Buffer
	ComPtr<ID3D12Resource> Buffer;

	CD3DX12_RESOURCE_DESC BufferResourceDESC = CD3DX12_RESOURCE_DESC::Buffer(InDataSize);//����ͨ����Դ����
	CD3DX12_HEAP_PROPERTIES BufferProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);//����һ��Ĭ��Buffer��ջ
	//ΪBuffer����һ��Resource
	ANALYSIS_HRESULT(GetD3dDevice()->CreateCommittedResource(
		&BufferProperties,
		D3D12_HEAP_FLAG_NONE,
		&BufferResourceDESC,
		D3D12_RESOURCE_STATE_COMMON,
		NULL, IID_PPV_ARGS(Buffer.GetAddressOf())));
	//�Դ��ݹ�����Buffer�����ϴ�����
	CD3DX12_HEAP_PROPERTIES UpdateBufferProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	ANALYSIS_HRESULT(GetD3dDevice()->CreateCommittedResource(
		&UpdateBufferProperties,
		D3D12_HEAP_FLAG_NONE,
		&BufferResourceDESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL, IID_PPV_ARGS(OutTmpBuffer.GetAddressOf())));
	//��Դ����
	D3D12_SUBRESOURCE_DATA SubResourceData = {};
	SubResourceData.pData = InData;//��Դָ��
	SubResourceData.RowPitch = InDataSize;//��Դ�м�� �����С
	SubResourceData.SlicePitch = SubResourceData.RowPitch;//��Դ��ȼ�� �����С

	//�����ԴΪ����Ŀ��
	CD3DX12_RESOURCE_BARRIER CopyDestBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Buffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST);

	GetGraphicsCommandList()->ResourceBarrier(1, &CopyDestBarrier);

	//��������Դ��Ӧ�������������Դ����
	UpdateSubresources<1>(
		GetGraphicsCommandList().Get(),
		Buffer.Get(),
		OutTmpBuffer.Get(),
		0,//0 -> D3D12_REQ_SUBRESOURCES  ��Դ������
		0,//0 -> D3D12_REQ_SUBRESOURCES  ��Դ�е�����Դ����
		1,//��Դ�ĳߴ�size
		&SubResourceData);
	//��buffer��Copy����Ϊ����ɶ�
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
