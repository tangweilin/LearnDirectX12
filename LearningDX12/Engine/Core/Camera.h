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
	virtual void BeginInit();//��ʼ��
	virtual void Tick(float DeltaTime);

	virtual void ExecuteKeyboard(const FInputKey& InputKey);//ִ�м����¼�


public:
	FORCEINLINE CInputComponent* GetInputComponent() { return InputComponent; }//�ж��û��������
	FORCEINLINE CTransformationComponent* GetTransformationComponent() { return TransformationComponent; }//����ƶ����
};