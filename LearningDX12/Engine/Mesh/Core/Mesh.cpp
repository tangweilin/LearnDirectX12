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
	//��������������CBV��ջ
	//��ջ����
	D3D12_DESCRIPTOR_HEAP_DESC HeapDesc;
	HeapDesc.NumDescriptors = 1;
	HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//����ΪCBV
	HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//����shader�ɼ�
	HeapDesc.NodeMask = 0;
	GetD3dDevice()->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&CBVHeap));//����CBV��ջ



	//��������������
	objectConstants = make_shared<FRenderingResourcesUpdate>();
	objectConstants->Init(GetD3dDevice().Get(), sizeof(FObjectTransformation), 1);//��ʼ��
	D3D12_GPU_VIRTUAL_ADDRESS ObAddr = objectConstants.get()->GetBuffer()->GetGPUVirtualAddress();//��ó�����������GPU��ַ
	//������������
	D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
	CBVDesc.BufferLocation = ObAddr;//buffer��ַ
	CBVDesc.SizeInBytes = objectConstants->GetConstantBufferByteSize();//buffer��Size
	//��������������
	GetD3dDevice()->CreateConstantBufferView(
		&CBVDesc,
		CBVHeap->GetCPUDescriptorHandleForHeapStart());



	//������ǩ��
	CD3DX12_ROOT_PARAMETER RootParam[1];//������

	//CBV������
	CD3DX12_DESCRIPTOR_RANGE DescriptorRangeCBV;
	DescriptorRangeCBV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);//��ʼ��CBV������

	RootParam[0].InitAsDescriptorTable(1, &DescriptorRangeCBV);//�����������ݳ�ʼ��
	//������
	CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(
		1,//��������
		RootParam,//������
		0,//������̬����
		nullptr,//��̬����ʵ��
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> SerializeRootSignature;//���л�
	ComPtr<ID3DBlob> ErrorBlob;
	//���л���ǩ��
	D3D12SerializeRootSignature(
		&RootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		SerializeRootSignature.GetAddressOf(),//���л���ַ
		ErrorBlob.GetAddressOf());//�������Ļ� ������Ϣ��ŵ�λ��

	if (ErrorBlob)
	{
		Engine_Log_Error("%s", (char*)ErrorBlob->GetBufferPointer());
	}

	//������ǩ��
	GetD3dDevice()->CreateRootSignature(
		0,//�Ե���GPU����
		SerializeRootSignature->GetBufferPointer(),
		SerializeRootSignature->GetBufferSize(),
		IID_PPV_ARGS(&RootSignature));


	//����Shader
	//HLSL
	VertexShader.BuildShaders(L"../LearningDX12/Shader/Hello.hlsl", "VertexShaderMain", "vs_5_0");
	PixelShader.BuildShaders(L"../LearningDX12/Shader/Hello.hlsl", "PixelShaderMain", "ps_5_0");

	//shader�����������
	//HLSL����,Ԫ����������,HLSL����ĸ�ʽ������ۣ��ڴ�ƫ�ƣ�HLSL������ڵ�λ�ý׶�
	InputElementDesc =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};


	//��ʼ����ģ��
	VertexStrideInBytes = sizeof(FVertex);
	IndexSize = InRenderingData->IndexData.size();

	VertexSizeInBytes = InRenderingData->VertexData.size() * VertexStrideInBytes;//��ȡ�������ݴ�С
	IndexSizeInBytes = InRenderingData->IndexData.size() * sizeof(uint16_t);//��ȡ����������С

	ANALYSIS_HRESULT(D3DCreateBlob(VertexSizeInBytes, &CPUVertexBufferPtr));//����ָ����С���ڴ�Ļ�����
	memcpy(CPUVertexBufferPtr->GetBufferPointer(), InRenderingData->VertexData.data(), VertexSizeInBytes);//�ڴ濽��

	ANALYSIS_HRESULT(D3DCreateBlob(IndexSizeInBytes, &CPUIndexBufferPtr));
	memcpy(CPUIndexBufferPtr->GetBufferPointer(), InRenderingData->VertexData.data(), IndexSizeInBytes);

	//ͨ��Temp�ϴ�����������GPU�������ݻ����� ConstructDefaultBuffer���������ڲ�����һ��Buffer��Data������ȥ�󷵻ظ�VertexBufferTmpPtr
	GPUVertexBufferPtr = ConstructDefaultBuffer(
		VertexBufferTmpPtr,
		InRenderingData->VertexData.data(), VertexSizeInBytes);
	//ͨ��Temp�ϴ�����������GPU��������������
	GPUIndexBufferPtr = ConstructDefaultBuffer(IndexBufferTmpPtr,
		InRenderingData->IndexData.data(), IndexSizeInBytes);
}

void FMesh::Draw(float DeltaTime)
{
	ID3D12DescriptorHeap* DescriptorHeap[] = { CBVHeap.Get() };
	GetGraphicsCommandList()->SetDescriptorHeaps(_countof(DescriptorHeap), DescriptorHeap);

	GetGraphicsCommandList()->SetGraphicsRootSignature(RootSignature.Get());

	D3D12_VERTEX_BUFFER_VIEW VBV = GetVertexBufferView();

	//����Ⱦ��ˮ���ϵ�����ۣ�����������װ�����׶δ��붥������
	GetGraphicsCommandList()->IASetVertexBuffers(
		0,//��ʼ����� 0-15 
		1,//k k+1 ... k+n-1  nΪ����۶��㻺��������  kΪ��ʼ�����
		&VBV);

	D3D12_INDEX_BUFFER_VIEW IBV = GetIndexBufferView();
	GetGraphicsCommandList()->IASetIndexBuffer(&IBV);

	//��������Ҫ���Ƶ�����ͼԪ �� �� ��
	GetGraphicsCommandList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(0, CBVHeap->GetGPUDescriptorHandleForHeapStart());

	//�����Ļ���
	GetGraphicsCommandList()->DrawIndexedInstanced(
		IndexSize,//��������
		1,//����ʵ������
		0,//���㻺������һ�������Ƶ�����
		0,//GPU ��������������ȡ�ĵ�һ��������λ�á�
		0);//�ڴӶ��㻺������ȡÿ��ʵ������֮ǰ��ӵ�ÿ��������ֵ��
}

FMesh* FMesh::CreateMesh(const FMeshRenderingData* InRenderingData)
{
	FMesh *InMesh = new FMesh();
	InMesh->BuildMesh(InRenderingData);
	return nullptr;
}
D3D12_VERTEX_BUFFER_VIEW FMesh::GetVertexBufferView()
{
	//����Buffer View
	D3D12_VERTEX_BUFFER_VIEW VBV;
	//��������ַ
	VBV.BufferLocation = GPUVertexBufferPtr->GetGPUVirtualAddress();
	//��������С
	VBV.SizeInBytes = VertexSizeInBytes;
	//��������ÿ������Ĵ�С
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