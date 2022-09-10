#pragma once
#include "Core/Engine.h"
class FEngineFactory
{
public:
	FEngineFactory();//构造
	static FEngine* CreateEngine();//创建引擎

};