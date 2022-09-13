#pragma once
#include "Viewport.h"
#include "CoreObject/CoreMinimalObject.h"
#include "../CodeReflection/CodeReflectionMacroTag.h"

struct FInputKey;
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
	virtual void BeginInit();//初始化
	virtual void Tick(float DeltaTime);

	virtual void ExecuteKeyboard(const FInputKey& InputKey);//执行键盘事件


public:
	FORCEINLINE CInputComponent* GetInputComponent() { return InputComponent; }//判断用户输入组件
	FORCEINLINE CTransformationComponent* GetTransformationComponent() { return TransformationComponent; }//相机移动组件
};