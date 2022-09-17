#pragma once

#include "Core/Mesh.h"

class CSphereMesh :public CMesh
{
	typedef CMesh Super;
public:
	virtual void Init();

	virtual void BuildMesh(const FMeshRenderingData* InRenderingData);

	virtual void Draw(float DeltaTime);

	void CreateMesh(FMeshRenderingData& MeshData, float InRadius, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision);
};