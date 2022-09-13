#include "InputComponent.h"
void CInputComponent::BeginInit()
{
}

void CInputComponent::Tick(float DeltaTime)
{
	if (CaptureKeyboardInforDelegate.IsBound())//如果绑定了代理
	{
		FInputKey InputKey;
		CaptureKeyboardInforDelegate.Execute(InputKey);//代理广播
	}
}