#pragma once
#include "Core/Component.h"
#include "Input/InputType.h"

DEFINITION_SIMPLE_SINGLE_DELEGATE(FCaptureKeyboardInforDelegate, void, const FInputKey&);//定义按键单播代理

class CInputComponent :public CComponent
{
public:
	CVARIABLE()
	FCaptureKeyboardInforDelegate CaptureKeyboardInforDelegate;//创建单播代理


public:
	virtual void BeginInit();
	virtual void Tick(float DeltaTime);
};

