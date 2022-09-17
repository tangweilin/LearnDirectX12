#pragma once
#include "../../Core/CoreObject/CoreMinimalObject.h"
#include "../../Shader/Core/Shader.h"
#include "MeshType.h"
#include "Mesh.h"

class FRenderingResourcesUpdate;

class CMeshManager :public CCoreMinimalObject,public IRenderingInterface
{
public:
	CMeshManager();

	virtual void Init();//��ʼ��

	virtual void BuildMesh(const FMeshRenderingData* InRenderingData);//����Mesh

	virtual void PreDraw(float DeltaTime);//Ԥ��Ⱦ 
	virtual void Draw(float DeltaTime);//��Ⱦ
	virtual void PostDraw(float DeltaTime);

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();//����Buffer��ͼ ��������Bufferλ�� ��С��
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();//����Buffer��ͼ
public:
	//����������  Box
	CMesh* CreateBoxMesh( 
		float InHeight,
		float InWidth,
		float InDepth);
	//���������� Բ׶
	CMesh* CreateConeMesh(
		float InRadius,
		float InHeight,
		uint32_t InAxialSubdivision,
		uint32_t InHeightSubdivision);
	//���������� Բ��
	CMesh* CreateCylinderMesh(
		float InTopRadius,
		float InBottomRadius,
		float InHeight,
		uint32_t InAxialSubdivision,
		uint32_t InHeightSubdivision);
	//���������� ƽ��
	CMesh* CreatePlaneMesh(
		float InHeight, 
		float InWidth,
		uint32_t InHeightSubdivide, 
		uint32_t InWidthSubdivide);
	//���������� ��
	CMesh* CreateSphereMesh(
		float InRadius,
		uint32_t InAxialSubdivision,
		uint32_t InHeightSubdivision);
	//�Զ��弸���� �ⲿ����
	CMesh* CreateMesh(string& InPath);

protected:
	//����Mesh��ģ��
	template<class T, typename ...ParamTypes>
	T* CreateMesh(ParamTypes &&...Params);

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
	shared_ptr<FRenderingResourcesUpdate> ObjectConstants;//��������

	ComPtr<ID3D12PipelineState> PSO;//PSO ��Ⱦ��ˮ��

	FShader VertexShader;//������ɫ��
	FShader PixelShader;//������ɫ��

	vector<D3D12_INPUT_ELEMENT_DESC> InputElementDesc;//��������
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
