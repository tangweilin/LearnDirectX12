#pragma once
#include "../../Rendering/Core/Rendering.h"
#include "MeshType.h"
#include "../../Shader/Core/Shader.h"

struct FObjectTransformation //模型位置描述结构
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
	
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();//顶点Buffer视图 用于描述Buffer位置 大小等
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();//索引Buffer视图

protected:
	//通过Temp缓存将CPU端的缓存发送值GPU端
	ComPtr<ID3DBlob> CPUVertexBufferPtr;//CPU顶点缓存
	ComPtr<ID3DBlob> CPUIndexBufferPtr;//CPUindex缓存

	ComPtr<ID3D12Resource> GPUVertexBufferPtr;//GPU顶点缓存
	ComPtr<ID3D12Resource> GPUIndexBufferPtr;//GPUIndex缓存

	ComPtr<ID3D12Resource> VertexBufferTmpPtr;//Temp顶点缓存
	ComPtr<ID3D12Resource> IndexBufferTmpPtr;//TempIndex缓存

	ComPtr<ID3D12RootSignature>  RootSignature;//根签名
	ComPtr<ID3D12DescriptorHeap> CBVHeap;//常量缓冲视图堆栈

	shared_ptr<FRenderingResourcesUpdate> objectConstants;//常量缓冲

	FShader VertexShader;//顶点着色器
	FShader PixelShader;//像素着色器
	vector<D3D12_INPUT_ELEMENT_DESC> InputElementDesc;//输入描述

	ComPtr<ID3D12PipelineState> PSO;//PSO 渲染流水线

protected:
	UINT VertexSizeInBytes;//顶点数据缓冲区的大小
	UINT VertexStrideInBytes;//缓冲区内每个顶点的大小

	UINT IndexSizeInBytes;//顶点index缓冲区大小
	DXGI_FORMAT IndexFormat;
	UINT IndexSize;//顶点数量

	XMFLOAT4X4 WorldMatrix;//世界空间矩阵
	XMFLOAT4X4 ViewMatrix;//视图空间矩阵
	XMFLOAT4X4 ProjectMatrix;//投影矩阵
};