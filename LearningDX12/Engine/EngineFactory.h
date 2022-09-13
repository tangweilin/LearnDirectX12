#pragma once
#include "Core/Engine.h"
class FEngineFactory
{
public:
	FEngineFactory();//构造
	static CEngine* CreateEngine();//创建引擎

};