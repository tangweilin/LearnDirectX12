#pragma once
#include "Viewport.h"
#include "CoreObject/CoreMinimalObject.h"
#include "../CodeReflection/CodeReflectionMacroTag.h"

class CTransformationComponent;
class CInputComponent;
class CCamera :public CCoreMinimalObject, public FViewport
{
	CVARIABLE()
	CTransformationComponent* TransformationComponent;
	CVARIABLE()
	CInputComponent* InputComponent;
public:
	CCamera();
};