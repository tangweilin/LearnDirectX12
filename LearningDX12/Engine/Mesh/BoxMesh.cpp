#include "BoxMesh.h"
#include "Core/MeshType.h"
void CBoxMesh::Init()
{
	Super::Init();
}

void CBoxMesh::BuildMesh(const FMeshRenderingData* InRenderingData)
{
	Super::BuildMesh(InRenderingData);

}

void CBoxMesh::Draw(float DeltaTime)
{
	Super::Draw(DeltaTime);
}

CBoxMesh* CBoxMesh::CreateMesh()
{
	FMeshRenderingData MeshData;;

	//�������ǵĶ���
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT4(Colors::White)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT4(Colors::Red)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(1.f, 1.f, 0.f), XMFLOAT4(Colors::Green)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT4(Colors::Yellow)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(0.f, 0.f, 1.f), XMFLOAT4(Colors::Orange)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(0.f, 1.f, 1.f), XMFLOAT4(Colors::Blue)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(1.f, 1.f, 1.f), XMFLOAT4(Colors::Purple)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(1.f, 0.f, 1.f), XMFLOAT4(Colors::Brown)));

	//����Mesh Index
	//ǰ
	MeshData.IndexData.push_back(0); MeshData.IndexData.push_back(1); MeshData.IndexData.push_back(2);
	MeshData.IndexData.push_back(0); MeshData.IndexData.push_back(2); MeshData.IndexData.push_back(3);

	//��
	MeshData.IndexData.push_back(4); MeshData.IndexData.push_back(6); MeshData.IndexData.push_back(5);
	MeshData.IndexData.push_back(4); MeshData.IndexData.push_back(7); MeshData.IndexData.push_back(6);

	//��
	MeshData.IndexData.push_back(4); MeshData.IndexData.push_back(5); MeshData.IndexData.push_back(1);
	MeshData.IndexData.push_back(4); MeshData.IndexData.push_back(1); MeshData.IndexData.push_back(0);

	//��
	MeshData.IndexData.push_back(3); MeshData.IndexData.push_back(2); MeshData.IndexData.push_back(6);
	MeshData.IndexData.push_back(3); MeshData.IndexData.push_back(6); MeshData.IndexData.push_back(7);

	//��
	MeshData.IndexData.push_back(1); MeshData.IndexData.push_back(5); MeshData.IndexData.push_back(6);
	MeshData.IndexData.push_back(1); MeshData.IndexData.push_back(6); MeshData.IndexData.push_back(2);

	//��
	MeshData.IndexData.push_back(4); MeshData.IndexData.push_back(0); MeshData.IndexData.push_back(3);
	MeshData.IndexData.push_back(4); MeshData.IndexData.push_back(3); MeshData.IndexData.push_back(7);
	

	CBoxMesh* BoxMesh = new CBoxMesh;
	BoxMesh->BuildMesh(&MeshData);

	BoxMesh->Init();

	return BoxMesh;
}
