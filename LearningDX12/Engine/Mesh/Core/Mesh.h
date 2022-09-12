#pragma once
#include "../../Rendering/Core/Rendering.h"
#include "MeshType.h"
#include "../../Shader/Core/Shader.h"

struct FObjectTransformation //ģ��λ�������ṹ
{
	FObjectTransformation();

	XMFLOAT4X4 World;

	static XMFLOAT4X4 IdentityMatrix4x4();
};
class FMesh : public IRenderingInterface
{
public:
	FMesh();

	virtual void Init();

	virtual void BuildMesh(const FMeshRenderingData* InRenderingData);

	virtual void PreDraw(float DeltaTime);

	virtual void Draw(float DeltaTime);

	virtual void PostDraw(float DeltaTime);

	static FMesh *CreateMesh(const FMeshRenderingData *InRenderingData);
	
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();//����Buffer��ͼ ��������Bufferλ�� ��С��
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();//����Buffer��ͼ

protected:
	//ͨ��Temp���潫CPU�˵Ļ��淢��ֵGPU��
	ComPtr<ID3DBlob> CPUVertexBufferPtr;//CPU���㻺��
	ComPtr<ID3DBlob> CPUIndexBufferPtr;//CPUindex����

	ComPtr<ID3D12Resource> GPUVertexBufferPtr;//GPU���㻺��
	ComPtr<ID3D12Resource> GPUIndexBufferPtr;//GPUIndex����

	ComPtr<ID3D12Resource> VertexBufferTmpPtr;//Temp���㻺��
	ComPtr<ID3D12Resource> IndexBufferTmpPtr;//TempIndex����

	ComPtr<ID3D12RootSignature>  RootSignature;//��ǩ��
	ComPtr<ID3D12DescriptorHeap> CBVHeap;//����������ͼ��ջ

	shared_ptr<FRenderingResourcesUpdate> objectConstants;//��������

	FShader VertexShader;//������ɫ��
	FShader PixelShader;//������ɫ��
	vector<D3D12_INPUT_ELEMENT_DESC> InputElementDesc;//��������

	ComPtr<ID3D12PipelineState> PSO;//PSO ��Ⱦ��ˮ��

protected:
	UINT VertexSizeInBytes;//�������ݻ������Ĵ�С
	UINT VertexStrideInBytes;//��������ÿ������Ĵ�С

	UINT IndexSizeInBytes;//����index��������С
	DXGI_FORMAT IndexFormat;
	UINT IndexSize;//��������

	XMFLOAT4X4 WorldMatrix;//����ռ����
	XMFLOAT4X4 ViewMatrix;//��ͼ�ռ����
	XMFLOAT4X4 ProjectMatrix;//ͶӰ����
};