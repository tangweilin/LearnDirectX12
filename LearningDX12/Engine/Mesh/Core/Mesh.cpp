#include "Mesh.h"
#include "../../Config/EngineRenderConfig.h"

FMesh::FMesh()
	:VertexSizeInBytes(0)
	, VertexStrideInBytes(0)
	, IndexSizeInBytes(0)
	, IndexFormat(DXGI_FORMAT_R16_UINT)
	, IndexSize(0)
	, WorldMatrix(FObjectTransformation::IdentityMatrix4x4())
	, ViewMatrix(FObjectTransformation::IdentityMatrix4x4())
	, ProjectMatrix(FObjectTransformation::IdentityMatrix4x4())
{
}
const float PI = 3.1415926535f;
void FMesh::Init()
{

}
void FMesh::BuildMesh(const FMeshRenderingData* InRenderingData)
{
	//构建常量缓冲区CBV堆栈
	//堆栈描述
	D3D12_DESCRIPTOR_HEAP_DESC HeapDesc;
	HeapDesc.NumDescriptors = 1;
	HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//类型为CBV
	HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//设置shader可见
	HeapDesc.NodeMask = 0;
	GetD3dDevice()->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&CBVHeap));//创建CBV堆栈



	//构建常量缓冲区
	objectConstants = make_shared<FRenderingResourcesUpdate>();
	objectConstants->Init(GetD3dDevice().Get(), sizeof(FObjectTransformation), 1);//初始化
	D3D12_GPU_VIRTUAL_ADDRESS ObAddr = objectConstants.get()->GetBuffer()->GetGPUVirtualAddress();//获得常量缓冲区的GPU地址
	//常量缓冲描述
	D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
	CBVDesc.BufferLocation = ObAddr;//buffer地址
	CBVDesc.SizeInBytes = objectConstants->GetConstantBufferByteSize();//buffer的Size
	//创建常量缓冲区
	GetD3dDevice()->CreateConstantBufferView(
		&CBVDesc,
		CBVHeap->GetCPUDescriptorHandleForHeapStart());



	//构建根签名
	CD3DX12_ROOT_PARAMETER RootParam[1];//根参数

	//CBV描述表
	CD3DX12_DESCRIPTOR_RANGE DescriptorRangeCBV;
	DescriptorRangeCBV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);//初始化CBV描述表

	RootParam[0].InitAsDescriptorTable(1, &DescriptorRangeCBV);//根参数的内容初始化
	//根描述
	CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(
		1,//几个参数
		RootParam,//根参数
		0,//几个静态采样
		nullptr,//静态采样实例
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> SerializeRootSignature;//序列化
	ComPtr<ID3DBlob> ErrorBlob;
	//序列化根签名
	D3D12SerializeRootSignature(
		&RootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		SerializeRootSignature.GetAddressOf(),//序列化地址
		ErrorBlob.GetAddressOf());//如果出错的话 出错信息存放的位置

	if (ErrorBlob)
	{
		Engine_Log_Error("%s", (char*)ErrorBlob->GetBufferPointer());
	}

	//创建根签名
	GetD3dDevice()->CreateRootSignature(
		0,//对单个GPU操作
		SerializeRootSignature->GetBufferPointer(),
		SerializeRootSignature->GetBufferSize(),
		IID_PPV_ARGS(&RootSignature));


	//构建Shader
	//HLSL
	VertexShader.BuildShaders(L"../LearningDX12/Shader/Hello.hlsl", "VertexShaderMain", "vs_5_0");
	PixelShader.BuildShaders(L"../LearningDX12/Shader/Hello.hlsl", "PixelShaderMain", "ps_5_0");

	//shader输入参数描述
	//HLSL语义,元素语义索引,HLSL语义的格式，插入槽，内存偏移，HLSL语义存在的位置阶段
	InputElementDesc =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};


	//开始构建模型
	VertexStrideInBytes = sizeof(FVertex);
	IndexSize = InRenderingData->IndexData.size();

	VertexSizeInBytes = InRenderingData->VertexData.size() * VertexStrideInBytes;//获取顶点数据大小
	IndexSizeInBytes = InRenderingData->IndexData.size() * sizeof(uint16_t);//获取顶点索引大小

	ANALYSIS_HRESULT(D3DCreateBlob(VertexSizeInBytes, &CPUVertexBufferPtr));//创建指定大小的内存的缓冲区
	memcpy(CPUVertexBufferPtr->GetBufferPointer(), InRenderingData->VertexData.data(), VertexSizeInBytes);//内存拷贝

	ANALYSIS_HRESULT(D3DCreateBlob(IndexSizeInBytes, &CPUIndexBufferPtr));
	memcpy(CPUIndexBufferPtr->GetBufferPointer(), InRenderingData->VertexData.data(), IndexSizeInBytes);

	//通过Temp上传缓冲区创建GPU顶点数据缓冲区 ConstructDefaultBuffer函数会在内部创建一个Buffer将Data拷贝进去后返回给VertexBufferTmpPtr
	GPUVertexBufferPtr = ConstructDefaultBuffer(
		VertexBufferTmpPtr,
		InRenderingData->VertexData.data(), VertexSizeInBytes);
	//通过Temp上传缓冲区创建GPU顶点索引缓冲区
	GPUIndexBufferPtr = ConstructDefaultBuffer(IndexBufferTmpPtr,
		InRenderingData->IndexData.data(), IndexSizeInBytes);
}

void FMesh::Draw(float DeltaTime)
{
	ID3D12DescriptorHeap* DescriptorHeap[] = { CBVHeap.Get() };
	GetGraphicsCommandList()->SetDescriptorHeaps(_countof(DescriptorHeap), DescriptorHeap);

	GetGraphicsCommandList()->SetGraphicsRootSignature(RootSignature.Get());

	D3D12_VERTEX_BUFFER_VIEW VBV = GetVertexBufferView();

	//绑定渲染流水线上的输入槽，可以在输入装配器阶段传入顶点数据
	GetGraphicsCommandList()->IASetVertexBuffers(
		0,//起始输入槽 0-15 
		1,//k k+1 ... k+n-1  n为输入槽顶点缓冲区数量  k为起始输入槽
		&VBV);

	D3D12_INDEX_BUFFER_VIEW IBV = GetIndexBufferView();
	GetGraphicsCommandList()->IASetIndexBuffer(&IBV);

	//定义我们要绘制的哪种图元 点 线 面
	GetGraphicsCommandList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(0, CBVHeap->GetGPUDescriptorHandleForHeapStart());

	//真正的绘制
	GetGraphicsCommandList()->DrawIndexedInstanced(
		IndexSize,//顶点数量
		1,//绘制实例数量
		0,//顶点缓冲区第一个被绘制的索引
		0,//GPU 从索引缓冲区读取的第一个索引的位置。
		0);//在从顶点缓冲区读取每个实例数据之前添加到每个索引的值。
}

FMesh* FMesh::CreateMesh(const FMeshRenderingData* InRenderingData)
{
	FMesh *InMesh = new FMesh();
	InMesh->BuildMesh(InRenderingData);
	return nullptr;
}
D3D12_VERTEX_BUFFER_VIEW FMesh::GetVertexBufferView()
{
	//创建Buffer View
	D3D12_VERTEX_BUFFER_VIEW VBV;
	//获得虚拟地址
	VBV.BufferLocation = GPUVertexBufferPtr->GetGPUVirtualAddress();
	//缓冲区大小
	VBV.SizeInBytes = VertexSizeInBytes;
	//缓冲区内每个顶点的大小
	VBV.StrideInBytes = VertexStrideInBytes;

	return VBV;
}

D3D12_INDEX_BUFFER_VIEW FMesh::GetIndexBufferView()
{
	D3D12_INDEX_BUFFER_VIEW IBV;
	IBV.BufferLocation = GPUIndexBufferPtr->GetGPUVirtualAddress();
	IBV.SizeInBytes = IndexSizeInBytes;
	IBV.Format = IndexFormat;

	return IBV;
}
FObjectTransformation::FObjectTransformation()
	:World(FObjectTransformation::IdentityMatrix4x4())
{
}

XMFLOAT4X4 FObjectTransformation::IdentityMatrix4x4()
{
	return XMFLOAT4X4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}