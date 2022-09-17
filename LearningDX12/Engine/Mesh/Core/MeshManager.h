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

	virtual void Init();//初始化

	virtual void BuildMesh(const FMeshRenderingData* InRenderingData);//创建Mesh

	virtual void PreDraw(float DeltaTime);//预渲染 
	virtual void Draw(float DeltaTime);//渲染
	virtual void PostDraw(float DeltaTime);

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();//顶点Buffer视图 用于描述Buffer位置 大小等
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();//索引Buffer视图
public:
	//基本几何体  Box
	CMesh* CreateBoxMesh( 
		float InHeight,
		float InWidth,
		float InDepth);
	//基本几何体 圆锥
	CMesh* CreateConeMesh(
		float InRadius,
		float InHeight,
		uint32_t InAxialSubdivision,
		uint32_t InHeightSubdivision);
	//基本几何体 圆柱
	CMesh* CreateCylinderMesh(
		float InTopRadius,
		float InBottomRadius,
		float InHeight,
		uint32_t InAxialSubdivision,
		uint32_t InHeightSubdivision);
	//基本几何体 平面
	CMesh* CreatePlaneMesh(
		float InHeight, 
		float InWidth,
		uint32_t InHeightSubdivide, 
		uint32_t InWidthSubdivide);
	//基本几何体 球
	CMesh* CreateSphereMesh(
		float InRadius,
		uint32_t InAxialSubdivision,
		uint32_t InHeightSubdivision);
	//自定义几何体 外部导入
	CMesh* CreateMesh(string& InPath);

protected:
	//生成Mesh的模板
	template<class T, typename ...ParamTypes>
	T* CreateMesh(ParamTypes &&...Params);

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
	shared_ptr<FRenderingResourcesUpdate> ObjectConstants;//常量缓冲

	ComPtr<ID3D12PipelineState> PSO;//PSO 渲染流水线

	FShader VertexShader;//顶点着色器
	FShader PixelShader;//像素着色器

	vector<D3D12_INPUT_ELEMENT_DESC> InputElementDesc;//输入描述
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
