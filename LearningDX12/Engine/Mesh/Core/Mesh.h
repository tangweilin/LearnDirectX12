#pragma once
#include "../../Rendering/Core/Rendering.h"
#include "MeshType.h"

class CMesh :public CCoreMinimalObject, public IRenderingInterface
{
public:
	CMesh();

	virtual void Init();

	virtual void BuildMesh(const FMeshRenderingData* InRenderingData);

	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);
	virtual void PostDraw(float DeltaTime);
};