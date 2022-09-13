#pragma once
#include "../../EngineMinimal.h"

enum EPressState
{
	Press ,//按下
	Release ,//松开
};

struct FInputKey
{
	FInputKey();

	string KeyName;//键名
	EPressState PressState;//按键状态
};