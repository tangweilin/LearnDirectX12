#pragma once
#include "Core/Component.h"


class CInputComponent :public CComponent
{
	

public:
	virtual void BeginInit();
	virtual void Tick(float DeltaTime);
};

