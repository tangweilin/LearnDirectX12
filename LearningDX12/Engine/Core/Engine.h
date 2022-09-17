#pragma once
#include "CoreObject/CoreMinimalObject.h"

#if defined(_WIN32)  //windows
#include "WinMainCommandParameters.h"
#endif //0

class CEngine:public CCoreMinimalObject
{
public:
	CEngine();

	virtual int PreInit(
#if defined(_WIN32)
		FWinMainCommandParameters InParameters
#endif
	) = 0;//引擎预初始化
	virtual int Init(
#if defined(_WIN32)
		FWinMainCommandParameters InParameters
#endif
	) = 0;//引擎初始化
	virtual int PostInit() = 0;//结束初始化

	virtual void Tick(float DeltaTime) {};//渲染

	virtual int PreExit() = 0;//预退出
	virtual int Exit() = 0;//退出
	virtual int PostExit() = 0;//退出结束
};