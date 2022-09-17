#include "SphereMesh.h"
#include "Core/MeshType.h"

void CSphereMesh::Init()
{
	Super::Init();


}

void CSphereMesh::BuildMesh(const FMeshRenderingData* InRenderingData)
{
	Super::BuildMesh(InRenderingData);


}

void CSphereMesh::Draw(float DeltaTime)
{
	Super::Draw(DeltaTime);


}
//球面坐标系转笛卡尔坐标系公式: x = rsinθcosφ y= rsinθsinφ z = rcosθ  反之笛卡尔坐标系与球坐标系的转换关系为 r=sqrt(x^2+y^2+z^2)  θ = arccos(z/r)  φ = arctan(y/x)
//其中dx由于是Z向上所以公式中的y和z需要交换位置
CSphereMesh* CSphereMesh::CreateMesh(float InRadius, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision)// InRadius 半径 InAxialSubdivision轴向细分  InHeightSubdivision高度细分
{
	FMeshRenderingData MeshData;

	float ThetaValue = XM_2PI / InHeightSubdivision; //高度细分旋转一周(横向)
	float BetaValue = XM_PI / InAxialSubdivision;//轴向细分旋转一半(竖向)

	MeshData.VertexData.push_back(FVertex(
		XMFLOAT3(0.f, InRadius, 0.f), XMFLOAT4(Colors::White)));//添加第一个坐标起点

	//构建球面坐标
	for (uint32_t i = 1; i < InAxialSubdivision; ++i)
	{
		float Beta = i * BetaValue;

		for (uint32_t j = 0; j <= InHeightSubdivision; ++j)
		{
			float Theta = j * ThetaValue;

			//依据公式 球面坐标转为笛卡尔坐标
			MeshData.VertexData.push_back(FVertex(
				XMFLOAT3(
					InRadius * sinf(Beta) * cosf(Theta),//x
					InRadius * cosf(Beta),//y
					InRadius * sinf(Beta) * sinf(Theta)), //z
				XMFLOAT4(Colors::White)));

			int TopIndex = MeshData.VertexData.size() - 1;//获得刚添加进入的对象

			XMVECTOR Pos = XMLoadFloat3(&MeshData.VertexData[TopIndex].Position);
			XMStoreFloat3(&MeshData.VertexData[TopIndex].Normal, XMVector3Normalize(Pos));
		}
	}
	MeshData.VertexData.push_back(FVertex(
		XMFLOAT3(0.f, -InRadius, 0.f), XMFLOAT4(Colors::White)));//添加最后一个坐标终点
	//绘制北极
	for (uint32_t Index = 0; Index < InAxialSubdivision; ++Index)
	{
		MeshData.IndexData.push_back(0);
		MeshData.IndexData.push_back(Index + 1);
		MeshData.IndexData.push_back(Index);
	}
	float BaseIndex = 1;
	float VertexCircleNum = InAxialSubdivision + 1;
	//绘制腰围
	for (uint32_t i = 0; i < InHeightSubdivision - 2; ++i)//减掉上下
	{
		for (uint32_t j = 0; j < InAxialSubdivision; ++j)
		{
			//绘制的是四边形
			//三角形1
			MeshData.IndexData.push_back(BaseIndex + i * VertexCircleNum + j);
			MeshData.IndexData.push_back(BaseIndex + i * VertexCircleNum + j + 1);
			MeshData.IndexData.push_back(BaseIndex + (i + 1) * VertexCircleNum + j);
			//三角形2
			MeshData.IndexData.push_back(BaseIndex + (i + 1) * VertexCircleNum + j);
			MeshData.IndexData.push_back(BaseIndex + i * VertexCircleNum + j + 1);
			MeshData.IndexData.push_back(BaseIndex + (i + 1) * VertexCircleNum + j + 1);
		}
	}

	//绘制南极
	uint32_t SouthBaseIndex = MeshData.VertexData.size() - 1;
	BaseIndex = SouthBaseIndex - VertexCircleNum;
	for (uint32_t Index = 0; Index < InAxialSubdivision; ++Index)
	{
		MeshData.IndexData.push_back(SouthBaseIndex);
		MeshData.IndexData.push_back(BaseIndex + Index);
		MeshData.IndexData.push_back(BaseIndex + Index + 1);
	}
	CSphereMesh* SphereMesh = new CSphereMesh;
	SphereMesh->BuildMesh(&MeshData);

	SphereMesh->Init();
	
	return SphereMesh;
}

