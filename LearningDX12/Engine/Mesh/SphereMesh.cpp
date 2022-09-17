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
//��������ϵת�ѿ�������ϵ��ʽ: x = rsin��cos�� y= rsin��sin�� z = rcos��  ��֮�ѿ�������ϵ��������ϵ��ת����ϵΪ r=sqrt(x^2+y^2+z^2)  �� = arccos(z/r)  �� = arctan(y/x)
//����dx������Z�������Թ�ʽ�е�y��z��Ҫ����λ��
CSphereMesh* CSphereMesh::CreateMesh(float InRadius, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision)// InRadius �뾶 InAxialSubdivision����ϸ��  InHeightSubdivision�߶�ϸ��
{
	FMeshRenderingData MeshData;

	float ThetaValue = XM_2PI / InHeightSubdivision; //�߶�ϸ����תһ��(����)
	float BetaValue = XM_PI / InAxialSubdivision;//����ϸ����תһ��(����)

	MeshData.VertexData.push_back(FVertex(
		XMFLOAT3(0.f, InRadius, 0.f), XMFLOAT4(Colors::White)));//��ӵ�һ���������

	//������������
	for (uint32_t i = 1; i < InAxialSubdivision; ++i)
	{
		float Beta = i * BetaValue;

		for (uint32_t j = 0; j <= InHeightSubdivision; ++j)
		{
			float Theta = j * ThetaValue;

			//���ݹ�ʽ ��������תΪ�ѿ�������
			MeshData.VertexData.push_back(FVertex(
				XMFLOAT3(
					InRadius * sinf(Beta) * cosf(Theta),//x
					InRadius * cosf(Beta),//y
					InRadius * sinf(Beta) * sinf(Theta)), //z
				XMFLOAT4(Colors::White)));

			int TopIndex = MeshData.VertexData.size() - 1;//��ø���ӽ���Ķ���

			XMVECTOR Pos = XMLoadFloat3(&MeshData.VertexData[TopIndex].Position);
			XMStoreFloat3(&MeshData.VertexData[TopIndex].Normal, XMVector3Normalize(Pos));
		}
	}
	MeshData.VertexData.push_back(FVertex(
		XMFLOAT3(0.f, -InRadius, 0.f), XMFLOAT4(Colors::White)));//������һ�������յ�
	//���Ʊ���
	for (uint32_t Index = 0; Index < InAxialSubdivision; ++Index)
	{
		MeshData.IndexData.push_back(0);
		MeshData.IndexData.push_back(Index + 1);
		MeshData.IndexData.push_back(Index);
	}
	float BaseIndex = 1;
	float VertexCircleNum = InAxialSubdivision + 1;
	//������Χ
	for (uint32_t i = 0; i < InHeightSubdivision - 2; ++i)//��������
	{
		for (uint32_t j = 0; j < InAxialSubdivision; ++j)
		{
			//���Ƶ����ı���
			//������1
			MeshData.IndexData.push_back(BaseIndex + i * VertexCircleNum + j);
			MeshData.IndexData.push_back(BaseIndex + i * VertexCircleNum + j + 1);
			MeshData.IndexData.push_back(BaseIndex + (i + 1) * VertexCircleNum + j);
			//������2
			MeshData.IndexData.push_back(BaseIndex + (i + 1) * VertexCircleNum + j);
			MeshData.IndexData.push_back(BaseIndex + i * VertexCircleNum + j + 1);
			MeshData.IndexData.push_back(BaseIndex + (i + 1) * VertexCircleNum + j + 1);
		}
	}

	//�����ϼ�
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

